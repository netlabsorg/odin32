# $Id: makefile,v 1.8 2000-12-02 23:32:39 bird Exp $

#
# Odin32 API
#
#       win32k.lib makefile
#


#
# Tell build environment we're build a public library which should
# work with both exe and dlls (hence EXETARGET). It should also
# invoke the additional dependency rule.
#
LIBTARGET=1
PUBLICLIB=1
EXETARGET=1
ADDITIONAL_DEP = mydep
WIN32KINCLUDE = ..\include


#
# Include
#
!include ..\..\..\makefile.inc


#
# Tools and Flags Addjustments
#
CINCLUDES  = -I$(WIN32KINCLUDE) $(CINCLUDES)
CDEFINES   = -DRING3 -DWIN32KLIB
!if "$(VAC3)" == "1" | "$(VAC36)" == "1"
CFLAGS     = $(CFLAGS)    -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Rn
CXXFLAGS   = $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft-             -Rn
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\libInit.obj \
$(OBJDIR)\libTerm.obj \
$(OBJDIR)\libWin32kInstalled.obj \
$(OBJDIR)\libWin32kQueryOptionsStatus.obj \
$(OBJDIR)\libWin32kSetOptions.obj \
$(OBJDIR)\libDosAllocMemEx.obj \
$(OBJDIR)\libW32kQueryOTEs.obj \
$(OBJDIR)\libW32kProcessReadWrite.obj \
$(OBJDIR)\libGetCS.obj


#
# Target name - name of the lib without extention and path.
#
TARGET  = win32k


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Dependencies.
#
mydep:
    $(DEPEND) -a -o$$(OBJDIR) $(CINCLUDES) $(WIN32KINCLUDE)\*.h

