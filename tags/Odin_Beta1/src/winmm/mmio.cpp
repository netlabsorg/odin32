/* $Id: mmio.cpp,v 1.8 2001-07-20 15:35:00 sandervl Exp $ */
/*
 * MMIO functions
 *
 * Copyright 1999 Przemyslaw Dobrowolski
 *
 * Based on Wine code:
 * Copyright 1998 Andrew Taylor
 * Copyright 1998 Ove Kåven
 *
 */

#include <os2win.h>
#include <mmsystem.h>
#include <odinwrap.h>
#include <misc.h>
#include <string.h>
#include <heapstring.h>

#include <stdlib.h>
#include <errno.h>
#include "windef.h"
#include "file.h"
#include "mmsystem.h"
#include "debugtools.h"

#define DBG_LOCALLOG	DBG_mmio
#include "dbglocal.h"

ODINDEBUGCHANNEL(WINMM-MMIO)

/**************************************************************************
 *               mmioDosIOProc           [internal]
 */
static LRESULT WIN32API mmioDosIOProc(LPMMIOINFO lpmmioinfo, UINT uMessage, LPARAM lParam1, LPARAM lParam2)
{
	dprintf(("mmioDosIOProc (%p, %X, %ld, %ld);\n", lpmmioinfo, uMessage, lParam1, lParam2));

	switch (uMessage) {

		case MMIOM_OPEN: {
			/* Parameters:
			 * lParam1 = szFileName parameter from mmioOpen
             * lParam2 = resrved
			 * NOTE: lDiskOffset automatically set to zero
			 */

			OFSTRUCT ofs;
			LPSTR szFileName = (LPSTR) lParam1;

			if (lpmmioinfo->dwFlags & MMIO_GETTEMP) {
				dprintf(("WINMM: mmioDosIOProc MMIO_GETTEMP not implemented\n"));
				return MMIOERR_CANNOTOPEN;
			}

			lpmmioinfo->adwInfo[0] =
				(DWORD) OpenFile(szFileName, &ofs, lpmmioinfo->dwFlags);
			if (lpmmioinfo->adwInfo[0] == -1)
				return MMIOERR_CANNOTOPEN;

			return 0;
		}

		case MMIOM_CLOSE: {
			/* Parameters:
			 * lParam1 = wFlags parameter from mmioClose
			 * lParam2 = unused
			 * Returns: zero on success, error code on error
			 */

			UINT uFlags = (UINT)lParam1;

			if (uFlags & MMIO_FHOPEN)
				return 0;

			_lclose((HFILE)lpmmioinfo->adwInfo[0]);
			return 0;

		}

		case MMIOM_READ: {
			/* Parameters:
			 * lParam1 = huge pointer to read buffer
			 * lParam2 = number of bytes to read
			 * Returns: number of bytes read, 0 for EOF, -1 for error (error code
			 *	   in wErrorRet)
			 * NOTE: lDiskOffset should be updated
			 */

			HPSTR pch = (HPSTR) lParam1;
			LONG cch = (LONG) lParam2;
			LONG count;

			count = _lread((HFILE)lpmmioinfo->adwInfo[0], pch, cch);
			if (count != -1)
				lpmmioinfo->lDiskOffset += count;

			return count;
		}

		case MMIOM_WRITE:
		case MMIOM_WRITEFLUSH: {
			/* no internal buffering, so WRITEFLUSH handled same as WRITE */

			/* Parameters:
			 * lParam1 = huge pointer to write buffer
			 * lParam2 = number of bytes to write
			 * Returns: number of bytes written, -1 for error (error code in
			 *		wErrorRet)
			 * NOTE: lDiskOffset should be updated
			 */

			HPSTR pch = (HPSTR) lParam1;
			LONG cch = (LONG) lParam2;
			LONG count;

#ifdef __WIN32OS2__
			count = _lwrite((HFILE)lpmmioinfo->adwInfo[0], pch, cch);
#else
			count = _hwrite((HFILE)lpmmioinfo->adwInfo[0], pch, cch);
#endif
			if (count != -1)
				lpmmioinfo->lDiskOffset += count;

			return count;
		}

		case MMIOM_SEEK: {
            /* Parameters:
             * lParam1 = new position
             * lParam2 = from whence to seek (SEEK_SET, SEEK_CUR, SEEK_END)
             * Returns: new file postion, -1 on error
             * NOTE: lDiskOffset should be updated
             */

            LONG Offset = (LONG) lParam1;
            LONG Whence = (LONG) lParam2;
            LONG pos;

            pos = _llseek((HFILE)lpmmioinfo->adwInfo[0], Offset, Whence);
            if (pos != -1)
                lpmmioinfo->lDiskOffset = pos;

            return pos;
		}

		case MMIOM_RENAME: {
            /* Parameters:
             * lParam1 = old name
             * lParam2 = new name
             * Returns: zero on success, non-zero on failure
             */

            dprintf(("WINMM: mmioDosIOProc MMIOM_RENAME unimplemented\n"));
            return MMIOERR_FILENOTFOUND;
		}

		default:
			dprintf(("WINMM: mmioDosIOProc unexpected message %u\n", uMessage));
			return 0;
	}

	return 0;
}

