#
# PD-Win32 API
#
#

PDWIN32_INCLUDE = ..\..\include
PDWIN32_LIB = ..\..\lib
PDWIN32_BIN = ..\..\bin

!include $(PDWIN32_INCLUDE)/pdwin32.mk

PROJ = odininst

all: $(PROJ).exe

# Define project specific macros
PROJ_OBJS  = odininst.obj 


CFLAGS     = -Q  -Si -Ti -Tm+ -Ge- -Ss+ -W3 -Gm+ /Gn+ -I$(PDWIN32_INCLUDE)\Win -D__WIN32OS2__ -DDEBUG -D__i386__
CXXFLAGS   = -Q  -Si -Ti -Tm+ -Ge- -Ss+ -W3 -Gm+ /Gn+ -I$(PDWIN32_INCLUDE)\Win -D__WIN32OS2__ -DDEBUG -D__i386__

CFLAGS = $(CFLAGS) /Ge+ -I$(PDWIN32_INCLUDE)
CXXFLAGS = $(CXXFLAGS) /Ge+ -I$(PDWIN32_INCLUDE)
LDFLAGSEXE = $(LDFLAGS) /Ge+ /B"/pmtype:pm /stack:0x30000 /NOBASE /Map" \
             $(EXTRA_LIBS) $(PDWIN32_LIB)\kernel32.lib $(PDWIN32_LIB)\user32.lib \
             $(PDWIN32_LIB)\gdi32.lib os2386.lib $(PDWIN32_LIB)\odincrt.lib $(RTLLIB_O)

# Build rule for EXE
$(PROJ).exe: $(PROJ_OBJS) 
        $(LD) $(LDFLAGSEXE) -Fe$@ $(PROJ_OBJS)
        copy $(PROJ).exe ..\bin

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc
