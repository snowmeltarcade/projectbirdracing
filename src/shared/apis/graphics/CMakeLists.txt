target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        application_information.h
        igraphics_manager.h
        performance_settings.h
        vulkan_graphics_manager.h
        vulkan_performance_settings.h
    PRIVATE
        vulkan_graphics_manager.cpp
        vulkan_performance_settings.cpp
)

add_subdirectory("vulkan")