/**************************************************************************
*               mmioMemIOProc           [internal]
*/
static LRESULT WIN32API mmioMemIOProc(LPMMIOINFO lpmmioinfo, UINT uMessage, LPARAM lParam1, LPARAM lParam2) {
//	TRACE("(%p,0x%04x,0x%08lx,0x%08lx)\n",lpmmioinfo,uMessage,lParam1,lParam2);
	switch (uMessage) {

		case MMIOM_OPEN: {
			/* Parameters:
			 * lParam1 = filename (must be NULL)
			 * lParam2 = reserved
			 * Returns: zero on success, error code on error
			 * NOTE: lDiskOffset automatically set to zero
			 */

			if (!(lpmmioinfo->dwFlags & MMIO_CREATE))
				lpmmioinfo->pchEndRead = lpmmioinfo->pchEndWrite;

			return 0;
		}

		case MMIOM_CLOSE: {
			/* Parameters:
			 * lParam1 = wFlags parameter from mmioClose
			 * lParam2 = unused
			 * Returns: zero on success, error code on error
			 */

			return 0;

		}

		case MMIOM_READ: {
			/* Parameters:
			 * lParam1 = huge pointer to read buffer
			 * lParam2 = number of bytes to read
			 * Returns: number of bytes read, 0 for EOF, -1 for error (error code
			 *	   in wErrorRet)
			 * NOTE: lDiskOffset should be updated
			 */

			/* HPSTR pch = (HPSTR) lParam1; */
			/* LONG cch = (LONG) lParam2; */

			dprintf(("WINMM (mmioMemIOProc) MMIOM_READ on memory files should not occur, buffer may be lost!\n"));
			return 0;
		}

		case MMIOM_WRITE:
		case MMIOM_WRITEFLUSH: {
			/* no internal buffering, so WRITEFLUSH handled same as WRITE */

			/* Parameters:
			 * lParam1 = huge pointer to write buffer
			 * lParam2 = number of bytes to write
			 * Returns: number of bytes written, -1 for error (error code in
			 *		wErrorRet)
			 * NOTE: lDiskOffset should be updated
			 */

			/* HPSTR pch = (HPSTR) lParam1; */
			/* LONG cch = (LONG) lParam2; */

			dprintf(("WINMM (mmioMemIOProc) MMIOM_WRITE on memory files should not occur, buffer may be lost!\n"));
			return 0;
		}

		case MMIOM_SEEK: {
			/* Parameters:
			 * lParam1 = new position
			 * lParam2 = from whence to seek (SEEK_SET, SEEK_CUR, SEEK_END)
			 * Returns: new file postion, -1 on error
			 * NOTE: lDiskOffset should be updated
			 */

			/* LONG Offset = (LONG) lParam1; */
			/* LONG Whence = (LONG) lParam2; */

			dprintf(("WINMM (mmioMemIOProc) MMIOM_SEEK on memory files should not occur, buffer may be lost!\n"));
			return -1;
		}

		default:
			dprintf(("WINMM (mmioMemIOProc) unexpected message %u\n", uMessage));
			return 0;
	}

	return 0;
}

/**************************************************************************
 * 		MMIO_Open      		[internal]
 */
static HMMIO MMIO_Open(LPSTR szFileName, LPMMIOINFO refmminfo, DWORD dwOpenFlags)
{
	LPMMIOINFO lpmminfo;
	HMMIO      hmmio;

//	TRACE("('%s', %08lX);\n", szFileName, dwOpenFlags);

	if (dwOpenFlags & MMIO_PARSE) {
		char	buffer[MAX_PATH];

		if (GetFullPathNameA(szFileName, sizeof(buffer), buffer, NULL) >= sizeof(buffer))
			return (HMMIO)FALSE;
		strcpy(szFileName, buffer);
		return (HMMIO)TRUE;
	}

	hmmio = GlobalAlloc(GHND, sizeof(MMIOINFO));
	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL)
		return 0;
	memset(lpmminfo, 0, sizeof(MMIOINFO));

	/* assume DOS file if not otherwise specified */
	if (refmminfo->fccIOProc == 0 && refmminfo->pIOProc == NULL) {
		lpmminfo->fccIOProc = FOURCC_DOS;
		lpmminfo->pIOProc = (LPMMIOPROC) mmioDosIOProc;
	}
	/* if just the four character code is present, look up IO proc */
	else if (refmminfo->pIOProc == NULL) {

		lpmminfo->fccIOProc = refmminfo->fccIOProc;
		lpmminfo->pIOProc = mmioInstallIOProcA(refmminfo->fccIOProc, NULL, MMIO_FINDPROC);

	}
	/* if IO proc specified, use it and specified four character code */
	else {

		lpmminfo->fccIOProc = refmminfo->fccIOProc;
		lpmminfo->pIOProc = (LPMMIOPROC)refmminfo->pIOProc;
	}

	if (dwOpenFlags & MMIO_ALLOCBUF) {
		if ((refmminfo->wErrorRet = mmioSetBuffer(hmmio, NULL, MMIO_DEFAULTBUFFER, 0))) {
			return 0;
		}
	} else if (lpmminfo->fccIOProc == FOURCC_MEM) {
		if ((refmminfo->wErrorRet = mmioSetBuffer(hmmio, refmminfo->pchBuffer, refmminfo->cchBuffer, 0))) {
			return 0;
		}
	}

	/* see mmioDosIOProc for that one */
	lpmminfo->adwInfo[0] = refmminfo->adwInfo[0];
	lpmminfo->dwFlags = dwOpenFlags;
	lpmminfo->hmmio = hmmio;

	/* call IO proc to actually open file */
	refmminfo->wErrorRet = (UINT) mmioSendMessage(hmmio, MMIOM_OPEN, (LPARAM) szFileName, (LPARAM) 0);

	GlobalUnlock(hmmio);

	if (refmminfo->wErrorRet != 0) {
		GlobalFree(hmmio);
		return 0;
	}

	return hmmio;
}

