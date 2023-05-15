project(LegoRR C)

set(CMAKE_C_STANDARD 11)

set(MAIN_SRC
    # Sources
    main.c
    registry.c
    error.c
    file.c
    mem.c
    config.c
    input.c
    directdraw.c
    sound.c
    init.c
    animation.c
    draw.c
    lego.c
    maths.c

    # Headers
    resource/resource.h
    main.h
    registry.h
    error.h
    file.h
    mem.h
    config.h
    input.h
    directdraw.h
    sound.h
    init.h
    animation.h
    draw.h
    lego.h
    maths.h
    ../Shared/types.h

    # Resource
    resource/LegoRR.rc
)

add_executable(LegoRR WIN32 ${MAIN_SRC})
target_include_directories(LegoRR PRIVATE ${CMAKE_SOURCE_DIR}/src/Shared)

target_link_libraries(LegoRR ddraw dinput8 winmm dsound dxguid)

# Include D3DRM
target_include_directories(LegoRR PRIVATE ${CMAKE_SOURCE_DIR}/lib/d3drm/inc)
target_link_libraries(LegoRR ${CMAKE_SOURCE_DIR}/lib/d3drm/d3drm.lib)

# Output to output directory with configuration in name
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${ProjectOutDir}")
set_property(TARGET ${PROJECT_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${ProjectOutDir}")
if (CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELEASE ${PROJECT_NAME}64)
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_DEBUG ${PROJECT_NAME}64_DEBUG)
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_MINSIZEREL ${PROJECT_NAME}64_MINSIZEREL)
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELWITHDEBINFO ${PROJECT_NAME}64_RELWITHDEBINFO)
else()
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELEASE ${PROJECT_NAME})
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_DEBUG ${PROJECT_NAME}_DEBUG)
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_MINSIZEREL ${PROJECT_NAME}_MINSIZEREL)
    set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME_RELWITHDEBINFO ${PROJECT_NAME}_RELWITHDEBINFO)
endif()
