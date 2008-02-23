//$Id: region.h,v 1.5 2004-03-25 14:52:32 sandervl Exp $
#ifndef __REGION_H__
#define __REGION_H__

#include <dcdata.h>

BOOL InitRegionSpace();
void DestroyRegionSpace();

BOOL WIN32API OSLibDeleteRegion(HANDLE hRegion);

HRGN GdiCopyClipRgn(pDCData pHps);
BOOL GdiDestroyRgn(pDCData pHps, HRGN hrgnClip);


LONG hdcHeight(HWND hwnd, pDCData pHps);
LONG hdcWidth(HWND hwnd, pDCData pHps);

#endif //__REGION_H__