/**************************************************************************
 * 				mmioOpenW       		[WINMM.123]
 */
ODINFUNCTION3(HMMIO, mmioOpenW,
              LPWSTR,szFileName,
              LPMMIOINFO,lpmmioinfo,
              DWORD,dwOpenFlags)
{
	LPSTR	szFn = HEAP_strdupWtoA(GetProcessHeap(),0,szFileName);
	HMMIO 	ret = mmioOpenA(szFn, lpmmioinfo, dwOpenFlags);

	HeapFree(GetProcessHeap(),0,szFn);
	return ret;
}

/**************************************************************************
 * 				mmioOpenA       		[WINMM.122]
 */
ODINFUNCTION3(HMMIO, mmioOpenA,
              LPSTR,szFileName,
              LPMMIOINFO,lpmmioinfo,
              DWORD,dwOpenFlags)
{
	HMMIO 	ret;

	if (lpmmioinfo) {
		ret = MMIO_Open(szFileName, lpmmioinfo, dwOpenFlags);
	} else {
	   MMIOINFO	mmioinfo;

	   mmioinfo.fccIOProc = 0;
	   mmioinfo.pIOProc = NULL;
	   mmioinfo.pchBuffer = NULL;
	   mmioinfo.cchBuffer = 0;

	   ret = MMIO_Open(szFileName, &mmioinfo, dwOpenFlags);
	}
	return ret;
}


/**************************************************************************
 * 				mmioClose      		[WINMM.114]
 */
ODINFUNCTION2(MMRESULT, mmioClose,
              HMMIO,hmmio,
              UINT,uFlags)
{
	LPMMIOINFO lpmminfo;
	MMRESULT   result;

//	TRACE("(%04X, %04X);\n", hmmio, uFlags);

	lpmminfo = (LPMMIOINFO) GlobalLock(hmmio);
	if (lpmminfo == NULL)
		return 0;

	/* flush the file - if error reported, ignore */
	if (mmioFlush(hmmio, MMIO_EMPTYBUF) != 0)
		lpmminfo->dwFlags &= ~MMIO_DIRTY;

	result = mmioSendMessage(hmmio,MMIOM_CLOSE,(LPARAM)uFlags,(LPARAM)0);

	mmioSetBuffer(hmmio, NULL, 0, 0);

	GlobalUnlock(hmmio);
	GlobalFree(hmmio);

	return result;
}



/**************************************************************************
 * 				mmioRead	       	[WINMM.124]
 */
ODINFUNCTION3(LONG, mmioRead,
              HMMIO,hmmio,
              HPSTR,pch,
              LONG,cch)
{
	LONG       count;
	LPMMIOINFO lpmminfo;

//	TRACE("(%04X, %p, %ld);\n", hmmio, pch, cch);

	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL)
		return -1;

	if (lpmminfo->pchNext != lpmminfo->pchEndRead) {
		count = lpmminfo->pchEndRead - lpmminfo->pchNext;
		if (count > cch || count < 0) count = cch;
		memcpy(pch, lpmminfo->pchNext, count);
		lpmminfo->pchNext += count;
		pch += count;
		cch -= count;
	} else
		count = 0;

	if (cch&&(lpmminfo->fccIOProc!=FOURCC_MEM)) {
		if (lpmminfo->cchBuffer) {
			mmioFlush(hmmio, MMIO_EMPTYBUF);

			while (cch) {
				LONG size;
				lpmminfo->lBufOffset = lpmminfo->lDiskOffset;
				lpmminfo->pchNext = lpmminfo->pchBuffer;
				lpmminfo->pchEndRead = lpmminfo->pchBuffer;
				size = mmioSendMessage(hmmio, MMIOM_READ,
						(LPARAM) lpmminfo->pchBuffer,
						(LPARAM) lpmminfo->cchBuffer);
				if (size<=0) break;
				lpmminfo->pchEndRead = lpmminfo->pchBuffer + size;
				if (size > cch) size = cch;
				memcpy(pch, lpmminfo->pchNext, size);
				lpmminfo->pchNext += size;
				pch += size;
				cch -= size;
				count += size;
			}
		} else {
			count += mmioSendMessage(hmmio, MMIOM_READ, (LPARAM) pch, (LPARAM) cch);
			if (count>0) lpmminfo->lBufOffset += count;
		}
	}

	GlobalUnlock(hmmio);
	TRACE("count=%ld\n", count);
	return count;
}

/**************************************************************************
 * 				mmioWrite      		[WINMM.133]
 */
