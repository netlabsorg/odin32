# $Id: makefile.wat,v 1.1 2000-08-29 21:39:34 sandervl Exp $

#===================================================================
#
#   Setup the environment properly
#
#===================================================================
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin
PDWIN32_TOOLS   = ..\..\tools\bin

!include $(PDWIN32_INCLUDE)/pdwin32.wat.mk

# Flag overloads and local macros.
CLEANEXTRAS = advapi32rsrc.asm

#===================================================================
#
#   List of source files
#
#===================================================================
#FILE0 = $(OBJDIR)\advapi32.obj $(OBJDIR)\advapi32rsrc.obj $(OBJDIR)\lsa.obj
#FILE1 = $(OBJDIR)\security.obj $(OBJDIR)\service.obj $(OBJDIR)\eventlog.obj
#FILE2 = $(OBJDIR)\crypt.obj $(PDWIN32_LIB)\dllentry.obj

FILE0 = advapi32.obj rsrcadvapi32.obj lsa.obj
FILE1 = security.obj service.obj eventlog.obj
FILE2 = crypt.obj

OBJS = $(FILE0) $(FILE1) $(FILE2)

OTHEROBJS = $(PDWIN32_LIB)\kernel32.lib &
# VAC++'s dllentry.obj makes wlink crash... leave it out for now
#            $(PDWIN32_LIB)\dllentry.obj
            $(PDWIN32_LIB)\ntdll.lib &
            $(PDWIN32_LIB)\pmwinx.lib

LIBS =      clbrdll.lib &
            os2386.lib

TARGET  = advapi32


#===================================================================
#
#   Specific dependencies
#
#===================================================================
all: $(OBJDIR) $(OBJDIR)\$(TARGET).dll $(PDWIN32_BIN)\$(TARGET).dll lib

$(OBJDIR)\$(TARGET).lnk: makefile.wat
    @%write $^@ name $(OBJDIR)\$(TARGET).dll
    @for %f in ($(OBJS)) do @%append $^@ file $(OBJDIR)\%f
    @for %f in ($(OTHEROBJS)) do @%append $^@ file %f
    @for %f in ($(LIBS)) do @%append $^@ library %f

# use a response file
$(OBJDIR)\$(TARGET).dll: $(OBJDIR)\$(TARGET).lnk $(OBJS)
    $(LD) $(LDFLAGS) @$(OBJDIR)\$(TARGET).lnk

# or don't; the command line might get huge though, so that's
# probably not a great idea
#$(OBJDIR)\$(TARGET).dll: $(OBJS)
#    $(LD) $(LDFLAGS) F {$<} F {$(OTHEROBJS)}

# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.wat.post

