# $Id: odin32.tools.emx.mk,v 1.8 2003-02-28 10:28:11 sandervl Exp $

#
# Odin32 API
#
#       GNU EMX tools and inference rules.
#


#
# EMX Specific tools
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
.SUFFIXES: .orc_asm .orc .pre-c .pre-cpp .S

_SRC = $(MAKEDIR)\$(<F)
_DST = $(OBJDIR)\$(@F)

.c{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o $(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -o $(_DST) -c $(_SRC)
!endif

.c.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o $(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -o $(_DST) -c $(_SRC)
!endif

.c.pre-c:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -E -c $(_SRC) > $@
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -E -c $(_SRC) > $@
!endif


.cpp{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o $(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -o $(_DST) -c $(_SRC)
!endif

.cpp.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o $(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -o $(_DST) -c $(_SRC)
!endif

.cpp.pre-cpp:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -E -c $(_SRC) > $@
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -E -c $(_SRC) > $@
!endif


.asm{$(OBJDIR)}.obj:
    $(CMDQD_SUB) $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(_SRC)

.orc{$(OBJDIR)}.orc_asm:
    $(CMDQD_SUB) $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $(_SRC)

.orc{$(OBJDIR)}.obj:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $(_SRC)
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(OBJDIR)\$(@B).orc_asm

.rc{$(OBJDIR)}.res:
    $(CMDQD_SUB) $(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $(_SRC) $@

.S{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o $(_DST) -c $(_SRC)
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -o $(_DST) -c $(_SRC)
!endif