ODINFUNCTION3(LONG, mmioWrite,
              HMMIO,hmmio,
              HPCSTR,pch,
              LONG,cch)
{
	LONG       count;
	LPMMIOINFO lpmminfo;

//	TRACE("(%04X, %p, %ld);\n", hmmio, pch, cch);

	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL)
		return -1;

	if (lpmminfo->cchBuffer) {
		count = 0;
		while (cch) {
			if (lpmminfo->pchNext != lpmminfo->pchEndWrite) {
				count = lpmminfo->pchEndWrite - lpmminfo->pchNext;
				if (count > cch || count < 0) count = cch;
				memcpy(lpmminfo->pchNext, pch, count);
				lpmminfo->pchNext += count;
				pch += count;
				cch -= count;
				lpmminfo->dwFlags |= MMIO_DIRTY;
			} else
			if (lpmminfo->fccIOProc==FOURCC_MEM) {
				if (lpmminfo->adwInfo[0]) {
					/* from where would we get the memory handle? */
					dprintf(("WINMM (mmioWrite) memory file expansion not implemented!\n"));
				} else break;
			}

			if (lpmminfo->pchNext == lpmminfo->pchEndWrite
				&& mmioFlush(hmmio, MMIO_EMPTYBUF)) break;
		}
	} else {
		count = mmioSendMessage(hmmio, MMIOM_WRITE, (LPARAM) pch, (LPARAM) cch);
		lpmminfo->lBufOffset = lpmminfo->lDiskOffset;
	}

	GlobalUnlock(hmmio);
//	TRACE("count=%ld\n", count);
	return count;
}

/**************************************************************************
 * 				mmioSeek       		[MMSYSTEM.1214]
 */
ODINFUNCTION3(LONG, mmioSeek,
              HMMIO, hmmio,
              LONG,lOffset,
              INT,iOrigin)
{
	int        offset;
	LPMMIOINFO lpmminfo;

//	TRACE("(%04X, %08lX, %d);\n", hmmio, lOffset, iOrigin);

	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL)
		return -1;

	offset = (iOrigin==SEEK_SET)?(lOffset - lpmminfo->lBufOffset):
		 (iOrigin==SEEK_CUR)?(lOffset +
		 (lpmminfo->pchNext - lpmminfo->pchBuffer)):-1;

	if ((lpmminfo->cchBuffer<0)||
	    ((offset>=0)&&(offset<=(lpmminfo->pchEndRead-lpmminfo->pchBuffer)))) {
		lpmminfo->pchNext = lpmminfo->pchBuffer + offset;
		GlobalUnlock(hmmio);
		return lpmminfo->lBufOffset + offset;
	}

	if ((lpmminfo->fccIOProc==FOURCC_MEM)||mmioFlush(hmmio, MMIO_EMPTYBUF)) {
		GlobalUnlock(hmmio);
		return -1;
	}

	offset = mmioSendMessage(hmmio, MMIOM_SEEK, (LPARAM) lOffset, (LPARAM) iOrigin);
	lpmminfo->lBufOffset = lpmminfo->lDiskOffset;

	GlobalUnlock(hmmio);
	return offset;
}

/**************************************************************************
 * 				mmioGetInfo	       	[MMSYSTEM.1215]
 */
ODINFUNCTION3(UINT, mmioGetInfo,
              HMMIO,hmmio,
              LPMMIOINFO,lpmmioinfo,
              UINT,uFlags)
{
	LPMMIOINFO	lpmminfo;
//	TRACE("mmioGetInfo\n");
	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL) return 0;
	memcpy(lpmmioinfo, lpmminfo, sizeof(MMIOINFO));
	GlobalUnlock(hmmio);
	return 0;
}


/**************************************************************************
 * 				mmioSetInfo    		[WINMM.130]
 */
ODINFUNCTION3(MMRESULT,mmioSetInfo,
             HMMIO, hmmio,
             const MMIOINFO*,lpmmioinfo,
             UINT,uFlags)
{
	LPMMIOINFO	lpmminfo;
//	TRACE("mmioSetInfo\n");
	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL) return 0;
	lpmminfo->pchNext	= lpmmioinfo->pchNext;
	lpmminfo->pchEndRead	= lpmmioinfo->pchEndRead;
	GlobalUnlock(hmmio);
	return 0;
}

/**************************************************************************
* 				mmioSetBuffer		[WINMM.129]
*/
ODINFUNCTION4(UINT,mmioSetBuffer,
              HMMIO,hmmio,
              LPSTR,pchBuffer,
              LONG,cchBuffer,
              UINT,uFlags)
{
	LPMMIOINFO	lpmminfo;

	if (mmioFlush(hmmio, MMIO_EMPTYBUF) != 0)
		return MMIOERR_CANNOTWRITE;

//	TRACE("(hmmio=%04x, pchBuf=%p, cchBuf=%ld, uFlags=%#08x)\n",
//	      hmmio, pchBuffer, cchBuffer, uFlags);

	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL) return 0;
	if ((!cchBuffer || pchBuffer) && lpmminfo->dwFlags&MMIO_ALLOCBUF) {
		GlobalUnlock(lpmminfo->dwReserved1);
		GlobalFree(lpmminfo->dwReserved1);
		lpmminfo->dwFlags &= ~MMIO_ALLOCBUF;
	}
	if (pchBuffer) {
		lpmminfo->pchBuffer = pchBuffer;
	} else if (lpmminfo->dwFlags&MMIO_ALLOCBUF) {
		HGLOBAL hNewBuf;
		GlobalUnlock(lpmminfo->dwReserved1);
		hNewBuf = GlobalReAlloc(lpmminfo->dwReserved1, cchBuffer, 0);
		if (!hNewBuf) {
			/* FIXME: this assumes the memory block didn't move */
			GlobalLock(lpmminfo->dwReserved1);
			GlobalUnlock(hmmio);
			return MMIOERR_OUTOFMEMORY;
		}
		lpmminfo->dwReserved1 = hNewBuf;
		lpmminfo->pchBuffer = (LPSTR)GlobalLock(hNewBuf);
	} else if (cchBuffer) {
		HGLOBAL hNewBuf = GlobalAlloc(GMEM_MOVEABLE, cchBuffer);
		if (!hNewBuf) {
			GlobalUnlock(hmmio);
			return MMIOERR_OUTOFMEMORY;
		}
		lpmminfo->dwReserved1 = hNewBuf;
		lpmminfo->pchBuffer = (LPSTR)GlobalLock(hNewBuf);
		lpmminfo->dwFlags |= MMIO_ALLOCBUF;
	} else
		lpmminfo->pchBuffer = NULL;
	lpmminfo->cchBuffer = cchBuffer;
	lpmminfo->pchNext = lpmminfo->pchBuffer;
	lpmminfo->pchEndRead = lpmminfo->pchBuffer;
	lpmminfo->pchEndWrite = lpmminfo->pchBuffer + cchBuffer;
	lpmminfo->lBufOffset = 0;

	GlobalUnlock(hmmio);
	return 0;
}

