# $Id: makefile,v 1.5 2000-10-05 07:28:39 bird Exp $

#
# PD-Win32 API
#
# Makefile for the ImpDef utility.
#
#


# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_BIN     = ..\..\bin\$(OBJDIR)
PDWIN32_LIB     = ..\..\lib
PDWIN32_TOOLS   = ..\bin
PDWIN32_TCOMMON = ..\common


# Defines for the pdwin32.post and pdwin32.mk
LOCALCLEAN  = 1
CLEANEXTRAS = $(PDWIN32_TOOLS)\impdef.exe *.log
EXETARGET   = 1


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)\pdwin32.mk
!include $(PDWIN32_TCOMMON)\common.mk



# Flag overloads and local macros.
CINCLUDES= -I$(PDWIN32_TCOMMON) -I$(PDWIN32_INCLUDE) -I$(PDWIN32_INCLUDE)\Win
CFLAGS   = $(CFLAGS)   $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft-
CXXFLAGS = $(CXXFLAGS) $(CINCLUDES) -Wall+ppt-ppc-inl-cnv-gnr-vft- -Gx-


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS= \
$(OBJDIR)\ImpDef.obj


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(PDWIN32_TOOLS)\ImpDef.exe


$(OBJDIR)\ImpDef.exe: $(OBJS) $(COMMONLIB)
   $(LD) $(LDFLAGS) -Fe$@ $** $(RTLLIB) os2386.lib

$(PDWIN32_TOOLS)\ImpDef.exe: $(OBJDIR)\ImpDef.exe
   $(CP) $** $@


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) $(CINCLUDES) *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h \
        *.c *.cpp


# Include common rules
!include $(PDWIN32_INCLUDE)\pdwin32.post
!include $(PDWIN32_TCOMMON)\common.mk.post
