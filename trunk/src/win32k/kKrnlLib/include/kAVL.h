/* $Id: kAVL.h,v 1.3 2001-09-27 03:05:58 bird Exp $
 *
 * kAVL - header file for all the AVL trees.
 *
 * Copyright (c) 1999-2001 knut st. osmundsen (kosmunds@csc.com)
 *
 */
#ifndef _kAVL_h_
#define _kAVL_h_


/*******************************************************************************
*   AVL tree of unique case-sensitive strings.                                 *
*******************************************************************************/
#define KAVLSTR_MAX_HEIGHT  26
/**
 * AVL key type
 */
typedef const char *AVLSTRKEY;

/**
 * AVL Core node.
 */
typedef struct _AVLStrNodeCore
{
    AVLSTRKEY                Key;       /* Key value. */
    struct  _AVLStrNodeCore *pLeft;     /* Pointer to left leaf node. */
    struct  _AVLStrNodeCore *pRight;    /* Pointer to right leaf node. */
    unsigned char           uchHeight;  /* Height of this tree: max(heigth(left), heigth(right)) + 1 */
} AVLSTRNODECORE, *PAVLSTRNODECORE, **PPAVLSTRNODECORE;


/**
 * AVL Enum data - All members are PRIVATE! Don't touch!
 */
typedef struct _AVLStrEnumData
{
    char            fFromLeft;
    char            cEntries;
    char            achFlags[KAVLSTR_MAX_HEIGHT];
    PAVLSTRNODECORE aEntries[KAVLSTR_MAX_HEIGHT];
} AVLSTRENUMDATA, *PAVLSTRENUMDATA;

/*
 * callback type
 */
typedef unsigned ( _PAVLSTRCALLBACK)(PAVLSTRNODECORE, void*);
typedef _PAVLSTRCALLBACK *PAVLSTRCALLBACK;


/*
 * Functions.
 */
PAVLSTRNODECORE     KLIBCALL AVLStrRemove(PPAVLSTRNODECORE ppTree, AVLSTRKEY Key);
KBOOL               KLIBCALL AVLStrInsert(PPAVLSTRNODECORE ppTree, PAVLSTRNODECORE pNode);
PAVLSTRNODECORE     KLIBCALL AVLStrGet(PPAVLSTRNODECORE ppTree, AVLSTRKEY Key);
PAVLSTRNODECORE     KLIBCALL AVLStrBeginEnumTree(PPAVLSTRNODECORE ppTree, PAVLSTRENUMDATA pEnumData, int fFromLeft);
PAVLSTRNODECORE     KLIBCALL AVLStrGetNextNode(PAVLSTRENUMDATA pEnumData);
unsigned            KLIBCALL AVLStrDoWithAll(PPAVLSTRNODECORE ppTree, int fFromLeft, PAVLSTRCALLBACK pfnCallBack, void *pvParam);



/*******************************************************************************
*   AVL tree of unique case-insensitive strings.                               *
*******************************************************************************/
#define KAVLSTRI_MAX_HEIGHT  26
/**
 * AVL key type
 */
typedef const char *AVLSTRIKEY;

/**
 * AVL Core node.
 */
typedef struct _AVLStrINodeCore
{
    AVLSTRIKEY               Key;       /* Key value. */
    struct  _AVLStrINodeCore *pLeft;    /* Pointer to left leaf node. */
    struct  _AVLStrINodeCore *pRight;   /* Pointer to right leaf node. */
    unsigned char           uchHeight;  /* Height of this tree: max(heigth(left), heigth(right)) + 1 */
} AVLSTRINODECORE, *PAVLSTRINODECORE, **PPAVLSTRINODECORE;


/**
 * AVL Enum data - All members are PRIVATE! Don't touch!
 */
typedef struct _AVLStrIEnumData
{
    char                fFromLeft;
    char                cEntries;
    char                achFlags[KAVLSTRI_MAX_HEIGHT];
    PAVLSTRINODECORE    aEntries[KAVLSTRI_MAX_HEIGHT];
} AVLSTRIENUMDATA, *PAVLSTRIENUMDATA;

