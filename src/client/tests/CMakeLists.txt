set(CLIENT_TEST_PROJECT_NAME "${CLIENT_PROJECT_NAME}_tests")

add_executable(
    "${CLIENT_TEST_PROJECT_NAME}"
    main.cpp
)

target_link_libraries(
    "${CLIENT_TEST_PROJECT_NAME}"
    Catch2::Catch2
    "${SHARED_PROJECT_NAME}"
)

target_include_directories(
    "${CLIENT_TEST_PROJECT_NAME}"
    SYSTEM PRIVATE
    "${CATCH2_INCLUDE_DIRS}"
)

catch_discover_tests("${CLIENT_TEST_PROJECT_NAME}")
