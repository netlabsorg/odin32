# $Id: kLibR3.inc,v 1.2 2002-04-29 11:43:04 bird Exp $

#
# kLib.mk - configuration overloader for kLib in kLibKrnl.sys.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#
# GPL
#


#
# The Win32k Makefile.inc
#
!include $(PATH_ROOT)\Makefile.inc


#
# Some changes are to be done.. :-)
#
ALL_INCLUDES    = -I$(PATH_KKRNLLIB:\=/)/include -I$(PATH_KKRNLLIB:\=/)/include/kLib
ALL_DEFINES     = -DKKRNLLIB
KLIB_POSTFIX    = R3
PATH_ROOT       = $(PATH_WIN32K)\..\..
!include $(PATH_ROOT)\make\setup.mak



