/* $Id: ldr.h,v 1.5 2000-01-22 18:20:59 bird Exp $
 *
 * ldr - loader header file.
 *
 * Copyright (c)  1999 knut  St.  osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
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
                ModuleBase *pModule;    /* Pointer to a Pe2Lx object. (Win32 executables) */
                #if defined(_PE2LX_H_)
                Pe2Lx *     pPe2Lx;     /* Pointer to a Pe2Lx object. (Win32 executables) */
                #endif
                #if defined(_ELF2LX_H_)
                Elf2Lx *    pElf2Lx;    /* Pointer to a Elf2Lx object. (ELF executables) */
                #endif
                #if defined(_SCRIPT_H_)
                Script *    pScript;    /* Pointer to a Script object. (Shell scripts) */
                #endif
                #if defined(_PE_H_)
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
         * Modules operations.
         */
        PMODULE     getModuleBySFN(SFN hFile);
        PMODULE     getModuleByMTE(PMTE pMTE);
        PMODULE     getModuleByFilename(PCSZ pszFilename);

        ULONG       addModule(SFN hFile, PMTE pMTE, ULONG fFlags, ModuleBase *pModObj);
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
