# $Id: odin32.tools.wat.mk,v 1.6 2001-09-30 09:46:17 bird Exp $

#
# Odin32 API
#
#       Watcom C/C++ 11.x tools and inference rules.
#


#
# Watcom Specific tools
#
MAPSYM   = $(ODIN32_TOOLS)\mapsymWat.cmd
KDEF2WAT = $(ODIN32_TOOLS)\kDef2Wat.exe


#
# Make command definition.
#
MAKE_CMDLINE = wmake -u
!ifdef CUSTOMBUILD
MAKE_CMDLINE += CUSTOMBUILD=1
!endif
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

.c.obj:   .AUTODEPEND
!ifdef USERAPP
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -fo$(OBJDIR)\$^&.obj $^&.c
!else
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(OBJDIR)\$^&.obj $^&.c
!endif

.cpp.obj: .AUTODEPEND
!ifdef USERAPP
    $(CC) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -fo$(OBJDIR)\$^&.obj $^&.c
!else
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(OBJDIR)\$^&.obj $^&.cpp
!endif

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

