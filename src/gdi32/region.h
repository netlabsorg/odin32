//$Id: region.h,v 1.4 2004-01-11 11:42:22 sandervl Exp $
#ifndef __REGION_H__
#define __REGION_H__

#include <dcdata.h>

BOOL InitRegionSpace();
void DestroyRegionSpace();

BOOL WIN32API OSLibDeleteRegion(HANDLE hRegion);

HRGN GdiCopyClipRgn(pDCData pHps);
BOOL GdiDestroyRgn(pDCData pHps, HRGN hrgnClip);

#endif //__REGION_H__