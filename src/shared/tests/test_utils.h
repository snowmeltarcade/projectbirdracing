#pragma once

#include <filesystem>

/// The current working directory of the test executable
extern std::filesystem::path current_working_directory;

/// Returns the file path of a file in the `test_data` directory
/// \param relative_path The relative path from the `test_data` directory
/// \returns The file path of a file in the `test_data` directory
std::filesystem::path get_test_data_file_path(std::filesystem::path relative_path) noexcept;
