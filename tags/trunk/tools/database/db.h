/* $Id: db.h,v 1.2 1999-12-06 18:11:50 bird Exp $ */
/*
 * DB - contains all database routines
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _db_h_
#define _db_h_

#pragma pack(4)

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
    typedef struct _FunctionDescription
    {
        /* buffers */
        char  szFnDclBuffer[512];
        char  szFnHdrBuffer[1024];

        /* function name and type */
        char *pszName;
        char *pszReturnType;
        long  lRefCode;

        /* parameters */
        int   cParams;
        char *apszParamType[30];
        char *apszParamName[30];

        /* authors */
        int   cAuthors;
        char *apszAuthor[20];
        long  alAuthorRefCode[20];

        /* status */
        char *pszStatus;
        long  lStatus;
    } FNDESC, *PFNDESC;


    typedef struct _FunctionFindBuffer
    {
        unsigned long cFns;
        signed long   alRefCode[10];
        signed long   alDllRefCode[10];
    } FNFINDBUF, *PFNFINDBUF;

    typedef long (_System DBCALLBACKFETCH)(const char*, const char *, void *);

/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
    char *          _System dbGetLastErrorDesc(void);

    BOOL            _System dbConnect(const char *pszHost, const char *pszUser, const char *pszPassword, const char *pszDatabase);
    BOOL            _System dbDisconnect();
    signed short    _System dbCheckInsertDll(const char *pszDll);
    unsigned short  _System dbGet(const char *pszTable, const char *pszGetColumn,
                                const char *pszMatch1, const char *pszMatchValue1);
    BOOL            _System dbInsertUpdateFunction(unsigned short usDll, const char *pszFunction,
                                unsigned long ulOrdinal, BOOL fIgnoreOrdinal);
    BOOL            _System dbFindFunction(const char *pszFunctionName, PFNFINDBUF pFnFindBuf);
    signed long     _System dbFindAuthor(const char *pszAuthor);
    signed long     _System dbGetFunctionState(signed long lRefCode);
    unsigned long   _System dbUpdateFunction(PFNDESC pFnDesc, char *pszError);
    unsigned long   _System dbCreateHistory(char *pszError);
    unsigned long   _System dbCheckIntegrity(char *pszError);

    /* kHtml stuff */
    void *          _System dbExecuteQuery(const char *pszQuery);
    signed long     _System dbQueryResultRows(void *pres);
    BOOL            _System dbFreeResult(void *pres);
    BOOL            _System dbFetch(void *pres, DBCALLBACKFETCH dbFetchCallBack, void *pvUser);
    signed long     _System dbDateToDaysAfterChrist(const char *pszDate);
    BOOL            _System dbDaysAfterChristToDate(signed long ulDays, char *pszDate);

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif

