COMPILER=icc
CFLAGS= -Gm+
DEBUG= -Ti+
OPTIM=

OBJS = vio32Dll.cpp vio32Utils.obj vio32SharedData.obj vio32Client.obj vio32StdFilters.obj

%.obj : %.cpp
	$(COMPILER) -c $(CFLAGS) $(DEBUG) $(OPTIM)  $*.cpp -o $*.obj


all: $(OBJS) vio32.dll vio32.lib vio32VioServ.exe vio32test.exe vio32test2.exe vio32test3.exe test.exe
	echo done

vio32VioServ.exe: vio32VioServ.obj vio32.lib vio32.h
	$(COMPILER) $(CFLAGS) $(DEBUG) $(OPTIM) vio32VioServ.obj vio32.lib -Fe vio32VioServ.exe

vio32test.exe: vio32test.obj vio32.lib  vio32.h
	$(COMPILER) $(CFLAGS) $(DEBUG) $(OPTIM) vio32test.obj vio32.lib -Fe vio32test.exe

vio32test2.exe: vio32test2.obj vio32.lib  vio32.h
	$(COMPILER) $(CFLAGS) $(DEBUG) $(OPTIM) vio32test2.obj vio32.lib -Fe vio32test2.exe

vio32test3.exe: vio32test3.obj vio32.lib  vio32.h
	$(COMPILER) $(CFLAGS) $(DEBUG) $(OPTIM) vio32test3.obj vio32.lib -Fe vio32test3.exe

vio32.dll: $(OBJS) vio32.def vio32.h moncalls.lib MonitorApi.h vio32private.h
	$(COMPILER)  $(CFLAGS)  $(DEBUG) $(OPTIM) -Ge- $(OBJS) DosQuerySysState.lib moncalls.lib vio32.def -Fe vio32.dll

#/B"/NOE"

vio32.lib: vio32.def
	implib vio32.lib vio32.def

test.exe: test.obj vio32.lib  vio32.h vio32Utils.h
	$(COMPILER) $(CFLAGS) $(DEBUG) $(OPTIM) test.obj vio32.lib -Fe test.exe


$(OBJS):    vio32Client.h       \
        vio32Private.h      \
        vio32Utils.h        \
        vio32SharedData.h   \
        vio32.h         \
        vio32Errors.h
