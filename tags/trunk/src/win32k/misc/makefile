##############################################################################
# $Id: makefile,v 1.2 2000-12-02 23:32:40 bird Exp $
#
# Odin32 API
#
#       heaptest.exe makefile
#
##############################################################################

!error "don't think this makefile is up-to-date any longer."

##############################################################################
# Directories
##############################################################################
ODIN32_INCLUDE = ..\..\..\include
ODIN32_LIB     = ..\..\..\lib
ODIN32_BIN     = ..\..\..\bin
HEAPTESTOBJ     = ..\object
HEAPTESTLIST    = ..\list
WIN32KBASE      = ..
WIN32K_INCLUDE  = $(WIN32KBASE)\include
WIN32K_MISC     = $(WIN32KBASE)\misc
WIN32K_LDR      = $(WIN32KBASE)\ldr
OBJEXT = t1_obj

##############################################################################
# Include
##############################################################################
OLD_STYLE=1
!include ../../../makefile.inc


##############################################################################
# Tools and Flags Addjustments
##############################################################################
CFLAGS     = $(CFLAGS)   -DRING3 -I$(ODIN32_INCLUDE) -I$(WIN32K_INCLUDE) -DHEAPTEST \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-trd- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CXXFLAGS) -DRING3 -I$(ODIN32_INCLUDE) -I$(WIN32K_INCLUDE) -DHEAPTEST \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn

LD         = ilink
LDFLAGS    = /nologo /NOI /A:16 /NOE /O:$@ /packcode /packdata \
             /MAP:$(HEAPTESTLIST)\$(@B).map /pmtype:vio /Stack:4096 \
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
{$(WIN32K_MISC)}.c{$(HEAPTESTOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_MISC)}.cpp{$(HEAPTESTOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$@ $<

{$(WIN32K_LDR)}.cpp{$(HEAPTESTOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$@ $<

.cpp{$(HEAPTESTOBJ)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$@ $<


#
# Visual Slickedit!
#
{$(WIN32K_MISC)}.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$(HEAPTESTOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32K_MISC)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$(HEAPTESTOBJ)\$(@B).$(OBJEXT) $<

{$(WIN32K_LDR)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$(HEAPTESTOBJ)\$(@B).$(OBJEXT) $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(HEAPTESTLIST)\$(@B).asm -Fo$(HEAPTESTOBJ)\$(@B).$(OBJEXT) $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = heaptest

OBJS     =  $(HEAPTESTOBJ)\heaptest.$(OBJEXT)      \
            $(HEAPTESTOBJ)\rmalloc_avl.$(OBJEXT)     \
            $(HEAPTESTOBJ)\smalloc_avl.$(OBJEXT)     \
            $(HEAPTESTOBJ)\avl.$(OBJEXT)


all: $(TARGET).exe

$(TARGET).exe: $(OBJS)
    @echo linking $@
    $(LD) $(LDFLAGS) $** $(RTLLIB)
    $(CP) $@ $(ODIN32_BIN)


##############################################################################
# Cleanup
##############################################################################
clean:
    @-$(RM) $(OBJS) $(TARGET).exe $(HEAPTESTLIST)\$(TARGET).map *.pch

