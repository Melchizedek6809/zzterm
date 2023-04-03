#pragma once
extern char *ansiRS;
extern char *ansiFG[16];
extern  int msPerTick;

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PI    (3.1415926535897932384626433832795f)
#define PI180 (3.1415926535897932384626433832795f / 180.f)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MINMAX(a, b, v) (MAX(a,MIN(b,v)))
#define countof(x) (sizeof(x)/sizeof(*x))

typedef unsigned int     uint;
typedef unsigned short ushort;
typedef unsigned char   uchar;

typedef uint64_t          u64;
typedef uint32_t          u32;
typedef uint16_t          u16;
typedef uint8_t            u8;

typedef  int64_t          i64;
typedef  int32_t          i32;
typedef  int16_t          i16;
typedef  int8_t            i8;

#pragma pack(push, 1)
typedef struct vertex2D {
	i16 x,y;
	i16 u,v;
} vertex2D;
#pragma pack(pop)

typedef struct {
	uint ID,w,h,d;
	const char *filename;
} texture;

typedef struct {
	uint pID;
	uint vsID,fsID;
	const char *vss,*fss;
	const char *defines;
	const char *name;
	uint attrMask;
	int lMVP,lAlpha,lColor,lTransform,lSizeMul;
} shader;
