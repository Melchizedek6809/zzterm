/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "texture.h"
#include "gl.h"
#include "../misc.h"
#include "../../vendor/lodepng.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

uint boundTexture = 0;
uint textureCount = 0;
texture textureList[16];

texture *tFont;

extern uint font16_png_len;
extern u8   font16_png_data[];

extern uint font8_png_len;
extern u8   font8_png_data[];

void textureLoadSurface(texture *t, uint w, uint h, const void *data, bool linear){
	t->w = w;
	t->h = h;
	if(t->ID == 0){glGenTextures(1, &t->ID);}
	glBindTexture(GL_TEXTURE_2D, t->ID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,linear ? GL_LINEAR : GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void textureLoadTermBuffer(texture *t, uint w, uint h, const void *data){
	t->w = w;
	t->h = h;
	if(t->ID == 0){glGenTextures(1, &t->ID);}
	glBindTexture(GL_TEXTURE_2D, t->ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, w, h, 0, GL_RG, GL_UNSIGNED_BYTE, data);
}

static void textureLoad(texture *t, const u8 *data, const uint dataLen){
	u8 *pixels = NULL;
	if(lodepng_decode32(&pixels, &t->w, &t->h, data, dataLen)){
		fprintf(stderr,"Error decoding PNG\n");
		exit(4);
	}
	if(t->ID == 0){glGenTextures(1, &t->ID);}
	glBindTexture(GL_TEXTURE_2D, t->ID);
	gfxObjectLabel(GL_TEXTURE, t->ID, "%s", t->filename);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	free(pixels);
}

static void textureGetTile(texture *t, u8 *pixels, u8 *pbuf, int iw, int tx, int ty){
	for(int y = t->h-1; y >= 0; y--){
		u8 *p = &pixels[((ty*t->h)*iw*4)+(tx*t->w*4)+(y*iw*4)];
		memcpy(pbuf,p,t->w*4);
		pbuf += t->w*4;
	}
}

static void textureLoadArray(texture *t, const u8 *data, const uint dataLen){
	u8 *pixels = NULL;
	u8 *pbuf = NULL;
	uint iw,ih,itw,ith;
	itw = ith = sqrt(t->d);
	if(lodepng_decode32(&pixels, &iw, &ih, data, dataLen)){
		fprintf(stderr,"Error decoding PNG\n");
		exit(4);
	}
	t->w = iw/itw;
	t->h = ih/ith;

	u8 *p = pbuf = malloc(itw*t->w*ith*t->h*4);

	for(uint ty=0;ty<itw;ty++){
	for(uint tx=0;tx<ith;tx++){
		textureGetTile(t,pixels,p,iw,tx,ty);
		p += t->w * t->h * 4;
	}
	}
	if(t->ID == 0){glGenTextures(1, &t->ID);}
	glBindTexture(GL_TEXTURE_2D_ARRAY, t->ID);
	gfxObjectLabel(GL_TEXTURE, t->ID, "%s array", t->filename);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, t->w, t->h, t->d, 0, GL_RGBA, GL_UNSIGNED_BYTE, pbuf);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	free(pixels);
	free(pbuf);
}

texture *textureNewRaw(){
	texture *tex = &textureList[textureCount++];
	return tex;
}

texture *textureNew(const u8 *data, uint dataLen,const char *filename){
	texture *tex = &textureList[textureCount++];
	tex->filename = filename;
	tex->ID = 0;
	tex->d  = 0;
	textureLoad(tex,data,dataLen);
	return tex;
}

texture *textureNewArray(const u8 *data, uint dataLen,const char *filename, int d){
	texture *tex = &textureList[textureCount++];
	tex->filename = filename;
	tex->ID = 0;
	tex->d  = d;
	textureLoadArray(tex,data,dataLen);
	return tex;
}

texture *textureNewSurface(uint w, uint h, const void *data){
	texture *tex = &textureList[textureCount++];
	tex->ID = 0;
	tex->d  = 0;
	textureLoadSurface(tex,w,h,data,false);
	return tex;
}

void textureFree(){
	for(uint i=0;i<textureCount;i++){
		if(textureList[i].ID == 0){continue;}
		glDeleteTextures(1, &textureList[i].ID);
	}
}

void textureBindID(uint ID){
	if(boundTexture == ID){return;}
	glBindTexture(GL_TEXTURE_2D, ID);
	boundTexture = ID;
}

void textureBind(const texture *tex){
	if(boundTexture == tex->ID){return;}
	if(tex->d > 1){
		glBindTexture(GL_TEXTURE_2D_ARRAY, tex->ID);
	}else{
		glBindTexture(GL_TEXTURE_2D, tex->ID);
	}
	boundTexture = tex->ID;
}

void textureInit(){
	tFont = textureNew(font16_png_data, font16_png_len, "gfx/font16.png");
}

void reloadTexture(texture *tex){
	size_t dataLen;
	void *data;
	if((tex->filename == NULL) || (tex->filename[0] == 0)){return;}
	data = loadFile(tex->filename,&dataLen);
	if(data != NULL){
		fprintf(stderr,"Loading %s\n",tex->filename);
		textureLoad(tex,data,dataLen);
		free(data);
	}
}

void textureReload(){
	fprintf(stderr,"Reloading all textures\n");
	for(uint i=0;i<textureCount;i++){
		reloadTexture(&textureList[i]);
	}
}
