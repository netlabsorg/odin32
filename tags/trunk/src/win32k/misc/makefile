##############################################################################
# $Id: makefile,v 1.1 2000-01-24 18:19:00 bird Exp $
#
# PD-Win32 API
#
#       heaptest.exe makefile
#
##############################################################################

##############################################################################
# Directories
##############################################################################
PDWIN32_INCLUDE = ..\..\..\include
PDWIN32_LIB     = ..\..\..\lib
PDWIN32_BIN     = ..\..\..\bin
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
!include $(PDWIN32_INCLUDE)\pdwin32.mk


##############################################################################
# Tools and Flags Addjustments
##############################################################################
CFLAGS     = $(CFLAGS)   -DRING3 -I$(PDWIN32_INCLUDE) -I$(WIN32K_INCLUDE) -DHEAPTEST \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-trd- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CXXFLAGS) -DRING3 -I$(PDWIN32_INCLUDE) -I$(WIN32K_INCLUDE) -DHEAPTEST \
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
    $(CP) $@ $(PDWIN32_BIN)


##############################################################################
# Cleanup
##############################################################################
clean:
    @-$(RM) $(OBJS) $(TARGET).exe $(HEAPTESTLIST)\$(TARGET).map *.pch

