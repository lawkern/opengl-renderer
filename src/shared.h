/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#define Assert(Cond) do { if(!(Cond)) { __builtin_trap(); } } while(0)
#define Array_Count(Array) (sizeof(Array) / sizeof((Array)[0]))

#include <stdint.h>
typedef int32_t s32;
typedef uint8_t u8;
typedef uint32_t u32;

#include <stddef.h>
typedef ptrdiff_t size;

typedef struct {
   float X;
   float Y;
} vec2;

typedef struct {
   float X;
   float Y;
   float Z;
} vec3;

typedef struct {
   float R;
   float G;
   float B;
   float A;
} vec4;
