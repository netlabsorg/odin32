/* $Id: avl.c,v 1.1 1999-10-27 02:02:59 bird Exp $
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
 * AVL configuration.
 */
#define AVL_MAX_HEIGHT      19          /* Up to 2^16 nodes. */
#define assert

/*
 * AVL helper macros.
 */
#define AVL_HEIGHTOF(pNode) ((pNode) != NULL ? pNode->uchHeight : 0UL)
#define max(a,b) (((a) > (b)) ? (a) : (b))


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "avl.h"
#include "dev32.h"


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


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
_Inline void AVLRebalance(PAVLSTACK pStack);


/**
 * Inserts a node into the AVL-tree.
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
void AVLInsert(PPAVLNODECORE ppTree, PAVLNODECORE pNode)
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
        if (pCurNode->Key > Key)
            ppCurNode = &pCurNode->pLeft;
        else
            ppCurNode = &pCurNode->pRight;
    }

    pNode->pLeft = pNode->pRight = NULL;
    pNode->uchHeight = 1;
    *ppCurNode = pNode;

    AVLRebalance(SSToDS(&AVLStack));
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
        if (pDeleteNode->Key == Key)
            break;

        if (pDeleteNode->Key > Key)
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
 * Gets node from the tree (does not remove it!)
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

    while (pNode != NULL && pNode->Key != Key)
    {
        if (pNode->Key > Key)
            pNode = pNode->pLeft;
        else
            pNode = pNode->pRight;
    }

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

    while ((pNode = *ppNode) != NULL && pNode->Key != Key)
    {
        assert(AVLStack.cEntries < AVL_MAX_HEIGHT);
        AVLStack.aEntries[AVLStack.cEntries++] = ppNode;
        if (pNode->Key > Key)
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
            if (pCurNode->Key < Key && (*ppLeft == NULL || pCurNode->Key > (*ppLeft)->Key))
                *ppLeft = pCurNode;
            else if (pCurNode->Key > Key && (*ppRight == NULL || pCurNode->Key < (*ppRight)->Key))
                *ppRight = pCurNode;
        }
    }
    else
        *ppLeft = *ppRight = NULL;

    return pNode;
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
        unsigned        uLeftHeight = AVL_HEIGHTOF(pLeftNode);
        PAVLNODECORE    pRightNode = pNode->pRight;
        unsigned        uRightHeight = AVL_HEIGHTOF(pRightNode);

        if (uRightHeight + 1 < uLeftHeight)
        {
            PAVLNODECORE    pLeftLeftNode = pLeftNode->pLeft;
            PAVLNODECORE    pLeftRightNode = pLeftNode->pRight;
            unsigned        uLeftRightHeight = AVL_HEIGHTOF(pLeftRightNode);

            if (AVL_HEIGHTOF(pLeftLeftNode) >= uLeftRightHeight)
            {
                pNode->pLeft = pLeftRightNode;
                pLeftNode->pRight = pNode;
                pLeftNode->uchHeight = 1 + (pNode->uchHeight = 1 + uLeftRightHeight);
                *ppNode = pLeftNode;
            }
            else
            {
                pLeftNode->pRight = pLeftRightNode->pLeft;
                pNode->pLeft = pLeftRightNode->pRight;
                pLeftRightNode->pLeft = pLeftNode;
                pLeftRightNode->pRight = pNode;
                pLeftNode->uchHeight = pNode->uchHeight = uLeftRightHeight;
                pLeftRightNode->uchHeight = uLeftHeight;
                *ppNode = pLeftRightNode;
            }
        }
        else if (uLeftHeight + 1 < uRightHeight)
        {
            PAVLNODECORE    pRightLeftNode = pRightNode->pLeft;
            unsigned        uRightLeftHeight = AVL_HEIGHTOF(pRightLeftNode);
            PAVLNODECORE    pRightRightNode = pRightNode->pRight;

            if (AVL_HEIGHTOF(pRightRightNode) >= uRightLeftHeight)
            {
                pNode->pRight = pRightLeftNode;
                pRightNode->pLeft = pNode;
                pRightNode->uchHeight = 1 + (pNode->uchHeight = 1 + uRightLeftHeight);
                *ppNode = pRightNode;
            }
            else
            {
                pRightNode->pLeft = pRightLeftNode->pRight;
                pNode->pRight = pRightLeftNode->pLeft;
                pRightLeftNode->pRight = pRightNode;
                pRightLeftNode->pLeft = pNode;
                pRightNode->uchHeight = pNode->uchHeight = uRightLeftHeight;
                pRightLeftNode->uchHeight = uRightHeight;
                *ppNode = pRightLeftNode;
            }
        }
        else
        {
            register unsigned uHeight = max(uLeftHeight, uRightHeight) + 1;
            if (uHeight == pNode->uchHeight)
                break;
            pNode->uchHeight = uHeight;
        }
    }
}

