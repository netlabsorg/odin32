##############################################################################
# $Id: makefile,v 1.1 2000-01-22 00:49:21 bird Exp $
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
ELF2LXOBJ       = ..\object
ELF2LXLIST      = ..\list
WIN32KBASE      = ..
WIN32K_INCLUDE  = $(WIN32KBASE)\include
WIN32K_MISC     = $(WIN32KBASE)\misc
WIN32K_LDR      = $(WIN32KBASE)\ldr
OBJEXT          = elf_obj

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
             /MAP:$(ELF2LXLIST)\$(@B).map /pmtype:vio /Stack:4096 \
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
{$(WIN32K_MISC)}.c{$(ELF2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_MISC)}.cpp{$(ELF2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_LDR)}.cpp{$(ELF2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$@ $<

.cpp{$(ELF2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$@ $<


#
# Visual slick edit!
#
{$(WIN32K_MISC)}.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$(ELF2LXOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32K_MISC)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$(ELF2LXOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32K_LDR)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$(ELF2LXOBJ)\$(@B).$(OBJEXT) $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(ELF2LXLIST)\$(@B).asm -Fo$(ELF2LXOBJ)\$(@B).$(OBJEXT) $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = pe2lx

OBJS     =  $(ELF2LXOBJ)\pe2lx.$(OBJEXT)      \
            $(ELF2LXOBJ)\pe2lxmain.$(OBJEXT)  \
            $(ELF2LXOBJ)\modulebase.$(OBJEXT) \
            $(ELF2LXOBJ)\malloc.$(OBJEXT)     \
            $(ELF2LXOBJ)\new.$(OBJEXT)        \
            $(ELF2LXOBJ)\stricmp.$(OBJEXT)    \
            $(ELF2LXOBJ)\vprintf.$(OBJEXT)

all: ELFDumper.exe #$(TARGET).exe

$(TARGET).exe: $(OBJS)
    @echo linking $@
    $(LD) $(LDFLAGS) $**
    $(CP) $@ $(PDWIN32_BIN)

ELFDumper.exe: ELFDumper.cpp ..\include\elf.h
    $(CXX) $(CXXFLAGS) -Re -Gn- -Fa$(ELF2LXLIST)\ELFDumper.asm \
        -Fo$(ELF2LXOBJ)\ELFDumper.$(OBJEXT) -Fe$@ ELFDumper.cpp


##############################################################################
# Cleanup
##############################################################################
clean:
    @-$(RM) $(OBJS) $(TARGET).exe $(PE2LXLIST)\$(TARGET).map *.pch

