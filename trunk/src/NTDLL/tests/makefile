# $Id: makefile,v 1.1 2003-04-09 09:38:58 sandervl Exp $

#
# Odin32 API
#
#       ntdll tests makefile
#


CCENV = EMX
STACKSIZE = 0x100000
EXETARGET = 1

#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\large_int.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\generated.obj \
$(OBJDIR)\rtlstr.obj \
$(OBJDIR)\main_test.obj


#
# Libraries. One space before the '\'.
#

# -liberty_s -lstdcxx -lExCRuntime -lExDllSupport -lc_alias -lgcc_eh
# -lgcc -lc -lc_dllso -lgcc_eh -lgcc -lc -lsys -los2 -lend

LIBS = \
$(EMX)\lib\iberty_s.lib \
$(EMX)\lib\gcc-lib\i386-pc-os2-emx\3.2.1\mt\stdcxx.lib \
$(EMX)\lib\c_alias.lib \
$(EMX)\lib\gcc-lib\i386-pc-os2-emx\3.2.1\mt\gcc_eh.lib \
$(EMX)\lib\gcc-lib\i386-pc-os2-emx\3.2.1\mt\gcc.lib \
$(EMX)\lib\mt\c.lib \
$(EMX)\lib\mt\c_app.lib \
$(EMX)\lib\mt\sys.lib \
$(EMX)\lib\os2.lib \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\user32.lib 

#
# Target name - name of the dll without extention and path.
#
TARGET = ntdll_test

#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

