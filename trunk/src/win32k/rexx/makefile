##############################################################################
# $Id: makefile,v 1.2 2000-06-03 03:50:46 bird Exp $
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
LDFLAGS    = /nologo /NOI /A:16 /NOE /packcode /packdata /pmtype:vio /Stack:0x10000 \
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

.asm{$(OBJDIR)}.obj:
    @$(ECHO) assembling:      $<
    @$(AS) $(ASFLAGS) $(ADEFINES) $(AINCLUDES) $< -Fo:$@ -Fl:$(WIN32KLIST)\$(*B).lst

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

OBJS     =  $(OBJDIR)\kRxa.obj


all:        $(OBJDIR) \
            $(OBJDIR)\$(TARGET).exe


$(OBJDIR)\$(TARGET).exe: $(OBJS) $(TARGET).def
    @echo linking $@
    -4 $(LD) $(LDFLAGS) /BASE:0x10000 /OUT:$@ /MAP:$*.map $** OS2386.lib REXX.lib
    $(CP) $@ $(PDWIN32_BIN)

$(OBJDIR)\$(TARGET)c.exe: $(OBJDIR)\kRx.obj $(TARGET).def
    @echo linking $@
    $(LD) $(LDFLAGS) /BASE:0x10000 /OUT:$@ /MAP:$*.map $** OS2386.lib REXX.lib
    $(CP) $@ $(PDWIN32_BIN)



##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -o$$(OBJDIR) $(CINCLUDES) *.c *.cpp *.h



# Includes the common rules.
!include $(PDWIN32_INCLUDE)\pdwin32.post
