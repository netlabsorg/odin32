/* $Id: ldr.cpp,v 1.1 1999-09-06 02:20:00 bird Exp $
 *
 * ldr.cpp - Loader helper functions a structures.
 *
 * Copyright (c)  1999 knut  St.  osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <string.h>

#include "malloc.h"
#include "new.h"
#include "log.h"
#include "pefile.h"
#include "lx.h"
#include "ldr.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
unsigned char   ahStates[MAX_FILE_HANDLES/4];
UNCERTAIN       ahUncertain[MAX_UNCERTAIN_FILES];
PPENODE         pPE;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static PPENODE  findNodePtr2(PPENODE pRoot, const char *pszFilename);
static ULONG    freeNode(PPENODE pNode);
static ULONG    depth(PPENODE pNode);


/**
 * Get a free entry in the ahUncertain table.
 * @returns   Index to free entry in the ahUncertain table.
 */
ULONG getFreeUncertainEntry(void)
{
    ULONG i;

    i = 0;
    while (i < MAX_UNCERTAIN_FILES && ahUncertain[i].hFile != 0xFFFF)
        i++;

    return i < MAX_UNCERTAIN_FILES ? i : -1;
}


/**
 * Free uncertain entry.
 * Mark entry with hFile in the uncertain table as unused
 * @returns   NO_ERROR on success, -1 on error.
 * @param     hFile  Filehandle (key) to free.
 */
ULONG freeUncertainEntry(SFN hFile)
{
    int i;
    int rc;

    i = 0;
    while (i < MAX_UNCERTAIN_FILES && ahUncertain[i].hFile != hFile)
        i++;

    if (i < MAX_UNCERTAIN_FILES && ahUncertain[i].hFile == hFile)
    {
        ahUncertain[i].hFile = 0xFFFF;
        ahUncertain[i].offsetNEHdr = 0;
        ahUncertain[i].fMZ = 0;
        ahUncertain[i].fPE = 0;
        free(ahUncertain[i].pszName);
        ahUncertain[i].pszName = NULL;
        rc = NO_ERROR;
    }
    else
        rc  = -1;
    return rc;
}


/**
 * Find the uncertain entry for hFile
 * @returns   Index into ahUncertain if found. ~0UL on error.
 * @param     hFile  Filehandle (key) to find.
 */
ULONG findUncertainEntry(SFN hFile)
{
    int i = 0;
    while (i < MAX_UNCERTAIN_FILES && ahUncertain[i].hFile != hFile)
        i++;

    return i < MAX_UNCERTAIN_FILES ? (ULONG)i : ~0UL;
}


/**
 * Inserts a PENode into the pPE tree.
 * @returns    NO_ERROR on success. !0 on error.
 * @param      pNode  Pointer to node to insert.
 */
ULONG insertNode(PPENODE pNode)
{
    int level;
    PPENODE pPrev;
    PPENODE pTmp;

    if (pPE == NULL)
    {
        pPE = pNode;
        pNode->left = pNode->right = NULL;
    }
    else
    {
        level = 0;
        pPrev = NULL;
        pTmp = pPE;
        while (pTmp != NULL)
        {
            level = 0;
            pPrev = pTmp;
            pTmp = AdjustKey(pNode->hFile) < AdjustKey(pTmp->hFile) ? pTmp->left : pTmp->right;
        }

        if (pNode->hFile != pPrev->hFile)
        {
            if (AdjustKey(pNode->hFile) < AdjustKey(pPrev->hFile))
                pPrev->left = pNode;
            else
                pPrev->right = pNode;
            pNode->left = pNode->right = NULL;
        }
        else
            return -1;
    }

    return NO_ERROR;
}


/**
 * Deletes a node from the pPE tree.
 * @returns   NO_ERROR on success. !0 on error.
 * @param     key  Filehandle, which is the key.
 */
