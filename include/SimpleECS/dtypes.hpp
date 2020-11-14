#pragma once

#include <inttypes.h>
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define i8  int8_t
#define i16 int16_t
#define i32 int32_t


#include <chrono>
#define millisecs u32
#define timePoint std::chrono::steady_clock::time_point