/*
 * callback type
 */
typedef unsigned ( _PAVLSTRICALLBACK)(PAVLSTRINODECORE, void*);
typedef _PAVLSTRICALLBACK *PAVLSTRICALLBACK;


/*
 * Functions.
 */
PAVLSTRINODECORE    KLIBCALL AVLStrIRemove(PPAVLSTRINODECORE ppTree, AVLSTRIKEY Key);
KBOOL               KLIBCALL AVLStrIInsert(PPAVLSTRINODECORE ppTree, PAVLSTRINODECORE pNode);
PAVLSTRINODECORE    KLIBCALL AVLStrIGet(PPAVLSTRINODECORE ppTree, AVLSTRIKEY Key);
PAVLSTRINODECORE    KLIBCALL AVLStrIBeginEnumTree(PPAVLSTRINODECORE ppTree, PAVLSTRIENUMDATA pEnumData, int fFromLeft);
PAVLSTRINODECORE    KLIBCALL AVLStrIGetNextNode(PAVLSTRIENUMDATA pEnumData);
unsigned            KLIBCALL AVLStrIDoWithAll(PPAVLSTRINODECORE ppTree, int fFromLeft, PAVLSTRICALLBACK pfnCallBack, void *pvParam);



/*******************************************************************************
*   AVL tree of void pointers.                                                 *
*******************************************************************************/
#define KAVLPV_MAX_HEIGHT  26
/**
 * AVL key type
 */
typedef void *AVLPVKEY;

/**
 * AVL Core node.
 */
typedef struct _AVLPVNodeCore
{
    AVLPVKEY                Key;        /* Key value. */
    struct  _AVLPVNodeCore *pLeft;      /* Pointer to left leaf node. */
    struct  _AVLPVNodeCore *pRight;     /* Pointer to right leaf node. */
    unsigned char           uchHeight;  /* Height of this tree: max(heigth(left), heigth(right)) + 1 */
} AVLPVNODECORE, *PAVLPVNODECORE, **PPAVLPVNODECORE;


/**
 * AVL Enum data - All members are PRIVATE! Don't touch!
 */
typedef struct _AVLPVEnumData
{
    char                fFromLeft;
    char                cEntries;
    char                achFlags[KAVLPV_MAX_HEIGHT];
    PAVLPVNODECORE      aEntries[KAVLPV_MAX_HEIGHT];
} AVLPVENUMDATA, *PAVLPVENUMDATA;

/*
 * callback type
 */
typedef unsigned ( _PAVLPVCALLBACK)(PAVLPVNODECORE, void*);
typedef _PAVLPVCALLBACK *PAVLPVCALLBACK;


/*
 * Functions.
 */
PAVLPVNODECORE      KLIBCALL AVLPVRemove(PPAVLPVNODECORE ppTree, AVLPVKEY Key);
KBOOL               KLIBCALL AVLPVInsert(PPAVLPVNODECORE ppTree, PAVLPVNODECORE pNode);
PAVLPVNODECORE      KLIBCALL AVLPVGet(PPAVLPVNODECORE ppTree, AVLPVKEY Key);
PAVLPVNODECORE      KLIBCALL AVLPVGetWithParent(PPAVLPVNODECORE ppTree, PPAVLPVNODECORE ppParent, AVLPVKEY Key);
PAVLPVNODECORE      KLIBCALL AVLPVGetWithAdjacentNodes(PPAVLPVNODECORE ppTree, AVLPVKEY Key, PPAVLPVNODECORE ppLeft, PPAVLPVNODECORE ppRight);
PAVLPVNODECORE      KLIBCALL AVLPVGetBestFit(PPAVLPVNODECORE ppTree, AVLPVKEY Key, KBOOL fAbove);
PAVLPVNODECORE      KLIBCALL AVLPVRemoveBestFit(PPAVLPVNODECORE ppTree, AVLPVKEY Key, KBOOL fAbove);
PAVLPVNODECORE      KLIBCALL AVLPVBeginEnumTree(PPAVLPVNODECORE ppTree, PAVLPVENUMDATA pEnumData, int fFromLeft);
PAVLPVNODECORE      KLIBCALL AVLPVGetNextNode(PAVLPVENUMDATA pEnumData);
unsigned            KLIBCALL AVLPVDoWithAll(PPAVLPVNODECORE ppTree, int fFromLeft, PAVLPVCALLBACK pfnCallBack, void *pvParam);


