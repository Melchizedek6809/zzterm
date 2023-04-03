CC               := gcc
STRIP            := strip
MUSL_CC          := musl-gcc

LIBS             := $(shell pkg-config --silence-errors --libs gl || pkg-config --libs opengl) -lm -lpthread -ldl -lSDL2
LIN_REL          := releases/linux-$(ARCH)/zzterm-linux-$(ARCH)

ifneq (, $(shell which mold))
	LDFLAGS += -fuse-ld=mold
endif

.PHONY:  release.linux
release: release.linux

release.linux: releases/linux-$(ARCH)/zzterm-linux-$(ARCH).tar.xz
releases/linux-$(ARCH)/zzterm-linux-$(ARCH).tar.xz: $(LIN_REL)/README
releases/linux-$(ARCH)/zzterm-linux-$(ARCH).tar.xz: $(LIN_REL)/zzterm
	cd releases/linux-$(ARCH)/ && tar -c zzterm-linux-$(ARCH)/ | xz -9 - > zzterm-linux-$(ARCH).tar.xz

$(LIN_REL)/README: mk/README
	@mkdir -p $(LIN_REL)
	cp $< $@

$(LIN_REL)/zzterm: $(SOURCES) $(TMP_SOURCES)
	@mkdir -p $(LIN_REL)
	$(CC) $^ -o $@ $(RELEASE_OPTIMIZATION) $(CFLAGS) $(CSTD) $(CINCLUDES) $(LIBS) $(LDFLAGS)
	$(STRIP) -gxX $@
