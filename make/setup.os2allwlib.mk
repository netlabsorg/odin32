# $Id: setup.os2allwlib.mk,v 1.1 2002-08-29 10:01:41 bird Exp $

#
# The tool(s)
#
AR              = wlib.exe
IMPLIB          = implib.exe

#
# The flags
#
AR_FLAGS        = -q -c -fo -b
AR_CMD          = $(AR) $(AR_FLAGS) @"$(TARGET_LNK)"
AR_LNK1         = -l="$(@R).lst"
AR_LNK2         = "$(@R).$(EXT_LIB)"
_AR_LNK3        = +"$(TARGET_OBJS: ="^
 +")"
AR_LNK3         = $(_AR_LNK3:+""&^
=)

IMPLIB_FLAGS    = /NOI /Nologo

