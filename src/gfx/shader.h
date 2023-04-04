#pragma once
#include "../common.h"
#include "../gfx/gl.h"

#define SHADER_ATTRIDX_POS       0
#define SHADER_ATTRIDX_BUF_POS   1

#define SHADER_ATTRMASK_POS      (1 << SHADER_ATTRIDX_POS)
#define SHADER_ATTRMASK_BUF_POS  (1 << SHADER_ATTRIDX_BUF_POS)

typedef struct {
	uint pID;
	uint vsID,fsID;
	const char *vss,*fss;
	const char *defines;
	const char *name;
	uint attrMask;
	int lMVP, lCurTex, lTextBuf;
} shader;

void    shaderInit();
void    shaderInitBlockMesh();
void    shaderFree();
void    shaderBind       (shader *s);
void    shaderMatrix     (shader *s, float mvp[16]);
void    shaderCurTex     (shader *s, GLuint texUnit);
void    shaderTextBuf    (shader *s, GLuint texUnit);

extern shader *sTermBuffer;