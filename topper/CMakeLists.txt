# The star tree topper: Topper, TopperAnimation and friends, plus the
# animations themselves. Built on PicoLEDs' Strip and Renderer.

file(GLOB TOPPER_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_library(Topper ${TOPPER_SOURCES})

target_include_directories(Topper PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(Topper PUBLIC
    PicoLEDs
    pico_stdlib
)
