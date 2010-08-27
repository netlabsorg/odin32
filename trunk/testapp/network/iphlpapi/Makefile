ODIN = ../../..

include $(ODIN)/testapp/common.mak

CFLAGS += -liphlpapi.lib -lwsock32.lib

all: GetAdaptersInfo.exe GetIfTable.exe GetIpAddrTable.exe

run: run-GetAdaptersInfo.exe run-GetIfTable.exe run-GetIpAddrTable.exe
