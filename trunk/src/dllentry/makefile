# $Id: makefile,v 1.5 2000-09-28 03:19:53 bird Exp $

#
# Odin32 API
#
#       dllentry library makefile
#


# For the time being
NEW_STYLE = 1


# We have our own rules
NO_ALL_RULE = 1
NO_LIB_RULE = 1
NO_DLL_RULE = 1
NO_LNK_RULE = 1


#
# Compiler, tools, and interference rules.
#
!include ../../include/pdwin32.mk


#
# Object files. All objects should be prefixed with $(OBJDIR)!
#
OBJS = \
$(OBJDIR)\dllentry.obj


#
# Target name - name of the obj without extention and path.
#  NB. Watcom will need it's own target...
#
!ifndef WAT
TARGET = dllentry
!else
TARGET = dllentry_watcom
!endif


#
# All rule - build objs and copies obj to lib.
#
all: $(OBJDIR) $(PDWIN32_LIB)\$(TARGET).obj


#
# Lib rule - same as all.
#
lib: all


# Build objs and copies obj to lib.
$(PDWIN32_LIB)\$(TARGET).obj: $(OBJS)
    $(CP) $(OBJS) $@


#
# Includes the common rules.
#
!include $(PDWIN32_INCLUDE)/pdwin32.post

