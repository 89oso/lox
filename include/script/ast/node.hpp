#pragma once

#include "visitor.hpp"

struct Node {
    using ptr = std::unique_ptr<Node>;

    virtual ~Node() = default;
    virtual void accept(Visitor* visitor) = 0;
};
