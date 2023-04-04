#pragma once
#include "../common.h"

extern int  textInputBufferLen;
extern int  textInputCursorPos;
extern int  textInputMark;
extern char textInputBuffer[65536];
extern bool textInputChanged;

int   textInputActive ();
void  textInputEnter  ();