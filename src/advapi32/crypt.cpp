/*
 * dlls/advapi32/crypt.c
 */

#include <os2win.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <odinwrap.h>
#include <misc.h>
#include "wincrypt.h"
/*
#include "windef.h"
#include "winerror.h"
#include "wincrypt.h"
#include "debugtools.h"
*/

ODINDEBUGCHANNEL(ADVAPI32-CRYPT)

/******************************************************************************
 * CryptAcquireContextA
 * Acquire a crypto provider context handle.
 * 
 * PARAMS
 * phProv: Pointer to HCRYPTPROV for the output.
 * pszContainer: FIXME (unknown)
 * pszProvider: FIXME (unknown)
 * dwProvType: Crypto provider type to get a handle.
 * dwFlags: flags for the operation
 *
 * RETURNS TRUE on success, FALSE on failure.
 */

BOOL WINAPI CryptAcquireContextA( HCRYPTPROV *phProv, LPCSTR pszContainer,
		      LPCSTR pszProvider, DWORD dwProvType, DWORD dwFlags)
{
    dprintf(("(%p, %s, %s, %ld, %08lx): not implemented.\n", phProv, pszContainer,
	  pszProvider, dwProvType, dwFlags));
    return FALSE;
}

/******************************************************************************
 * CryptSetKeyParam
 */
BOOL WINAPI CryptSetKeyParam( HCRYPTKEY hKey, DWORD dwParam, BYTE *pbData, DWORD dwFlags)
{
    dprintf(("(%lx, %lx, %p, %lx): not implemented.\n", hKey, dwParam, pbData, dwFlags));
    return FALSE;
}

