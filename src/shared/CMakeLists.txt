add_library("${SHARED_PROJECT_NAME}")

target_link_libraries("${SHARED_PROJECT_NAME}"
    PUBLIC
        glm::glm
)

add_subdirectory("apis")
add_subdirectory("game")
add_subdirectory("memory")
add_subdirectory("tests")
add_subdirectory("utils")
