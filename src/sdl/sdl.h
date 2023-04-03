#pragma once
#include "../common.h"

extern  int GLVersionMajor;
extern  int GLVersionMinor;
extern uint  frameTimeLast;
extern uint  frameCountSinceLast;
extern float curFPS;
extern uint  worstFrame;

void fpsTick();
void initSDL();
void handleEvents();
void sdlResize(int newW,int newH);
void closeSDL();

int getWindowX();
int getWindowY();

void swapWindow();
uint getMouseState(int *mx, int *my);
uint getTicks();
