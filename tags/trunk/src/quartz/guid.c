/* $Id: guid.c,v 1.3 2001-09-05 13:36:35 bird Exp $ */
// This causes all the DEFINE_GUID macros to instantiate their GUID's
#define INITGUID

#include "config.h"

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winerror.h"
#include "wine/obj_base.h"
#include "wine/obj_oleaut.h"
#include "strmif.h"
#include "uuids.h"
#include "control.h"