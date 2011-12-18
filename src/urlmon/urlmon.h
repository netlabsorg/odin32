/*
 * urlmon.h
 */

#ifndef __WINE_URLMON_H
#define __WINE_URLMON_H

extern "C"
HRESULT CreateURLMoniker(IMoniker *pmkContext, LPWSTR szURL, IMoniker **ppmk);

#endif /* __WINE_URLMON_H */