/*******************************************************************************
*   AVL tree of unsigned long.                                                 *
*******************************************************************************/
#define KAVLUL_MAX_HEIGHT  26
/**
 * AVL key type
 */
typedef unsigned long AVLULKEY;

/**
 * AVL Core node.
 */
typedef struct _AVLULNodeCore
{
    AVLULKEY                Key;        /* Key value. */
    struct  _AVLULNodeCore *pLeft;      /* Pointer to left leaf node. */
    struct  _AVLULNodeCore *pRight;     /* Pointer to right leaf node. */
    unsigned char           uchHeight;  /* Height of this tree: max(heigth(left), heigth(right)) + 1 */
} AVLULNODECORE, *PAVLULNODECORE, **PPAVLULNODECORE;


/**
 * AVL Enum data - All members are PRIVATE! Don't touch!
 */
typedef struct _AVLULEnumData
{
    char                fFromLeft;
    char                cEntries;
    char                achFlags[KAVLUL_MAX_HEIGHT];
    PAVLULNODECORE      aEntries[KAVLUL_MAX_HEIGHT];
} AVLULENUMDATA, *PAVLULENUMDATA;

/*
 * callback type
 */
typedef unsigned ( _PAVLULCALLBACK)(PAVLULNODECORE, void*);
typedef _PAVLULCALLBACK *PAVLULCALLBACK;


/*
 * Functions.
 */
PAVLULNODECORE      KLIBCALL AVLULRemove(PPAVLULNODECORE ppTree, AVLULKEY Key);
KBOOL               KLIBCALL AVLULInsert(PPAVLULNODECORE ppTree, PAVLULNODECORE pNode);
PAVLULNODECORE      KLIBCALL AVLULGet(PPAVLULNODECORE ppTree, AVLULKEY Key);
PAVLULNODECORE      KLIBCALL AVLULGetWithParent(PPAVLULNODECORE ppTree, PPAVLULNODECORE ppParent, AVLULKEY Key);
PAVLULNODECORE      KLIBCALL AVLULGetWithAdjacentNodes(PPAVLULNODECORE ppTree, AVLULKEY Key, PPAVLULNODECORE ppLeft, PPAVLULNODECORE ppRight);
PAVLULNODECORE      KLIBCALL AVLULGetBestFit(PPAVLULNODECORE ppTree, AVLULKEY Key, KBOOL fAbove);
PAVLULNODECORE      KLIBCALL AVLULRemoveBestFit(PPAVLULNODECORE ppTree, AVLULKEY Key, KBOOL fAbove);
PAVLULNODECORE      KLIBCALL AVLULBeginEnumTree(PPAVLULNODECORE ppTree, PAVLULENUMDATA pEnumData, int fFromLeft);
PAVLULNODECORE      KLIBCALL AVLULGetNextNode(PAVLULENUMDATA pEnumData);
unsigned            KLIBCALL AVLULDoWithAll(PPAVLULNODECORE ppTree, int fFromLeft, PAVLULCALLBACK pfnCallBack, void *pvParam);

/* since ULONG and PVOID is of the same size don't duplicated the code! */
#if defined(__IBMC__) || defined(__IBMCPP__)
    #pragma map(AVLULRemove               , "AVLPVRemove")
    #pragma map(AVLULInsert               , "AVLPVInsert")
    #pragma map(AVLULGet                  , "AVLPVGet")
    #pragma map(AVLULGetWithParent        , "AVLPVGetWithParent")
    #pragma map(AVLULGetWithAdjacentNodes , "AVLPVGetWithAdjacentNodes")
    #pragma map(AVLULGetBestFit           , "AVLPVGetBestFit")
    #pragma map(AVLULRemoveBestFit        , "AVLPVRemoveBestFit")
    #pragma map(AVLULBeginEnumTree        , "AVLPVBeginEnumTree")
    #pragma map(AVLULGetNextNode          , "AVLPVGetNextNode")
    #pragma map(AVLULDoWithAll            , "AVLPVDoWithAll")
