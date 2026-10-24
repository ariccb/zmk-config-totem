/*
 * Exponential Backspace, from Getreuer's key-repeat macro.
 *
 * https://getreuer.info/posts/keyboards/macros3/index.html#exponential-key-repeating
 *
 * Press deletes once, then again after 250 ms, then on the REP_DELAY_MS
 * curve down to 10 ms. Release cancels the timer. The key is tapped, not
 * held, so the host's own key repeat does not run as well.
 */

#define DT_DRV_COMPAT zmk_behavior_exp_bspc

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/events/keycode_state_changed.h>

#include <dt-bindings/zmk/keys.h>

#define EXP_BSPC_INIT_DELAY_MS 250

static const uint8_t rep_delay_ms[] = {
    99, 79, 65, 57, 49, 43, 40, 35, 33, 30, 28, 26, 25, 23, 22, 20,
    20, 19, 18, 17, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10,
};

static struct k_work_delayable bspc_work;
static uint8_t rep_count;
static bool held;

static void tap_bspc(void) {
    int64_t ts = k_uptime_get();

    raise_zmk_keycode_state_changed_from_encoded(BSPC, true, ts);
    raise_zmk_keycode_state_changed_from_encoded(BSPC, false, ts);
}

static void bspc_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    if (!held) {
        return;
    }

    tap_bspc();

    if (!held) {
        return;
    }

    if (rep_count < ARRAY_SIZE(rep_delay_ms)) {
        rep_count++;
    }

    k_work_schedule(&bspc_work, K_MSEC(rep_delay_ms[rep_count - 1]));
}

static int on_exp_bspc_pressed(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    held = true;
    rep_count = 0;
    tap_bspc();
    k_work_schedule(&bspc_work, K_MSEC(EXP_BSPC_INIT_DELAY_MS));
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_exp_bspc_released(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    held = false;
    k_work_cancel_delayable(&bspc_work);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_exp_bspc_driver_api = {
    .binding_pressed = on_exp_bspc_pressed,
    .binding_released = on_exp_bspc_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif
};

static int exp_bspc_init(const struct device *dev) {
    ARG_UNUSED(dev);
    k_work_init_delayable(&bspc_work, bspc_work_handler);
    return 0;
}

#define EXP_BSPC_INST(n)                                                                           \
    BEHAVIOR_DT_INST_DEFINE(n, exp_bspc_init, NULL, NULL, NULL, POST_KERNEL,                       \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_exp_bspc_driver_api);

DT_INST_FOREACH_STATUS_OKAY(EXP_BSPC_INST)
