# $Id: makefile.wat,v 1.4 2000-12-02 23:58:01 bird Exp $

#===================================================================
#
#   Setup the environment properly
#
#===================================================================
ODIN32_INCLUDE = ..\..\include
ODIN32_LIB = ..\..\lib
ODIN32_BIN = ..\..\bin
INC     = $(%WATCOM)\h;f:\toolkit\h;.;..\..\include;..\..\include\win
DEF     =
!ifndef %RELEASE
#DBGFLG   = -dDEBUG=1
DBGFLG =
!endif

#===================================================================
#
#   Auto-dependency information
#
#===================================================================
.ERASE
.SUFFIXES:
.SUFFIXES: .lst .obj .cpp .asm

CFLAGS  = -w4 -e25 -otexan -d2 -bm -ez -mf -i$(INC) -5r -bt=os2 -zq -D__WIN32OS2__ -D__PE2LX__
CC      = WPP386 $(CFLAGS) $(DEF)

AFLAGS  = -q
ASM     = WASM $(AFLAGS)

DFLAGS  = -l -s
DIS     = WDISASM $(DFLAGS)

#LFLAGS  = sys os2v2 pm op m d all op st=16384 op c op maxe=25 op q op symf
LFLAGS   = sys os2v2 op m d all op st=131072 op maxe=25 op q op symf
QFLAGS  = system os2 dll option quiet, map, align=512
LINK    = WLINK $(LFLAGS)

BFLAGS  = -s -t -c -b -q -n
LIB     = WLIB $(BFLAGS)

.obj.lst:
        $(DIS) $*

.cpp.obj: .AUTODEPEND
        $(CC) $*.cpp

.asm.obj: .AUTODEPEND
        $(ASM) $*.asm

#===================================================================
#
#   List of source files
#
#===================================================================
FILE0   = pefile.obj pe.obj
FILE2   = lx.obj icon.obj menu.obj dialog.obj misc.obj
FILE3   = rcdata.obj icongrp.obj
FILE4   = strings.obj bitmap.obj
FILE5   = accelerator.obj
FILE6   = cursor.obj cursorgrp.obj
FILE7   = $(ODIN32_LIB)\LIBCONV.LIB
FILE8   =
FILE9   =
FILES   = $(FILE0) $(FILE1) $(FILE2) $(FILE3) $(FILE4) $(FILE5) $(FILE6) $(FILE7) $(FILE8) $(FILE9)
TARGET  = PE2LX



#===================================================================
#
#   Specific dependencies
#
#===================================================================
all: $(TARGET).EXE

$(TARGET).lnk: makefile
    @%write $^@ name $(TARGET).EXE
    @for %f in ($(FILES)) do @%append $^@ file %f

$(TARGET).exe: $(TARGET).lnk $(FILES)
    $(LINK) @$(TARGET).lnk
    copy $(TARGET).exe ..\..\bin

#    rc -i . -i d:\WATCOM\h -i d:\WATCOM\h\os2 gusmod.res ultimod.exe