/**************************************************************************
 * 				mmioFlush      		[WINMM.117]
 */
ODINFUNCTION2(UINT,mmioFlush,
              HMMIO,hmmio,
              UINT,uFlags)
{
	LPMMIOINFO	lpmminfo;
//	TRACE("(%04X, %04X)\n", hmmio, uFlags);
	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL) return 0;

	if ((!lpmminfo->cchBuffer)||(lpmminfo->fccIOProc==FOURCC_MEM)) {
		GlobalUnlock(hmmio);
		return 0;
	}
	/* not quite sure what to do here, but I'll guess */
	if (lpmminfo->dwFlags & MMIO_DIRTY) {
		mmioSendMessage(hmmio, MMIOM_SEEK,
			(LPARAM) lpmminfo->lBufOffset,
			(LPARAM) SEEK_SET);
		mmioSendMessage(hmmio, MMIOM_WRITE,
			(LPARAM) lpmminfo->pchBuffer,
			(LPARAM) (lpmminfo->pchNext - lpmminfo->pchBuffer) );
		lpmminfo->dwFlags &= ~MMIO_DIRTY;
	}
	if (uFlags & MMIO_EMPTYBUF) {
		/* seems Windows doesn't do any seeking here, hopefully this
		   won't matter, otherwise a slight rewrite is necessary */
		mmioSendMessage(hmmio, MMIOM_SEEK,
			(LPARAM) (lpmminfo->lBufOffset +
				  (lpmminfo->pchNext - lpmminfo->pchBuffer)),
			(LPARAM) SEEK_SET);
		lpmminfo->pchNext = lpmminfo->pchBuffer;
		lpmminfo->pchEndRead = lpmminfo->pchBuffer;
		lpmminfo->lBufOffset = lpmminfo->lDiskOffset;
	}

	GlobalUnlock(hmmio);
	return 0;
}


/**************************************************************************
 * 				mmioAdvance    		[WINMM.113]
 */
ODINFUNCTION3(UINT,mmioAdvance,
              HMMIO, hmmio,
              LPMMIOINFO,lpmmioinfo,
              UINT,uFlags)
{
	LPMMIOINFO	lpmminfo;
//	TRACE("mmioAdvance\n");
	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);
	if (lpmminfo == NULL) return 0;
	if (!lpmminfo->cchBuffer) {
		GlobalUnlock(hmmio);
		return MMIOERR_UNBUFFERED;
	}
	lpmminfo->pchNext = lpmmioinfo->pchNext;
	if (mmioFlush(hmmio, MMIO_EMPTYBUF)) {
		GlobalUnlock(hmmio);
		return MMIOERR_CANNOTWRITE;
	}
	if (uFlags == MMIO_READ)
	        lpmmioinfo->pchEndRead = lpmmioinfo->pchBuffer +
	            mmioSendMessage(hmmio, MMIOM_READ,
	                (LPARAM) lpmmioinfo->pchBuffer,
	                (LPARAM) lpmmioinfo->cchBuffer);
	lpmmioinfo->pchNext = lpmmioinfo->pchBuffer;
	GlobalUnlock(hmmio);
	return 0;
}


/**************************************************************************
 * 				mmioStringToFOURCCA	[WINMM.131]
 */
ODINFUNCTION2(FOURCC,mmioStringToFOURCCA,
              LPCSTR, sz, UINT, uFlags)
{
  return mmioFOURCC(sz[0],sz[1],sz[2],sz[3]);
}

/**************************************************************************
 * 				mmioStringToFOURCCW	[WINMM.132]
 */
ODINFUNCTION2(FOURCC,mmioStringToFOURCCW,
              LPCWSTR, sz, UINT, uFlags)
{
	LPSTR	szA = HEAP_strdupWtoA(GetProcessHeap(),0,sz);
	FOURCC	ret = mmioStringToFOURCCA(szA,uFlags);

	HeapFree(GetProcessHeap(),0,szA);
	return ret;
}


/* maximum number of I/O procedures which can be installed */

struct IOProcList
{
  struct IOProcList *pNext;      /* Next item in linked list */
  FOURCC            fourCC;     /* four-character code identifying IOProc */
  LPMMIOPROC        pIOProc;    /* pointer to IProc */
};

