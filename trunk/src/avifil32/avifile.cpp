/*
 * Copyright 1999 Marcus Meissner
 */
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <odin.h>
#define CINTERFACE
#define ICOM_CINTERFACE 1
#define INITGUID

#include "winbase.h"
#include "vfw.h"
#include "wine/winestring.h"
#include "driver.h"
#include "mmsystem.h"
#include "winerror.h"
#include "debugstr.h"
#include "debugtools.h"

#include <misc.h>

DECLARE_DEBUG_CHANNEL(avifile)
DECLARE_DEBUG_CHANNEL(msvideo)
DECLARE_DEBUG_CHANNEL(relay)

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
        ICOM_VTABLE(IAVIStream)*        lpvtbl;
        DWORD           ref;
        /* IAVIStream stuff */
        LPVOID          lpInputFormat;
        DWORD           inputformatsize;
        BOOL            iscompressing;
        DWORD           curframe;

            /* Compressor stuff */
            HIC hic;
            LPVOID      lpCompressFormat;
            ICINFO      icinfo;
            DWORD       compbufsize;
            LPVOID      compbuffer;

            DWORD       decompbufsize;
            LPVOID      decompbuffer;
            LPVOID      decompformat;
            AVICOMPRESSOPTIONS  aco;

            LPVOID      lpPrev; /* pointer to decompressed frame later */
            LPVOID      lpPrevFormat; /* pointer to decompressed info later */
} IAVIStreamImpl;

void WINAPI
AVIFileInit(void) {
        FIXME_(avifile)("(),stub!\n");
}

typedef struct IAVIFileImpl {
        /* IUnknown stuff */
        ICOM_VTABLE(IAVIFile)*  lpvtbl;
        DWORD                           ref;
        /* IAVIFile stuff... */
} IAVIFileImpl;

static HRESULT WINAPI IAVIFile_fnQueryInterface(IAVIFile* iface,REFIID refiid,LPVOID *obj) {
        ICOM_THIS(IAVIFileImpl,iface);
        char    xrefiid[50];

        WINE_StringFromCLSID((LPCLSID)refiid,xrefiid);
        TRACE_(relay)("(%p)->QueryInterface(%s,%p)\n",This,xrefiid,obj);
        if (    !memcmp(&IID_IUnknown,refiid,sizeof(IID_IUnknown)) ||
                !memcmp(&IID_IAVIFile,refiid,sizeof(IID_IAVIFile))
        ) {
                *obj = iface;
                return S_OK;
        }
        return OLE_E_ENUM_NOMORE;
}

static ULONG WINAPI IAVIFile_fnAddRef(IAVIFile* iface) {
        ICOM_THIS(IAVIFileImpl,iface);

        FIXME_(relay)("(%p)->AddRef()\n",iface);
        return ++(This->ref);
}

static ULONG WINAPI IAVIFile_fnRelease(IAVIFile* iface) {
        ICOM_THIS(IAVIFileImpl,iface);

        FIXME_(relay)("(%p)->Release()\n",iface);
        if (!--(This->ref)) {
                HeapFree(GetProcessHeap(),0,iface);
                return 0;
        }
        return This->ref;
}

