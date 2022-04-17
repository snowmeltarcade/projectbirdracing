target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        application_information.h
        color.h
        igraphics_manager.h
        performance_settings.h
        renderable_entities.h
        renderable_entity_2d.h
    PRIVATE
        renderable_entities.cpp
)

add_subdirectory("vulkan")
