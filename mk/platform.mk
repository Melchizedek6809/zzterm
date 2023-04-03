ifeq ($(ARCH),armv7l)
	LDFLAGS += -march=native
	CFLAGS  += -march=armv7-a -mfloat-abi=hard -mfpu=neon
endif

ifeq ("$(ARCH)","amd64")
	ARCH := x86_64
endif

ifeq ($(OS),Windows_NT)
	include mk/win/win.mk
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		include mk/macos/macos.mk
	endif
	ifeq ($(UNAME_S),Linux)
		include mk/linux/linux.mk
	endif
	ifeq ($(UNAME_S),Haiku)
		include mk/haiku/haiku.mk
	endif
	ifeq ($(UNAME_S),DragonFly)
		include mk/bsd/bsd.mk
	endif
	ifeq ($(UNAME_S),OpenBSD)
		include mk/bsd/bsd.mk
	endif
	ifeq ($(UNAME_S),NetBSD)
		include mk/bsd/bsd.mk
	endif
	ifeq ($(UNAME_S),FreeBSD)
		include mk/bsd/bsd.mk
	endif
endif

ifdef EMSDK
	include mk/wasm/wasm.mk
endif