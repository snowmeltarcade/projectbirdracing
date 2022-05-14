target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        constructor.h
        constructor_types.h
        iscene_factory.h
        iscene_manager.h
        scene_base.h
        scene_manager.h
        scene_types.h
    PRIVATE
        constructor.cpp
        scene_base.cpp
        scene_manager.cpp
)

add_subdirectory("scenes")
