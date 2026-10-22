# Link the OS-mode listener immediately before hid_listener.c.
# Sticky key, caps word, and key repeat must observe the original keycode.
# HID must observe Ctrl and GUI already swapped. This file is included from
# the end of the application CMakeLists, after those sources exist.

get_filename_component(_os_src "${CMAKE_CURRENT_LIST_DIR}/../src/behaviors/behavior_os_mode.c" ABSOLUTE)

get_target_property(_sources app SOURCES)
if(NOT _sources)
    message(FATAL_ERROR "totem os-mode: app target has no sources")
endif()

set(_filtered "")
foreach(_src ${_sources})
    if(NOT _src MATCHES "behavior_os_mode\\.c$")
        list(APPEND _filtered "${_src}")
    endif()
endforeach()

set(_reordered "")
set(_inserted FALSE)
foreach(_src ${_filtered})
    if(NOT _inserted AND _src MATCHES "hid_listener\\.c$")
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
