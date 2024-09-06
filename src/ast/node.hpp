#pragma once

#include "visitor.hpp"

struct Node {
    virtual ~Node() = default;
    virtual void accept(Visitor* visitor) = 0;
};
