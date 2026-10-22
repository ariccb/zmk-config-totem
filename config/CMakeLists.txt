# In-tree adaptive-key behavior. The driver calls HID helpers that the
# split peripheral image does not link, and the peripheral does not run
# the keymap. Compile it on the central half only, matching upstream.
if((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_ADAPTIVE_KEY app PRIVATE src/behaviors/behavior_adaptive_key.c)
    zephyr_include_directories(include)

    # The listener has to sit after sticky-key and before HID. The reorder
    # runs at the end of the application CMakeLists, once hid_listener.c
    # is already a source. See cmake/link_os_mode.cmake.
    if(CONFIG_ZMK_BEHAVIOR_OS_MODE)
        target_sources(app PRIVATE src/behaviors/behavior_os_mode.c)
        cmake_language(EVAL CODE "
            cmake_language(DEFER DIRECTORY [[${CMAKE_SOURCE_DIR}]] CALL include [[${CMAKE_CURRENT_LIST_DIR}/cmake/link_os_mode.cmake]])
        ")
    endif()
endif()
