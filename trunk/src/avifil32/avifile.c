/*
 * Copyright 1999 Marcus Meissner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "winbase.h"
#include "winnls.h"
#include "mmsystem.h"
#include "winerror.h"
#include "vfw.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(avifile);

static HRESULT WINAPI IAVIFile_fnQueryInterface(IAVIFile* iface,REFIID refiid,LPVOID *obj);
static ULONG WINAPI IAVIFile_fnAddRef(IAVIFile* iface);
static ULONG WINAPI IAVIFile_fnRelease(IAVIFile* iface);
static HRESULT WINAPI IAVIFile_fnInfo(IAVIFile*iface,AVIFILEINFOW*afi,LONG size);
static HRESULT WINAPI IAVIFile_fnGetStream(IAVIFile*iface,PAVISTREAM*avis,DWORD fccType,LONG lParam);
static HRESULT WINAPI IAVIFile_fnCreateStream(IAVIFile*iface,PAVISTREAM*avis,AVISTREAMINFOW*asi);
static HRESULT WINAPI IAVIFile_fnWriteData(IAVIFile*iface,DWORD ckid,LPVOID lpData,LONG size);
static HRESULT WINAPI IAVIFile_fnReadData(IAVIFile*iface,DWORD ckid,LPVOID lpData,LONG *size);
static HRESULT WINAPI IAVIFile_fnEndRecord(IAVIFile*iface);
static HRESULT WINAPI IAVIFile_fnDeleteStream(IAVIFile*iface,DWORD fccType,LONG lParam);

struct ICOM_VTABLE(IAVIFile) iavift = {
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IAVIFile_fnQueryInterface,
    IAVIFile_fnAddRef,
    IAVIFile_fnRelease,
    IAVIFile_fnInfo,
    IAVIFile_fnGetStream,
    IAVIFile_fnCreateStream,
    IAVIFile_fnWriteData,
    IAVIFile_fnReadData,
    IAVIFile_fnEndRecord,
    IAVIFile_fnDeleteStream
};

static HRESULT WINAPI IAVIStream_fnQueryInterface(IAVIStream*iface,REFIID refiid,LPVOID *obj);
static ULONG WINAPI IAVIStream_fnAddRef(IAVIStream*iface);
static ULONG WINAPI IAVIStream_fnRelease(IAVIStream* iface);
static HRESULT WINAPI IAVIStream_fnCreate(IAVIStream*iface,LPARAM lParam1,LPARAM lParam2);
static HRESULT WINAPI IAVIStream_fnInfo(IAVIStream*iface,AVISTREAMINFOW *psi,LONG size);
static LONG WINAPI IAVIStream_fnFindSample(IAVIStream*iface,LONG pos,LONG flags);
static HRESULT WINAPI IAVIStream_fnReadFormat(IAVIStream*iface,LONG pos,LPVOID format,LONG *formatsize);
static HRESULT WINAPI IAVIStream_fnSetFormat(IAVIStream*iface,LONG pos,LPVOID format,LONG formatsize);
static HRESULT WINAPI IAVIStream_fnRead(IAVIStream*iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,LONG *bytesread,LONG *samplesread);
static HRESULT WINAPI IAVIStream_fnWrite(IAVIStream*iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,DWORD flags,LONG *sampwritten,LONG *byteswritten);
static HRESULT WINAPI IAVIStream_fnDelete(IAVIStream*iface,LONG start,LONG samples);
static HRESULT WINAPI IAVIStream_fnReadData(IAVIStream*iface,DWORD fcc,LPVOID lp,LONG *lpread);
static HRESULT WINAPI IAVIStream_fnWriteData(IAVIStream*iface,DWORD fcc,LPVOID lp,LONG size);
static HRESULT WINAPI IAVIStream_fnSetInfo(IAVIStream*iface,AVISTREAMINFOW*info,LONG infolen);

struct ICOM_VTABLE(IAVIStream) iavist = {
    ICOM_MSVTABLE_COMPAT_DummyRTTIVALUE
    IAVIStream_fnQueryInterface,
    IAVIStream_fnAddRef,
    IAVIStream_fnRelease,
    IAVIStream_fnCreate,
    IAVIStream_fnInfo,
    IAVIStream_fnFindSample,
    IAVIStream_fnReadFormat,
    IAVIStream_fnSetFormat,
    IAVIStream_fnRead,
    IAVIStream_fnWrite,
    IAVIStream_fnDelete,
    IAVIStream_fnReadData,
    IAVIStream_fnWriteData,
    IAVIStream_fnSetInfo
};

typedef struct IAVIStreamImpl {
	/* IUnknown stuff */
	ICOM_VFIELD(IAVIStream);
	DWORD		ref;
	/* IAVIStream stuff */
	LPVOID		lpInputFormat;
	DWORD		inputformatsize;
	BOOL		iscompressing;
	DWORD		curframe;

	    /* Compressor stuff */
	    HIC	hic;
	    LPVOID	lpCompressFormat;
	    ICINFO	icinfo;
	    DWORD	compbufsize;
	    LPVOID	compbuffer;

	    DWORD	decompbufsize;
	    LPVOID	decompbuffer;
	    LPVOID	decompformat;
	    AVICOMPRESSOPTIONS	aco;

	    LPVOID	lpPrev;	/* pointer to decompressed frame later */
	    LPVOID	lpPrevFormat; /* pointer to decompressed info later */
} IAVIStreamImpl;

