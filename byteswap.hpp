#pragma once
#include <cstdint>

uint16_t byte_swap(uint16_t);
int16_t byte_swap(int16_t);
uint32_t byte_swap(uint32_t);
int32_t byte_swap(int32_t);
uint64_t byte_swap(uint64_t);
int64_t byte_swap(int64_t);

inline uint16_t big_to_little(uint16_t v) { return byte_swap(v); }
inline uint16_t little_to_big(uint16_t v) { return byte_swap(v); }
inline uint32_t big_to_little(uint32_t v) { return byte_swap(v); }
inline uint32_t little_to_big(uint32_t v) { return byte_swap(v); }
inline uint64_t big_to_little(uint64_t v) { return byte_swap(v); }
inline uint64_t little_to_big(uint64_t v) { return byte_swap(v); }
