# $Id: Makefile,v 1.5 2002-06-25 01:21:19 bird Exp $

#
# Main script for the CUI part.
#

# Include the setup.mak
PATH_ROOT = ..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include $(PATH_MAKE)\testsuite.tools.mk

# Target Config
TARGET_MODE = TESTCASE
TARGET_NAME = CUI

# Makefiles
TESTCASE_MAKEFILES = \
DoxyGen.mak \
watcomc.mak \
# vac42.mak \
# vac50.mak \
# vac60.mak \
# php.mak \
# mysql.mak \
# mysqld.mak \
# mp3encoder.mak \
# ghostscript.mak \
# TeX.mak \
# jikes.mak \
# javac.mak \
# java.mak \
# gbm.mak \
# gdlib.mak \
# libtiff.mak \
# 4nt.mak \
# regina.mak \
# orexx.mak \
# zip.mak \
# sti.mak (http://setiathome.ssl.berkeley.edu/stats/team/team_1331.html)

# SubDirs
#TESTCASE_SUBDIRS =


# Include the process rulez.
!include $(MAKE_INCLUDE_PROCESS)

#
# Make everything as quiet as possible.
#
!if [set BUILD_QUIET=1]
!endif
!if [set BUILD_VERBOSE=]
!endif


#
# The testcase rule.
#
testcase:
!if "$(MAKE_INCLUDE_PROCESS:process.forwarder=)" == "$(MAKE_INCLUDE_PROCESS)"
    @echo CUI testcases: >> $(TS_LOGFILE)
    $(TOOL_DOMAKES) "$(TESTCASE_MAKEFILES)" $(TOOL_MAKE)
    $(TOOL_DODIRS) "$(TESTCASE_SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE)
    @echo CUI testcases completed >> $(TS_LOGFILE)
!else
! ifndef BUILD_VERBOSE
    @ \
! endif
    $(TOOL_BUILDENV) $(BUILD_ENVS_CHANGE) * $(TOOL_MAKE) -f $(MAKEFILE) $@
!endif


#
# Our own clean rule.
#
!if "$(MAKE_INCLUDE_PROCESS:process.forwarder=)" == "$(MAKE_INCLUDE_PROCESS)"
clean:
    $(TOOL_RM) *.log *.exe *.err
    $(TOOL_DOMAKES) "$(TESTCASE_MAKEFILES)" $(TOOL_MAKE) clean
    $(TOOL_DODIRS) "$(TESTCASE_SUBDIRS)" $(TOOL_MAKE) -f $(BUILD_MAKEFILE) clean
!endif
