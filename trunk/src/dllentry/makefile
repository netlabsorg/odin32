# $Id: makefile,v 1.9 2000-12-17 03:37:33 bird Exp $

#
# Odin32 API
#
#       dllentry library makefile
#


# We have our own rules
NO_ALL_RULE = 1
NO_LIB_RULE = 1
NO_MAIN_RULE = 1
NO_LNK_RULE = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. All objects should be prefixed with $(OBJDIR)!
#
OBJS = \
$(OBJDIR)\dllentry.obj


#
# Target name - name of the obj without extention and path.
#
TARGET = dllentry


#
# All rule - build objs and copies obj to lib.
#
all:     $(OBJDIR) \
         $(ODIN32_LIB) \
         $(ODIN32_LIB)\$(TARGET).obj


#
# Lib rule - same as all.
#
lib: all


# Build objs and copies obj to lib.
$(ODIN32_LIB)\$(TARGET).obj: $(OBJS)
    $(CP) $(OBJS) $@


# Create lib directory.
$(ODIN32_LIB):
    @if not exist $(ODIN32_LIB) $(CREATEPATH) $(ODIN32_LIB)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