#else
    #error "Mapping not supported/implemented for this compiler."
#endif


/*******************************************************************************
*   AVL tree of unsigned long.                                                 *
*******************************************************************************/
#define KAVLULM_MAX_HEIGHT  26
/**
 * AVL key type
 */
typedef unsigned long AVLULMKEY;

/**
 * AVL Core node.
 */
typedef struct _AVLULMNodeCore
{
    AVLULMKEY               Key;        /* Key value. */
    struct  _AVLULMNodeCore *pLeft;     /* Pointer to left leaf node. */
    struct  _AVLULMNodeCore *pRight;    /* Pointer to right leaf node. */
    struct  _AVLULMNodeCore *pList;     /* Pointer to list of equal nodes. */
    unsigned char           uchHeight;  /* Height of this tree: max(heigth(left), heigth(right)) + 1 */
} AVLULMNODECORE, *PAVLULMNODECORE, **PPAVLULMNODECORE;


/**
 * AVL Enum data - All members are PRIVATE! Don't touch!
 */
typedef struct _AVLULMEnumData
{
    char                fFromLeft;
    char                cEntries;
    char                achFlags[KAVLULM_MAX_HEIGHT];
    PAVLULMNODECORE     aEntries[KAVLULM_MAX_HEIGHT];
} AVLULMENUMDATA, *PAVLULMENUMDATA;

/*
 * callback type
 */
typedef unsigned ( _PAVLULMCALLBACK)(PAVLULMNODECORE, void*);
typedef _PAVLULMCALLBACK *PAVLULMCALLBACK;


/*
 * Functions.
 */
PAVLULMNODECORE     KLIBCALL AVLULMRemove(PPAVLULMNODECORE ppTree, AVLULMKEY Key);
PAVLULMNODECORE     KLIBCALL AVLULMRemove2(PPAVLULMNODECORE ppTree, PAVLULMNODECORE pNode);
KBOOL               KLIBCALL AVLULMInsert(PPAVLULMNODECORE ppTree, PAVLULMNODECORE pNode);
PAVLULMNODECORE     KLIBCALL AVLULMGet(PPAVLULMNODECORE ppTree, AVLULMKEY Key);
PAVLULMNODECORE     KLIBCALL AVLULMGet2(PPAVLULMNODECORE ppTree, PAVLULMNODECORE pNodeGet);
PAVLULMNODECORE     KLIBCALL AVLULMGetWithParent(PPAVLULMNODECORE ppTree, PPAVLULMNODECORE ppParent, AVLULMKEY Key);
PAVLULMNODECORE     KLIBCALL AVLULMGetWithAdjacentNodes(PPAVLULMNODECORE ppTree, AVLULMKEY Key, PPAVLULMNODECORE ppLeft, PPAVLULMNODECORE ppRight);
PAVLULMNODECORE     KLIBCALL AVLULMGetBestFit(PPAVLULMNODECORE ppTree, AVLULMKEY Key, KBOOL fAbove);
PAVLULMNODECORE     KLIBCALL AVLULMRemoveBestFit(PPAVLULMNODECORE ppTree, AVLULMKEY Key, KBOOL fAbove);
PAVLULMNODECORE     KLIBCALL AVLULMBeginEnumTree(PPAVLULMNODECORE ppTree, PAVLULMENUMDATA pEnumData, int fFromLeft);
PAVLULMNODECORE     KLIBCALL AVLULMGetNextNode(PAVLULMENUMDATA pEnumData);
unsigned            KLIBCALL AVLULMDoWithAll(PPAVLULMNODECORE ppTree, int fFromLeft, PAVLULMCALLBACK pfnCallBack, void *pvParam);


#endif
