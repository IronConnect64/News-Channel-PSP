TARGET = News Channel
SRCDIR = ../Source

SRCS = \
	$(SRCDIR)/main.c \ #just an example

CFLAGS	= -O2 -G0 -Wall
CFLAGS += -Wno-unused-value

CORE_OBJS := $(CORE_SRCS)
CORE_OBJS := $(CORE_OBJS:.cpp=.o)
CORE_OBJS := $(CORE_OBJS:.c=.o)
CORE_OBJS := $(CORE_OBJS:.S=.o)

#OBJS = $(SRCS)
#OBJS := $(OBJS:.cpp=.o)
#OBJS := $(OBJS:.c=.o)
#OBJS := $(OBJS:.S=.o)
#OBJS += news.a

#DEP_FILES = $(SRCS) $(CORE_SRCS)
#DEP_FILES := $(DEP_FILES:.cpp=.d)
#DEP_FILES := $(DEP_FILES:.c=.d)
#DEP_FILES := $(DEP_FILES:.S=.d)

LIBS = -lstdc++ -lpsppower -lpspgu -lpspaudio -lpsprtc -lc -lpng -lz -lg -lm -lpspfpu -lpspkubridge

EXTRA_TARGETS = EBOOT.PBP #$(CORE_OBJS)

PSP_EBOOT_TITLE = News Channel
PSP_EBOOT_ICON  = icon0.png
PSP_EBOOT_PIC1  = pic1.png
#PSP_EBOOT_ICON1 = ICON1.PMF
#PSP_EBOOT_UNKPNG = PIC0.PNG
#PSP_EBOOT_SND0 = SND0.AT3
#PSP_EBOOT_PSAR =

PSPSDK=$(shell psp-config --pspsdk-path)
#USE_PSPSDK_LIBC=1
PSP_FW_VERSION=500
#BUILD_PRX = 1
#PSP_LARGE_MEMORY = 1

EXTRA_CLEAN=$(DEP_FILES)


DATA_DIR = ../../Data
BUILDS_DIR = ./Builds
BUILDS_PSP_DIR = $(BUILDS_DIR)/PSP
BUILDS_GAME_DIR = $(BUILDS_PSP_DIR)/GAME
BUILDS_DX_DIR = $(BUILDS_GAME_DIR)/News


VERSION = 1.0

ifeq ($(VERSION),)
	#Windows
	EXTRA_TARGETS := svn $(EXTRA_TARGETS)
	include $(PSPSDK)/lib/build.mak

svn:
	@echo svnversion not found, trying SubWCRev
	@-SubWCRev . $(SRCDIR)/svnversion.txt $(SRCDIR)/svnversion.h
else
	#Unix
	CFLAGS += -DSVNVERSION=\"$(VERSION)\"
	PSP_EBOOT_TITLE += $(VERSION)

	include $(PSPSDK)/lib/build.mak
endif

psplink: $(PSP_EBOOT) $(TARGET).elf
	prxtool -y $(TARGET).elf > $(BUILDS_DX_DIR)/$(TARGET).sym

#extra PRXes are from Daedalus, might be useful if we ever need to use them eg. KernelButtons
allclean: clean
	$(MAKE) -C $(SRCDIR)/SysPSP/ExceptionHandler/prx clean
	$(MAKE) -C $(SRCDIR)/SysPSP/MediaEnginePRX clean
	$(MAKE) -C $(SRCDIR)/SysPSP/DveMgr clean
	$(MAKE) -C $(SRCDIR)/SysPSP/KernelButtonsPrx clean
	rm -rf /Builds/ 
