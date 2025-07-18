#pragma once

#include <cstdint>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace poker {

/**
 * Cross-platform bit manipulation utilities
 * Provides portable implementations for common bit operations
 */

// Cross-platform popcount (population count - count number of set bits)
inline int popcount(uint32_t x) {
#ifdef _MSC_VER
    // MSVC: Use intrinsic
    return static_cast<int>(__popcnt(x));
#elif defined(__GNUC__) || defined(__clang__)
    // GCC/Clang: Use builtin
    return __builtin_popcount(x);
#else
    // Software fallback for maximum compatibility
    // Brian Kernighan's algorithm - efficient bit counting
    int count = 0;
    while (x) {
        count++;
        x &= x - 1;  // Clear the lowest set bit
    }
    return count;
#endif
}

// 64-bit version
inline int popcount64(uint64_t x) {
#ifdef _MSC_VER
    return static_cast<int>(__popcnt64(x));
#elif defined(__GNUC__) || defined(__clang__)
    return __builtin_popcountll(x);
#else
    // Software fallback
    int count = 0;
    while (x) {
        count++;
        x &= x - 1;
    }
    return count;
#endif
}

// Count leading zeros
inline int clz(uint32_t x) {
#ifdef _MSC_VER
    unsigned long index;
    return _BitScanReverse(&index, x) ? (31 - static_cast<int>(index)) : 32;
#elif defined(__GNUC__) || defined(__clang__)
    return x ? __builtin_clz(x) : 32;
#else
    // Software fallback
    if (x == 0) return 32;
    int count = 0;
    if (x <= 0x0000FFFF) { count += 16; x <<= 16; }
    if (x <= 0x00FFFFFF) { count += 8; x <<= 8; }
    if (x <= 0x0FFFFFFF) { count += 4; x <<= 4; }
    if (x <= 0x3FFFFFFF) { count += 2; x <<= 2; }
    if (x <= 0x7FFFFFFF) { count += 1; }
    return count;
#endif
}

// Count trailing zeros
inline int ctz(uint32_t x) {
#ifdef _MSC_VER
    unsigned long index;
    return _BitScanForward(&index, x) ? static_cast<int>(index) : 32;
#elif defined(__GNUC__) || defined(__clang__)
    return x ? __builtin_ctz(x) : 32;
#else
    // Software fallback
    if (x == 0) return 32;
    int count = 0;
    if ((x & 0x0000FFFF) == 0) { count += 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { count += 8; x >>= 8; }
    if ((x & 0x0000000F) == 0) { count += 4; x >>= 4; }
    if ((x & 0x00000003) == 0) { count += 2; x >>= 2; }
    if ((x & 0x00000001) == 0) { count += 1; }
    return count;
#endif
}

// Fast bit manipulation utilities for poker hand evaluation
namespace bit_ops {

// Check if only one bit is set (power of 2)
inline bool is_power_of_two(uint32_t x) {
    return x && !(x & (x - 1));
}

// Get the lowest set bit
inline uint32_t lowest_bit(uint32_t x) {
    return x & (-x);
}

// Clear the lowest set bit
inline uint32_t clear_lowest_bit(uint32_t x) {
    return x & (x - 1);
}

// Reverse bits in a 32-bit integer
inline uint32_t reverse_bits(uint32_t x) {
    x = ((x & 0xAAAAAAAA) >> 1) | ((x & 0x55555555) << 1);
    x = ((x & 0xCCCCCCCC) >> 2) | ((x & 0x33333333) << 2);
    x = ((x & 0xF0F0F0F0) >> 4) | ((x & 0x0F0F0F0F) << 4);
    x = ((x & 0xFF00FF00) >> 8) | ((x & 0x00FF00FF) << 8);
    return (x >> 16) | (x << 16);
}

// Pack multiple small integers into a single word
inline uint32_t pack_4x8(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return static_cast<uint32_t>(a) |
           (static_cast<uint32_t>(b) << 8) |
           (static_cast<uint32_t>(c) << 16) |
           (static_cast<uint32_t>(d) << 24);
}

// Unpack 4 bytes from a 32-bit word
inline void unpack_4x8(uint32_t packed, uint8_t& a, uint8_t& b, uint8_t& c, uint8_t& d) {
    a = static_cast<uint8_t>(packed & 0xFF);
    b = static_cast<uint8_t>((packed >> 8) & 0xFF);
    c = static_cast<uint8_t>((packed >> 16) & 0xFF);
    d = static_cast<uint8_t>((packed >> 24) & 0xFF);
}

} // namespace bit_ops

} // namespace poker
