##############################################################################
# $Id: makefile,v 1.19 2000-02-27 02:18:42 bird Exp $
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
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\bin
PDWIN32_TOOLS   = ..\..\..\tools\bin
PE2LXOBJ        = ..\object
PE2LXLIST       = ..\list
WIN32KBASE      = ..
WIN32K_INCLUDE  = $(WIN32KBASE)\include
WIN32K_MISC     = $(WIN32KBASE)\misc
WIN32K_LDR      = $(WIN32KBASE)\ldr
OBJEXT          = pe_obj

##############################################################################
# Include
##############################################################################
!include $(PDWIN32_INCLUDE)\pdwin32.mk



##############################################################################
# Tools and Flags Addjustments
##############################################################################
CINCLUDES  = -I$(WIN32K_INCLUDE) -I$(PDWIN32_INCLUDE)
CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn

LD         = ilink
LDFLAGS    = /nologo /NOI /A:16 /NOE /O:$@ /packcode /packdata \
             /MAP:$(PE2LXLIST)\$(@B).map /pmtype:vio /Stack:4096 \
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
{$(WIN32K_MISC)}.c{$(PE2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_MISC)}.cpp{$(PE2LXOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(PE2LXLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_LDR)}.cpp{$(PE2LXOBJ)}.$(OBJEXT):
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

{$(WIN32K_LDR)}.cpp.obj:
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
            $(PE2LXOBJ)\modulebase.$(OBJEXT)    \
            $(PE2LXOBJ)\malloc.$(OBJEXT)        \
            $(PE2LXOBJ)\smalloc_avl.$(OBJEXT)   \
            $(PE2LXOBJ)\avl.$(OBJEXT)           \
            $(PE2LXOBJ)\rmalloc_avl.$(OBJEXT)   \
            $(PE2LXOBJ)\new.$(OBJEXT)           \
            $(PE2LXOBJ)\stricmp.$(OBJEXT)       \
            $(PE2LXOBJ)\vprintf.$(OBJEXT)



all: $(TARGET).exe

$(TARGET).exe: $(OBJS)
    @echo linking $@
    $(LD) $(LDFLAGS) $**
    $(CP) $@ $(PDWIN32_BIN)


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
    @$(RM) $(OBJS) $(TARGET).exe $(PE2LXLIST)\$(TARGET).map *.pch

