# $Id: makefile,v 1.38 1999-12-28 13:02:22 sandervl Exp $

#
# PD-Win32 API
#
#       Top of the src tree makefile
#
#
#       Usage: nmake ( all | libs | clean )
#
#            all: Build the entire src tree.
#            lib: Builds the libraries.
#            clean: Bring src tree back to a "virgin" state.
#
#
#
!ifdef DEBUG
MAKE_CMD = nmake -nologo DEBUG=1
!else
MAKE_CMD = nmake -nologo
!endif

!ifdef NODEBUGINFO
MAKE_CMD = $(MAKE_CMD) NODEBUGINFO=1
!endif

LIBS = advapi32\advapi32.lib  avifil32\avifil32.lib  capi2032\capi2032.lib    \
       comctl32\comctl32.lib  comdlg32\comdlg32.lib  crtdll\crtdll.lib        \
       ddraw\ddraw.lib        dplay\dplay.lib        dsound\dsound.lib        \
       gdi32\gdi32.lib        imagehlp\imagehlp.lib  imm32\imm32.lib          \
       kernel32\kernel32.lib  lz32\lz32.lib          mpr\mpr.lib              \
       msacm32\msacm32.lib    msvfw32\msvfw32.lib    ntdll\ntdll.lib          \
       odincrt\odincrt.lib    ole32\ole32.lib        oleaut32\oleaut32.lib    \
       olecli32\olecli32.lib  oledlg\oledlg.lib      olesvr32\olesvr32.lib    \
       psapi\psapi.lib        rasapi32\rasapi32.lib  shell32\shell32.lib      \
       shlwapi\shlwapi.lib    tapi32\tapi32.lib      user32\user32.lib        \
       version\version.lib    winmm\winmm.lib        winspool\winspool.lib    \
       wnaspi32\wnaspi32.lib  wnetap32\wnetap32.lib  wsock32\wsock32.lib      \
       wing32\wing32.lib      dplayx\dplayx.lib      wininet\wininet.lib


all: libs
        cd odincrt
        $(MAKE_CMD)
        cd ..\kernel32
        $(MAKE_CMD)
        cd ..\gdi32
        $(MAKE_CMD)
        cd ..\user32
        $(MAKE_CMD)
        cd ..\advapi32
        $(MAKE_CMD)
        cd ..\shell32
        $(MAKE_CMD)
        cd ..\comctl32
        $(MAKE_CMD)
        cd ..\capi2032
        $(MAKE_CMD)
        cd ..\wsock32
        $(MAKE_CMD)
        cd ..\comdlg32
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
        cd ..\NTDLL
        $(MAKE_CMD)
        cd ..\LZ32
        $(MAKE_CMD)
        cd ..\version
        $(MAKE_CMD)
        cd ..\peldr
        $(MAKE_CMD)
        cd ..\win32k\pe2lx
        $(MAKE_CMD)
        cd ..\..\ddraw
        $(MAKE_CMD)
        cd ..\dinput
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
        cd ..\psapi
        $(MAKE_CMD)
        cd ..\msvfw32
        $(MAKE_CMD)
        cd ..\crtdll
        $(MAKE_CMD)
        cd ..\msvcrt
        $(MAKE_CMD)
        cd ..\msacm32
        $(MAKE_CMD)
        cd ..\imagehlp
        $(MAKE_CMD)
        cd ..\avifil32
        $(MAKE_CMD)
        cd ..\dplay
        $(MAKE_CMD)
        cd ..\dplayx
        $(MAKE_CMD)
        cd ..\wnaspi32
        $(MAKE_CMD)
        cd ..\shlwapi
        $(MAKE_CMD)
        cd ..\dsound
        $(MAKE_CMD)
        cd ..\wing32
        $(MAKE_CMD)
        cd ..\wininet
        $(MAKE_CMD)


libs: $(LIBS)

$(LIBS): $(@D)\$(@B).def
        cd $(@D)
        $(MAKE_CMD) $(@F)
        cd ..


clean:
        cd odincrt
        $(MAKE_CMD) clean
        cd ..\peldr
        $(MAKE_CMD) clean
        cd ..\win32k\pe2lx
        $(MAKE_CMD) clean
        cd ..\..\kernel32
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
        cd ..\msacm32
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
        cd ..\crtdll
        $(MAKE_CMD) clean
        cd ..\msvcrt
        $(MAKE_CMD) clean
        cd ..\shell32
        $(MAKE_CMD) clean
        cd ..\NTDLL
        $(MAKE_CMD) clean
        cd ..\LZ32
        $(MAKE_CMD) clean
        cd ..\psapi
        $(MAKE_CMD) clean
        cd ..\msvfw32
        $(MAKE_CMD) clean
        cd ..\avifil32
        $(MAKE_CMD) clean
        cd ..\dplay
        $(MAKE_CMD) clean
        cd ..\dplayx
        $(MAKE_CMD) clean
        cd ..\dinput
        $(MAKE_CMD) clean
        cd ..\wnaspi32
        $(MAKE_CMD) clean
        cd ..\shlwapi
        $(MAKE_CMD) clean
        cd ..\wing32
        $(MAKE_CMD) clean
        cd ..\wininet
        $(MAKE_CMD) clean