/* This array will be the entire list for most apps */

static struct IOProcList defaultProcs[] = {
  { &defaultProcs[1], (FOURCC) FOURCC_DOS,(LPMMIOPROC) mmioDosIOProc },
  { NULL, (FOURCC) FOURCC_MEM,(LPMMIOPROC) mmioMemIOProc },
};

static struct IOProcList *pIOProcListAnchor = &defaultProcs[0];

ODINFUNCTION3(LPMMIOPROC, mmioInstallIOProcA,
              FOURCC,fccIOProc,
              LPMMIOPROC,pIOProc,
              DWORD,dwFlags)
{
	LPMMIOPROC        lpProc = NULL;
        struct IOProcList  *pListNode;

//	TRACE("(%ld, %p, %08lX)\n",
//				 fccIOProc, pIOProc, dwFlags);

	if (dwFlags & MMIO_GLOBALPROC) {
		dprintf(("WINMM: mmioInstallIOProcA global procedures not implemented\n"));
	}

	/* just handle the known procedures for now */
	switch(dwFlags & (MMIO_INSTALLPROC|MMIO_REMOVEPROC|MMIO_FINDPROC)) {
		case MMIO_INSTALLPROC:
	          /* Create new entry for the IOProc list */
	          pListNode = (struct IOProcList  *)HeapAlloc(GetProcessHeap(),0,sizeof(*pListNode));
	          if (pListNode)
	          {
	            /* Find the end of the list, so we can add our new entry to it */
	            struct IOProcList *pListEnd = pIOProcListAnchor;
	            while (pListEnd->pNext)
	               pListEnd = pListEnd->pNext;

	            /* Fill in this node */
	            pListNode->fourCC = fccIOProc;
	            pListNode->pIOProc = pIOProc;

	            /* Stick it on the end of the list */
	            pListEnd->pNext = pListNode;
	            pListNode->pNext = NULL;

	            /* Return this IOProc - that's how the caller knows we succeeded */
	            lpProc = pIOProc;
	          };
	          break;

		case MMIO_REMOVEPROC:
	          /*
	           * Search for the node that we're trying to remove - note
	           * that this method won't find the first item on the list, but
	           * since the first two items on this list are ones we won't
	           * let the user delete anyway, that's okay
	           */

	          pListNode = pIOProcListAnchor;
	          while (pListNode && pListNode->pNext->fourCC != fccIOProc)
	            pListNode = pListNode->pNext;

	          /* If we found it, remove it, but only if it isn't builtin */
	          if (pListNode &&
	              ((pListNode >= defaultProcs) && (pListNode < defaultProcs + sizeof(defaultProcs))))
	          {
	            /* Okay, nuke it */
	            pListNode->pNext = pListNode->pNext->pNext;
	            HeapFree(GetProcessHeap(),0,pListNode);
	          };
	          break;

		case MMIO_FINDPROC:
		      /* Iterate through the list looking for this proc identified by fourCC */
		      for (pListNode = pIOProcListAnchor; pListNode; pListNode=pListNode->pNext)
		      {
		        if (pListNode->fourCC == fccIOProc)
		        {
		          lpProc = pListNode->pIOProc;
		          break;
		        };
		      };
		      break;
	}

	return lpProc;
}


/**************************************************************************
* 				mmioSendMessage		[MMSYSTEM.1222]
*/
ODINFUNCTION4(LRESULT,mmioSendMessage,
              HMMIO,hmmio,
              UINT,uMessage,
              LPARAM,lParam1,
              LPARAM,lParam2)
{
	LPMMIOINFO lpmminfo;
	LRESULT result;
	const char *msg = NULL;

#ifdef DEBUG
	switch (uMessage) {
#define msgname(x) case x: msg = #x; break;
		msgname(MMIOM_OPEN);
		msgname(MMIOM_CLOSE);
		msgname(MMIOM_READ);
		msgname(MMIOM_WRITE);
		msgname(MMIOM_WRITEFLUSH);
		msgname(MMIOM_SEEK);
		msgname(MMIOM_RENAME);
#undef msgname
	}
#endif

	if (msg)
		dprintf(("WINMM: mmioSendMessage (%04X, %s, %ld, %ld)\n",
					 hmmio, msg, lParam1, lParam2));
	else
		dprintf(("WINMM: mmioSendMessage (%04X, %u, %ld, %ld)\n",
					 hmmio, uMessage, lParam1, lParam2));

	lpmminfo = (LPMMIOINFO)GlobalLock(hmmio);

	if (lpmminfo && lpmminfo->pIOProc)
		result = (*lpmminfo->pIOProc)((LPSTR)lpmminfo, (UINT)uMessage, lParam1, lParam2);
	else
		result = MMSYSERR_INVALPARAM;

	GlobalUnlock(hmmio);

	return result;
}

