# $Id: Makefile,v 1.2 2002-04-07 22:37:23 bird Exp $

#
# kKrnlLib/testcase makefile.
#
# Test makefile is setup to automatically execute all testcases
# for the kKrnlLib.sys driver.
#

#
# Load the build setup.
#
!include ..\..\Makefile.inc
PATH_ROOT = ..\..\..\..
!include $(PATH_ROOT)\make\setup.mak


#
# Macros
#
KKRNLLIBR3TST = $(PATH_OBJ)\kKrnlLibR3Tst.exe\kKrnlLibR3Tst.exe

testcase: \
    Kernel-Identifications \

Kernel-Identifications:
    @$(ECHO) $(CLRFIL)kKrnlLib.sys $(CLRTXT)- Testcase 1: Massive kernel idenfication test.$(CLRRST)
    if exist h:\kernels\syms\14040R4.sym ..\tools\TestKernels.cmd $(KKRNLLIBR3TST) h:\kernels\syms
    @$(ECHO) $(CLRFIL)kKrnlLib.sys $(CLRTXT)- Testcase 1: Completed.$(CLRRST)
