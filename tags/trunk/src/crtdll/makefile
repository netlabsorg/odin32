# $Id: makefile,v 1.19 2003-01-10 13:14:56 sandervl Exp $

#
# Odin32 API
#
#       crtdll.dll makefile
#


CCENV = EMX
NOTEXPDEF = 1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\crtdll_main.obj \
$(OBJDIR)\crtdllrsrc.obj \
$(OBJDIR)\maincrtdll.obj 

#
# Target name - name of the dll without extention and path.
# Original target
#
TARGET      = crtdll32
ORGTARGET   = crtdll

all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(ORGTARGET).lib

$(OBJDIR)\$(TARGET).dll: $(OBJS) $(ORGTARGET).def
    $(LD) $(LDFLAGS) -L$(ODIN32_LIB_)\Release -Zso -Zsys $(OBJS) $(ORGTARGET).def -L.\libs -lm\
      -liberty_s -lmsvcrt -o $(OBJDIR)\$(TARGET).dll
    touch $(OBJDIR)\$(TARGET).map
    @echo "Illegal Sym File for EMX" > $(OBJDIR)\$(TARGET).sym

# Includes the common rules.
#
!include $(ODIN32_POST_INC)

$(ORGTARGET).lib: 
    implib $(OBJDIR)\$(ORGTARGET).lib crtdll.def
    @copy $(OBJDIR)\$(ORGTARGET).lib $(ODIN32_LIB_)\Release > nul:
    @copy $(OBJDIR)\$(ORGTARGET).lib $(ODIN32_LIB_)\Debug  > nul:



