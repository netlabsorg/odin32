ODIN = D:\Coding\odin32

CFLAGS += -D__WIN32OS2__ -D__i386__ -DSTRICT -D_POSIX_SOURCE \
          -D_POSIX_C_SOURCE=200112 -D_EMX_SOURCE -D_XOPEN_SOURCE=600 \
          -D_SVID_SOURCE
CFLAGS += -I$(ODIN)/include -I$(ODIN)/include/win

CFLAGS += -L$(ODIN)/lib/Debug -liphlpapi.lib -lkernel32.lib -lwsock32.lib

all: GetAdaptersInfo.exe GetIfTable.exe GetIpAddrTable.exe

run: run-GetAdaptersInfo.exe run-GetIfTable.exe run-GetIpAddrTable.exe

clean:
	-del *.exe *.log

run-%: %
	cmd /c "set BEGINLIBPATH=$(ODIN)/bin/Debug;$(ODIN)/bin;%BEGINLIBPATH% && $<"

%.exe: %.c
	gcc $< -Zomf -o $@ $(CFLAGS)
