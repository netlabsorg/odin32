# $Id: setup.win32relwat11.mk,v 1.1 2003-06-30 13:19:52 bird Exp $

# ---WIN32, RELEASE, WAT11-------------------------
ENV_NAME="Win32, Debug, Watcom C/C++ v11.0c"
!include $(PATH_MAKE)\setup.win32debwat11.mk
_CXX_OPTIONAL  = $(_CXX_OPTIONAL) -UDEBUG
_CC_OPTIONAL   = $(_CC_OPTIONAL)  -UDEBUG
