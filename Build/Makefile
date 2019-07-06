TARGET = News Channel
SRCDIR = ../Source

SRCS = \
	$(SRCDIR)/main.c \

CFLAGS	= -Os -Wall
CFLAGS += -Wno-unused-value

CORE_OBJS := $(CORE_SRCS)
CORE_OBJS := $(CORE_OBJS:.cpp=.o)
CORE_OBJS := $(CORE_OBJS:.c=.o)
CORE_OBJS := $(CORE_OBJS:.S=.o)

LIBS = -lstdc++ -lpsppower -lpspgu -lpspaudio -lpsprtc -lc -lpng -lz -lg -lm # -lpspfpu -lpspkubridge

EXTRA_TARGETS = EBOOT.PBP #$(CORE_OBJS)

PSP_EBOOT_TITLE = News Channel
PSP_EBOOT_ICON  = icon0.png
PSP_EBOOT_PIC1  = pic1.png
#PSP_EBOOT_ICON1 = ICON1.PMF
#PSP_EBOOT_UNKPNG = PIC0.PNG
#PSP_EBOOT_SND0 = SND0.AT3
#PSP_EBOOT_PSAR =

PSPSDK=$(shell psp-config --pspsdk-path)

PSP_FW_VERSION=500

EXTRA_CLEAN=$(DEP_FILES)

DATA_DIR = ../../Data
BUILDS_DIR = ./Build
BUILDS_PSP_DIR = $(BUILDS_DIR)/PSP
BUILDS_GAME_DIR = $(BUILDS_PSP_DIR)/GAME

VERSION = 1.0
