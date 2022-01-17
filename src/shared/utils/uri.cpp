#include "uri.h"
#include "strings.h"

namespace pbr::shared::utils {
    std::optional<uri> build_uri(const std::string& s) noexcept {
        if (s.empty()) {
            return {};
        }

        // We are only supporting a small subset of URIs, as we only need
        // a small subset of features right now. We can expand this as needs
        // arise.
        if (!starts_with(s, "file:///")) {
            return {};
        }

        auto parts = split("file:///", s);
        if (parts.size() != 2 || parts[1].empty()) {
            return {};
        }

        return { { "file", parts[1] } };
    }
}
