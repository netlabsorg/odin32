/* $Id: PerTaskW32kData.c,v 1.2 2001-07-10 16:39:19 bird Exp $
 *
 * Per Task (Win32k) Data.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS                  /* Error codes */
#define INCL_OS2KRNL_VM                 /* OS2KRNL: Virtual Memory Management */
#define INCL_OS2KRNL_TK                 /* OS2KRNL: Task Stuff */
#define INCL_OS2KRNL_PTDA

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include "dev32hlp.h"
#include "log.h"
#include "OS2Krnl.h"
#include <string.h>
#include "macros.h"
#include "avl.h"
#include "PerTaskW32kData.h"
#include "rmalloc.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*static*/ PAVLNODECORE pPTDTree;       /* Pointer to PTD tree */
                                        /* Currently assumed protected by ldrSem. */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
PID InternalGetPid(void);


/**
 * Get the task data structure for a given process id.
 * If it doesn't exist we'll allocate a new one.
 * @returns Pointer to task data structure.
 *          Might be NULL (see fCreate).
 * @param   pPTDA   Pointer to PTDA for the task.
 * @param   fCreate TRUE:  Create structure if not found.
 *                  FALSE: Return NULL if not found.
 * @status  completely implemented.
 * @author  knut st. osmundsen (kosmunds@csc.no)
 */
PPTD    GetTaskData(PPTDA pPTDA, BOOL fCreate)
{
    PPTD    pptd;
    if (pPTDA == 0)
        pPTDA = ptdaGetCur();
    pptd = (PPTD)(void*)AVLGet(&pPTDTree, (AVLKEY)pPTDA);
    if (!pptd && fCreate)
    {
        pptd = rmalloc(sizeof(*pptd));
        if (!pptd)
        {
            kprintf(("GetTaskData: rmalloc failed!\n"));
            return NULL;
        }
        memset(pptd, 0, sizeof(*pptd));
        pptd->core.Key = (AVLKEY)pPTDA;
        AVLInsert(&pPTDTree, &pptd->core);
    }
    return pptd;
}


/**
 * Remove the give process id.
 * @param   pPTDA   Pointer to PTDA for the task.
 * @status  completely implemented.
 * @author  knut st. osmundsen (kosmunds@csc.no)
 */
void    RemoveTaskData(PPTDA pPTDA)
{
    PPTD  pptd;
    if (pPTDA == 0)
        pPTDA = ptdaGetCur();
    pptd = (PPTD)(void*)AVLRemove(&pPTDTree, (AVLKEY)pPTDA);
    if (pptd)
    {
        /* perhaps free data here... */
        if (*(PULONG)(void*)&pptd->lockOdin32Env)
        {
            D32Hlp_VMUnLock(&pptd->lockOdin32Env);
            memset(&pptd->lockOdin32Env, 0, sizeof(pptd->lockOdin32Env));
        }
        pptd->pszzOdin32Env = NULL;
        rfree(pptd);
    }
}


/**
 * Get the current Process Id.
 * @returns Process id.
 *          -1 on error.
 * @status  completely implemented.
 * @author  knut st. osmundsen (kosmunds@csc.no)
 * @remark
 */
PID InternalGetPid(void)
{
    struct InfoSegLDT * pLIS;
    pLIS = (struct InfoSegLDT*)D32Hlp_GetDOSVar(DHGETDOSV_LOCINFOSEG, 0);
    if (!pLIS)
    {
        kprintf(("InternalGetPid: D32Hlp_GetDOSVar(DHGETDOSV_LOCINFOSEG, 0) failed\n"));
        return (PID)-1;
    }
    return pLIS->LIS_CurProcID;
}

