# $Id: Makefile,v 1.6 2005-05-16 14:16:27 sao2l02 Exp $

# Subdirectories.
SUBDIRS = \
.\idl2h

#
# Include makefile.
#
!include ..\makefile.inc

dep: needed
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc win\*.h win\*.asm win\*.inc
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

all: dep
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

clean:
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

cleanall: clean
    $(RM) .depend
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@

cleandlls needed:
    @$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@
    @echo Nothing to do for $@

