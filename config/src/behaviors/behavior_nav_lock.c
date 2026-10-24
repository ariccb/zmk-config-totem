/*
 * Latched Nav. &tog and a momentary hold share one layer bit, so the LED
 * cannot tell them apart from layer state alone. This behavior is the
 * only thing that marks Nav as latched. A later deactivate, including
 * &to and a second press of this key, clears the mark. Holding the
 * pinky (&a_nav) does not.
 */

#define DT_DRV_COMPAT zmk_behavior_nav_lock

#include <zephyr/device.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

#include "totem_status.h"

static int latched_layer = -1;

bool totem_nav_is_latched(void) { return latched_layer >= 0; }

static int nav_lock_listener(const zmk_event_t *eh) {
    struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);

    if (ev != NULL && !ev->state && ev->layer == latched_layer) {
        latched_layer = -1;
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_nav_lock, nav_lock_listener);
ZMK_SUBSCRIPTION(behavior_nav_lock, zmk_layer_state_changed);

static int on_nav_lock_pressed(struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event) {
    ARG_UNUSED(event);

    uint8_t layer = (uint8_t)binding->param1;

    zmk_keymap_layer_toggle(layer, false);
    latched_layer = zmk_keymap_layer_active(layer) ? layer : -1;
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_nav_lock_released(struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);
    return ZMK_BEHAVIOR_OPAQUE;
}

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static const struct behavior_parameter_value_metadata param_values[] = {
    {
        .display_name = "Layer",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_LAYER_ID,
    },
};

static const struct behavior_parameter_metadata_set param_metadata_set[] = {{
    .param1_values = param_values,
    .param1_values_len = ARRAY_SIZE(param_values),
}};

static const struct behavior_parameter_metadata nav_lock_metadata = {
    .sets_len = ARRAY_SIZE(param_metadata_set),
    .sets = param_metadata_set,
};

#endif

static const struct behavior_driver_api behavior_nav_lock_driver_api = {
    .binding_pressed = on_nav_lock_pressed,
    .binding_released = on_nav_lock_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &nav_lock_metadata,
#endif
};

#define NAV_LOCK_INST(n)                                                                           \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_nav_lock_driver_api);

DT_INST_FOREACH_STATUS_OKAY(NAV_LOCK_INST)
