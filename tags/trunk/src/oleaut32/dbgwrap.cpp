#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <wincon.h>
#include <winthunk.h>
#include <winnls.h>
#include <ntddk.h>
#include <heapstring.h>
#include "oleaut32.h"
#include "wine/obj_errorinfo.h"
#include "wine/obj_oleaut.h"

#define DBGWRAP_MODULE "OLEAUT32"
#include <dbgwrap.h>



NODEF_DEBUGWRAP12(OLEAUT32_DllGetClassObject)
DEBUGWRAP4(SysAllocString)
DEBUGWRAP8(SysReAllocString)
DEBUGWRAP8(SysAllocStringLen)
DEBUGWRAP12(SysReAllocStringLen)
DEBUGWRAP4(SysFreeString)
DEBUGWRAP4(SysStringLen)
DEBUGWRAP4(VariantInit)
DEBUGWRAP4(VariantClear)
DEBUGWRAP8(VariantCopy)
DEBUGWRAP8(VariantCopyInd)
DEBUGWRAP16(VariantChangeType)
DEBUGWRAP16(VariantTimeToDosDateTime)
DEBUGWRAP12(DosDateTimeToVariantTime)
DEBUGWRAP12(SafeArrayCreate)
DEBUGWRAP4(SafeArrayDestroy)
DEBUGWRAP4(SafeArrayGetDim)
DEBUGWRAP4(SafeArrayGetElemsize)
DEBUGWRAP12(SafeArrayGetUBound)
DEBUGWRAP12(SafeArrayGetLBound)
DEBUGWRAP4(SafeArrayLock)
DEBUGWRAP4(SafeArrayUnlock)
DEBUGWRAP8(SafeArrayAccessData)
DEBUGWRAP4(SafeArrayUnaccessData)
DEBUGWRAP12(SafeArrayGetElement)
DEBUGWRAP12(SafeArrayPutElement)
DEBUGWRAP8(SafeArrayCopy)
DEBUGWRAP20(DispGetParam)
DEBUGWRAP16(DispGetIDsOfNames)
DEBUGWRAP32(DispInvoke)
DEBUGWRAP12(CreateDispTypeInfo)
DEBUGWRAP16(CreateStdDispatch)
NODEF_DEBUGWRAP16(RegisterActiveObject)
NODEF_DEBUGWRAP8(RevokeActiveObject)
NODEF_DEBUGWRAP12(GetActiveObject)
DEBUGWRAP8(SafeArrayAllocDescriptor)
DEBUGWRAP4(SafeArrayAllocData)
DEBUGWRAP4(SafeArrayDestroyDescriptor)
DEBUGWRAP4(SafeArrayDestroyData)
DEBUGWRAP8(SafeArrayRedim)
NODEF_DEBUGWRAP0(OACreateTypeLib2)
DEBUGWRAP20(VarParseNumFromStr)
DEBUGWRAP16(VarNumFromParseNum)
DEBUGWRAP8(VarI2FromUI1)
DEBUGWRAP8(VarI2FromI4)
DEBUGWRAP8(VarI2FromR4)
DEBUGWRAP12(VarI2FromR8)
DEBUGWRAP12(VarI2FromCy)
DEBUGWRAP12(VarI2FromDate)
DEBUGWRAP16(VarI2FromStr)
DEBUGWRAP12(VarI2FromDisp)
DEBUGWRAP8(VarI2FromBool)
DEBUGWRAP8(VarI4FromUI1)
DEBUGWRAP8(VarI4FromI2)
DEBUGWRAP8(VarI4FromR4)
DEBUGWRAP12(VarI4FromR8)
DEBUGWRAP12(VarI4FromCy)
DEBUGWRAP12(VarI4FromDate)
DEBUGWRAP16(VarI4FromStr)
DEBUGWRAP12(VarI4FromDisp)
DEBUGWRAP8(VarI4FromBool)
DEBUGWRAP8(VarR4FromUI1)
DEBUGWRAP8(VarR4FromI2)
DEBUGWRAP8(VarR4FromI4)
DEBUGWRAP12(VarR4FromR8)
DEBUGWRAP12(VarR4FromCy)
DEBUGWRAP12(VarR4FromDate)
DEBUGWRAP16(VarR4FromStr)
DEBUGWRAP12(VarR4FromDisp)
DEBUGWRAP8(VarR4FromBool)
DEBUGWRAP8(VarR8FromUI1)
DEBUGWRAP8(VarR8FromI2)
DEBUGWRAP8(VarR8FromI4)
DEBUGWRAP8(VarR8FromR4)
DEBUGWRAP12(VarR8FromCy)
DEBUGWRAP12(VarR8FromDate)
DEBUGWRAP16(VarR8FromStr)
DEBUGWRAP12(VarR8FromDisp)
DEBUGWRAP8(VarR8FromBool)
DEBUGWRAP8(VarDateFromUI1)
DEBUGWRAP8(VarDateFromI2)
DEBUGWRAP8(VarDateFromI4)
DEBUGWRAP8(VarDateFromR4)
DEBUGWRAP12(VarDateFromR8)
DEBUGWRAP12(VarDateFromCy)
DEBUGWRAP16(VarDateFromStr)
DEBUGWRAP12(VarDateFromDisp)
DEBUGWRAP8(VarDateFromBool)
DEBUGWRAP8(VarCyFromUI1)
DEBUGWRAP8(VarCyFromI2)
DEBUGWRAP8(VarCyFromI4)
DEBUGWRAP8(VarCyFromR4)
DEBUGWRAP12(VarCyFromR8)
DEBUGWRAP12(VarCyFromDate)
DEBUGWRAP16(VarCyFromStr)
DEBUGWRAP12(VarCyFromDisp)
DEBUGWRAP8(VarCyFromBool)
DEBUGWRAP16(VarBstrFromUI1)
DEBUGWRAP16(VarBstrFromI2)
DEBUGWRAP16(VarBstrFromI4)
DEBUGWRAP16(VarBstrFromR4)
DEBUGWRAP20(VarBstrFromR8)
DEBUGWRAP20(VarBstrFromCy)
DEBUGWRAP20(VarBstrFromDate)
DEBUGWRAP16(VarBstrFromDisp)
DEBUGWRAP16(VarBstrFromBool)
DEBUGWRAP8(VarBoolFromUI1)
DEBUGWRAP8(VarBoolFromI2)
DEBUGWRAP8(VarBoolFromI4)
DEBUGWRAP8(VarBoolFromR4)
DEBUGWRAP12(VarBoolFromR8)
DEBUGWRAP12(VarBoolFromDate)
DEBUGWRAP12(VarBoolFromCy)
DEBUGWRAP16(VarBoolFromStr)
DEBUGWRAP12(VarBoolFromDisp)
DEBUGWRAP8(VarUI1FromI2)
DEBUGWRAP8(VarUI1FromI4)
DEBUGWRAP8(VarUI1FromR4)
DEBUGWRAP12(VarUI1FromR8)
DEBUGWRAP12(VarUI1FromCy)
DEBUGWRAP12(VarUI1FromDate)
DEBUGWRAP16(VarUI1FromStr)
DEBUGWRAP12(VarUI1FromDisp)
DEBUGWRAP8(VarUI1FromBool)
DEBUGWRAP32(DispCallFunc)
DEBUGWRAP20(VariantChangeTypeEx)
DEBUGWRAP12(SafeArrayPtrOfIndex)
DEBUGWRAP4(SysStringByteLen)
DEBUGWRAP8(SysAllocStringByteLen)
DEBUGWRAP12(CreateTypeLib)
DEBUGWRAP8(LoadTypeLib)
DEBUGWRAP20(LoadRegTypeLib)
DEBUGWRAP12(RegisterTypeLib)
DEBUGWRAP20(QueryPathOfRegTypeLib)
DEBUGWRAP12(LHashValOfNameSys)
DEBUGWRAP12(LHashValOfNameSysA)
NODEF_DEBUGWRAP0(OaBuildVersion)
NODEF_DEBUGWRAP4(ClearCustData)
DEBUGWRAP12(CreateTypeLib2)
DEBUGWRAP12(LoadTypeLibEx)
DEBUGWRAP8(SystemTimeToVariantTime)
DEBUGWRAP12(VariantTimeToSystemTime)
DEBUGWRAP20(UnRegisterTypeLib)
DEBUGWRAP8(VarDecFromUI1)
DEBUGWRAP8(VarDecFromI2)
DEBUGWRAP8(VarDecFromI4)
DEBUGWRAP8(VarDecFromR4)
DEBUGWRAP12(VarDecFromR8)
DEBUGWRAP12(VarDecFromDate)
DEBUGWRAP12(VarDecFromCy)
DEBUGWRAP16(VarDecFromStr)
DEBUGWRAP12(VarDecFromDisp)
DEBUGWRAP8(VarDecFromBool)
DEBUGWRAP8(GetErrorInfo)
DEBUGWRAP8(SetErrorInfo)
DEBUGWRAP4(CreateErrorInfo)
DEBUGWRAP8(VarI2FromI1)
DEBUGWRAP8(VarI2FromUI2)
DEBUGWRAP8(VarI2FromUI4)
DEBUGWRAP8(VarI2FromDec)
DEBUGWRAP8(VarI4FromI1)
DEBUGWRAP8(VarI4FromUI2)
DEBUGWRAP8(VarI4FromUI4)
DEBUGWRAP8(VarI4FromDec)
DEBUGWRAP8(VarR4FromI1)
DEBUGWRAP8(VarR4FromUI2)
DEBUGWRAP8(VarR4FromUI4)
DEBUGWRAP8(VarR4FromDec)
DEBUGWRAP8(VarR8FromI1)
DEBUGWRAP8(VarR8FromUI2)
DEBUGWRAP8(VarR8FromUI4)
DEBUGWRAP8(VarR8FromDec)
DEBUGWRAP8(VarDateFromI1)
DEBUGWRAP8(VarDateFromUI2)
DEBUGWRAP8(VarDateFromUI4)
DEBUGWRAP8(VarDateFromDec)
DEBUGWRAP8(VarCyFromI1)
DEBUGWRAP8(VarCyFromUI2)
DEBUGWRAP8(VarCyFromUI4)
DEBUGWRAP8(VarCyFromDec)
DEBUGWRAP16(VarBstrFromI1)
DEBUGWRAP16(VarBstrFromUI2)
DEBUGWRAP16(VarBstrFromUI4)
DEBUGWRAP16(VarBstrFromDec)
DEBUGWRAP8(VarBoolFromI1)
DEBUGWRAP8(VarBoolFromUI2)
DEBUGWRAP8(VarBoolFromUI4)
DEBUGWRAP8(VarBoolFromDec)
DEBUGWRAP8(VarUI1FromI1)
DEBUGWRAP8(VarUI1FromUI2)
DEBUGWRAP8(VarUI1FromUI4)
DEBUGWRAP8(VarUI1FromDec)
DEBUGWRAP8(VarDecFromI1)
DEBUGWRAP8(VarDecFromUI2)
DEBUGWRAP8(VarDecFromUI4)
DEBUGWRAP8(VarI1FromUI1)
DEBUGWRAP8(VarI1FromI2)
DEBUGWRAP8(VarI1FromI4)
DEBUGWRAP8(VarI1FromR4)
DEBUGWRAP12(VarI1FromR8)
DEBUGWRAP12(VarI1FromDate)
DEBUGWRAP12(VarI1FromCy)
DEBUGWRAP16(VarI1FromStr)
DEBUGWRAP12(VarI1FromDisp)
DEBUGWRAP8(VarI1FromBool)
DEBUGWRAP8(VarI1FromUI2)
DEBUGWRAP8(VarI1FromUI4)
DEBUGWRAP8(VarI1FromDec)
DEBUGWRAP8(VarUI2FromUI1)
DEBUGWRAP8(VarUI2FromI2)
DEBUGWRAP8(VarUI2FromI4)
DEBUGWRAP8(VarUI2FromR4)
DEBUGWRAP12(VarUI2FromR8)
DEBUGWRAP12(VarUI2FromDate)
DEBUGWRAP12(VarUI2FromCy)
DEBUGWRAP16(VarUI2FromStr)
DEBUGWRAP12(VarUI2FromDisp)
DEBUGWRAP8(VarUI2FromBool)
DEBUGWRAP8(VarUI2FromI1)
DEBUGWRAP8(VarUI2FromUI4)
DEBUGWRAP8(VarUI2FromDec)
DEBUGWRAP8(VarUI4FromUI1)
DEBUGWRAP8(VarUI4FromI2)
DEBUGWRAP8(VarUI4FromI4)
DEBUGWRAP8(VarUI4FromR4)
DEBUGWRAP12(VarUI4FromR8)
DEBUGWRAP12(VarUI4FromDate)
DEBUGWRAP12(VarUI4FromCy)
DEBUGWRAP16(VarUI4FromStr)
DEBUGWRAP12(VarUI4FromDisp)
DEBUGWRAP8(VarUI4FromBool)
DEBUGWRAP8(VarUI4FromI1)
DEBUGWRAP8(VarUI4FromUI2)
DEBUGWRAP8(VarUI4FromDec)
NODEF_DEBUGWRAP12(BSTR_UserSize)
NODEF_DEBUGWRAP12(BSTR_UserMarshal)
NODEF_DEBUGWRAP12(BSTR_UserUnmarshal)
NODEF_DEBUGWRAP8(BSTR_UserFree)
NODEF_DEBUGWRAP12(VARIANT_UserSize)
NODEF_DEBUGWRAP12(VARIANT_UserMarshal)
NODEF_DEBUGWRAP12(VARIANT_UserUnmarshal)
NODEF_DEBUGWRAP8(VARIANT_UserFree)
NODEF_DEBUGWRAP12(LPSAFEARRAY_UserSize)
NODEF_DEBUGWRAP12(LPSAFEARRAY_UserMarshal)
NODEF_DEBUGWRAP12(LPSAFEARRAY_UserUnmarshal)
NODEF_DEBUGWRAP8(LPSAFEARRAY_UserFree)
NODEF_DEBUGWRAP0(LPSAFEARRAY_Size)
NODEF_DEBUGWRAP0(LPSAFEARRAY_Marshal)
NODEF_DEBUGWRAP0(LPSAFEARRAY_Unmarshal)
NODEF_DEBUGWRAP0(OLEAUT32_DllRegisterServer)
NODEF_DEBUGWRAP0(OLEAUT32_DllUnregisterServer)
DEBUGWRAP12(VarDateFromUdate)
DEBUGWRAP16(VarUdateFromDate)
NODEF_DEBUGWRAP8(GetAltMonthNames)
NODEF_DEBUGWRAP0(UserHWND_from_local)
NODEF_DEBUGWRAP0(UserHWND_to_local)
NODEF_DEBUGWRAP0(UserHWND_free_inst)
NODEF_DEBUGWRAP0(UserHWND_free_local)
NODEF_DEBUGWRAP0(UserBSTR_from_local)
NODEF_DEBUGWRAP0(UserBSTR_to_local)
NODEF_DEBUGWRAP0(UserBSTR_free_inst)
NODEF_DEBUGWRAP0(UserBSTR_free_local)
NODEF_DEBUGWRAP0(UserVARIANT_from_local)
NODEF_DEBUGWRAP0(UserVARIANT_to_local)
NODEF_DEBUGWRAP0(UserVARIANT_free_inst)
NODEF_DEBUGWRAP0(UserVARIANT_free_local)
NODEF_DEBUGWRAP0(UserEXCEPINFO_from_local)
NODEF_DEBUGWRAP0(UserEXCEPINFO_to_local)
NODEF_DEBUGWRAP0(UserEXCEPINFO_free_inst)
NODEF_DEBUGWRAP0(UserEXCEPINFO_free_local)
NODEF_DEBUGWRAP0(UserMSG_from_local)
NODEF_DEBUGWRAP0(UserMSG_to_local)
NODEF_DEBUGWRAP0(UserMSG_free_inst)
NODEF_DEBUGWRAP0(UserMSG_free_local)
NODEF_DEBUGWRAP0(OLEAUT32_DllCanUnloadNow)
DEBUGWRAP12(SafeArrayCreateVector)
DEBUGWRAP8(SafeArrayCopyData)
NODEF_DEBUGWRAP8(VectorFromBstr)
NODEF_DEBUGWRAP8(BstrFromVector)
NODEF_DEBUGWRAP8(OleIconToCursor)
NODEF_DEBUGWRAP4(OleCreatePropertyFrameIndirect)
NODEF_DEBUGWRAP44(OleCreatePropertyFrame)
NODEF_DEBUGWRAP20(OleLoadPicture)
NODEF_DEBUGWRAP16(OleCreatePictureIndirect)
NODEF_DEBUGWRAP12(OleCreateFontIndirect)
NODEF_DEBUGWRAP12(OleTranslateColor)
NODEF_DEBUGWRAP20(OleLoadPictureFile)
NODEF_DEBUGWRAP8(OleSavePictureFile)
NODEF_DEBUGWRAP24(OleLoadPicturePath)
NODEF_DEBUGWRAP32(OleLoadPictureEx)
NODEF_DEBUGWRAP12(SafeArrayAllocDescriptorEx)
NODEF_DEBUGWRAP12(VarBstrCat)
NODEF_DEBUGWRAP16(VarBstrCmp)
DEBUGWRAP12(VarCat)


