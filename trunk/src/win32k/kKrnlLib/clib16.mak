# $Id: clib16.mak,v 1.2 2002-04-01 12:42:46 bird Exp $

#
# clib16
#
# Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
#


#
# Setup.
#
PATH_ROOT = ..\..\..
!include $(PATH_ROOT)\tools\make\setup.mak
!include ..\makefile.inc


#
# Config.
#
TARGET_MODE = LIB
TARGET_NAME = kKrnlLib_clib16
MAKEFILE    = clib16.mak


#
# Object files.
#
TARGET_OBJS =\
$(PATH_TARGET)\inp.$(EXT_OBJ)\
$(PATH_TARGET)\outp.$(EXT_OBJ)\
$(PATH_TARGET)\anfalmul.$(EXT_OBJ)\
$(PATH_TARGET)\anfaldiv.$(EXT_OBJ)\
$(PATH_TARGET)\anfalrem.$(EXT_OBJ)\
$(PATH_TARGET)\anfauldi.$(EXT_OBJ)\
$(PATH_TARGET)\anuldiv.$(EXT_OBJ)\
$(PATH_TARGET)\anldiv.$(EXT_OBJ)\
$(PATH_TARGET)\fmemcpy.$(EXT_OBJ)\
$(PATH_TARGET)\anulrem.$(EXT_OBJ)\
$(PATH_TARGET)\anlrem.$(EXT_OBJ)\
$(PATH_TARGET)\anlmul.$(EXT_OBJ)\
$(PATH_TARGET)\__AHINCR.$(EXT_OBJ)\
$(PATH_TARGET)\anlshl.$(EXT_OBJ)\


#
# Process
#
!include $(MAKE_INCLUDE_PROCESS)


#
# Convert the library - temporary file.
#
$(PATH_TARGET)\_clib16_.lib: $(PATH_MSC)\lib\clibcep.lib
    $(PATH_TOOLS)\libconv.exe $** $@

#
# Extract required object files from the converted library.
#
$(TARGET_OBJS): $(PATH_TARGET)\_clib16_.lib
    $(AR) $(AR_FLAGS) $** *$@;

