# $Id: makefile,v 1.2 2000-12-02 23:40:43 bird Exp $

#
# Odin32 API
#
#       ole32.dll makefile
#



# Compiler, tools, and interference rules.
OLD_STYLE = 1
!include ../../../makefile.inc


# Flag overloads and local macros.
CLEANEXTRAS = ole32rsrc.asm $(ODIN32_BIN)\regsvr32.exe


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\apartment.obj \
$(OBJDIR)\objectcontext.obj \
$(OBJDIR)\ole32.obj \
$(OBJDIR)\antimoniker.obj \
$(OBJDIR)\bindctx.obj \
$(OBJDIR)\clsid.obj \
$(OBJDIR)\compositemoniker.obj \
$(OBJDIR)\datacache.obj \
$(OBJDIR)\defaulthandler.obj \
$(OBJDIR)\filemoniker.obj \
$(OBJDIR)\guid.obj \
$(OBJDIR)\hglobalstream.obj \
$(OBJDIR)\initialise.obj \
$(OBJDIR)\itemmoniker.obj \
$(OBJDIR)\iunknown.obj \
$(OBJDIR)\imessagefilter.obj \
$(OBJDIR)\library.obj \
$(OBJDIR)\memlockbytes.obj \
$(OBJDIR)\moniker.obj \
$(OBJDIR)\oString.obj \
$(OBJDIR)\ole2impl.obj \
$(OBJDIR)\ole2.obj \
$(OBJDIR)\oleobj.obj \
$(OBJDIR)\oleClip.obj \
$(OBJDIR)\oleDrag.obj \
$(OBJDIR)\oleMenu.obj \
$(OBJDIR)\stg_bigblockfile.obj \
$(OBJDIR)\stg_stream.obj \
$(OBJDIR)\storage.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\taskmem.obj \
$(OBJDIR)\ole32rsrc.obj


# Target and original target names - names of the dll without extention and path
TARGET      = ole32os2
ORGTARGET   = ole32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(ODIN32_BIN)\$(TARGET).dll \
        lib \
        $(OBJDIR)\regsvr32.exe \
        $(ODIN32_BIN)\regsvr32.exe


# Lib rule - build importlibrary (and evt. other libs)
lib: $(ORGTARGET).lib $(ODIN32_LIB)\$(ORGTARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(ORGTARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(ODIN32_LIB)/rpcrt4.lib
$(ODIN32_LIB)/user32.lib
$(ODIN32_LIB)/gdi32.lib
$(ODIN32_LIB)/comctl32.lib
$(ODIN32_LIB)/advapi32.lib
$(ODIN32_LIB)/kernel32.lib
$(ODIN32_LIB)/oleaut32.lib
$(ODIN32_LIB)/odincrt.lib
OS2386.LIB
$(RTLLIB_O)
$(ORGTARGET).def
<<keep



#
# Regsrv32.exe rules.
#
$(OBJDIR)\regsvr32.obj:   regsvr32.cpp
    $(CC) -C $(CXXFLAGS) -Ge+ -Tm+ -Fo$@ regsvr32.cpp

$(OBJDIR)\regsvr32.exe:   $(OBJDIR)\regsvr32.obj regsvr32.def \
                $(ODIN32_LIB)/user32.lib \
                $(ODIN32_LIB)/kernel32.lib \
                $(ODIN32_LIB)/ole32.lib
    $(LD) $(LDFLAGS) -Ge+ -Fm -Fe$@ $** \
        $(ODIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)

$(ODIN32_BIN)\regsvr32.exe: $(OBJDIR)\regsvr32.exe
    $(CP) $** $@


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(ODIN32_INCLUDE);$(ODIN32_INCLUDE)\win;. \
        *.c *.cpp *.h *.asm *.inc $(ODIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(ODIN32_POST_INC)

