# Link listeners that must see keycodes in a fixed order:
# sticky key, caps word, and key repeat keep their earlier slots;
# autocorrect is immediately before the OS-mode swap; the swap is
# immediately before hid_listener.c. HID must observe Ctrl and GUI
# already swapped, and must not see a trigger autocorrect swallowed.
# This file is included from the end of the application CMakeLists,
# after those sources exist.

get_filename_component(_os_src "${CMAKE_CURRENT_LIST_DIR}/../src/behaviors/behavior_os_mode.c" ABSOLUTE)
get_filename_component(_ac_src "${CMAKE_CURRENT_LIST_DIR}/../src/behaviors/behavior_autocorrect.c" ABSOLUTE)

get_target_property(_sources app SOURCES)
if(NOT _sources)
    message(FATAL_ERROR "totem os-mode: app target has no sources")
endif()

set(_filtered "")
set(_have_ac FALSE)
foreach(_src ${_sources})
    if(_src MATCHES "behavior_autocorrect\\.c$")
        set(_have_ac TRUE)
    endif()
    if(NOT _src MATCHES "behavior_autocorrect\\.c$" AND NOT _src MATCHES "behavior_os_mode\\.c$")
        list(APPEND _filtered "${_src}")
    endif()
endforeach()

set(_reordered "")
set(_inserted FALSE)
foreach(_src ${_filtered})
    if(NOT _inserted AND _src MATCHES "hid_listener\\.c$")
        if(_have_ac)
            list(APPEND _reordered "${_ac_src}")
        endif()
        list(APPEND _reordered "${_os_src}")
        set(_inserted TRUE)
    endif()
    list(APPEND _reordered "${_src}")
endforeach()

if(NOT _inserted)
    message(FATAL_ERROR "totem os-mode: hid_listener.c is not linked into this image")
endif()

set_property(TARGET app PROPERTY SOURCES ${_reordered})
message(STATUS "totem os-mode: keycode swap listener is linked immediately before hid_listener.c")
