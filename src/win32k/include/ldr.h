/* $Id: ldr.h,v 1.2 1999-10-14 01:16:49 bird Exp $
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

        extern unsigned char achHandleStates[MAX_FILE_HANDLES/8];

        #define HSTATE_UNUSED       0x00    /* Handle not used (or OS/2). */
        #define HSTATE_OS2          0x00    /* OS/2 module filehandle. */
        #define HSTATE_OUR          0x01    /* Our module filehandle. */
        #define HSTATE_MASK         0xFE
        #define HSTATE_UMASK        0x01

        #define GetState(a)         (HSTATE_UMASK & (achHandleStates[(a)/8] >> ((a)%8)))
        #define SetState(a,b)       (achHandleStates[(a)/8] = (achHandleStates[(a)/8] & (HSTATE_MASK << ((a)%8) | HSTATE_MASK >> 8-((a)%8)) | ((b) & 0x1) << ((a)%8)))


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

            /* Pe2Lx object */
            Pe2Lx *pPe2Lx;
        } PENODE, *PPENODE;

        #define SIZEOF_NODE (sizeof(NODE))

        ULONG       insertNode(PPENODE pNode);
        ULONG       deleteNode(SFN key);        /* removes from tree and freeNode */
        PPENODE     getNodePtr(SFN key);
        PPENODE     findNodePtr(const char *pszFilename);
        ULONG       depthPE(void);
        PPENODE     allocateNode(void);
        ULONG       freeNode(PPENODE pNode);    /* don't remove from tree! */

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
