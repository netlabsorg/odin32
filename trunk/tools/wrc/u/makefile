# $Id: makefile,v 1.1 2001-06-25 13:54:44 bird Exp $


#
# Directory macro.
#
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Wanna make a library for .exe files.
#
LIBTARGET = 1
EXETARGET = 1


#
# include common definitions
#
CCENV=EMX
!include ../../../makefile.inc


#
# Addjust common definitions
#
#DEFS      = -D__WINE__ -D__WIN32OS2__ -idirafter ../../include/win -idirafter ../../include
CDEFINES   = -D__EMX__ -DMASM -D__SEMICOLON__


#
# Object extension.
#
OBJ = obj

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS=\
$(OBJDIR)\string.$(OBJ) \
$(OBJDIR)\cptable.$(OBJ) \
$(OBJDIR)\utf8.$(OBJ) \
$(OBJDIR)\mbtowc.$(OBJ) \
$(OBJDIR)\wctomb.$(OBJ) \
$(OBJDIR)\c_869.$(OBJ) \
$(OBJDIR)\c_775.$(OBJ) \
$(OBJDIR)\c_865.$(OBJ) \
$(OBJDIR)\c_857.$(OBJ) \
$(OBJDIR)\c_10000.$(OBJ) \
$(OBJDIR)\c_10006.$(OBJ) \
$(OBJDIR)\c_861.$(OBJ) \
$(OBJDIR)\c_737.$(OBJ) \
$(OBJDIR)\c_860.$(OBJ) \
$(OBJDIR)\c_1253.$(OBJ) \
$(OBJDIR)\c_28597.$(OBJ) \
$(OBJDIR)\c_10029.$(OBJ) \
$(OBJDIR)\c_10081.$(OBJ) \
$(OBJDIR)\c_1257.$(OBJ) \
$(OBJDIR)\c_1252.$(OBJ) \
$(OBJDIR)\c_1254.$(OBJ) \
$(OBJDIR)\c_28591.$(OBJ) \
$(OBJDIR)\c_437.$(OBJ) \
$(OBJDIR)\c_10079.$(OBJ) \
$(OBJDIR)\c_852.$(OBJ) \
$(OBJDIR)\c_875.$(OBJ) \
$(OBJDIR)\c_863.$(OBJ) \
$(OBJDIR)\c_037.$(OBJ) \
$(OBJDIR)\c_1258.$(OBJ) \
$(OBJDIR)\c_1250.$(OBJ) \
$(OBJDIR)\c_850.$(OBJ) \
$(OBJDIR)\c_28605.$(OBJ) \
$(OBJDIR)\c_500.$(OBJ) \
$(OBJDIR)\c_28599.$(OBJ) \
$(OBJDIR)\c_28600.$(OBJ) \
$(OBJDIR)\c_28603.$(OBJ) \
$(OBJDIR)\c_28604.$(OBJ) \
$(OBJDIR)\c_28594.$(OBJ) \
$(OBJDIR)\c_1026.$(OBJ) \
$(OBJDIR)\c_28592.$(OBJ) \
$(OBJDIR)\c_28593.$(OBJ) \
$(OBJDIR)\c_856.$(OBJ) \
$(OBJDIR)\c_1006.$(OBJ) \
$(OBJDIR)\c_862.$(OBJ) \
$(OBJDIR)\c_864.$(OBJ) \
$(OBJDIR)\c_878.$(OBJ) \
$(OBJDIR)\c_866.$(OBJ) \
$(OBJDIR)\c_855.$(OBJ) \
$(OBJDIR)\c_20866.$(OBJ) \
$(OBJDIR)\c_874.$(OBJ) \
$(OBJDIR)\c_424.$(OBJ) \
$(OBJDIR)\c_1255.$(OBJ) \
$(OBJDIR)\c_1256.$(OBJ) \
$(OBJDIR)\c_28596.$(OBJ) \
$(OBJDIR)\c_28598.$(OBJ) \
$(OBJDIR)\c_10007.$(OBJ) \
$(OBJDIR)\c_1251.$(OBJ) \
$(OBJDIR)\c_28595.$(OBJ) \
$(OBJDIR)\c_042.$(OBJ) \
$(OBJDIR)\compose.$(OBJ) \
$(OBJDIR)\c_932.$(OBJ) \
$(OBJDIR)\c_950.$(OBJ) \
$(OBJDIR)\c_936.$(OBJ) \
$(OBJDIR)\c_949.$(OBJ) \
$(OBJDIR)\casemap.$(OBJ) \
$(OBJDIR)\wctype.$(OBJ)


#
# Target name - name of the exe without extention and path.
#
TARGET = u


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

