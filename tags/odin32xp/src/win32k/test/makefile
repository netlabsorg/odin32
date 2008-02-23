# $Id: makefile,v 1.1 2000-12-11 06:43:14 bird Exp $
#
# Makefile for the standalone tests (not win32ktst.exe).
# Makes:
#       invokelongexename-*.exe
#       longexecutablename*
#
# Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#
#


#
# Compiler, tools, and interference rules.
#
!include ../../../makefile.inc


#
# The all rule.
#
all:    invokelongexename-drv.exe \
        invokelongexename-dll.exe \
        invokelongexename-..exe \
        longexecutablename.drv \
        longexecutablename.dll \
        longexecutablename


#
# libraries.
#
longexecutablename.drv.lib: longexecutablename.drv.def
    $(IMPLIB) longexecutablename.drv.lib longexecutablename.drv.def

longexecutablename.lib: longexecutablename.def
    $(IMPLIB) longexecutablename.lib longexecutablename.def

longexecutablename..lib: longexecutablename..def
    $(IMPLIB) longexecutablename..lib longexecutablename..def

#
# Dlls
#
longexecutablename.drv: longexecutablename.drv.c longexecutablename.drv.def
    icc /Ge- /Felongexecutablename.drv longexecutablename.drv.c longexecutablename.drv.def

longexecutablename.dll: longexecutablename.c longexecutablename.def
    icc /Ge- /Felongexecutablename.dll longexecutablename.c longexecutablename.def

longexecutablename: longexecutablename..c longexecutablename..def
    icc /Ge- /Felongexecutablename. longexecutablename..c longexecutablename..def

#
# Exes
#
invokelongexename-drv.exe: invokelongexename.c longexecutablename.drv.lib
    icc /Ge+ /Feinvokelongexename-drv.exe invokelongexename.c longexecutablename.drv.lib

invokelongexename-dll.exe: invokelongexename.c longexecutablename.lib
    icc /Ge+ /Feinvokelongexename-dll.exe invokelongexename.c longexecutablename.lib

invokelongexename-..exe: invokelongexename.c longexecutablename..lib
    icc /Ge+ /Feinvokelongexename-..exe invokelongexename.c longexecutablename..lib


#
# Clean rule.
#
clean:
    $(RM) *.exe *.dll *.lib *.obj *.drv longexecutablename
