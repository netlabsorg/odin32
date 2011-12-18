/* $Id: avl.c,v 1.5 2000-09-02 21:08:13 bird Exp $
 *
 * AVL-Tree (lookalike) implementation.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/

/*
 * AVL helper macros.
 */
#define AVL_HEIGHTOF(pNode) ((unsigned char)((pNode) != NULL ? pNode->uchHeight : 0))
#define max(a,b) (((a) > (b)) ? (a) : (b))


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "avl.h"
#if defined(RING0) || defined(RING3)
    #include "dev32.h"
#else
    #define SSToDS(a) (a)
#endif
#include "string.h"

#ifdef __GNUC__
#define _Inline static inline
#define __interrupt(n) ({ __asm__ __volatile__ ("int" #n "\n\tnop"); })
#else
#include <builtin.h>
#endif
#define assert(a) ((a) ? (void)0 : __interrupt(3))


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/*
 * A stack used to avoid recursive calls...
 */
typedef struct _AVLStack
{
    unsigned     cEntries;
    PPAVLNODECORE aEntries[AVL_MAX_HEIGHT];
} AVLSTACK, *PAVLSTACK;
typedef struct _AVLStack2
{
    unsigned     cEntries;
    PAVLNODECORE aEntries[AVL_MAX_HEIGHT];
    char         achFlags[AVL_MAX_HEIGHT];
} AVLSTACK2, *PAVLSTACK2;


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
_Inline void AVLRebalance(PAVLSTACK pStack);


/**
 * Inserts a node into the AVL-tree.
 * @returns   TRUE if inserted.
 *            FALSE if node exists in tree.
 * @param     ppTree  Pointer to the AVL-tree root node pointer.
 * @param     pNode   Pointer to the node which is to be added.
 * @sketch    Find the location of the node (using binary three algorithm.):
 *            LOOP until NULL leaf pointer
 *            BEGIN
 *                Add node pointer pointer to the AVL-stack.
 *                IF new-node-key < node key THEN
 *                    left
 *                ELSE
 *                    right
 *            END
 *            Fill in leaf node and insert it.
 *            Rebalance the tree.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BOOL AVLInsert(PPAVLNODECORE ppTree, PAVLNODECORE pNode)
{
    AVLSTACK                AVLStack;
    PPAVLNODECORE           ppCurNode = ppTree;
    register AVLKEY         Key = pNode->Key;
    register PAVLNODECORE   pCurNode;

    AVLStack.cEntries = 0;

    while ((pCurNode = *ppCurNode) != NULL)
    {
        assert(AVLStack.cEntries < AVL_MAX_HEIGHT);
        AVLStack.aEntries[AVLStack.cEntries++] = ppCurNode;
        if (AVL_G(pCurNode->Key, Key))
            ppCurNode = &pCurNode->pLeft;
        else
            ppCurNode = &pCurNode->pRight;
    }

#ifdef AVL_MAY_TRY_INSERT_EQUAL
    /* check if equal */
    if (AVLStack.cEntries > 0 && AVL_E((*AVLStack.aEntries[AVLStack.cEntries-1])->Key, pNode->Key))
        return FALSE;
#endif

    pNode->pLeft = pNode->pRight = NULL;
    pNode->uchHeight = 1;
    *ppCurNode = pNode;

    AVLRebalance(SSToDS(&AVLStack));
    return TRUE;
}


