/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */

#include "shader.h"
#include "gl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

shader  shaderList[16];
int     shaderCount = 0;
uint    activeProgram = 0;

shader *sTermBuffer = NULL;

extern u8 shader_textShader_frag_data[];
extern u8 shader_textShader_vert_data[];

static shader *shaderNew(shader *slot,const char *name,const char *vss,const char *fss,uint attrMask);

void shaderInit(){
	sTermBuffer  = shaderNew(NULL, "termBuffer",  (const char *)shader_textShader_vert_data,     (const char *)shader_textShader_frag_data,     SHADER_ATTRMASK_POS | SHADER_ATTRMASK_BUF_POS);
}

static void shaderPrintLog(uint obj, const char *msg, const char *src){
	int infologLength = 0;
	int maxLength;

	if(glIsShader(obj)){
		glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
	} else {
		glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
	}
	char infoLog[maxLength];

	if (glIsShader(obj)){
		glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
	}else{
		glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);
	}

	if (infologLength > 0){
		printf("------------------\n%s\n\n%s\n\nSrc:\n%s\n\n------------------",msg,infoLog,src);
	}
}

static void compileVertexShader(shader *s,const char *name){
	(void)name;
	char buf[1 << 14];
	char *bufp[2] = {(char *)&buf,0};

	s->vsID = glCreateShader(GL_VERTEX_SHADER);
	#ifdef ZZTERM__GL_ES
	snprintf(buf,sizeof(buf),"#version 300 es\n"
"%s\n"
"precision mediump float;\n"
"precision mediump int;\n"
"\n"
"#line 0\n"
"%s",s->defines,s->vss);
	#elif defined(__APPLE__)
	snprintf(buf,sizeof(buf),"#version 330 core\n"
"%s\n"
"precision mediump float;\n"
"precision mediump int;\n"
"\n"
"#line 0\n"
"%s",s->defines,s->vss);
	#else
	snprintf(buf,sizeof(buf),"#version 140\n"
"%s\n"
"\n"
"#line 0\n"
"%s",s->defines,s->vss);
	#endif
	glShaderSource(s->vsID,1,(const GLchar **)&bufp,NULL);
	glCompileShader(s->vsID);
	shaderPrintLog(s->vsID,"Vertex Shader",buf);
	gfxObjectLabel(GL_SHADER,s->vsID,"%s VS",name);
	glAttachShader(s->pID,s->vsID);
}

static void compileFragmentShader(shader *s,const char *name){
	(void)name;
	char buf[1 << 14];
	char *bufp[2] = {(char *)&buf,0};

	s->fsID = glCreateShader(GL_FRAGMENT_SHADER);
	#ifdef ZZTERM__GL_ES
	snprintf(buf,sizeof(buf),"#version 300 es\n"
"%s\n"
"precision mediump float;\n"
"precision mediump int;\n"
"precision lowp sampler2DArray;\n"
"\n"
"#line 0\n"
"%s",s->defines,s->fss);
	#elif defined(__APPLE__)
	snprintf(buf,sizeof(buf),"#version 330 core\n"
"%s\n"
"precision mediump float;\n"
"precision mediump int;\n"
"precision lowp sampler2DArray;\n"
"\n"
"#line 0\n"
"%s",s->defines,s->fss);
	#else
	snprintf(buf,sizeof(buf),"#version 130\n"
"%s\n"
"\n"
"#line 0\n"
"%s",s->defines,s->fss);
	#endif

	glShaderSource(s->fsID,1,(const GLchar **)&bufp,NULL);
	glCompileShader(s->fsID);
	shaderPrintLog(s->fsID,"Fragment Shader",buf);
	gfxObjectLabel(GL_SHADER,s->fsID,"%s FS",name);
	glAttachShader(s->pID,s->fsID);
}

static void shaderCompile(shader *s,const char *name){
	s->pID = glCreateProgram();
	gfxObjectLabel(GL_PROGRAM,s->pID,"%s", name);
	compileVertexShader(s,name);
	compileFragmentShader(s,name);

	if(s->attrMask & SHADER_ATTRMASK_POS){glBindAttribLocation(s->pID,SHADER_ATTRIDX_POS,"pos");}
	if(s->attrMask & SHADER_ATTRMASK_BUF_POS){glBindAttribLocation(s->pID,SHADER_ATTRIDX_BUF_POS,"bufPos");}

	glLinkProgram(s->pID);
	shaderPrintLog(s->pID,"Program","");

	s->lMVP        = glGetUniformLocation(s->pID,"matMVP");
	s->lCurTex     = glGetUniformLocation(s->pID,"curTex");
	s->lTextBuf    = glGetUniformLocation(s->pID,"textBuf");
}

static shader *shaderNew(shader *slot,const char *name,const char *vss,const char *fss,uint attrMask){
	shader *s = slot == NULL ? &shaderList[shaderCount++] : slot;
	// If overwriting an existing shader slot, free up old shader resources
	if(s->pID){
		glDeleteProgram(s->pID);
	}
	if(s->vsID){
		glDeleteShader (s->vsID);
	}
	if(s->fsID){
		glDeleteShader (s->fsID);
	}
	s->name     = name;
	s->vss      = (char *)vss;
	s->fss      = (char *)fss;

	s->defines = "";

	s->pID        = 0;
	s->vsID       = 0;
	s->fsID       = 0;

	s->attrMask   = attrMask;
	s->lMVP       = -1;
	s->lCurTex    = -1;
	s->lTextBuf   = -1;

	shaderCompile(s,name);
	return s;
}

void shaderFree(){
	for(int i=0;i<shaderCount;i++){
		if(shaderList[i].pID){
			glDeleteProgram(shaderList[i].pID);
		}
		if(shaderList[i].vsID){
			glDeleteShader (shaderList[i].vsID);
		}
		if(shaderList[i].fsID){
			glDeleteShader (shaderList[i].fsID);
		}
	}
}

void shaderBind(shader *s){
	if(activeProgram == s->pID){return;}
	glUseProgram(s->pID);
}

void shaderMatrix(shader *s, float mvp[16]){
	if(s->lMVP == -1){
		fprintf(stderr,"%s has no location for matrix Uniform\n", s->name);
		exit(123);
	}
	glUniformMatrix4fv(s->lMVP,1,GL_FALSE,mvp);
}

void shaderCurTex(shader *s, GLuint texUnit){
	if(s->lCurTex == -1){
		fprintf(stderr,"%s has no location for curTex Uniform\n", s->name);
		exit(123);
	}
	glUniform1i(s->lCurTex, texUnit);
}

void shaderTextBuf(shader *s, GLuint texUnit){
	if(s->lTextBuf == -1){
		fprintf(stderr,"%s has no location for textBuf Uniform\n", s->name);
		exit(123);
	}
	glUniform1i(s->lTextBuf, texUnit);
}