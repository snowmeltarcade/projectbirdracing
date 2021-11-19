set(SHARED_TEST_PROJECT_NAME "${SHARED_PROJECT_NAME}_tests")

add_executable(
    "${SHARED_TEST_PROJECT_NAME}"
    main.cpp
)

target_link_libraries(
    "${SHARED_TEST_PROJECT_NAME}"
    Catch2::Catch2
    "${SHARED_PROJECT_NAME}"
)

target_include_directories(
    "${SHARED_TEST_PROJECT_NAME}"
    SYSTEM PRIVATE
    "${CATCH2_INCLUDE_DIRS}"
)

add_subdirectory("apis")
add_subdirectory("memory")

catch_discover_tests("${SHARED_TEST_PROJECT_NAME}")
