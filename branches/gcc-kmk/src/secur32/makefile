# $Id: makefile,v 1.36 2004/12/04 10:22:58 sao2l02 Exp $

#
# Odin32 API
#
#       rsaenh.dll makefile
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
$(OBJDIR)\base64_codec.obj \
$(OBJDIR)\dispatcher.obj \
$(OBJDIR)\hmac_md5.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initsecur32.obj \
$(OBJDIR)\lsa.obj \
$(OBJDIR)\negotiate.obj \
$(OBJDIR)\ntlm.obj \
$(OBJDIR)\schannel.obj \
$(OBJDIR)\secur32.obj \
$(OBJDIR)\thunks.obj \
$(OBJDIR)\util.obj \
$(OBJDIR)\wrapper.obj

#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/crypt32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/ntdll.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = secur32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

