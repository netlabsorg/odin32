# $Id: Makefile,v 1.3 2003-02-28 10:28:09 sandervl Exp $


#
# Include makefile.
#
!include ..\makefile.inc

_all: all
$(COMMONRULES:dep=):
    @echo Nothing to do for $@

dep:
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc win\*.h win\*.asm win\*.inc

cleanall clean:
    $(RM) .depend
