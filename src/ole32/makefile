# $Id: makefile,v 1.22 2000-03-06 23:38:50 bird Exp $

#
# Odin32 API
#
#       ole32.dll makefile
#


# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = ole32rsrc.asm $(PDWIN32_BIN)\regsvr32.exe


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
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
$(OBJDIR)\library.obj \
$(OBJDIR)\memlockbytes.obj \
$(OBJDIR)\moniker.obj \
$(OBJDIR)\oString.obj \
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
$(OBJDIR)\ole32rsrc.obj \
$(PDWIN32_LIB)\dllentry.obj


# Target and original target names - names of the dll without extention and path
TARGET      = ole32os2
ORGTARGET   = ole32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib \
        $(OBJDIR)\regsvr32.exe \
        $(PDWIN32_BIN)\regsvr32.exe


# Lib rule - build importlibrary (and evt. other libs)
lib: $(ORGTARGET).lib $(PDWIN32_LIB)\$(ORGTARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(ORGTARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/user32.lib
$(PDWIN32_LIB)/gdi32.lib
$(PDWIN32_LIB)/comctl32.lib
$(PDWIN32_LIB)/advapi32.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/oleaut32.lib
$(PDWIN32_LIB)/odincrt.lib
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
                $(PDWIN32_LIB)/user32.lib \
                $(PDWIN32_LIB)/kernel32.lib \
                $(PDWIN32_LIB)/ole32.lib
    $(LD) $(LDFLAGS) -Ge+ -Fm -Fe$@ $** \
        $(PDWIN32_LIB)/odincrt.lib OS2386.LIB $(RTLLIB_O)

$(PDWIN32_BIN)\regsvr32.exe: $(OBJDIR)\regsvr32.exe
    $(CP) $** $@


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

