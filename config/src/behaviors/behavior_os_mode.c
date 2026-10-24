/*
 * Mac/Windows shortcut mode for the Totem keymap.
 *
 * The keymap is written as Mac: GUI is Cmd on the inner home-row column,
 * Ctrl is the index finger, and shortcut combos are LG(...). Windows mode
 * does not change those nodes. A listener rewrites Ctrl and GUI on the keycode event
 * after the other behaviors have seen the original code and before HID sends
 * the report. Hold-tap timing is therefore the same in both modes.
 *
 * config/cmake/link_os_mode.cmake inserts this file immediately before
 * hid_listener.c. Sticky keys match their own keycode and would treat a
 * swapped event as a different key. Caps word and key repeat also run before
 * that slot and must see the unswapped event.
 *
 * Redo, clipboard history, browser back/forward, current-app windows, and
 * the app switcher are not a pure swap (Ctrl+Y versus Cmd+Shift+Z, Win+V
 * versus Alt+Shift+Cmd+Space, Alt+arrows versus Cmd+[ / Cmd+] / Cmd+Right,
 * Alt+` versus Control+Down, Alt+Tab versus Cmd+Tab). Those behaviors send
 * the finished chord with os_passthrough set so this listener leaves it
 * alone. The app switcher holds its modifier until release and taps Tab
 * once, the same shape as the cmd_tab macro.
 *
 * The choice is stored in ZMK settings (totem/os/win) when settings are
 * enabled, which they are on this BLE keyboard. Until a Mac or Windows key
 * has been pressed, the boot value is CONFIG_TOTEM_OS_MODE_DEFAULT_WINDOWS.
 * config/totem.conf sets that, so a fresh board boots in Windows mode.
 * A saved value overrides it. Press Win to store Windows again, or flash
 * settings_reset to drop the saved value.
 * Release Ctrl and Cmd before changing mode. A modifier that is already down
 * is released with the other mode's code and can stick on the host.
 */

#define DT_DRV_COMPAT zmk_behavior_os_mode

#include <errno.h>

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/keys.h>

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk/modifiers.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

enum os_command {
    OS_MAC = 0,
    OS_WIN,
    OS_REDO,
    OS_CLIP,
    OS_FWD,
    OS_BACK,
    OS_FWD_BR,
    OS_APPS,
    OS_SWITCH,
};

struct behavior_os_mode_config {
    uint8_t command;
};

struct behavior_os_mode_data {
    bool held;
    uint32_t keycode;
};

static bool windows_mode = IS_ENABLED(CONFIG_TOTEM_OS_MODE_DEFAULT_WINDOWS);
static bool os_passthrough;

/*
 * Swap Ctrl and GUI without dropping either one.
 *
 * Both families are cleared first, then set from the original bits. Doing
 * it in place (if Ctrl, write GUI; if GUI, write Ctrl) would see the GUI
 * just written and turn a chord that held both into Ctrl alone.
 *
 * Hyper is not one event. The macro presses LCTRL, LSHIFT, LALT, and LGUI
 * as four key events, and a plain modifier key carries its identity in the
 * keycode with the modifier byte clear. zmk_hid_keyboard_press registers
 * that keycode as the modifier bit. Swapping the keycodes trades LCTRL
 * with LGUI, so the report still has Ctrl, Shift, Alt, and GUI. The same
 * holds for one event whose implicit modifiers already contain both, such
 * as mic mute LC(LA(LG(M))).
 */
static uint8_t swap_ctrl_gui_bits(uint8_t mods) {
    uint8_t swapped = mods & (uint8_t) ~(MOD_LCTL | MOD_RCTL | MOD_LGUI | MOD_RGUI);

    if (mods & MOD_LCTL) {
        swapped |= MOD_LGUI;
    }
    if (mods & MOD_RCTL) {
        swapped |= MOD_RGUI;
    }
    if (mods & MOD_LGUI) {
        swapped |= MOD_LCTL;
    }
    if (mods & MOD_RGUI) {
        swapped |= MOD_RCTL;
    }

    return swapped;
}

static uint32_t swap_ctrl_gui_usage(uint32_t keycode) {
    switch (keycode) {
    case HID_USAGE_KEY_KEYBOARD_LEFTCONTROL:
        return HID_USAGE_KEY_KEYBOARD_LEFT_GUI;
    case HID_USAGE_KEY_KEYBOARD_LEFT_GUI:
        return HID_USAGE_KEY_KEYBOARD_LEFTCONTROL;
    case HID_USAGE_KEY_KEYBOARD_RIGHTCONTROL:
        return HID_USAGE_KEY_KEYBOARD_RIGHT_GUI;
    case HID_USAGE_KEY_KEYBOARD_RIGHT_GUI:
        return HID_USAGE_KEY_KEYBOARD_RIGHTCONTROL;
    default:
        return keycode;
    }
}

