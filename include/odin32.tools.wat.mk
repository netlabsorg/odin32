# $Id: odin32.tools.wat.mk,v 1.2 2000-12-09 16:04:23 phaller Exp $

#
# Odin32 API
#
#       Watcom C/C++ 11.x tools and interference rules.
#


#
# Watcom Specific tools
#
MAPSYM   = ..\..\$(ODIN32_TOOLS)\mapsymWat.cmd
KDEF2WAT = $(ODIN32_TOOLS)\kDef2Wat.exe


#
# Make command definition.
#
MAKE_CMDLINE= wmake -u
!if "$(%DEBUG)" != ""
DEBUG = 1
!endif
!ifdef DEBUG
MAKE_CMDLINE= $(MAKE_CMDLINE) DEBUG=1
!endif
!ifdef PROFILE
MAKE_CMDLINE= $(MAKE_CMDLINE) DEBUG=1 PROFILE=1
!endif
!if "$(%NODEBUGINFO)" != ""
NODEBUGINFO = 1
!endif
!ifdef NODEBUGINFO
MAKE_CMDLINE= $(MAKE_CMDLINE) NODEBUGINFO=1
!endif
!if "$(%NODEP)" != ""
NODEP = 1
!endif
!ifdef NODEP
MAKE_CMDLINE= $(MAKE_CMDLINE) NODEP=1
!endif

MAKE_CMD    = $(MAKE_CMDLINE) # -f makefile.os2 # What is this?


#
# Interference rules.
#
.ERASE
.EXTENSIONS:
.EXTENSIONS: .lst .res .obj .orc_asm .orc .cpp .c .rc .asm

.obj : $(OBJDIR)
.res : $(OBJDIR)

.c.obj:   # .AUTODEPEND
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(OBJDIR)\$^&.obj $^&.c

.cpp.obj: # .AUTODEPEND
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(OBJDIR)\$^&.obj $^&.cpp

.asm.obj:
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $^&.asm

.orc.orc_asm:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $^&.orc_asm $^&.orc

.orc_asm.obj:
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $^&.orc_asm

.orc.obj:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$^&.asm $^&.orc
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(OBJDIR)\$^&.asm

.rc.res:
    $(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $^&.rc

