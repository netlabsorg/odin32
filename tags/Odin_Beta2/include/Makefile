# $Id: Makefile,v 1.2 2001-09-30 05:31:21 bird Exp $


#
# Include makefile.
#
!include ..\makefile.inc

_all: all
$(COMMONRULES:dep=):
    @echo Nothing to do for $@

dep:
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc win\*.h win\*.asm win\*.inc

