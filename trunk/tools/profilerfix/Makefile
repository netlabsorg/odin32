# $Id: Makefile,v 1.1 2002-04-11 21:15:28 bird Exp $

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
all:    $(ODIN32_LIB)\cppopa3_fix2.obj \
        $(ODIN32_LIB)\cppopa3.obj

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
# Applies the stack size fixes to the .exe and .dll
# to prevent WPS traps.
#
stack_fixes:


#
# Clean rule.
#
clean:
    $(RM) $(ODIN32_LIB)\cppopa3_fix2.obj $(ODIN32_LIB)\cppopa3.obj
