# $Id: makefile,v 1.64 2000-10-23 11:24:01 sandervl Exp $

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

#
# Exclude lists which is to be passed in to DOWITHDIRS
#
NOTTOBEMADE = CVS;pe2lx;odin32api;odinwps;spy           # generally not made
NOTDLLS     = $(NOTTOBEMADE);peldr;dllentry             # not dlls
NOTHAVELIBS = $(NOTTOBEMADE);peldr                      # don't generate libs
NOTWIN32DLLS= $(NOTDLLS);win32k;odincrt;dllentry        # not win32 dlls.


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
    cd ..\shfolder
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
    cd ..\dciman32
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
#    cd ..\win32k
#    $(MAKE_CMD)
    cd ..\ddraw
    $(MAKE_CMD)
    cd ..\dinput
    $(MAKE_CMD)
    cd ..\rpcrt4
    $(MAKE_CMD)
    cd ..\ole32
    $(MAKE_CMD)
    cd ..\oleaut32
    $(MAKE_CMD)
    cd ..\olesvr32
    $(MAKE_CMD)
    cd ..\olecli32
    $(MAKE_CMD)
    cd ..\olepro32
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
    cd ..\riched32
    $(MAKE_CMD)
    cd ..\icmp
    $(MAKE_CMD)
    cd ..\urlmon
    $(MAKE_CMD)

#
# Sander, could you evaluate these two rules:
# build:    This is a possible replacement of the all-rule in this makefile.
#           It builds everything in directory order.
#
# revbuild: This is a possible replacement of the makefile.smp file.
#           It builds everything in reverse directory order.
#
build: libs
    $(DOWITHDIRS) -e$(NOTTOBEMADE)    -cp $(MAKE_CMD)

revbuild:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -r -cp $(MAKE_CMD)


libs:
    $(DOWITHDIRS) -e$(NOTHAVELIBS) -cp $(MAKE_CMD) lib

dep:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -cp $(MAKE_CMD) NODEP=1 dep

clean:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -cp $(MAKE_CMD) NODEP=1 clean


#
# Odin32 API DB Management
#
APIImport:
    $(DOWITHDIRS) -e$(NOTWIN32DLLS) -cp \
        ..\..\tools\bin\APIImport.cmd

StateUpd:
    ..\tools\bin\StateUpd.exe -io+
    -$(DOWITHDIRS) -e$(NOTWIN32DLLS) -cp -i \
        ..\..\tools\bin\StateUpd.cmd -ie- -ib-
    ..\tools\bin\StateUpd.exe -io+

ReadSignals:
    -$(DOWITHDIRS) -e$(NOTWIN32DLLS) -cp -i \
        IF EXIST signals.log list signals.log

ReadStateUpdLogs:
    -$(DOWITHDIRS) -e$(NOTWIN32DLLS) -cp -i \
        IF EXIST stateupd.log list stateupd.log

ReadAPIImportLogs:
    -$(DOWITHDIRS) -e$(NOTWIN32DLLS) -cp -i \
        IF EXIST apiimport.log list apiimport.log


CleanDBLogs:
    $(DOWITHDIRS) -e$(NOTWIN32DLLS) -cp \
        $(RM) apiimport.log signals.log stateupd.log apiimport.def
    $(RM) signals.log stateupd.log

