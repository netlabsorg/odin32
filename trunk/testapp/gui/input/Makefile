ODIN = ../../..

include $(ODIN)/testapp/common.mak

CFLAGS += -lcomctl32.lib -luser32.lib

input_a.exe: input.c

input_w.exe: input.c
CFLAGS.input_w.exe = -DUNICODE 

all: input_a.exe input_w.exe

run:
	@echo Use run_a or run_w!
	
run_a: run-input_a.exe
run_w: run-input_w.exe
