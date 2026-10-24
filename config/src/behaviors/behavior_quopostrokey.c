/*
 * Getreuer Quopostrokey.
 *
 * https://getreuer.info/posts/keyboards/macros3/index.html#quopostrokey
 *
 * If the previous resolved key was a letter, tap apostrophe. Otherwise
 * type "" and Left so the cursor sits between the quotes.
 *
 * The listener sees keycodes after hold-taps and adaptive keys have
 * chosen what to send. A home-row tap is the letter (or its adaptive
 * replacement, which is also a letter). A home-row hold is the modifier,
 * which is not a letter. Shifted letters still count: the keycode is
 * A–Z and Shift is not treated as clearing the word. A modifier key on
 * its own, space, punctuation, and a layer turning on all clear it.
 */

#define DT_DRV_COMPAT zmk_behavior_quopostrokey

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/layer_state_changed.h>

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/keys.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static bool within_word;

static void tap_encoded(uint32_t encoded) {
    int64_t ts = k_uptime_get();

    raise_zmk_keycode_state_changed_from_encoded(encoded, true, ts);
    raise_zmk_keycode_state_changed_from_encoded(encoded, false, ts);
}

static int quop_listener(const zmk_event_t *eh) {
    struct zmk_keycode_state_changed *kc = as_zmk_keycode_state_changed(eh);

    if (kc != NULL) {
        if (!kc->state) {
            return ZMK_EV_EVENT_BUBBLE;
        }

        if (kc->usage_page == HID_USAGE_KEY && kc->keycode >= HID_USAGE_KEY_KEYBOARD_A &&
            kc->keycode <= HID_USAGE_KEY_KEYBOARD_Z) {
            within_word = true;
        } else {
            within_word = false;
        }
        return ZMK_EV_EVENT_BUBBLE;
    }

    /* A layer key that does not type a letter. Release leaves the flag. */
    struct zmk_layer_state_changed *layer = as_zmk_layer_state_changed(eh);
    if (layer != NULL && layer->state) {
        within_word = false;
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_quopostrokey, quop_listener);
ZMK_SUBSCRIPTION(behavior_quopostrokey, zmk_keycode_state_changed);
ZMK_SUBSCRIPTION(behavior_quopostrokey, zmk_layer_state_changed);

static int on_quop_pressed(struct zmk_behavior_binding *binding,
                           struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    if (within_word) {
        tap_encoded(SQT);
    } else {
        tap_encoded(DQT);
        tap_encoded(DQT);
        tap_encoded(LEFT);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_quop_released(struct zmk_behavior_binding *binding,
                            struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_quopostrokey_driver_api = {
    .binding_pressed = on_quop_pressed,
    .binding_released = on_quop_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define QUOP_INST(n)                                                                               \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                                   \
                            &behavior_quopostrokey_driver_api);

DT_INST_FOREACH_STATUS_OKAY(QUOP_INST)
