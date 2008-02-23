# $Id: makefile,v 1.11 2005-03-06 10:36:01 sao2l02 Exp $

#
# Odin32 API
#
#       support libraries makefile
#


#
# Include makefile.
#
!include ../makefile.inc


#
# All rule.
#
all:    $(ODIN32_LIB) \
        $(ODIN32_LIB)\pmwinx.lib \
        $(ODIN32_LIB)\libconv.lib \
        $(ODIN32_LIB)\libuls.lib \
!if !defined(DEBUG) && defined(PROFILE)
        $(ODIN32_LIB)\kProfile.lib \
!endif
        $(ODIN32_LIB)\wgss50.lib

#
# lib is all.
#
lib: all


#
# Clean all rule.
#		
cleanall:
	$(RM) -Rf 	Debug Release Profile \
				Debug.vac36 Release.vac36 Profile.vac36 \
				Debug.wat Release.wat Profile.wat \
				Debug.emx Release.emx Profile.emx

#
# Make output director.
#
$(ODIN32_LIB):
    @if not exist $(ODIN32_LIB) mkdir $(ODIN32_LIB)


#
# Make our own pmwinx (Open32) import library.
#
$(ODIN32_LIB)\pmwinx.lib: pmwinx.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\pmwinx.lib pmwinx.def


#
# Make our own pmwinx (Open32) import library.
#
$(ODIN32_LIB)\wgss50.lib: wgss50.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\wgss50.lib wgss50.def


#
# Make our own libconv (UNICODE) import library.
#
$(ODIN32_LIB)\libconv.lib: libconv.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\libconv.lib libconv.def


#
# Make our own libuls (UNICODE) import library.
#
$(ODIN32_LIB)\libuls.lib: libuls.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\libuls.lib libuls.def


#
# Make our profiler import library.
#
$(ODIN32_LIB)\kProfile.lib: kProfile.def
    $(IMPLIB) $(IMPLIBFLAGS) $(ODIN32_LIB)\kProfile.lib kProfile.def


#
# Make our own libuls import library.
#
clean:
    $(RM) $(ODIN32_LIB)\*.lib


#
# Dummy rules.
#
dep cleandlls:


#
# Nothing rule used for makefile debugging.
#
nothing:
    @echo Did nothing with makefile $(MAKEDIR)\$(MAKEFILE)

