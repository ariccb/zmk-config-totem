/*
 * Copyright 2021-2022 Google LLC
 * SPDX-License-Identifier: Apache-2.0
 *
 * Getreuer trie autocorrect, ported from the Leeloo QMK keymap
 * (features/autocorrection.c) onto ZMK keycode events.
 *
 * https://getreuer.info/posts/keyboards/autocorrection
 *
 * The trie is generated from config/autocorrect/autocorrection_dict.txt.
 * Its backspace count assumes the key that completed the match has not
 * been sent yet. A letter trigger is therefore swallowed. A space or
 * other word-break trigger is sent after the correction.
 *
 * Hold-taps have already resolved: this listener sees the letter or the
 * modifier, not the hold-tap binding. Adaptive keys emit their
 * replacement as a later keycode event, and that is what is matched.
 * The listener is linked immediately before the OS-mode swap, which sits
 * immediately before HID. Sticky key, caps word, and key repeat still
 * see the original event. Returning HANDLED keeps HID from sending a
 * swallowed trigger. Correction taps are raised as their own events and
 * ignored here so they are not matched again.
 *
 * On by default. System's &os_ac toggles it. The choice is stored in
 * ZMK settings (totem/ac/en) when settings are enabled.
 */

#define DT_DRV_COMPAT zmk_behavior_autocorrect

#include <errno.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/hid.h>

#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/keys.h>
#include <dt-bindings/zmk/modifiers.h>

#include "autocorrection_data.h"

#if AUTOCORRECTION_MIN_LENGTH < 4
#error "Min typo length is less than 4. Autocorrection may behave poorly."
#endif

/* HID usages shared with make_autocorrection_data.py (KC_A = 4, KC_SPC = 0x2C). */
enum {
    AC_KC_A = 0x04,
    AC_KC_Z = 0x1d,
    AC_KC_1 = 0x1e,
    AC_KC_ENT = 0x28,
    AC_KC_ESC = 0x29,
    AC_KC_BSPC = 0x2a,
    AC_KC_SPC = 0x2c,
    AC_KC_SLSH = 0x38,
};

BUILD_ASSERT((A & 0xff) == AC_KC_A, "trie letter codes must match ZMK HID usages");
BUILD_ASSERT((Z & 0xff) == AC_KC_Z, "trie letter codes must match ZMK HID usages");
BUILD_ASSERT((BSPC & 0xff) == AC_KC_BSPC, "trie backspace must match ZMK HID usage");
BUILD_ASSERT((SPACE & 0xff) == AC_KC_SPC, "trie space must match ZMK HID usage");

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static uint8_t typo_buffer[AUTOCORRECTION_MAX_LENGTH];
static uint8_t typo_buffer_size;
static bool applying;
static bool ac_enabled = true;

#if IS_ENABLED(CONFIG_SETTINGS)

static int ac_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;

    if (settings_name_steq(name, "en", &next) && !next) {
        if (len != sizeof(uint8_t)) {
            return -EINVAL;
        }

        uint8_t value;
        int err = read_cb(cb_arg, &value, sizeof(value));
        if (err < (int)sizeof(value)) {
            return err < 0 ? err : -ENODATA;
        }

        ac_enabled = value != 0;
        LOG_INF("autocorrect loaded: %s", ac_enabled ? "on" : "off");
    }

    return 0;
}

static struct settings_handler ac_settings_handler = {
    .name = "totem/ac",
    .h_set = ac_settings_set,
};

static int ac_settings_init(void) { return settings_register(&ac_settings_handler); }

