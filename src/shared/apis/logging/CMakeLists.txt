target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        endpoint.h
        log_levels.h
        log_manager.h
    PRIVATE
        log_manager.cpp
        log_levels.cpp
)

add_subdirectory("endpoints")
