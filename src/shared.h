/* (c) copyright 2025 Lawrence D. Kern /////////////////////////////////////// */

#define Assert(Cond) do { if(!(Cond)) { __builtin_trap(); } } while(0)

#include <stdint.h>
typedef int32_t s32;
typedef uint32_t u32;

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
