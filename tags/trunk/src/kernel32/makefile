# $Id: makefile,v 1.89 2000-03-07 11:08:01 bird Exp $

#
# Odin32 API
#
#       kernel32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = kernelrsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\KERNEL32.OBJ \
$(OBJDIR)\KOBJECTS.OBJ \
$(OBJDIR)\CONSOLE.OBJ \
$(OBJDIR)\CONIN.OBJ \
$(OBJDIR)\CONBUFFER.OBJ \
$(OBJDIR)\CONOUT.OBJ \
$(OBJDIR)\UNICODE.OBJ \
$(OBJDIR)\network.OBJ \
$(OBJDIR)\HMDEVIO.OBJ \
$(OBJDIR)\profile.obj \
$(OBJDIR)\THREAD.OBJ \
$(OBJDIR)\virtual.obj \
$(OBJDIR)\THUNK.OBJ \
$(OBJDIR)\OBSOLETE.OBJ \
$(OBJDIR)\COMM.OBJ \
$(OBJDIR)\MESSAGE.OBJ \
$(OBJDIR)\RESOURCE.OBJ \
$(OBJDIR)\EXCEPTIONS.OBJ \
$(OBJDIR)\heapshared.obj \
$(OBJDIR)\cpuhlp.obj \
$(OBJDIR)\heapcode.obj \
$(OBJDIR)\LFILE.OBJ \
$(OBJDIR)\NPIPE.OBJ \
$(OBJDIR)\oslibdos.obj \
$(OBJDIR)\oslibmisc.obj \
$(OBJDIR)\MISC.OBJ \
$(OBJDIR)\EXCEPTUTIL.OBJ \
$(OBJDIR)\LANG.OBJ \
$(OBJDIR)\ICCIO.OBJ \
$(OBJDIR)\MAP.OBJ \
$(OBJDIR)\WIN32UTIL.OBJ \
$(OBJDIR)\heap.OBJ \
$(OBJDIR)\heapstring.obj \
$(OBJDIR)\os2heap.OBJ \
$(OBJDIR)\vmutex.OBJ \
$(OBJDIR)\initterm.OBJ \
$(OBJDIR)\handlemanager.OBJ \
$(OBJDIR)\environ.obj \
$(OBJDIR)\initsystem.obj \
$(OBJDIR)\hmdevice.obj \
$(OBJDIR)\hmopen32.obj \
$(OBJDIR)\hmobjects.obj \
$(OBJDIR)\hmevent.obj \
$(OBJDIR)\hmmutex.obj \
$(OBJDIR)\hmcomm.obj \
$(OBJDIR)\hmsemaphore.obj \
$(OBJDIR)\wprocess.OBJ \
$(OBJDIR)\conprop.OBJ \
$(OBJDIR)\conprop2.obj \
$(OBJDIR)\winimagelx.obj \
$(OBJDIR)\winimagebase.OBJ \
$(OBJDIR)\windllbase.OBJ \
$(OBJDIR)\winexebase.OBJ \
$(OBJDIR)\time.obj \
$(OBJDIR)\mmap.obj \
$(OBJDIR)\winimagepe2lx.obj \
$(OBJDIR)\winimagepeldr.obj \
$(OBJDIR)\windllpe2lx.obj \
$(OBJDIR)\windlllx.obj \
$(OBJDIR)\windllpeldr.obj \
$(OBJDIR)\winexepe2lx.obj \
$(OBJDIR)\winexelx.obj \
$(OBJDIR)\winexepeldr.obj \
$(OBJDIR)\WINRES.OBJ \
$(OBJDIR)\critsection.obj \
$(OBJDIR)\pefile.OBJ \
$(OBJDIR)\winimgres.OBJ \
$(OBJDIR)\wintls.obj \
$(OBJDIR)\async.OBJ \
$(OBJDIR)\fileio.obj \
$(OBJDIR)\hmtoken.obj \
$(OBJDIR)\atom.obj \
$(OBJDIR)\disk.obj \
$(OBJDIR)\directory.obj \
$(OBJDIR)\cvtbitmap.obj \
$(OBJDIR)\hmmmap.obj \
$(OBJDIR)\winfakepeldr.obj \
$(OBJDIR)\cvtaccel.obj \
$(OBJDIR)\cvticon.obj \
$(OBJDIR)\cvticongrp.obj \
$(OBJDIR)\oslibexcept.obj \
$(OBJDIR)\cpu.obj \
$(OBJDIR)\process.obj \
$(OBJDIR)\cvtcursor.obj \
$(OBJDIR)\cvtcursorgrp.obj \
$(OBJDIR)\stubs.obj \
$(OBJDIR)\interlock.obj \
$(OBJDIR)\toolhelp.obj \
$(OBJDIR)\codepage.obj \
$(OBJDIR)\debug.obj \
$(OBJDIR)\oslibdebug.obj \
$(OBJDIR)\dbglocal.obj \
$(OBJDIR)\registry.obj \
$(OBJDIR)\kernelrsrc.obj


# Target name - name of the dll without extention and path.
TARGET = kernel32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf \
                          $(OBJDIR)\console.res
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf
    $(OS2RC) $(OBJDIR)\console.res $@


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$(OBJDIR)\$(@B).lrf
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)\odincrt.lib
$(PDWIN32_LIB)\PMWINX.LIB
$(PDWIN32_LIB)\LIBULS.LIB
$(PDWIN32_LIB)\LIBCONV.LIB
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

