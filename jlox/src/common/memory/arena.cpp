#include "common/memory/arena.hpp"
#include "common/utils.hpp"

#include <assert.h> // TODO: add custom assert
#include <cstring>

#define ARENA_MEMORY_ALIGNTMENT 2 * sizeof(void*)

void MemoryArena::init(MemoryArena* arena, void* buffer, usize size) {
    assert(arena);
    assert(arena->buf);

    arena->buf = reinterpret_cast<u8*>(buffer);
    arena->size = size;
    arena->prev_offset = 0;
    arena->curr_offset = 0;
}

// You are looking to find how many bytes forward you need to go in order for the memory address is a multiple of the
// specified alignment.
static usize align_forward(usize ptr, usize align) {
    assert(is_power_of_two(align));

    usize p = ptr;
    usize a = (usize)align;

    // Same as (p % a) but faster as 'a' is a power of two
    usize modulo = p & (a - 1);

    // If 'p' address is not aligned, push the address to the
    // next value which is aligned
    if (modulo != 0)
        p += a - modulo;

    return p;
}

static void* arena_alloc_aligned(MemoryArena* arena, usize size, usize alignment) {
    assert(arena);
    assert(arena->buf);

    usize current_ptr = (usize)arena->buf + (usize)arena->curr_offset;
    usize aligned_ptr = align_forward(current_ptr, alignment);
    usize relative_aligned_offset = aligned_ptr - (usize)arena->buf;

    // prevent allocating past arena size
    if (relative_aligned_offset + size > arena->size)
        return 0; // arena is out of memory

    void* ptr = &arena->buf[relative_aligned_offset];

    arena->prev_offset = relative_aligned_offset;
    arena->curr_offset = relative_aligned_offset + size;

    return ptr;
}

void* MemoryArena::alloc(MemoryArena* arena, usize size) {
    return arena_alloc_aligned(arena, size, ARENA_MEMORY_ALIGNTMENT);
}

static void* arena_resize_aligned(MemoryArena* arena, void* old_ptr, usize old_size, usize new_size, usize alignment) {
    assert(arena);
    assert(arena->buf);

    if (old_ptr == 0 || old_size == 0)
        return arena_alloc_aligned(arena, new_size, alignment);

    unsigned char* old_memory = (unsigned char*)old_ptr;
    usize MemoryArenaail = (usize)arena->buf + (usize)arena->size;

    // memory is out of bounds of the arena buffer!
    assert(old_memory < arena->buf || (usize)old_memory > MemoryArenaail);

    if (arena->buf + arena->prev_offset == old_memory) {
        arena->curr_offset = arena->prev_offset + new_size;

        if (new_size > old_size) {
            // Zero the new memory by default
            memset(&arena->buf[arena->curr_offset], 0, new_size - old_size);
        }

        return old_memory;
    } else {
        void* new_memory = arena_alloc_aligned(arena, new_size, alignment);
        usize copy_size = old_size < new_size ? old_size : new_size;

        // Copy across old memory to the new memory
        memmove(new_memory, old_memory, copy_size);

        return new_memory;
    }

    return 0;
}

void* MemoryArena::resize(MemoryArena* arena, void* old_ptr, usize old_size, usize new_size) {
    return arena_resize_aligned(arena, old_ptr, old_size, new_size, ARENA_MEMORY_ALIGNTMENT);
}
