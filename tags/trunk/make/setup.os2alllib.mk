# $Id: setup.os2alllib.mk,v 1.1 2002-08-29 10:01:41 bird Exp $

#
# The tool(s)
#
AR              = lib.exe
IMPLIB          = implib.exe

#
# The flags
#
AR_FLAGS        = /nologo /noignorecase
AR_CMD          = $(AR) $(AR_FLAGS) @"$(TARGET_LNK)"
AR_LNK1         = $(@R).$(EXT_LIB)
AR_LNK2         = y
_AR_LNK3        = +$(TARGET_OBJS: =&^
 +)
AR_LNK3         = $(_AR_LNK3:+&^
=)
AR_LNK4         = $(@R).lst;

IMPLIB_FLAGS    = /NOI /Nologo

