#pragma once
#include "../common.h"

texture *textureNewRaw            ();
texture *textureNew               (const u8 *data, uint dataLen,const char *filename);
texture *textureNewArray          (const u8 *data, uint dataLen,const char *filename, int d);
void     textureLoadSurface       (texture *t, uint w, uint h, const void *data, bool linear);
void     textureFree              ();
void     textureBind              (const texture *tex);
void     textureBindID            (uint ID);
void     textureInit              ();
void     textureReload            ();
void     checkTexturesForReloading();

extern texture *tFont;
