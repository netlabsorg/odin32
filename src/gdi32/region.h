//$Id: region.h,v 1.3 2001-10-16 11:40:19 sandervl Exp $
#ifndef __REGION_H__
#define __REGION_H__

BOOL InitRegionSpace();
void DestroyRegionSpace();

BOOL WIN32API OSLibDeleteRegion(HANDLE hRegion);

#endif //__REGION_H__