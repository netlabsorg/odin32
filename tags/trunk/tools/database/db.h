/* $Id: db.h,v 1.9 2000-07-18 07:33:15 bird Exp $ */
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
#define NBR_PARAMETERS      30
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
        char  szFnDclBuffer[2048];
        char  szFnHdrBuffer[10240];

        /* function name and type */
        char *pszName;
        char *pszReturnType;
        long  cRefCodes;
        long  alRefCode[NBR_FUNCTIONS];
        signed long  lImpDll; /* -1 is SQL-NULL, -2 is do not mind, >= 0 ref to dll. */

        /* parameters */
        int   cParams;
        char *apszParamType[NBR_PARAMETERS];
        char *apszParamName[NBR_PARAMETERS];
        char *apszParamDesc[NBR_PARAMETERS];

        /* authors */
        int   cAuthors;
        char *apszAuthor[NBR_AUTHORS];
        long  alAuthorRefCode[NBR_AUTHORS];

        /* other description fields */
        char *pszDescription;
        char *pszRemark;
        char *pszReturnDesc;
        char *pszSketch;
        char *pszEquiv;
        char *pszTime;

        /* status */
        char *pszStatus;
        long  lStatus;
    } FNDESC, *PFNDESC;


    typedef struct _FunctionFindBuffer
    {
        unsigned long cFns;
        signed long   alRefCode[NBR_FUNCTIONS];
        signed long   alDllRefCode[NBR_FUNCTIONS];
        signed long   alAliasFn[NBR_FUNCTIONS];     /* -1 is SQL-NULL, -2 is "do not mind", >= 0 ref to function. */
        signed long   alFileRefCode[NBR_FUNCTIONS]; /* -1 is SQL-NULL, -2 is "do not mind", >= 0 ref to file. */
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
    signed long     _System dbCountFunctionInDll(signed long ulDll,
                                                 BOOL fNotAliases);
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
    BOOL            _System dbInsertUpdateFile(unsigned short usDll,
                                               const char *pszFilename,
                                               const char *pszDescription,
                                               const char *pszLastDateTime,
                                               signed long lLastAuthor,
                                               const char *pszRevision);
    BOOL            _System dbFindFunction(const char *pszFunctionName,
                                           PFNFINDBUF pFnFindBuf,
                                           signed long lDll);
    signed long     _System dbFindFile(signed long lDll, const char *pszFilename);
    signed long     _System dbFindAuthor(const char *pszAuthor, const char *pszEmail);
    signed long     _System dbGetFunctionState(signed long lRefCode);
    unsigned long   _System dbUpdateFunction(PFNDESC pFnDesc,
                                             signed long lDll,
                                             char *pszError);
    BOOL            _System dbRemoveDesignNotes(signed long lFile);
    BOOL            _System dbAddDesignNote(signed long lDll,
                                            signed long lFile,
                                            const char *pszTitle,
                                            const char *pszText,
                                            signed long lSeqNbr,
                                            signed long lSeqNbrFile);
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

