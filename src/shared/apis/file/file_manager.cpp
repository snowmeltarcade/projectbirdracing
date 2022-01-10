#include "file_manager.h"

#include <fstream>

namespace pbr::shared::apis::file {
    std::optional<std::vector<std::byte>> file_manager::read_file_bytes(const utils::uri& uri) const noexcept {
        // we are only supporting `file://` at the moment
        if (uri.scheme != "file") {
            return {};
        }

        std::ifstream fs(uri.path, std::ios::ate | std::ios::binary);
        if (!fs.is_open()) {
            return {};
        }

        std::vector<std::byte> bytes;

        auto file_size = fs.tellg();
        bytes.resize(file_size);

        fs.seekg(0);

        fs.read(reinterpret_cast<char*>(bytes.data()), file_size);

        // make sure all the expected bytes in `read` above were read
        if (fs.gcount() != file_size) {
            return {};
        }

        fs.close();

        return bytes;
    }

    std::future<std::optional<std::vector<std::byte>>> file_manager::read_file_bytes_async(
        const utils::uri& uri) const noexcept {
        auto f = std::async(std::launch::async, [this, uri](){
            auto result = this->read_file_bytes(uri);
            return result;
        });

        return f;
    }

    std::optional<std::string> file_manager::read_file_text(const utils::uri& uri) const noexcept {
        // we are only supporting `file://` at the moment
        if (uri.scheme != "file") {
            return {};
        }

        std::ifstream fs(uri.path, std::ios::ate);
        if (!fs.is_open()) {
            return {};
        }

        std::string text;
        fs >> text;

        fs.close();

        return text;
    }

    std::future<std::optional<std::string>> file_manager::read_file_text_async(const utils::uri& uri) const noexcept {
        auto f = std::async(std::launch::async, [this, uri](){
            auto result = this->read_file_text(uri);
            return result;
        });

        return f;
    }
}
