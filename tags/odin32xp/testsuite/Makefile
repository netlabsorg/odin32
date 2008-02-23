# $Id: Makefile,v 1.3 2002-06-19 02:29:51 bird Exp $

#
# Main testsuite make script.
#
# (The testsuite is driven by make script since they are the best
#  job control tool we have available.)
#

#
# Short howto:
# ------------
#
# - The scripts implements the 'testcase' rule.
# - TARGET_MODE = TESTCASE
# - Include $(PATH_MAKE)\testsuite.tools.mk.
# - Any stuff generated on the fly should be put in PATH_TARGET and
#   removed by the clean rule. (Also remove *.log!)
# - Try to make one .mak file per program.
# - Any program binaries should be put in testsuite/bin, if legally possible,
#   in the testsuite-bin branch. (This branching to save bandwidth and time.)
# - If the tool isn't checked in, you *must* test for presense and warn about
#   that. Should *not* fail in such situation.
# - Output should whenever possibly be generated on the fly in for doing
#   compares. Ie. Use inline files in the makescript.
# - Currently you need to have the Odin release to be tested installed and
#   in the PATH. (This will be changed!)
#

# Include the setup.mak
PATH_ROOT = ..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)

# Target Config
TARGET_MODE = EMPTY

# Odin install script / TODO
#PREMAKEFILES = odin32inst.mak

# Subdirs to process.
SUBDIRS = \
    cui \

# Include the process rulez.
!include $(MAKE_INCLUDE_PROCESS)


#
# This rule checksout the binaries which is residing in the
# testsuite-bin branch
#
checkout-bin:
!if "$(MAKE_INCLUDE_PROCESS:process.forwarder=)" == "$(MAKE_INCLUDE_PROCESS)"
    cvs update -r testsuite-bin -d bin
!else
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@
!endif

