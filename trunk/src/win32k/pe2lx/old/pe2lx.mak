#
# PD-Win32 API
#
#	pe2lx.exe makefile
#

!include makefile.inc

#
# Tools
#
CC=icc
AS=alp
CXX=icc
LD=icc
IMPLIB=implib


#
# Tool Flags
#
CFLAGS = -Q -Si -Ti -Ge- -Ss+ -W3 -Gm+   -DWIN32OS2
CXXFLAGS = -Q -Si -Ti -Ge- -Ss+ -W3 -Gm+ -DWIN32OS2
ASFLAGS = -Sc -Sv:M510
LDFLAGS = -Q -Si -Ti -Ge- -Ss+ -W3 -Gm+ /B"/noe"
IMPLIBFLAGS = /nologo


CFLAGS = -I..\include -I$(PDWIN32_INCLUDE)  $(CFLAGS) -Ge+ /Tm+
CXXFLAGS = -I..\include -I$(PDWIN32_INCLUDE)  $(CXXFLAGS) -Ge+ /Tm+
LDFLAGS = $(LDFLAGS) -Ge+


#
# System Utilites
#
RM = del
CP = copy
MKDIR = mkdir
ECHO = echo


#
# Targets
#
.c.obj:
	@echo compiling: $(@B).c
	@$(CC) $(CFLAGS) -DDEBUG -c -Fo$@ $(@B).c

.cpp.obj:
	@echo compiling: $(@B).cpp
	@$(CXX) $(CXXFLAGS) -DDEBUG -c -Fo$@ $(@B).cpp

.asm.obj:
	@echo assembling: $<
	@$(AS) $(ASFLAGS) $< -Fo:$@




#TARGET = pe2lx_ring3
TARGET = pe2lx

OBJS =  conv\pefile.obj conv\pe.obj conv\lxsuper.obj conv\lxhdr.obj conv\icon.obj \
        conv\menu.obj conv\dialog.obj conv\misc.obj \
        conv\rcdata.obj conv\icongrp.obj \
        conv\strings.obj conv\bitmap.obj conv\accelerator.obj \
        conv\cursor.obj conv\cursorgrp.obj

all: $(TARGET).exe


$(TARGET).exe: $(OBJS)
		@echo linking $@
		@$(LD) $(LDFLAGS) -Fe$@ $(OBJS)	LIBCONV.LIB
		@$(CP) $@ $(PDWIN32_BIN)


conv\accelerator.cpp: accelerator.cpp ..\include\pefile.h ..\include\lx.h \
                 ..\include\accelerator.h  ..\include\icon.h ..\include\misc.h
conv\bitmap.cpp: bitmap.cpp ..\include\pefile.h ..\include\lx.h \
            ..\include\bitmap.h ..\include\icon.h ..\include\misc.h
conv\cursor.cpp: cursor.cpp ..\include\pefile.h ..\include\lx.h \
            ..\include\cursor.h ..\include\icon.h ..\include\misc.h
conv\cursorgrp.cpp: cursorgrp.cpp ..\include\pefile.h ..\include\lx.h  \
               ..\include\cursor.h ..\include\cursorgrp.h ..\include\misc.h
conv\dialog.cpp: dialog.cpp ..\include\pefile.h ..\include\lx.h \
            ..\include\dialog.h ..\include\misc.h
conv\icon.cpp: icon.cpp ..\include\pefile.h ..\include\lx.h ..\include\icon.h ..\include\misc.h
conv\icongrp.cpp: icongrp.cpp ..\include\pefile.h ..\include\lx.h ..\include\icon.h \
             ..\include\icongrp.h ..\include\misc.h
conv\lxhdr.cpp: lxhdr.cpp ..\include\lx.h
conv\lxsuper.cpp: lxsuper.cpp ..\include\lx.h
conv\menu.cpp: menu.cpp ..\include\pefile.h ..\include\lx.h ..\include\menu.h
conv\misc.cpp: misc.cpp ..\include\pefile.h ..\include\lx.h $(PDWIN32_INCLUDE)\version.h
conv\pe.cpp: pe.cpp ..\include\pefile.h ..\include\lx.h \
        ..\include\accelerator.h ..\include\bitmap.h ..\include\cursor.h \
        ..\include\cursorgrp.h ..\include\dialog.h ..\include\icon.h \
        ..\include\icongrp.h ..\include\menu.h ..\include\misc.h ..\include\rcdata.h \
        ..\include\strings.h
conv\pefile.cpp: pefile.cpp ..\include\pefile.h ..\include\misc.h
conv\rcdata.cpp: rcdata.cpp ..\include\lx.h ..\include\rcdata.h
conv\strings.cpp: strings.cpp ..\include\lx.h ..\include\misc.h ..\include\strings.h


clean:
	@-$(RM) $(OBJS) $(TARGET).lib $(TARGET).dll *~ $(TARGET).map *.pch $(TARGET).exe >& nul
#	$(RM) *.obj *.lib *.dll *~ *.map *.pch *.exe
#	$(RM) $(PDWIN32_BIN)\$(TARGET).exe
