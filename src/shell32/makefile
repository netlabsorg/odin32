# $Id: makefile,v 1.21 2000-02-09 23:47:02 bird Exp $

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


lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib

$(PDWIN32_LIB)\$(TARGET).lib: $(TARGET).lib
    $(CP) $** $@

$(TARGET).lib: $(TARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
    $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
    $(IMPDEF) $** $@


resource.asm: shres.rc shell32_en.rc
    $(RC) $(RCFLAGS) -o resource.asm shres.rc


dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h

!ifndef NODEP
!include .depend
!endif


clean:
    $(RM) *.obj *.lib *.dll *.map *.pch \
        $(PDWIN32_BIN)\$(TARGET).dll $(PDWIN32_LIB)\$(TARGET).lib \
        resource.asm $(TARGET)exp.def

