# $Id: odin32.post.wat.mk,v 1.3 2000-12-03 01:07:01 bird Exp $

#
# Odin32 API
#
# Common dll makefile rules (must be included at the last line of the makefile)
#
# If ORGTARGET is defined it is used to generate the importlibrary.
#
# Define NOTEXPDEF to remove the $(TARGET).lib and $(TARGET)exp.def rules.
# Define EXETARGET to make an executable. (This also applies to pdwin32.mk.)
# Define LIBTARGET to make an internal library.
# Define LOCALCLEAN if only the local directory is to be clean.
# Define CLEAN2 to invoke a second clean rule named 'clean2'.
# Define NOCLEAN to exclude the clean rule. (Remember to make your own!)
#
# New style: (!defined OLD_STYLE)
# Define NO_ALL_RULE to not make default all rule.
# Define NO_LIB_RULE to not make default lib rule.
# Define NO_MAIN_RULE to not make default main target rule.
# Define NO_LNKFILE_RULE to not make default link-file rule.
# Define NO_DEP_RULE to not make dependencies rule.
# Define NO_CLEAN_RULE to not make clean rule (same as NOCLEAN - use this!).
# Define NO_INTERNAL_LIBS to not include libs rule for subdirectories.
# Define NO_MAIN_BIN_COPY to not copy the target and sym file to the main
#    binary directory. It is only copied to the compiler specific directory.
#    Main bin is /bin. Compiler specific bin is /bin/debug, /bin/debug.vac36, etc.
# Define ADDITIONAL_DEP to add dependencies rules.
#
# Define MAKEFILE if the name of the makefile isn't "makefile".
# Define DEFFILE to specify another file than $(TARGET).def or $(ORGTARGET).def.
#    Define this if your .exe file have a .def file.
# Define OS2RES as the names of the OS/2 .res file to create and link
#    with the target .dll or .exe.
# Define LIBS as a list of all the libraries to link
#    with the target .dll or .exe.
# Define SUBDIRS when you have subdirectories which should be made.
#    dep and lib supports this fully. The rule libs are added, since we assume you
#    makes internal libraries in the subdirectories. The libs rule is one of the
#    dependencies of the main target.
#


!ifndef TARGET_EXTENSION
!   ifdef LIBTARGET
TARGET_EXTENSION=lib
!    else
!       ifdef EXETARGET
TARGET_EXTENSION=exe
!       else
TARGET_EXTENSION=dll
!       endif
!   endif
!endif


!ifndef OLD_STYLE

# Set default MAKEFILE if needed
!ifndef MAKEFILE
MAKEFILE = makefile
!endif

# Set default ORGTARGET if needed.
!ifndef ORGTARGET
ORGTARGET=$(TARGET)
!endif

# Set default DEFFILE if needed. (Currently for dlls only.)
!ifndef DEFFILE
!   ifndef EXETARGET
DEFFILE = $(ORGTARGET).def
!   endif
!endif

# Set INTLIBS (interal) if SUBDIRS is defined and NO_INTERNAL_LIBS is undefined.
!ifdef SUBDIRS
!   ifndef NO_INTERNAL_LIBS
!       ifndef LIBTARGET
INTLIBS = libs
!       else
INTLIBS = libs_sub
!       endif
!   endif
!endif


!ifndef LIBTARGET
!ifndef EXETARGET
###############################################################################
# DLL Target
###############################################################################

#
# Dll: All rule - build objs, target dll, copies target to bin and makes libs.
#
!ifndef NO_ALL_RULE
all:    $(OBJDIR) \
        $(INTLIBS) \
        $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION) \
        $(OBJDIR)\$(TARGET).sym \
        $(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION) \
        $(ODIN32_BIN)\$(TARGET).sym \
        lib \
        .SYMBOLIC
!endif

#
# Dll: Lib rule - build importlibrary (and evt. other libs)
#
!ifndef NO_LIB_RULE
lib:    $(OBJDIR)\$(ORGTARGET).lib \
        $(ODIN32_LIB)\$(ORGTARGET).lib
!endif


