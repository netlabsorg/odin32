# $Id: Makefile,v 1.4 2002-05-02 23:15:12 bird Exp $

#
# Odin32 API
#
#       Profiler hacks.
#


#
# Directory macros.
#
!include ../../makefile.inc


#
# The all rule.
#
#!ifdef VAC3
#all:    $(ODIN32_LIB)\cppopa3_fix2.obj \
#        $(ODIN32_LIB)\cppopa3.obj
all:     $(ODIN32_LIB)\cpppan40.obj
#!else
#all:
#!endif

# aliases for all...
needed: all
lib: all

# The other targets.
nothing dep libs:

#
# DosLoadModule override.
#
cppopa3_fix2.obj $(ODIN32_LIB)\cppopa3_fix2.obj: cppopa3_fix2.asm
    $(AS) $(ASFLAGS) cppopa3_fix2.asm -Fo:$(ODIN32_LIB)\cppopa3_fix2.obj


#
# Apply fix to the cppopa3.obj module.
#
cppopa3.obj $(ODIN32_LIB)\cppopa3.obj: $(CPPMAIN)\lib\cppopa3.obj
    $(CP) $(CPPMAIN)\lib\cppopa3.obj $(ODIN32_LIB)\cppopa3.obj
    .\prfpatch.cmd $(ODIN32_LIB)\cppopa3.obj


#
# Assemble the cpppan40 module with FS fixes.
#
cpppan40.obj $(ODIN32_LIB)\cpppan40.obj: cppopa4.asm
    @if not exist $(ODIN32_LIB) $(CREATEPATH) $(ODIN32_LIB)
    $(AS) $(ASFLAGS) +Scs cppopa4.asm -Fo:$(ODIN32_LIB)\cpppan40.obj -Fl:cpppan40.lst


#
# Applies the stack size fixes to the .exe and .dll
# to prevent WPS traps.
#
stack_fixes:


#
# Clean rule.
#
clean:
    $(RM) $(ODIN32_LIB)\cppopa3_fix2.obj $(ODIN32_LIB)\cppopa3.obj