/***********************************************************************
 *		AVIFileInit (AVIFIL32.@)
 *		AVIFileInit (AVIFILE.100)
 */
void WINAPI
AVIFileInit(void) {
	FIXME("(),stub!\n");
}

typedef struct IAVIFileImpl {
	/* IUnknown stuff */
	ICOM_VFIELD(IAVIFile);
	DWORD				ref;
	/* IAVIFile stuff... */
} IAVIFileImpl;

static HRESULT WINAPI IAVIFile_fnQueryInterface(IAVIFile* iface,REFIID refiid,LPVOID *obj) {
	ICOM_THIS(IAVIFileImpl,iface);

	TRACE("(%p)->QueryInterface(%s,%p)\n",This,debugstr_guid(refiid),obj);
	if (	!memcmp(&IID_IUnknown,refiid,sizeof(IID_IUnknown)) ||
		!memcmp(&IID_IAVIFile,refiid,sizeof(IID_IAVIFile))
	) {
		*obj = iface;
		return S_OK;
	}
	return OLE_E_ENUM_NOMORE;
}

static ULONG WINAPI IAVIFile_fnAddRef(IAVIFile* iface) {
	ICOM_THIS(IAVIFileImpl,iface);

	FIXME("(%p)->AddRef()\n",iface);
	return ++(This->ref);
}

static ULONG WINAPI IAVIFile_fnRelease(IAVIFile* iface) {
	ICOM_THIS(IAVIFileImpl,iface);

	FIXME("(%p)->Release()\n",iface);
	if (!--(This->ref)) {
		HeapFree(GetProcessHeap(),0,iface);
		return 0;
	}
	return This->ref;
}

