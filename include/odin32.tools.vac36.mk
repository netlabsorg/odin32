# $Id: odin32.tools.vac36.mk,v 1.3 2001-04-02 12:38:04 sandervl Exp $

#
# Odin32 API
#
#       IBM C and C++ Compilers for OS/2 Version 3.6.5 tools
#       and interference rules.
#


#
# VAC36 Specific tools
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
.SUFFIXES: .orc_asm .orc

.c{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(OBJDIR)\$(@B).obj -c $<
!else
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<
!endif

.cpp{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -Fo$(OBJDIR)\$(@B).obj -c $<
!else
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<
!endif

.asm{$(OBJDIR)}.obj:
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $<

.orc{$(OBJDIR)}.orc_asm:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $<

.orc{$(OBJDIR)}.obj:
    $(RC) $(RCFLAGS) $(CINCLUDES) -o $(OBJDIR)\$(@B).orc_asm $<
    $(AS) $(ASFLAGS) -Fdo:$(OBJDIR) $(OBJDIR)\$(@B).orc_asm

.rc{$(OBJDIR)}.res:
    $(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $< $@


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

