# $Id: Makefile,v 1.4 2005-03-06 10:35:51 sao2l02 Exp $


#
# Include makefile.
#
!include ..\makefile.inc

_all cleandlls: all
$(COMMONRULES:dep=):
    @echo Nothing to do for $@

dep:
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc win\*.h win\*.asm win\*.inc

cleanall clean:
    $(RM) .depend
