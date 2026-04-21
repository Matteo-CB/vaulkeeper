#pragma once

#include <cassert>

#if defined(_MSC_VER)
    #define VK_LIKELY(x) (x)
    #define VK_UNLIKELY(x) (x)
    #define VK_FORCE_INLINE __forceinline
    #define VK_NO_INLINE __declspec(noinline)
    #define VK_RESTRICT __restrict
#else
    #define VK_LIKELY(x) __builtin_expect(!!(x), 1)
    #define VK_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define VK_FORCE_INLINE inline __attribute__((always_inline))
    #define VK_NO_INLINE __attribute__((noinline))
    #define VK_RESTRICT __restrict__
#endif

#define VK_ASSERT(expr) assert(expr)

#define VK_NONCOPYABLE(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete

#define VK_NONMOVABLE(Class) \
    Class(Class&&) = delete; \
    Class& operator=(Class&&) = delete

#define VK_DEFAULT_MOVE(Class) \
    Class(Class&&) noexcept = default; \
    Class& operator=(Class&&) noexcept = default