/**************************************************************************
* 				mmioDescend	       	[MMSYSTEM.1223]
*/
ODINFUNCTION4(UINT,mmioDescend,
              HMMIO,hmmio,
              LPMMCKINFO,lpck,
              const MMCKINFO *,lpckParent,
              UINT,uFlags)
{
	DWORD		dwOldPos;
	FOURCC		srchCkId;
	FOURCC		srchType;


//	TRACE("(%04X, %p, %p, %04X);\n", hmmio, lpck, lpckParent, uFlags);

	if (lpck == NULL)
		return MMSYSERR_INVALPARAM;

	dwOldPos = mmioSeek(hmmio, 0, SEEK_CUR);
	dprintf(("WINMM: mmioDescend - dwOldPos=%ld\n", dwOldPos));

	if (lpckParent != NULL) {
		TRACE("seek inside parent at %ld !\n", lpckParent->dwDataOffset);
		/* EPP: was dwOldPos = mmioSeek(hmmio,lpckParent->dwDataOffset,SEEK_SET); */
		if (dwOldPos < lpckParent->dwDataOffset || dwOldPos >= lpckParent->dwDataOffset + lpckParent->cksize) {
         	dprintf(("WINMM: mmioDescend - outside parent chunk\n"));
			return MMIOERR_CHUNKNOTFOUND;
		}
	}

	/* The SDK docu says 'ckid' is used for all cases. Real World
	 * examples disagree -Marcus,990216.
	 */

	srchType = 0;
	/* find_chunk looks for 'ckid' */
	if (uFlags & MMIO_FINDCHUNK)
		srchCkId = lpck->ckid;
	/* find_riff and find_list look for 'fccType' */
	if (uFlags & MMIO_FINDLIST) {
		srchCkId = FOURCC_LIST;
		srchType = lpck->fccType;
	}
	if (uFlags & MMIO_FINDRIFF) {
		srchCkId = FOURCC_RIFF;
		srchType = lpck->fccType;
	}
   	dprintf(("WINMM: mmioDescend - searching for %.4s.%.4s\n",
	      (LPSTR)&srchCkId,
	      srchType?(LPSTR)&srchType:"<any>"));

	if (uFlags & (MMIO_FINDCHUNK|MMIO_FINDLIST|MMIO_FINDRIFF)) {
		while (TRUE) {
		        LONG ix;

			ix = mmioRead(hmmio, (LPSTR)lpck, 3 * sizeof(DWORD));
			if (ix < 2*sizeof(DWORD)) {
				mmioSeek(hmmio, dwOldPos, SEEK_SET);
				WARN("return ChunkNotFound\n");
				return MMIOERR_CHUNKNOTFOUND;
			}
			lpck->dwDataOffset = dwOldPos + 2 * sizeof(DWORD);
			if (ix < lpck->dwDataOffset - dwOldPos) {
				mmioSeek(hmmio, dwOldPos, SEEK_SET);
				WARN("return ChunkNotFound\n");
				return MMIOERR_CHUNKNOTFOUND;
			}
			dprintf(("WINMM: mmioDescend - ckid=%.4ss fcc=%.4ss cksize=%08lX !\n",
			      (LPSTR)&lpck->ckid,
			      srchType?(LPSTR)&lpck->fccType:"<unused>",
			      lpck->cksize));
			if ((srchCkId == lpck->ckid) &&
			    (!srchType || (srchType == lpck->fccType))
			    )
				break;

			dwOldPos = lpck->dwDataOffset + ((lpck->cksize + 1) & ~1);
			mmioSeek(hmmio, dwOldPos, SEEK_SET);
		}
	} else {
		/* FIXME: unverified, does it do this? */
		if (mmioRead(hmmio, (LPSTR)lpck, 3 * sizeof(DWORD)) < 3 * sizeof(DWORD)) {
			mmioSeek(hmmio, dwOldPos, SEEK_SET);
			dprintf(("WINMM: mmioDescend - return ChunkNotFound 2nd\n"));
			return MMIOERR_CHUNKNOTFOUND;
		}
		lpck->dwDataOffset = dwOldPos + 2 * sizeof(DWORD);
	}
	lpck->dwFlags = 0;
	/* If we were looking for RIFF/LIST chunks, the final file position
	 * is after the chunkid. If we were just looking for the chunk
	 * it is after the cksize. So add 4 in RIFF/LIST case.
	 */
	if (lpck->ckid == FOURCC_RIFF || lpck->ckid == FOURCC_LIST)
		mmioSeek(hmmio, lpck->dwDataOffset + sizeof(DWORD), SEEK_SET);
	else
		mmioSeek(hmmio, lpck->dwDataOffset, SEEK_SET);
	dprintf(("WINMM: mmioDescend - lpck: ckid=%.4s, cksize=%ld, dwDataOffset=%ld fccType=%08lX (%.4s)!\n",
	      (LPSTR)&lpck->ckid, lpck->cksize, lpck->dwDataOffset,
	      lpck->fccType, srchType?(LPSTR)&lpck->fccType:""));
	return 0;
}

/**************************************************************************
 * 				mmioAscend     		[WINMM.113]
 */
ODINFUNCTION3(UINT,mmioAscend,
              HMMIO,hmmio,
              LPMMCKINFO,lpck,
              UINT,uFlags)
{
//	TRACE("(%04X, %p, %04X);\n", hmmio, lpck, uFlags);

	if (lpck->dwFlags & MMIO_DIRTY) {
		DWORD	dwOldPos, dwNewSize, dwSizePos;

		dprintf(("WINMM: mmioAscend - chunk is marked MMIO_DIRTY, correcting chunk size\n"));
		dwOldPos = mmioSeek(hmmio, 0, SEEK_CUR);
		dprintf(("WINMM: mmioAscend - dwOldPos=%ld\n", dwOldPos));
		dwNewSize = dwOldPos - lpck->dwDataOffset;
		if (dwNewSize != lpck->cksize) {
			TRACE("dwNewSize=%ld\n", dwNewSize);
			lpck->cksize = dwNewSize;

			dwSizePos = lpck->dwDataOffset - sizeof(DWORD);
            dprintf(("WINMM: mmioAscend - dwSizePos=%ld\n", dwSizePos));

			mmioSeek(hmmio, dwSizePos, SEEK_SET);
			mmioWrite(hmmio, (LPSTR)&dwNewSize, sizeof(DWORD));
		}
	}

	mmioSeek(hmmio, lpck->dwDataOffset + ((lpck->cksize + 1) & ~1), SEEK_SET);

	return 0;
}

