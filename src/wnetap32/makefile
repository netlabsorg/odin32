# $Id: makefile,v 1.19 2001-09-07 08:20:58 phaller Exp $

#
# Odin32 API
#
#       wnetap32.dll makefile
#


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\wnetap32.obj \
$(OBJDIR)\group.obj \
$(OBJDIR)\server.obj \
$(OBJDIR)\user.obj \
$(OBJDIR)\use.obj \
$(OBJDIR)\service.obj \
$(OBJDIR)\share.obj \
$(OBJDIR)\wksta.obj \
$(OBJDIR)\repl.obj \
$(OBJDIR)\schedule.obj \
$(OBJDIR)\mem.obj \
$(OBJDIR)\errorlog.obj \
$(OBJDIR)\oslibnet.obj \
$(OBJDIR)\netapi32rsrc.obj \
$(DLLENTRY)


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)/kernel32.lib \
$(ODIN32_LIB)/user32.lib \
$(ODIN32_LIB)/$(ODINCRT).lib \
OS2386.LIB \
$(RTLLIB_O)


#
# Target and original target names - names of the dll without extention and path
#
TARGET      = wnetap32
ORGTARGET   = netapi32


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