/**
 * Removes a node from the AVL-tree.
 * @returns   Pointer to the node.
 * @param     ppTree  Pointer to the AVL-tree root node pointer.
 * @param     Key     Key value of the node which is to be removed.
 * @sketch    Find the node which is to be removed:
 *            LOOP until not found
 *            BEGIN
 *                Add node pointer pointer to the AVL-stack.
 *                IF the keys matches THEN break!
 *                IF remove key < node key THEN
 *                    left
 *                ELSE
 *                    right
 *            END
 *            IF found THEN
 *            BEGIN
 *                IF left node not empty THEN
 *                BEGIN
 *                    Find the right most node in the left tree while adding the pointer to the pointer to it's parent to the stack:
 *                    Start at left node.
 *                    LOOP until right node is empty
 *                    BEGIN
 *                        Add to stack.
 *                        go right.
 *                    END
 *                    Link out the found node.
 *                    Replace the node which is to be removed with the found node.
 *                    Correct the stack entry for the pointer to the left tree.
 *                END
 *                ELSE
 *                BEGIN
 *                    Move up right node.
 *                    Remove last stack entry.
 *                END
 *                Balance tree using stack.
 *            END
 *            return pointer to the removed node (if found).
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PAVLNODECORE AVLRemove(PPAVLNODECORE ppTree, AVLKEY Key)
{
    AVLSTACK                AVLStack;
    PPAVLNODECORE           ppDeleteNode = ppTree;
    register PAVLNODECORE   pDeleteNode;

    AVLStack.cEntries = 0;

    while ((pDeleteNode = *ppDeleteNode) != NULL)
    {
        assert(AVLStack.cEntries < AVL_MAX_HEIGHT);
        AVLStack.aEntries[AVLStack.cEntries++] = ppDeleteNode;
        if (AVL_E(pDeleteNode->Key, Key))
            break;

        if (AVL_G(pDeleteNode->Key, Key))
            ppDeleteNode = &pDeleteNode->pLeft;
        else
            ppDeleteNode = &pDeleteNode->pRight;
    }

    if (pDeleteNode != NULL)
    {
        if (pDeleteNode->pLeft != NULL)
        {
            unsigned                iStackEntry = AVLStack.cEntries;
            PPAVLNODECORE           ppLeftLeast = &pDeleteNode->pLeft;
            register PAVLNODECORE   pLeftLeast;

            while ((pLeftLeast = *ppLeftLeast)->pRight != NULL) /* Left most node. */
            {
                assert(AVLStack.cEntries < AVL_MAX_HEIGHT);
                AVLStack.aEntries[AVLStack.cEntries++] = ppLeftLeast;
                ppLeftLeast = &pLeftLeast->pRight;
                pLeftLeast  = pLeftLeast->pRight;
            }

            /* link out pLeftLeast */
            *ppLeftLeast = pLeftLeast->pLeft;

            /* link in place of the delete node. */
            pLeftLeast->pLeft = pDeleteNode->pLeft;
            pLeftLeast->pRight = pDeleteNode->pRight;
            pLeftLeast->uchHeight = pDeleteNode->uchHeight;
            *ppDeleteNode = pLeftLeast;
            AVLStack.aEntries[iStackEntry] = &pLeftLeast->pLeft;
        }
        else
        {
            *ppDeleteNode = pDeleteNode->pRight;
            AVLStack.cEntries--;
        }

        AVLRebalance(SSToDS(&AVLStack));
    }

    return pDeleteNode;
}


