/* $Id: ldr.h,v 1.7 2000-12-11 06:44:35 bird Exp $
 *
 * ldr - Our loader "subsystem" public header file.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _ldr_h_
#define _ldr_h_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef LDR_INCL_INITONLY

/*
 * Fail if dependent header files is missing
 */
#ifndef _AVL_H_
#error "You'll have to include avl.h before ldr.h!"
#endif


/** @design Loader State.
 *
 * Used to determin behaviour in different cases.
 * Use the isLdrState<State> macros to query current state.
 * IMPORTANT! Don't change this variable if you don't really mean it!
 *            And only change it thru the setLdrState* macros!
 *
 * The state is changing as follows:
 *    1) Load a new program
 *      mytkExecPgm will set the state to LDRSTATE_TKEXECPGM on successful overloading.
 *          myldrOpenPath will set the type part of the loaderbits. (EXE,DLL or UNSUPPORTED)
 *          (NB! myldrOpenPath is called several times. First for the EXE then for imported modules.)
 *              IF executable THEN myLdrOpen might set the LDRSTATE_OUR flag.
 *          myldrOpenPath will reset the type part of the loaderbits upon return.
 *      mytkExecPgm resets the state to LDRSTATE_UNKNOWN upon return.
 *
 *    2) Query program type.
 *      myLDRQAppType will set the state to LDRSTATE_LDRQAPPTYPE on entry.
 *          myldrOpenPath will set the type part of the loaderbits. (EXE,DLL or UNSUPPORTED)
 *          (NB! myldrOpenPath may be called several times.)
 *              IF executable THEN myLdrOpen might set the LDRSTATE_OUR flag.
 *          myldrOpenPath will reset the type part of the loaderbits upon return.
 *      myLDRQAppType resets the state to LDRSTATE_UNKNOWN upon return.
 *
 *    3) Unknown invocation - probably DosLoadModule. Base state is LDRSTATE_UNKNOWN.
 *          myldrOpenPath will set the type part of the loaderbits. (EXE,DLL or UNSUPPORTED)
 *          (NB! myldrOpenPath is probably called several times. Import modules.)
 *          myldrOpenPath will reset the type part of the loaderbits upon return.
 *
 */
extern ULONG    ulLdrState;

#define LDRSTATE_UNKNOWN        0       /* Default state - undertermined. */
#define LDRSTATE_TKEXECPGM      1       /* A program is being loaded. */
#define LDRSTATE_LDRQAPPTYPE    2       /* Loader called from LDRQAPPTYPE */
/*#define LDRSTATE_LOADMODULE     3 */  /* A module is being loaded by DosLoadModule. Not implemented! */
#define LDRSTATE_MASK           0x00FF  /* State mask. */

/*
 * The following flags are only valid when myldrOpenPath is on the stack!, ie. in ldrOpen.
 * These flags is the "loading-bits".
 */
#define LDRSTATE_EXE            0x0100  /* Flags telling that an executable is being opened. */
#define LDRSTATE_DLL            0x0200  /* Flags telling that an dll is being opened. */
#define LDRSTATE_UNSUPPORTED    0x0300  /* Flags telling to not override this open call. */
#define LDRSTATE_TYPE_MASK      0x0f00  /* Load Type Mask. */
/* The following flag will tell us if the executable which is loading is ours or not. */
#define LDRSTATE_OUREXE         0x1000
#define LDRSTATE_LOADERBITS     0xff00  /* Mask */


/*
 * Query macros.
 */
#define isLdrStateUnknown()             ((ulLdrState & LDRSTATE_MASK) == LDRSTATE_UNKNOWN)
#define isLdrStateExecPgm()             ((ulLdrState & LDRSTATE_MASK) == LDRSTATE_TKEXECPGM)
#define isLdrStateQAppType()            ((ulLdrState & LDRSTATE_MASK) == LDRSTATE_LDRQAPPTYPE)
/*#define isLdrStateLoadModule()        ((ulLdrState & LDRSTATE_MASK) == LDRSTATE_LOADMODULE)*/

#define isLdrStateLoadingEXE()          ((ulLdrState & LDRSTATE_TYPE_MASK) == LDRSTATE_EXE)
#define isLdrStateLoadingDLL()          ((ulLdrState & LDRSTATE_TYPE_MASK) == LDRSTATE_DLL)
#define isLdrStateLoadingUnsupported()  ((ulLdrState & LDRSTATE_TYPE_MASK) == LDRSTATE_UNSUPPORTED)

#define isLdrStateLoadingOurEXE()       (ulLdrState & LDRSTATE_OUREXE)


/*
 * Set macros.
 */
#define setLdrStateQAppType()           ulLdrState = LDRSTATE_LDRQAPPTYPE
#define setLdrStateUnknown()            ulLdrState = LDRSTATE_UNKNOWN
/*      setLdrStateExecPgm() isn't needed as this is in assembly source! */
/*#define setLdrStateLoadModule()         ulLdrState = LDRSTATE_LOADMODULE */

#define setLdrStateLoadingEXE()         ulLdrState = (ulLdrState & (ULONG)(~LDRSTATE_TYPE_MASK)) | LDRSTATE_EXE
#define setLdrStateLoadingDLL()         ulLdrState = (ulLdrState & (ULONG)(~LDRSTATE_TYPE_MASK)) | LDRSTATE_DLL
#define setLdrStateLoadingUnsupported() ulLdrState = (ulLdrState & (ULONG)(~LDRSTATE_TYPE_MASK)) | LDRSTATE_UNSUPPORTED
#define setLdrStateClearLoadingType()   ulLdrState &= (ULONG)(~LDRSTATE_TYPE_MASK)

