/*
 *  Shell UID
 *
 *  1998 Juergen Schmied (jsch)  *  <juergen.schmied@metronet.de>
 *
 *  this in in a single file due to interfering definitions
 *
 */
#include <odin.h>

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
//#include <docobj.h>

/*
 * Francis Beaudet <francis@macadamian.com>
 *
 * I moved the contents of this file to the ole/guid.c file.
 *
 * I know that the purpose of this file being here is that it would
 * separate the definitions reserved to the shell DLL into one place. *
 However, until the shell DLL is a real DLL, as long as it is *
 statically linked with the rest of wine, the initializer of it's * GUIDs
 will have to be in the same place as everybody else. This is * the same
 problem as with "real" Windows programs. */
