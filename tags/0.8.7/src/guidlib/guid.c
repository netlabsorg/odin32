/* $Id: guid.c,v 1.8 2004-07-06 16:57:30 sandervl Exp $ */
#include <odin.h>

#define CINTERFACE
#define INITGUID
#include "wine/obj_base.h"

#include "shlwapi.h"
#include "shlguid.h"
#include "shlobj.h"
#include "docobj.h"
#include "olectl.h"
#include "dinput.h"
//#include "../shell32/shellfolder.h"

#include "wine/obj_inplace.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_surrogate.h"
#include "wine/obj_errorinfo.h"
#include "wine/obj_oleview.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_cache.h"
#include "wine/obj_oleaut.h"
#include "wine/obj_olefont.h"

#include "wine/obj_oleview.h"
#include "wine/obj_dragdrop.h"
#include "wine/obj_inplace.h"
#include "wine/obj_channel.h"
#include "wine/obj_marshal.h"
#include "wine/obj_control.h"
#include "wine/obj_shellfolder.h"
#include "wine/obj_shelllink.h"
#include "wine/obj_contextmenu.h"
#include "wine/obj_commdlgbrowser.h"
#include "wine/obj_extracticon.h"
#include "wine/obj_shellextinit.h"
#include "wine/obj_shellbrowser.h"
#include "wine/obj_serviceprovider.h"
#include "wine/obj_queryassociations.h"
#include "wine/obj_webbrowser.h"
#include "wine/unicode.h"

#include <misc.h>

DEFINE_GUID(IID_ISFHelper, 0x1fe68efbL, 0x1874, 0x9812, 0x56, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
DEFINE_GUID(GUID_NULL,   0,0,0,0,0,0,0,0,0,0,0);

#define DEFINE_AVIGUID(name, l, w1, w2) \
    DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)

DEFINE_AVIGUID(IID_IAVIFile,            0x00020020, 0, 0);
DEFINE_AVIGUID(IID_IAVIStream,          0x00020021, 0, 0);
DEFINE_AVIGUID(IID_IAVIStreaming,       0x00020022, 0, 0);
DEFINE_AVIGUID(IID_IGetFrame,           0x00020023, 0, 0);
DEFINE_AVIGUID(IID_IAVIEditStream,      0x00020024, 0, 0);

DEFINE_AVIGUID(CLSID_AVIFile,           0x00020000, 0, 0);

DEFINE_GUID(IID_INotifyDBEvents,
0xdb526cc0, 0xd188, 0x11cd, 0xad, 0x48, 0x0, 0xaa, 0x0, 0x3c, 0x9c, 0xb6);
