# The coordinated Christmas show: the schedule every prop shares, the time
# keeping on core 1 (DS3231 + NTP over WiFi), and the animators that play the
# schedule on the tree, topper and arches. See README.md.

# WiFi details for NTP, kept out of git. Any one of:
#   - show/wifi_secrets.cmake (gitignored), containing
#       set(SHOW_WIFI_SSID "myssid")
#       set(SHOW_WIFI_PASSWORD "mypassword")
#   - SHOW_WIFI_SSID / SHOW_WIFI_PASSWORD environment variables
#   - cmake -DSHOW_WIFI_SSID=... -DSHOW_WIFI_PASSWORD=...
# With no SSID the props go by their DS3231s alone.
include(${CMAKE_CURRENT_LIST_DIR}/wifi_secrets.cmake OPTIONAL)
if(NOT DEFINED SHOW_WIFI_SSID)
    set(SHOW_WIFI_SSID "$ENV{SHOW_WIFI_SSID}")
endif()
if(NOT DEFINED SHOW_WIFI_PASSWORD)
    set(SHOW_WIFI_PASSWORD "$ENV{SHOW_WIFI_PASSWORD}")
endif()
set(SHOW_WIFI_SSID "${SHOW_WIFI_SSID}" CACHE STRING "WiFi SSID the show props use for NTP")
set(SHOW_WIFI_PASSWORD "${SHOW_WIFI_PASSWORD}" CACHE STRING "WiFi password for SHOW_WIFI_SSID")

# The schedule itself: plain C++, also built on a computer by show/test.
add_library(ShowSchedule src/ShowSchedule.cpp)
target_include_directories(ShowSchedule PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

# Time keeping and the director.
add_library(Show src/ShowTime.cpp src/ShowDirector.cpp)
target_compile_options(Show PRIVATE -Wall -Wextra)
target_include_directories(Show PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(Show PUBLIC
    ShowSchedule
    usc_ds3231
    hardware_i2c
    pico_multicore
    pico_stdlib
)
if(SHOW_WIFI_SSID STREQUAL "")
    message(STATUS "Show: no SHOW_WIFI_SSID, so no NTP; the props go by their DS3231s")
    target_compile_definitions(Show PRIVATE SHOW_WIFI=0)
else()
    message(STATUS "Show: NTP over WiFi ${SHOW_WIFI_SSID}")
    target_compile_definitions(Show PRIVATE
        SHOW_WIFI=1
        SHOW_WIFI_SSID="${SHOW_WIFI_SSID}"
        SHOW_WIFI_PASSWORD="${SHOW_WIFI_PASSWORD}"
    )
    target_link_libraries(Show PUBLIC pico_cyw43_arch_lwip_poll)
endif()

add_library(ShowTreeAnimator src/ShowTreeAnimator.cpp)
target_link_libraries(ShowTreeAnimator PUBLIC Show PicoLEDs Topper)
target_compile_options(ShowTreeAnimator PRIVATE -Wall -Wextra)

add_library(ShowArchAnimator src/ShowArchAnimator.cpp)
target_link_libraries(ShowArchAnimator PUBLIC Show Arches)
target_compile_options(ShowArchAnimator PRIVATE -Wall -Wextra)
