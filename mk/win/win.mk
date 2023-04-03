ZZTERM    := ./zzterm.exe

CC               := cc
CFLAGS           := $(shell pkg-config --cflags sdl2)
WINLIBS          := -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid -lgdi32 -lopengl32 -lshlwapi -lsetupapi
STATICLIBS       := -static-libgcc -static -lmingw32 -lm -lpthread -lopengl32 -lSDL2main -lSDL2 $(WINLIBS)
CLIENT_LIBS      := -mwindows -Wl,-subsystem,windows $(WINLIBS) $(shell pkg-config --cflags sdl2) $(shell pkg-config --libs sdl2) -lopengl32 -static-libgcc -lmingw32 -lpthread
WIN_RELDIR       := releases/win/zzterm-win-$(VERSION_NAME)

.PHONY: release.win
release: release.win

release.win: releases/win/zzterm-win-$(VERSION_NAME).7z
releases/win/zzterm-win-$(VERSION_NAME).7z: $(WIN_RELDIR)/README.txt
releases/win/zzterm-win-$(VERSION_NAME).7z: $(WIN_RELDIR)/zzterm.exe
	cd releases/win/ && 7z a -mx9 zzterm-win-$(VERSION_NAME).7z zzterm-win-$(VERSION_NAME)

$(WIN_RELDIR)/README.txt: common/README
	@mkdir -p $(WIN_RELDIR)
	cp $< $@

$(WIN_RELDIR)/zzterm.exe: $(SOURCES) $(TMP_SOURCES)
$(WIN_RELDIR)/zzterm.exe: releases/win/zzterm.res
	@mkdir -p $(WIN_RELDIR)
	@$(CC) $^ -o $@ $(RELEASE_OPTIMIZATION) $(CFLAGS) $(CSTD) $(CINCLUDES) $(STATICLIBS)
	@strip -x $@
	@echo "$(ANSI_BG_GREEN)" "[CC] " "$(ANSI_RESET)" $@

$(zzterm): releases/win/zzterm.res

releases/win/zzterm.res: mk/win/zzterm.rc mk/win/icon.ico
	@mkdir -p $(WIN_RELDIR)
	@windres $< -O coff -o $@
	@echo "$(ANSI_BG_YELLOW)" "[WIN]" "$(ANSI_RESET)" $@
