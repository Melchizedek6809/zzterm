/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "termBuffer.h"
#include "gfx/gfx.h"
#include "gfx/gl.h"
#include "gfx/shader.h"
#include "gfx/texture.h"
#include <stdlib.h>
#include <string.h>

static void vertexSet(termBufferVertex *v, u16 x, u16 y, u16 bufX, u16 bufY) {
    *v = (termBufferVertex){x, y, bufX, bufY};
}

static void termBufferPosition(termBufferVertex *v, u16 x, u16 y, u16 bufferX,
                               u16 bufferY, u16 w, u16 h) {
    vertexSet(&v[0], x, y, bufferX, bufferY);
    vertexSet(&v[1], x + w, y, bufferX, bufferY);
    vertexSet(&v[2], x + w, y + h, bufferX, bufferY);

    vertexSet(&v[3], x + w, y + h, bufferX, bufferY);
    vertexSet(&v[4], x, y + h, bufferX, bufferY);
    vertexSet(&v[5], x, y, bufferX, bufferY);
}

static void termBufferBuild(termBuffer *buf) {
    const u16 w = 16;
    const u16 h = 16;
    for (int x = 0; x < buf->width; x++) {
        for (int y = 0; y < buf->height; y++) {
            termBufferVertex *v = &buf->verts[(y * buf->width + x) * 6];
            termBufferPosition(v, x * w, y * h, x, y, w, h);
        }
    }
}

void termBufferAppend(termBuffer *buf, char *src) {
    while (*src) {
        const unsigned char c = *src++;
        if (c > 32) {
            buf->buffer[buf->cursor++] = c | 0xFF00;
        } else {
            buf->cursor++;
        }
    }
    buf->dirty = true;
}

termBuffer *termBufferNew(u16 width, u16 height) {
    termBuffer *buf = malloc(sizeof(termBuffer));
    buf->width = width;
    buf->height = height;
    buf->buffer = calloc(1, width * height * sizeof(buf->buffer[0]));
    buf->cursor = 0;
    buf->texBuffer = NULL;
    buf->vao = buf->vbo = 0;
    buf->dirty = true;
    buf->verts = calloc(1, width * height * 6 * sizeof(termBufferVertex));
    termBufferBuild(buf);
    termBufferAppend(buf, "!!! Test");
    return buf;
}

void termBufferClear(termBuffer *buf) {
    if (buf == NULL) {
        return;
    }
    memset(buf->buffer, 0, buf->width * buf->height * sizeof(buf->buffer[0]));
    buf->dirty = true;
}

void termBufferFree(termBuffer *buf) {
    if (buf == NULL) {
        return;
    }
    textureFree(buf->texBuffer);
    buf->texBuffer = NULL;
    free(buf->buffer);
    buf->buffer = NULL;
    free(buf->verts);
    buf->verts = NULL;
    free(buf);
}

static void termBufferDrawFast(termBuffer *buf) {
    if (buf->vao == 0) {
        glGenVertexArrays(1, &buf->vao);
    }
    glBindVertexArray(buf->vao);

    if (buf->vbo == 0) {
        glGenBuffers(1, &buf->vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, buf->vbo);
    if (buf->dirty) {
        glBufferData(GL_ARRAY_BUFFER,
                     buf->width * buf->height * 6 * sizeof(termBufferVertex),
                     buf->verts, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
            SHADER_ATTRIDX_POS, 2, GL_SHORT, GL_FALSE, sizeof(termBufferVertex),
            (void *)(((char *)&buf->verts[0].x) - ((char *)buf->verts)));
        glVertexAttribIPointer(
            SHADER_ATTRIDX_BUF_POS, 2, GL_SHORT, sizeof(termBufferVertex),
            (void *)(((char *)&buf->verts[0].bufX) - ((char *)buf->verts)));
        glEnableVertexAttribArray(SHADER_ATTRIDX_POS);
        glEnableVertexAttribArray(SHADER_ATTRIDX_BUF_POS);
        if (buf->texBuffer == NULL) {
            buf->texBuffer = textureNewRaw();
        }
        textureLoadTermBuffer(buf->texBuffer, buf->width, buf->height,
                              buf->buffer);
        buf->dirty = false;
    }
    GLuint textures[2] = {
        tFont->ID,
        buf->texBuffer->ID,
    };
    glBindTextures(0, countof(textures), textures);
    shaderCurTex(sTermBuffer, 0);
    shaderTextBuf(sTermBuffer, 1);
    glDrawArrays(GL_TRIANGLES, 0, buf->width * buf->height * 6);
}

static void termBufferBind(termBuffer *buf) {
    (void)buf;
    shaderBind(sTermBuffer);
    shaderMatrix(sTermBuffer, matOrthoProj);
}

void termBufferDraw(termBuffer *buf) {
    if (buf == NULL) {
        return;
    }
    termBufferBind(buf);
    termBufferDrawFast(buf);
}