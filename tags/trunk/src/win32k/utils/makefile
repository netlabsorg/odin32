# $Id: makefile,v 1.5 2000-12-02 23:32:41 bird Exp $

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

