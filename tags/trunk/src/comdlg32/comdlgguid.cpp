/* $Id: comdlgguid.cpp,v 1.1 1999-11-02 19:09:42 sandervl Exp $ */

/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>

#define ICOM_CINTERFACE 1
#define CINTERFACE 1
#define INITGUID

#include <os2win.h>
#include <shlguid.h>
#include <wine/obj_oleview.h>
#include <wine/obj_dragdrop.h>
#include <wine/obj_inplace.h>
#include <wine/obj_control.h>
#include <wine/obj_shellfolder.h>
#include <wine/obj_shelllink.h>
#include <wine/obj_contextmenu.h>
#include <wine/obj_commdlgbrowser.h>
#include <wine/obj_extracticon.h>
#include <wine/obj_shellextinit.h>
#include <wine/obj_shellbrowser.h>
#include <docobj.h>

ODINDEBUGCHANNEL(COMDLG32-SHELLGUID)


