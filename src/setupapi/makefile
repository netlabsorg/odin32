# $Id: makefile,v 1.3 2002-06-15 10:56:59 sandervl Exp $

#
# Odin32 API
#
#       setupapi.dll makefile
#


!if "$(DEBUG)" == "1"
DEFFILE    = setupapidbg.def
ORGDEFFILE = setupapi.def
!endif

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\devinst.obj \
$(OBJDIR)\dirid.obj \
$(OBJDIR)\infparse.obj \
$(OBJDIR)\install.obj \
$(OBJDIR)\parser.obj \
$(OBJDIR)\queue.obj \
$(OBJDIR)\stubs.obj \
##$(OBJDIR)\virtcopy.obj \
$(OBJDIR)\setupapirsrc.obj \
!if "$(DEBUG)" == "1"
$(OBJDIR)\dbgwrap.obj \
!endif
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/ole32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = setupapi


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

