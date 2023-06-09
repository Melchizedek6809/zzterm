/* ZZTerm - Copyright (C) 2023 - Benjamin Vincent Schulenburg
 * Licensed under the MIT License, see the full text in /LICENSE
 */
#include "main.h"

#include "gfx/gfx.h"
#include "gfx/shader.h"
#include "gfx/texture.h"
#include "sdl/sdl.h"

#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

bool quit = false;
u64 ticks = 0;
termBuffer *mainTerm = NULL;

void signalQuit(int signo) {
    (void)signo;
    if (quit) {
        exit(0);
    }
    quit = true;
}

void initSignals() {
#ifdef __MINGW32__
    signal(SIGTERM, signalQuit);
    signal(SIGABRT, signalQuit);
#else
    signal(SIGPIPE, SIG_IGN);
    signal(SIGKILL, signalQuit);
    signal(SIGSTOP, signalQuit);
    signal(SIGHUP, signalQuit);
#endif
    signal(SIGINT, signalQuit);
}

void mainloop() {
    ticks++;
    renderFrame();
    handleEvents();
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    initSignals();

    initSDL();
    shaderInit();
    textureInit();

    mainTerm = termBufferNew(80, 50);

    while (!quit) {
        mainloop();
    }

    textureFree();
    shaderFree();
    closeSDL();

    return 0;
}
