# $Id: kLibR3.mak,v 1.1 2002-08-20 04:37:16 bird Exp $

#
# kLib main file.
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\makefile.inc
!include $(PATH_ROOT)\make\setup.mak


#
# kLib overrides variables.
#
!if [SET KLIB_INC=$(MAKEDIR)\kLibR3.inc]
!endif


#
# Config.
#
TARGET_MODE = EMPTY
TARGET_NAME = kLibR3_empty
MAKEFILE    = kLibR3.mak
!ifdef WITH_KLIB
SUBDIRS     = kLib\src
!endif


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)

