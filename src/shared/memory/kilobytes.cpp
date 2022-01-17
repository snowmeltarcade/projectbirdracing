#include "bytes.h"
#include "kilobytes.h"
#include "megabytes.h"
#include "gigabytes.h"

namespace pbr::shared::memory {
    kilobytes::kilobytes(bytes value) {
        this->_value = value.get_value() / static_cast<type>(1024u);
    }

    kilobytes::kilobytes(megabytes value) {
        this->_value = value.get_value() * static_cast<type>(1024u);
    }

    kilobytes::kilobytes(gigabytes value) {
        this->_value = value.get_value() * static_cast<type>(1024u) * static_cast<type>(1024u);
    }
}
