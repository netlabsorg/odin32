# $Id: odin32.tools.vac3.mk,v 1.11 2006-03-25 14:02:54 sao2l02 Exp $

#
# Odin32 API
#
#       IBM Visual Age for C++ v3.08 tools and inference rules.
#


#
# VAC3 Specific tools
#
MAPSYM      = mapsym


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
MAKE_CMD    = $(MAKE_CMD) PROFILE=1 DEBUG=1
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
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(_DST) -c $(_SRC)
!endif

.c.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(_DST) -c $(_SRC)
!endif

.c.pre-c:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -P+ -Pd -Pc+ -c $(_SRC) > $@
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -P+ -Pd -Pc+ -c $(_SRC) > $@
!endif


.cpp{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(_DST) -c $(_SRC)
!endif

.cpp.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(_DST) -c $(_SRC)
!endif

.cpp.pre-cpp:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -P+ -Pd -Pc+ -c $(_SRC) > $@
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -P+ -Pd -Pc+ -c $(_SRC) > $@
!endif


.asm{$(OBJDIR)}.obj:
    $(CMDQD_SUB) $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(_SRC)

.orc{$(OBJDIR)}.orc_asm:
    $(CMDQD_SUB) $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $(_SRC)

.orc{$(OBJDIR)}.obj:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $(_SRC)
#    $(RC) $(RCFLAGS) $(RCINCLUDES) -o $(OBJDIR)\$(@B).orc_asm -i $(_SRC)
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(OBJDIR)\$(@B).orc_asm

.rc{$(OBJDIR)}.res:
    $(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $(_SRC) $(_DST)

