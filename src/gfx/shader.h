#pragma once
#include "../common.h"

#define SHADER_ATTRIDX_POS       0
#define SHADER_ATTRIDX_TEX       1
#define SHADER_ATTRIDX_COLOR     2

#define SHADER_ATTRMASK_POS      (1 << SHADER_ATTRIDX_POS)
#define SHADER_ATTRMASK_TEX      (1 << SHADER_ATTRIDX_TEX)
#define SHADER_ATTRMASK_COLOR    (1 << SHADER_ATTRIDX_COLOR)

void    shaderInit();
void    shaderInitBlockMesh();
void    shaderFree();
void    shaderBind       (shader *s);
void    shaderMatrix     (shader *s, float mvp[16]);

extern shader *sTextMesh;
