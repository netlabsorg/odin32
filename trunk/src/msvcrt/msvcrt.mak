# $Id: msvcrt.mak,v 1.8 2003-04-10 10:28:03 sandervl Exp $

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
WRC_PREFIX_RESOURCE=1
NOTEXPDEF = 1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


!ifndef WMAKE
CDEFINES    = -I$(ODIN32_INCLUDE)\emxruntime -I. $(CDEFINES) -DUSE_MSVCRT_PREFIX -D_MT
!else
CDEFINES   += -DUSE_MSVCRT_PREFIX -D_MT
!endif

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\initmsvcrtdll.obj \
$(OBJDIR)\ctype.obj \
$(OBJDIR)\cpp.obj \
$(OBJDIR)\cppexcept.obj \
$(OBJDIR)\console.obj \
$(OBJDIR)\data.obj \
#$(OBJDIR)\dbgcalls.obj \
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
$(OBJDIR)\forwarders.obj \
$(OBJDIR)\math.obj \
$(OBJDIR)\mbcs.obj \
$(OBJDIR)\misc.obj \
$(OBJDIR)\process.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\thread.obj \
$(OBJDIR)\time.obj \
$(OBJDIR)\wcs.obj \
$(OBJDIR)\relay2.obj \
$(OBJDIR)\$(TARGET)rsrc.obj 

LIBS = \
$(ODIN32_LIB_)\EmxSupport\EMXOS2FSRoutines.lib \
$(ODIN32_LIB_)\EmxSupport\ExCROS2Calls.lib \
$(ODIN32_LIB_)\EmxSupport\ExCRuntime.lib \
$(ODIN32_LIB_)\EmxSupport\ExDllSupport.lib \
$(ODIN32_LIB_)\EmxSupport\GCCBuiltins.lib \
$(ODIN32_LIB_)\EmxSupport\m.lib \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\ntdll.lib \
$(ODIN32_LIB)\user32.lib

#
# Target name - name of the dll without extention and path.
#
TARGET = msvcrt

all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(TARGET).lib

$(TARGET).lib: 
    implib $(OBJDIR)\$(TARGET).lib msvcrt.def
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Release
    @copy $(OBJDIR)\$(TARGET).lib $(ODIN32_LIB_)\Debug

# Includes the common rules.
#
!include $(ODIN32_POST_INC)

