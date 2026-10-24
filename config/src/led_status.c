/*
 * Left-half XIAO RGB status.
 *
 * The onboard LED is three active-low GPIOs (red P0.26, green P0.30,
 * blue P0.06), the same pins caksoylar/zmk-rgbled-widget uses. That
 * widget only blinks battery, connection, and a layer index. Caps Word,
 * latched Nav, and a one-shot OS-mode flash are not in it, and running
 * both would fight over the pins. This drives the gpio-leds device
 * directly and leaves the widget out. The right half does not link this
 * file. Idle is off. Caps Lock comes from CONFIG_ZMK_HID_INDICATORS,
 * which is set on the left half.
 *
 * Tick is 150 ms, Getreuer's fast-blink half period. Patterns, one bit
 * per tick over eight ticks:
 *   solid  0xff
 *   slow   0x0f   600 ms on, 600 ms off
 *   fast   0xaa   toggle every tick
 *
 * Order: an OS-mode flash, then Caps Lock solid, Caps Word fast,
 * latched Nav slow, then off.
 */

#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <zmk/hid_indicators.h>

#include <dt-bindings/zmk/hid_indicators.h>

#include "totem_status.h"

#define LED_TICK_MS 150
#define LED_OS_TICKS 6

#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_MAGENTA 5
#define COLOR_WHITE 7

#define PATTERN_SLOW 0x0f
#define PATTERN_FAST 0xaa
#define PATTERN_SOLID 0xff

BUILD_ASSERT(DT_NODE_EXISTS(DT_ALIAS(led_red)), "led-red alias missing");
BUILD_ASSERT(DT_NODE_EXISTS(DT_ALIAS(led_green)), "led-green alias missing");
BUILD_ASSERT(DT_NODE_EXISTS(DT_ALIAS(led_blue)), "led-blue alias missing");

static const struct device *led_dev = DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds));
static const uint8_t rgb_idx[] = {
    DT_NODE_CHILD_IDX(DT_ALIAS(led_red)),
    DT_NODE_CHILD_IDX(DT_ALIAS(led_green)),
    DT_NODE_CHILD_IDX(DT_ALIAS(led_blue)),
};

static struct k_work_delayable led_work;
static uint8_t phase;
static uint8_t os_ticks;

/* ZMK's caps-word object is { bool active; } and is not otherwise visible. */
struct totem_caps_word_data {
    bool active;
};

void totem_led_os_mode_changed(void) { os_ticks = LED_OS_TICKS; }

static bool caps_lock_on(void) {
    return (zmk_hid_indicators_get_current_profile() & HID_INDICATOR_CAPS_LOCK) != 0;
}

static bool caps_word_on(void) {
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(caps_word));

    if (!device_is_ready(dev) || dev->data == NULL) {
        return false;
    }

    return ((const struct totem_caps_word_data *)dev->data)->active;
}

static void apply_color(uint8_t color) {
    for (int i = 0; i < 3; i++) {
        if (color & BIT(i)) {
            led_on(led_dev, rgb_idx[i]);
        } else {
            led_off(led_dev, rgb_idx[i]);
        }
    }
}

static void led_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    uint8_t color = 0;
    uint8_t pattern = 0;

    if (!device_is_ready(led_dev)) {
        k_work_schedule(&led_work, K_MSEC(LED_TICK_MS));
        return;
    }

    if (os_ticks > 0) {
        color = COLOR_MAGENTA;
        pattern = PATTERN_FAST;
        os_ticks--;
    } else if (caps_lock_on()) {
        color = COLOR_WHITE;
        pattern = PATTERN_SOLID;
    } else if (caps_word_on()) {
        color = COLOR_YELLOW;
        pattern = PATTERN_FAST;
    } else if (totem_nav_is_latched()) {
        color = COLOR_BLUE;
        pattern = PATTERN_SLOW;
    }

    apply_color((pattern & BIT(phase)) ? color : 0);
    phase = (phase + 1) & 7;
    k_work_schedule(&led_work, K_MSEC(LED_TICK_MS));
}

static int led_status_init(void) {
    k_work_init_delayable(&led_work, led_work_handler);
    k_work_schedule(&led_work, K_MSEC(LED_TICK_MS));
    return 0;
}

SYS_INIT(led_status_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
