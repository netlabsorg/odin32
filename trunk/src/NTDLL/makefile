# $Id: makefile,v 1.35 2003-01-23 20:21:58 sandervl Exp $

#
# Odin32 API
#
#       ntdll.dll makefile
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
$(OBJDIR)\ntdll.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\exception.obj \
$(OBJDIR)\nt.obj \
$(OBJDIR)\om.obj \
$(OBJDIR)\reg.obj \
$(OBJDIR)\rtl.obj \
$(OBJDIR)\rtlstr.obj \
$(OBJDIR)\rtlbitmap.obj \
$(OBJDIR)\wcstring.obj \
$(OBJDIR)\sec.obj \
$(OBJDIR)\sync.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initntdll.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\unknown.obj \
$(OBJDIR)\large_int.obj \
$(OBJDIR)\large_int_odin.obj \
$(OBJDIR)\regfunc.obj \
$(OBJDIR)\error.obj \
$(OBJDIR)\loader.obj \
##$(OBJDIR)\heap.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\ntdllrsrc.obj


#
# Libraries. One space before the '\'.
#


#
# Target name - name of the dll without extention and path.
#
TARGET = ntdll

all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(TARGET).lib

$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -L$(ODIN32_LIB_)\Release -Zso -Zsys $(OBJS) $(TARGET).def -L.\libs -lm\
      -liberty_s -lstdcxx -o $(OBJDIR)\$(TARGET).dll
    touch $(OBJDIR)\$(TARGET).map
    @echo "Illegal Sym File for EMX" > $(OBJDIR)\$(TARGET).sym

$(TARGET).lib: 
    implib $(OBJDIR)\$(TARGET).lib ntdll.def
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Release > nul:
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Debug  > nul:

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

lib:
    $(IMPDEF) $(IMPDEF_FLAGS) ntdll-vac.def $(OBJDIR)\ntdllexp.def
    implib $(OBJDIR)\$(TARGET).lib $(OBJDIR)\ntdllexp.def
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Release > nul:
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Debug  > nul: