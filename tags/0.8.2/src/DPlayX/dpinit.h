// $Id: dpinit.h,v 1.2 2000-09-24 22:47:39 hugh Exp $

#ifndef __WINE_DPINIT_H
#define __WINE_DPINIT_H

#include "wtypes.h"
#include "dplay_global.h"

extern HRESULT DP_CreateInterface( REFIID riid, LPVOID* ppvObj );
extern HRESULT DPL_CreateInterface( REFIID riid, LPVOID* ppvObj );
extern HRESULT DPSP_CreateInterface( REFIID riid, LPVOID* ppvObj,
                                     IDirectPlay2Impl* dp );

#endif
