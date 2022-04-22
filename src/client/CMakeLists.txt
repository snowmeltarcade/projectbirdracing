add_executable("${CLIENT_PROJECT_NAME}")

target_link_libraries(
    "${CLIENT_PROJECT_NAME}"
    PRIVATE
        "${SHARED_PROJECT_NAME}"
)

target_sources(
    "${CLIENT_PROJECT_NAME}"
	PUBLIC
    PRIVATE
        main.cpp
)

target_include_directories(
	"${CLIENT_PROJECT_NAME}"
	SYSTEM PRIVATE
		${SDL2_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}
		${Vulkan_INCLUDE_DIRS}
		${VMA_INCLUDE_DIRS}
		${GLEW_INCLUDE_DIRS}
)

install(
	TARGETS
		"${CLIENT_PROJECT_NAME}"
	DESTINATION
		"${INSTALL_DIRECTORY}"
)

add_subdirectory("tests")
