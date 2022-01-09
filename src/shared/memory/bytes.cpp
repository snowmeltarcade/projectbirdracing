#include "bytes.h"
#include "kilobytes.h"
#include "megabytes.h"
#include "gigabytes.h"

namespace pbr::shared::memory {
    bytes::bytes(kilobytes value) {
        this->_value = static_cast<type>(value.get_value()) * static_cast<type>(1024u);
    }

    bytes::bytes(megabytes value) {
        this->_value = static_cast<type>(value.get_value()) * static_cast<type>(1024u) * static_cast<type>(1024u);
    }

    bytes::bytes(gigabytes value) {
        this->_value = static_cast<type>(value.get_value()) *
                        static_cast<type>(1024u) * static_cast<type>(1024u) * static_cast<type>(1024u);
    }
}
