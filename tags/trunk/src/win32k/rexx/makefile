##############################################################################
# $Id: makefile,v 1.1 2000-05-10 16:04:31 bird Exp $
#
# PD-Win32 API
#
#       kRx.exe makefile
#
##############################################################################

##############################################################################
# Include
##############################################################################
EXETARGET=1
!include ..\makefile.inc
!include $(PDWIN32_INCLUDE)\pdwin32.mk

##############################################################################
# Tools and Flags Addjustments
##############################################################################
CINCLUDES  = -I$(WIN32KINCLUDE)

CFLAGS     = $(CINCLUDES) $(CFLAGS) \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+ -Rn
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) \
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
.c{$(OBJDIR)}.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(OBJDIR)\$(@B).obj $<

.cpp{$(OBJDIR)}.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(@) $<

#
# Visual slick edit!
#
.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(OBJDIR)\$(@B).obj $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B).asm -Fo$(OBJDIR)\$(@B).obj $<


##############################################################################
# Main targets.
##############################################################################
TARGET   = kRx

OBJS     =  $(OBJDIR)\kRx.obj


all:      $(OBJDIR) $(OBJDIR)\$(TARGET).exe


$(OBJDIR)\$(TARGET).exe: $(OBJS) $(TARGET).def
    @echo linking $@
    $(LD2) $(LD2FLAGS) /OUT:$@ /MAP:$*.map $** OS2386.lib $(RTLLIB) REXX.LIB
    $(CP) $@ $(PDWIN32_BIN)


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -o$$(OBJDIR) $(CINCLUDES) *.c *.cpp *.h



# Includes the common rules.
!include $(PDWIN32_INCLUDE)\pdwin32.post
