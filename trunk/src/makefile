# $Id: makefile,v 1.69 2001-05-08 12:16:48 bird Exp $

#
# Odin32 API
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


#
# Build environment.
#
!include ../makefile.inc


#
# Exclude lists which is to be passed in to DOWITHDIRS
#
NOTTOBEMADE = CVS;pe2lx;odin32api;odinwps;spy;odinldr;msvcrt;ws2help   # generally not made
NOTDLLS     = $(NOTTOBEMADE);peldr;dllentry             # not dlls
NOTHAVELIBS = $(NOTTOBEMADE);peldr                      # don't generate libs
NOTWIN32DLLS= $(NOTDLLS);win32k;odincrt;dllentry        # not win32 dlls.


#
# Check for SMP and MASSIVE.
#   SMP:        Working backwards thru the directories traversed by a rule.
#   MASSIVE:    Spawn a child processes for each process.
#
!ifdef SMP
EXTRA = -r
!endif
!ifdef MASSIVE
EXTRA = -s start "Odin32 Massive Build" /B /C
!endif



#
# All rule. Make libraries and then build every directory.
#
all: libs build



#
# Smp rule - Dual build. Starts a second build process after libraries
#            was made.
#
smp: startsmp libs build

startsmp:   # Internal helper for the smp rule.
    start "Odin32 SMP Build Process" /B $(MAKE_CMD) SMP=1 libs build


#
# Rules which supports the SMP and MASSIVE features.
#
# build:    Invokes the all rule in every directory.
# clean:    Invokes the clean rule in every directory.
# lib(s):   Invokes the lib rule in every directory known to support it.
# dep:      Invokes the dep rule in every directory.
#
build:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -lbld -c $(EXTRA) \
        $(MAKE_CMD) all

lib libs:
    $(DOWITHDIRS) -e$(NOTHAVELIBS) -c -llib $(EXTRA) \
        $(MAKE_CMD) lib

dep:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -c -ldep $(EXTRA) \
        $(MAKE_CMD) NODEP=1 dep

clean:
    $(DOWITHDIRS) -e$(NOTTOBEMADE) -c -lcln $(EXTRA) \
        $(MAKE_CMD) NODEP=1 clean


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

