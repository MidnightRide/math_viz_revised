/* date = June 30th 2022 2:55 pm */

#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>

#define SCREEN_WIDTH 1280.f
#define SCREEN_HEIGHT 720.f

#define ASSERT(x) if (!(x)) __debugbreak();

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef uint32_t b32;
typedef uint8_t b8;

typedef float f32;
typedef double f64;

#define TRUE 1
#define FALSE 0

#endif //DEFINES_H