#
# Dll: Main target rule - builds the target dll.
#
!ifndef NO_MAIN_RULE
$(OBJDIR)\$(TARGET).$(TARGET_EXTENSION): $(OBJS) $(OS2RES) $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    $(LD2) @$(OBJDIR)\$(TARGET).lrf
!ifdef OS2RES
    $(OS2RC) $(OS2RCLFLAGS) $(OS2RES) $@
!endif
!endif


#
# Dll: Linker file - creates the parameter file passed on to the linker.
#
!ifndef NO_LNKFILE_RULE
$(OBJDIR)\$(TARGET).lrf: makefile $(DEFFILE) $(ODIN32_INCLUDE)\pdwin32.wat.post
    $(RM) $(OBJDIR)\$(TARGET).lrf2 $@
    $(KDEF2WAT) $(DEFFILE) $@ <<$(OBJDIR)\$(TARGET).lrf2
$(LD2FLAGS)
name $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION)
option map=$(OBJDIR)\$(TARGET).map
file    {$(OBJS)}
library {$(LIBS)}
<<
!endif


!else # !ifndef EXETARGET
###############################################################################
# EXE Target
###############################################################################

#
# Exe: All rule - build objs, target exe, copies target to bin.
#
!ifndef NO_ALL_RULE
all:    $(OBJDIR) \
        $(INTLIBS) \
        $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION) \
        $(OBJDIR)\$(TARGET).sym \
        $(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION) \
        $(ODIN32_BIN)\$(TARGET).sym \
        .SYMBOLIC
!endif

#
# Exe: Lib rule - dummy rule
#
!ifndef NO_LIB_RULE
lib: .SYMBOLIC
!endif


#
# Exe: Main target rule - builds the target exe.
#
!ifndef NO_MAIN_RULE
$(OBJDIR)\$(TARGET).$(TARGET_EXTENSION): $(OBJS) $(OS2RES) $(DEFFILE) $(OBJDIR)\$(TARGET).lrf
    $(LD2) @$(OBJDIR)\$(TARGET).lrf
!ifdef OS2RES
    $(OS2RC) $(OS2RCLFLAGS) $(OS2RES) $@
!endif
!endif


#
# Exe: Linker file - creates the parameter file passed on to the linker.
#
!ifndef NO_LNKFILE_RULE
$(OBJDIR)\$(TARGET).lrf: makefile  $(ODIN32_INCLUDE)\pdwin32.wat.post $(DEFFILE)
    $(RM) $(OBJDIR)\$(TARGET).lrf2 $@
!ifdef DEFFILE
    $(KDEF2WAT) $(DEFFILE) $@ <<$(OBJDIR)\$(TARGET).lrf2
!else
    $(ECHO) Creating <<$@
!endif
$(LD2FLAGS)
name $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION)
option map=$(OBJDIR)\$(TARGET).map
file    {$(OBJS)}
library {$(LIBS)}
<<
!endif

!endif # !ifndef EXETARGET !else
!else # !ifndef LIBTARGET
###############################################################################
# LIB target (internal object library)
###############################################################################

#
# Localcleanup is default for internal object libraries.
#
LOCALCLEAN = 1


#
# Lib: All rule - build objs and target library.
#
!ifndef NO_ALL_RULE
all:    $(OBJDIR) \
        $(INTLIBS) \
        $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION) \
        .SYMBOLIC
!endif


#
# Lib: Lib rule - dummy rule
#
!ifndef NO_LIB_RULE
lib: .SYMBOLIC
!endif


#
# Lib: Internal library rule. Same as the all rule.
#
!ifndef NO_INTERNAL_LIBS
libs: all .SYMBOLIC
!endif


#
# Lib: Main target rule - builds the target internal library.
#       (fixme)
!ifndef NO_MAIN_RULE
$(OBJDIR)\$(TARGET).$(TARGET_EXTENSION): $(OBJS)
    $(RM) $@
    $(ILIB) $(ILIBFLAGS) $@ @<<
$(OBJS:  =&^
)
$(OBJDIR)\$^&.lst
<<
!endif


!endif # !ifndef LIBTARGET


###############################################################################
# Common rules for both exe and dll, new style
###############################################################################

