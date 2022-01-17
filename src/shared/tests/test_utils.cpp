#include "test_utils.h"

std::filesystem::path current_working_directory;

std::filesystem::path get_test_data_file_path(std::filesystem::path relative_path) noexcept {
    auto path = current_working_directory / "../../shared/tests/test_data/" / relative_path;
    return path.lexically_normal();
}
