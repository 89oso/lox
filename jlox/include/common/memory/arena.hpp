#pragma once

#include "../common.hpp"

struct MemoryArena {
    u8* buf;
    usize size;
    usize prev_offset;
    usize curr_offset;

    static void init(MemoryArena* arena, void* buffer, usize size);
    static void* alloc(MemoryArena* arena, usize size);
    static void* resize(MemoryArena* arena, void* old_ptr, usize old_size, usize new_size);
};
