# $Id: makefile,v 1.6 2001-02-10 11:11:48 bird Exp $

#
# Odin32 API
#
#       Win32kCC.exe makefile
#


#
# Tell buildenvironment that we're making an exe and to invoke additional dep rule
#
EXETARGET=1
ADDITIONAL_DEP = mydep
WIN32KAPI     = ..\api
WIN32KMISC    = ..\misc
WIN32KINCLUDE = ..\include


#
# Compiler, tools, and interference rules thru the win32k makefile.inc file.
#
!include ..\..\..\makefile.inc


#
# Tools and Flags Addjustments
#  (The author of this is kinda SM when it comes to warnings... ;-)
#
CINCLUDES  = -I$(WIN32KINCLUDE) $(CINCLUDES)
CDEFINES   = $(CDEFINES) -DRING3
!if "$(VAC3)" == "1" | "$(VAC36)" == "1"
CFLAGS     = $(CFLAGS)   -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CXXFLAGS   = $(CXXFLAGS) -Wall+ppt-ppc-inl-cnv-gnr-vft-
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\Win32kCC.obj 
#$(OBJDIR)\api.obj \
#$(OBJDIR)\locks.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(RTLLIB) \
$(ODIN32_LIB)\win32k.lib \
os2386.lib


#
# OS/2 resources.
#
OS2RES = \
$(OBJDIR)\$(TARGET).res


#
# Target name - name of the exe without extention and path.
#
TARGET = Win32kCC


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Dependencies.
#
mydep:
    $(DEPEND) -a -o$$(OBJDIR) $(CINCLUDES) $(WIN32KINCLUDE)\*.h


#
# Extra rules to build api.obj and locks.obj
#
!if "$(VAC3)" == "1" | "$(VAC36)" == "1"
$(OBJDIR)\api.obj: $(WIN32KAPI)\api.cpp
    $(CC) -c $(CXXFLAGS) $(CDEFINES) $(CINCLUDES) -Fo$(OBJDIR)\$(@B).obj $**

$(OBJDIR)\locks.obj: $(WIN32KMISC)\locks.asm
    $(AS) $(ASFLAGS) -I:$(WIN32KINCLUDE) -Fdo:$(OBJDIR) $**
!else
!error  "not ready for this compiler yet"
!endif




