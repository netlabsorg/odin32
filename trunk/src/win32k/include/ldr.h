/* $Id: ldr.h,v 1.1 1999-09-06 02:19:58 bird Exp $
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

        /***************************************************************/
        /* handle state - Array of handle states. Four state per byte! */
        /***************************************************************/
        #define MAX_FILE_HANDLES 0x10000

        extern unsigned char ahStates[MAX_FILE_HANDLES/4];

        #define HSTATE_UNUSED       0x00    /* nowhere */
        #define HSTATE_CHECK        0x01    /* look in uncertain files */
        #define HSTATE_NOT_PE       0x02    /* nowhere */
        #define HSTATE_PE           0x03    /* look in PE handles */
        #define HSTATE_MASK         0xFC
        #define HSTATE_UMASK        0x03

        #define GetState(a)         (HSTATE_UMASK & (ahStates[a/4] >> (a%4)*2))
        #define SetState(a,b)       (ahStates[a/4] = (ahStates[a/4] & (HSTATE_MASK << (a%4)*2 | HSTATE_MASK >> 8-(a%4)*2) | b << (a%4)*2))



        /*********************/
        /* uncertain file(s) */
        /*********************/
        #define MAX_UNCERTAIN_FILES 0x10 /* probably never more that one uncertain file at the time */

        typedef struct _uncertain
        {
            SFN             hFile;
            ULONG           offsetNEHdr;
            PSZ             pszName;
            unsigned int    fMZ:1;
            unsigned int    fPE:1;
        } UNCERTAIN, *PUNCERTAIN;

        extern UNCERTAIN ahUncertain[MAX_UNCERTAIN_FILES];

        ULONG getFreeUncertainEntry(void);
        ULONG freeUncertainEntry(SFN hFile);
        ULONG findUncertainEntry(SFN hFile);


        /**************/
        /* PE handles */
        /**************/
        typedef struct _PENode
        {
            /* linking stuff */
            struct _PENode *left;
            struct _PENode *right;

            /* key */
            SFN hFile;                      /* system file number or file handle if you prefer that */

            /* misc */
            PMTE pMTE;                      /* pointer to MTE if we got one - may be NULL */

            /* LXFile object */
            LXFile lxfile;
        } PENODE, *PPENODE;

        #define SIZEOF_NODE (sizeof(NODE))

        ULONG       insertNode(PPENODE pNode);
        ULONG       deleteNode(SFN key);
        PPENODE     getNodePtr(SFN key);
        PPENODE     findNodePtr(const char *pszFilename);
        ULONG       depthPE(void);
        PPENODE     allocateNode(void);

        /* if sequential insertion - this will give a lower tree. */
        /* testing shows that 3 gives best results for 27 to 134 nodes */
        #define ROTATION 3
        #define AdjustKey(a) ((USHORT)(a << 16-ROTATION) | (USHORT)(a >> ROTATION) )
        #define UnAdjustKey(a) ((USHORT)(a >> 16-ROTATION) | (USHORT)(a << ROTATION) )

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
