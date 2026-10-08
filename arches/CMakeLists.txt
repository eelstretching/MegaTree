# Leaping arches: Arch, ArchSet, Archimation and friends, plus the
# Archimations themselves. Built on PicoLEDs' Strip and Renderer.

file(GLOB ARCHES_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_library(Arches ${ARCHES_SOURCES})

target_include_directories(Arches PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(Arches PUBLIC
    PicoLEDs
    pico_stdlib
)
