# $Id: msvcrt.mak,v 1.7 2003-01-15 10:43:16 sandervl Exp $

#
# Odin32 API
#
#       msvcrt.dll makefile
#


#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = msvcrt.mak
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
$(OBJDIR)\ctype.obj \
$(OBJDIR)\cpp.obj \
$(OBJDIR)\cppexcept.obj \
$(OBJDIR)\console.obj \
$(OBJDIR)\data.obj \
$(OBJDIR)\dir.obj \
$(OBJDIR)\environ.obj \
$(OBJDIR)\errno.obj \
$(OBJDIR)\except.obj \
$(OBJDIR)\exit.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\heap.obj \
$(OBJDIR)\lconv.obj \
$(OBJDIR)\locale.obj \
$(OBJDIR)\lock.obj \
$(OBJDIR)\main.obj \
$(OBJDIR)\scanf.obj \
$(OBJDIR)\math.obj \
$(OBJDIR)\mbcs.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\process.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\thread.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\wcs.obj \
$(OBJDIR)\rtlbitmap.obj \
$(OBJDIR)\relay2.obj \
$(OBJDIR)\$(TARGET)rsrc.obj 


#
# Target name - name of the dll without extention and path.
#
TARGET = msvcrt

all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(TARGET).lib

$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def
    $(LD) $(LDFLAGS) -Zso -Zsys $(OBJS) $(TARGET).def -L.\libs -lm\
      -liberty_s -o $(OBJDIR)\$(TARGET).dll
    touch $(OBJDIR)\$(TARGET).map
    @echo "Illegal Sym File for EMX" > $(OBJDIR)\$(TARGET).sym

# Includes the common rules.
#
!include $(ODIN32_POST_INC)

$(TARGET).lib: 
    implib $(OBJDIR)\$(TARGET).lib msvcrt.def
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Release > nul:
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Debug  > nul:

lib: $(TARGET).lib
