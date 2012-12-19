# $Id: msc60.mak,v 1.1 2001-06-13 02:54:57 bird Exp $

#
#  MSC v6.0a makefile.
#
#  Requires env. var. MSCPATH to be defined.
#


#
# 16-bit compiler and stuff.
#
CC          = $(MSCPATH)\binp\cl.exe
!ifdef DEBUG
CCFLAGS     = -c -nologo -G2 -FPi87 -Zp1 -W3 -Zi -AL -Od -D "_DEBUG" -DMSC6 -I$(MSCPATH)\include -I. -Imsc60
!else
CCFLAGS     = -c -nologo -G2 -FPi87 -Zp1 -W3     -AL -O1 -D "NDEBUG" -DMSC6 -I$(MSCPATH)\include -I. -Imsc60
!endif

LD          = link
!ifdef DEBUG
LDFLAGS     = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
!else
LDFLAGS     = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
!endif


#
# Work according to existing environment.
#
!ifdef DEBUG
OBJDIR=.\bin\debug.msc6
!else
OBJDIR=.\bin\release.msc6
!endif
!if [ mkdir bin 2> nul > nul ]
!endif
!if [ mkdir $(OBJDIR) 2> nul > nul ]
!endif

#
# The all rule.
#
all:    $(OBJDIR)\w16odin.exe  $(OBJDIR)\odindll.dll


#
# W16Odin.EXE.
#
w16odin.exe: $(OBJDIR)\w16odin.exe
$(OBJDIR)\w16odin.exe: $(OBJDIR)\odin.obj w16odin.def msc60.mak
    $(LD) $(LDFLAGS) $(OBJDIR)\odin.obj, $@, $(@R).map, oldnames libw commdlg shell olecli olesvr llibcew, $(@B).def;

#
# OdinDLL.DLL.
#
odindll.dll: $(OBJDIR)\odindll.dll
$(OBJDIR)\odindll.dll: $(OBJDIR)\odindll.obj odindll.def msc60.mak
    $(LD) $(LDFLAGS) $(OBJDIR)\odindll.obj, $@, $(@R).map, oldnames libw commdlg shell olecli olesvr llibcew, $(@B).def;


#
# Rule for executable objects.
#
.c{$(OBJDIR)}.obj:
    $(CC) $(CCFLAGS) /Fo$(OBJDIR)\$(@F) $<

.c.obj:
    $(CC) $(CCFLAGS) /Fo$(OBJDIR)\$(@F) $<

