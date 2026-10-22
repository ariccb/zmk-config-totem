# In-tree adaptive-key behavior. The driver calls HID helpers that the
# split peripheral image does not link, and the peripheral does not run
# the keymap. Compile it on the central half only, matching upstream.
if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_ADAPTIVE_KEY app PRIVATE src/behaviors/behavior_adaptive_key.c)
    zephyr_include_directories(include)
endif()
