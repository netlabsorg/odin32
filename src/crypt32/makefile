# $Id: makefile,v 1.36 2004/12/04 10:22:58 sao2l02 Exp $

#
# Odin32 API
#
#       crypt32.dll makefile
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
$(OBJDIR)\base64.obj \
$(OBJDIR)\cert.obj \
$(OBJDIR)\chain.obj \
$(OBJDIR)\collectionstore.obj \
$(OBJDIR)\context.obj \
$(OBJDIR)\crl.obj \
$(OBJDIR)\ctl.obj \
$(OBJDIR)\decode.obj \
$(OBJDIR)\encode.obj \
$(OBJDIR)\filestore.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initcrypt32.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\message.obj \
$(OBJDIR)\msg.obj \
$(OBJDIR)\object.obj \
$(OBJDIR)\oid.obj \
$(OBJDIR)\proplist.obj \
$(OBJDIR)\protectdata.obj \
$(OBJDIR)\provstore.obj \
$(OBJDIR)\regstore.obj \
$(OBJDIR)\rootstore.obj \
$(OBJDIR)\serialize.obj \
$(OBJDIR)\sip.obj \
$(OBJDIR)\store.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\str.obj \
$(OBJDIR)\crypt32rsrc.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = crypt32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

