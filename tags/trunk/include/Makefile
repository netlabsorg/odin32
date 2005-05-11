# $Id: Makefile,v 1.5 2005-05-11 21:28:19 sao2l02 Exp $

# Subdirectories.
SUBDIRS = \
.\idl2h

#
# Include makefile.
#
!include ..\makefile.inc

_all: all

dep:
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc win\*.h win\*.asm win\*.inc
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

all: dep
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

cleandlls:

clean:
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

cleanall: clean
    $(RM) .depend

$(COMMONRULES:clean= cleanall= cleandlls= dep= libs= all=) needed:
    @echo Nothing to do for $@

