/* $Id: integrity.cpp,v 1.2 2000-08-02 14:56:26 bird Exp $ */
/*
 *	IMAGEHLP library
 *
 *	Copyright 1998	Patrik Stridvall
 */

#include <os2win.h>
#include <odinwrap.h>
#include <imagehlp.h>
#include <heapstring.h>

ODINDEBUGCHANNEL(imagehlp)

/***********************************************************************
 *		ImageAddCertificate (IMAGEHLP.@)
 */

BOOL WINAPI ImageAddCertificate(
  HANDLE FileHandle, PWIN_CERTIFICATE Certificate, PDWORD Index)
{
  dprintf(("(0x%08x, %p, %p): stub\n",
    FileHandle, Certificate, Index
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		ImageEnumerateCertificates (IMAGEHLP.@)
 */
BOOL WINAPI ImageEnumerateCertificates(
  HANDLE FileHandle, WORD TypeFilter, PDWORD CertificateCount,
  PDWORD Indices, DWORD IndexCount)
{
  dprintf(("(0x%08x, %hd, %p, %p, %ld): stub\n",
    FileHandle, TypeFilter, CertificateCount, Indices, IndexCount
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		ImageGetCertificateData (IMAGEHLP.@)
 */
BOOL WINAPI ImageGetCertificateData(
  HANDLE FileHandle, DWORD CertificateIndex,
  PWIN_CERTIFICATE Certificate, PDWORD RequiredLength)
{
  dprintf(("(0x%08x, %ld, %p, %p): stub\n",
    FileHandle, CertificateIndex, Certificate, RequiredLength
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		ImageGetCertificateHeader (IMAGEHLP.@)
 */
BOOL WINAPI ImageGetCertificateHeader(
  HANDLE FileHandle, DWORD CertificateIndex,
  PWIN_CERTIFICATE Certificateheader)
{
  dprintf(("(0x%08x, %ld, %p): stub\n",
    FileHandle, CertificateIndex, Certificateheader
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		ImageGetDigestStream (IMAGEHLP.@)
 */
BOOL WINAPI ImageGetDigestStream(
  HANDLE FileHandle, DWORD DigestLevel,
  DIGEST_FUNCTION DigestFunction, DIGEST_HANDLE DigestHandle)
{
  dprintf(("(0x%08x, %ld, %p, %p): stub\n",
    FileHandle, DigestLevel, DigestFunction, DigestHandle
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *		ImageRemoveCertificate (IMAGEHLP.@)
 */
BOOL WINAPI ImageRemoveCertificate(HANDLE FileHandle, DWORD Index)
{
  dprintf(("(0x%08x, %ld): stub\n", FileHandle, Index));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
