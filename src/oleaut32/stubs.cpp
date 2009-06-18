/* $Id: stubs.cpp,v 1.17 2004-10-12 10:11:10 cinc Exp $ */
/* 
 * Win32 COM/OLE stubs for OS/2
 * 
 * 7/9/99
 * 
 * Copyright 1999 David J. Raison
 * 
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 */

#include "oleaut32.h"
#include "olectl.h"
#include "oleauto.h"

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API OACreateTypeLib2()
{
    dprintf(("OLEAUT32: OACreateTypeLib2 - stub"));
    return S_OK;
}


//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarI2FromDisp(IDispatch * pdispIn, LCID lcid, SHORT * psOut)
{
    dprintf(("OLEAUT32: VarI2FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarI4FromDisp(IDispatch * pdispIn, LCID lcid, LONG * plOut)
{
    dprintf(("OLEAUT32: VarI4FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarR4FromDisp(IDispatch * pdispIn, LCID lcid, FLOAT * pfltOut)
{
    dprintf(("OLEAUT32: VarR4FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarR8FromDisp(IDispatch * pdispIn, LCID lcid, DOUBLE * pdblOut)
{
    dprintf(("OLEAUT32: VarR8FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDateFromDisp(IDispatch * pdispIn, LCID lcid, DATE * pdateOut)
{
    dprintf(("OLEAUT32: VarDateFromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarCyFromDisp(IDispatch * pdispIn, LCID lcid, CY * pcyOut)
{
    dprintf(("OLEAUT32: VarCyFromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarBstrFromDisp(IDispatch * pdispIn, LCID lcid, ULONG dwFlags, BSTR * pbstrOut)
{
    dprintf(("OLEAUT32: VarBstrFromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarBoolFromDisp(IDispatch * pdispIn, LCID lcid, VARIANT_BOOL * pboolOut)
{
    dprintf(("OLEAUT32: VarBoolFromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarUI1FromDisp(IDispatch * pdispIn, LCID lcid, BYTE * pbOut)
{
    dprintf(("OLEAUT32: VarUI1FromDisp - stub"));
    return S_OK;
}


//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API CreateTypeLib2
   (SYSKIND		syskind,
    LPCOLESTR		szFile,
    ICreateTypeLib2 **	ppctlib)
{
    dprintf(("OLEAUT32: CreateTypeLib2 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
void WIN32API ClearCustData(LPCUSTDATA pCustData)
{
    dprintf(("OLEAUT32: ClearCustData - stub"));
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromUI1(BYTE bIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromUI1 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromI2(SHORT uiIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromI2 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromI4(LONG lIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromI4 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromR4(FLOAT fltIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromR4 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromR8(DOUBLE dblIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromR8 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromDate(DATE dateIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromDate - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromCy(CY cyIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromCy - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromStr(OLECHAR *strIn, LCID lcid, ULONG dwFlags, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromStr - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromDisp(IDispatch *pdispIn, LCID lcid, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromBool(VARIANT_BOOL boolIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromBool - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromI1(CHAR cIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromI1 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromUI2(USHORT uiIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromUI2 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDecFromUI4(ULONG ulIn, DECIMAL *pdecOut)
{
    dprintf(("OLEAUT32: VarDecFromUI4 - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarI2FromDec(DECIMAL *pdecIn, SHORT *psOut)
{
    dprintf(("OLEAUT32: VarI2FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarI4FromDec(DECIMAL *pdecIn, LONG *plOut)
{
    dprintf(("OLEAUT32: VarI4FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarR4FromDec(DECIMAL *pdecIn, FLOAT *pfltOut)
{
    dprintf(("OLEAUT32: VarR4FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarR8FromDec(DECIMAL *pdecIn, DOUBLE *pdblOut)
{
    dprintf(("OLEAUT32: VarR8FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarDateFromDec(DECIMAL *pdecIn, DATE *pdateOut)
{
    dprintf(("OLEAUT32: VarDateFromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarCyFromDec(DECIMAL *pdecIn, CY *pcyOut)
{
    dprintf(("OLEAUT32: VarCyFromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarBstrFromDec(DECIMAL *pdecIn, LCID lcid, ULONG dwFlags, BSTR *pbstrOut)
{
    dprintf(("OLEAUT32: VarBstrFromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarBoolFromDec(DECIMAL *pdecIn, VARIANT_BOOL *pboolOut)
{
    dprintf(("OLEAUT32: VarBoolFromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarUI1FromDec(DECIMAL *pdecIn, BYTE *pbOut)
{
    dprintf(("OLEAUT32: VarUI1FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarI1FromDisp(IDispatch *pdispIn, LCID lcid, CHAR *pcOut)
{
    dprintf(("OLEAUT32: VarI1FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarI1FromDec(DECIMAL *pdecIn, CHAR *pcOut)
{
    dprintf(("OLEAUT32: VarI1FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarUI2FromDisp(IDispatch *pdispIn, LCID lcid, USHORT *puiOut)
{
    dprintf(("OLEAUT32: VarUI2FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarUI2FromDec(DECIMAL *pdecIn, USHORT *puiOut)
{
    dprintf(("OLEAUT32: VarUI2FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarUI4FromDisp(IDispatch *pdispIn, LCID lcid, ULONG *pulOut)
{
    dprintf(("OLEAUT32: VarUI4FromDisp - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VarUI4FromDec(DECIMAL *pdecIn, ULONG *pulOut)
{
    dprintf(("OLEAUT32: VarUI4FromDec - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//*****************************************************************************
unsigned long WIN32API  BSTR_UserSize(unsigned long *, unsigned long, BSTR * )
{
    dprintf(("OLEAUT32: BSTR_UserSize - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
unsigned char * WIN32API  BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR * )
{
    dprintf(("OLEAUT32: BSTR_UserMarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
unsigned char * WIN32API  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * )
{
    dprintf(("OLEAUT32: BSTR_UserUnmarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
void WIN32API  BSTR_UserFree(unsigned long *, BSTR * )
{
    dprintf(("OLEAUT32: BSTR_UserFree - stub"));
}

//*****************************************************************************
//*****************************************************************************
unsigned long WIN32API  VARIANT_UserSize(unsigned long *, unsigned long, VARIANT * )
{
    dprintf(("OLEAUT32: VARIANT_UserSize - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
unsigned char * WIN32API  VARIANT_UserMarshal(unsigned long *, unsigned char *, VARIANT * )
{
    dprintf(("OLEAUT32: VARIANT_UserMarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
unsigned char * WIN32API  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * )
{
    dprintf(("OLEAUT32: VARIANT_UserUnmarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
void WIN32API  VARIANT_UserFree(unsigned long *, VARIANT * )
{
    dprintf(("OLEAUT32: VARIANT_UserFree - stub"));
}

//*****************************************************************************
//*****************************************************************************
unsigned long WIN32API LPSAFEARRAY_UserSize(unsigned long *, unsigned long, LPSAFEARRAY *)
{
    dprintf(("OLEAUT32: LPSAFEARRAY_UserSize - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
unsigned char * WIN32API LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY *)
{
    dprintf(("OLEAUT32: LPSAFEARRAY_UserMarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
unsigned char * WIN32API LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY *)
{
    dprintf(("OLEAUT32: LPSAFEARRAY_UserUnmarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
void WIN32API LPSAFEARRAY_UserFree(unsigned long *, LPSAFEARRAY *)
{
    dprintf(("OLEAUT32: LPSAFEARRAY_UserFree - stub"));
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API LPSAFEARRAY_Size()
{
    dprintf(("OLEAUT32: LPSAFEARRAY_Size - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API LPSAFEARRAY_Marshal()
{
    dprintf(("OLEAUT32: LPSAFEARRAY_Marshal - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API LPSAFEARRAY_Unmarshal()
{
    dprintf(("OLEAUT32: LPSAFEARRAY_Unmarshal - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API GetAltMonthNames(LCID lcid, LPOLESTR * * prgp)
{
    dprintf(("OLEAUT32: GetAltMonthNames - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserHWND_from_local()
{
    dprintf(("OLEAUT32: UserHWND_from_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserHWND_to_local()
{
    dprintf(("OLEAUT32: UserHWND_to_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserHWND_free_inst()
{
    dprintf(("OLEAUT32: UserHWND_free_inst - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserHWND_free_local()
{
    dprintf(("OLEAUT32: UserHWND_free_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserBSTR_from_local()
{
    dprintf(("OLEAUT32: UserBSTR_from_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserBSTR_to_local()
{
    dprintf(("OLEAUT32: UserBSTR_to_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserBSTR_free_inst()
{
    dprintf(("OLEAUT32: UserBSTR_free_inst - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserBSTR_free_local()
{
    dprintf(("OLEAUT32: UserBSTR_free_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserVARIANT_from_local()
{
    dprintf(("OLEAUT32: UserVARIANT_from_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserVARIANT_to_local()
{
    dprintf(("OLEAUT32: UserVARIANT_to_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserVARIANT_free_inst()
{
    dprintf(("OLEAUT32: UserVARIANT_free_inst - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserVARIANT_free_local()
{
    dprintf(("OLEAUT32: UserVARIANT_free_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserEXCEPINFO_from_local()
{
    dprintf(("OLEAUT32: UserEXCEPINFO_from_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserEXCEPINFO_to_local()
{
    dprintf(("OLEAUT32: UserEXCEPINFO_to_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserEXCEPINFO_free_inst()
{
    dprintf(("OLEAUT32: UserEXCEPINFO_free_inst - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserEXCEPINFO_free_local()
{
    dprintf(("OLEAUT32: UserEXCEPINFO_free_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserMSG_from_local()
{
    dprintf(("OLEAUT32: UserMSG_from_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserMSG_to_local()
{
    dprintf(("OLEAUT32: UserMSG_to_local - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserMSG_free_inst()
{
    dprintf(("OLEAUT32: UserMSG_free_inst - stub"));
    return S_OK;
}

//*****************************************************************************
// TODO: Parameters
//*****************************************************************************
HRESULT WIN32API UserMSG_free_local()
{
    dprintf(("OLEAUT32: UserMSG_free_local - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API VectorFromBstr (BSTR bstr, SAFEARRAY ** ppsa)
{
    dprintf(("OLEAUT32: VectorFromBstr - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HRESULT WIN32API BstrFromVector (SAFEARRAY *psa, BSTR *pbstr)
{
    dprintf(("OLEAUT32: BstrFromVector - stub"));
    return S_OK;
}

//*****************************************************************************
//*****************************************************************************
HCURSOR WIN32API OleIconToCursor(HINSTANCE hinstExe, HICON hIcon)
{
    dprintf(("OLEAUT32: OleIconToCursor - stub"));
    return S_OK;
}
//*****************************************************************************
//*****************************************************************************
/***********************************************************************
 * OleCreatePropertyFrameIndirect (OLEAUT32.416)
 */
HRESULT WINAPI OleCreatePropertyFrameIndirect( LPOCPFIPARAMS lpParams)
{
        dprintf(("not implemented"));
	return S_OK;
}
 
/***********************************************************************
 * OleCreatePropertyFrame (OLEAUT32.417)
 */
HRESULT WINAPI OleCreatePropertyFrame(
    HWND hwndOwner, UINT x, UINT y, LPCOLESTR lpszCaption,ULONG cObjects,
    LPUNKNOWN* ppUnk, ULONG cPages, LPCLSID pPageClsID, LCID lcid, 
    DWORD dwReserved, LPVOID pvReserved )
{
        dprintf(("not implemented"));
	return S_OK;
}

#ifdef __WIN32OS2__

// ----------------------------------------------------------------------
// OleLoadPictureFile
// ----------------------------------------------------------------------
HRESULT WIN32API OleLoadPictureFile(VARIANT varFileName, LPDISPATCH* lplpdispPicture)
{
    dprintf(("OLEAUT32: OleLoadPictureFile - stub"));
    return S_OK;
}

// ----------------------------------------------------------------------
// OleSavePictureFile
// ----------------------------------------------------------------------
HRESULT WIN32API OleSavePictureFile(LPDISPATCH lpdispPicture,
    BSTR bstrFileName)
{
    dprintf(("OLEAUT32: OleSavePictureFile - stub"));
    return S_OK;
}

// ----------------------------------------------------------------------
// OleLoadPicturePath
// ----------------------------------------------------------------------
HRESULT WIN32API OleLoadPicturePath
   (LPOLESTR  		szURLorPath,
    LPUNKNOWN 		punkCaller,
    DWORD     		dwReserved,
    OLE_COLOR 		clrReserved,
    REFIID    		riid,
    LPVOID *  		ppvRet )
{
    dprintf(("OLEAUT32: OleLoadPicturePath - stub"));
    return S_OK;
}
#endif
