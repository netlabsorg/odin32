/* $Id: avl.h,v 1.1 1999-10-27 02:02:55 bird Exp $
 *
 * AVL-Tree (lookalike) declaration.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _AVL_H_
#define _AVL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * AVL key type
 */
typedef unsigned long AVLKEY;

/**
 * AVL Core node.
 */
typedef struct _AVLNodeCore
{
    unsigned char           uchHeight; /* Height of this tree: max(heigth(left), heigth(right)) + 1 */
    struct  _AVLNodeCore *  pLeft;     /* Pointer to left leaf node. */
    struct  _AVLNodeCore *  pRight;    /* Pointer to right leaf node. */
    AVLKEY                  Key;       /* Key value. */
} AVLNODECORE, *PAVLNODECORE, **PPAVLNODECORE;


void AVLInsert(PPAVLNODECORE ppTree, PAVLNODECORE pNode);
PAVLNODECORE AVLRemove(PPAVLNODECORE ppTree, AVLKEY Key);
PAVLNODECORE AVLGet(PPAVLNODECORE ppTree, AVLKEY Key);
PAVLNODECORE AVLGetWithAdjecentNodes(PPAVLNODECORE ppTree, AVLKEY Key, PPAVLNODECORE ppLeft, PPAVLNODECORE ppRight);


/*
 * Just in case NULL is undefined.
 */
#ifndef NULL
    #define NULL ((void*)0)
#endif

#ifdef __cplusplus
}
#endif

#endif
