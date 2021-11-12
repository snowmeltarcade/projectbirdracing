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

install(
	TARGETS
		"${CLIENT_PROJECT_NAME}"
	DESTINATION
		"${INSTALL_DIRECTORY}"
)

add_subdirectory("tests")
