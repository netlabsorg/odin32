# $Id: odin32.tools.watn.mk,v 1.2 2003-02-28 10:28:12 sandervl Exp $

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
!ifndef CUSTOMBUILD
MAKE_CMD    = nmake /nologo
!else
MAKE_CMD    = nmake /nologo CUSTOMBUILD=1
!endif
!ifdef DEBUG
! ifndef PROFILE
MAKE_CMD    = $(MAKE_CMD) DEBUG=1
! else
MAKE_CMD    = $(MAKE_CMD) DEBUG=1 PROFILE=1
! endif
!else
! ifdef PROFILE
MAKE_CMD    = $(MAKE_CMD) PROFILE=1
! endif
!endif
!ifdef NODEBUGINFO
MAKE_CMD    = $(MAKE_CMD) NODEBUGINFO=1
!endif
!ifdef NODEP
MAKE_CMD    = $(MAKE_CMD) NODEP=1
!endif


#
# Interference rules.
#
.SUFFIXES: .orc_asm .orc .pre-c .pre-cpp

!ifdef MULTIJOBS
.SILENT :
!endif

_SRC = $(MAKEDIR)\$(<F)
_DST = $(OBJDIR)\$(@F)

.c{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -fo$(_DST) $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(_DST) $(_SRC)
!endif

.c.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -fo$(_DST) $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(_DST) $(_SRC)
!endif

.c.pre-c:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -p -pl -pc -fo$@  $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -p -pl -pc -fo$@  $(_SRC)
!endif


.cpp{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -fo$(_DST) $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(_DST) $(_SRC)
!endif

.cpp.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -fo$(_DST) $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -fo$(_DST) $(_SRC)
!endif

.cpp.pre-cpp:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -p -pl -pc -fo$@  $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -p -pl -pc -fo$@  $(_SRC)
!endif


.asm{$(OBJDIR)}.obj:
    $(CMDQD_SUB) $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(_SRC)

.orc{$(OBJDIR)}.orc_asm:
    $(CMDQD_SUB) $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $(_SRC)

.orc{$(OBJDIR)}.obj:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $(_SRC)
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(OBJDIR)\$(@B).orc_asm

.rc{$(OBJDIR)}.res:
    $(CMDQD_SUB) $(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $(_SRC) $(_DST)