static HRESULT WINAPI IAVIFile_fnInfo(IAVIFile*iface,AVIFILEINFOW*afi,LONG size) {
        FIXME_(avifile)("(%p)->Info(%p,%ld)\n",iface,afi,size);

        /* FIXME: fill out struct? */
        return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnGetStream(IAVIFile*iface,PAVISTREAM*avis,DWORD fccType,LONG lParam) {
        FIXME_(avifile)("(%p)->GetStream(%p,0x%08lx,%ld)\n",iface,avis,fccType,lParam);
        /* FIXME: create interface etc. */
        return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnCreateStream(IAVIFile*iface,PAVISTREAM*avis,AVISTREAMINFOW*asi) {
        ICOM_THIS(IAVIStreamImpl,iface);
        char            fcc[5];
        IAVIStreamImpl  *istream;

        FIXME_(avifile)("(%p,%p,%p)\n",This,avis,asi);
        istream = (IAVIStreamImpl*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(IAVIStreamImpl));
        istream->ref = 1;
        istream->lpvtbl = &iavist;
        fcc[4]='\0';
        memcpy(fcc,(char*)&(asi->fccType),4);
        FIXME_(avifile)("\tfccType '%s'\n",fcc);
        memcpy(fcc,(char*)&(asi->fccHandler),4);
        FIXME_(avifile)("\tfccHandler '%s'\n",fcc);
        FIXME_(avifile)("\tdwFlags 0x%08lx\n",asi->dwFlags);
        FIXME_(avifile)("\tdwCaps 0x%08lx\n",asi->dwCaps);
        FIXME_(avifile)("\tname '%s'\n",debugstr_w(asi->szName));

        istream->curframe = 0;
        *avis = (PAVISTREAM)istream;
        return S_OK;
}

static HRESULT WINAPI IAVIFile_fnWriteData(IAVIFile*iface,DWORD ckid,LPVOID lpData,LONG size) {
        FIXME_(avifile)("(%p)->WriteData(0x%08lx,%p,%ld)\n",iface,ckid,lpData,size);
        /* FIXME: write data to file */
        return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnReadData(IAVIFile*iface,DWORD ckid,LPVOID lpData,LONG *size) {
        FIXME_(avifile)("(%p)->ReadData(0x%08lx,%p,%p)\n",iface,ckid,lpData,size);
        /* FIXME: read at most size bytes from file */
        return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnEndRecord(IAVIFile*iface) {
        FIXME_(avifile)("(%p)->EndRecord()\n",iface);
        /* FIXME: end record? */
        return E_FAIL;
}

static HRESULT WINAPI IAVIFile_fnDeleteStream(IAVIFile*iface,DWORD fccType,LONG lParam) {
        FIXME_(avifile)("(%p)->DeleteStream(0x%08lx,%ld)\n",iface,fccType,lParam);
        /* FIXME: delete stream? */
        return E_FAIL;
}

HRESULT WINAPI AVIFileOpenA(
        PAVIFILE * ppfile,LPCSTR szFile,UINT uMode,LPCLSID lpHandler
) {
        char    buf[80];
        IAVIFileImpl    *iavi;


        if (HIWORD(lpHandler))
                WINE_StringFromCLSID(lpHandler,buf);
        else
                sprintf(buf,"<clsid-0x%04lx>",(DWORD)lpHandler);

        FIXME_(avifile)("(%p,%s,0x%08lx,%s),stub!\n",ppfile,szFile,(DWORD)uMode,buf);
        iavi = (IAVIFileImpl*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(IAVIFileImpl));
        iavi->ref = 1;
        iavi->lpvtbl = &iavift;
        *ppfile = (PAVIFILE)iavi;
        return S_OK;
}

static HRESULT WINAPI IAVIStream_fnQueryInterface(IAVIStream*iface,REFIID refiid,LPVOID *obj) {
        ICOM_THIS(IAVIStreamImpl,iface);
        char    xrefiid[50];

        WINE_StringFromCLSID((LPCLSID)refiid,xrefiid);
        TRACE_(relay)("(%p)->QueryInterface(%s,%p)\n",This,xrefiid,obj);
        if (    !memcmp(&IID_IUnknown,refiid,sizeof(IID_IUnknown)) ||
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

        FIXME_(relay)("(%p)->AddRef()\n",iface);
        return ++(This->ref);
}

static ULONG WINAPI IAVIStream_fnRelease(IAVIStream* iface) {
        ICOM_THIS(IAVIStreamImpl,iface);

        FIXME_(relay)("(%p)->Release()\n",iface);
        if (!--(This->ref)) {
                HeapFree(GetProcessHeap(),0,This);
                return 0;
        }
        return This->ref;
}

static HRESULT WINAPI IAVIStream_fnCreate(IAVIStream*iface,LPARAM lParam1,LPARAM lParam2) {
        FIXME_(avifile)("(%p)->Create(0x%08lx,0x%08lx)\n",iface,lParam1,lParam2);
        return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnInfo(IAVIStream*iface,AVISTREAMINFOW *psi,LONG size) {
        FIXME_(avifile)("(%p)->Info(%p,%ld)\n",iface,psi,size);
        return E_FAIL;
}

static LONG WINAPI IAVIStream_fnFindSample(IAVIStream*iface,LONG pos,LONG flags) {
        FIXME_(avifile)("(%p)->FindSample(%ld,0x%08lx)\n",iface,pos,flags);
        return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnReadFormat(IAVIStream*iface,LONG pos,LPVOID format,LONG *formatsize) {
        FIXME_(avifile)("(%p)->ReadFormat(%ld,%p,%p)\n",iface,pos,format,formatsize);
        return E_FAIL;
}

/*****************************************************************************
 *                                              [IAVIStream::SetFormat]
 */
static HRESULT WINAPI IAVIStream_fnSetFormat(IAVIStream*iface,LONG pos,LPVOID format,LONG formatsize) {
        IAVIStreamImpl  *as = (IAVIStreamImpl*)iface;

        FIXME_(avifile)("(%p)->SetFormat(%ld,%p,%ld)\n",iface,pos,format,formatsize);
        if (as->lpInputFormat) HeapFree(GetProcessHeap(),0,as->lpInputFormat);
        as->inputformatsize = formatsize;
        as->lpInputFormat = HeapAlloc(GetProcessHeap(),0,formatsize);
        memcpy(as->lpInputFormat,format,formatsize);
        if (as->iscompressing) {
                int     xsize;
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
        FIXME_(avifile)("(%p)->Read(%ld,%ld,%p,%ld,%p,%p)\n",iface,start,samples,buffer,buffersize,bytesread,samplesread);
        return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnWrite(IAVIStream*iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,DWORD flags,LONG *sampwritten,LONG *byteswritten) {
        IAVIStreamImpl  *as = (IAVIStreamImpl*)iface;
        DWORD           ckid,xflags;

        FIXME_(avifile)("(%p)->Write(%ld,%ld,%p,%ld,0x%08lx,%p,%p)\n",iface,start,samples,buffer,buffersize,flags,sampwritten,byteswritten);

        ICCompress(
                as->hic,flags,
                (LPBITMAPINFOHEADER)as->lpCompressFormat,
                as->compbuffer,
                (LPBITMAPINFOHEADER)as->lpInputFormat,
                buffer,
                &ckid,&xflags,
                as->curframe,0xffffff/*framesize*/,as->aco.dwQuality,
                (LPBITMAPINFOHEADER)as->lpPrevFormat,as->lpPrev
        );
        ICDecompress(
                as->hic,
                flags,  /* FIXME: check */
                (LPBITMAPINFOHEADER)as->lpCompressFormat,
                as->compbuffer,
                (LPBITMAPINFOHEADER)as->decompformat,
                as->decompbuffer
        );
        /* We now have a prev format for the next compress ... */
        as->lpPrevFormat = as->decompformat;
        as->lpPrev = as->decompbuffer;
        return S_OK;
}

static HRESULT WINAPI IAVIStream_fnDelete(IAVIStream*iface,LONG start,LONG samples) {
        FIXME_(avifile)("(%p)->Delete(%ld,%ld)\n",iface,start,samples);
        return E_FAIL;
}
static HRESULT WINAPI IAVIStream_fnReadData(IAVIStream*iface,DWORD fcc,LPVOID lp,LONG *lpread) {
        FIXME_(avifile)("(%p)->ReadData(0x%08lx,%p,%p)\n",iface,fcc,lp,lpread);
        return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnWriteData(IAVIStream*iface,DWORD fcc,LPVOID lp,LONG size) {
        FIXME_(avifile)("(%p)->WriteData(0x%08lx,%p,%ld)\n",iface,fcc,lp,size);
        return E_FAIL;
}

static HRESULT WINAPI IAVIStream_fnSetInfo(IAVIStream*iface,AVISTREAMINFOW*info,LONG infolen) {
        FIXME_(avifile)("(%p)->SetInfo(%p,%ld)\n",iface,info,infolen);
        return E_FAIL;
}

HRESULT WINAPI AVIFileCreateStreamA(PAVIFILE iface,PAVISTREAM *ppavi,AVISTREAMINFOA * psi) {
        AVISTREAMINFOW  psiw;

        /* Only the szName at the end is different */
        memcpy(&psiw,psi,sizeof(*psi)-sizeof(psi->szName));
        lstrcpynAtoW(psiw.szName,psi->szName,sizeof(psi->szName));
        return iface->lpvtbl->fnCreateStream(iface,ppavi,&psiw);
}

HRESULT WINAPI AVIFileCreateStreamW(IAVIFile*iface,PAVISTREAM*avis,AVISTREAMINFOW*asi) {
        return iface->lpvtbl->fnCreateStream(iface,avis,asi);
}


HRESULT WINAPI AVIFileGetStream(IAVIFile*iface,PAVISTREAM*avis,DWORD fccType,LONG lParam) {
        return iface->lpvtbl->fnGetStream(iface,avis,fccType,lParam);
}

HRESULT WINAPI AVIFileInfoA(PAVIFILE iface,LPAVIFILEINFOA afi,LONG size) {
        AVIFILEINFOW    afiw;
        HRESULT         hres;

        if (size < sizeof(AVIFILEINFOA))
                return AVIERR_BADSIZE;
        hres = iface->lpvtbl->fnInfo(iface,&afiw,sizeof(afiw));
        memcpy(afi,&afiw,sizeof(*afi)-sizeof(afi->szFileType));
        lstrcpynWtoA(afi->szFileType,afiw.szFileType,sizeof(afi->szFileType));
        return hres;
}

HRESULT WINAPI AVIStreamInfoW(PAVISTREAM iface,AVISTREAMINFOW *asi,LONG
 size) {
        return iface->lpvtbl->fnInfo(iface,asi,size);
}

HRESULT WINAPI AVIStreamInfoA(PAVISTREAM iface,AVISTREAMINFOA *asi,LONG
 size) {
        AVISTREAMINFOW  asiw;
        HRESULT                 hres;

        if (size<sizeof(AVISTREAMINFOA))
                return AVIERR_BADSIZE;
        hres = iface->lpvtbl->fnInfo(iface,&asiw,sizeof(asiw));
        memcpy(asi,&asiw,sizeof(asiw)-sizeof(asiw.szName));
        lstrcpynWtoA(asi->szName,asiw.szName,sizeof(asi->szName));
        return hres;
}

HRESULT WINAPI AVIFileInfoW(PAVIFILE iface,LPAVIFILEINFOW afi,LONG size) {
        return iface->lpvtbl->fnInfo(iface,afi,size);
}

HRESULT WINAPI AVIMakeCompressedStream(PAVISTREAM *ppsCompressed,PAVISTREAM ppsSource,AVICOMPRESSOPTIONS *aco,CLSID *pclsidHandler) {
        char                    fcc[5];
        IAVIStreamImpl  *as;
        FIXME_(avifile)("(%p,%p,%p,%p)\n",ppsCompressed,ppsSource,aco,pclsidHandler);
        fcc[4]='\0';
        memcpy(fcc,&(aco->fccType),4);
        FIXME_(avifile)("\tfccType: '%s'\n",fcc);
        memcpy(fcc,&(aco->fccHandler),4);
        FIXME_(avifile)("\tfccHandler: '%s'\n",fcc);
        FIXME_(avifile)("\tdwFlags: 0x%08lx\n",aco->dwFlags);

        /* we just create a duplicate for now */
        ((IUnknown*)ppsSource)->lpvtbl->fnAddRef((IUnknown*)ppsSource);
        *ppsCompressed = ppsSource;
        as = (IAVIStreamImpl*)ppsSource;

        /* this is where the fun begins. Open a compressor and prepare it. */
        as->hic = ICOpen(aco->fccType,aco->fccHandler,ICMODE_COMPRESS);

        /* May happen. for instance if the codec is not able to compress */
        if (!as->hic)
                return AVIERR_UNSUPPORTED;

        ICGetInfo(as->hic,&(as->icinfo),sizeof(ICINFO));
        FIXME_(avifile)("Opened compressor: '%s' '%s'\n",debugstr_w(as->icinfo.szName),debugstr_w(as->icinfo.szDescription));
        as->iscompressing = TRUE;
        memcpy(&(as->aco),aco,sizeof(*aco));
        if (as->icinfo.dwFlags & VIDCF_COMPRESSFRAMES) {
                ICCOMPRESSFRAMES        icf;

                /* now what to fill in there ... Hmm */
                memset(&icf,0,sizeof(icf));
                icf.lDataRate   = aco->dwBytesPerSecond;
                icf.lQuality    = aco->dwQuality;
                icf.lKeyRate    = aco->dwKeyFrameEvery;

                icf.GetData = (LONG (_stdcall *)(LPARAM,LONG,LPVOID,LONG)) 0xdead4242;
                icf.PutData = (LONG (_stdcall *)(LPARAM,LONG,LPVOID,LONG)) 0xdead4243;
                ICSendMessage(as->hic,ICM_COMPRESS_FRAMES_INFO,(LPARAM)&icf,sizeof(icf));
        }
        return S_OK;
}

HRESULT WINAPI AVIStreamSetFormat(PAVISTREAM iface,LONG pos,LPVOID format,LONG formatsize) {
        return iface->lpvtbl->fnSetFormat(iface,pos,format,formatsize);
}

HRESULT WINAPI AVIStreamReadFormat(PAVISTREAM iface,LONG pos,LPVOID format,LONG *formatsize) {
        return iface->lpvtbl->fnReadFormat(iface,pos,format,formatsize);
}

HRESULT WINAPI AVIStreamWrite(PAVISTREAM iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,DWORD flags,LONG *sampwritten,LONG *byteswritten) {
        return iface->lpvtbl->fnWrite(iface,start,samples,buffer,buffersize,flags,sampwritten,byteswritten);
}

HRESULT WINAPI AVIStreamRead(PAVISTREAM iface,LONG start,LONG samples,LPVOID buffer,LONG buffersize,LONG *bytesread,LONG *samplesread) {
        return iface->lpvtbl->fnRead(iface,start,samples,buffer,buffersize,bytesread,samplesread);
}

HRESULT WINAPI AVIStreamWriteData(PAVISTREAM iface,DWORD fcc,LPVOID lp,LONG size) {
        return iface->lpvtbl->fnWriteData(iface,fcc,lp,size);
}

HRESULT WINAPI AVIStreamReadData(PAVISTREAM iface,DWORD fcc,LPVOID lp,LONG *lpread) {
        return iface->lpvtbl->fnReadData(iface,fcc,lp,lpread);
}

LONG WINAPI AVIStreamStart(PAVISTREAM iface) {
        AVISTREAMINFOW  si;

        iface->lpvtbl->fnInfo(iface,&si,sizeof(si));
        return si.dwStart;
}

LONG WINAPI AVIStreamLength(PAVISTREAM iface) {
        AVISTREAMINFOW  si;
        HRESULT                 ret;

        ret = iface->lpvtbl->fnInfo(iface,&si,sizeof(si));
        if (ret) /* error */
                return 1;
        return si.dwLength;
}

ULONG WINAPI AVIStreamRelease(PAVISTREAM iface) {
        return ((LPUNKNOWN)iface)->lpvtbl->fnRelease((LPUNKNOWN)iface);
}

PGETFRAME WINAPI AVIStreamGetFrameOpen(PAVISTREAM iface,LPBITMAPINFOHEADER bmi) {
        FIXME_(msvideo)("(%p)->(%p),stub!\n",iface,bmi);
        return NULL;
}

LPVOID WINAPI AVIStreamGetFrame(PGETFRAME pg,LONG pos) {
        return pg->lpvtbl->fnGetFrame(pg,pos);
}

HRESULT WINAPI AVIStreamGetFrameClose(PGETFRAME pg) {
        if (pg) ((LPUNKNOWN)pg)->lpvtbl->fnRelease((LPUNKNOWN)pg);
        return 0;
}

ULONG WINAPI AVIFileRelease(PAVIFILE iface) {
        return ((LPUNKNOWN)iface)->lpvtbl->fnRelease((LPUNKNOWN)iface);
}

void WINAPI AVIFileExit(void) {
        FIXME_(avifile)("(), stub.\n");
}
