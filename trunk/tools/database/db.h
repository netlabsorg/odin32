/* $Id: db.h,v 1.5 2000-02-12 17:55:03 bird Exp $ */
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
*   Defined Constants                                                          *
*******************************************************************************/
#define NBR_FUNCTIONS       20
#define NBR_AUTHORS         20
#define ALIAS_NULL          -1
#define ALIAS_DONTMIND      -2


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
        long  cRefCodes;
        long  alRefCode[NBR_FUNCTIONS];
        signed long  lImpDll; /* -1 is SQL-NULL, -2 is do not mind, >= 0 ref to dll. */

        /* parameters */
        int   cParams;
        char *apszParamType[30];
        char *apszParamName[30];

        /* authors */
        int   cAuthors;
        char *apszAuthor[NBR_AUTHORS];
        long  alAuthorRefCode[NBR_AUTHORS];

        /* status */
        char *pszStatus;
        long  lStatus;
    } FNDESC, *PFNDESC;


    typedef struct _FunctionFindBuffer
    {
        unsigned long cFns;
        signed long   alRefCode[NBR_FUNCTIONS];
        signed long   alDllRefCode[NBR_FUNCTIONS];
        signed long   alAliasFn[NBR_FUNCTIONS]; /* -1 is SQL-NULL, -2 is do not mind, >= 0 ref to function. */
    } FNFINDBUF, *PFNFINDBUF;

    typedef long (_System DBCALLBACKFETCH)(const char*, const char *, void *);

/*******************************************************************************
*   Exported Functions                                                         *
*******************************************************************************/
    char *          _System dbGetLastErrorDesc(void);

    BOOL            _System dbConnect(const char *pszHost,
                                      const char *pszUser,
                                      const char *pszPassword,
                                      const char *pszDatabase);
    BOOL            _System dbDisconnect();
    signed short    _System dbGetDll(const char *pszDllName);
    signed long     _System dbCountFunctionInDll(signed long ulDll);

    signed short    _System dbCheckInsertDll(const char *pszDll);
    unsigned short  _System dbGet(const char *pszTable,
                                  const char *pszGetColumn,
                                  const char *pszMatch1,
                                  const char *pszMatchValue1);
    BOOL            _System dbInsertUpdateFunction(unsigned short usDll,
                                                   const char *pszFunction,
                                                   const char *pszIntFunction,
                                                   unsigned long ulOrdinal,
                                                   BOOL fIgnoreOrdinal);
    BOOL            _System dbFindFunction(const char *pszFunctionName,
                                           PFNFINDBUF pFnFindBuf,
                                           signed long lDll);
    signed long     _System dbFindAuthor(const char *pszAuthor);
    signed long     _System dbGetFunctionState(signed long lRefCode);
    unsigned long   _System dbUpdateFunction(PFNDESC pFnDesc,
                                             signed long lDll,
                                             char *pszError);
    unsigned long   _System dbCreateHistory(char *pszError);
    unsigned long   _System dbCheckIntegrity(char *pszError);

    /* kHtml stuff */
    void *          _System dbExecuteQuery(const char *pszQuery);
    signed long     _System dbQueryResultRows(void *pres);
    BOOL            _System dbFreeResult(void *pres);
    BOOL            _System dbFetch(void *pres,
                                    DBCALLBACKFETCH dbFetchCallBack,
                                    void *pvUser);
    signed long     _System dbDateToDaysAfterChrist(const char *pszDate);
    BOOL            _System dbDaysAfterChristToDate(signed long ulDays,
                                                    char *pszDate);
    /* StateUpd stuff */
    BOOL            _System dbGetNotUpdatedFunction(signed long lDll,
                                                    DBCALLBACKFETCH dbFetchCallBack);
    signed long     _System dbGetNumberOfUpdatedFunction(signed long lDll);

#ifdef __cplusplus
}
#endif

#pragma pack()

#endif

