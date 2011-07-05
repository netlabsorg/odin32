ODIN = ../../..

include $(ODIN)/testapp/common.mak

all: except.exe finally.exe longjmp.exe crash.exe # except.s

except.exe_ARGS.1 = 1
except.exe_ARGS.2 = 2
except.exe_ARGS.3 = 3
except.exe_ARGS.4 = 4

finally.exe_ARGS.1 = 1
finally.exe_ARGS.2 = 2
finally.exe_ARGS.3 = 3

longjmp.exe_ARGS.1 = 1
longjmp.exe_ARGS.2 = 2

run: \
     $(call run-with-args,except.exe,1 2 3 4,all) \
     $(call run-with-args,finally.exe,1 2 3,all) \
     $(call run-with-args,longjmp.exe,1 2,all) \
     run-crash.exe

CLEAN += *.s
