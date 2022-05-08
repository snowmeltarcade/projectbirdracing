set(SHARED_TEST_PROJECT_NAME "${SHARED_PROJECT_NAME}_tests")

add_executable(
    "${SHARED_TEST_PROJECT_NAME}"
    main.cpp
    test_utils.cpp
    test_utils.h
)

target_link_libraries(
    "${SHARED_TEST_PROJECT_NAME}"
    Catch2::Catch2
    ${Vulkan_LIBRARIES}
    ${SDL_LIBRARIES}
    "${SHARED_PROJECT_NAME}"
)

target_include_directories(
    "${SHARED_TEST_PROJECT_NAME}"
    SYSTEM PRIVATE
    ${Vulkan_INCLUDE_DIRS}
    ${VMA_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    "${CATCH2_INCLUDE_DIRS}"
    ${GLEW_INCLUDE_DIRS}
)

add_subdirectory("apis")
add_subdirectory("data")
add_subdirectory("diagnostics")
add_subdirectory("entity_component_system")
add_subdirectory("game")
add_subdirectory("memory")
add_subdirectory("resource")
add_subdirectory("scene")
add_subdirectory("utils")
add_subdirectory("test_data")

catch_discover_tests("${SHARED_TEST_PROJECT_NAME}")
