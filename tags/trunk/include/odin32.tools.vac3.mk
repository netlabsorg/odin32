# $Id: odin32.tools.vac3.mk,v 1.5 2001-08-16 04:13:20 bird Exp $

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
MAKE_CMD    = nmake /nologo
!ifdef DEBUG
MAKE_CMD    = $(MAKE_CMD) DEBUG=1
!endif
!ifdef PROFILE
MAKE_CMD    = $(MAKE_CMD) DEBUG=1 PROFILE=1
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

.c{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(OBJDIR)\$(@B).obj -c $<
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<
!endif

.c.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(OBJDIR)\$(@B).obj -c $<
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<
!endif

.c.pre-c:
!ifdef USERAPP
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -P+ -Pd -Pc+ -c $< > $@
!else
    $(CMDQD_SUB) $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -P+ -Pd -Pc+ -c $< > $@
!endif


.cpp{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(OBJDIR)\$(@B).obj -c $<
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<
!endif

.cpp.obj:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(OBJDIR)\$(@B).obj -c $<
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<
!endif

.cpp.pre-cpp:
!ifdef USERAPP
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -P+ -Pd -Pc+ -c $< > $@
!else
    $(CMDQD_SUB) $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -P+ -Pd -Pc+ -c $< > $@
!endif


.asm{$(OBJDIR)}.obj:
    $(CMDQD_SUB) $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $<

.orc{$(OBJDIR)}.orc_asm:
    $(CMDQD_SUB) $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $<

.orc{$(OBJDIR)}.obj:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $<
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(OBJDIR)\$(@B).orc_asm"

.rc{$(OBJDIR)}.res:
    $(CMDQD_SUB) $(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $< $@


!if 0 # disabled.
#
# Old Targets
#
.c.obj:
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -c $<

.cpp.obj:
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -c $<

.asm.obj:
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $<

.orc.asm:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $@ $<

.rc.res:
    $(OS2RC) $(OS2RCFLAGS) $< $@
!endif

