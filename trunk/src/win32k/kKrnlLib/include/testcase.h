/* $Id: testcase.h,v 1.1 2001-09-17 01:41:52 bird Exp $
 *
 * Definitions and declarations for testcase moduls.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _testcase_h_
#define _testcase_h_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _TstFaker
{
    unsigned   uAddress;
    int        fObj;                   /* 1 = CODE32, 2 = CODE16, 3 = DATA32, 4 = DATA16 */
} TSTFAKER, *PTSTFAKER, **PPTSTFAKER;
#ifdef NBR_OF_KRNLIMPORTS
extern TSTFAKER aTstFakers[NBR_OF_KRNLIMPORTS];
#endif

/*******************************************************************************
*   Function Prototypes.                                                       *
*******************************************************************************/
VOID  _Optlink ThunkStack32To16(VOID);  /* dh.asm */
VOID  _Optlink ThunkStack16To32(VOID);  /* dh.asm */
VOID  _Optlink dhinit(VOID);            /* dh.asm */
ULONG _Optlink getDHRouterFarPtr(VOID); /* dh.asm */
ULONG _Optlink InitkKrnlHlp(PVOID);          /* init.asm */
ULONG _Optlink InitkKrnlLib(PVOID);       /* init.asm */
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
ULONG LDRCALL fakeldrSetVMflags(
    PMTE        pMTE,
    ULONG       flObj,
    PULONG      pflFlags1,
    PULONG      pflFlags2
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
APIRET KRNLCALL fakeVMFreeMem(
    PVOID   pv,
    USHORT  hPTDA,
    ULONG   flFlags
    );
APIRET KRNLCALL fakeVMObjHandleInfo(
    USHORT  usHob,
    PULONG  pulAddr,
    PUSHORT pushPTDA);
PMTE KRNLCALL fakeldrASMpMTEFromHandle(
    HMTE  hMTE);
APIRET KRNLCALL fakeVMMapDebugAlias(
    ULONG   flVMFlags,
    ULONG   ulAddress,
    ULONG   cbSize,
    HPTDA   hPTDA,
    PVMAC   pvmac);
APIRET KRNLCALL fakeVMCreatePseudoHandle(
    PVOID   pvData,
    VMHOB   usOwner,
    PVMHOB  phob);
APIRET KRNLCALL fakeVMFreePseudoHandle(
    VMHOB   hob);


ULONG LDRCALL   fakeldrOpenPath(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful, ULONG lLibPath);
ULONG LDRCALL   fakeldrOpenPath_new(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful, ULONG lLibPath);
ULONG LDRCALL   fakeldrOpenPath_old(PCHAR pachFilename, USHORT cchFilename, ldrlv_t *plv, PULONG pful);
ULONG LDRCALL   fakeLDRClearSem(void);
ULONG LDRCALL   fakeldrFindModule(PCHAR pachFilename, USHORT cchFilename, USHORT usClass, PPMTE ppMTE);

ULONG LDRCALL   fakeldrCheckInternalName(PMTE pMTE);
ULONG LDRCALL   fakeldrTransPath(PSZ pszFilename);
ULONG LDRCALL   fakeldrGetFileName(PSZ pszFilename, PCHAR *ppchName, PCHAR *ppchExt);
VOID  LDRCALL   fakeldrUCaseString(PCHAR pachString, unsigned cchString);

ULONG KRNLCALL  fakeKSEMRequestMutex(HKSEMMTX hkmtx, ULONG ulTimeout);
ULONG KRNLCALL  fakeKSEMReleaseMutex(HKSEMMTX hkmtx);
BOOL  KRNLCALL  fakeKSEMQueryMutex(HKSEMMTX hkmtx, PUSHORT pcusNest);
VOID  KRNLCALL  fakeKSEMInit(PKSEM pksem, ULONG fulType, ULONG fulFlags);
extern KSEMMTX  fakeLdrSem;
extern char *   fakeLDRLibPath;
ULONG KRNLCALL  fakeTKFuBuff(PVOID pv, PVOID pvUsr, ULONG cb, ULONG fl);
ULONG KRNLCALL  fakeTKSuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl);
ULONG KRNLCALL  fakeTKFuBufLen(PLONG pcch, PVOID pvUsr, ULONG cchMax, ULONG fl, BOOL fDblNULL);
ULONG KRNLCALL  fakeTKSuFuBuff(PVOID pvUsr, PVOID pv, ULONG cb, ULONG fl);
ULONG KRNLCALL  fakeTKPidToPTDA(PID pid, PPPTDA ppPTDA);
ULONG KRNLCALL  fakeTKScanTasks(ULONG flFlags, ULONG id, PTKSCANTASKWORKER pfnWorker, ULONG ulArg);
void KRNLCALL   fakeTKForceThread(ULONG flFlag, PTCB pTCB);
void KRNLCALL   fakeTKForceTask(ULONG flFlag, PPTDA pPTDA, BOOL fForce);
ULONG KRNLCALL  fakeTKGetPriority(PTCB pTCB);
ULONG KRNLCALL  fakeTKSleep(ULONG ulSleepId, ULONG ulTimeout, ULONG fUnInterruptable, ULONG flWakeupType);
ULONG KRNLCALL  fakeTKWakeup(ULONG ulSleepId, ULONG flWakeupType, PULONG cWakedUp);
ULONG KRNLCALL  fakeTKWakeThread(PTCB pTCB);
PTCB  KRNLCALL  fakeTKQueryWakeup(ULONG ulSleepId, ULONG flWakeupType);