ULONG deleteNode(SFN key)
{
    int     level,level2;
    ULONG   rc;
    PPENODE pTmp,pTmp2;
    PPENODE pPrev,pPrev2;
    int     left;

    if (pPE != NULL)
    {
        /* find node to delete */
        level = 1;
        pPrev = NULL;
        pTmp = pPE;
        while (pTmp != NULL  &&  key != pTmp->hFile)
        {
            pPrev = pTmp;
            pTmp = (left = (AdjustKey(key) < AdjustKey(pTmp->hFile))) == TRUE ? pTmp->left : pTmp->right;
            level ++;
        }

        if (pTmp != NULL)
        {
            /*found it: pTmp -> node to delete  -  pPrev -> parent node*/
            level--;
            rc = -1;
            if (pTmp->left != NULL && pTmp->right != NULL)
            {
                /* hard case - fetch the leftmost node in the right subtree */
                level2 = level;
                pPrev2 = pTmp;
                pTmp2 = pTmp->right;
                while (pTmp2->left != NULL)
                {
                    pPrev2 = pTmp2;
                    pTmp2 = pTmp2->left;
                    level2++;
                }
                /* pTmp2 -> new root  -  pPrev2 -> parent node */

                /* left child of pTmp2 */
                pTmp2->left = pTmp->left;

                /* parent of pTmp2 and pTmp2->right */
                if (pPrev2 != pTmp)
                {
                    pPrev2->left = pTmp2->right;
                    pTmp2->right = pTmp->right;
                }
                //else pTmp2->right = pTmp2->right;

                /* link in pTmp2 */
                if (pTmp != pPE)
                {
                    if (left)
                        pPrev->left = pTmp2;
                    else
                        pPrev->right = pTmp2;
                }
                else
                    pPE = pTmp2;
                rc = NO_ERROR;
            }

            /* leaf */
            if (rc!=0 && pTmp->left == NULL && pTmp->right == NULL)
            {
                if (pTmp != pPE)
                {
                    if (left)
                        pPrev->left = NULL;
                    else
                        pPrev->right = NULL;
                }
                else
                    pPE = NULL;
                rc = NO_ERROR;
            }

            /* left is NULL */
            if (rc!=0 && pTmp->left == NULL && pTmp->right != NULL)
            {
                /* move up right node */
                if (pTmp != pPE)
                {
                    if (left)
                        pPrev->left = pTmp->right;
                    else
                        pPrev->right = pTmp->right;
                }
                else
                    pPE = pTmp->right;
                rc = NO_ERROR;
            }

            /* right is NULL */
            if (rc!=0 && pTmp->left != NULL && pTmp->right == NULL)
            {
                /* move up left node */
                if (pTmp != pPE)
                {
                    if (left)
                        pPrev->left = pTmp->left;
                    else
                        pPrev->right = pTmp->left;
                }
                else
                    pPE = pTmp->left;
                rc = NO_ERROR;
            }

            /* free node */
            if (rc == NO_ERROR)
                rc = freeNode( pTmp );
        }
        else
            rc  = 1;      //not found
    }
    else
        rc  = 1; //not found
    return rc;
}


/**
 * Get the pointer to a node in the pPE tree.
 * @returns   Pointer to node on success. NULL if not found or error occured.
 * @param     key  Filehandle, which is the key for the pPE tree.
 */
PPENODE getNodePtr(SFN key)
{
    PPENODE pTmp = pPE;
    int level = 1;
    while (pTmp != NULL && pTmp->hFile != key)
    {
        pTmp = AdjustKey(key) < AdjustKey(pTmp->hFile) ? pTmp->left : pTmp->right;
        level++;
    }
    return pTmp;
}


/**
 * Find a PENode by filename in the node tree.
 * @returns   Pointer to PENode if found, NULL if not found.
 * @param     pszFilename  Pointer to filename.
 */
PPENODE findNodePtr(const char *pszFilename)
{
    /*depth first search thru the whole tree */
    return findNodePtr2(pPE, pszFilename);
}


/**
 * Find a PENode by filename in the given tree.
 * Depth first search thru the whole tree.
 * @returns   Pointer to matching PENode.
 * @param     pRoot        Tree root.
 * @param     pszFilename  Pointer to filename.
 * @remark    sub-function of findNodePtr.
 */
static PPENODE findNodePtr2(PPENODE pRoot, const char *pszFilename)
{
    PPENODE pNode = NULL;

    /*depth first search thru the whole tree */
    if ( pRoot == NULL || pRoot->lxfile.queryIsModuleName(pszFilename))
        return pRoot;

    //search subtrees
    if (pRoot->left != NULL)
        pNode = findNodePtr2(pRoot->left,pszFilename);
    if (pNode == NULL && pRoot->right != NULL)
        pNode = findNodePtr2(pRoot->right,pszFilename);
    return pNode;
}


/**
 * Allocate memory for a PENode.
 * @returns   Pointer to new PENode on success. NULL pointer on error.
 */
PPENODE allocateNode(void)
{
    PPENODE pNode;

    pNode = new PENODE;
    if (pNode == NULL)
        kprintf(("allocateNode: malloc returned an NULL-pointer\n"));

    return pNode;
}


/**
 * Frees node.
 * @returns   NO_ERROR on success.
 * @param     pNode  Pointer to node which is to be freed.
 */
static ULONG freeNode(PPENODE pNode)
{
    if (pNode != NULL)
        delete pNode;

    return NO_ERROR;
}


/**
 * Gets the depth of the pPE tree.
 * @returns   Number of levels in the the pPE tree.
 */
ULONG depthPE(void)
{
    return depth(pPE);
}



/**
 * Gets the depth of the pPE tree.
 * @returns   Number of levels in the the pPE tree.
 * @param     pNode  Node to start at. (root node...)
 */
static ULONG depth(PPENODE pNode)
{
    if (pNode != NULL)
    {
        int l, r;
        l = depth(pNode->left);
        r = depth(pNode->right);
        return 1 + (l > r ? l : r);
    }
    else
        return 0;
}


/**
 * Initiate the loader "sub-system".
 * @returns   NO_ERROR on success. !0 on error.
 */
ULONG ldrInit(void)
{
    int rc = NO_ERROR;
    int i;

    /* init state table */
    for (i = 0; i < MAX_FILE_HANDLES/4; i++)
        ahStates[i] = 0;

    /* init uncertain files */
    for (i = 0; i < MAX_UNCERTAIN_FILES; i++)
    {
        ahUncertain[i].hFile = 0xFFFF;
        ahUncertain[i].offsetNEHdr = 0;
        ahUncertain[i].fMZ = 0;
        ahUncertain[i].fPE = 0;
        ahUncertain[i].pszName = NULL;
    }

    /* init pPEFiles* */
    pPE = NULL;

    return rc;
}

