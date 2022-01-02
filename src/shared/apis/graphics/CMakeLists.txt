target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        application_information.h
        color.h
        igraphics_manager.h
        performance_settings.h
        renderable_entities.h
        renderable_entity_2d.h
        vulkan_graphics_manager.h
        vulkan_performance_settings.h
    PRIVATE
        renderable_entities.cpp
        vulkan_graphics_manager.cpp
        vulkan_performance_settings.cpp
)

add_subdirectory("vulkan")
