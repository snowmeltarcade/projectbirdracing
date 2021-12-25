# FindVulkan.cmake relies in the `VULKAN_SDK` environment variable
# See: https://github.com/Kitware/CMake/blob/master/Modules/FindVulkan.cmake

if (WIN32)
    set(VULKAN_SDK "${LIBRARIES_ROOT_DIR}/vulkan/Windows/windows")
elseif (LINUX)
    set(VULKAN_SDK "${LIBRARIES_ROOT_DIR}/vulkan/Linux/linux")
else()
    set(VULKAN_SDK "${LIBRARIES_ROOT_DIR}/vulkan/Darwin/macOS")

    add_definitions(-DREQUIRES_MOLTEN_VK)
    add_definitions(-DVK_LAYER_PATH="${VULKAN_SDK}/share/vulkan/explicit_layer.d")

    add_definitions(-DMOLTENVK_LIB_FILENAME="lib/libMoltenVK.dylib")
    add_definitions(-DVK_ICD_FILENAMES="share/vulkan/icd.d/MoltenVK_icd.json")

    file(COPY "${VULKAN_SDK}/lib/libMoltenVK.dylib"
         DESTINATION "${PROJECT_BINARY_DIR}/bin/lib/")

    file(COPY "${VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json"
         DESTINATION "${PROJECT_BINARY_DIR}/bin/share/vulkan/icd.d/")
endif()

set(ENV{VULKAN_SDK} ${VULKAN_SDK})
add_definitions(-DVULKAN_SDK_PATH="${VULKAN_SDK}")

find_package(Vulkan REQUIRED)
