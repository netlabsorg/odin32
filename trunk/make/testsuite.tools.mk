# $Id: testsuite.tools.mk,v 1.2 2002-06-19 02:17:07 bird Exp $

#
# Tools for the testsuite.
#
# Note! All macros is prefixed 'TS_'.
#

# PE/PEC from Odin.
TS_PE           = pe.exe
TS_PEC          = pec.exe

# UNIX stuff!
TS_TEST         = test.exe
TS_TEST_FILE_Z  = $(TS_TEST) ! -s
TS_TEST_FILE_NZ = $(TS_TEST) -s
TS_TEST_DIR     = $(TS_TEST) -d
TS_SED          = sed.exe
TS_CMP          = cmp.exe
# UNIX or 4OS2 will not be used in append mode so no '-' vs. '/' problems.
TS_TEE          = tee

# dir (files only)
TS_DIR_B        = dir /B /ON