/**************************************************************************
 * 					mmioCreateChunk					[WINMM.115]
 */
ODINFUNCTION3(UINT,mmioCreateChunk,
              HMMIO,hmmio,
              LPMMCKINFO,lpck,
              UINT,uFlags)
{
	DWORD	dwOldPos;
	LONG 	size;
	LONG 	ix;

//	TRACE("(%04X, %p, %04X);\n", hmmio, lpck, uFlags);

	dwOldPos = mmioSeek(hmmio, 0, SEEK_CUR);
	dprintf(("WINMM: mmioCreateChunk - dwOldPos=%ld\n", dwOldPos));

	if (uFlags == MMIO_CREATELIST)
		lpck->ckid = FOURCC_LIST;
	else if (uFlags == MMIO_CREATERIFF)
		lpck->ckid = FOURCC_RIFF;

	dprintf(("WINMM: mmioCreateChunk - ckid=%08lX\n", lpck->ckid));

	size = 2 * sizeof(DWORD);
	lpck->dwDataOffset = dwOldPos + size;

	if (lpck->ckid == FOURCC_RIFF || lpck->ckid == FOURCC_LIST)
		size += sizeof(DWORD);
	lpck->dwFlags = MMIO_DIRTY;

	ix = mmioWrite(hmmio, (LPSTR)lpck, size);
	TRACE("after mmioWrite ix = %ld req = %ld, errno = %d\n",ix, size, errno);
	if (ix < size) {
		mmioSeek(hmmio, dwOldPos, SEEK_SET);
		dprintf(("WINMM: mmioCreateChunk - return CannotWrite\n"));
		return MMIOERR_CANNOTWRITE;
	}

	return 0;
}

/**************************************************************************
 * 				mmioRenameA     			[WINMM.125]
 */
ODINFUNCTION4(UINT,mmioRenameA,
              LPCSTR,szFileName,
              LPCSTR,szNewFileName,
	      LPMMIOINFO,lpmmioinfo,
              DWORD,dwRenameFlags)
{
	UINT result;
	LPMMIOINFO lpmminfo;
	HMMIO hmmio;

//	TRACE("('%s', '%s', %p, %08lX);\n",
//				 szFileName, szNewFileName, lpmmioinfo, dwRenameFlags);
	dprintf(("WINMM: This may fail - function untested\n"));
	hmmio = GlobalAlloc(GHND, sizeof(MMIOINFO));
	lpmminfo = (LPMMIOINFO) GlobalLock(hmmio);

	if (lpmmioinfo)
		memcpy(lpmminfo, lpmmioinfo, sizeof(MMIOINFO));

	/* assume DOS file if not otherwise specified */
	if (lpmminfo->fccIOProc == 0 && lpmminfo->pIOProc == NULL) {

		lpmminfo->fccIOProc = mmioFOURCC('D', 'O', 'S', ' ');
		lpmminfo->pIOProc = (LPMMIOPROC) mmioDosIOProc;

	}
	/* if just the four character code is present, look up IO proc */
	else if (lpmminfo->pIOProc == NULL) {

		lpmminfo->pIOProc = mmioInstallIOProcA(lpmminfo->fccIOProc, NULL, MMIO_FINDPROC);

	}
	/* (if IO proc specified, use it and specified four character code) */

	result = (UINT) mmioSendMessage(hmmio, MMIOM_RENAME, (LPARAM) szFileName, (LPARAM) szNewFileName);

	GlobalUnlock(hmmio);
	GlobalFree(hmmio);

	return result;
}


/**************************************************************************
 * 				mmioRenameW     			[WINMM.126]
 */
ODINFUNCTION4(UINT,mmioRenameW,
              LPCWSTR,szFileName,
              LPCWSTR,szNewFileName,
	      LPMMIOINFO,lpmmioinfo,
              DWORD,dwRenameFlags)
{
	LPSTR		szFn = HEAP_strdupWtoA(GetProcessHeap(), 0, szFileName);
	LPSTR		sznFn = HEAP_strdupWtoA(GetProcessHeap(), 0, szNewFileName);
	UINT	ret = mmioRenameA(szFn, sznFn, lpmmioinfo, dwRenameFlags);

	HeapFree(GetProcessHeap(),0,szFn);
	HeapFree(GetProcessHeap(),0,sznFn);
	return ret;
}

ODINFUNCTION3(LPMMIOPROC, mmioInstallIOProcW,
              FOURCC, fccIOProc,
              LPMMIOPROC, pIOProc,
              DWORD, dwFlags)
{
  // TODO: What is difference in mmioInstallIOProcW and mmioInstallIOProcA?
  dprintf(("WINMM:mmioInstallIOProcW - stub\n"));
  return 0;
}
