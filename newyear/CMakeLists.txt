# The New Year's Eve countdown: the clock, the big digits, and the show
# itself. Built on PicoLEDs' Canvas and animations.

file(GLOB NEWYEAR_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_library(NewYearShow ${NEWYEAR_SOURCES})

target_include_directories(NewYearShow PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(NewYearShow PUBLIC
    PicoLEDs
    pico_stdlib
)
