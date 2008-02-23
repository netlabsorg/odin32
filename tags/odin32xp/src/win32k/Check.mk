################################################################################
# $Id: Check.mk,v 1.1 2000-03-09 20:30:43 bird Exp $
#
# This makefile checks if makefile.inc is up-to-date.
#
# Copyright 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#
################################################################################

makefile.inc: configure.cmd
    @echo "nmake < ver5 needs this command" > nul
