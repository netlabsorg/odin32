# $Id: makefile,v 1.2 2000-12-02 23:27:03 bird Exp $

#
# Odin32 API
#
#       Makefile for regedit.exe.
#


#
# Directory macros.
#
ODIN32_BIN  = $(ODIN32_TOOLS)


#
# Tell buildenvironment that we're making an .exe.
# Tell buildenvironment that we like to use static linked CRT.
# Tell buildenvironment that we should not copy this into /bin.
#
EXETARGET   = 1
NO_MAIN_BIN_COPY = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Set Win32APP flags.
#
CFLAGS   = $(CFLAGS_WIN32APP)
CXXFLAGS = $(CXXFLAGS_WIN32APP)
CDEFINES = $(CDEFINES_WIN32APP)
#LD2FLAGS = $(LD2FLAGS_WIN32APP)


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS= \
$(OBJDIR)\regapi.obj \
$(OBJDIR)\odinexe.obj


#
# Libraries. One space before the '\'.
#
LIBS = \
$(ODIN32_LIB)\kernel32.lib \
$(ODIN32_LIB)\advapi32.lib \
$(ODIN32_LIB)\user32.lib \
$(ODIN32_LIB)\gdi32.lib \
$(ODIN32_LIB)\$(ODINCRT).lib \
os2386.lib \
$(RTLLIB_O)


#
# Target name - name of the exe without extention and path.
#
TARGET = regedit


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)




#CFLAGS     = -Q  -Si -Ti -Tm+ -Ge- -Ss+ -W3 -Gm+ /Gn+ -I$(ODIN32_INCLUDE)\Win -D__WIN32OS2__ -DDEBUG -D__i386__
#CXXFLAGS   = -Q  -Si -Ti -Tm+ -Ge- -Ss+ -W3 -Gm+ /Gn+ -I$(ODIN32_INCLUDE)\Win -D__WIN32OS2__ -DDEBUG -D__i386__
#CFLAGS = $(CFLAGS) /Ge+ -I$(ODIN32_INCLUDE)
#CXXFLAGS = $(CXXFLAGS) /Ge+ -I$(ODIN32_INCLUDE)
#LDFLAGSEXE = $(LDFLAGS) /Ge+ /B"/pmtype:pm /stack:0x30000 /NOBASE /Map" \
#             $(EXTRA_LIBS) $(ODIN32_LIB)\kernel32.lib $(ODIN32_LIB)\user32.lib \
#             $(ODIN32_LIB)\gdi32.lib os2386.lib $(ODIN32_LIB)\odincrt.lib $(RTLLIB_O)

# Build rule for EXE
#$(PROJ).EXE: $(BASE_OBJS) $(PROJ_OBJS)
#        $(LD) $(LDFLAGSEXE) -Fe$@ $(PROJ_OBJS) $(BASE_OBJS)

# Rules for cleaning out those old files
#clean:
#    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc
