# $Id: makefile,v 1.36 2004/12/04 10:22:58 sao2l02 Exp $

#
# Odin32 API
#
#       mscms.dll makefile
#


#
# Compiler, tools, and interference rules.
#
WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Overrides.
#
CDEFINES = $(CDEFINES) -DWIN32 -DSTDARG -DTHINK_C

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initmscms.obj \
$(OBJDIR)\handle.obj \
$(OBJDIR)\icc.obj \
$(OBJDIR)\mscms_main.obj \
$(OBJDIR)\profile.obj \
$(OBJDIR)\stub.obj \
$(OBJDIR)\transform.obj \


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/msvcrt.lib \
#$(ODIN32_LIB)/libwrap0.lib \
#$(ODIN32_LIB)/libwrap1.lib \
#$(ODIN32_LIB)/iphlpapi.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = mscms


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

