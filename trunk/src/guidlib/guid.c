/* $Id: guid.c,v 1.2 2001-12-06 10:15:21 sandervl Exp $ */
#define ICOM_CINTERFACE 1
#include <odin.h>

#define CINTERFACE 1
#define INITGUID
#include <wine\obj_oleview.h>
#include "initguid.h"

#include "shlwapi.h"
#include "shlguid.h"
#include "shlobj.h"
#include "docobj.h"
#include "olectl.h"
#include "dinput.h"
//#include "..\shell32\shellfolder.h"

#include "wine/obj_inplace.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_surrogate.h"
#include "wine/obj_errorinfo.h"
#include "wine/obj_oleview.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_cache.h"
#include "wine\obj_oleaut.h"
#include "wine\obj_olefont.h"

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
#include <wine/obj_serviceprovider.h>
#include <wine/unicode.h>

#include <misc.h>

DEFINE_GUID(IID_ISFHelper, 0x1fe68efbL, 0x1874, 0x9812, 0x56, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
DEFINE_GUID(GUID_NULL,   0,0,0,0,0,0,0,0,0,0,0);
