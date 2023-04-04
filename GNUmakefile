ifneq (, $(shell which dash))
	SHELL   := $(shell which dash)
endif

ZZTERM               := ./zzterm
ASSET                := tools/assets
CC                   := cc
CFLAGS               := -g -D_GNU_SOURCE $(shell pkg-config --cflags sdl2)
LDFLAGS              :=
LIBS                 := $(shell pkg-config --libs sdl2)
CSTD                 := -std=c99
OPTIMIZATION         := -O2 -fno-lto -ffast-math -freciprocal-math -fno-math-errno
WARNINGS             := -Wall -Werror -Wextra -Wshadow -Wcast-align -Wno-missing-braces

RELEASE_OPTIMIZATION := -O3 -flto -ffast-math -freciprocal-math -fno-math-errno
ARCH                 := $(shell uname -m)
GFX_ASSETS           := $(shell find gfx -type f -name '*')
SHADER_ASSETS        := $(shell find src/shader -type f -name '*.glsl')
SOURCES              := $(shell find src -type f -name '*.c') $(shell find vendor -type f -name '*.c') src/tmp/assets.c
OBJS                 := ${SOURCES:.c=.o}
DEPS                 := ${SOURCES:.c=.d}

all: $(ZZTERM)
.PHONY: all release

include mk/disable_implicit_rules.mk
include mk/ansi_colors.mk
include mk/platform.mk

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: clean
clean:
	@rm -f gmon.out tools/assets callgrind.out.* vgcore.* mk/win/zzterm.res
	@rm -f $(shell find src -type f \( -name '*.o' -o -name '*.d' \))
	@rm -f zzterm zzterm.exe tools/assets tools/assets.exe
	@rm -rf src/tmp releases
	@echo "$(ANSI_BG_RED)" "[CLEAN]" "$(ANSI_RESET)" "zzterm"

.PHONY: sanitize
sanitize: CFLAGS += -fsanitize=address
sanitize: all

.PHONY: archive
archive:
	git archive --format=tar --prefix=zzterm-HEAD.tar.gz/ HEAD | gzip > zzterm-HEAD.tar.gz

$(ZZTERM): $(OBJS)
	@$(CC) $^ -o $@ $(OPTIMIZATION) $(CFLAGS) $(CINCLUDES) $(LIBS) $(CSTD) $(LDFLAGS)
	@echo "$(ANSI_BG_GREEN)" "[LD] " "$(ANSI_RESET)" $@

%.o: %.c
	@$(CC) $(OPTIMIZATION) $(WARNINGS) $(CSTD) $(CFLAGS) $(CINCLUDES) -c $< -o $@ -MMD > ${<:.c=.d}
	@echo "$(ANSI_GREEN)" "[CC] " "$(ANSI_RESET)" $@

$(ASSET): tools/assets.c
	@$(CC) $(OPTIMIZATION) $(CSTD) $(CFLAGS) $^ -o $@
	@echo "$(ANSI_BG_GREY)" "[CC] " "$(ANSI_RESET)" $@

src/tmp/assets.c: $(ASSET) $(GFX_ASSETS) $(SHADER_ASSETS)
	@mkdir -p src/tmp/
	@$(ASSET) src/tmp/assets $(GFX_ASSETS) $(SHADER_ASSETS)
	@echo "$(ANSI_GREY)" "[ST] " "$(ANSI_RESET)" $@
src/tmp/assets.h: src/tmp/assets.c
	@true

.PHONY: optimizegfx
optimizegfx: all
	tools/optimizegfx $(GFX_ASSETS)

.PHONY: run
run: all
	@./zzterm
