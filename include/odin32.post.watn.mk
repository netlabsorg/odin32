# $Id: odin32.post.watn.mk,v 1.4 2002-06-09 15:07:00 bird Exp $

#
# Odin32 API
#
# Common dll makefile rules (must be included at the last line of the makefile)
#
# If ORGTARGET is defined it is used to generate the importlibrary.
#
# Define NOTEXPDEF to remove the $(TARGET).lib and $(TARGET)exp.def rules.
# Define EXETARGET to make an executable. (This also applies to odin32.mk.)
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
# Define ADDITIONAL_ALL to add targets all should depend on.
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

# Sanity check
!ifndef ODIN32_LIB
!   error "ODIN32_LIB is not defined"
!endif ODIN32_LIB
!ifndef ODIN32_TOOLS
!   error "ODIN32_TOOLS is not defined"
!endif ODIN32_TOOLS


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

# Set default DEFFILE if needed. (Required for both DLLs and EXEs!)
!ifndef DEFFILE
DEFFILE = $(ORGTARGET).def
!endif
!ifndef ORGDEFFILE
ORGDEFFILE = $(ORGTARGET).def
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
        $(ADDITIONAL_ALL)
!endif


#
# Dll: Lib rule - build importlibrary (and evt. other libs)
#
!ifndef NO_LIB_RULE
lib:    $(OBJDIR) \
        $(OBJDIR)\$(ORGTARGET).lib \
        $(ODIN32_LIB)\$(ORGTARGET).lib
!endif


#
# Dll: Main target rule - builds the target dll.
#
!ifndef NO_MAIN_RULE
$(OBJDIR)\$(TARGET).$(TARGET_EXTENSION): $(LIBS) $(OBJS) $(OS2RES) $(OBJDIR)\$(TARGET).lrf
    $(CMDQD_WAIT)
    $(LD2) @$(OBJDIR)\$(TARGET).lrf
!ifdef OS2RES
    $(OS2RC) $(OS2RCLFLAGS) $(OS2RES) $@
!endif
#!if !defined(DEBUG) && !defined(NO_LXLITE)
#    $(LXLITE) $@
#!endif
!endif


#
# Dll: Linker file - creates the parameter file passed on to the linker.
#
!ifndef NO_LNKFILE_RULE
$(OBJDIR)\$(TARGET).lrf: $(MAKEFILE) $(OBJDIR)\bldlevel.$(ORGDEFFILE) $(DEFFILE) $(ODIN32_INCLUDE)\odin32.post.watn.mk
    $(RM) $(OBJDIR)\$(TARGET).lrf2 $@
    $(KDEF2WAT) $(OBJDIR)\bldlevel.$(ORGDEFFILE) $@ <<$(OBJDIR)\$(TARGET).lrf2
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
        $(ADDITIONAL_ALL)
!endif


#
# Exe: Lib rule - dummy rule
#
!ifndef NO_LIB_RULE
lib:
!endif


#
# Exe: Main target rule - builds the target exe.
#
!ifndef NO_MAIN_RULE
$(OBJDIR)\$(TARGET).$(TARGET_EXTENSION): $(LIBS) $(OBJS) $(OS2RES) $(OBJDIR)\$(TARGET).lrf
    $(CMDQD_WAIT)
    $(LD2) @$(OBJDIR)\$(TARGET).lrf
!ifdef OS2RES
    $(OS2RC) $(OS2RCLFLAGS) $(OS2RES) $@
!endif
!if !defined(DEBUG) && !defined(NO_LXLITE)
    $(LXLITE) $@
!endif
!endif


#
# Exe: Linker file - creates the parameter file passed on to the linker.
#
!ifndef NO_LNKFILE_RULE
$(OBJDIR)\$(TARGET).lrf: $(MAKEFILE) $(ODIN32_INCLUDE)\odin32.post.watn.mk $(OBJDIR)\bldlevel.$(ORGDEFFILE)
    $(RM) $(OBJDIR)\$(TARGET).lrf2 $@
