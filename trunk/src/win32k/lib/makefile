##############################################################################
# $Id: makefile,v 1.1 2000-02-26 17:48:23 bird Exp $
#
# PD-Win32 API
#
#       win32k.lib makefile
#
##############################################################################

##############################################################################
# Include
##############################################################################
!include ..\makefile.inc
!include $(PDWIN32_INCLUDE)\pdwin32.mk

##############################################################################
# Tools and Flags Addjustments
##############################################################################
OBJEXT     = obj

CINCLUDES  = -I$(WIN32KINCLUDE) -I$(PDWIN32_INCLUDE)
CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 -DWIN32KLIB \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn


##############################################################################
# Interference rules. Note: -Fo is IBMCPP specific.
##############################################################################
{$(WIN32KLIB)}.c{$(WIN32KOBJ)}.$(OBJEXT):
    @$(ECHO) compiling: $(@B).c
    $(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<

{$(WIN32KLIB)}.cpp.obj:
    @$(ECHO) compiling: $(@B).cpp
    $(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = win32k.lib

OBJS     =  $(WIN32KOBJ)\libInit.$(OBJEXT)         \
            $(WIN32KOBJ)\libTerm.$(OBJEXT)     \
            $(WIN32KOBJ)\libDosAllocMemEx.$(OBJEXT)



all: $(TARGET)

##############################################################################
# win32k.lib rule.
##############################################################################
$(TARGET): $(OBJS)
    @echo making library $@
    -$(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS);
    $(CP) $@ $(PDWIN32_LIB)


##############################################################################
# Cleanup
##############################################################################
clean:
    @$(RM) $(OBJS) $(TARGET) $(PDWIN32_LIB)\$(TARGET) ..\$(TARGET) *.pch


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -obj$(OBJEXT) -o$(WIN32KOBJ) $(CINCLUDES) $(WIN32KBASE)\include\*.h

!ifndef NODEP
!include .depend
!endif