/**
 * Gets a node from the tree (does not remove it!)
 * @returns   Pointer to the node holding the given key.
 * @param     ppTree  Pointer to the AVL-tree root node pointer.
 * @param     Key     Key value of the node which is to be found.
 * @sketch
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PAVLNODECORE AVLGet(PPAVLNODECORE ppTree, AVLKEY Key)
{
    register PAVLNODECORE  pNode = *ppTree;

    while (pNode != NULL && AVL_NE(pNode->Key, Key))
    {
        if (AVL_G(pNode->Key, Key))
            pNode = pNode->pLeft;
        else
            pNode = pNode->pRight;
    }

    return pNode;
}



/**
 * Gets a node from the tree and its parent node (if any) (does not remove any nodes!)
 * @returns   Pointer to the node holding the given key.
 * @param     ppTree    Pointer to the AVL-tree root node pointer.
 * @param     ppParent  Pointer to a variable which will hold the pointer to the partent node on
 *                      return. When no node is found, this will hold the last searched node.
 * @param     Key       Key value of the node which is to be found.
 * @sketch
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PAVLNODECORE    AVLGetWithParent(PPAVLNODECORE ppTree, PPAVLNODECORE ppParent, AVLKEY Key)
{
    register PAVLNODECORE  pNode = *ppTree;
    register PAVLNODECORE  pParent = NULL;

    while (pNode != NULL && AVL_NE(pNode->Key, Key))
    {
        pParent = pNode;
        if (AVL_G(pNode->Key, Key))
            pNode = pNode->pLeft;
        else
            pNode = pNode->pRight;
    }

    *ppParent = pParent;
    return pNode;
}



/**
 * Gets node from the tree (does not remove it!) and it's adjecent (by key value) nodes.
 * @returns   Pointer to the node holding the given key.
 * @param     ppTree   Pointer to the AVL-tree root node pointer.
 * @param     Key      Key value of the node which is to be found.
 * @param     ppLeft   Pointer to left node pointer.
 * @param     ppRight  Pointer to right node pointer.
 * @sketch    Find node with the given key, record search path on the stack.
 *            IF found THEN
 *            BEGIN
 *                Find the right-most node in the left subtree.
 *                Find the left-most node in the right subtree.
 *                Rewind the stack while searching for more adjecent nodes.
 *            END
 *            return node with adjecent nodes.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PAVLNODECORE AVLGetWithAdjecentNodes(PPAVLNODECORE ppTree, AVLKEY Key, PPAVLNODECORE ppLeft, PPAVLNODECORE ppRight)
{
    AVLSTACK        AVLStack;
    PPAVLNODECORE   ppNode = ppTree;
    PAVLNODECORE    pNode;

    AVLStack.cEntries = 0;

    while ((pNode = *ppNode) != NULL && AVL_NE(pNode->Key, Key))
    {
        assert(AVLStack.cEntries < AVL_MAX_HEIGHT);
        AVLStack.aEntries[AVLStack.cEntries++] = ppNode;
        if (AVL_G(pNode->Key, Key))
            ppNode = &pNode->pLeft;
        else
            ppNode = &pNode->pRight;
    }

    if (pNode != NULL)
    {
        PAVLNODECORE    pCurNode;

        /* find rigth-most node in left subtree. */
        pCurNode = pNode->pLeft;
        if (pCurNode != NULL)
            while (pCurNode->pRight != NULL)
                pCurNode = pCurNode->pRight;
        *ppLeft = pCurNode;

        /* find left-most node in right subtree. */
        pCurNode = pNode->pRight;
        if (pCurNode != NULL)
            while (pCurNode->pLeft != NULL)
                pCurNode = pCurNode->pLeft;
        *ppRight = pCurNode;

        /* rewind stack */
        while (AVLStack.cEntries-- > 0)
        {
            pCurNode = *AVLStack.aEntries[AVLStack.cEntries];
            if (AVL_L(pCurNode->Key, Key) && (*ppLeft == NULL || AVL_G(pCurNode->Key, (*ppLeft)->Key)))
                *ppLeft = pCurNode;
            else if (AVL_G(pCurNode->Key, Key) && (*ppRight == NULL || AVL_L(pCurNode->Key, (*ppRight)->Key)))
                *ppRight = pCurNode;
        }
    }
    else
        *ppLeft = *ppRight = NULL;

    return pNode;
}


/**
 * Iterates tru all nodes in the given tree.
 * @returns   0 on success. Return from callback on failiure.
 * @param     ppTree   Pointer to the AVL-tree root node pointer.
 * @param     fFromLeft    TRUE:  Left to right.
 *                         FALSE: Right to left.
 * @param     pfnCallBack  Pointer to callback function.
 * @param     pvParam      Userparameter passed on to the callback function.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
unsigned AVLDoWithAll(PPAVLNODECORE ppTree, int fFromLeft, PAVLCALLBACK pfnCallBack, void *pvParam)
{
    AVLSTACK2       AVLStack;
    PAVLNODECORE    pNode;
    unsigned        rc;

    if (*ppTree == NULL)
        return 0;

    AVLStack.cEntries = 1;
    AVLStack.achFlags[0] = 0;
    AVLStack.aEntries[0] = *ppTree;

    if (fFromLeft)
    {   /* from left */
        while (AVLStack.cEntries > 0)
        {
            pNode = AVLStack.aEntries[AVLStack.cEntries - 1];

            /* left */
            if (!AVLStack.achFlags[AVLStack.cEntries - 1]++)
            {
                if (pNode->pLeft != NULL)
                {
                    AVLStack.achFlags[AVLStack.cEntries] = 0; /* 0 first, 1 last */
                    AVLStack.aEntries[AVLStack.cEntries++] = pNode->pLeft;
                    continue;
                }
            }

            /* center */
            rc = pfnCallBack(pNode, pvParam);
            if (rc != 0)
                return rc;

            /* right */
            AVLStack.cEntries--;
            if (pNode->pRight != NULL)
            {
                AVLStack.achFlags[AVLStack.cEntries] = 0;
                AVLStack.aEntries[AVLStack.cEntries++] = pNode->pRight;
            }
        } /* while */
    }
    else
    {   /* from right */
        while (AVLStack.cEntries > 0)
        {
            pNode = AVLStack.aEntries[AVLStack.cEntries - 1];


            /* right */
            if (!AVLStack.achFlags[AVLStack.cEntries - 1]++)
            {
                if (pNode->pRight != NULL)
                {
                    AVLStack.achFlags[AVLStack.cEntries] = 0;  /* 0 first, 1 last */
                    AVLStack.aEntries[AVLStack.cEntries++] = pNode->pRight;
                    continue;
                }
            }

            /* center */
            rc = pfnCallBack(pNode, pvParam);
            if (rc != 0)
                return rc;

            /* left */
            AVLStack.cEntries--;
            if (pNode->pLeft != NULL)
            {
                AVLStack.achFlags[AVLStack.cEntries] = 0;
                AVLStack.aEntries[AVLStack.cEntries++] = pNode->pLeft;
            }
        } /* while */
    }

    return 0;
}


/**
 * Starts an enumeration of all nodes in the given AVL tree.
 * @returns   Pointer to the first node in the tree.
 * @param     ppTree     Pointer to the AVL-tree root node pointer.
 * @param     pEnumData  Pointer to enumeration control data.
 * @param     fFromLeft  TRUE:  Left to right.
 *                       FALSE: Right to left.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PAVLNODECORE AVLBeginEnumTree(PPAVLNODECORE ppTree, PAVLENUMDATA pEnumData, int fFromLeft)
{
    if (*ppTree != NULL)
    {
        pEnumData->fFromLeft = (char)fFromLeft;
        pEnumData->cEntries = 1;
        pEnumData->aEntries[0] = *ppTree;
        pEnumData->achFlags[0] = 0;
    }
    else
        pEnumData->cEntries = 0;

    return AVLGetNextNode(pEnumData);
}


/**
 * Get the next node in the tree enumeration.
 * @returns   Pointer to the first node in the tree.
 * @param     pEnumData  Pointer to enumeration control data.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
PAVLNODECORE AVLGetNextNode(PAVLENUMDATA pEnumData)
{
    PAVLNODECORE    pNode;

    if (pEnumData->fFromLeft)
    {   /* from left */
        while (pEnumData->cEntries > 0)
        {
            pNode = pEnumData->aEntries[pEnumData->cEntries - 1];

            /* left */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 0)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                if (pNode->pLeft != NULL)
                {
                    pEnumData->achFlags[pEnumData->cEntries] = 0; /* 0 left, 1 center, 2 right */
                    pEnumData->aEntries[pEnumData->cEntries++] = pNode->pLeft;
                    continue;
                }
            }

            /* center */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 1)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                return pNode;
            }

            /* right */
            pEnumData->cEntries--;
            if (pNode->pRight != NULL)
            {
                pEnumData->achFlags[pEnumData->cEntries] = 0;
                pEnumData->aEntries[pEnumData->cEntries++] = pNode->pRight;
            }
        } /* while */
    }
    else
    {   /* from right */
        while (pEnumData->cEntries > 0)
        {
            pNode = pEnumData->aEntries[pEnumData->cEntries - 1];


            /* right */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 0)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                if (pNode->pRight != NULL)
                {
                    pEnumData->achFlags[pEnumData->cEntries] = 0;  /* 0 right, 1 center, 2 left */
                    pEnumData->aEntries[pEnumData->cEntries++] = pNode->pRight;
                    continue;
                }
            }

            /* center */
            if (pEnumData->achFlags[pEnumData->cEntries - 1] == 1)
            {
                pEnumData->achFlags[pEnumData->cEntries - 1]++;
                return pNode;
            }

            /* left */
            pEnumData->cEntries--;
            if (pNode->pLeft != NULL)
            {
                pEnumData->achFlags[pEnumData->cEntries] = 0;
                pEnumData->aEntries[pEnumData->cEntries++] = pNode->pLeft;
            }
        } /* while */
    }

    return NULL;

}




