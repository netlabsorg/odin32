/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 */
#ifndef __OLE32_H__
#define __OLE32_H__

typedef ULONG           MONIKER, *PMONIKER, *LPMONIKER;
typedef ULONG           IStorage;
typedef ULONG           IStream;
typedef ULONG           IOleClientSite;
typedef ULONG           IDataAdviseHolder;
typedef ULONG           IOleAdviseHolder;
typedef ULONG           IDropSource;
typedef ULONG           IDataObject;
typedef ULONG           ILockBytes;
typedef ULONG           IDropTarget;
typedef ULONG           IPersistStorage;
typedef IUnknown        UNKNOWN, *PUNKNOWN, *LPUNKNOWN;
typedef IOleClientSite  OLECLIENTSITE, *POLECLIENTSITE, *LPOLECLIENTSITE;
typedef ULONG           STORAGE, *PSTORAGE, *LPSTORAGE;
typedef IDataObject     DATAOBJECT, *PDATAOBJECT, *LPDATAOBJECT;
typedef const WCHAR *   LPCOLESTR;
typedef WCHAR *         LPOLESTR;
typedef ULONG           COSERVERINFO;
typedef ULONG           STREAM, *PSTREAM, *LPSTREAM;
typedef ULONG           MALLOC, *PMALLOC, *LPMALLOC;
typedef ULONG           MARSHALL, *PMARSHALL, *LPMARSHALL;
typedef ULONG           MALLOCSPY, *PMALLOCSPY, *LPMALLOCSPY;
typedef ULONG           MESSAGEFILTER, *PMESSAGEFILTER, *LPMESSAGEFILTER;
typedef ULONG           BC, *PBC, *LPBC;
typedef ULONG           RUNNINGOBJECTTABLE, *PRUNNINGOBJECTTABLE, *LPRUNNINGOBJECTTABLE;
typedef ULONG           OLESTREAM, *POLESTREAM, *LPOLESTREAM;
typedef ULONG           CLIPFORMAT;
typedef ULONG           DVTARGETDEVICE;
typedef ULONG           STGMEDIUM;
typedef ULONG           FORMATETC, *PFORMATETC, *LPFORMATETC;
typedef ULONG           CLASSFACTORY, *PCLASSFACTORY, *LPCLASSFACTORY;
typedef ULONG           SNB;
typedef HMENU           HOLEMENU;
typedef ULONG           OLEMENUGROUPWIDTHS, *POLEMENUGROUPWIDTHS, *LPOLEMENUGROUPWIDTHS;
typedef ULONG           OLEINPLACEFRAME, *POLEINPLACEFRAME, *LPOLEINPLACEFRAME;
typedef ULONG           OLEINPLACEACTIVEOBJECT, *POLEINPLACEACTIVEOBJECT, *LPOLEINPLACEACTIVEOBJECT;
typedef ULONG           OLEINPLACEFRAMEINFO, *POLEINPLACEFRAMEINFO, *LPOLEINPLACEFRAMEINFO;
typedef ULONG           CRECT, *PCRECT, *LPCRECT;
typedef ULONG           ENUMFORMATETC, *PENUMFORMATETC, *LPENUMFORMATETC;
typedef ULONG           ENUMOLEVERB, *PENUMOLEVERB, *LPENUMOLEVERB;
typedef ULONG           OLEOBJECT, *POLEOBJECT, *LPOLEOBJECT;

#endif