# $Id: makefile,v 1.18 2000-01-18 22:27:55 sandervl Exp $

#
# PD-Win32 API
#
#       shell32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)


RC = $(PDWIN32_TOOLS)\wrc
RCFLAGS = -s -I. -I$(CPPMAIN)\include -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\win

TARGET = shell32

OBJS =  initterm.obj \
        brsfolder.obj classes.obj contmenu.obj dataobject.obj \
        enumidlist.obj folders.obj iconcache.obj memorystream.obj \
        pidl.obj regstream.obj shell32_main.obj \
        shellguid.obj shelllink.obj shlmenu.obj shellole.obj \
        shellord.obj shellpath.obj shv_bg_cmenu.obj \
        shlfolder.obj shlview.obj shell.obj unknown.obj misc.obj \
        she.obj sh.obj resource.obj shpolicy.obj

all: $(TARGET).dll $(TARGET).lib


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/pmwinx.lib $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/user32.lib $(PDWIN32_LIB)/advapi32.lib \
              $(PDWIN32_LIB)/gdi32.lib $(PDWIN32_LIB)/ole32.lib  \
              $(PDWIN32_LIB)/version.lib $(PDWIN32_LIB)/comctl32.lib   \
              $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

resource.asm: shres.rc shell32_en.rc
    $(RC) $(RCFLAGS) -o resource.asm shres.rc

resource.obj: resource.asm


initterm.obj:     initterm.cpp
brsfolder.obj:    brsfolder.cpp
classes.obj:      classes.cpp
contmenu.obj:     contmenu.cpp
dataobject.obj:   dataobject.cpp
enumidlist.obj:   enumidlist.cpp
folders.obj:      folders.cpp
iconcache.obj:    iconcache.cpp
memorystream.obj: memorystream.cpp
pidl.obj:         pidl.cpp
regstream.obj:    regstream.cpp
shell32_main.obj: shell32_main.cpp
shell.obj:        shell.cpp
shellguid.obj:    shellguid.cpp
shelllink.obj:    shelllink.cpp
shlmenu.obj:      shlmenu.cpp
shellole.obj:     shellole.cpp
shellord.obj:     shellord.cpp
shellpath.obj:    shellpath.cpp
shlfolder.obj:    shlfolder.cpp
shlview.obj:      shlview.cpp
shv_bg_cmenu.obj: shv_bg_cmenu.cpp
unknown.obj:      unknown.cpp
misc.obj:         misc.cpp
she.obj:          she.cpp
sh.obj:           sh.cpp
shpolicy.obj:     shpolicy.cpp shpolicy.h

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib



