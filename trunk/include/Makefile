# $Id: Makefile,v 1.1 2001-09-30 00:27:12 bird Exp $


#
# Include makefile.
#
!include ..\makefile.inc

_all: all
$(COMMONRULES:clean dep=):
    @echo Nothing to do for $@

dep:
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc win\*.h win\*.asm win\*.inc

clean:
    $(RM) -f .depend