SYS_INIT(ac_settings_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void ac_save(void) {
    uint8_t value = ac_enabled ? 1 : 0;
    int err = settings_save_one("totem/ac/en", &value, sizeof(value));
    if (err) {
        LOG_WRN("Failed to save autocorrect toggle (%d)", err);
    }
}

#else

static void ac_save(void) {}

#endif

static const uint32_t ac_letters[26] = {
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

static uint32_t ascii_encoded(char c) {
    if (c >= 'a' && c <= 'z') {
        return ac_letters[c - 'a'];
    }
    if (c >= 'A' && c <= 'Z') {
        return LS(ac_letters[c - 'A']);
    }
    if (c == ' ') {
        return SPACE;
    }
    if (c == '\'') {
        return SQT;
    }
    return 0;
}

static void ac_tap(uint32_t encoded) {
    int64_t ts = k_uptime_get();

    raise_zmk_keycode_state_changed_from_encoded(encoded, true, ts);
    raise_zmk_keycode_state_changed_from_encoded(encoded, false, ts);
}

static void apply_correction(int backspaces, const uint8_t *text) {
    applying = true;
    for (int i = 0; i < backspaces; i++) {
        ac_tap(BSPC);
    }
    for (const uint8_t *p = text; *p != 0; p++) {
        uint32_t encoded = ascii_encoded((char)*p);
        if (encoded == 0) {
            LOG_WRN("autocorrect skipped byte 0x%02x", *p);
            continue;
        }
        ac_tap(encoded);
    }
    applying = false;
}

static bool non_shift_mod(const struct zmk_keycode_state_changed *ev) {
    uint8_t mods = ev->implicit_modifiers | ev->explicit_modifiers | zmk_hid_get_explicit_mods();

    return (mods & (uint8_t) ~(MOD_LSFT | MOD_RSFT)) != 0;
}

static int autocorrect_keycode_listener(const zmk_event_t *eh) {
    struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);

    if (ev == NULL || applying || !ev->state || !ac_enabled) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    if (ev->usage_page != HID_USAGE_KEY || non_shift_mod(ev)) {
        typo_buffer_size = 0;
        return ZMK_EV_EVENT_BUBBLE;
    }

    uint16_t keycode = (uint16_t)ev->keycode;

    if (!(AC_KC_A <= keycode && keycode <= AC_KC_Z)) {
        if (keycode == AC_KC_BSPC) {
            if (typo_buffer_size > 0) {
                typo_buffer_size--;
            }
            return ZMK_EV_EVENT_BUBBLE;
        }
        if (AC_KC_1 <= keycode && keycode <= AC_KC_SLSH && keycode != AC_KC_ESC) {
            /* Enter must not complete a word. Clear, then record a break. */
            if (keycode == AC_KC_ENT) {
                typo_buffer_size = 0;
            }
            keycode = AC_KC_SPC;
        } else {
            typo_buffer_size = 0;
            return ZMK_EV_EVENT_BUBBLE;
        }
    }

    if (typo_buffer_size >= AUTOCORRECTION_MAX_LENGTH) {
        memmove(typo_buffer, typo_buffer + 1, AUTOCORRECTION_MAX_LENGTH - 1);
        typo_buffer_size = AUTOCORRECTION_MAX_LENGTH - 1;
    }

    typo_buffer[typo_buffer_size++] = (uint8_t)keycode;
    if (typo_buffer_size < AUTOCORRECTION_MAX_LENGTH) {
        typo_buffer[typo_buffer_size] = 0;
        if (typo_buffer_size < AUTOCORRECTION_MIN_LENGTH) {
            return ZMK_EV_EVENT_BUBBLE;
        }
    }

    uint16_t state = 0;
    uint8_t code = autocorrection_data[state];

    for (int8_t i = typo_buffer_size - 1; i >= 0; i--) {
        const uint8_t key_i = typo_buffer[i];

        if (code & 64) {
            code &= 63;
            for (; code != key_i; code = autocorrection_data[state += 3]) {
                if (!code) {
                    return ZMK_EV_EVENT_BUBBLE;
                }
            }
            state = (uint16_t)((uint16_t)autocorrection_data[state + 1] |
                               (uint16_t)autocorrection_data[state + 2] << 8);
        } else if (code != key_i) {
            return ZMK_EV_EVENT_BUBBLE;
        } else if (!(code = autocorrection_data[++state])) {
            state++;
        }

        code = autocorrection_data[state];
        if (code & 128) {
            const int backspaces = code & 63;

            apply_correction(backspaces, &autocorrection_data[state + 1]);
            if (keycode == AC_KC_SPC) {
                typo_buffer[0] = AC_KC_SPC;
                typo_buffer_size = 1;
                return ZMK_EV_EVENT_BUBBLE;
            }
            typo_buffer_size = 0;
            return ZMK_EV_EVENT_HANDLED;
        }
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_autocorrect, autocorrect_keycode_listener);
ZMK_SUBSCRIPTION(behavior_autocorrect, zmk_keycode_state_changed);

static int on_ac_binding_pressed(struct zmk_behavior_binding *binding,
                                 struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    ac_enabled = !ac_enabled;
    typo_buffer_size = 0;
    ac_save();
    LOG_INF("autocorrect %s", ac_enabled ? "on" : "off");
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_ac_binding_released(struct zmk_behavior_binding *binding,
                                  struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_autocorrect_driver_api = {
    .binding_pressed = on_ac_binding_pressed,
    .binding_released = on_ac_binding_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

#define AC_INST(n)                                                                                 \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_autocorrect_driver_api);

DT_INST_FOREACH_STATUS_OKAY(AC_INST)
