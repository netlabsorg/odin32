/* $Id: ldr.h,v 1.3 1999-10-27 02:02:56 bird Exp $
 *
 * ldr - loader header file.
 *
 * Copyright (c)  1999 knut  St.  osmundsen
 *
 */


#ifndef _ldr_h_
#define _ldr_h_
    #ifndef LDR_INCL_INITONLY
        /* state variable */
        extern BOOL fQAppType;

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
                Pe2Lx *     pPe2Lx;     /* Pointer to a Pe2Lx object. (Win32 executables) */
                #if 0
                Elf2Lx *    pElf2Lx;    /* Pointer to a Elf2Lx object. (ELF executables) */
                Script *    pScript;    /* Pointer to a Script object. (Shell scripts) */
                Pe *        pPe;        /* Pointer to a Pe object. (Ring3 loader) */
                #endif
                void *      pv;
            } Data;                     /* Pointer to data. Currently it's allways a Pe2Lx object! */
        } MODULE, *PMODULE;

        #define MOD_FLAGS_IN_MTETREE    0x00000010 /* The node is present in the MTE-tree. */
        #define MOD_TYPE_MASK           0x0000000F /* Type mask. */
        #define MOD_TYPE_PE2LX          0x00000001 /* Pe2Lx module. */
        #define MOD_TYPE_ELF2LX         0x00000002 /* Elf2Lx module. */
        #define MOD_TYPE_SCRIPT         0x00000003 /* Script module. */
        #define MOD_TYPE_PE             0x00000004 /* Pe module. */


        /*
         * Modules operations.
         */
        PMODULE     getModuleBySFN(SFN hFile);
        PMODULE     getModuleByMTE(PMTE pMTE);
        PMODULE     getModuleByFilename(PCSZ pszFilename);

        ULONG       addModule(SFN hFile, PMTE pMTE, ULONG fFlags, void *pData);
        ULONG       removeModule(SFN hFile);

    #endif

    /*************/
    /* functions */
    /*************/
    #ifdef __cplusplus
    extern "C" {
    #endif

    ULONG ldrInit(void);

    #ifdef __cplusplus
    }
    #endif

    #pragma pack()

#endif