static HRESULT WINAPI IAVIFile_fnInfo(IAVIFile*iface,AVIFILEINFOW*afi,LONG size) {
	FIXME("(%p)->Info(%p,%ld)\n",iface,afi,size);

	/* FIXME: fill out struct? */
	return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnGetStream(IAVIFile*iface,PAVISTREAM*avis,DWORD fccType,LONG lParam) {
	FIXME("(%p)->GetStream(%p,0x%08lx,%ld)\n",iface,avis,fccType,lParam);
	/* FIXME: create interface etc. */
	return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnCreateStream(IAVIFile*iface,PAVISTREAM*avis,AVISTREAMINFOW*asi) {
	ICOM_THIS(IAVIStreamImpl,iface);
	char		fcc[5];
	IAVIStreamImpl	*istream;

	FIXME("(%p,%p,%p)\n",This,avis,asi);
	istream = (IAVIStreamImpl*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(IAVIStreamImpl));
	istream->ref = 1;
	ICOM_VTBL(istream) = &iavist;
	fcc[4]='\0';
	memcpy(fcc,(char*)&(asi->fccType),4);
	FIXME("\tfccType '%s'\n",fcc);
	memcpy(fcc,(char*)&(asi->fccHandler),4);
	FIXME("\tfccHandler '%s'\n",fcc);
	FIXME("\tdwFlags 0x%08lx\n",asi->dwFlags);
	FIXME("\tdwCaps 0x%08lx\n",asi->dwCaps);
	FIXME("\tname %s\n",debugstr_w(asi->szName));

	istream->curframe = 0;
	*avis = (PAVISTREAM)istream;
	return S_OK;
}

static HRESULT WINAPI IAVIFile_fnWriteData(IAVIFile*iface,DWORD ckid,LPVOID lpData,LONG size) {
	FIXME("(%p)->WriteData(0x%08lx,%p,%ld)\n",iface,ckid,lpData,size);
	/* FIXME: write data to file */
	return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnReadData(IAVIFile*iface,DWORD ckid,LPVOID lpData,LONG *size) {
	FIXME("(%p)->ReadData(0x%08lx,%p,%p)\n",iface,ckid,lpData,size);
	/* FIXME: read at most size bytes from file */
	return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnEndRecord(IAVIFile*iface) {
	FIXME("(%p)->EndRecord()\n",iface);
	/* FIXME: end record? */
	return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnDeleteStream(IAVIFile*iface,DWORD fccType,LONG lParam) {
	FIXME("(%p)->DeleteStream(0x%08lx,%ld)\n",iface,fccType,lParam);
	/* FIXME: delete stream? */
	return E_FAIL;
}

/***********************************************************************
 *		AVIFileOpenA (AVIFIL32.@)
 *		AVIFileOpen  (AVIFILE.102)
 */
HRESULT WINAPI AVIFileOpenA(
	PAVIFILE * ppfile,LPCSTR szFile,UINT uMode,LPCLSID lpHandler
) {
	IAVIFileImpl	*iavi;

	FIXME("(%p,%s,0x%08lx,%s),stub!\n",ppfile,szFile,(DWORD)uMode,debugstr_guid(lpHandler));
	iavi = (IAVIFileImpl*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(IAVIFileImpl));
	iavi->ref = 1;
	ICOM_VTBL(iavi) = &iavift;
	*ppfile = (LPVOID)iavi;
	return S_OK;
}

static HRESULT WINAPI IAVIStream_fnQueryInterface(IAVIStream*iface,REFIID refiid,LPVOID *obj) {
	ICOM_THIS(IAVIStreamImpl,iface);

	TRACE("(%p)->QueryInterface(%s,%p)\n",This,debugstr_guid(refiid),obj);
	if (	!memcmp(&IID_IUnknown,refiid,sizeof(IID_IUnknown)) ||
		!memcmp(&IID_IAVIStream,refiid,sizeof(IID_IAVIStream))
	) {
		*obj = This;
		return S_OK;
	}
	/* can return IGetFrame interface too */
	return OLE_E_ENUM_NOMORE;
}

static ULONG WINAPI IAVIStream_fnAddRef(IAVIStream*iface) {
	ICOM_THIS(IAVIStreamImpl,iface);

	FIXME("(%p)->AddRef()\n",iface);
	return ++(This->ref);
}

static ULONG WINAPI IAVIStream_fnRelease(IAVIStream* iface) {
	ICOM_THIS(IAVIStreamImpl,iface);

	FIXME("(%p)->Release()\n",iface);
	if (!--(This->ref)) {
		HeapFree(GetProcessHeap(),0,This);
		return 0;
	}
	return This->ref;
}

static HRESULT WINAPI IAVIStream_fnCreate(IAVIStream*iface,LPARAM lParam1,LPARAM lParam2) {
	FIXME("(%p)->Create(0x%08lx,0x%08lx)\n",iface,lParam1,lParam2);
	return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnInfo(IAVIStream*iface,AVISTREAMINFOW *psi,LONG size) {
	FIXME("(%p)->Info(%p,%ld)\n",iface,psi,size);
	return E_FAIL;
}

static LONG WINAPI IAVIStream_fnFindSample(IAVIStream*iface,LONG pos,LONG flags) {
	FIXME("(%p)->FindSample(%ld,0x%08lx)\n",iface,pos,flags);
	return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnReadFormat(IAVIStream*iface,LONG pos,LPVOID format,LONG *formatsize) {
	FIXME("(%p)->ReadFormat(%ld,%p,%p)\n",iface,pos,format,formatsize);
	return E_FAIL;
}

/***********************************************************************
 *		IAVIStream::SetFormat
 */
static HRESULT WINAPI IAVIStream_fnSetFormat(IAVIStream*iface,LONG pos,LPVOID format,LONG formatsize) {
	IAVIStreamImpl	*as = (IAVIStreamImpl*)iface;

	FIXME("(%p)->SetFormat(%ld,%p,%ld)\n",iface,pos,format,formatsize);
	if (as->lpInputFormat) HeapFree(GetProcessHeap(),0,as->lpInputFormat);
	as->inputformatsize = formatsize;
	as->lpInputFormat = HeapAlloc(GetProcessHeap(),0,formatsize);
	memcpy(as->lpInputFormat,format,formatsize);
	if (as->iscompressing) {
		int	xsize;
		/* Set up the Compressor part */
		xsize = ICCompressGetFormatSize(as->hic,as->lpInputFormat);
		as->lpCompressFormat = HeapAlloc(GetProcessHeap(),0,xsize);
		ICCompressGetFormat(as->hic,as->lpInputFormat,as->lpCompressFormat);
		ICCompressBegin(as->hic,as->lpInputFormat,as->lpCompressFormat);
		as->compbufsize = ICCompressGetSize(as->hic,as->lpInputFormat,as->lpCompressFormat);
		as->compbuffer = HeapAlloc(GetProcessHeap(),0,as->compbufsize);

		/* Set up the Decompressor part (for prev frames?) */
		xsize=ICDecompressGetFormatSize(as->hic,as->lpCompressFormat);
		as->decompformat = HeapAlloc(GetProcessHeap(),0,xsize);
		ICDecompressGetFormat(as->hic,as->lpCompressFormat,as->decompformat);
		as->decompbufsize=((LPBITMAPINFOHEADER)as->decompbuffer)->biSizeImage;
		as->decompbuffer = HeapReAlloc(GetProcessHeap(),0,as->decompbuffer,as->decompbufsize);
		memset(as->decompbuffer,0xff,as->decompbufsize);
		assert(HeapValidate(GetProcessHeap(),0,NULL));

		ICDecompressGetFormat(as->hic,as->lpCompressFormat,as->decompformat);
		ICDecompressBegin(as->hic,as->lpCompressFormat,as->decompformat);
		as->lpPrev = as->lpPrevFormat = NULL;
	}
	return S_OK;
}

static HRESULT WINAPI IAVIStream_fnRead(IAVIStream*iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,LONG *bytesread,LONG *samplesread) {
	FIXME("(%p)->Read(%ld,%ld,%p,%ld,%p,%p)\n",iface,start,samples,buffer,buffersize,bytesread,samplesread);
	return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnWrite(IAVIStream*iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,DWORD flags,LONG *sampwritten,LONG *byteswritten) {
	IAVIStreamImpl	*as = (IAVIStreamImpl*)iface;
	DWORD		ckid,xflags;

	FIXME("(%p)->Write(%ld,%ld,%p,%ld,0x%08lx,%p,%p)\n",iface,start,samples,buffer,buffersize,flags,sampwritten,byteswritten);

	ICCompress(
		as->hic,flags,
		as->lpCompressFormat,
		as->compbuffer,
		as->lpInputFormat,buffer,
		&ckid,&xflags,
		as->curframe,0xffffff/*framesize*/,as->aco.dwQuality,
		as->lpPrevFormat,as->lpPrev
	);
	ICDecompress(
		as->hic,
		flags,	/* FIXME: check */
		as->lpCompressFormat,
		as->compbuffer,
		as->decompformat,
		as->decompbuffer
	);
	/* We now have a prev format for the next compress ... */
	as->lpPrevFormat = as->decompformat;
	as->lpPrev = as->decompbuffer;
	return S_OK;
}

static HRESULT WINAPI IAVIStream_fnDelete(IAVIStream*iface,LONG start,LONG samples) {
	FIXME("(%p)->Delete(%ld,%ld)\n",iface,start,samples);
	return E_FAIL;
}
static HRESULT WINAPI IAVIStream_fnReadData(IAVIStream*iface,DWORD fcc,LPVOID lp,LONG *lpread) {
	FIXME("(%p)->ReadData(0x%08lx,%p,%p)\n",iface,fcc,lp,lpread);
	return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnWriteData(IAVIStream*iface,DWORD fcc,LPVOID lp,LONG size) {
	FIXME("(%p)->WriteData(0x%08lx,%p,%ld)\n",iface,fcc,lp,size);
	return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnSetInfo(IAVIStream*iface,AVISTREAMINFOW*info,LONG infolen) {
	FIXME("(%p)->SetInfo(%p,%ld)\n",iface,info,infolen);
	return E_FAIL;
}

/***********************************************************************
 *		AVIFileCreateStreamA (AVIFIL32.@)
 */
HRESULT WINAPI AVIFileCreateStreamA(PAVIFILE iface,PAVISTREAM *ppavi,AVISTREAMINFOA * psi) {
	AVISTREAMINFOW	psiw;

	/* Only the szName at the end is different */
	memcpy(&psiw,psi,sizeof(*psi)-sizeof(psi->szName));
        MultiByteToWideChar( CP_ACP, 0, psi->szName, -1,
                             psiw.szName, sizeof(psiw.szName) / sizeof(WCHAR) );
	return IAVIFile_CreateStream(iface,ppavi,&psiw);
}

/***********************************************************************
 *		AVIFileCreateStreamW (AVIFIL32.@)
 */
HRESULT WINAPI AVIFileCreateStreamW(IAVIFile*iface,PAVISTREAM*avis,AVISTREAMINFOW*asi) {
	return IAVIFile_CreateStream(iface,avis,asi);
}


/***********************************************************************
 *		AVIFileGetStream (AVIFIL32.@)
 *		AVIFileGetStream (AVIFILE.143)
 */
HRESULT WINAPI AVIFileGetStream(IAVIFile*iface,PAVISTREAM*avis,DWORD fccType,LONG lParam) {
	return IAVIFile_GetStream(iface,avis,fccType,lParam);
}

/***********************************************************************
 *		AVIFileInfoA (AVIFIL32.@)
 */
HRESULT WINAPI AVIFileInfoA(PAVIFILE iface,LPAVIFILEINFOA afi,LONG size) {
	AVIFILEINFOW	afiw;
	HRESULT		hres;

	if (size < sizeof(AVIFILEINFOA))
		return AVIERR_BADSIZE;
	hres = IAVIFile_Info(iface,&afiw,sizeof(afiw));
	memcpy(afi,&afiw,sizeof(*afi)-sizeof(afi->szFileType));
        WideCharToMultiByte( CP_ACP, 0, afiw.szFileType, -1,
                             afi->szFileType, sizeof(afi->szFileType), NULL, NULL );
        afi->szFileType[sizeof(afi->szFileType)-1] = 0;
	return hres;
}

/***********************************************************************
 *		AVIStreamInfoW (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamInfoW(PAVISTREAM iface,AVISTREAMINFOW *asi,LONG
 size) {
 	return IAVIFile_Info(iface,asi,size);
}

/***********************************************************************
 *		AVIStreamInfoA (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamInfoA(PAVISTREAM iface,AVISTREAMINFOA *asi,LONG
 size) {
 	AVISTREAMINFOW	asiw;
	HRESULT			hres;

	if (size<sizeof(AVISTREAMINFOA))
		return AVIERR_BADSIZE;
 	hres = IAVIFile_Info(iface,&asiw,sizeof(asiw));
	memcpy(asi,&asiw,sizeof(asiw)-sizeof(asiw.szName));
        WideCharToMultiByte( CP_ACP, 0, asiw.szName, -1,
                             asi->szName, sizeof(asi->szName), NULL, NULL );
        asi->szName[sizeof(asi->szName)-1] = 0;
	return hres;
}

/***********************************************************************
 *		AVIFileInfoW (AVIFIL32.@)
 */
HRESULT WINAPI AVIFileInfoW(PAVIFILE iface,LPAVIFILEINFOW afi,LONG size) {
	return IAVIFile_Info(iface,afi,size);
}

/***********************************************************************
 *		AVIMakeCompressedStream (AVIFIL32.@)
 */
HRESULT WINAPI AVIMakeCompressedStream(PAVISTREAM *ppsCompressed,PAVISTREAM ppsSource,AVICOMPRESSOPTIONS *aco,CLSID *pclsidHandler) {
	char			fcc[5];
	IAVIStreamImpl	*as;
	FIXME("(%p,%p,%p,%p)\n",ppsCompressed,ppsSource,aco,pclsidHandler);
	fcc[4]='\0';
	memcpy(fcc,&(aco->fccType),4);
	FIXME("\tfccType: '%s'\n",fcc);
	memcpy(fcc,&(aco->fccHandler),4);
	FIXME("\tfccHandler: '%s'\n",fcc);
	FIXME("\tdwFlags: 0x%08lx\n",aco->dwFlags);

	/* we just create a duplicate for now */
	IAVIStream_AddRef(ppsSource);
	*ppsCompressed = ppsSource;
	as = (IAVIStreamImpl*)ppsSource;

	/* this is where the fun begins. Open a compressor and prepare it. */
	as->hic = ICOpen(aco->fccType,aco->fccHandler,ICMODE_COMPRESS);

	/* May happen. for instance if the codec is not able to compress */
	if (!as->hic)
		return AVIERR_UNSUPPORTED;

	ICGetInfo(as->hic,&(as->icinfo),sizeof(ICINFO));
	FIXME("Opened compressor: %s %s\n",debugstr_w(as->icinfo.szName),debugstr_w(as->icinfo.szDescription));
	as->iscompressing = TRUE;
	memcpy(&(as->aco),aco,sizeof(*aco));
	if (as->icinfo.dwFlags & VIDCF_COMPRESSFRAMES) {
		ICCOMPRESSFRAMES	icf;

		/* now what to fill in there ... Hmm */
		memset(&icf,0,sizeof(icf));
		icf.lDataRate 	= aco->dwBytesPerSecond;
		icf.lQuality 	= aco->dwQuality;
		icf.lKeyRate 	= aco->dwKeyFrameEvery;

		icf.GetData = (void *)0xdead4242;
		icf.PutData = (void *)0xdead4243;
		ICSendMessage(as->hic,ICM_COMPRESS_FRAMES_INFO,(LPARAM)&icf,sizeof(icf));
	}
	return S_OK;
}

/***********************************************************************
 *		AVIStreamSetFormat (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamSetFormat(PAVISTREAM iface,LONG pos,LPVOID format,LONG formatsize) {
	return IAVIStream_SetFormat(iface,pos,format,formatsize);
}

/***********************************************************************
 *		AVIStreamReadFormat (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamReadFormat(PAVISTREAM iface,LONG pos,LPVOID format,LONG *formatsize) {
	return IAVIStream_ReadFormat(iface,pos,format,formatsize);
}

/***********************************************************************
 *		AVIStreamWrite (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamWrite(PAVISTREAM iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,DWORD flags,LONG *sampwritten,LONG *byteswritten) {
	return IAVIStream_Write(iface,start,samples,buffer,buffersize,flags,sampwritten,byteswritten);
}

/***********************************************************************
 *		AVIStreamRead (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamRead(PAVISTREAM iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,LONG *bytesread,LONG *samplesread) {
	return IAVIStream_Read(iface,start,samples,buffer,buffersize,bytesread,samplesread);
}

/***********************************************************************
 *		AVIStreamWriteData (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamWriteData(PAVISTREAM iface,DWORD fcc,LPVOID lp,LONG size) {
	return IAVIStream_WriteData(iface,fcc,lp,size);
}

/***********************************************************************
 *		AVIStreamReadData (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamReadData(PAVISTREAM iface,DWORD fcc,LPVOID lp,LONG *lpread) {
	return IAVIStream_ReadData(iface,fcc,lp,lpread);
}

/***********************************************************************
 *		AVIStreamStart (AVIFIL32.@)
 */
LONG WINAPI AVIStreamStart(PAVISTREAM iface) {
	AVISTREAMINFOW	si;

	IAVIStream_Info(iface,&si,sizeof(si));
	return si.dwStart;
}

/***********************************************************************
 *		AVIStreamLength (AVIFIL32.@)
 */
LONG WINAPI AVIStreamLength(PAVISTREAM iface) {
	AVISTREAMINFOW	si;
	HRESULT			ret;

	ret = IAVIStream_Info(iface,&si,sizeof(si));
	if (ret) /* error */
		return 1;
	return si.dwLength;
}

/***********************************************************************
 *		AVIStreamRelease (AVIFIL32.@)
 */
ULONG WINAPI AVIStreamRelease(PAVISTREAM iface) {
	return IAVIStream_Release(iface);
}

/***********************************************************************
 *		AVIStreamGetFrameOpen (AVIFIL32.@)
 */
PGETFRAME WINAPI AVIStreamGetFrameOpen(PAVISTREAM iface,LPBITMAPINFOHEADER bmi) {
	FIXME("(%p)->(%p),stub!\n",iface,bmi);
	return NULL;
}

/***********************************************************************
 *		AVIStreamGetFrame (AVIFIL32.@)
 */
LPVOID WINAPI AVIStreamGetFrame(PGETFRAME pg,LONG pos) {
	return IGetFrame_GetFrame(pg,pos);
}

/***********************************************************************
 *		AVIStreamGetFrameClose (AVIFIL32.@)
 */
HRESULT WINAPI AVIStreamGetFrameClose(PGETFRAME pg) {
	if (pg) IGetFrame_Release(pg);
	return 0;
}

/***********************************************************************
 *		AVIFileRelease (AVIFIL32.@)
 *		AVIFileRelease (AVIFILE.141)
 */
ULONG WINAPI AVIFileRelease(PAVIFILE iface) {
	return IAVIFile_Release(iface);
}

/***********************************************************************
 *		AVIFileExit (AVIFIL32.@)
 *		AVIFileExit (AVIFILE.101)
 */
void WINAPI AVIFileExit(void) {
	FIXME("(), stub.\n");
}
