# $Id: makefile,v 1.13 2000-12-03 05:13:58 bird Exp $

#
# Odin32 API
#
#       Glide\cvg makefile
#


#
# Tell buildenvironment that we should not copy this into /bin.
# Tell buildenvironment to make addtional target in the all rule.
#
NO_MAIN_BIN_COPY = 1
ADDITIONAL_ALL = $(ODIN32_BIN)\readme.txt


#
# Compiler, tools, and interference rules.
#
!include ../../../../makefile.inc


#
# Change bin directory.
#
ODIN32_BIN = $(ODIN32_BIN__)\Glide\Voodoo2


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


#
# Copyrule for the readme.txt file.
#
$(ODIN32_BIN)\readme.txt: readme.txt
    $(CP) readme.txt $@

