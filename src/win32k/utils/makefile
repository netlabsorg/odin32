##############################################################################
# $Id: makefile,v 1.2 2000-09-02 21:08:23 bird Exp $
#
# PD-Win32 API
#
#       Win32kCC.exe makefile
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
OBJEXT          = obj

##############################################################################
# Tools and Flags Addjustments
##############################################################################
OBJDIR     = $(WIN32KOBJ)
CINCLUDES  = -I$(WIN32KINCLUDE)

CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext- -Gm- -Gn- -Ti+
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 \
             -Ge+ -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gm- -Gn- -Ti+

LD2FLAGS    = $(LD2FLAGS) /pmtype:pm


##############################################################################
# Interference rules. Note: -Fo is IBMCPP specific.
##############################################################################
.c{$(OBJDIR)}.$(OBJEXT):
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fo$@ $<

.cpp{$(OBJDIR)}.$(OBJEXT):
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fo$@ $<

.rc{$(OBJDIR)}.res:
    @echo Compiling resources: $(@B).res
    @$(OS2RC) $(OS2RCFLAGS) $(CINCLUDES:-I=-i ) $< $@


#
# Visual slick edit!
#
.c.obj:
    @echo compiling: $(@B).c
    @$(CC) $(CFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$(OBJDIR)\$(@B).$(OBJEXT) $<

.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) -c -Fa$(WIN32KLIST)\$(@B)_pe.asm -Fo$(OBJDIR)\$(@B).$(OBJEXT) $<


##############################################################################
# Main targets.
##############################################################################
TARGET = Win32kCC

OBJS   = $(OBJDIR)\Win32kCC.$(OBJEXT)

all:    $(WIN32KBIN)\$(TARGET).exe
    $(CP) $(WIN32KBIN)\$(TARGET).exe

$(WIN32KBIN)\$(TARGET).exe: $(OBJS) $(OBJDIR)\$(@B).res $(PDWIN32_LIB)\win32k.lib
    @echo linking $@
    $(LD2) @<<$(OBJDIR)\$(*B).lnk
$(LD2FLAGS)
/OUT:$@
/MAP:$*.map
$(OBJS)
$(RTLLIB)
$(PDWIN32_LIB)\win32k.lib
os2386.lib
<<KEEP
    $(OS2RC) $(OS2RCLFLAGS) $(OBJDIR)\$(@B).res $@
    $(CP) $@ $(PDWIN32_BIN)


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) -obj$(OBJEXT) -o$$(OBJDIR) $(CINCLUDES) *.cpp *.c *.h \
        *.rc *.dlg ..\include\*.h


##############################################################################
# Cleanup
##############################################################################
#clean2:
#    @$(RM) $(OBJS) $(TARGET).exe $(WIN32KLIST)\$(TARGET).map *.pch



# Includes the common rules.
!include $(PDWIN32_INCLUDE)\pdwin32.post
