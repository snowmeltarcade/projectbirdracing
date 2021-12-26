add_executable("${SERVER_PROJECT_NAME}")

target_link_libraries(
    "${SERVER_PROJECT_NAME}"
    PRIVATE
        "${SHARED_PROJECT_NAME}"
		${SDL2_LIBRARIES}
		${Vulkan_LIBRARIES}
)

target_sources(
    "${SERVER_PROJECT_NAME}"
	PUBLIC
    PRIVATE
        main.cpp
)

target_include_directories(
	"${SERVER_PROJECT_NAME}"
	SYSTEM PRIVATE
		${SDL2_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}
		${Vulkan_INCLUDE_DIRS}
		${VMA_INCLUDE_DIRS}
)

install(
	TARGETS
		"${SERVER_PROJECT_NAME}"
	DESTINATION
		"${INSTALL_DIRECTORY}"
)

add_subdirectory("scene")
add_subdirectory("tests")
