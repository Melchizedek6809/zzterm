/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "sdl.h"
#include "../main.h"
#include "../gfx/gfx.h"
#include "../gfx/gl.h"

#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_hints.h>

bool textInputHandler(const SDL_Event *e);

SDL_Window* gWindow = NULL;
SDL_GLContext gContext;

uint  frameTimeLast       = 0;
uint  frameCountSinceLast = 0;
float curFPS              = 0;
uint  worstFrame          = 0;

void fpsTick() {
	uint curTicks = SDL_GetTicks();
	static uint lastFrame = 0;

	if(lastFrame == 0){lastFrame = curTicks;}
	worstFrame = MAX(worstFrame,curTicks - lastFrame);
	lastFrame = curTicks;

	frameCountSinceLast++;
	if(curTicks > frameTimeLast+1000){
		curFPS = ((float)curTicks - (float)frameTimeLast) / (float)frameCountSinceLast;
		curFPS = 1000.f / curFPS;
		frameTimeLast += (curTicks - frameTimeLast);
		frameCountSinceLast = 0;
	}
}

int GLVersionMajor = 0;
int GLVersionMinor = 0;

void initSDL(){
	SDL_DisplayMode dm;
	char windowTitle[64];

	snprintf(windowTitle,sizeof(windowTitle),"ZZTerm");

	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS,"0");
	uint initFlags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_AUDIO;
	if(SDL_Init( initFlags) < 0){
		fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}

	#ifdef ZZTERM__GL_ES
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
	#else
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
	#endif
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &GLVersionMajor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &GLVersionMinor);

	int cwflags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_GRABBED;
	if( SDL_GetDesktopDisplayMode(0, &dm) == 0){
		screenWidth  = dm.w;
		screenHeight = dm.h;
		screenRefreshRate = dm.refresh_rate;
	}

	gWindow = SDL_CreateWindow( windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, cwflags);
	if( gWindow == NULL ) {
		fprintf(stderr, "[CLI] Window could not be created! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}
	SDL_SetWindowGrab(gWindow,SDL_FALSE);

	gContext = SDL_GL_CreateContext( gWindow );
	if( gContext == NULL ){
		fprintf(stderr, "[CLI] OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
		exit(1);
	}
	#ifndef __HAIKU__
	if( SDL_GL_SetSwapInterval(-1)){ //Try to use AdaptiveVsync
		SDL_GL_SetSwapInterval(1);
	}
	#endif

	initGL();
}

void handleEvents(){
	SDL_Event e;

	while( SDL_PollEvent( &e ) != 0 ){
		switch(e.type){
		case SDL_QUIT:
			quit = true;
			break;
		case SDL_WINDOWEVENT:
			if((e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) || (e.window.event == SDL_WINDOWEVENT_RESIZED)){
				sdlResize(e.window.data1,e.window.data2);
			}
			break;
		case SDL_KEYDOWN:
			switch(e.key.keysym.scancode){
			case SDL_SCANCODE_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}
			break;
		}
		textInputHandler(&e);
	}
}

void sdlResize(int newW,int newH){
	if((newW == screenWidth) && (newH == screenHeight)){return;}
	screenWidth  = newW;
	screenHeight = newH;
	SDL_SetWindowSize(gWindow,screenWidth,screenHeight);

	initGL();
}

void closeSDL(){
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
}

int getWindowX(){
	int x,y;
	SDL_GetWindowPosition(gWindow,&x,&y);
	return x;
}
int getWindowY(){
	int x,y;
	SDL_GetWindowPosition(gWindow,&x,&y);
	return y;
}

void swapWindow(){
	SDL_GL_SwapWindow(gWindow);
}

uint getMouseState(int *mx, int *my){
	return SDL_GetMouseState(mx,my);
}

uint getTicks(){
	return SDL_GetTicks();
}

