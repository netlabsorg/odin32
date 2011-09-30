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
$(OBJDIR)\aes.obj \
$(OBJDIR)\des.obj \
$(OBJDIR)\handle.obj \
$(OBJDIR)\implglue.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initrsaenh.obj \
$(OBJDIR)\md2.obj \
$(OBJDIR)\mpi.obj \
$(OBJDIR)\rc2.obj \
$(OBJDIR)\rc4.obj \
$(OBJDIR)\rsa.obj \
$(OBJDIR)\rsaenh.obj

#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/crypt32.lib \
$(ODIN32_LIB)/advapi32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = rsaenh


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

