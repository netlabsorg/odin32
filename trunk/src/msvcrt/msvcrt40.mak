# $Id: msvcrt40.mak,v 1.8 2003-04-10 10:28:05 sandervl Exp $

#
# Odin32 API
#
#       msvcrt.dll makefile
#


#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = msvcrt40.mak
CCENV = EMX
NOTEXPDEF = 1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc

!ifndef WMAKE
CDEFINES    = -I. $(CDEFINES) -DUSE_MSVCRT_PREFIX -D_MT -I$(ODIN32_INCLUDE)\win\msvcrt 
!else
CDEFINES   += -DUSE_MSVCRT_PREFIX -D_MT -I$(ODIN32_INCLUDE)\win\msvcrt
!endif

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\mainmsvcrt40.obj \
$(OBJDIR)\$(TARGET)rsrc.obj 

LIBS = \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\msvcrt.lib \
$(ODIN32_LIB)\ntdll.lib \
$(ODIN32_LIB)\user32.lib \
$(EMX)\lib\mt\c.lib \
$(EMX)\lib\mt\c_dllso.lib \
$(EMX)\lib\mt\sys.lib \
$(EMX)\lib\os2.lib \

#
# Target name - name of the dll without extention and path.
#
TARGET = msvcrt40

all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(TARGET).lib

$(TARGET).lib: 
    implib $(OBJDIR)\$(TARGET).lib msvcrt40.def
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Release
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Debug

lib: $(TARGET).lib

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

