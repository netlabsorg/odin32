# $Id: odin32.tools.emx.mk,v 1.3 2001-06-25 23:41:25 bird Exp $

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

.c{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o$(OBJDIR)\$(@B).obj -c $<
!else
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -o$(OBJDIR)\$(@B).obj -c $<
!endif

.c.obj:
!ifdef USERAPP
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o$(OBJDIR)\$(@B).obj -c $<
!else
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -o$(OBJDIR)\$(@B).obj -c $<
!endif

.c.pre-c:
!ifdef USERAPP
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -E -c $< > $@
!else
    $(CC) $(CFLAGS) $(CINCLUDES) $(CDEFINES) -E -c $< > $@
!endif


.cpp{$(OBJDIR)}.obj:
!ifdef USERAPP
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o$(OBJDIR)\$(@B).obj -c $<
!else
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -o$(OBJDIR)\$(@B).obj -c $<
!endif

.cpp.obj:
!ifdef USERAPP
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -o$(OBJDIR)\$(@B).obj -c $<
!else
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -o$(OBJDIR)\$(@B).obj -c $<
!endif

.cpp.pre-cpp:
!ifdef USERAPP
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES_WIN32APP) -E -c $< > $@
!else
    $(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -E -c $< > $@
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

