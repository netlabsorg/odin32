# $Id: makefile,v 1.11 2000-11-19 08:58:10 bird Exp $

#
# Odin32 API
#
#       Glide\cvg makefile
#


#
# Directory macros.
#
PDWIN32_INCLUDE = ..\..\..\..\include
PDWIN32_LIB     = ..\..\..\..\lib
PDWIN32_BIN     = ..\..\..\..\$(OBJDIR)\Glide\Voodoo2
PDWIN32_TOOLS   = ..\..\..\..\tools\bin


#
# Tell buildenvironment that we should not copy this into /bin.
#
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.mk


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
$(PDWIN32_LIB)/kernel32.lib \
$(PDWIN32_LIB)/$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = glide2x


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

