##############################################################################
# $Id: makefile,v 1.2 2000-02-26 20:20:46 bird Exp $
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
             -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Ge+ -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 -DPE2LX \
             -Wall+ppt-ppc-inl-cnv-gnr-vft- -Ge+ -Gm- -Gn- -Ti+ -Gx -Rn


##############################################################################
# Interference rules. Note: -Fo is IBMCPP specific.
##############################################################################
{$(WIN32KLIB)}.c{$(WIN32KOBJ)}.$(OBJEXT):
    @$(ECHO) compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<

{$(WIN32KLIB)}.asm{$(WIN32KOBJ)}.$(OBJEXT):
    @$(ECHO) assmbling: $(@B).c
    $(AS) $(ASFLAGS) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst

{$(WIN32KLIB)}.cpp.obj:
    @$(ECHO) compiling: $(@B).cpp
    $(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = win32k.lib

OBJS     =  $(WIN32KOBJ)\libInit.$(OBJEXT)         \
            $(WIN32KOBJ)\libTerm.$(OBJEXT)     \
            $(WIN32KOBJ)\libDosAllocMemEx.$(OBJEXT) \
            $(WIN32KOBJ)\libGetCS.obj



all: $(TARGET)

##############################################################################
# win32k.lib rule.
##############################################################################
$(TARGET): $(OBJS)
    @$(ECHO) making library $@
    -$(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS);
    $(CP) $@ $(PDWIN32_LIB)

##############################################################################
# libtest.exe - test program for the library.
##############################################################################
libtest.exe: libTest.c $(TARGET)
    @$(ECHO) making $@
    $(CC) $(CFLAGS) -Ge+ -Gn+ -Re -Fe$(WIN32KOBJ)\libTest.obj \
        -Fe$@ $(TARGET) -Fm$(*B).map libTest.c


##############################################################################
# Cleanup
##############################################################################
clean:
    @$(RM) $(OBJS) $(PDWIN32_LIB)\$(TARGET) ..\$(TARGET) *.pch \
        $(WIN32KOBJ)\libTest.obj *.obj *.exe *.lib *.map


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -obj$(OBJEXT) -o$(WIN32KOBJ) $(CINCLUDES) $(WIN32KBASE)\include\*.h

!ifndef NODEP
!include .depend
!endif

