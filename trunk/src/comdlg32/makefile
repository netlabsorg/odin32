# $Id: makefile,v 1.20 2000-11-09 21:09:28 sandervl Exp $

#
# Odin32 API
#
#       comdlg32.dll makefile
#

# Directory macros.
PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB     = ..\..\lib
PDWIN32_BIN     = ..\..\$(OBJDIR)
PDWIN32_TOOLS   = ..\..\tools\bin


# Compiler, tools, and interference rules.
!include $(PDWIN32_INCLUDE)/pdwin32.mk


# Flag overloads and local macros.
CLEANEXTRAS = rsrc.asm


# Object files. All objects should be prefixed with $(OBJDIR)!
OBJS = \
$(OBJDIR)\colordlg.obj \
$(OBJDIR)\fontdlg.obj \
$(OBJDIR)\initterm.obj \
$(OBJDIR)\cdlg32.obj \
$(OBJDIR)\filetitle.obj \
$(OBJDIR)\finddlg32.obj \
$(OBJDIR)\filedlg95.obj \
$(OBJDIR)\comdlg32.obj \
$(OBJDIR)\filedlgbrowser.obj \
$(OBJDIR)\comdlgguid.obj \
$(OBJDIR)\rsrc.obj


# Target name - name of the dll without extention and path.
TARGET = comdlg32


# All rule - build objs, target dll, copies dll to bin and makes libs.
all:    $(OBJDIR) \
        $(OBJDIR)\$(TARGET).dll \
        $(PDWIN32_BIN)\$(TARGET).dll \
        lib


# Lib rule - build importlibrary (and evt. other libs)
lib: $(TARGET).lib $(PDWIN32_LIB)\$(TARGET).lib


# Dll rule - builds the target dll.
$(OBJDIR)\$(TARGET).dll: $(OBJS) $(TARGET).def $(OBJDIR)\$(TARGET).lrf
    $(LD2) $(LD2FLAGS) @$(OBJDIR)\$(TARGET).lrf


# Linker file - creates the parameter file passed on to the linker.
$(OBJDIR)\$(TARGET).lrf: makefile
    @echo Creating file <<$@
/OUT:$(OBJDIR)\$(TARGET).dll
/MAP:$(OBJDIR)\$(TARGET).map
$(OBJS)
$(PDWIN32_LIB)/pmwinx.lib
$(PDWIN32_LIB)/kernel32.lib
$(PDWIN32_LIB)/user32.lib
$(PDWIN32_LIB)/$(ODINCRT).lib
$(PDWIN32_LIB)/shlwapi.lib
$(PDWIN32_LIB)/shell32.lib
$(PDWIN32_LIB)/comctl32.lib
$(PDWIN32_LIB)/gdi32.lib
OS2386.LIB
$(RTLLIB_O)
$(TARGET).def
<<keep


# Dep rule - makes depenencies for C, C++ and Asm files.
dep:
    $(DEPEND) -I$(PDWIN32_INCLUDE);$(PDWIN32_INCLUDE)\win \
        *.c *.cpp *.h *.asm *.inc $(PDWIN32_INCLUDE)\*.h


$(OBJDIR)\rsrc.obj: \
cdlg_Ca.orc \
cdlg_Cs.orc \
cdlg_Da.orc \
cdlg_De.orc \
cdlg_En.orc \
cdlg_Eo.orc \
cdlg_Es.orc \
cdlg_Fi.orc \
cdlg_Fr.orc \
cdlg_Hu.orc \
cdlg_It.orc \
cdlg_Ja.orc \
cdlg_Ko.orc \
cdlg_Nl.orc \
cdlg_No.orc \
cdlg_Pl.orc \
cdlg_Pt.orc \
cdlg_Ru.orc \
cdlg_Sk.orc \
cdlg_Sv.orc \
cdlg_Wa.orc \
cdlg_xx.orc \
rsrc.orc

# Includes the common rules.
!include $(PDWIN32_INCLUDE)/pdwin32.post

