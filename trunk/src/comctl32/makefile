# $Id: makefile,v 1.19 2000-02-05 01:53:54 sandervl Exp $
#
# PD-Win32 API
#
#       comctl32.dll makefile
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS = ..\..\tools\bin


!include $(PDWIN32_INCLUDE)/pdwin32.mk


CFLAGS = $(CFLAGS) -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) -I$(PDWIN32_INCLUDE)

TARGET = comctl32

OBJS =  comctl32.obj initterm.obj progress.obj comctl32undoc.obj imagelist.obj \
        comboex.obj animate.obj datetime.obj flatsb.obj hotkey.obj monthcal.obj \
        nativefont.obj pager.obj tab.obj status.obj header.obj updown.obj \
        rebar.obj cache.obj trackbar.obj tooltips.obj toolbar.obj treeview.obj \
        propsheet.obj listview.obj draglist.obj ipaddress.obj resource.obj

all: $(TARGET).dll $(TARGET).lib

resource.asm: rsrc.rc
    $(RC) $(RCFLAGS) -o resource.asm rsrc.rc

resource.obj: resource.asm


$(TARGET).dll: $(OBJS) $(TARGET).def
        $(LD) $(LDFLAGS) -Fm -Fe$@ $(OBJS) $(TARGET).def \
              $(PDWIN32_LIB)/kernel32.lib \
              $(PDWIN32_LIB)/gdi32.lib \
              $(PDWIN32_LIB)/user32.lib  $(PDWIN32_LIB)/odincrt.lib \
              OS2386.LIB  $(RTLLIB_O)
        $(CP) $@ $(PDWIN32_BIN)


$(TARGET).lib: $(TARGET)exp.def
        $(IMPLIB) $(IMPLIBFLAGS) $@ $(TARGET)exp.def
        $(CP) $@ $(PDWIN32_LIB)

$(TARGET)exp.def: $(TARGET).def
        $(IMPDEF) $** $@

comctl32.obj: comctl32.c
initterm.obj: initterm.cpp
progress.obj: progress.c
comctl32undoc.obj: comctl32undoc.c
imagelist.obj: imagelist.c
comboex.obj: comboex.c
animate.obj: animate.c
datetime.obj: datetime.c
flatsb.obj: flatsb.c
hotkey.obj: hotkey.c
ipaddress.obj: ipaddress.c
monthcal.obj: monthcal.c
nativefont.obj: nativefont.c
pager.obj: pager.c
tab.obj: tab.c
status.obj: status.c
header.obj: header.c
updown.obj: updown.c
rebar.obj: rebar.c
cache.obj: cache.c
trackbar.obj: trackbar.c
tooltips.obj: tooltips.c
toolbar.obj: toolbar.c
treeview.obj: treeview.c
propsheet.obj: propsheet.c
listview.obj: listview.c
draglist.obj: draglist.c

clean:
        $(RM) *.obj *.lib *.dll *.map *.pch
        $(RM) $(PDWIN32_BIN)\$(TARGET).dll
        $(RM) $(PDWIN32_LIB)\$(TARGET).lib