PMTE LDRCALL    fakeldrValidateMteHandle(HMTE hMTE);
PSZ  SECCALL    fakeSecPathFromSFN(SFN hFile);
ULONG KRNLCALL  fakePGPhysAvail(void);
ULONG KRNLCALL  fakePGPhysPresent(void);
VOID  KRNLCALL  fakevmRecalcShrBound(ULONG flFlags, PULONG pulSentinelAddress);

void _Optlink   fakeg_tkExecPgm(void);      /* Not callable! (fakea.asm) */
void _Optlink   faketkStartProcess(void);   /* Not callable! (fakea.asm) */
void _Optlink   fakef_FuStrLenZ(void);      /* Not callable! (fakea.asm) */
void _Optlink   fakef_FuStrLen(void);       /* Not callable! (fakea.asm) */
void _Optlink   fakef_FuBuff(void);         /* Not callable! (fakea.asm) */
void _Optlink   fakedh_SendEvent(void);     /* Not callable! (fakea.asm) */
void _Optlink   fakeh_POST_SIGNAL(void);    /* Not callable! (fakea.asm) */
void _Optlink   fakeRASRST(void);           /* Not callable! (fakea.asm) */
void _Optlink   fakeKMEnterKmodeSEF(void);  /* Not callable! (fakea.asm) */
void _Optlink   fakeKMExitKmodeSEF8(void);  /* Not callable! (fakea.asm) */
extern PTCB     fakepTCBCur;
extern PPTDA    fakepPTDACur;
extern char     fakeptda_start;
extern USHORT   fakeptda_environ;
extern KSEMMTX  fakeptda_ptdasem;
extern HMTE     fakeptda_handle;
extern HMTE     fakeptda_module;
extern PSZ      fakeptda_pBeginLIBPATH;
extern PSZ      fakeldrpFileNameBuf;
extern PMTE     fakemte_h;
extern PMTE     fakeglobal_h;
extern PMTE     fakeglobal_l;
extern PMTE     fakespecific_h;
extern PMTE     fakespecific_l;
extern PMTE     fakeprogram_h;
extern PMTE     fakeprogram_l;

extern ULONG fakeSMcDFInuse;
extern ULONG fakesmFileSize;
extern ULONG fakeSMswapping;
extern ULONG fakesmcBrokenDF;
extern ULONG fakepgPhysPages;
extern ULONG fakeSMcInMemFile;
extern ULONG fakeSMCFGMinFree;
extern ULONG fakesmcGrowFails;
extern ULONG fakePGSwapEnabled;
extern ULONG fakepgcPageFaults;
extern ULONG fakeSMCFGSwapSize;
extern ULONG fakepgResidentPages;
extern ULONG fakepgSwappablePages;
extern ULONG fakepgDiscardableInmem;
extern ULONG fakepgDiscardablePages;
extern ULONG fakeSMMinFree;
extern ULONG fakepgcPageFaultsActive;
extern ULONG fakepgPhysMax;
extern ULONG fakeVirtualAddressLimit;

extern VMAH  fakeahvmShr;
extern VMAH  fakeahvmSys;
extern VMAH  fakeahvmhShr;

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