/**
 * Finds the best fitting node in the tree for the given Key value.
 * @returns   Pointer to the best fitting node found.
 * @param     ppTree  Pointer to Pointer to the tree root node.
 * @param     Key     The Key of which is to be found a best fitting match for..
 * @param     fAbove  TRUE:  Returned node is have the closest key to Key from above.
 *                    FALSE: Returned node is have the closest key to Key from below.
 * @status    completely implemented.
 * @sketch    The best fitting node is always located in the searchpath above you.
 *            >= (above): The node where you last turned left.
 *            <= (below): the node where you last turned right.
 * @author    knut st. osmundsen
 */
PAVLNODECORE    AVLGetBestFit(PPAVLNODECORE ppTree, AVLKEY Key, int fAbove)
{
    register PAVLNODECORE   pNode = *ppTree;
    PAVLNODECORE            pNodeLast = NULL;

    if (fAbove)
    {   /* pNode->Key >= Key */
        while (pNode != NULL && AVL_NE(pNode->Key, Key))
        {
            if (AVL_G(pNode->Key, Key))
            {
                pNodeLast = pNode;
                pNode = pNode->pLeft;
            }
            else
                pNode = pNode->pRight;
        }
    }
    else
    {   /* pNode->Key <= Key */
        while (pNode != NULL && AVL_NE(pNode->Key, Key))
        {
            if (AVL_L(pNode->Key, Key))
            {
                pNodeLast = pNode;
                pNode = pNode->pRight;
            }
            else
                pNode = pNode->pLeft;
        }
    }

    return pNode == NULL ? pNodeLast /* best fit */ : pNode /* perfect match */;
}


/**
 * Rewindes a stack of pointer to pointers to nodes, rebalancing the tree.
 * @param     pStack  Pointer to stack to rewind.
 * @sketch    LOOP thru all stack entries
 *            BEGIN
 *                Get pointer to pointer to node (and pointer to node) from stack.
 *                IF 2 higher left subtree than in right subtree THEN
 *                BEGIN
 *                    IF higher (or equal) left-sub-subtree than right-sub-subtree THEN
 *                                *                       n+2|n+3
 *                              /   \                     /     \
 *                            n+2    n       ==>         n+1   n+1|n+2
 *                           /   \                             /     \
 *                         n+1 n|n+1                          n|n+1  n
 *
 *                         Or with keys:
 *
 *                               4                           2
 *                             /   \                       /   \
 *                            2     5        ==>          1     4
 *                           / \                               / \
 *                          1   3                             3   5
 *
 *                    ELSE
 *                                *                         n+2
 *                              /   \                      /   \
 *                            n+2    n                   n+1   n+1
 *                           /   \           ==>        /  \   /  \
 *                          n    n+1                    n  L   R   n
 *                               / \
 *                              L   R
 *
 *                         Or with keys:
 *                               6                           4
 *                             /   \                       /   \
 *                            2     7        ==>          2     6
 *                          /   \                       /  \  /  \
 *                          1    4                      1  3  5  7
 *                              / \
 *                             3   5
 *                END
 *                ELSE IF 2 higher in right subtree than in left subtree THEN
 *                BEGIN
 *                    Same as above but left <==> right. (invert the picture)
 *                ELSE
 *                    IF correct height THEN break
 *                    ELSE correct height.
 *            END
 * @status
 * @author    knut st. osmundsen
 * @remark
 */
