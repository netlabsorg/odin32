# $Id: makefile,v 1.11 2001-02-18 14:49:04 bird Exp $

#
# Odin32 API
#
#       win32k.lib makefile
#


#
# Tell build environment we're build a public library which should
# work with both exe and dlls (hence EXETARGET).
#
LIBTARGET=1
PUBLICLIB=1
EXETARGET=1


#
# Include
#
!include ..\..\..\makefile.inc


#
# Tools and Flags Addjustments
#
CDEFINES   = -DRING3 -DWIN32KLIB
!if "$(VAC3)" == "1" | "$(VAC36)" == "1"
CFLAGS     = $(CFLAGS)    -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Rn
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
$(OBJDIR)\libW32kHandleSystemEvent.obj \
$(OBJDIR)\libW32kQueryOTEs.obj \
$(OBJDIR)\libW32kQuerySystemMemInfo.obj \
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

