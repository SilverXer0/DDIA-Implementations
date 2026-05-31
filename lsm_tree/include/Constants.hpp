#pragma once
#include <string>

namespace Constants {
    inline constexpr size_t MIN_BYTE_VALUE = 0;
    inline constexpr size_t MAX_BYTE_THRESHOLD = 1024; // this is 1 KB - an actual impl would use 4 * 1024 * 1024, which is 4 MB
}