/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "../gfx/gfx.h"

#include "../main.h"
#include "../gfx/gl.h"
#include "../gfx/mat.h"
#include "../gfx/shader.h"
#include "../gfx/texture.h"
#include "../sdl/sdl.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

float matOrthoProj[16];

int    screenWidth          = 800;
int    screenHeight         = 600;
int    screenRefreshRate    = 60;
size_t vboTrisCount         = 0;
size_t drawCallCount        = 0;

bool  gfxUseSubData = false;

GLenum glCheckError_(const char *file, int line){
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR){
		const char *error = "Unknown";
		switch (errorCode){
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM";                  break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE";                 break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION";             break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY";                 break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		fprintf(stderr,"glError: (%i) %s in (%s:%i)\n",errorCode,error,file,line);
	}
	return errorCode;
}

/* (re)initialize some OpenGL Values, can be called multiple times, and for the most part
 * shouldn't have much of an effect, definetly has to be called though when the Viewport Dimensions
 * have changed
 */
void initGL(){
	if(!glInitialize()){
		exit(3);
	}
	glClearColor( 0.2f, 0.f, 0.f, 1.f );
	glViewport(0,0,screenWidth,screenHeight);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc( GL_LEQUAL );

	glScissor(0,0,screenWidth,screenHeight);
	glEnable(GL_SCISSOR_TEST);
}

static void renderTerminal(){

}

/* Render a single Frame */
void renderFrame(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderTerminal();
	swapWindow();
}
