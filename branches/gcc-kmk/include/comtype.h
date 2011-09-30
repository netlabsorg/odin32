/* $Id: comtype.h,v 1.2 1999-05-27 15:17:57 phaller Exp $ */

#ifndef __COMTYPE_H__
#define __COMTYPE_H__

#define _WIN32

#define FAR           
#define NEAR 
#define __RPC_FAR

#undef PASCAL
#define PASCAL __stdcall

//#define E_OUTOFMEMORY                    0x8007000EL

//
// Create an HRESULT value from component pieces
//

#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif

typedef struct  _GUID
{
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[ 8 ];
} GUID, *LPGUID;

#define CDECL           

// macros to define byte pattern for a GUID.
//      Example: DEFINE_GUID(GUID_XXX, a, b, c, ...);
//
// Each dll/exe must initialize the GUIDs once.  This is done in one of
// two ways.  If you are not using precompiled headers for the file(s) which
// initializes the GUIDs, define INITGUID before including objbase.h.  This
// is how OLE builds the initialized versions of the GUIDs which are included
// in ole2.lib.  The GUIDs in ole2.lib are all defined in the same text
// segment GUID_TEXT.
//
// The alternative (which some versions of the compiler don't handle properly;
// they wind up with the initialized GUIDs in a data, not a text segment),
// is to use a precompiled version of objbase.h and then include initguid.h
// after objbase.h followed by one or more of the guid defintion files.
#ifdef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID CDECL name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

#define DEFINE_OLEGUID(name, l, w1, w2) \
    DEFINE_GUID(name, l, w1, w2, 0xC0,0,0,0,0,0,0,0x46)
#else
    #define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

    #define DEFINE_OLEGUID(name, l, w1, w2)
#endif

#define STDMETHODCALLTYPE       __stdcall
#define STDMETHODVCALLTYPE      __cdecl

#define STDAPICALLTYPE          __stdcall
#define STDAPIVCALLTYPE         __cdecl

#define interface               struct

#define STDMETHOD(method)       HRESULT (STDMETHODCALLTYPE * method)
#define STDMETHOD_(type,method) type (STDMETHODCALLTYPE * method)


#define PURE
//Was INTERFACE; IBM VAC++ doesn't behave like the MS compiler: (bug?)
//#undef INTERFACE
//#define INTERFACE IDirectDraw
//Use INTERFACE
//...
//#undef INTERFACE
//#define INTERFACE IDirectSurface
//Use INTERFACE
//The first INTERFACE gets translated to IDirectSurface instead of IDirectDraw
#define THIS_                   VOID FAR* This,
#define THIS                    VOID FAR* This
#define DECLARE_INTERFACE(iface)    typedef interface iface { \
                                    struct iface##Vtbl FAR* lpVtbl; \
                                } iface; \
                                typedef struct iface##Vtbl iface##Vtbl; \
                                struct iface##Vtbl
#define DECLARE_INTERFACE_(iface, baseiface)    DECLARE_INTERFACE(iface)

#define IUnknown 	void


#define __IID_DEFINED__
			/* size is 16 */
typedef GUID IID;
			/* size is 4 */
typedef IID __RPC_FAR *LPIID;
#define IID_NULL            GUID_NULL
//#define IsEqualIID(riid1, riid2) IsEqualGUID(riid1, riid2)
			/* size is 16 */
typedef GUID CLSID;
			/* size is 4 */
typedef CLSID __RPC_FAR *LPCLSID;
#define CLSID_NULL          GUID_NULL
//#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)

#if defined(__cplusplus)
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#define REFGUID             const GUID &
#endif // !_REFGUID_DEFINED
#ifndef _REFIID_DEFINED
#define _REFIID_DEFINED
#define REFIID              const IID &
#endif // !_REFIID_DEFINED
#ifndef _REFCLSID_DEFINED
#define _REFCLSID_DEFINED
#define REFCLSID            const CLSID &
#endif // !_REFCLSID_DEFINED
#else // !__cplusplus
#ifndef _REFGUID_DEFINED
#define _REFGUID_DEFINED
#define REFGUID             const GUID * const
#endif // !_REFGUID_DEFINED
#ifndef _REFIID_DEFINED
#define _REFIID_DEFINED
#define REFIID              const IID * const
#endif // !_REFIID_DEFINED
#ifndef _REFCLSID_DEFINED
#define _REFCLSID_DEFINED
#define REFCLSID            const CLSID * const
#endif // !_REFCLSID_DEFINED
#endif // !__cplusplus


inline BOOL IsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
    return !memcmp(&rguid1, &rguid2, sizeof(GUID));
}

inline BOOL operator==(GUID &a, GUID &b)
{
    return !memcmp((const void *)&a, (const void *)&b, sizeof(GUID));
}

inline BOOL operator!=(GUID &a, GUID &b)
{
    return memcmp((const void *)&a, (const void *)&b, sizeof(GUID));
}

#endif