CC := gcc
CFLAGS  += -gdwarf-2

CLIENT_LIBS := -lnetwork -lm $(shell sdl2-config --libs) -lGL -lpthread -lSDL2
LIBS        := -lnetwork -lm
