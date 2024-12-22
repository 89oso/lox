#pragma once

#include <cstddef>
#include <cstdint>

typedef int8_t i8;
typedef uint8_t u8;

typedef int16_t i16;
typedef uint16_t u16;

typedef int32_t i32;
typedef uint32_t u32;

typedef int64_t i64;
typedef uint64_t u64;

typedef size_t usize;

typedef float f32;
typedef double f64;

#include <memory>
#include <string>

template<typename T>
using Arc = std::shared_ptr<T>;

template<typename T, typename... TArgs>
constexpr Arc<T> create_arc(TArgs&&... args) {
    return std::make_unique<T>(std::forward<TArgs>(args)...);
}

template<typename T>
using Box = std::unique_ptr<T>;

template<typename T, typename... TArgs>
constexpr Box<T> create_box(TArgs&&... args) {
    return std::make_unique<T>(std::forward<TArgs>(args)...);
}
