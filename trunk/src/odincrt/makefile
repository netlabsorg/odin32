# $Id: makefile,v 1.30 2002-08-23 23:41:03 bird Exp $

#
# Odin32 API
#
#       odincrt.dll makefile
#

!ifdef BUILD_SETUP_MAK

#
# Setup config
#
PATH_ROOT       = ..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

#
# Just some helper macros.
#
_DBG=
!if "$(BUILD_MODE)" == "DEBUG"
_DBG=d
!endif
!if "$(BUILD_MODE)" == "PROFILE"
_DBG=p
!endif

#
# Target config
#
TARGET_MODE     = DLL
TARGET_NAME     = odincrt$(_DBG)
TARGET_PUBLIC   = 1

!ifdef WITH_KLIB
ALL_DEFINES     = -DWITH_KLIB
!endif

TARGET_OBJS =\
$(PATH_TARGET)\odincrt.$(EXT_OBJ) \
$(PATH_TARGET)\initterm.$(EXT_OBJ) \
!if "$(BUILD_ENV)" != "WAT11C"
! if "$(BUILD_ENV)" == "VAC308"
$(PATH_TARGET)\math64.$(EXT_OBJ) \
! endif
! ifdef WITH_KLIB
$(PATH_LIB)\kHeapDbgVACWrappersR3.lib \
! endif
$(PATH_TARGET)\malloc.$(EXT_OBJ) \
$(PATH_TARGET)\string.$(EXT_OBJ) \
$(PATH_TARGET)\file.$(EXT_OBJ) \
$(PATH_TARGET)\critsect.$(EXT_OBJ) \
$(PATH_TARGET)\interlock.$(EXT_OBJ) \
!else
#$(PATH_WATCOM)\lib386\os2\..
!error watcom is broken!
!endif

TARGET_LIBS = \
!ifdef WITH_KLIB
$(PATH_LIB)\kLibR3.lib \
!endif
!if "$(BUILD_ENV)" != "WAT11C"
$(LIB_OS) \
$(LIB_C_OBJ) \
somtk.$(EXT_LIB) \
!else
clib3r.$(EXT_LIB) plbrdll.$(EXT_LIB) mt7rdll.$(EXT_LIB) \
!error watcom is broken!
!endif

# this mess is for finding getting the right .def file.
!if "$(BUILD_ENV)" == "VAC308"
_TARGET_DEF     = odincrt$(_DBG).def
!endif
!if "$(BUILD_ENV)" == "VAC365"
_TARGET_DEF     = odin36$(_DBG).def
!endif
!if "$(BUILD_ENV)" == "WAT11C"
_TARGET_DEF     = odinwat$(_DBG).def
!endif
!if "$(_TARGET_DEF)" == "" && !$(BUILD_FORWARDING)
!error "not supported on this compiler yet!"
!endif
!ifdef WITH_KLIB
TARGET_DEF      = $(PATH_TARGET)\$(_TARGET_DEF).klib.$(EXT_DEF)
!else
TARGET_DEF      = $(_TARGET_DEF)
!endif

#
# Rules config
#
!ifdef WITH_KLIB
RULES_FORWARD = $(TARGET_DEF)
!endif
!include $(MAKE_INCLUDE_PROCESS)

!if defined(WITH_KLIB) && !$(BUILD_FORWARDING)
# Add kLib export to the def-file.
$(TARGET_DEF): Makefile $(TARGET_DEF_ORG)
    $(TOOL_CP) $(TARGET_DEF_ORG) $@
    $(TOOL_ECHO)     kHeapDbgException           @1500 >> $@
!endif


!else


#
# Tell the buildenvironment not to
#
NOTEXPDEF   = 1

#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Overrides.
#
CXXFLAGS    = $(CXXFLAGS_ODINCRT)
LD2FLAGS    = $(LD2FLAGS_ODINCRT)
!ifdef WITH_KLIB
CDEFINES    = $(CDEFINES_ODINCRT) -DWITH_KLIB
!else
CDEFINES    = $(CDEFINES_ODINCRT)
!endif
IMPLIBFLAGS = $(IMPLIBFLAGS) /NOIgnoreCase

!ifdef DEBUG
DBG=d
!endif

!ifdef PROFILE
DBG=p
!endif

!ifdef RELEASE
DBG=
!endif

# Sorry, this looks like hell now.
!ifdef VAC3
! ifdef WITH_KLIB
DEFFILE_ORG = odincrt$(DBG).def
DEFFILE     = $(OBJDIR)\odincrt$(DBG)klib.def
! else
DEFFILE     = odincrt$(DBG).def
! endif
!endif
!ifdef VAC36
! ifdef WITH_KLIB
DEFFILE_ORG = odin36$(DBG).def
DEFFILE     = $(OBJDIR)\odin36$(DBG)klib.def
! else
DEFFILE     = odin36$(DBG).def
! endif
!endif
!ifdef WAT
! ifdef WITH_KLIB
DEFFILE_ORG = odinwat$(DBG).def
DEFFILE     = $(OBJDIR)\odinwat$(DBG)klib.def
! else
DEFFILE     = odinwat$(DBG).def
! endif
!endif
!ifndef DEFFILE
!error "not supported on this compiler yet."
!endif


#
# Object files. All objects should be prefixed with $(OBJDIR)!
#
OBJS = \
$(OBJDIR)\odincrt.obj \
$(OBJDIR)\initterm.obj \
!ifndef WAT
!ifdef VAC3
$(OBJDIR)\math64.obj \
!endif
!ifdef WITH_KLIB
$(ODIN32_LIB)\kHeapDbgVACWrappersR3.lib \
!endif
$(OBJDIR)\malloc.obj \
$(OBJDIR)\string.obj \
$(OBJDIR)\file.obj \
$(OBJDIR)\critsect.obj \
$(OBJDIR)\interlock.obj \
!else
!ifdef NMAKE
$(WATCOM)\lib386\os2\$(RTLLIB_O)
!else
$(%WATCOM)\lib386\os2\$(RTLLIB_O)
!endif
!endif


#
# Libraries. One space before the '\'.
#
LIBS = \
!ifdef WITH_KLIB
$(ODIN32_LIB)\kLibR3.lib \
!endif
!ifdef WAT
$(RTLLIB_O)
!endif
!ifdef VAC36
$(SOMLIB)
!endif

#
# Target name - name of the dll without extention and path.
#
TARGET = $(ODINCRT)


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)



!ifdef DEFFILE_ORG
# Add kLib export to the def-file.
$(DEFFILE): Makefile $(DEFFILE_ORG)
    $(CP) $(DEFFILE_ORG) $@
    $(ECHO)     kHeapDbgException           @1500 >> $@
!endif


!endif
