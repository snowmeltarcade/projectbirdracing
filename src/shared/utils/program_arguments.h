#pragma once

#include <exception>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace pbr::shared::utils {
    /// Helps extract and validate program arguments
    /// Currently, we only support returning the executable's path
    class program_arguments {
    public:
        /// Constructs the program arguments
        /// The first argument is taken as the executable's path
        /// Any further arguments should take the form: `-arg=value`
        /// \param arguments The program arguments
        program_arguments(std::vector<std::string> arguments) {
            if (arguments.empty()) {
                throw std::runtime_error("`arguments` is empty.");
            }

            if (!this->setup_arguments(arguments)) {
                throw std::runtime_error("Invalid program arguments.");
            }
        }

        /// Returns the executable's path
        /// \returns The executable's path
        std::filesystem::path get_executable_path() const noexcept {
            return this->_executable_path;
        }

    private:
        /// Sets up the arguments
        /// \param arguments The program arguments
        /// \returns `true` upon success, else `false`
        [[nodiscard]]
        bool setup_arguments(const std::vector<std::string>& arguments) noexcept;

        /// The program's path
        std::filesystem::path _executable_path;

        /// The formatted arguments
        std::unordered_map<std::string, std::string> _arguments;
    };
}
