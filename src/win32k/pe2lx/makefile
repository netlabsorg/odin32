# $Id: makefile,v 1.25.2.1 2001-10-14 22:59:29 bird Exp $

#
# Odin32 API
#
#       pe2lx.exe makefile
#


#
# Tell buildenvironment that we're making an vio exe and to invoke additional dep rule
#
VIO=1
EXETARGET=1
ADDITIONAL_DEP = mydep


#
# Compiler, tools, and interference rules thru the win32k makefile.inc file.
#
!include ..\..\..\makefile.inc
!include ..\makefile.inc


#
# Sanity check
#
!ifndef WIN32KINCLUDE
!error "Fatal error! WIN32KINCLUDE is undefined!"
!endif


#
# Tools and Flags Addjustments
#
CINCLUDES   = -I$(WIN32KINCLUDE) -I$(KKRNLLIBINCLUDE) $(CINCLUDES)
CDEFINES    = $(CDEFINES) -DRING3 -DPE2LX
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
CFLAGS      = $(CFLAGS)   -Ge+ -Gm- -Gn- -Ti+ -Rn     -Wall+ppt-ppc-inl-cnv-gnr-vft-gen-uni-ext-
CXXFLAGS    = $(CXXFLAGS) -Ge+ -Gm- -Gn- -Ti+ -Rn -Gx -Wall+ppt-ppc-inl-cnv-gnr-vft-
CLISTING    = -Fa$(OBJDIR)\$(@B).s
COBJOUT     = -Fo$(OBJDIR)\$(@B).obj
LD2FLAGS    = $(LD2FLAGS) /A:16 /Stack:4096
!else
!error  "Compiler is not yet supported."
!endif


#
# Interference rules.
#
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
{$(WIN32KMISC)}.c.obj:
    @echo compiling: $(@B).c
    @$(CC)  $(CFLAGS)   $(CINCLUDES) $(CDEFINES) -c $(CLISTING) $(COBJOUT) $<

{$(WIN32KMISC)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -c $(CLISTING) $(COBJOUT) $<

{$(WIN32KLDR)}.cpp.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -c $(CLISTING) $(COBJOUT) $<

.cpp{$(OBJDIR)}.obj:
    @echo compiling: $(@B).cpp
    @$(CXX) $(CXXFLAGS) $(CINCLUDES) $(CDEFINES) -c $(CLISTING) $(COBJOUT) $<
!else
!error  "Compiler is not yet supported."
!endif


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS =\
$(OBJDIR)\pe2lx.obj\
$(OBJDIR)\pe2lxmain.obj\
$(OBJDIR)\modulebase.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)\kKrnlLibR3.lib \
$(RTLLIB_NRE) \
os2386.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = pe2lx


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# Additional dependencies.
#
mydep:
    $(DEPEND) @<<
        -a -o$$(OBJDIR) $(CINCLUDES)
        $(WIN32KMISC)\*.c*
        $(WIN32KLDR)\*.c*
        $(WIN32KINCLUDE)\*.h
<<

