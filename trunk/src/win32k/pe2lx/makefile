##############################################################################
# $Id: makefile,v 1.5 1999-10-27 02:03:01 bird Exp $
#
# PD-Win32 API
#
#       pe2lx.exe makefile
#
##############################################################################

##############################################################################
# Directories
##############################################################################
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_BIN     = ..\..\..\bin
PE2LXOBJ        = ..\object
PE2LXLIST       = ..\list
WIN32KBASE      = ..
WIN32K_INCLUDE  = $(WIN32KBASE)\include
WIN32K_MISC     = $(WIN32KBASE)\misc
OBJEXT = pe_obj

##############################################################################
# Include
##############################################################################
!include $(PDWIN32_INCLUDE)\pdwin32.mk


##############################################################################
# Tools and Flags Addjustments
##############################################################################
CFLAGS     = $(CFLAGS)   -DRING3 -I$(PDWIN32_INCLUDE) -I$(WIN32K_INCLUDE) \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CXXFLAGS) -DRING3 -I$(PDWIN32_INCLUDE) -I$(WIN32K_INCLUDE) \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn

LD         = ilink
LDFLAGS    = /nologo /NOI /A:16 /NOE /O:$@ /packcode /packdata \
             /MAP:$(PE2LXLIST)\$(@B).map /pmtype:vio /Stack:4096 \
!ifdef DEBUG
    /debug /dbgpack
!else
    /exepack:2
!endif


##############################################################################
# Interference rules. Note: -Fo is IBMCPP specific.
##############################################################################
{$(WIN32K_MISC)}.c{$(PE2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_MISC)}.cpp{$(PE2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$@ $<

.cpp{$(PE2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$@ $<


#
# Visual slick edit!
#
{$(WIN32K_MISC)}.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$(PE2LXOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32K_MISC)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$(PE2LXOBJ)\$(@B).$(OBJEXT) $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$(PE2LXOBJ)\$(@B).$(OBJEXT) $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = pe2lx

OBJS     =  $(PE2LXOBJ)\pe2lx.$(OBJEXT)         \
            $(PE2LXOBJ)\pe2lxmain.$(OBJEXT)     \
            $(PE2LXOBJ)\malloc.$(OBJEXT)        \
            $(PE2LXOBJ)\new.$(OBJEXT)           \
            $(PE2LXOBJ)\stricmp.$(OBJEXT)       \
            $(PE2LXOBJ)\vprintf.$(OBJEXT)
#            $(PE2LXOBJ)\vsprintf.$(OBJEXT)



all: $(TARGET).exe

$(TARGET).exe: $(OBJS)
    @echo linking $@
    $(LD) $(LDFLAGS) $**
    $(CP) $@ $(PDWIN32_BIN)


##############################################################################
# Cleanup
##############################################################################
clean:
    @-$(RM) $(OBJS) $(TARGET).exe $(PE2LXLIST)\$(TARGET).map *.pch


##############################################################################
# dependencies by hand.
##############################################################################
$(PE2LXOBJ)\pe2lx.$(OBJEXT):     \
    pe2lx.cpp \
    $(WIN32K_INCLUDE)\pe2lx.h \
    $(WIN32K_INCLUDE)\OS2Krnl.h \
    $(WIN32K_INCLUDE)\malloc.h \
    $(WIN32K_INCLUDE)\new.h

$(PE2LXOBJ)\pe2lxmain.$(OBJEXT): \
    pe2lxmain.cpp \
    $(WIN32K_INCLUDE)\pe2lx.h \
    $(WIN32K_INCLUDE)\OS2Krnl.h

$(PE2LXOBJ)\malloc.$(OBJEXT): \
    $(WIN32K_MISC)\malloc.c \
    $(WIN32K_INCLUDE)\log.h \
    $(WIN32K_INCLUDE)\malloc.h

$(PE2LXOBJ)\new.$(OBJEXT): \
    $(WIN32K_MISC)\new.cpp \
    $(WIN32K_INCLUDE)\new.h \
    $(WIN32K_INCLUDE)\malloc.h \
    $(WIN32K_INCLUDE)\log.h

$(PE2LXOBJ)\vprintf.$(OBJEXT): \
    $(WIN32K_MISC)\vprintf.c \
    $(WIN32K_INCLUDE)\vprintf.h

$(PE2LXOBJ)\stricmp.$(OBJEXT): \
    $(WIN32K_MISC)\stricmp.c

