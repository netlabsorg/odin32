# $Id: makefile,v 1.3 1999-06-07 20:57:55 sandervl Exp $ 

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
        cd kernel32
        $(MAKE_CMD)
        cd ..\gdi32
        $(MAKE_CMD)
        cd ..\user32
        $(MAKE_CMD)
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
        cd ..\wnetap32
        $(MAKE_CMD)
        cd ..\mpr
        $(MAKE_CMD)
        cd ..\version
        $(MAKE_CMD)
        cd ..\winmm
        $(MAKE_CMD)
        cd ..\winspool
        $(MAKE_CMD)
        cd ..\shell32
        $(MAKE_CMD)
        cd ..\NTDLL
        $(MAKE_CMD)
        cd ..\peldr
        $(MAKE_CMD)
        cd ..\pe2lx
#        wmake -f makefile.wat
        $(MAKE_CMD)
        cd ..\ddraw
        $(MAKE_CMD)
        cd ..\dsound
        $(MAKE_CMD)
        cd ..\ole32
        $(MAKE_CMD)

clean:
        cd pe2lx
        $(MAKE_CMD) clean
        cd ..\kernel32
        $(MAKE_CMD) clean
        cd ..\user32
        $(MAKE_CMD) clean
        cd ..\gdi32
        $(MAKE_CMD) clean
        cd ..\advapi32
        $(MAKE_CMD) clean
        cd ..\comdlg32
        $(MAKE_CMD) clean
        cd ..\comctl32
        $(MAKE_CMD) clean
        cd ..\ole32
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
