##############################################################################
# $Id: makefile,v 1.7 2000-11-09 02:44:27 bird Exp $
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
            $(WIN32KOBJ)\libWin32kInstalled.$(OBJEXT) \
            $(WIN32KOBJ)\libWin32kQueryOptionsStatus.$(OBJEXT) \
            $(WIN32KOBJ)\libWin32kSetOptions.$(OBJEXT) \
            $(WIN32KOBJ)\libDosAllocMemEx.$(OBJEXT) \
            $(WIN32KOBJ)\libW32kQueryOTEs.$(OBJEXT) \
            $(WIN32KOBJ)\libW32kProcessReadWrite.$(OBJEXT) \
            $(WIN32KOBJ)\libGetCS.obj



all:    $(TARGET) \
        $(PDWIN32_LIB)\$(TARGET)

##############################################################################
# win32k.lib rule.
##############################################################################
$(TARGET): $(OBJS)
    @$(ECHO) making library $@
    -$(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ $(OBJS);

$(PDWIN32_LIB)\$(TARGET): $(TARGET)
    $(CP) $(TARGET) $(PDWIN32_LIB)

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
    $(DEPEND) @<<
        -obj$(OBJEXT) -o$(WIN32KOBJ) $(CINCLUDES)
        $(WIN32KBASE)\include\*.h *.c* *.asm
<<

!if [$(EXISTS) .depend] == 0
!   include .depend
!else
!   if [$(ECHO) .depend doesn't exist]
!   endif
!endif

