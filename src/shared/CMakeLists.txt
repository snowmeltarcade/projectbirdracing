add_library("${SHARED_PROJECT_NAME}")

target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        types.h
)

if (IOS)
    target_link_libraries(
        "${SHARED_PROJECT_NAME}"
        "${OPENGLES_LIBRARIES}"
    )

    target_include_directories(
        "${SHARED_PROJECT_NAME}"
        SYSTEM PRIVATE
            ${OPENGLES_INCLUDE_DIR}
    )
else()
    target_link_libraries(
        "${SHARED_PROJECT_NAME}"
        PUBLIC
            OpenGL::GL
    )
endif()

target_link_libraries("${SHARED_PROJECT_NAME}"
    PUBLIC
        glm::glm
        ${SDL2_LIBRARIES}
        ${Vulkan_LIBRARIES}
        "nlohmann_json::nlohmann_json"
        "${GLEW_LIBRARIES}"
)

target_include_directories(
    "${SHARED_PROJECT_NAME}"
    SYSTEM PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${Vulkan_INCLUDE_DIRS}
        ${VMA_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
)

copy_sdl_dependencies(${PROJECT_BINARY_DIR})

add_subdirectory("apis")
add_subdirectory("data")
add_subdirectory("diagnostics")
add_subdirectory("entity_component_system")
add_subdirectory("game")
add_subdirectory("memory")
add_subdirectory("platform")
add_subdirectory("resource")
add_subdirectory("scene")
add_subdirectory("tests")
add_subdirectory("utils")
