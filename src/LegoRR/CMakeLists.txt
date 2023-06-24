project(LegoRR C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

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
    dxbug.c
    wad.c
    container.c
    fonts.c
    lego_camera.c
    lego_object.c
    text_window.c
    viewport.c
    front_end.c
    activities.c
    anim_clone.c
    utils.c
    compress.c
    images.c
    bmp.c
    sfx.c
    common.c
    loader.c
    mesh.c
    nerps.c
    advisor.c
    keys.c
    info.c
    interface.c
    bubble.c
    pointer.c
    smoke.c
    message.c
    aitask.c
    radarmap.c
    panels.c
    tooltip.c
    text.c
    priorities.c
    lws.c
    damage_font.c
    detail.c
    rewards.c
    effects.c
    3DSound.c
    select_place.c
    vehicle.c
    creature.c
    building.c
    upgrade.c
    help_window.c
    obj_info.c
    weapons.c
    light_effects.c
    stats.c
    dependencies.c
    encyclopedia.c
    save.c
    flic.c
    movie.cpp
    G98CMovie.cpp
    platform/d3drm.c

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
    dxbug.h
    wad.h
    container.h
    fonts.h
    lego_camera.h
    lego_object.h
    text_window.h
    viewport.h
    front_end.h
    activities.h
    anim_clone.h
    utils.h
    compress.h
    images.h
    bmp.h
    sfx.h
    common.h
    loader.h
    mesh.h
    nerps.h
    advisor.h
    keys.h
    info.h
    interface.h
    bubble.h
    pointer.h
    smoke.h
    message.h
    aitask.h
    radarmap.h
    panels.h
    tooltip.h
    text.h
    priorities.h
    lws.h
    damage_font.h
    detail.h
    rewards.h
    effects.h
    3DSound.h
    select_place.h
    vehicle.h
    creature.h
    building.h
    upgrade.h
    help_window.h
    obj_info.h
    weapons.h
    light_effects.h
    stats.h
    dependencies.h
    encyclopedia.h
    save.h
    flic.h
    movie.h
    G98CMovie.hpp
    ../Shared/types.h
    ../Shared/game_config.h
    ../Shared/platform.h

    # Resource
    resource/LegoRR.rc
)

add_executable(LegoRR WIN32 ${MAIN_SRC})
target_include_directories(LegoRR PRIVATE ${CMAKE_SOURCE_DIR}/src/Shared)

target_link_libraries(LegoRR ddraw dinput8 winmm dsound dxguid vfw32 strmbase strmiids)

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
