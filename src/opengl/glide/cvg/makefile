# $Id: makefile,v 1.12 2000-12-03 00:26:11 bird Exp $

#
# Odin32 API
#
#       Glide\cvg makefile
#


#
# Tell buildenvironment that we should not copy this into /bin.
#
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include ../../../../makefile.inc


#
# Subdirectories to travers.
#
SUBDIRS = \
.\..\swlibs\pcilib \
.\init \
.\glide \
.\texus


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
..\swlibs\pcilib\$(OBJDIR)\pcilib.lib \
texus\$(OBJDIR)/texus.lib \
init\$(OBJDIR)/init.lib \
glide\$(OBJDIR)/glide.lib \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = glide2x


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