#
# Common (new): Dep rule - makes depenencies for C, C++ and Asm files.
#
!ifndef NO_DEP_RULE
dep: dep_internal $(ADDITIONAL_DEP) .SYMBOLIC
dep_internal: .SYMBOLIC
    $(DEPEND) $(CINCLUDES) *.c *.cpp *.h *.asm *.inc \
        *.rc *.dlg $(ODIN32_INCLUDE)\*.h -robj *.orc
!ifdef SUBDIRS
    $(DODIRS) "$(SUBDIRS)"  $(MAKE_CMD) dep
!endif
!endif


#
# Common: Internal library rule. Make libraries in subdirectories.
#   For makefiles with subdirectories is common to have make libraries in subdirectories.
#
!ifndef NO_INTERNAL_LIBS
!ifdef INTLIBS
$(INTLIBS): .SYMBOLIC
    $(DODIRS) "$(SUBDIRS)"  $(MAKE_CMD) libs
!endif
!endif

!endif # OLD_STYLE

###############################################################################
# Common rules for both exe and dll, old and new style.
###############################################################################

#
# Common: Copy library rule.
#
$(ODIN32_LIB)\$(ORGTARGET).lib: $(OBJDIR)\$(ORGTARGET).lib
    @if not exist $^: $(CREATEPATH) $^:
    $(CP) $[@ $@


#
# Common: Copy dll/exe rule.
#
$(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION): $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION)
    @if not exist $^: $(CREATEPATH) $^:
    $(CP) $[@ $^@
!ifndef NO_MAIN_BIN_COPY
    -$(CP) $[@ $^:..\..\$^.
!endif


#
# Common: Copy sym rule.
#
$(ODIN32_BIN)\$(TARGET).sym: $(OBJDIR)\$(TARGET).sym
    @if not exist $^: $(CREATEPATH) $^:
    $(CP) $[@ $@
!ifndef NO_MAIN_BIN_COPY
    -$(CP) $[@ $^:..\..\$^.
!endif


#
# Common: Make sym file rule.
#
$(OBJDIR)\$(TARGET).sym: $(OBJDIR)\$(TARGET).map
    cd $(OBJDIR)
    $(MAPSYM) $(TARGET).map
    cd ..\..


#
# Common: Make library from the <>exp.def or the <>.def file.
#
!ifndef LIBTARGET
!ifndef NOTEXPDEF
$(OBJDIR)\$(ORGTARGET).lib: $(OBJDIR)\$(ORGTARGET)exp.def
    $(IMPLIB) $(IMPLIBFLAGS) $@ $[@
!else
$(OBJDIR)\$(TARGET).lib: $(DEFFILE)
    $(IMPLIB) $(IMPLIBFLAGS) $@ $[@
!endif
!endif


#
# Common: Make the <>exp.def file.
#
!ifndef LIBTARGET
!ifndef NOTEXPDEF
$(OBJDIR)\$(ORGTARGET)exp.def: $(OBJDIR) $(DEFFILE)
    $(IMPDEF) $(DEFFILE) $@
!endif
!endif


#
# Common: Create the object directory.
#
$(OBJDIR): .SYMBOLIC
    @if not exist $(OBJDIR) $(CREATEPATH) $(OBJDIR)


#
# Common: General clean rule. To clean more add it to CLEANEXTRAS!
#
!ifndef NOCLEAN
!ifndef CLEAN2
clean: .SYMBOLIC
!else
clean:  clean2 .SYMBOLIC
!endif
    $(RM) *.lib *.res *.map *.pch \
!if "$(OBJDIR)" != ""
     $(OBJDIR)\* \
!endif
!ifndef LOCALCLEAN
        $(ODIN32_LIB)\$(ORGTARGET).lib \
        $(ORGTARGET)exp.def \
        $(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION) *.$(TARGET_EXTENSION) \
        $(ODIN32_BIN)\$(TARGET).sym *.sym \
        $(CLEANEXTRAS)
!else
        $(CLEANEXTRAS)
!endif
!endif
!ifdef SUBDIRS
    $(DODIRS) "$(SUBDIRS)"  $(MAKE_CMD) clean
!endif


#
# Common: Include the .depend file.
#   If the depend file don't exists we'll complain about it.
#
#!ifndef NODEP
#!   if [$(EXISTS) .depend] == 0
#!       include .depend
#!   else
#!       if [$(ECHO) .depend doesn't exist]
#!       endif
#!   endif
#!endif

