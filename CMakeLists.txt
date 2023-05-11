cmake_minimum_required(VERSION 3.25)
project(LegoRockRaiders C)

set(CMAKE_C_STANDARD 11)

set(MAIN_SRC
    # Sources
    main.c

    # Headers
    main.h
    types.h
)

add_executable(LegoRockRaiders WIN32 ${MAIN_SRC})
target_link_libraries(LegoRockRaiders ddraw)

# Include D3DRM
target_include_directories(LegoRockRaiders PRIVATE ${CMAKE_SOURCE_DIR}/lib/d3drm/inc)
target_link_libraries(LegoRockRaiders ${CMAKE_SOURCE_DIR}/lib/d3drm/d3drm.lib)