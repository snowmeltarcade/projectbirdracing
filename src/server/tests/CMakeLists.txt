set(SERVER_TEST_PROJECT_NAME "${SERVER_PROJECT_NAME}_tests")

add_executable(
    "${SERVER_TEST_PROJECT_NAME}"
    main.cpp
    ../scene/scene_factory.cpp
)

target_link_libraries(
    "${SERVER_TEST_PROJECT_NAME}"
    Catch2::Catch2
    "${SHARED_PROJECT_NAME}"
)

target_include_directories(
    "${SERVER_TEST_PROJECT_NAME}"
    SYSTEM PRIVATE
        "${CATCH2_INCLUDE_DIRS}"
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_subdirectory("scene")

catch_discover_tests("${SERVER_TEST_PROJECT_NAME}")
