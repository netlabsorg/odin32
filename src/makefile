# $Id: makefile,v 1.14 1999-09-01 07:28:03 sandervl Exp $

#
# PD-Win32 API
#
#       Top of the src tree makefile
#
#
#       Usage: nmake ( all | clean )
#
#            all: Build the entire src tree.
#            clean: Bring src tree back to a "virgin" state.
#
#
#
!ifdef DEBUG
MAKE_CMD = nmake -nologo DEBUG=1
!else
MAKE_CMD = nmake -nologo
!endif

all:
        cd odincrt
        $(MAKE_CMD)
        cd ..\kernel32
        $(MAKE_CMD)
        cd ..\gdi32
        $(MAKE_CMD)
        cd ..\user32
        $(MAKE_CMD)
        cd new
        $(MAKE_CMD)
        cd ..
        cd ..\advapi32
        $(MAKE_CMD)
        cd ..\comdlg32
        $(MAKE_CMD)
        cd ..\comctl32
        $(MAKE_CMD)
        cd ..\capi2032
        $(MAKE_CMD)
        cd ..\wsock32
        $(MAKE_CMD)
        cd ..\tapi32
        $(MAKE_CMD)
        cd ..\rasapi32
        $(MAKE_CMD)
        cd ..\wnetap32
        $(MAKE_CMD)
        cd ..\mpr
        $(MAKE_CMD)
        cd ..\winmm
        $(MAKE_CMD)
        cd ..\winspool
        $(MAKE_CMD)
        cd ..\shell32
        $(MAKE_CMD)
        cd ..\NTDLL
        $(MAKE_CMD)
        cd ..\LZ32
        $(MAKE_CMD)
        cd ..\version
        $(MAKE_CMD)
        cd ..\peldr
        $(MAKE_CMD)
        cd ..\pe2lx
#        wmake -f makefile.wat
        $(MAKE_CMD)
        cd ..\ddraw
        $(MAKE_CMD)
        cd ..\ole32
        $(MAKE_CMD)
        cd ..\oleaut32
        $(MAKE_CMD)
        cd ..\olesvr32
        $(MAKE_CMD)
        cd ..\olecli32
        $(MAKE_CMD)
        cd ..\oledlg
        $(MAKE_CMD)
        cd ..\imm32
        $(MAKE_CMD)
        cd ..\imagehlp
        $(MAKE_CMD)
        cd ..\dsound
        $(MAKE_CMD)

clean:
        cd odincrt
        $(MAKE_CMD) clean
        cd ..\pe2lx
        $(MAKE_CMD) clean
        cd ..\kernel32
        $(MAKE_CMD) clean
        cd ..\user32\new
        $(MAKE_CMD) clean
        cd ..\..\gdi32
        $(MAKE_CMD) clean
        cd ..\advapi32
        $(MAKE_CMD) clean
        cd ..\comdlg32
        $(MAKE_CMD) clean
        cd ..\comctl32
        $(MAKE_CMD) clean
        cd ..\rasapi32
        $(MAKE_CMD) clean
        cd ..\ole32
        $(MAKE_CMD) clean
        cd ..\oleaut32
        $(MAKE_CMD) clean
        cd ..\olesvr32
        $(MAKE_CMD) clean
        cd ..\olecli32
        $(MAKE_CMD) clean
        cd ..\imm32
        $(MAKE_CMD) clean
        cd ..\imagehlp
        $(MAKE_CMD) clean
        cd ..\oledlg
        $(MAKE_CMD) clean
        cd ..\capi2032
        $(MAKE_CMD) clean
        cd ..\tapi32
        $(MAKE_CMD) clean
        cd ..\wsock32
        $(MAKE_CMD) clean
        cd ..\wnetap32
        $(MAKE_CMD) clean
        cd ..\ddraw
        $(MAKE_CMD) clean
        cd ..\dsound
        $(MAKE_CMD) clean
        cd ..\mpr
        $(MAKE_CMD) clean
        cd ..\version
        $(MAKE_CMD) clean
        cd ..\winmm
        $(MAKE_CMD) clean
        cd ..\winspool
        $(MAKE_CMD) clean
        cd ..\shell32
        $(MAKE_CMD) clean
        cd ..\NTDLL
        $(MAKE_CMD) clean
        cd ..\LZ32
        $(MAKE_CMD) clean
