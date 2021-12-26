#pragma once

#include <string>
#include <cstdint>

namespace pbr::shared::apis::graphics {
    /// Information about the application that the graphics API needs to know
    struct application_information {
        /// The name of this application
        std::string _application_name;

        /// The name of this game engine
        std::string _engine_name;

        /// The application's major version number
        uint32_t _version_major {0u};

        /// The application's minor version number
        uint32_t _version_minor {0u};

        /// The application's build version number
        uint32_t _version_build {0u};
    };
}
