# $Id: Makefile,v 1.1.2.2 2001-09-20 03:55:26 bird Exp $

#
# Odin32 API
#
#   Makefile for the Quick-and-Dirty dependency utility. (FastDep)
#
#


#
# Directory macro.
#
ODIN32_BIN     = $(ODIN32_TOOLS)


#
# Tell buildenvironment that we're making an VIO .exe.
# Tell buildenvironment that we like to use static linked CRT.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET = 1
VIO = 1
STATIC_CRT = 1
NO_MAIN_BIN_COPY = 1


# Subdirectories.
SUBDIRS = \
.\gc


#
# include common definitions
#
!include ../../../makefile.inc


#
# Addjust common definitions
#
CINCLUDES = -Igc
!ifdef DEBUG
CDEFINES = -DOS2 -DOPERATOR_NEW_ARRAY -DSDS -DDEBUG
!else
CDEFINES = -DOS2 -DOPERATOR_NEW_ARRAY -DSDS
!endif
!if "$(VAC3)" == "1" || "$(VAC36)" == "1"
!ifdef DEBUG
CXXFLAGS = $(CXXFLAGS) -Tm-
!endif
!endif

#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS =\
$(OBJDIR)\main-con.obj \
$(OBJDIR)\buffer.obj \
$(OBJDIR)\cbodywalk.obj \
$(OBJDIR)\classwalk.obj \
$(OBJDIR)\driver.obj \
$(OBJDIR)\driver2.obj \
$(OBJDIR)\encoding.obj \
$(OBJDIR)\env.obj \
$(OBJDIR)\hash.obj \
$(OBJDIR)\member.obj \
$(OBJDIR)\metaclass.obj \
$(OBJDIR)\mop.obj \
$(OBJDIR)\parse.obj \
$(OBJDIR)\pattern.obj \
$(OBJDIR)\ptree-core.obj \
$(OBJDIR)\ptree.obj \
$(OBJDIR)\quote-class.obj \
$(OBJDIR)\token.obj \
$(OBJDIR)\typeinfo.obj \
$(OBJDIR)\walker.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
gc\$(OBJDIR)\gc.lib\
$(RTLLIB) \
os2386.lib \
tcp32dll.lib


#
# Target name - name of the exe without extention and path.
#
TARGET = occ


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)


#
# We need all.
#
needed: all

