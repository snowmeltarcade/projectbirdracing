target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        application_information.h
        igraphics_manager.h
        vulkan_graphics_manager.h
    PRIVATE
        vulkan_graphics_manager.cpp
)

add_subdirectory("vulkan")
