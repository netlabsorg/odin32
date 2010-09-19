ODIN = ../../..

include $(ODIN)/testapp/common.mak

all: except.exe finally.exe # except.s

finally.exe_ARGS.1 = 1
finally.exe_ARGS.2 = 2
finally.exe_ARGS.3 = 3

run: run-except.exe $(call run-with-args,finally.exe,1 2 3,all)

CLEAN += *.s
