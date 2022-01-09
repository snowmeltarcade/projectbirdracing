#include "bytes.h"
#include "kilobytes.h"
#include "megabytes.h"
#include "gigabytes.h"

namespace pbr::shared::memory {
    gigabytes::gigabytes(bytes value) {
        this->_value = value.get_value() / static_cast<type>(1024u) / static_cast<type>(1024u) / static_cast<type>(1024u);
    }

    gigabytes::gigabytes(kilobytes value) {
        this->_value = value.get_value() / static_cast<type>(1024u) / static_cast<type>(1024u);
    }

    gigabytes::gigabytes(megabytes value) {
        this->_value = value.get_value() / static_cast<type>(1024u);
    }
}
