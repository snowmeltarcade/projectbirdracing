add_library("${SHARED_PROJECT_NAME}")

target_link_libraries("${SHARED_PROJECT_NAME}"
    PUBLIC
        glm::glm
)

add_subdirectory("apis")
add_subdirectory("tests")