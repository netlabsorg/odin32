
#
# Odin32 API
#
#       winspool.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!if "$(DEBUG)" == "1"
DEFFILE    = winspooldbg.def
ORGDEFFILE = winspool.def
!endif

WRC_PREFIX_RESOURCE=1
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\winspool.obj \
$(OBJDIR)\oslibspl.obj \
$(OBJDIR)\winspoolrsrc.obj \
$(OBJDIR)\initwinspool.obj \
!ifdef DEBUG
$(OBJDIR)\dbgwrap.obj \
!endif
$(OBJDIR)\stubs.obj \
$(OBJDIR)\initterm.obj 


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/gdi32.lib \
$(ODIN32_LIB)/wgss50.lib \
$(ODIN32_LIB)/libwrap0.lib \
$(ODIN32_LIB)/libwrap1.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target name - name of the dll without extention and path.
#
TARGET = winspool


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

