# $Id: odin32.tools.vac3.mk,v 1.1 2000-12-02 23:50:49 bird Exp $

#
# Odin32 API
#
#       IBM Visual Age for C++ v3.08 tools and interference rules.
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
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<

.cpp{$(OBJDIR)}.obj:
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -Fo$(OBJDIR)\$(@B).obj -c $<

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

