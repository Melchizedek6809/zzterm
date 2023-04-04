#pragma once
#include "common.h"
#include "gfx/gl.h"
#include "gfx/texture.h"

#pragma pack(push, 1)
typedef struct termBufferVertex {
    i16 x, y;
    i16 bufX, bufY;
} termBufferVertex;
#pragma pack(pop)

typedef struct {
    size_t cursor;
    u16 width, height;
    texture *texBuffer;
    u16 *buffer;

    termBufferVertex *verts;
    GLuint vao, vbo;
    bool dirty;
} termBuffer;

termBuffer *termBufferNew(u16 width, u16 height);
void termBufferFree(termBuffer *buf);
void termBufferClear(termBuffer *buf);
void termBufferDraw(termBuffer *buf);