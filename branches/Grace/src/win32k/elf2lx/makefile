##############################################################################
# $Id: makefile,v 1.2 2000-02-27 02:15:35 bird Exp $
#
# PD-Win32 API
#
#       elf2lx.exe makefile
#
##############################################################################

##############################################################################
# Include
##############################################################################
!include ..\makefile.inc
!include $(PDWIN32_INCLUDE)\pdwin32.mk

##############################################################################
# Object extention
##############################################################################
OBJEXT          = elf_obj

##############################################################################
# Tools and Flags Addjustments
##############################################################################
CINCLUDES  = -I$(WIN32KINCLUDE) -I$(PDWIN32_INCLUDE)
CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 -DELF2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 -DELF2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn

LD         = ilink
LDFLAGS    = /nologo /NOI /A:16 /NOE /O:$@ /packcode /packdata \
             /MAP:$(WIN32KLIST)\$(@B).map /pmtype:vio /Stack:4096 \
!ifdef DEBUG
!ifndef NODEBUGINFO
    /debug /dbgpack
!endif
!else
    /exepack:2
!endif


##############################################################################
# Interference rules. Note: -Fo is IBMCPP specific.
##############################################################################
{$(WIN32KMISC)}.c{$(WIN32KOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<

{$(WIN32KMISC)}.cpp{$(WIN32KOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<

{$(WIN32KLDR)}.cpp{$(WIN32KOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<

.cpp{$(WIN32KOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$@ $<


#
# Visual slick edit!
#
{$(WIN32KMISC)}.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(WIN32KOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32KMISC)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(WIN32KOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32KLDR)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(WIN32KOBJ)\$(@B).$(OBJEXT) $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(WIN32KOBJ)\$(@B).$(OBJEXT) $<


##############################################################################
# Main targets.
##############################################################################
TARGET   =  Elf2Lx

OBJS     =  $(WIN32KOBJ)\elf2lx.$(OBJEXT) \
            $(WIN32KOBJ)\elf2lxmain.$(OBJEXT) \
            $(WIN32KOBJ)\modulebase.$(OBJEXT) \
            $(WIN32KOBJ)\malloc.$(OBJEXT) \
            $(WIN32KOBJ)\smalloc_avl.$(OBJEXT) \
            $(WIN32KOBJ)\avl.$(OBJEXT) \
            $(WIN32KOBJ)\rmalloc_avl.$(OBJEXT) \
            $(WIN32KOBJ)\new.$(OBJEXT) \
            $(WIN32KOBJ)\stricmp.$(OBJEXT) \
            $(WIN32KOBJ)\vprintf.$(OBJEXT)

all: ELFDumper.exe $(TARGET).exe

$(TARGET).exe: $(OBJS)
    @echo linking $@
    $(LD) $(LDFLAGS) $**
    $(CP) $@ $(PDWIN32_BIN)

ELFDumper.exe: ELFDumper.cpp ..\include\elf.h
    $(CXX) $(CXXFLAGS) -Re -Gn- -Fa$(WIN32KLIST)\ELFDumper.asm \
        -Fo$(WIN32KOBJ)\ELFDumper.$(OBJEXT) -Fe$@ ELFDumper.cpp


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -obj$(OBJEXT) -o..\Object $(CINCLUDES) ..\misc\*.c* ..\include\*.h

!ifndef NODEP
!include .depend
!endif

##############################################################################
# Cleanup
##############################################################################
clean:
    @-$(RM) $(OBJS) $(TARGET).exe $(PE2LXLIST)\$(TARGET).map *.pch

