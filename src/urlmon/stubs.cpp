/* $Id: stubs.cpp,v 1.1 2001-07-07 04:36:06 bird Exp $
 *
 * Stubs.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <windows.h>
#include <objbase.h>
#include <urlmon.h>
#include <debugtools.h>


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef void IBindStatusCallback;

/**
 * Executes a hyperlink jump to a new document or object.
 *
 * @returns Returns S_OK if successful, or an error value otherwise.
 * @param   szTarget    String that helps identify the hyperlink target. This string is resolved into a
 *                      moniker for underlying binding operations through MkParseDisplayNameEx. If this is
 *                      NULL, the navigation is within a document. This parameteris required. (IN)
 * @param   szLocation  Optional. String that specifies the location within the hyperlink target for the
 *                      new hyperlink. (IN)
 * @param   szTargetFrameName    Optional. String that specifies the name of the target frame for the
 *                      hyperlink navigation. This argument only affects navigation within a document
 *                      container that supports framesets. (IN)
 * @param   pUnk        Address of the IUnknown interface on the document or object that is initiating the
 *                      hyperlink. If this is NULL, it is assumed the hyperlink originates from an
 *                      Microsoft© ActiveX©-unaware application. Note that if the caller of this function is
 *                      an ActiveX control or document object, you must pass a valid value for this
 *                      parameter for navigation to succeed. (IN)
 * @param   pbc         Address of the IBindCtx interface on the bind context to use for any moniker binding
 *                      performed during the navigation. This must not be NULL. (IN)
 * @param   pbsc        Address of the IBindStatusCallback interface on the bind-status-callback to use for
 *                      any asynchronous moniker binding performed during the navigation. The bind-status-callback
 *                      will only return the standard IUnknown methods: AddRef and QueryInterface. When navigating
 *                      in a new window, the only IBindStatusCallback method that gets called is
 *                      IBindStatusCallback::GetBindInfo. If this is NULL, the caller does not receive progress
 *                      notification, cancellation, pausing, or low-level binding information. (IN)
 * @param   grfHLNF     Unsigned long integer value that specifies one of the HLNF values. (IN)
 * @param   dwReserved  Reserved for future (yeah sure!) use; must be set to NULL. (IN)
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  http://msdn.microsoft.com/library/default.asp?url=/workshop/misc/hlink/reference/functions/HlinkSimpleNavigateToString.asp
 */
HRESULT WIN32API HlinkSimpleNavigateToString(
    LPCWSTR             szTarget,
    LPCWSTR             szLocation,
    LPCWSTR             szTargetFrameName,
    IUnknown *          pUnk,
    IBindCtx *          pbc,
    IBindStatusCallback *pbsc,
    DWORD               grfHLNF,
    DWORD               dwReserved
    )
{
    dprintf(("URLMON:HlinkSimpleNavigateToString(%ls %ls %ls 0x%x 0x%x 0x%x 0x%x 0x%x) - STUB\n",
             szTarget, szLocation ? szLocation : (LPCWSTR)L"<NULL>" , szTargetFrameName ? szTargetFrameName : (LPCWSTR)L"<NULL>" ,
             pUnk, pbc, pbsc, grfHLNF, dwReserved));
    return S_FALSE;
}



/**
 * Registers a callback interface with an existing bind context.
 *
 * @returns S_OK on Success. E_INVALIDARG if one or more parameters are invalid.
 *          E_OUTOFMEMORY if there was insufficient memory to register the callback with the bind context. (IN)
 * @param   pbc             Address of the IBindCtx interface from which to receive callbacks. (IN)
 * @param   pbsc            Address of the IBindStatusCallback interface implementation to be registered. (IN)
 * @param   ppbscPrevious   Address of a pointer to a previously registered instance of IBindStatusCallback. (OUT)
 * @param   dwReserved      Reserved - Must be set to zero. (yeah, sure!)
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  http://msdn.microsoft.com/library/default.asp?url=/workshop/networking/moniker/reference/functions/RegisterBindStatusCallback.asp
 */
HRESULT WIN32API RegisterBindStatusCallback(
    IBindCtx *              pbc,
    IBindStatusCallback *   pbsc,
    IBindStatusCallback **  ppbscPrevious,
    DWORD                   dwReserved
    )
{
    dprintf(("URLMON: RegisterBindStatusCallback(%x, %x, %x, %x) - STUB\n",
             pbc, pbsc, ppbscPrevious, dwReserved));
    return E_INVALIDARG;
}
