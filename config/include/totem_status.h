/*
 * Shared hooks for the left-half status LED and latched Nav.
 * Both sides are compiled only into the central image.
 */

#pragma once

#include <stdbool.h>

/* Short magenta flash. The persistent indicator returns when it ends. */
void totem_led_os_mode_changed(void);

/* True while Nav was turned on with &nav_lock and has not been turned off. */
bool totem_nav_is_latched(void);