_Inline void AVLRebalance(PAVLSTACK pStack)
{
    while (pStack->cEntries > 0)
    {
        PPAVLNODECORE   ppNode = pStack->aEntries[--pStack->cEntries];
        PAVLNODECORE    pNode = *ppNode;
        PAVLNODECORE    pLeftNode = pNode->pLeft;
        unsigned char   uchLeftHeight = AVL_HEIGHTOF(pLeftNode);
        PAVLNODECORE    pRightNode = pNode->pRight;
        unsigned char   uchRightHeight = AVL_HEIGHTOF(pRightNode);

        if (uchRightHeight + 1 < uchLeftHeight)
        {
            PAVLNODECORE    pLeftLeftNode = pLeftNode->pLeft;
            PAVLNODECORE    pLeftRightNode = pLeftNode->pRight;
            unsigned char   uchLeftRightHeight = AVL_HEIGHTOF(pLeftRightNode);

            if (AVL_HEIGHTOF(pLeftLeftNode) >= uchLeftRightHeight)
            {
                pNode->pLeft = pLeftRightNode;
                pLeftNode->pRight = pNode;
                pLeftNode->uchHeight = (unsigned char)(1 + (pNode->uchHeight = (unsigned char)(1 + uchLeftRightHeight)));
                *ppNode = pLeftNode;
            }
            else
            {
                pLeftNode->pRight = pLeftRightNode->pLeft;
                pNode->pLeft = pLeftRightNode->pRight;
                pLeftRightNode->pLeft = pLeftNode;
                pLeftRightNode->pRight = pNode;
                pLeftNode->uchHeight = pNode->uchHeight = uchLeftRightHeight;
                pLeftRightNode->uchHeight = uchLeftHeight;
                *ppNode = pLeftRightNode;
            }
        }
        else if (uchLeftHeight + 1 < uchRightHeight)
        {
            PAVLNODECORE    pRightLeftNode = pRightNode->pLeft;
            unsigned char   uchRightLeftHeight = AVL_HEIGHTOF(pRightLeftNode);
            PAVLNODECORE    pRightRightNode = pRightNode->pRight;

            if (AVL_HEIGHTOF(pRightRightNode) >= uchRightLeftHeight)
            {
                pNode->pRight = pRightLeftNode;
                pRightNode->pLeft = pNode;
                pRightNode->uchHeight = (unsigned char)(1 + (pNode->uchHeight = (unsigned char)(1 + uchRightLeftHeight)));
                *ppNode = pRightNode;
            }
            else
            {
                pRightNode->pLeft = pRightLeftNode->pRight;
                pNode->pRight = pRightLeftNode->pLeft;
                pRightLeftNode->pRight = pRightNode;
                pRightLeftNode->pLeft = pNode;
                pRightNode->uchHeight = pNode->uchHeight = uchRightLeftHeight;
                pRightLeftNode->uchHeight = uchRightHeight;
                *ppNode = pRightLeftNode;
            }
        }
        else
        {
            register unsigned char uchHeight = (unsigned char)(max(uchLeftHeight, uchRightHeight) + 1);
            if (uchHeight == pNode->uchHeight)
                break;
            pNode->uchHeight = uchHeight;
        }
    }
}