#define setLdrStateLoadingOurEXE()      ulLdrState |= LDRSTATE_OUREXE


/*
 * Loader State assert macros.
 */
#define ASSERT_LdrStateUnknown(fn)      ASSERT_LdrState(fn, LDRSTATE_UNKNOWN)
#define ASSERT_LdrStateExecPgm(fn)      ASSERT_LdrState(fn, LDRSTATE_TKEXECPGM)
#define ASSERT_LdrStateQAppType(fn)     ASSERT_LdrState(fn, LDRSTATE_LDRQAPPTYPE)

#define ASSERT_LdrState(fn, state) \
    {                              \
        if ((ulLdrState & LDRSTATE_MASK) != (state)) \
        {                          \
            kprintf((fn ": assertion incorrect loader state. ulLdrState (%d) != " #state "(%d)", \
                     ulLdrState, state)); \
        }                          \
    }


/*
 * This flag is used by myldrOpenPath to communicate to myldrOpen that
 * the .DLL extention should be removed before opening the file.
 */
extern BOOL fldrOpenExtentionFix;


/*
 * handle state - Array of handle states. Eight state per byte!
 */
#define MAX_FILE_HANDLES 0x10000

extern unsigned char achHandleStates[MAX_FILE_HANDLES/8];

#define HSTATE_UNUSED       0x00    /* Handle not used (or OS/2). */
#define HSTATE_OS2          0x00    /* OS/2 module filehandle. */
#define HSTATE_OUR          0x01    /* Our module filehandle. */
#define HSTATE_MASK         0xFE
#define HSTATE_UMASK        0x01

#define GetState(a)         (HSTATE_UMASK & (achHandleStates[(a)/8] >> ((a)%8)))
#define SetState(a,b)       (achHandleStates[(a)/8] = (achHandleStates[(a)/8] & (HSTATE_MASK << ((a)%8) | HSTATE_MASK >> 8-((a)%8)) | ((b) & 0x1) << ((a)%8)))


/*
 * Declare the module classes used below in case they aren't declared yet.
 */
#ifdef __cplusplus
class ModuleBase;
class Pe2Lx;
class Elf2Lx;
#else
typedef char ModuleBase;
typedef char Pe2Lx;
typedef char Elf2Lx;
#endif


/*
 * Module struct.
 */
typedef struct _Module
{
    AVLNODECORE     coreKey;    /* Key is hFile. */
    AVLNODECORE     coreMTE;    /* Key is pMTE. */

    SFN             hFile;      /* System file number or file handle if you prefer that. */
    PMTE            pMTE;       /* Pointer to MTE if we got one - NULL is allowed. */

    ULONG           fFlags;     /* Flags. Flags if coreMte is in use and what Data contains. */
    union
    {
        ModuleBase *pModule;    /* Pointer to base module. */
        Pe2Lx *     pPe2Lx;     /* Pointer to a Pe2Lx object. (Win32 executables) */
        Elf2Lx *    pElf2Lx;    /* Pointer to a Elf2Lx object. (ELF executables) */
        #if 0
        Script *    pScript;    /* Pointer to a Script object. (Shell scripts) */
        Pe *        pPe;        /* Pointer to a Pe object. (Ring3 loader) */
        #endif
    } Data;                     /* Pointer to data. Currently it's allways a Pe2Lx object! */
} MODULE, *PMODULE;

#define MOD_FLAGS_IN_MTETREE    0x00000010UL /* The node is present in the MTE-tree. */
#define MOD_TYPE_MASK           0x0000000FUL /* Type mask. */
#define MOD_TYPE_PE2LX          0x00000001UL /* Pe2Lx module. */
#define MOD_TYPE_ELF2LX         0x00000002UL /* Elf2Lx module. */
#define MOD_TYPE_SCRIPT         0x00000003UL /* Script module. */
#define MOD_TYPE_PE             0x00000004UL /* Pe module. */


/*
 * Pointer to the currently loading executable module.
 * Available at tkExecPgm time when loading a converted module.
 */
extern PMODULE pExeModule;


/*
 * Modules operations.
 */
PMODULE     getModuleBySFN(SFN hFile);
PMODULE     getModuleByMTE(PMTE pMTE);
PMODULE     getModuleByhMTE(HMTE hMTE);
PMODULE     getModuleByFilename(PCSZ pszFilename);

ULONG       addModule(SFN hFile, PMTE pMTE, ULONG fFlags, ModuleBase *pModObj);
ULONG       removeModule(SFN hFile);



/*
 * mytkExecPgm variables and functions
 *
 * (See ldr\mytkExecPgm.asm for further info on these variabels and functions.)
 */
#define CCHFILENAME     261             /* This is defined in mytkExecPgm.asm too. */
#define CCHARGUMENTS   1536             /* This is defined in mytkExecPgm.asm too. */
extern const char   fTkExecPgm;
extern char         achTkExecPgmFilename[CCHFILENAME];
extern char         achTkExecPgmArguments[CCHARGUMENTS];
#endif
ULONG _Optlink  tkExecPgmEnvLength(void);
ULONG _Optlink  tkExecPgmCopyEnv(char *pachBuffer, unsigned cchBuffer);



/*
 * functions
 */
PSZ         ldrGetExePath(PSZ pszPath, BOOL fExecChild);
ULONG       ldrInit(void);

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif
