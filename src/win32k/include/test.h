/* $Id: test.h,v 1.1.4.7 2000-08-28 22:44:23 bird Exp $
 *
 * Definitions and declarations for test moduls.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _test_h_
#define _test_h_

#ifdef __cplusplus
extern "C" {
#endif

VOID  _Optlink ThunkStack32To16(VOID);  /* dh.asm */
VOID  _Optlink ThunkStack16To32(VOID);  /* dh.asm */
VOID  _Optlink dhinit(VOID);            /* dh.asm */
ULONG _Optlink getDHRouterFarPtr(VOID); /* dh.asm */
ULONG _Optlink InitElf(PVOID);          /* init.asm */
ULONG _Optlink InitWin32k(PVOID);       /* init.asm */
ULONG _Optlink CalltkExecPgm(ULONG execFlags, PCSZ pArg, PCSZ pEnv, PCSZ pExecName); /* fakea.asm */

USHORT  _Optlink GetSelectorCODE16(void);
USHORT  _Optlink GetSelectorDATA16(void);
USHORT  _Optlink GetSelectorCODE32(void);
USHORT  _Optlink GetSelectorDATA32(void);


#ifndef INCL_NO_FAKE
/* fake functions */
ULONG LDRCALL fakeldrOpen(PSFN phFile, PSZ pszFilename, PULONG pfl);
ULONG LDRCALL fakeldrClose(SFN hFile);
ULONG LDRCALL fakeldrRead(SFN hFile, ULONG ulOffset, PVOID pvBuffer, ULONG fpBuffer, ULONG cbToRead, PMTE pMTE);
ULONG LDRCALL fakeLDRQAppType(PSZ pszFilename, PULONG pul);
ULONG LDRCALL fakeldrEnum32bitRelRecs(
    PMTE    pMTE,
    ULONG   iObject,
    ULONG   iPageTable,
    PVOID   pvPage,
    ULONG   ulPageAddress,
    PVOID   pvPTDA
    );
APIRET KRNLCALL fakeIOSftOpen(
    PSZ pszFilename,
    ULONG flOpenFlags,
    ULONG fsOpenMode,
    PSFN phFile,
    PULONG pulsomething
    );
APIRET KRNLCALL fakeIOSftClose(
    SFN hFile
    );
APIRET KRNLCALL fakeIOSftTransPath(
    PSZ pszPath
    );
APIRET KRNLCALL fakeIOSftReadAt(
    SFN hFile,
    PULONG pcbActual,
    PVOID pvBuffer,
    ULONG fpBuffer,
    ULONG ulOffset
    );
APIRET KRNLCALL fakeIOSftWriteAt(
    SFN hFile,
    PULONG pcbActual,
    PVOID pvBuffer,
    ULONG fpBuffer,
    ULONG ulOffset
    );
APIRET KRNLCALL fakeSftFileSize(
    SFN hFile,
    PULONG pcbFile
    );
HMTE KRNLCALL fakeVMGetOwner(
    ULONG ulCS,
    ULONG ulEIP);
APIRET KRNLCALL fakeVMAllocMem(
    ULONG   cbSize,
    ULONG   cbCommit,
    ULONG   flFlags1,
    HPTDA   hPTDA,
    USHORT  usVMOwnerId,
    HMTE    hMTE,
    ULONG   flFlags2,
    ULONG   SomeArg2,
    PVMAC   pvmac);
APIRET KRNLCALL fakeVMObjHandleInfo(
    USHORT  usHob,
    PULONG  pulAddr,
    PUSHORT pushPTDA);
PMTE KRNLCALL fakeldrASMpMTEFromHandle(
    HMTE  hMTE);
ULONG LDRCALL   fakeldrOpenPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful);
ULONG LDRCALL   fakeLDRClearSem(void);
ULONG LDRCALL   fakeldrFindModule(PCHAR pachFilename, USHORT cchFilename, USHORT usClass, PPMTE ppMTE);
ULONG KRNLCALL  fakeKSEMRequestMutex(HKSEMMTX hkmtx, ULONG ulTimeout);
ULONG KRNLCALL  fakeKSEMReleaseMutex(HKSEMMTX hkmtx);
BOOL  KRNLCALL  fakeKSEMQueryMutex(HKSEMMTX hkmtx, PUSHORT pcusNest);
VOID  KRNLCALL  fakeKSEMInit(PKSEM pksem, ULONG fulType, ULONG fulFlags);
extern KSEMMTX  fakeLDRSem;
extern char *   fakeLDRLibPath;
ULONG KRNLCALL  fakeTKFuBuff(PVOID pv, PVOID pvUsr, ULONG cb, ULONG fl);
ULONG KRNLCALL  fakeTKSuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl);
ULONG KRNLCALL  fakeTKFuBufLen(PLONG pcch, PVOID pvUsr, ULONG cchMax, ULONG fl, BOOL fDblNULL);
PMTE LDRCALL    fakeldrValidateMteHandle(HMTE hMTE);
void _Optlink   fakeg_tkExecPgm(void);    /* Not callable! (fakea.asm) */
void _Optlink   fake_tkStartProcess(void);/* Not callable! (fakea.asm) */
void _Optlink   fakef_FuStrLenZ(void);    /* Not callable! (fakea.asm) */
void _Optlink   fakef_FuStrLen(void);     /* Not callable! (fakea.asm) */
void _Optlink   fakef_FuBuff(void);       /* Not callable! (fakea.asm) */
extern PTCB     fakepTCBCur;
extern PPTDA    fakepPTDACur;
extern char     fakeptda_start;
extern USHORT   fakeptda_environ;
extern KSEMMTX  fakeptda_ptdasem;
extern HMTE     fakeptda_module;
extern PSZ      fakeldrpFileNameBuf;

#endif /* INCL_NO_FAKE */

#ifdef INCL_16
extern USHORT   usFakeVerMajor;         /* define in probkrnl.c */
extern USHORT   usFakeVerMinor;         /* define in probkrnl.c */
#else
extern USHORT   _usFakeVerMajor;
extern USHORT   _usFakeVerMinor;
#endif

#ifdef _OS2Krnl_h_
extern int      cObjectsFake;           /* defined in win32ktst.c */
extern OTE      aKrnlOTE[24];           /* defined in win32ktst.c */
#endif

#ifdef __cplusplus
}
#endif

#endif
