# $Id: msvcrt20.mak,v 1.6 2003-01-10 13:13:49 sandervl Exp $

#
# Odin32 API
#
#       msvcrt.dll makefile
#


#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = msvcrt20.mak
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
$(OBJDIR)\mainmsvcrt20.obj \
$(OBJDIR)\$(TARGET)rsrc.obj 

#
# Target name - name of the dll without extention and path.
#
TARGET = msvcrt20

all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(TARGET).lib

$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Zso -Zsys $(OBJS) $(TARGET).def -L.\libs -lm\
      -L$(OBJDIR) -liberty_s -lmsvcrt -o $(OBJDIR)\$(TARGET).dll
    touch $(OBJDIR)\$(TARGET).map
    @echo "Illegal Sym File for EMX" > $(OBJDIR)\$(TARGET).sym

$(TARGET).lib: 
    implib $(OBJDIR)\$(TARGET).lib msvcrt20.def
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Release
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Debug


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

