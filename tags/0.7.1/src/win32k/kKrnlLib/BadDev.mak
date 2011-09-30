# $Id: BadDev.mak,v 1.1 2001-12-03 00:05:40 bird Exp $

#
# kKrnlLib.sys makefile.
#
# Copyright 1998-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
#
# Project Odin Software License can be found in LICENSE.TXT
#


#
# Include path definitions, common stuff and our own tools.
#
CDEFINESEXTRA   = -DKKRNLLIB
CDEFINESEXTRA16 = -DKKRNLLIB
!include makefile.inc
!include ../../../makefile.inc
!include ../win32k.tools.mk


#
# Main targetname
#
TARGET = BadDev


#
# Main rules.
#
all:    $(WIN32KBIN)\$(TARGET).sys


#
# BadDev.sys object.
#
OBJS  =\
$(WIN32KOBJ)\$(TARGET).obj


LIBS =\




#
# BadDev.sys rule.
#
$(TARGET).sys: $(WIN32KBIN)\$(TARGET).sys

$(WIN32KBIN)\$(TARGET).sys: $(OBJS) \
                            $(LIBS) \
                            $(WIN32KOBJ)\$(@B).lnk \
                            BadDev.mak makefile.inc ..\..\..\makefile.inc
    -@$(ECHO) linking: $@
    $(LD) $(LFLAGS) @$(WIN32KOBJ)\$(@B).lnk
    cd $(WIN32KBIN)
    @mapsym $(*B).map > nul
    cd $(WIN32KBASE)
    if not exist $(ODIN32_BIN) $(CREATEPATH) $(ODIN32_BIN)
    $(CP) $@ $(ODIN32_BIN)
    $(CP) $*.sym $(ODIN32_BIN)

# Linker file.
$(WIN32KOBJ)\$(TARGET).lnk: BadDev.mak makefile.inc ..\..\..\makefile.inc
    echo Creating linkerfile: @<<$(WIN32KOBJ)\$(TARGET).lnk
/OUT:$(WIN32KBIN)\$(TARGET).sys
/MAP:$(WIN32KBIN)\$(TARGET).map
$(OBJS:  =^
)
$(LIBS:  =^
)
$(LASTLIB)
kKrnlLib.def
<<KEEP






