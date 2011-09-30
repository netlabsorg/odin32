ODIN = ../../..

include $(ODIN)/testapp/common.mak

CFLAGS += -lcomctl32.lib -luser32.lib -lshell32.lib

StealthDialog.cpp: stdafx.h resource.h

StealthDialog.s: StealthDialog.rc
CLEAN += StealthDialog.s

StealthDialog.exe: StealthDialog.cpp StealthDialog.s

all: StealthDialog.exe

run: run-StealthDialog.exe