!ifdef DEFFILE
    $(KDEF2WAT) $(OBJDIR)\bldlevel.$(ORGDEFFILE) $@ <<$(OBJDIR)\$(TARGET).lrf2
$(LD2FLAGS)
name $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION)
option map=$(OBJDIR)\$(TARGET).map
file    {$(OBJS)}
library {$(LIBS)}
<<
!else
    $(ECHO) Creating <<$@
$(LD2FLAGS)
name $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION)
option map=$(OBJDIR)\$(TARGET).map
file    {$(OBJS)}
library {$(LIBS)}
<<
!endif
!endif


!endif # !ifndef EXETARGET
!else # !ifndef LIBTARGET
###############################################################################
# LIB target (internal or public object library)
###############################################################################

#
# Localcleanup is default for internal object libraries.
#
!ifndef PUBLICLIB
LOCALCLEAN = 1
!endif


#
# Lib: All rule - build objs and target library.
#
!ifndef NO_ALL_RULE
all:    $(OBJDIR) \
        $(INTLIBS) \
!ifndef PUBLICLIB
        $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION) \
        $(ADDITIONAL_ALL)
!else
        $(OBJDIR)\$(TARGET)$(CUST).$(TARGET_EXTENSION) \
        $(ODIN32_LIB)\$(TARGET)$(CUST).$(TARGET_EXTENSION) \
        $(ADDITIONAL_ALL)
!endif
!endif


#
# Lib: Lib rule - dummy rule
#
!ifndef NO_LIB_RULE
!ifndef PUBLICLIB
lib:
!else
lib: all
!endif
!endif


#
# Lib: Internal library rule. Same as the all rule.
#
!ifndef NO_INTERNAL_LIBS
libs: all
!endif


#
# Lib: Main target rule - builds the target internal library.
#
!ifndef NO_MAIN_RULE
$(OBJDIR)\$(TARGET)$(CUST).$(TARGET_EXTENSION): $(OBJS) $(ODIN32_INCLUDE)\odin32.post.watn.mk
    $(CMDQD_WAIT)
    $(RM) $@
    $(ILIB) $(ILIBFLAGS) -l = $(OBJDIR)\$(@B).lst $@ @<<$@.parm
+$(OBJS:  =^
+)
<<keep
!endif


!endif # !ifndef LIBTARGET


###############################################################################
# Common rules for both exe and dll, new style
###############################################################################

#
# Common (new): Dep rule - makes depenencies for C, C++ and Asm files.
#
!ifndef NO_DEP_RULE
dep: dep_internal $(ADDITIONAL_DEP)
dep_internal:
    $(DEPEND) $(CINCLUDES) -xdummy.c *.c *.cpp *.h *.asm *.inc *.rc *.dlg -robj *.orc
!ifdef SUBDIRS
    @$(DODIRS) "$(SUBDIRS)"  $(MAKE_CMD) dep
!endif
!endif


#
# Common: Internal library rule. Make libraries in subdirectories.
#   For makefiles with subdirectories is common to have make libraries in subdirectories.
#
!ifndef NO_INTERNAL_LIBS
!ifdef INTLIBS
$(INTLIBS):
    @$(DODIRS) "$(SUBDIRS)"  $(MAKE_CMD) libs
!endif
!endif

!endif # OLD_STYLE

###############################################################################
# Common rules for both exe and dll, old and new style.
###############################################################################

#
# Common: Copy library rule.
#
!ifndef PUBLICLIB
$(ODIN32_LIB)\$(ORGTARGET).lib: $(OBJDIR)\$(ORGTARGET).lib
!else
$(ODIN32_LIB)\$(TARGET)$(CUST).$(TARGET_EXTENSION): $(OBJDIR)\$(TARGET)$(CUST).$(TARGET_EXTENSION)
!endif
    @if not exist $(@D) $(CREATEPATH) $(@D)
    $(CP) $** $@


#
# Common: Copy dll/exe rule.
#
$(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION): $(OBJDIR)\$(TARGET).$(TARGET_EXTENSION)
    @if not exist $(@D) $(CREATEPATH) $(@D)
    $(CP) $** $@
