/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "textInput.h"
#include "sdl.h"

#include <stdio.h>
#include <string.h>
#include <SDL.h>

int  textInputMark      = -1;
int  textInputBufferLen = 0;
int  textInputCursorPos = 0;
bool textInputStarted   = true;
char textInputBuffer[65536];
bool textInputChanged = false;

int keyboardCmdKey(const SDL_Event *e){
	#ifdef __APPLE__
	return e->key.keysym.mod & KMOD_GUI;
	#elif __HAIKU__
	return e->key.keysym.mod & KMOD_ALT;
	#else
	return e->key.keysym.mod & KMOD_CTRL;
	#endif
}

int textInputActive(){
	return 1;
}

void textInputEnter(){
	if(!textInputActive()){return;}
	printf("Input: '%s'\n", textInputBuffer);
	textInputCursorPos = textInputBufferLen = 0;
	textInputBuffer[textInputBufferLen] = 0;
	textInputChanged = true;
}

void textInputDelSelection(){
	if(!textInputActive()){return;}
	if(textInputMark < 0){return;}
	int sMin = MIN(textInputMark,textInputCursorPos);
	int sMax = MAX(textInputMark,textInputCursorPos);
	int len = sMax - sMin;
	if(len <= 0){return;}

	memmove(&textInputBuffer[sMin],&textInputBuffer[sMax],textInputBufferLen - sMax);
	textInputBufferLen -= len;
	textInputBuffer[textInputBufferLen] = 0;
	textInputCursorPos = sMin;
	textInputMark = -1;
	textInputChanged = true;
}

void textInputBackspace(int moveForward){
	if(!textInputActive()){return;}
	if(textInputMark >= 0){return textInputDelSelection();}
	if((textInputCursorPos + moveForward) == 0){return;}
	if((textInputCursorPos + moveForward) > textInputBufferLen){return;}
	textInputCursorPos += moveForward;
	for(int i=textInputCursorPos-1;i<textInputBufferLen-1;i++){
		textInputBuffer[i] = textInputBuffer[i+1];
	}
	--textInputCursorPos;
	--textInputBufferLen;
	textInputBuffer[textInputBufferLen] = 0;
	textInputChanged = true;
}

void textInputAppend(const char *s){
	int slen = strnlen(s,256);
	if(!textInputActive()){return;}
	if(textInputMark >= 0){return textInputDelSelection();}
	if((slen + textInputBufferLen) > 255){slen = 255-textInputBufferLen;}
	if(textInputCursorPos != textInputBufferLen){
		for(int i=textInputBufferLen-1;i>=textInputCursorPos;i--){
			textInputBuffer[i+slen] = textInputBuffer[i];
		}
	}
	strncpy(textInputBuffer+textInputCursorPos,s,slen);
	textInputCursorPos += slen;
	textInputBufferLen += slen;
	if(textInputCursorPos >= 255){ textInputCursorPos = 255; }
	if(textInputBufferLen >= 255){ textInputBufferLen = 255; }
	textInputBuffer[textInputBufferLen] = 0;
	textInputChanged = true;
}

void textInputCopy(){
	if(!textInputActive()){return;}
	int sMin = MIN(textInputMark,textInputCursorPos);
	int sMax = MAX(textInputMark,textInputCursorPos);
	int len  = sMax - sMin;
	if(len <= 0){return;}
	char *buf = malloc(len+1);
	memcpy(buf,&textInputBuffer[sMin],len);
	buf[len]=0;
	SDL_SetClipboardText(buf);
	free(buf);
}

void textInputCut(){
	textInputCopy();
	textInputBackspace(0);
}

void textInputPaste(){
	if(!textInputActive()){return;}
	if(!SDL_HasClipboardText()){return;}
	char *text = SDL_GetClipboardText();
	if(text == NULL){return;}
	textInputAppend(text);
	SDL_free(text);
}

void textInputHome(){
	char *c = &textInputBuffer[0];
	for(; isspace((u8)*c); c++){}
	const int indentPos = c - textInputBuffer;
	if(textInputCursorPos <= indentPos){
		textInputCursorPos = 0;
	}else{
		textInputCursorPos = indentPos;
	}
}

void textInputEnd(){
	char *c = &textInputBuffer[textInputBufferLen-1];
	for(; isspace((u8)*c); c--){}
	const int indentPos = c - textInputBuffer + 1;
	if(textInputCursorPos >= indentPos){
		textInputCursorPos = textInputBufferLen;;
	}else{
		textInputCursorPos = indentPos;
	}
}

void textInputCheckMark(const SDL_Event *e){
	if(e->key.keysym.mod & KMOD_SHIFT){
		if(textInputMark < 0){
			textInputMark = textInputCursorPos;
		}
	}else{
		textInputMark = -1;
	}
}

bool textInputHandler(const SDL_Event *e){
	if(!textInputActive()){return false;}

	switch(e->type){
	case SDL_TEXTINPUT:
		textInputAppend(e->text.text);
		break;
	case SDL_TEXTEDITING:
		//fprintf(stderr,"composition: %s %i %i\n",e->edit.text, e->edit.start, e->edit.length);
		break;
	case SDL_KEYUP:
		switch(e->key.keysym.sym){
		case SDLK_RETURN:
		case SDLK_KP_ENTER:
			textInputEnter();
			return true;
		}
		break;
	case SDL_KEYDOWN:
		switch(e->key.keysym.sym){
		case SDLK_BACKSPACE:
			textInputBackspace(0);
			return true;
		case SDLK_DELETE:
			textInputBackspace(1);
			return true;
		case SDLK_LEFT:
			textInputCheckMark(e);
			if(keyboardCmdKey(e)){
				textInputHome();
			}else if(textInputCursorPos > 0){
				--textInputCursorPos;
			}
			return true;
		case SDLK_RIGHT:
			textInputCheckMark(e);
			if(keyboardCmdKey(e)){
				textInputEnd();
			}else if(textInputCursorPos < textInputBufferLen){
				++textInputCursorPos;
			}
			return true;
		case SDLK_HOME:
			textInputCheckMark(e);
			textInputHome();
			return true;
		case SDLK_END:
			textInputCheckMark(e);
			textInputEnd();
			return true;
		case SDLK_INSERT:
			textInputPaste();
			return true;
		case SDLK_a:
			if(keyboardCmdKey(e)){
				textInputCursorPos = textInputBufferLen;
				textInputMark = 0;
				return true;
			}
			break;
		case SDLK_c:
			if(keyboardCmdKey(e)){
				textInputCopy();
				return true;
			}
			break;
		case SDLK_x:
			if(keyboardCmdKey(e)){
				textInputCut();
				return true;
			}
			break;
		case SDLK_v:
			if(keyboardCmdKey(e)){
				textInputPaste();
				return true;
			}
			break;
		}
	}
	return false;
}
