#pragma once
#include "../common.h"
#include "gl.h"

extern float  matProjection[16], matView[16];
extern int    screenWidth;
extern int    screenHeight;
extern int    screenRefreshRate;
extern bool   gfxUseSubData;
extern size_t vboTrisCount;
extern size_t drawCallCount;

void initGL            ();
void renderFrame       ();

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__)
