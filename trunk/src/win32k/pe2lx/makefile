##############################################################################
# $Id: makefile,v 1.20 2000-03-08 09:22:21 bird Exp $
#
# PD-Win32 API
#
#       pe2lx.exe makefile
#
##############################################################################

##############################################################################
# Include
##############################################################################
EXETARGET=1
!include ..\makefile.inc
!include $(PDWIN32_INCLUDE)\pdwin32.mk

##############################################################################
# Object extention
##############################################################################
OBJEXT          = pe_obj


##############################################################################
# Tools and Flags Addjustments
##############################################################################
CINCLUDES  = -I$(WIN32KINCLUDE)

CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+ -Gx -Rn

LD         = ilink
LDFLAGS    = /nologo /NOI /A:16 /NOE /packcode /packdata /pmtype:vio /Stack:4096 \
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
{$(WIN32KMISC)}.c{$(OBJDIR)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$@ $<

{$(WIN32KMISC)}.cpp{$(OBJDIR)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$@ $<

{$(WIN32KLDR)}.cpp{$(OBJDIR)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$@ $<

.cpp{$(OBJDIR)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$@ $<


#
# Visual slick edit!
#
{$(WIN32KMISC)}.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$(OBJDIR)\$(@B).$(OBJEXT) $<

{$(WIN32KMISC)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$(OBJDIR)\$(@B).$(OBJEXT) $<

{$(WIN32KLDR)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$(OBJDIR)\$(@B).$(OBJEXT) $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$(OBJDIR)\$(@B).$(OBJEXT) $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = pe2lx

OBJS     =  $(OBJDIR)\pe2lx.$(OBJEXT)         \
            $(OBJDIR)\pe2lxmain.$(OBJEXT)     \
            $(OBJDIR)\modulebase.$(OBJEXT)    \
            $(OBJDIR)\malloc.$(OBJEXT)        \
            $(OBJDIR)\smalloc_avl.$(OBJEXT)   \
            $(OBJDIR)\avl.$(OBJEXT)           \
            $(OBJDIR)\rmalloc_avl.$(OBJEXT)   \
            $(OBJDIR)\new.$(OBJEXT)           \
            $(OBJDIR)\stricmp.$(OBJEXT)       \
            $(OBJDIR)\vprintf.$(OBJEXT)



all:      $(OBJDIR) $(OBJDIR)\$(TARGET).exe


$(OBJDIR)\$(TARGET).exe: $(OBJS)
    @echo linking $@
    $(LD) $(LDFLAGS) /OUT:$@ /MAP:$*.map $**
    $(CP) $@ $(PDWIN32_BIN)


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -obj$(OBJEXT) -o$$(OBJDIR) $(CINCLUDES) ..\misc\*.c* ..\include\*.h


##############################################################################
# Cleanup
##############################################################################
#clean2:
#    @$(RM) $(OBJS) $(TARGET).exe $(WIN32KLIST)\$(TARGET).map *.pch



# Includes the common rules.
!include $(PDWIN32_INCLUDE)\pdwin32.post
