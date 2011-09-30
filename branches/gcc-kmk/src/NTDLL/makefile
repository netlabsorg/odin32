# $Id: makefile,v 1.38 2003-04-08 12:47:06 sandervl Exp $

#
# Odin32 API
#
#       ntdll.dll makefile
#


CCENV = EMX
WRC_PREFIX_RESOURCE=1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc

CDEFINES   =  -I$(ODIN32_INCLUDE)\emxruntime $(CDEFINES)

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\ntdll.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\exception.obj \
$(OBJDIR)\nt.obj \
$(OBJDIR)\om.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\rtl.obj \
$(OBJDIR)\rtlstr.obj \
$(OBJDIR)\rtlbitmap.obj \
$(OBJDIR)\wcstring.obj \
$(OBJDIR)\sec.obj \
$(OBJDIR)\sync.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initntdll.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\large_int.obj \
$(OBJDIR)\large_int_odin.obj \
$(OBJDIR)\regfunc.obj \
$(OBJDIR)\error.obj \
$(OBJDIR)\loader.obj \
##$(OBJDIR)\heap.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\ntdllrsrc.obj


#
# Libraries. One space before the '\'.
#

# -liberty_s -lstdcxx -lExCRuntime -lExDllSupport -lc_alias -lgcc_eh
# -lgcc -lc -lc_dllso -lgcc_eh -lgcc -lc -lsys -los2 -lend

LIBS = \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\user32.lib 

#
# Target name - name of the dll without extention and path.
#
TARGET = ntdll

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

