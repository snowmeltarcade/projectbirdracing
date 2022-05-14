target_sources(
    "${SHARED_PROJECT_NAME}"
    PUBLIC
        repository.h
    PRIVATE
        repository.cpp
)

add_subdirectory("components")
