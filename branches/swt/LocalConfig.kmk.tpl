## @file
# Local Project Configuration File (Template)
#
# The local project configuration file is used to specify local paths to
# external tools and libraries and also to optioanlly override the global
# project configuration options.
#
# NOTES:
#
#   This file is a template! Copy it to a file named LocalConfig.kmk in
#   the same directory and modify the copy to fit your local environment.
#
#   All paths in this file are specified using forward slashes unless specified
#   otherwise.
#

#
# Base directory where all build output will go. The directory will be created
# if does not exist. The default is "out" in the root of the source tree.
#
# PATH_OUT_BASE := out

#
# Path to the OS/2 Toolkit versoin 4.x. If not set, it is assumed that the
# toolkit is accessible through the environment.
#
# PATH_SDK_OS2TK4 := D:/Dev/OS2TK45

#
# Path to the IBM Assembly Language Processor (may be found in the
# OS/2 toolkit). Defaults to '$(PATH_SDK_OS2TK4)/bin/alp.exe' if
# PATH_SDK_OS2TK4 is set, or to 'alp.exe' otherwise.
#
# PATH_TOOL_ALP := alp.exe

#
# Globally enable simple console logging.
# Per source console logging can be enabled by defining DBG_CON _before_
# the inclusion of the <misc.h> header.
# DBG_CON=
