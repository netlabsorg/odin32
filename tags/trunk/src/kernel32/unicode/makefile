# $Id: makefile,v 1.3 2001-04-17 04:22:06 bird Exp $

#
# Odin32 API
#
#       common.lib makefile
#


#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# Overrides.
#
!if "$(CCENV)" == "VAC3" || "$(CCENV)" == "VAC36"
CFLAGS = $(CFLAGS) /Tdp
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\casemap.obj \
$(OBJDIR)\compose.obj \
$(OBJDIR)\cptable.obj \
$(OBJDIR)\mbtowc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\utf8.obj \
$(OBJDIR)\wctomb.obj \
$(OBJDIR)\wctype.obj \
$(OBJDIR)\c_037.obj \
$(OBJDIR)\c_042.obj \
$(OBJDIR)\c_424.obj \
$(OBJDIR)\c_437.obj \
$(OBJDIR)\c_500.obj \
$(OBJDIR)\c_737.obj \
$(OBJDIR)\c_775.obj \
$(OBJDIR)\c_850.obj \
$(OBJDIR)\c_852.obj \
$(OBJDIR)\c_855.obj \
$(OBJDIR)\c_856.obj \
$(OBJDIR)\c_857.obj \
$(OBJDIR)\c_860.obj \
$(OBJDIR)\c_861.obj \
$(OBJDIR)\c_862.obj \
$(OBJDIR)\c_863.obj \
$(OBJDIR)\c_864.obj \
$(OBJDIR)\c_865.obj \
$(OBJDIR)\c_866.obj \
$(OBJDIR)\c_869.obj \
$(OBJDIR)\c_874.obj \
$(OBJDIR)\c_875.obj \
$(OBJDIR)\c_878.obj \
$(OBJDIR)\c_932.obj \
$(OBJDIR)\c_936.obj \
$(OBJDIR)\c_949.obj \
$(OBJDIR)\c_950.obj \
$(OBJDIR)\c_1006.obj \
$(OBJDIR)\c_1026.obj \
$(OBJDIR)\c_1250.obj \
$(OBJDIR)\c_1251.obj \
$(OBJDIR)\c_1252.obj \
$(OBJDIR)\c_1253.obj \
$(OBJDIR)\c_1254.obj \
$(OBJDIR)\c_1255.obj \
$(OBJDIR)\c_1256.obj \
$(OBJDIR)\c_1257.obj \
$(OBJDIR)\c_1258.obj \
$(OBJDIR)\c_10000.obj \
$(OBJDIR)\c_10006.obj \
$(OBJDIR)\c_10007.obj \
$(OBJDIR)\c_10029.obj \
$(OBJDIR)\c_10079.obj \
$(OBJDIR)\c_10081.obj \
$(OBJDIR)\c_20866.obj \
$(OBJDIR)\c_28591.obj \
$(OBJDIR)\c_28592.obj \
$(OBJDIR)\c_28593.obj \
$(OBJDIR)\c_28594.obj \
$(OBJDIR)\c_28595.obj \
$(OBJDIR)\c_28596.obj \
$(OBJDIR)\c_28597.obj \
$(OBJDIR)\c_28598.obj \
$(OBJDIR)\c_28599.obj \
$(OBJDIR)\c_28600.obj \
$(OBJDIR)\c_28603.obj \
$(OBJDIR)\c_28604.obj \
$(OBJDIR)\c_28605.obj

#
# Target name - name of the library without extention and path.
#
TARGET  = unicode


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

