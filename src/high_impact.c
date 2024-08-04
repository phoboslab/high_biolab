// This will include all high_impact code into this one .c file, forming a
// "unity build" of the whole library. We do this to 
// 1. speed up compile times
// 2. facilitate some optimizations that the compiler would not be able to do
//    when all files are compiled into separate objects
// 3. make the Makefile a bit simpler - only this file needs to be compiled

#include "high_impact.h"

// Order is important here. Particularly we need to include render.c before
// platform.c as the renderer may use an OpenGL loader that interferes with
// with the gl version loaded by the platform.

#include "../high_impact/src/alloc.c"
#include "../high_impact/src/animation.c"
#include "../high_impact/src/camera.c"
#include "../high_impact/src/engine.c"
#include "../high_impact/src/entity.c"
#include "../high_impact/src/font.c"
#include "../high_impact/src/image.c"
#include "../high_impact/src/input.c"
#include "../high_impact/src/map.c"
#include "../high_impact/src/noise.c"
#include "../high_impact/src/render.c"
#include "../high_impact/src/platform.c"
#include "../high_impact/src/sound.c"
#include "../high_impact/src/trace.c"
#include "../high_impact/src/utils.c"
