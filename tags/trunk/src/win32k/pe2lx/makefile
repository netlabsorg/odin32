##############################################################################
# $Id: makefile,v 1.23 2000-10-17 12:07:23 bird Exp $
#
# PD-Win32 API
#
#       pe2lx.exe makefile
#
##############################################################################

##############################################################################
# Include
##############################################################################
NOCLEAN=1
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
OBJDIR     = $(WIN32KOBJ)
CINCLUDES  = -I$(WIN32KINCLUDE)

CFLAGS     = $(CINCLUDES) $(CFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Gm- -Gn- -Ti+ -Rn -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CXXFLAGS   = $(CINCLUDES) $(CXXFLAGS) -DRING3 -DPE2LX \
             -Ge+ -Gm- -Gn- -Ti+ -Rn -Gx -Wall+ppt-ppc-inl-cnv-gnr-vft-

LD2FLAGS   = /nologo /NOI /A:16 /NOE /NOD /packcode /packdata /pmtype:vio /Stack:4096 \
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
TARGET = pe2lx

OBJS =\
$(OBJDIR)\pe2lx.$(OBJEXT)\
$(OBJDIR)\pe2lxmain.$(OBJEXT)\
$(OBJDIR)\modulebase.$(OBJEXT)\
$(OBJDIR)\malloc.$(OBJEXT)\
$(OBJDIR)\smalloc_avl.$(OBJEXT)\
$(OBJDIR)\avl.$(OBJEXT)\
$(OBJDIR)\rmalloc_avl.$(OBJEXT)\
$(OBJDIR)\new.$(OBJEXT)\
$(OBJDIR)\stricmp.$(OBJEXT)\
$(OBJDIR)\vprintf.$(OBJEXT)

LIBS =\
!if "$(CCENV)" == "VAC36"
    $(VACPATH)\lib\cpprni36.lib \
!else
    $(VACPATH)\lib\cppon30.lib  \
!endif


all:    $(WIN32KBIN)\$(TARGET).exe


$(WIN32KBIN)\$(TARGET).exe: $(OBJS) makefile
    @echo linking $@
    $(LD2) @<<$(OBJDIR)\$(@B).lnk
$(LD2FLAGS)
/OUT:$@
/MAP:$*.map
$(OBJS: =^
)
$(LIBS)
os2386.lib
<<KEEP
    $(CP) $@ $(PDWIN32_BIN)


##############################################################################
# Dependencies.
##############################################################################
dep:
    $(DEPEND) @<<
        -obj$(OBJEXT) -o$$(OBJDIR)
        $(CINCLUDES) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win
        *.c* ..\misc\*.c* ..\include\*.h
<<


##############################################################################
# Cleanup
##############################################################################
clean:
    @$(RM) $(OBJDIR)\*.$(OBJEXT) $(WIN32KBIN)\$(TARGET).exe \
        $(WIN32KBIN)\$(TARGET).lnk $(WIN32KBIN)\$(TARGET).map


# Includes the common rules.
!include $(PDWIN32_INCLUDE)\pdwin32.post
