#pragma once

#include "common/common.hpp"

struct Location {
    const char* source;
    i32 line;
    i32 column;
};
