# $Id: makefile,v 1.44 2000-02-10 01:17:05 sandervl Exp $

#
# PD-Win32 API
#
#       Top of the src tree makefile
#
#
#       Usage: nmake ( all | libs | dep | clean )
#
#            all: Build the entire src tree.
#            lib: Builds the libraries.
#            dep: Builds depenency information for the entire src tree.
#            clean: Bring src tree back to a "virgin" state.
#
#
#

PDWIN32_INCLUDE = ..\include
PDWIN32_LIB     = ..\lib
PDWIN32_BIN     = ..\bin
PDWIN32_TOOLS   = ..\tools\bin
!include $(PDWIN32_INCLUDE)\pdwin32.mk


!ifdef DEBUG
MAKE_CMD = nmake -nologo DEBUG=1
!else
MAKE_CMD = nmake -nologo
!endif

!ifdef NODEBUGINFO
MAKE_CMD = $(MAKE_CMD) NODEBUGINFO=1
!endif

NOTTOBEMADE = CVS;win32k;pe2lx;odin32api;odinwps;spy
NOTDLLS     = $(NOTTOBEMADE);peldr
NOTHAVELIBS = $(NOTDLLS)

#LIBS = advapi32\advapi32.lib  avifil32\avifil32.lib  capi2032\capi2032.lib    \
#       comctl32\comctl32.lib  comdlg32\comdlg32.lib  crtdll\crtdll.lib        \
#       ddraw\ddraw.lib        dplay\dplay.lib        dsound\dsound.lib        \
#       gdi32\gdi32.lib        imagehlp\imagehlp.lib  imm32\imm32.lib          \
#       kernel32\kernel32.lib  lz32\lz32.lib          mpr\mpr.lib              \
#       msacm32\msacm32.lib    msvfw32\msvfw32.lib    ntdll\ntdll.lib          \
#       odincrt\odincrt.lib    ole32\ole32.lib        oleaut32\oleaut32.lib    \
#       olecli32\olecli32.lib  oledlg\oledlg.lib      olesvr32\olesvr32.lib    \
#       psapi\psapi.lib        rasapi32\rasapi32.lib  shell32\shell32.lib      \
#       shlwapi\shlwapi.lib    tapi32\tapi32.lib      user32\user32.lib        \
#       version\version.lib    winmm\winmm.lib        winspool\winspool.lib    \
#       wnaspi32\wnaspi32.lib  wnetap32\netapi32.lib  wsock32\wsock32.lib      \
#       wing32\wing32.lib      dplayx\dplayx.lib      wininet\wininet.lib      \
#       opengl\opengl32.lib


all: libs
    cd dllentry
    $(MAKE_CMD)
    cd ..\odincrt
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
    cd ..\opengl
    $(MAKE_CMD)
    cd ..\wing32
    $(MAKE_CMD)
    cd ..\wininet
    $(MAKE_CMD)
    cd ..\twain_32
    $(MAKE_CMD)


#libs: $(LIBS)
#
#$(LIBS): $(@D)\$(@B).def
#    cd $(@D)
#    $(MAKE_CMD) $(@F)
#    cd ..

libs:
    $(DOWITHDIRS) -e$(NOTHAVELIBS) -cp $(MAKE_CMD) lib


dep:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -cp $(MAKE_CMD) NODEP=1 dep
    cd win32k\pe2lx
    $(MAKE_CMD) NODEP=1 dep
    cd ..\..


clean:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -cp $(MAKE_CMD) NODEP=1 clean
    cd win32k\pe2lx
    $(MAKE_CMD) NODEP=1 clean
    cd ..\..
    cd win32k
    if exist makefile.inc $(MAKE_CMD) NODEP=1 clean

