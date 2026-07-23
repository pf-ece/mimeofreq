# Project Name
TARGET = freq-fiche

# Sources
CPP_SOURCES = freq-fiche.cpp \
			  phaser/Heavy_phaser.cpp \
			  phaser/HeavyContext.cpp \
			  phaser/HvHeavy.cpp
C_SOURCES =	  phaser/HvLightPipe.c \
			  phaser/HvMessage.c \
			  phaser/HvMessagePool.c \
			  phaser/HvMessageQueue.c \
			  phaser/HvSignalDel1.c \
			  phaser/HvSignalLine.c \
			  phaser/HvSignalPhasor.c \
			  phaser/HvSignalRPole.c \
			  phaser/HvSignalVar.c \
			  phaser/HvTable.c \
			  phaser/HvUtils.c

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
