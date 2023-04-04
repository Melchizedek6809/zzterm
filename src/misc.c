/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "misc.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __MINGW32__
#include <shlobj.h>
#include <windows.h>
#endif

/* Load filename into a fresly allocated buffer which is always zero terminated.
 * The length is returned in len if len is not NULL */
void *loadFile(const char *filename, size_t *len) {
    FILE *fp;
    size_t filelen, readlen, read;
    u8 *buf = NULL;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = malloc(filelen + 1);
    if (buf == NULL) {
        return NULL;
    }

    readlen = 0;
    while (readlen < filelen) {
        read = fread(buf + readlen, 1, filelen - readlen, fp);
        if (read == 0) {
            free(buf);
            return NULL;
        }
        readlen += read;
    }
    fclose(fp);
    buf[filelen] = 0;

    if (len) {
        *len = filelen;
    }
    return buf;
}

/* Save len bytes of buf into filename, overwriting whatever was there before */
void saveFile(const char *filename, const void *buf, size_t len) {
    FILE *fp;
    size_t written, wlen = 0;

    fp = fopen(filename, "wb");
    if (fp == NULL) {
        return;
    }

    while (wlen < len) {
        written = fwrite(buf + wlen, 1, len - wlen, fp);
        if (written == 0) {
            return;
        }
        wlen += written;
    }
    fclose(fp);
}
