# $Id: watcom.mak,v 1.1 2001-06-13 02:19:34 bird Exp $

#
#  Watcom v11.0 makefile.
#
#


#
# 16-bit compiler and stuff.
#
CC          = wcc
!ifdef DEBUG
CCFLAGS     = -ml -zq -zw -w4 -I$(WATCOM)\h\win -I$(WATCOM)\h -I. -d3 -d_DEBUG
!else
CCFLAGS     = -ml -zq -zw -w4 -I$(WATCOM)\h\win -I$(WATCOM)\h -I. -dNDEBUG
!endif
CCFLAGS_EXE = $(CCFLAGS) -bw
CCFLAGS_DLL = $(CCFLAGS) -bd

LD          = wlink
!ifdef DEBUG
LDFLAGS     = Debug Codeview All
!else
LDFLAGS     =
!endif


#
# Work according to existing environment.
#
!ifdef DEBUG
OBJDIR=.\bin\debug
!else
OBJDIR=.\bin\release
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
w16odin.exe: $(OBJDIR)\w16odin.exe watcom.mak
$(OBJDIR)\w16odin.exe: $(OBJDIR)\odin.obj
    $(LD) @<<$(OBJDIR)\$(@B).lnk
Name    $@
$(LDFLAGS)
File    {$(OBJDIR)\odin.obj}
LibPath $(WATCOM)\lib286\win
LibPath $(WATCOM)\lib286
Library {windows shell}

Format  Windows
Option  Map=$(@R).map
Option  Heapsize=1024
Option  Description 'W16Odin.EXE - WinOS2 Hack.'
Segment Type CODE SHARED
Segment Type DATA SHARED
<<keep


#
# OdinDLL.DLL.
#
$(OBJDIR)\odindll.dll: $(OBJDIR)\odindll.obj odindll.def watcom.mak
    $(LD) @<<$(OBJDIR)\$(@B).lnk
Name    $@
$(LDFLAGS)
File    {$(WATCOM)\lib286\win\libentry.obj $(OBJDIR)\odindll.obj}
LibPath $(WATCOM)\lib286\win
LibPath $(WATCOM)\lib286
Library {windows shell}

Format  Windows DLL
Option  Map=$(@R).map
Option  Heapsize=1024
Option  Description 'OdinDll.DLL - WinOS2 Hack.'
Segment Type CODE SHARED
Segment Type DATA SHARED
Export  MYGETVERSION.1, MYWINEXEC.2
<<keep


#
# Rule for executable objects.
#
$(OBJDIR)\odin.obj odin.obj: $(@B).c
    $(CC) $(CCFLAGS_EXE) -fo$(OBJDIR)\$(@F) $**

#
# Rule for dll objects.
#
$(OBJDIR)\odindll.obj odindll.obj: $(@B).c
    $(CC) $(CCFLAGS_DLL) -fo$(OBJDIR)\$(@F) $**

