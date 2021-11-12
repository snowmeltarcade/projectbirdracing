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

install(
	TARGETS
		"${SERVER_PROJECT_NAME}"
	DESTINATION
		"${INSTALL_DIRECTORY}"
)

add_subdirectory("tests")
