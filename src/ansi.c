/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "ansi.h"

#include <ctype.h>
#include <stdlib.h>

void parseAnsiCommand(int command, int *fgc, int *bgc){
	switch(command){
	case 0:
		*fgc = 15;
		*bgc = 0;
		break;
	case 1:
		*fgc |= 8;
		break;
	case 11:
		*fgc &= 7;
		break;
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
		*fgc = (*fgc & 0x8) | ((command-30) & 0x7);
		break;
	case 39:
		*fgc = (*fgc & 0x8) | 7;
		break;

	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
		*bgc = (*bgc & 0x8) | ((command-40) & 0x7);
		break;
	case 49:
		*bgc = (*bgc & 0x8) | 7;
		break;
	}
}

int parseAnsiCode(const char *start, int *fgc, int *bgc){
	if(start[1] != '['){return 1;}
	const char *cur = &start[2];
	const char *end = cur;
	while(*end != 'm'){
		for(;*end && isdigit((u8)*end);end++){}
		if(*end == 0){break;}
		if((*end == 'm') || (*end == ';')){
			const int command = atoi(cur);
			parseAnsiCommand(command,fgc,bgc);
			if(*end++ == 'm'){break;}
			cur = end;
			continue;
		}
		end++;
	}
	const int ret = end - start;
	return ret;
}