/* $Id: iPicture.h,v 1.1 2001-08-10 19:24:39 sandervl Exp $ */
/* 
 * OLE Picture functions private header
 *
 * 12/11/99 - New Code
 * 
 * Copyright 1999 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#if !defined(IPICTURE_INCLUDED)
#define IPICTURE_INCLUDED

class IPictureStrat;	// Forward defn.

typedef struct IPictureImpl {

    // IUnknown, IPicture
    ICOM_VTABLE(IPicture) *     	lpvtbl1;

    // IDispatch, IPictureDisp
    ICOM_VTABLE(IDispatch) *		lpvtbl2;

    // IPersistStream
    ICOM_VTABLE(IPersistStream) *	lpvtbl3;

    ULONG		ref;		// Reference counter...

    IPictureStrat *	pStrat;		// Current object strategy.

    union
    {
        struct
        {
            HBITMAP   hbitmap;        // Bitmap
            HPALETTE  hpal;           // Accompanying palette
        } bmp;

        struct
        {
            HMETAFILE hmeta;          // Metafile
            int       xExt;
            int       yExt;           // Extent
        } wmf;

        struct
        {
            HICON hicon;              // Icon
        } icon;

        struct
        {
            HENHMETAFILE hemf;        // Enhanced Metafile
        } emf;
    } u;

    BOOL		fKeepOriginalFormat;
    BOOL		fOwn;
    HDC			hCurDC;
    ULONG		lAttrib;

} IPictureImpl;

class IPictureStrat
{
public:
    virtual HRESULT get_Handle(OLE_HANDLE * pHandle) = 0;
    virtual HRESULT get_hPal(OLE_HANDLE * phPal) = 0;
    virtual HRESULT get_Type(SHORT * pType) = 0;
    virtual HRESULT get_Width(OLE_XSIZE_HIMETRIC * pWidth) = 0;
    virtual HRESULT get_Height(OLE_YSIZE_HIMETRIC * pHeight) = 0;
    virtual HRESULT Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				    OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				    OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				    LPCRECT pRcWBounds) = 0;
    virtual HRESULT set_hPal(OLE_HANDLE hPal) = 0;
    virtual HRESULT SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize) = 0;
    virtual HRESULT get_Attributes(DWORD * pDwAttr) = 0;

    virtual ~IPictureStrat() {};

protected:
    IPictureStrat(IPictureImpl * a_pPicture) {pPicture = a_pPicture; }

    IPictureImpl *	pPicture;

private:

};

// ======================================================================
// Uninitialised Picture
// ======================================================================

class IPictureNone : public IPictureStrat
{
public:
    IPictureNone(IPictureImpl * a_pPicture) : IPictureStrat(a_pPicture) {}
    HRESULT get_Handle(OLE_HANDLE * pHandle);
    HRESULT get_hPal(OLE_HANDLE * phPal);
    HRESULT get_Type(SHORT * pType);
    HRESULT get_Width(OLE_XSIZE_HIMETRIC * pWidth);
    HRESULT get_Height(OLE_YSIZE_HIMETRIC * pHeight);
    HRESULT Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				    OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				    OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				    LPCRECT pRcWBounds);
    HRESULT set_hPal(OLE_HANDLE hPal);
    HRESULT SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize);
    HRESULT get_Attributes(DWORD * pDwAttr);

protected:

private:

};


// ======================================================================
// Bitmap Picture
// ======================================================================

class IPictureBmp : public IPictureStrat
{
public:
    IPictureBmp(IPictureImpl * a_pPicture, HBITMAP hBitmap, HPALETTE hpal);
    ~IPictureBmp();
    HRESULT get_Handle(OLE_HANDLE * pHandle);
    HRESULT get_hPal(OLE_HANDLE * phPal);
    HRESULT get_Type(SHORT * pType);
    HRESULT get_Width(OLE_XSIZE_HIMETRIC * pWidth);
    HRESULT get_Height(OLE_YSIZE_HIMETRIC * pHeight);
    HRESULT Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				    OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				    OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				    LPCRECT pRcWBounds);
    HRESULT set_hPal(OLE_HANDLE hPal);
    HRESULT SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize);
    HRESULT get_Attributes(DWORD * pDwAttr);

protected:

private:

};


// ======================================================================
// Icon Picture
// ======================================================================

class IPictureIcon : public IPictureStrat
{
public:
    IPictureIcon(IPictureImpl * a_pPicture, HICON hicon);
    ~IPictureIcon();
    HRESULT get_Handle(OLE_HANDLE * pHandle);
    HRESULT get_hPal(OLE_HANDLE * phPal);
    HRESULT get_Type(SHORT * pType);
    HRESULT get_Width(OLE_XSIZE_HIMETRIC * pWidth);
    HRESULT get_Height(OLE_YSIZE_HIMETRIC * pHeight);
    HRESULT Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				    OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				    OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				    LPCRECT pRcWBounds);
    HRESULT set_hPal(OLE_HANDLE hPal);
    HRESULT get_CurDC(HDC * phDC);
    HRESULT put_KeepOriginalFormat(BOOL Keep);
    HRESULT SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize);
    HRESULT get_Attributes(DWORD * pDwAttr);

protected:

private:

};

// ======================================================================
// Metafile Picture
// ======================================================================

class IPictureMeta : public IPictureStrat
{
public:
    IPictureMeta(IPictureImpl * a_pPicture, HMETAFILE hmeta, int xExt, int yExt);
    ~IPictureMeta();
    HRESULT get_Handle(OLE_HANDLE * pHandle);
    HRESULT get_hPal(OLE_HANDLE * phPal);
    HRESULT get_Type(SHORT * pType);
    HRESULT get_Width(OLE_XSIZE_HIMETRIC * pWidth);
    HRESULT get_Height(OLE_YSIZE_HIMETRIC * pHeight);
    HRESULT Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				    OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				    OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				    LPCRECT pRcWBounds);
    HRESULT set_hPal(OLE_HANDLE hPal);
    HRESULT SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize);
    HRESULT get_Attributes(DWORD * pDwAttr);

protected:

private:

};

// ======================================================================
// Enhanced Metafile Picture
// ======================================================================

class IPictureEmf : public IPictureStrat
{
public:
    IPictureEmf(IPictureImpl * a_pPicture, HENHMETAFILE hemf);
    ~IPictureEmf();
    HRESULT get_Handle(OLE_HANDLE * pHandle);
    HRESULT get_hPal(OLE_HANDLE * phPal);
    HRESULT get_Type(SHORT * pType);
    HRESULT get_Width(OLE_XSIZE_HIMETRIC * pWidth);
    HRESULT get_Height(OLE_YSIZE_HIMETRIC * pHeight);
    HRESULT Render(HDC hdc, LONG x, LONG y, LONG cx, LONG cy,
				    OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
				    OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
				    LPCRECT pRcWBounds);
    HRESULT set_hPal(OLE_HANDLE hPal);
    HRESULT SaveAsFile(LPSTREAM pStream, BOOL fSaveMemCopy, LONG * pCbSize);
    HRESULT get_Attributes(DWORD * pDwAttr);

protected:

private:

};

// Access macros
#define _ICOM_THIS(class, name) class* This = (class*)name;
#define _ICOM_THIS_From_IDispatch(class, name) class * This = (class*)(((char*)name)-sizeof(void*));
#define _ICOM_THIS_From_IPersistStream(class, name) class * This = (class*)(((char*)name)-2*sizeof(void*));

#endif /* IPICTURE_INCLUDED */
