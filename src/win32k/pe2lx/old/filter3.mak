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
CFLAGS = -Q -Si -Ti -Ge- -Ss+ -W3 -Gm+
CXXFLAGS = -Q -Si -Ti -Ge- -Ss+ -W3 -Gm+
ASFLAGS = -Mb -Sc -Sv:M510 -I:..\include\os2
LDFLAGS = -Q -Si -Ti -Ge- -Ss+ -W3 -Gm+ /B"/noe"
IMPLIBFLAGS = /nologo

CDEFINES = -DRING0 -DRING0_DEBUG_IN_RING3 -DDYNAMIC_HEAP -DWIN32OS2

CFLAGS   = $(CDEFINES) -I..\include -I$(PDWIN32_INCLUDE)  $(CFLAGS)   -Ge+ /Tm+
CXXFLAGS = $(CDEFINES) -I..\include -I$(PDWIN32_INCLUDE)  $(CXXFLAGS) -Ge+ /Tm+
LDFLAGS  = $(LDFLAGS)  -Ge+



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
	@$(CC) $(CFLAGS) -DDEBUG -c -Foflt\$@ $(@B).c


.cpp.obj:
	@echo compiling: $(@B).cpp
	@$(CXX) $(CXXFLAGS) -DDEBUG -c -Fo$@ $(@B).cpp

.asm.obj:
	@echo assemling: $<
	@$(AS) $(ASFLAGS) $< -Fo:$@


TARGET   = pe2lx_r0

# all:  getver.exe setver.exe $(TARGET).exe
all:  $(TARGET).exe



#		flt\pefile.obj 				\

OBJS =  flt\pe2lx.obj				\
		flt\lxsuper.obj 			\
		flt\lxfile.obj 				\
		flt\icon.obj 				\
        flt\menu.obj 				\
        flt\dialog.obj 				\
        flt\misc.obj  				\
        flt\rcdata.obj 				\
        flt\icongrp.obj  			\
        flt\strings.obj				\
        flt\bitmap.obj 				\
        flt\accelerator.obj  		\
        flt\cursor.obj 				\
        flt\cursorgrp.obj			\
        ..\misc\asmutils.obj    	\
		flt\malloc.obj     			\
		flt\new.obj     			\
		flt\log2.obj

flt\pe2lx.cpp:              pe2lx.cpp        ..\include\pe2lx.h
flt\pefile.cpp:             pefile.cpp		 ..\include\pe2lx.h
flt\lxsuper.cpp:            lxsuper.cpp		 ..\include\pe2lx.h
flt\lxfile.cpp:             lxfile.cpp		 ..\include\pe2lx.h
flt\accelerator.cpp:        accelerator.cpp	 ..\include\pe2lx.h
flt\icon.cpp:               icon.cpp		 ..\include\pe2lx.h
flt\menu.cpp:               menu.cpp		 ..\include\pe2lx.h
flt\dialog.cpp:             dialog.cpp		 ..\include\pe2lx.h
flt\misc.cpp:               misc.cpp		 ..\include\pe2lx.h
flt\rcdata.cpp:             rcdata.cpp		 ..\include\pe2lx.h
flt\icongrp.cpp:            icongrp.cpp		 ..\include\pe2lx.h
flt\strings.cpp:            strings.cpp		 ..\include\pe2lx.h
flt\bitmap.cpp:             bitmap.cpp		 ..\include\pe2lx.h
flt\cursor.cpp:             cursor.cpp		 ..\include\pe2lx.h
flt\cursorgrp.cpp:          cursorgrp.cpp	 ..\include\pe2lx.h
..\asmutils.obj:    		..\asmutils.asm
..\include\pe2lx.h:			..\include\pefile.h ..\include\lx.h ..\include\misc.h ..\include\log2.h ..\include\os2\types.h ..\include\os2\StackToFlat.h ..\include\ldr_functions.h ..\include\pefilter.h

flt\malloc.obj:				..\misc\malloc.c  ..\include\malloc.h
	@echo compiling: $(@B).c
	@$(CC) $(CFLAGS) -DDEBUG -c -Foflt\$(@B).obj ..\misc\$(@B).c

flt\log2.obj:         		..\misc\log2.cpp ..\include\log2.h
	@echo compiling: $(@B).cpp
	@$(CC) $(CFLAGS) -DDEBUG -c -Foflt\$(@B).obj ..\misc\$(@B).cpp

flt\new.obj:       			..\misc\new.cpp ..\include\new.h
	@echo compiling: $(@B).cpp
	@$(CC) $(CFLAGS) -DDEBUG -c -Foflt\$(@B).obj ..\misc\$(@B).cpp


$(TARGET).exe: $(OBJS)
		@echo linking $@
		$(LD) $(LDFLAGS) -Fe$@ -Fm $(OBJS) LIBULS.LIB LIBCONV.LIB
		@$(CP) $@ $(PDWIN32_BIN)

clean:
	@-$(RM) $(OBJS) $(TARGET).lib $(TARGET).dll *~ $(TARGET).map *.pch $(TARGET).exe >& nul

# $(RM) $(PDWIN32_BIN)\$(TARGET).exe

