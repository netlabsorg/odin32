#
# Odin32 API
#
#       minivcrt.lib makefile
#


#
# Tell buildenvironment that we have a non-default makefilename.
#
MAKEFILE = minivcrt.mak

CCENV = EMX
LIBTARGET = 1
PUBLICLIB = 1

DIREXT2 = .minivcrt

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


CDEFINES = -D__MINIVCRT__ $(CDEFINES)


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\dir.obj \
$(OBJDIR)\environ.obj \
$(OBJDIR)\errno.obj \
$(OBJDIR)\wcs.obj

LIBS = \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\ntdll.lib \
$(ODIN32_LIB)\user32.lib


#
# Target name - name of the dll without extention and path.
#
TARGET = minivcrt


# Includes the common rules.
#
!include $(ODIN32_POST_INC)