static int os_mode_keycode_listener(const zmk_event_t *eh) {
    struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);

    if (ev == NULL || !windows_mode || os_passthrough) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->usage_page == HID_USAGE_KEY) {
        ev->keycode = swap_ctrl_gui_usage(ev->keycode);
    }
    ev->implicit_modifiers = swap_ctrl_gui_bits(ev->implicit_modifiers);
    ev->explicit_modifiers = swap_ctrl_gui_bits(ev->explicit_modifiers);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_os_mode, os_mode_keycode_listener);
ZMK_SUBSCRIPTION(behavior_os_mode, zmk_keycode_state_changed);

#if IS_ENABLED(CONFIG_SETTINGS)

static int os_mode_settings_set(const char *name, size_t len, settings_read_cb read_cb,
                                void *cb_arg) {
    const char *next;

    if (settings_name_steq(name, "win", &next) && !next) {
        if (len != sizeof(uint8_t)) {
            return -EINVAL;
        }

        uint8_t value;
        int err = read_cb(cb_arg, &value, sizeof(value));
        if (err < (int)sizeof(value)) {
            return err < 0 ? err : -ENODATA;
        }

        windows_mode = value != 0;
        LOG_INF("OS mode loaded: %s", windows_mode ? "windows" : "mac");
    }

    return 0;
}

static struct settings_handler os_mode_settings_handler = {
    .name = "totem/os",
    .h_set = os_mode_settings_set,
};

static int os_mode_settings_init(void) { return settings_register(&os_mode_settings_handler); }

SYS_INIT(os_mode_settings_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void os_mode_save(void) {
    uint8_t value = windows_mode ? 1 : 0;
    int err = settings_save_one("totem/os/win", &value, sizeof(value));
    if (err) {
        LOG_WRN("Failed to save OS mode (%d)", err);
    }
}

#else

static void os_mode_save(void) {}

#endif

static int send_encoded(uint32_t encoded, struct zmk_behavior_binding_event event, bool pressed) {
    os_passthrough = true;
    int ret = raise_zmk_keycode_state_changed_from_encoded(encoded, pressed, event.timestamp);
    os_passthrough = false;
    return ret;
}

static uint32_t chord_for(uint8_t command) {
    switch (command) {
    case OS_REDO:
        return windows_mode ? LC(Y) : LS(LG(Z));
    case OS_CLIP:
        return windows_mode ? LG(V) : LA(LS(LG(SPACE)));
    case OS_FWD:
        return windows_mode ? LA(RIGHT) : LG(RIGHT);
    case OS_BACK:
        return windows_mode ? LA(LEFT) : LG(LBKT);
    case OS_FWD_BR:
        return windows_mode ? LA(RIGHT) : LG(RBKT);
    case OS_APPS:
        /* Current-app windows: Mac App Exposé, Windows Alt+` (grave). */
        return windows_mode ? LA(GRAVE) : LC(DOWN);
    default:
        return 0;
    }
}

/*
 * Hold Cmd (Mac) or Alt (Windows) and tap Tab once. The modifier stays
 * down until the key is released, so the switcher stays open, matching
 * cmd_tab. The modifier is stored so release uses the same key if the
 * mode changes while the thumb is held.
 */
static int press_app_switch(struct behavior_os_mode_data *data,
                            struct zmk_behavior_binding_event event) {
    uint32_t mod = windows_mode ? LALT : LGUI;

    data->held = true;
    data->keycode = mod;
    send_encoded(mod, event, true);
    send_encoded(TAB, event, true);
    k_msleep(10);
    send_encoded(TAB, event, false);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_os_mode_data *data = dev->data;
    const struct behavior_os_mode_config *config = dev->config;

    switch (config->command) {
    case OS_MAC:
        windows_mode = false;
        os_mode_save();
        LOG_INF("OS mode set to mac");
        return ZMK_BEHAVIOR_OPAQUE;
    case OS_WIN:
        windows_mode = true;
        os_mode_save();
        LOG_INF("OS mode set to windows");
        return ZMK_BEHAVIOR_OPAQUE;
    case OS_SWITCH:
        return press_app_switch(data, event);
    default:
        break;
    }

    uint32_t keycode = chord_for(config->command);
    if (keycode == 0) {
        return ZMK_BEHAVIOR_OPAQUE;
    }

    data->held = true;
    data->keycode = keycode;
    send_encoded(keycode, event, true);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_os_mode_data *data = dev->data;

    if (!data->held) {
        return ZMK_BEHAVIOR_OPAQUE;
    }

    send_encoded(data->keycode, event, false);
    data->held = false;
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_os_mode_driver_api = {
    .binding_pressed = on_keymap_binding_pressed,
    .binding_released = on_keymap_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define OS_INST(n)                                                                                 \
    static struct behavior_os_mode_data behavior_os_mode_data_##n = {};                            \
    static const struct behavior_os_mode_config behavior_os_mode_config_##n = {                    \
        .command = DT_INST_ENUM_IDX(n, command),                                                   \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, &behavior_os_mode_data_##n,                             \
                            &behavior_os_mode_config_##n, POST_KERNEL,                             \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_os_mode_driver_api);

DT_INST_FOREACH_STATUS_OKAY(OS_INST)
