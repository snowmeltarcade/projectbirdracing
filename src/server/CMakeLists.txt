add_executable("${SERVER_PROJECT_NAME}")

target_link_libraries(
    "${SERVER_PROJECT_NAME}"
    PRIVATE
        "${SHARED_PROJECT_NAME}"
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
		${CMAKE_CURRENT_SOURCE_DIR}
)

install(
	TARGETS
		"${SERVER_PROJECT_NAME}"
	DESTINATION
		"${INSTALL_DIRECTORY}"
)

add_subdirectory("scene")
add_subdirectory("tests")
