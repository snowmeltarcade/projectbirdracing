# FindVulkan.cmake relies in the `VULKAN_SDK` environment variable
# See: https://github.com/Kitware/CMake/blob/master/Modules/FindVulkan.cmake

if (WIN32)
    set(ENV{VULKAN_SDK} "${LIBRARIES_ROOT_DIR}/vulkan/Windows/windows")
elseif (LINUX)
    set(ENV{VULKAN_SDK} "${LIBRARIES_ROOT_DIR}/vulkan/Linux/linux")
else()
    set(ENV{VULKAN_SDK} "${LIBRARIES_ROOT_DIR}/vulkan/Darwin/macOS")

    add_compile_definitions(REQUIRES_MOLTEN_VK)

    file(COPY "${LIBRARIES_ROOT_DIR}/vulkan/Darwin/macOS/lib/libMoltenVK.dylib"
         DESTINATION "${PROJECT_BINARY_DIR}/bin")
endif()

find_package(Vulkan REQUIRED)
