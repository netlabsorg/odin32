# $Id: odin32.post.mk,v 1.9 2003-02-28 10:28:10 sandervl Exp $

#
# Odin32 API
#
# Common dll makefile rules (must be included at the last line of the makefile)
#
# If ORGTARGET is defined it is used to generate the importlibrary.
#
# Define NOTEXPDEF to remove the $(TARGET).lib and $(TARGET)exp.def rules.
# Define EXETARGET to make an executable. (This also applies to pdwin32.mk.)
# Define LOCALCLEAN if only the local directory is to be clean.
# Define CLEAN2 to invoke a second clean rule named 'clean2'.
# Define NOCLEAN to exclude the clean rule. (Remember to make your own!)
#
# New style: (define NEW_STYLE)
# Define NO_ALL_RULE to not make default all rule.
# Define NO_LIB_RULE to not make default lib rule.
# Define NO_DLL_RULE to not make default dll rule.
# Define NO_EXE_RULE to not make default exe rule.
# Define NO_LNKFILE_RULE to not make default link-file rule.
# Define NO_DEP_RULE to not make dependencies rule.
# Define NO_CLEAN_RULE to not make clean rule (same as NOCLEAN - use this!).
#


#
# Altern configuration if we're making the custom build object library.
#
!if "$(CUSTOMBUILD)" == "1"
OBJS = $(OBJS:initterm.obj=dummy.obj)
!endif


#
# Include compiler spesific post rule file.
#
!include $(ODIN32_INCLUDE)/odin32.post.$(MKFILE).mk


#
# dummy.obj helper rule.
#
$(OBJDIR)\dummy.obj: dummy.c

dummy.c:
    echo Creating dummy file: <<$@
/* dummy file */
<<KEEP


#
# Dummy .lib dependencies.
#
OS2386.LIB \
mmpm2.lib \
so32dll.lib \
tcp32dll.lib \
rexx.lib \
netapi32.lib \
netapi.lib \
$(SOMLIB) \
$(RTLLIB) \
$(RTLLIB_O) \
$(RTLLIB_NRE) \
$(LIB_DEMANGLE) \
    :


#
# Nothing rule used for makefile debugging.
#
nothing:
    @echo Did nothing with makefile $(MAKEDIR)\$(MAKEFILE)
!if "$(SUBDIRS)" != ""
	$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) $@
!endif
	
	
#
# Common: Global clean rule.
# @todo Make separate define for this recursive delete.
#
cleanall:
    $(RM) -Rf bin .depend
!if "$(SUBDIRS)" != ""
	$(DODIRS) "$(SUBDIRS)" $(MAKE_CMD) NODEP=1 $@
!endif