!ifndef NO_MAIN_BIN_COPY
    -$(CP) $** $(@D)..\..\$(@F)
!endif


#
# Common: Copy sym rule.
#
$(ODIN32_BIN)\$(TARGET).sym: $(OBJDIR)\$(TARGET).sym
    @if not exist $(@D) $(CREATEPATH) $(@D)
    $(CP) $** $@
!ifndef NO_MAIN_BIN_COPY
    -$(CP) $** $(@D)..\..\$(@F)
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
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
!else
$(OBJDIR)\$(TARGET).lib: $(DEFFILE)
    $(IMPLIB) $(IMPLIBFLAGS) $@ $**
!endif
!endif


#
# Common: Make the <>exp.def file.
#
!ifndef LIBTARGET
!ifndef NOTEXPDEF
$(OBJDIR)\$(ORGTARGET)exp.def: $(ORGDEFFILE)
    $(IMPDEF) $(IMPDEF_FLAGS) $** $@
!endif
!endif


#
# Common: Make .def-file with buildlevel info.
#
$(OBJDIR)\bldlevel.$(ORGDEFFILE): $(DEFFILE)
    $(BLDLEVELINF) $(DEFFILE) $@ -R"$(DEFFILE)" \
        -V"#define=ODIN32_VERSION,$(ODIN32_INCLUDE)\odinbuild.h" \
        -M"#define=ODIN32_BUILD_NR,$(ODIN32_INCLUDE)\odinbuild.h"


#
# Common: Create the object directory.
#
$(OBJDIR):
    @if not exist $(OBJDIR) $(CREATEPATH) $(OBJDIR)


#
# Common: Generate Visual SlickEdit project.
#
!ifndef NO_VSLICKRULES
!ifdef ORGTARGET
vslick $(ORGTARGET).vpj:
    $(RM) $(ORGTARGET).vtg
    $(ODIN32_BASE)\tools\vslick\genproject.cmd $(CINCLUDES) $(ORGTARGET).vpj
!endif
!endif


#
# Common: General clean rule. To clean more add it to CLEANEXTRAS!
#
!ifndef NOCLEAN
!ifndef CLEAN2
clean:
!else
clean:  clean2
!endif
    $(RM) *.lib *.res *.map *.pch dummy.c \
!if "$(OBJDIR)" != ""
     $(OBJDIR)\* \
!endif
!ifndef LOCALCLEAN
        $(ODIN32_LIB)\$(ORGTARGET).lib \
        $(ORGTARGET)exp.def \
!ifndef PUBLICLIB
        $(ODIN32_BIN)\$(TARGET).$(TARGET_EXTENSION) *.$(TARGET_EXTENSION) \
        $(ODIN32_BIN)\$(TARGET).sym *.sym \
!endif
        $(CLEANEXTRAS)
!else
        $(CLEANEXTRAS)
!endif
!ifdef SUBDIRS
    @$(DODIRS) "$(SUBDIRS)"  $(MAKE_CMD) clean
!endif
!endif


#
# Common: Include the .depend file.
#   If the depend file don't exists we'll complain about it.
#
!ifndef NODEP
!   if [$(EXISTS) .depend] == 0
!       include .depend
!   else
!       if [$(ECHO) .depend doesn't exist]
!       endif
!   endif
!   if [$(EXISTS) $(ODIN32_INCLUDE)\.depend] == 0
!       include $(ODIN32_INCLUDE)\.depend
!   else
!       if [$(ECHO) /include/.depend doesn't exist]
!       endif
!   endif
!endif


#
# Common: If MULTIJOBS is defined make sure cmdqd is running.
#
!ifndef NO_MULTIJOBS
!   ifdef MULTIJOBS
!      if [$(CMDQD) queryrunning] != 0
!          if [$(DODIRS) "$(ODIN32_BASE)\tools\cmdqd" $(MAKE_CMD) all] == 0
!          endif
!          if [$(CMDQD) init 7] == 0
!          endif
!      endif
!   endif
!endif


