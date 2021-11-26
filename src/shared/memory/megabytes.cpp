#include "bytes.h"
#include "kilobytes.h"
#include "megabytes.h"
#include "gigabytes.h"

namespace pbr::shared::memory {
    megabytes::megabytes(bytes value) {
        this->_value = value.get_value() / 1024u / 1024u;
    }

    megabytes::megabytes(kilobytes value) {
        this->_value = value.get_value() / 1024u;
    }

    megabytes::megabytes(gigabytes value) {
        this->_value = value.get_value() * 1024u;
    }
}
