# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Builds for the universal-strip-controller board (a custom RP2350B design).
# Its board header lives with that project's firmware rather than being
# copied here, so the two can't drift apart. The prototype breakout's header
# (waveshare_core2350b_prototype) is in the same directory.
set(USC_FIRMWARE_PATH ${CMAKE_CURRENT_LIST_DIR}/../universal-strip-controller/firmware
    CACHE PATH "Path to the universal-strip-controller firmware directory")
set(PICO_BOARD_HEADER_DIRS ${USC_FIRMWARE_PATH}/boards)
# Lets board headers #include "boards/other.h" (see the note in the
# universal-strip-controller firmware's CMakeLists.txt).
include_directories(${USC_FIRMWARE_PATH})
set(PICO_BOARD universal_strip_controller CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(MegaTree C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# The PicoLEDs animation library, from its checkout next to this one.
set(PICOLEDS_PATH ${CMAKE_CURRENT_LIST_DIR}/../PicoLEDs CACHE PATH
    "Path to a PicoLEDs checkout")
add_subdirectory(${PICOLEDS_PATH}/src ${CMAKE_BINARY_DIR}/PicoLEDs)
# PicoLEDs' headers don't build cleanly under -Wall -Wextra, which the
# StatusLights driver uses, so treat them as system headers.
get_target_property(PICOLEDS_INCLUDES PicoLEDs INTERFACE_INCLUDE_DIRECTORIES)
set_target_properties(PicoLEDs PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${PICOLEDS_INCLUDES}")

# The board's status lights driver, from the universal-strip-controller firmware.
add_subdirectory(${USC_FIRMWARE_PATH}/drivers/status_lights ${CMAKE_BINARY_DIR}/status_lights)

# The DS3231 driver uses the SDK's datetime_t, which is only built in on the
# RP2040 unless we ask for it (the universal-strip-controller firmware does
# the same).
add_compile_definitions(PICO_INCLUDE_RTC_DATETIME=1)

# The board's DS3231 RTC driver, from the universal-strip-controller firmware.
add_subdirectory(${USC_FIRMWARE_PATH}/drivers/ds3231 ${CMAKE_BINARY_DIR}/ds3231)

# Leaping arches: the Arch and Archimation classes.
add_subdirectory(arches)

# The New Year's Eve countdown show.
add_subdirectory(newyear)

# The star tree topper: the Topper class and its animations.
add_subdirectory(topper)

# Add executable. Default name is the project name, version 0.1

add_executable(MegaTree MegaTree.cpp)

pico_set_program_name(MegaTree "MegaTree")
pico_set_program_version(MegaTree "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(MegaTree 1)
pico_enable_stdio_usb(MegaTree 1)

# Add the standard library to the build
target_link_libraries(MegaTree
    PicoLEDs
    usc_status_lights
    pico_stdlib
)

# Add the standard include files to the build
target_include_directories(MegaTree PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(MegaTree)

# A demo of the leaping arch Archimations on a single output.
add_executable(LeapingArches LeapingArches.cpp)
pico_set_program_name(LeapingArches "LeapingArches")
pico_set_program_version(LeapingArches "0.1")
pico_enable_stdio_uart(LeapingArches 1)
pico_enable_stdio_usb(LeapingArches 1)
target_link_libraries(LeapingArches
    Arches
    pico_stdlib
)
pico_add_extra_outputs(LeapingArches)

# The New Year's Eve countdown, as its own firmware.
add_executable(NewYear NewYear.cpp)
pico_set_program_name(NewYear "NewYear")
pico_set_program_version(NewYear "0.1")
pico_enable_stdio_uart(NewYear 1)
pico_enable_stdio_usb(NewYear 1)
target_link_libraries(NewYear
    NewYearShow
    PicoLEDs
    usc_status_lights
    usc_ds3231
    hardware_i2c
    pico_stdlib
)
pico_add_extra_outputs(NewYear)

# A demo of the star topper animations on a single output.
add_executable(StarTopper StarTopper.cpp)
pico_set_program_name(StarTopper "StarTopper")
pico_set_program_version(StarTopper "0.1")
pico_enable_stdio_uart(StarTopper 1)
pico_enable_stdio_usb(StarTopper 1)
target_link_libraries(StarTopper
    Topper
    pico_stdlib
)
pico_add_extra_outputs(StarTopper)
