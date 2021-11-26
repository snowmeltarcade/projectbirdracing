#include "bytes.h"
#include "kilobytes.h"
#include "megabytes.h"
#include "gigabytes.h"

namespace pbr::shared::memory {
    bytes::bytes(kilobytes value) {
        this->_value = value.get_value() * 1024u;
    }

    bytes::bytes(megabytes value) {
        this->_value = value.get_value() * 1024u * 1024u;
    }

    bytes::bytes(gigabytes value) {
        this->_value = value.get_value() * 1024u * 1024u * 1024u;
    }
}
