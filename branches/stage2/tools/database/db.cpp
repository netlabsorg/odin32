/* $Id: db.cpp,v 1.25.2.1 2001-09-07 10:26:41 bird Exp $ *
 *
 * DB - contains all database routines.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define CheckLogContinue(sprintfargs) \
    if (rc < 0)                       \
    {                                 \
        if (pszError[1] == '\xFE')    \
        {                             \
            strcat(pszError, "\n\t"); \
            pszError += 2;            \
        }                             \
        sprintf sprintfargs;          \
        ulRc++;                       \
        pszError += strlen(pszError); \
        pszError[1] = '\xFE';         \
    }                                 \
    rc=rc


#define CheckFKError(table, msg)       \
    pres2 = mysql_store_result(pmysql);\
    if (rc < 0 || pres2 == NULL ||     \
        mysql_num_rows(pres2) == 0)    \
    {                                  \
        if (pszError[1] == '\xFE')     \
        {                              \
            strcat(pszError, "\n\t");  \
            pszError += 2;             \
        }                              \
        sprintf(pszError, table ":"    \
                msg                    \
                " (refcode=%s) "       \
                "(sql=%s)",            \
                row1[0],               \
                pszQuery);             \
        ulRc++;                        \
        pszError += strlen(pszError);  \
        pszError[1] = '\xFE';          \
    }                                  \
    if (pres2 != NULL)                 \
        mysql_free_result(pres2)


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSMISC
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <signal.h>
#include <assert.h>
#include <limits.h>
#include <mysql.h>

#include "db.h"


/*@Global***********************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static MYSQL         mysql;
static MYSQL        *pmysql = NULL;


/*@IntFunc**********************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static long getvalue(int iField, MYSQL_ROW pRow);
static unsigned long CheckAuthorError(char * &pszError, const char *pszFieldName, const char *pszFieldValue, const char *pszQuery);
static unsigned long logDbError(char * &pszError, const char *pszQuery);
static char *sqlstrcat(char *pszQuery, const char *pszBefore, const char *pszStr, const char *pszAfter = NULL);

#ifndef DLL
    extern "C" void      dbHandler(int sig);
#endif


/**
 * Gets the descriptions of the last database error.
 * @returns   Readonly string.
 */
char * _System dbGetLastErrorDesc(void)
{
    return mysql_error(&mysql);
}


/**
 * Connects to local database.
 * @returns   Success indicator, TRUE / FALSE.
 * @param     pszDatabase   Name of database to use.
 */
BOOL _System dbConnect(const char *pszHost, const char *pszUser, const char *pszPassword, const char *pszDatabase)
{
    BOOL fRet = FALSE;
    #ifndef DLL
        static fHandler = FALSE;
        /* signal handler */
        if (!fHandler)
        {
            if (   SIG_ERR == signal(SIGBREAK, dbHandler)
                || SIG_ERR == signal(SIGINT,   dbHandler)
                || SIG_ERR == signal(SIGTERM,  dbHandler)
                || SIG_ERR == signal(SIGABRT,  dbHandler)
                || SIG_ERR == signal(SIGSEGV,  dbHandler)
                || SIG_ERR == signal(SIGILL,   dbHandler)
                )
                fprintf(stderr, "Error installing signalhandler...");
            else
                fHandler = TRUE;
        }
    #endif

    /* connect to server */
    memset(&mysql, 0, sizeof(mysql));
    pmysql = mysql_connect(&mysql, pszHost, pszUser, pszPassword);
    if (pmysql != NULL)
    {
        /* connect to database */
        fRet = mysql_select_db(pmysql, pszDatabase) >= 0;
        if (fRet)
            mysql_refresh(pmysql, REFRESH_TABLES);
    }

    return fRet;
}


/**
 * Disconnects from database.
 * @returns   Success indicator. TRUE / FALSE.
 */
BOOL _System dbDisconnect(void)
{
    if (pmysql != NULL)
    {
        mysql_refresh(pmysql, REFRESH_TABLES);
        mysql_close(pmysql);
        pmysql = NULL;
    }
    return TRUE;
}

/**
 * Gets the refid for the give mod name.
 * @returns Module refid. -1 on error.
 * @param   pszModName      Module name.
 */
signed long _System dbGetModule(const char *pszModName)
{
    int         rc;
    char        szQuery[256];
    MYSQL_RES * pres;

    sprintf(&szQuery[0], "SELECT refcode FROM module WHERE name = '%s'\n", pszModName);
    rc   = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);

    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) == 1)
        rc = (int)getvalue(0, mysql_fetch_row(pres));
    else
        rc = -1;
    mysql_free_result(pres);
    return (signed long)rc;
}


/**
 * Count the function in a given module.
 * @returns Number of functions. -1 on error.
 * @param   lModule         Module refcode.
 * @param   fNotAliases     TRUE: don't count aliased functions.
 */
signed long     _System dbCountFunctionInModule(signed long lModule, BOOL fNotAliases)
{
    signed long rc;
    char        szQuery[256];
    MYSQL_RES * pres;

    if (lModule >= 0)
    {
        sprintf(&szQuery[0], "SELECT count(refcode) FROM function WHERE module = %ld\n", lModule);
        if (fNotAliases)
            strcat(&szQuery[0], " AND aliasfn < 0");
        rc   = mysql_query(pmysql, &szQuery[0]);
        pres = mysql_store_result(pmysql);

        if (rc >= 0 && pres != NULL && mysql_num_rows(pres) == 1)
            rc = (int)getvalue(0, mysql_fetch_row(pres));
        else
            rc = -1;
        mysql_free_result(pres);
    }
    else
        rc = -1;
    return rc;
}



/**
 * Checks if module exists. If not exists the module is inserted.
 * @returns Module refcode. -1 on errors.
 * @param   pszModule   Module name.
 * @param   fchType     Module type.
 * @remark  This search must be case insensitive.
 *          (In the mysql-world everything is case insensitive!)
 */
signed long _System dbCheckInsertModule(const char *pszModule, char fchType)
{
    int         rc;
    char        szQuery[256];
    MYSQL_RES * pres;

    /* try find match */
    sprintf(&szQuery[0], "SELECT refcode, name FROM module WHERE name = '%s'\n", pszModule);
    rc   = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);

    /* not found? - insert module */
    if (rc < 0 || pres == NULL || mysql_num_rows(pres) == 0)
    {
        mysql_free_result(pres);

        sprintf(&szQuery[0], "INSERT INTO module(name, type) VALUES('%s', '%c')\n", pszModule, fchType);
        rc = mysql_query(pmysql, &szQuery[0]);
        if (rc < 0)
            return -1;

        /* select row to get refcode */
        sprintf(&szQuery[0], "SELECT refcode, name FROM module WHERE name = '%s'\n", pszModule);
        rc   = mysql_query(pmysql, &szQuery[0]);
        pres = mysql_store_result(pmysql);
    }

    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) == 1)
        rc = (int)getvalue(0, mysql_fetch_row(pres));
    else
        rc = -1;
    mysql_free_result(pres);

    return (long)rc;
}


/**
 * Simple select for a long value.
 * @returns   long value
 * @param     pszTable        From part.
 * @param     pszGetColumn    Name of column to retreive.
 * @param     pszMatch1       Match column/expression
 * @param     pszMatchValue1  Match value.
 * @remark    Dirty! Don't use this!
 */
unsigned short _System dbGet(const char *pszTable, const char *pszGetColumn,
                             const char *pszMatch1, const char *pszMatchValue1)
{
    int  rc;
    char szQuery[256];
    MYSQL_RES *pres;

    /* try find match */
    sprintf(&szQuery[0], "SELECT %s FROM %s WHERE %s = '%s'\n",
            pszGetColumn, pszTable, pszMatch1, pszMatchValue1);
    rc   = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);

    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) == 1)
        rc = (int)getvalue(0, mysql_fetch_row(pres));
    else
        rc = -1;
    mysql_free_result(pres);

    return (unsigned short)rc;
}


/**
 * Updates or inserts a function name into the database.
 * The update flags is always updated.
 * @returns     Success indicator. TRUE / FALSE.
 * @param       lModule             Module refcode.
 * @param       pszFunction         Function name.
 * @param       pszIntFunction      Internal function name. (required!)
 * @param       ulOrdinal           Ordinal value.
 * @param       fIgnoreOrdinal      Do not update ordinal value.
 * @param       fchType             Function type flag. One of the FUNCTION_* defines.
 */
BOOL _System dbInsertUpdateFunction(signed long lModule,
                                    const char *pszFunction, const char *pszIntFunction,
                                    unsigned long ulOrdinal, BOOL fIgnoreOrdinal, char fchType)
{
    int     rc;
    long    lFunction = -1;
    char    szQuery[512];
    char *  pszQuery = &szQuery[0];
    MYSQL_RES *pres;

    /* when no internal name fail! */
    if (pszIntFunction == NULL || *pszIntFunction == '\0')
        return FALSE;

    /* try find function */
    sprintf(pszQuery, "SELECT refcode, intname FROM function WHERE module = %d AND name = '%s'", lModule, pszFunction);
    rc = mysql_query(pmysql, pszQuery);
    pres = mysql_store_result(pmysql);
    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) != 0)
    {   /*
         * Found the function. So now we'll update it.
         */
        MYSQL_ROW parow;
        if (mysql_num_rows(pres) > 1)
        {
            fprintf(stderr, "internal database integrity error(%s): More function by the same name for the same module. "
                    "lModule = %d, pszFunction = %s\n", __FUNCTION__, lModule, pszFunction);
            return FALSE;
        }

        parow = mysql_fetch_row(pres);
        lFunction = getvalue(0, parow);
        mysql_free_result(pres);

        strcpy(pszQuery, "UPDATE function SET updated = updated + 1");
        pszQuery += strlen(pszQuery);
        if (strcmp(parow[1], pszIntFunction) != 0)
            pszQuery += sprintf(pszQuery, ", intname = '%s'", pszIntFunction);

        if (!fIgnoreOrdinal)
            pszQuery += sprintf(pszQuery, ", ordinal = %ld", ulOrdinal);

        sprintf(pszQuery, ", type = '%c' WHERE refcode = %ld", fchType, lFunction);
        rc = mysql_query(pmysql, &szQuery[0]);
    }
    else
    {   /*
         * The function was not found. (or maybe an error occured?)
         * Insert it.
         */
        sprintf(&szQuery[0], "INSERT INTO function(module, name, intname, ordinal, updated, type) VALUES(%d, '%s', '%s', %ld, 1, '%c')",
                lModule, pszFunction, pszIntFunction, ulOrdinal, fchType);
        rc = mysql_query(pmysql, &szQuery[0]);
    }

    return rc >= 0;
}



/**
 * Inserts or updates (existing) file information.
 * @returns     Success indicator (TRUE / FALSE).
 * @param       lModule         Module reference code.
 * @param       pszFilename     Filename.
 * @param       pszDescription  Pointer to file description.
 * @param       pszLastDateTime Date and time for last change (ISO).
 * @param       lLastAuthor     Author number. (-1 if not found.)
 * @param       pszRevision     Pointer to revision string.
 * @sketch
 * @remark
 */
BOOL            _System dbInsertUpdateFile(signed long lModule,
                                           const char *pszFilename,
                                           const char *pszDescription,
                                           const char *pszLastDateTime,
                                           signed long lLastAuthor,
                                           const char *pszRevision)
{
    int  rc;
    long lFile = -1;
    char szQuery[0x10000];
    MYSQL_RES *pres;

    /* parameter assertions */
    assert(lModule != 0);
    assert(pszFilename != NULL);
    assert(*pszFilename != '\0');

    /* try find file */
    sprintf(&szQuery[0], "SELECT refcode, name FROM file WHERE module = %d AND name = '%s'", lModule, pszFilename);
    rc = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);
    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) != 0)
    {   /* update file (file is found) */
        MYSQL_ROW parow;
        if (mysql_num_rows(pres) > 1)
        {
            fprintf(stderr, "internal database integrity error(%s): More files by the same name in the same module. "
                    "lModule = %d, pszFilename = %s\n", __FUNCTION__, lModule, pszFilename);
            return FALSE;
        }

        parow = mysql_fetch_row(pres);
        assert(parow);
        lFile = getvalue(0, parow);
        mysql_free_result(pres);

        if (strcmp(parow[1], pszFilename) != 0) /* case might have changed... */
        {
            sprintf(&szQuery[0], "UPDATE file SET name = '%s' WHERE refcode = %ld",
                    pszFilename, lFile);
            rc = mysql_query(pmysql, &szQuery[0]);
        }

        if (rc >= 0)
        {
            if (pszDescription != NULL && pszDescription != '\0')
            {
                szQuery[0] = '\0';
                sqlstrcat(&szQuery[0], "UPDATE file SET description = ", pszDescription, NULL);
                sprintf(&szQuery[strlen(szQuery)], " WHERE refcode = %ld", lFile);
            }
            else
                sprintf(&szQuery[0], "UPDATE file SET description = NULL WHERE refcode = %ld",
                        lFile);
            rc = mysql_query(pmysql, &szQuery[0]);
        }

        if (rc >= 0 && pszLastDateTime != NULL && *pszLastDateTime != '\0')
        {
            sprintf(&szQuery[0], "UPDATE file SET lastdatetime = '%s' WHERE refcode = %ld",
                    pszLastDateTime, lFile);
            rc = mysql_query(pmysql, &szQuery[0]);
        }

        if (rc >= 0)
        {
            sprintf(&szQuery[0], "UPDATE file SET lastauthor = %ld WHERE refcode = %ld",
                    lLastAuthor, lFile);
            rc = mysql_query(pmysql, &szQuery[0]);
        }

        if (rc >= 0 && pszRevision != NULL && *pszRevision != '\0')
        {
            sprintf(&szQuery[0], "UPDATE file SET revision = '%s' WHERE refcode = %ld",
                    pszRevision, lFile);
            rc = mysql_query(pmysql, &szQuery[0]);
        }

    }
    else
    {   /* insert */
        sprintf(&szQuery[0], "INSERT INTO file(module, name, lastauthor, description, lastdatetime, revision) VALUES(%d, '%s', %ld, ",
                lModule, pszFilename, lLastAuthor);
        if (pszDescription != NULL && *pszDescription != '\0')
            sqlstrcat(&szQuery[0], NULL, pszDescription);
        else
            strcat(&szQuery[0], "NULL");

        if (pszLastDateTime != NULL && *pszLastDateTime != '\0')
            sqlstrcat(&szQuery[0], ", ", pszLastDateTime);
        else
            strcat(&szQuery[0], ", '1975-03-13 14:00:00'"); /* dummy */

        if (pszRevision != NULL && *pszRevision != '\0')
            sqlstrcat(&szQuery[0], ", ", pszRevision, ")");
        else
            strcat(&szQuery[0], ", '')");

        rc = mysql_query(pmysql, &szQuery[0]);
    }

    return rc >= 0;
}


/**
 * Get a long value.
 * @returns   Number value of pRow[iField]. -1 on error.
 * @param     iField  Index into pRow.
 * @param     pRow    Pointer to array (of string pointers).
 */
static long getvalue(int iField, MYSQL_ROW papszRow)
{
    if (papszRow[iField] != NULL)
        return atol((char*)papszRow[iField]);

    return -1;
}


#if 0
/*
 * Stubs used while optimizing sqls.
 */
int     mysql_query1(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_query2(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_query3(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_query4(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_query5(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_query6(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }

#else

#define mysql_query1            mysql_query
#define mysql_query2            mysql_query
#define mysql_query3            mysql_query
#define mysql_query4            mysql_query
#define mysql_query5            mysql_query
#define mysql_query6            mysql_query

#endif



/**
 * Find occurences of a function, given by internal name.
 * @returns   success indicator, TRUE / FALSE.
 * @param     pszFunctionName   Pointer to a function name string. (input)
 * @param     pFnFindBuf        Pointer to a find buffer. (output)
 * @param     lModule           Module refcode (optional). If given the search is limited to
 *                              the given module and aliasing functions is updated (slow!).
 * @sketch    1) Get functions for this module(if given).
 *            2) Get functions which aliases the functions found in (1).
 *            3) Get new aliases by intname
 *            4) Get new aliases by name
 *            5) Update all functions from (1) to have aliasfn -2 (DONTMIND)
 *            6) Update all functions from (3) and (4) to alias the first function from 1.
 */
BOOL _System dbFindFunction(const char *pszFunctionName, PFNFINDBUF pFnFindBuf, signed long lModule)
{
    MYSQL_RES   *pres;
    MYSQL_ROW    row;
    int          rc;
    char         szQuery[1024];

    /*
     * 1) Get functions for this module(if given).
     */
    if (lModule < 0)
        sprintf(&szQuery[0], "SELECT refcode, module, aliasfn, file, name, type FROM function WHERE intname = '%s'",
                pszFunctionName);
    else
        sprintf(&szQuery[0], "SELECT refcode, module, aliasfn, file, name, type FROM function "
                "WHERE intname = '%s' AND module = %ld",
                pszFunctionName, lModule);

    rc = mysql_query1(pmysql, &szQuery[0]);
    if (rc >= 0)
    {
        pres = mysql_store_result(pmysql);
        if (pres != NULL)
        {
            char szFnName[NBR_FUNCTIONS][80];
            BOOL fAPI = FALSE;

            pFnFindBuf->cFns = 0;
            while ((row = mysql_fetch_row(pres)) != NULL)
            {
                pFnFindBuf->alRefCode[pFnFindBuf->cFns] = atol(row[0]);
                pFnFindBuf->alModRefCode[pFnFindBuf->cFns] = atol(row[1]);
                pFnFindBuf->alAliasFn[pFnFindBuf->cFns] = atol(row[2]);
                pFnFindBuf->alFileRefCode[pFnFindBuf->cFns] = atol(row[3]);
                strcpy(szFnName[pFnFindBuf->cFns], row[4]);
                pFnFindBuf->achType[pFnFindBuf->cFns] = *row[5];
                if (pFnFindBuf->achType[pFnFindBuf->cFns] == FUNCTION_ODIN32_API ||
                    pFnFindBuf->achType[pFnFindBuf->cFns] == FUNCTION_INTERNAL_ODIN32_API)
                    fAPI = TRUE;

                /* next */
                pFnFindBuf->cFns++;
            }
            mysql_free_result(pres);

            /* alias check and fix for apis. */
            if (fAPI && lModule >= 0 && pFnFindBuf->cFns != 0)
            {
                int cFnsThisModule, cFnsAliasesAndThisModule, i, f;

                /*
                 * 2) Get functions which aliases the functions found in (1).
                 */
                cFnsThisModule = (int)pFnFindBuf->cFns;
                strcpy(&szQuery[0], "SELECT refcode, module, aliasfn, file, name FROM function WHERE aliasfn IN (");
                for (i = 0; i < cFnsThisModule; i++)
                {
                    if (i > 0)  strcat(&szQuery[0], " OR ");
                    sprintf(&szQuery[strlen(szQuery)], "(%ld)", pFnFindBuf->alRefCode[i]);
                }
                strcat(&szQuery[0], ")");

                rc = mysql_query2(pmysql, &szQuery[0]);
                if (rc >= 0)
                {
                    pres = mysql_store_result(pmysql);
                    if (pres != NULL)
                    {
                        while ((row = mysql_fetch_row(pres)) != NULL)
                        {
                            pFnFindBuf->alRefCode[pFnFindBuf->cFns] = atol(row[0]);
                            pFnFindBuf->alModRefCode[pFnFindBuf->cFns] = atol(row[1]);
                            pFnFindBuf->alAliasFn[pFnFindBuf->cFns] = atol(row[2]);
                            pFnFindBuf->alFileRefCode[pFnFindBuf->cFns] = atol(row[3]);
                            strcpy(szFnName[pFnFindBuf->cFns], row[4]);

                            /* next */
                            pFnFindBuf->cFns++;
                        }
                        mysql_free_result(pres);

                        /*
                         * 3) Get new aliases by intname
                         */
                        cFnsAliasesAndThisModule = (int)pFnFindBuf->cFns;
                        sprintf(&szQuery[0], "SELECT refcode, module, aliasfn, file FROM function "
                                             "WHERE aliasfn = (-1) AND module <> %ld AND (intname = '%s'",
                                lModule, pszFunctionName);
                        for (i = 0; i < cFnsAliasesAndThisModule; i++)
                            sprintf(&szQuery[strlen(&szQuery[0])], " OR intname = '%s'", szFnName[i]);
                        strcat(&szQuery[0], ")");

                        rc = mysql_query3(pmysql, &szQuery[0]);
                        if (rc >= 0)
                        {
                            pres = mysql_store_result(pmysql);
                            if (pres != NULL)
                            {
                                while ((row = mysql_fetch_row(pres)) != NULL)
                                {
                                    pFnFindBuf->alRefCode[pFnFindBuf->cFns] = atol(row[0]);
                                    pFnFindBuf->alModRefCode[pFnFindBuf->cFns] = atol(row[1]);
                                    if (row[2] != NULL)
                                        pFnFindBuf->alAliasFn[pFnFindBuf->cFns] = atol(row[2]);
                                    else
                                        pFnFindBuf->alAliasFn[pFnFindBuf->cFns] = ALIAS_NULL;
                                    pFnFindBuf->alFileRefCode[pFnFindBuf->cFns] = atol(row[3]);

                                    /* next */
                                    pFnFindBuf->cFns++;
                                }
                                mysql_free_result(pres);


                                /*
                                 * 4) Get new aliases by name
                                 */
                                sprintf(&szQuery[0], "SELECT refcode, module, aliasfn, file FROM function "
                                                     "WHERE aliasfn = (-1) AND module <> %ld AND (name = '%s'",
                                        lModule, pszFunctionName);
                                for (i = 0; i < cFnsAliasesAndThisModule; i++)
                                    sprintf(&szQuery[strlen(&szQuery[0])], " OR name = '%s'", szFnName[i]);
                                strcat(&szQuery[0], ")");

                                rc = mysql_query4(pmysql, &szQuery[0]);
                                if (rc >= 0)
                                {
                                    pres = mysql_store_result(pmysql);
                                    if (pres != NULL)
                                    {
                                        while ((row = mysql_fetch_row(pres)) != NULL)
                                        {
                                            pFnFindBuf->alRefCode[pFnFindBuf->cFns] = atol(row[0]);
                                            pFnFindBuf->alModRefCode[pFnFindBuf->cFns] = atol(row[1]);
                                            if (row[2] != NULL)
                                                pFnFindBuf->alAliasFn[pFnFindBuf->cFns] = atol(row[2]);
                                            else
                                                pFnFindBuf->alAliasFn[pFnFindBuf->cFns] = ALIAS_NULL;
                                            pFnFindBuf->alFileRefCode[pFnFindBuf->cFns] = atol(row[3]);

                                            /* next */
                                            pFnFindBuf->cFns++;
                                        }
                                        mysql_free_result(pres);

                                        /*
                                         * 5) Update all functions from (1) to have aliasfn -2 (DONTMIND)
                                         */
                                        sprintf(&szQuery[0], "UPDATE function SET aliasfn = (-2) "
                                                             "WHERE refcode IN (",
                                                lModule, pszFunctionName);
                                        for (f = 0, i = 0; i < cFnsThisModule; i++)
                                            if (pFnFindBuf->alAliasFn[i] != ALIAS_DONTMIND)
                                                sprintf(&szQuery[strlen(&szQuery[0])],
                                                        f++ != 0 ? ", %ld" : "%ld", pFnFindBuf->alRefCode[i]);
                                        strcat(&szQuery[0], ") AND aliasfn <> (-2)");
                                        if (f > 0)
                                            rc = mysql_query5(pmysql, &szQuery[0]);
                                        else
                                            rc = 0;
                                        if (rc >= 0 && cFnsAliasesAndThisModule < pFnFindBuf->cFns)
                                        {
                                            /*
                                             * 6) Update all functions from (3) and (4) to alias the first function from 1.
                                             */
                                            sprintf(&szQuery[0], "UPDATE function SET aliasfn = (%ld), file = (%ld) "
                                                                 "WHERE aliasfn = (-1) AND refcode IN (",
                                                    pFnFindBuf->alRefCode[0], pFnFindBuf->alFileRefCode[0]);
                                            for (i = cFnsAliasesAndThisModule; i < pFnFindBuf->cFns; i++)
                                            {
                                                sprintf(&szQuery[strlen(&szQuery[0])],
                                                        i > cFnsAliasesAndThisModule ? ", %ld" : "%ld", pFnFindBuf->alRefCode[i]);
                                            }
                                            strcat(&szQuery[0], ")");
                                            rc = mysql_query6(pmysql, &szQuery[0]);
                                        } /* query 5 */
                                    }
                                } /* query 4 */
                            }
                        } /* query 3 */
                    }
                } /* query 2 */
            }
        } /* query 1 */
        else
            rc = -1;
    }

    return rc >= 0;
}


/**
 * Finds the refcode for a file (if it exists).
 * @returns File 'refcode'.
 *          -1 on error or not found.
 * @param   lModule         Refcode of the module which this file belongs to.
 * @param   pszFilename     The filename to search for.
 */
signed long     _System dbFindFile(signed long lModule, const char *pszFilename)
{
    char        szQuery[256];
    MYSQL_RES * pres;
    signed long lRefCode = -1;

    assert(lModule >= 0);
    assert(pszFilename != NULL);
    assert(*pszFilename != '\0');

    sprintf(&szQuery[0], "SELECT refcode FROM file WHERE module = %ld AND name = '%s'",
            lModule, pszFilename);
    if (mysql_query(pmysql, &szQuery[0]) >= 0)
    {
        pres = mysql_store_result(pmysql);
        if (pres != NULL)
        {
            MYSQL_ROW parow = mysql_fetch_row(pres);
            if (parow != NULL)
                lRefCode = getvalue(0, parow);
            mysql_free_result(pres);
        }
    }

    return lRefCode;
}


/**
 * Finds the refcode for an author, if the author exists.
 * @returns   Author 'refcode'.
 * @param     pszAuthor  String which holds the identifier of an author.
 *                       This doesn't have to be the name. Initials, alias and email
 *                       is also searched.
 * @param     pszEmail   Email address. Might be NULL!
 */
signed long _System dbFindAuthor(const char *pszAuthor, const char *pszEmail)
{
    signed long refcode = -1;
    MYSQL_RES *pres;
    char szQuery[512];

    /*
     * parameter validations
     */
    if (pszAuthor == NULL || strlen(pszAuthor) > 64)
        return -1;
    if (pszEmail != NULL && strlen(pszEmail) > 64)
    {
        fprintf(stderr, "email too long!");
        return -1;
    }

    /*
     * Query
     */
    sprintf(&szQuery[0],
            "SELECT refcode FROM author "
            "WHERE name     = '%s' OR "
            "      initials = '%s' OR "
            "      alias    = '%s' OR "
            "      email    = '%s'",
            pszAuthor, pszAuthor, pszAuthor, pszAuthor);

    if (pszEmail != NULL)
        sprintf(&szQuery[strlen(&szQuery[0])], " OR email = '%s'", pszEmail);

    if (mysql_query(pmysql, &szQuery[0]) >= 0)
    {
        pres = mysql_store_result(pmysql);
        if (pres != NULL)
        {
            MYSQL_ROW parow;

            /* integrety check */
            if (mysql_num_rows(pres) > 1)
                fprintf(stderr, "Integrety: author '%s' is not unique!\n", pszAuthor);
            parow = mysql_fetch_row(pres);
            if (parow != NULL)
                refcode = getvalue(0, parow);

            mysql_free_result(pres);
        }
    }

    return refcode;
}


/**
 * Gets the state of a function.
 * @returns   state code. On error -1.
 * @param     lRefCode  Function refcode.
 */
signed long _System dbGetFunctionState(signed long lRefCode)
{
    signed long lState = -1;
    MYSQL_RES *pres;
    char szQuery[128];

    sprintf(&szQuery[0], "SELECT state FROM function WHERE refcode = %ld", lRefCode);
    if (mysql_query(pmysql, &szQuery[0]) >= 0)
    {
        pres = mysql_store_result(pmysql);
        if (pres != NULL)
        {
            MYSQL_ROW parow = mysql_fetch_row(pres);
            if (parow != NULL)
                lState = getvalue(0, parow);
            mysql_free_result(pres);
        }
    }

    return lState;
}

#if 1
/*
 * Stubs used while optimizing sqls.
 */
int     mysql_queryu1(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu2(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu3(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu4(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu5(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu6(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu7(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
int     mysql_queryu8(MYSQL *mysql, const char *q)
{   return mysql_query(mysql, q); }
#else
#define mysql_queryu1 mysql_query
#define mysql_queryu2 mysql_query
#define mysql_queryu3 mysql_query
#define mysql_queryu4 mysql_query
#define mysql_queryu5 mysql_query
#define mysql_queryu6 mysql_query
#define mysql_queryu7 mysql_query
#define mysql_queryu8 mysql_query
#endif

/**
 * Updates function information.
 * @returns   number of errors.
 * @param     pFnDesc   Function description struct.
 * @param     lModule   Module which we are working at.
 * @param     pszError  Buffer for error messages
 * @result    on error(s) pszError will hold information about the error(s).
 */
unsigned long _System dbUpdateFunction(PFNDESC pFnDesc, signed long lModule, char *pszError)
{
    MYSQL_RES *     pres;
    MYSQL_ROW       row;
    char *          pszQuery2 = (char*)malloc(65500);
    char *          pszQuery = pszQuery2;
    long            lCurrentState;
    int             i,k,rc;
    unsigned long   ulRc = 0;

    /* check if malloc have failed allocating memory for us. */
    if (pszQuery2 == NULL)
    {
        strcpy(pszError, "internal dbUpdateFunction error - malloc failed!\n");
        return 1;
    }


    /*
     * Loop thru all functions in the array of refocodes.
     */
    for (k = 0; k < pFnDesc->cRefCodes; k++)
    {
        /*
         * Get current status
         */
        lCurrentState = dbGetFunctionState(pFnDesc->alRefCode[k]);
        if (lCurrentState == -1 && dbGetLastErrorDesc() != NULL && strlen(dbGetLastErrorDesc()) != 0)
        {
            strcpy(pszError, dbGetLastErrorDesc());
            /*
             * Set updated flag
             */
            sprintf(pszQuery, "UPDATE function SET updated = updated + 1 WHERE refcode = %ld",
                    pFnDesc->alRefCode[k]);
            rc = mysql_queryu1(pmysql, pszQuery2);
            free(pszQuery2);
            return 1;
        }


        /*
         * Update function table first
         */
        strcpy(pszQuery, "UPDATE function SET updated = updated + 1");
        pszQuery += strlen(pszQuery);

        /* Status */
        if (lCurrentState != pFnDesc->lStatus
            && pFnDesc->lStatus != 0
            && (lCurrentState == 0 || pFnDesc->lStatus != 99)
            )
            pszQuery += sprintf(pszQuery, ", state = %ld", pFnDesc->lStatus);

        /* File */
        if (pFnDesc->lFile >= 0)
            pszQuery += sprintf(pszQuery, ", file = %ld", pFnDesc->lFile);
        else
            pszQuery += sprintf(pszQuery, ", file = -1");

        /* Line */
        if (pFnDesc->lLine >= 0)
            pszQuery += sprintf(pszQuery, ", line = %ld", pFnDesc->lLine);
        else
            pszQuery += sprintf(pszQuery, ", line = -1");

        /* return type */
        if (pFnDesc->pszReturnType != NULL)
            pszQuery = sqlstrcat(pszQuery, ", return = ",  pFnDesc->pszReturnType);
        else
            pszQuery += sprintf(pszQuery, ", return = NULL");

        /* Description */
        if (pFnDesc->pszDescription != NULL)
            pszQuery = sqlstrcat(pszQuery, ", description  = ", pFnDesc->pszDescription);
        else
            pszQuery += sprintf(pszQuery, ", description = NULL");

        /* Remark */
        if (pFnDesc->pszRemark != NULL)
            pszQuery = sqlstrcat(pszQuery, ", remark = ", pFnDesc->pszRemark);
        else
            pszQuery += sprintf(pszQuery, ", remark = NULL");

        /* Description */
        if (pFnDesc->pszReturnDesc != NULL)
            pszQuery = sqlstrcat(pszQuery,  ", returndesc = ", pFnDesc->pszReturnDesc);
        else
            pszQuery += sprintf(pszQuery, ", returndesc = NULL");

        /* Sketch */
        if (pFnDesc->pszSketch != NULL)
            pszQuery = sqlstrcat(pszQuery,  ", sketch = ", pFnDesc->pszSketch);
        else
            pszQuery += sprintf(pszQuery, ", sketch = NULL");

        /* Equiv */
        if (pFnDesc->pszEquiv != NULL)
            pszQuery = sqlstrcat(pszQuery,  ", equiv = ", pFnDesc->pszEquiv);
        else
            pszQuery += sprintf(pszQuery, ", equiv = NULL");

        /* Time */
        if (pFnDesc->pszTime != NULL)
            pszQuery = sqlstrcat(pszQuery,  ", time = ", pFnDesc->pszTime);
        else
            pszQuery += sprintf(pszQuery, ", time = NULL");

        /* Execute update query? */
        sprintf(pszQuery + strlen(pszQuery), " WHERE refcode = %ld", pFnDesc->alRefCode[k]);
        rc = mysql_queryu2(pmysql, pszQuery2);
        if (rc < 0)
        {
            sprintf(pszError, "Updating functiontable failed with error: %s - (sql=%s) ",
                    dbGetLastErrorDesc(), pszQuery2);
            pszError += strlen(pszError) - 1;
            ulRc++;
        }


        /*
         * Parameters
         */
        pszQuery = pszQuery2;
        sprintf(pszQuery, "SELECT count(*) FROM parameter WHERE function = %ld", pFnDesc->alRefCode[k]);
        rc = mysql_queryu3(pmysql, pszQuery);
        if (rc >= 0)
        {
            pres = mysql_store_result(pmysql);
            if (pres != NULL)
                row = mysql_fetch_row(pres);
            if (pres != NULL && row != NULL && mysql_num_rows(pres) == 1)
            {
                #if 0 /* keep getting duplicate keys when parameter order/names are changed. */
                if (atol(row[0]) == pFnDesc->cParams)
                {   /* update parameters */
                    for (i = 0; i < pFnDesc->cParams; i++)
                    {
                        sprintf(pszQuery, "UPDATE parameter SET type = '%s', name = '%s'",
                                pFnDesc->apszParamType[i] != NULL ? pFnDesc->apszParamType[i] : "",
                                pFnDesc->apszParamName[i] != NULL ? pFnDesc->apszParamName[i] : "");
                        if (pFnDesc->apszParamDesc[i] != NULL)
                            sqlstrcat(pszQuery, ", description = ", pFnDesc->apszParamDesc[i]);
                        sprintf(pszQuery + strlen(pszQuery), " WHERE function = (%ld) AND sequencenbr = (%ld)",
                                pFnDesc->alRefCode[k], i);
                        rc = mysql_queryu4(pmysql, pszQuery);
                        if (rc < 0)
                        {
                            if (*pszError == ' ')
                                strcpy(pszError++, "\n\t");
                            sprintf(pszError, "Updating parameter %i failed with error: %s - (sql=%s) ",
                                    i, dbGetLastErrorDesc(), pszQuery);
                            pszError += strlen(pszError) - 1;
                            ulRc++;
                        }
                    }
                }
                else
                #endif
                {
                    if (atol(row[0]) != 0)
                    {   /* delete old parameters */
                        sprintf(pszQuery, "DELETE FROM parameter WHERE function = %ld", pFnDesc->alRefCode[k]);
                        rc = mysql_queryu5(pmysql, pszQuery);
                        if (rc < 0)
                        {
                            if (*pszError == ' ')
                                strcpy(pszError++, "\n\t");
                            sprintf(pszError, "Deleting old parameters failed with error: %s - (sql=%s) ",
                                    dbGetLastErrorDesc(), pszQuery);
                            pszError += strlen(pszError) - 1;
                            ulRc++;
                        }
                    }

                    /* insert parameters */
                    for (i = 0; i < pFnDesc->cParams; i++)
                    {
                        sprintf(pszQuery, "INSERT INTO parameter(function, sequencenbr, type, name, description) "
                                "VALUES (%ld, %d, '%s', '%s'",
                                pFnDesc->alRefCode[k], i,
                                pFnDesc->apszParamType[i] != NULL ? pFnDesc->apszParamType[i] : "",
                                pFnDesc->apszParamName[i] != NULL ? pFnDesc->apszParamName[i] : ""
                                );
                        if (pFnDesc->apszParamDesc[i] != NULL)
                            sqlstrcat(pszQuery, ", ", pFnDesc->apszParamDesc[i], ")");
                        else
                            strcat(pszQuery, ", NULL)");

                        rc = mysql_queryu6(pmysql, pszQuery2);
                        if (rc < 0)
                        {
                            if (*pszError == ' ')
                                strcpy(pszError++, "\n\t");
                            sprintf(pszError, "Inserting parameter %i failed with error: %s - (sql=%s) ",
                                    i, dbGetLastErrorDesc(), pszQuery);
                            pszError += strlen(pszError) - 1;
                            ulRc++;
                        }
                    }
                }
            }
            else
            {
                if (*pszError == ' ')
                    strcpy(pszError++, "\n\t");
                sprintf(pszError, "failed to store result or to fetch a row , error: %s - (sql=%s) ",
                        dbGetLastErrorDesc(), pszQuery);
                pszError += strlen(pszError) - 1;
                ulRc++;
            }
        }
        else
        {
            if (*pszError == ' ')
                strcpy(pszError++, "\n\t");
            sprintf(pszError, "Failed querying number of parameters, error: %s - (sql=%s) ",
                    dbGetLastErrorDesc(), pszQuery);
            pszError += strlen(pszError) - 1;
            ulRc++;
        }


        /*
         * Authors
         */
        sprintf(pszQuery, "DELETE FROM fnauthor WHERE function = %ld", pFnDesc->alRefCode[k]);
        rc = mysql_queryu7(pmysql, pszQuery);
        if (rc < 0)
        {
            if (*pszError == ' ')
                strcpy(pszError++, "\n\t");
            sprintf(pszError, "Deleting old authors failed with error: %s - (sql=%s) ",
                    dbGetLastErrorDesc(), pszQuery);
            pszError += strlen(pszError) - 1;
            ulRc++;
        }

        for (i = 0; i < pFnDesc->cAuthors; i++)
        {
            if (pFnDesc->alAuthorRefCode[i] == -1)
                continue;
            sprintf(pszQuery, "INSERT INTO fnauthor(author, function) "
                    "VALUES (%ld, %ld)",
                    pFnDesc->alAuthorRefCode[i], pFnDesc->alRefCode[k]);
            rc = mysql_queryu8(pmysql, pszQuery);
            if (rc < 0)
            {
                if (*pszError == ' ')
                    strcpy(pszError++, "\n\t");
                sprintf(pszError, "Inserting parameter %i failed with error: %s - (sql=%s) ",
                        i, dbGetLastErrorDesc(), pszQuery);
                pszError += strlen(pszError) - 1;
                ulRc++;
            }
        }
    } /* for */

    lModule = lModule;
    free(pszQuery2);
    return ulRc;
}


/**
 * Removes all the existing design notes in the specified file.
 * @returns Success indicator.
 * @param   lFile       File refcode of the file to remove all design notes for.
 * @author  knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
BOOL            _System dbRemoveDesignNotes(signed long lFile)
{
    char        szQuery[80];

    assert(lFile >= 0);
    sprintf(&szQuery[0], "DELETE FROM designnote WHERE file = %ld", lFile);
    return mysql_query(pmysql, &szQuery[0]) >= 0;
}


/**
 * Adds a design note.
 * @returns     Success indicator.
 * @param       lModule         Module refcode.
 * @param       lFile           File refcode.
 * @param       pszTitle        Design note title.
 * @param       pszText         Design note text.
 * @param       lLevel          Level of the note section. 0 is the design note it self.
 * @param       lSeqNbr         Sequence number (in module). If 0 the use next available number.
 * @param       lSeqNbrNote     Sequence number in note.
 * @param       lLine           Line number (1 - based!).
 * @param       fSubSection     TRUE if subsection FALSE if design note.
 *                              if TRUE *plRefCode will hold the reference id of the note.
 *                              if FALSE *plRefCode will receive the reference id of the note being created.
 * @param       plRefCode       Pointer to reference id of the design note. see fSubSection for more info.
 */
BOOL            _System dbAddDesignNote(signed long lModule,
                                        signed long lFile,
                                        const char *pszTitle,
                                        const char *pszText,
                                        signed long lLevel,
                                        signed long lSeqNbr,
                                        signed long lSeqNbrNote,
                                        signed long lLine,
                                        BOOL        fSubSection,
                                        signed long *plRefCode)
{
    int         rc;
    char        szQuery[0x10200];
    MYSQL_RES * pres;


    assert(lModule >= 0 && lFile >= 0);
    assert(lSeqNbrNote >= 0);

    /*
     * If no lSqlNbr the make one.
     */
    if (lSeqNbr == 0 && !fSubSection)
    {
        sprintf(&szQuery[0], "SELECT MAX(seqnbr) + 1 FROM designnote WHERE module = %ld AND level = 0", lModule);
        if (mysql_query(pmysql, &szQuery[0]) >= 0)
        {
            pres = mysql_store_result(pmysql);
            if (pres != NULL)
            {
                MYSQL_ROW parow = mysql_fetch_row(pres);
                if (parow != NULL && parow[0])
                    lSeqNbr = getvalue(0, parow);
                else
                    lSeqNbr = 1;
                mysql_free_result(pres);
            }
            else
                return FALSE;
        }
        else
            return FALSE;
    }

    /*
     * Create insert query.
     */
    if (!fSubSection)
        sprintf(&szQuery[0], "INSERT INTO designnote(module, file, level, seqnbrnote, seqnbr, line, name, note) "
                             "VALUES (%ld, %ld, %ld, %ld, %ld, %ld, ",
                lModule, lFile, lLevel, lSeqNbrNote, lSeqNbr, lLine);
    else
        sprintf(&szQuery[0], "INSERT INTO designnote(refcode, module, file, level, seqnbrnote, seqnbr, line, name, note) "
                             "VALUES (%ld, %ld, %ld, %ld, %ld, %ld, %ld, ",
                *plRefCode, lModule, lFile, lLevel, lSeqNbrNote, lSeqNbr, lLine);

    if (pszTitle != NULL && *pszTitle != '\0')
        sqlstrcat(&szQuery[0], NULL, pszTitle);
    else
        strcat(&szQuery[0], "NULL");
    sqlstrcat(&szQuery[0], ", ", pszText == NULL ? "" : pszText, ")");

    if (mysql_query(pmysql, &szQuery[0]) >= 0)
    {
        if (!fSubSection)
            *plRefCode = mysql_insert_id(pmysql);
        return TRUE;
    }
    return FALSE;
}



/**
 * Updates the history tables.
 * @returns   Number of signals/errors.
 * @param     pszError  Pointer to buffer which will hold the error messages.
 * @remark    This should be called whenever updates have been completed.
 */
unsigned long _System dbCreateHistory(char *pszError)
{
    unsigned long   ulRc = 0;
    MYSQL_RES      *pres;
    MYSQL_ROW       row;
    char            szQuery[256];
    char           *pszQuery = &szQuery[0];
    int             rc;
    char            szCurDt[20] = {0}; /*yyyy-mm-dd\0*/

    mysql_refresh(pmysql, REFRESH_TABLES);

    /* get currentdate - just in case the date changes between the delete and the update is completed. */
    strcpy(pszQuery, "SELECT CURDATE()");
    rc = mysql_query(pmysql, pszQuery);
    pres = mysql_use_result(pmysql);
    if (rc >= 0 && pres != NULL)
    {
        row = mysql_fetch_row(pres);
        if (row != NULL && mysql_num_rows(pres) == 1)
        {
            strcpy(&szCurDt[0], row[0]);
            while (mysql_fetch_row(pres) != NULL)
                pres=pres;

            /* delete - all rows on this date in the history tables */
            sprintf(pszQuery, "DELETE FROM historymodule WHERE date = '%s'", &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "DELETE FROM historyapigroup WHERE date = '%s'", &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "DELETE FROM historymoduletotal WHERE date = '%s'", &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "DELETE FROM historyapigrouptotal WHERE date = '%s'", &szCurDt[0]);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            /* insert new stats */
            sprintf(pszQuery, "INSERT INTO historymodule(module, state, date, count, type) "
                    "SELECT module, state, '%s', count(*), type FROM function GROUP BY module, state, type",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "INSERT INTO historyapigroup(apigroup, state, date, count) "
                    "SELECT apigroup, state, '%s', count(*) FROM function WHERE apigroup IS NOT NULL "
                    "GROUP BY apigroup, state",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            /* inserting new totals */
            sprintf(pszQuery, "INSERT INTO historymoduletotal(module, date, totalcount, type) "
                    "SELECT module, '%s', count(*), type FROM function GROUP BY module, type",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "INSERT INTO historyapigrouptotal(apigroup, date, totalcount) "
                    "SELECT apigroup, '%s', count(*) FROM function WHERE apigroup IS NOT NULL AND type = 'A' "
                    "GROUP BY apigroup",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));
        }
        else
        {
            sprintf(pszError, "error getting current date (row == NULL): %s - (sql=%s) ",
                    dbGetLastErrorDesc(), pszQuery);
            ulRc++;
        }
    }
    else
    {
        sprintf(pszError, "error getting current date: %s - (sql=%s) ",
                dbGetLastErrorDesc(), pszQuery);
        ulRc++;
    }

    mysql_refresh(pmysql, REFRESH_TABLES);

    return ulRc;
}


/**
 * Check that database integrety is ok. Verfies foreign keys.
 * @returns   numbers of errors.
 * @param     pszError   Very large buffer which will hold error messges (if any).
 * @sketch
 * @remark    current versions of mysql don't support 'SELECT ... WHERE id NOT IN(SELECT id FROM table)'
 */
unsigned long   _System dbCheckIntegrity(char *pszError)
{
    char          szQuery[384];
    char         *pszQuery = &szQuery[0];
    MYSQL_RES    *pres1;
    MYSQL_RES    *pres2;
    MYSQL_ROW     row1;
    int           rc;
    unsigned long ulRc = 0;

    mysql_refresh(pmysql, REFRESH_TABLES);

    /* foreign keys in function table */
    strcpy(pszQuery, "SELECT refcode, module, state, apigroup, file FROM function");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM module WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("function/module", "Foreign key 'module' not found in the module table");

                /* check state */
                sprintf(pszQuery, "SELECT refcode FROM state WHERE refcode = %s", row1[2]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("function/state", "Foreign key 'state' not found in the state table");

                /* check apigroup */
                if (row1[3] != NULL)
                {
                    sprintf(pszQuery, "SELECT refcode FROM apigroup WHERE refcode = %s", row1[3]);
                    rc = mysql_query(pmysql, pszQuery);
                    CheckFKError("function/state", "Foreign key 'state' not found in the state table");
                }

                /* check file */
                if (atoi(row1[4]) >= 0)
                {
                    sprintf(pszQuery, "SELECT refcode FROM file WHERE refcode = %s", row1[4]);
                    rc = mysql_query(pmysql, pszQuery);
                    CheckFKError("function/file", "Foreign key 'file' not found in the file table");
                }
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in file */
    strcpy(pszQuery, "SELECT refcode, module FROM file");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM module WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("apigroup/module", "Foreign key 'module' not found in the module table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in apigroup */
    strcpy(pszQuery, "SELECT refcode, module FROM apigroup");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM module WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("file/module", "Foreign key 'module' not found in the module table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in fnauthor */
    strcpy(pszQuery, "SELECT function, author FROM fnauthor");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check function */
                sprintf(pszQuery, "SELECT refcode FROM function WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("fnauthor/function", "Foreign key 'function' not found in the function table");

                /* check author */
                sprintf(pszQuery, "SELECT refcode FROM author WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("fnauthor/author", "Foreign key 'author' not found in the author table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in historymodule table */
    strcpy(pszQuery, "SELECT date, module, state FROM historymodule");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM module WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historymodule/module", "Foreign key 'module' not found in the module table");

                /* check state */
                sprintf(pszQuery, "SELECT refcode FROM state WHERE refcode = %s", row1[2]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historymodule/state", "Foreign key 'state' not found in the state table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in historyapigroup table */
    strcpy(pszQuery, "SELECT date, apigroup, state FROM historyapigroup");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM apigroup WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historyapigroup/apigroup", "Foreign key 'apigroup' not found in the apigroup table");

                /* check state */
                sprintf(pszQuery, "SELECT refcode FROM state WHERE refcode = %s", row1[2]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historyapigroup/state", "Foreign key 'state' not found in the state table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in historymoduletotal table */
    strcpy(pszQuery, "SELECT date, module FROM historymoduletotal");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM module WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historymoduletotal/module", "Foreign key 'module' not found in the module table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in historyapigroup table */
    strcpy(pszQuery, "SELECT date, apigroup FROM historyapigrouptotal");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check module */
                sprintf(pszQuery, "SELECT refcode FROM apigroup WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historyapigrouptotal/apigroup", "Foreign key 'apigroup' not found in the apigroup table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in parameter table */
    strcpy(pszQuery, "SELECT sequencenbr, function FROM parameter");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check function */
                sprintf(pszQuery, "SELECT refcode FROM function WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("parameter/function", "Foreign key 'function' not found in the function table");
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* Author table is special, since you should be able to interchangably reference an
     * author by any of the following tables:
     *    name
     *    initials
     *    alias
     *    email
     */
    strcpy(pszQuery, "SELECT name, initials, alias, email FROM author");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check name */
                sprintf(pszQuery, "SELECT name FROM author WHERE "
                        "initials = '%s' OR alias = '%s' OR email = '%s'",
                        row1[0], row1[0], row1[0]);
                ulRc += CheckAuthorError(pszError, "name", row1[0], pszQuery);

                /* check initials */
                sprintf(pszQuery, "SELECT name FROM author WHERE "
                        "alias = '%s' OR email = '%s'",
                        row1[1], row1[1]);
                ulRc += CheckAuthorError(pszError, "initials", row1[1], pszQuery);

                /* alias */
                if (row1[2] != NULL)
                {
                    sprintf(pszQuery, "SELECT name FROM author WHERE "
                            "email = '%s'",
                            row1[2]);
                    ulRc += CheckAuthorError(pszError, "alias", row1[2], pszQuery);
                }
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    return ulRc;
}


/**
 * Checks for duplicate key and sql error for a given author key in the author table... (arg!)
 * @returns   Number of errors.
 * @param     pszError       Reference to error buffer pointer.
 * @param     pszFieldName   Key field name; used for logging.
 * @param     pszFieldValue  Key value; used for logging
 * @param     pszQuery       Query which is to be exectued to test for duplicate key.
 * @remark    Uses pszError[1] == '\xFE' to detect when to insert '\n\t'.
 */
static unsigned long CheckAuthorError(char * &pszError, const char *pszFieldName, const char *pszFieldValue, const char *pszQuery)
{
    MYSQL_ROW  row;
    MYSQL_RES *pres;
    unsigned long ulRc = 0;
    int rc;

    rc = mysql_query(pmysql, pszQuery);
    pres = mysql_store_result(pmysql);
    if (rc < 0 || (pres != NULL && mysql_num_rows(pres) != 0))
    {   /* some kind of error has occurred */
        if (pszError[1] == '\xFE')
        {
            strcat(pszError, "\n\t");
            pszError += 2;
        }

        if (rc < 0) /* sql error or 'duplicate key' */
        {
            sprintf(pszError, "author/%s: select failed - %s (sql=%s)",
                    pszFieldName, dbGetLastErrorDesc(), pszQuery);
        }
        else
        {   /* 'duplicate key' - print duplicates */
            sprintf(pszError, "author/%s: 'duplicate key', %s='%s': ",
                    pszFieldName, pszFieldValue, pszFieldName);

            while ((row = mysql_fetch_row(pres)) != NULL)
            {
                pszError += strlen(pszError);
                sprintf(pszError, "'%s' ", row[0]);
            }
        }

        pszError += strlen(pszError);
        pszError[1] = '\xFE';
        ulRc = 1;
    }
    if (pres != NULL)
        mysql_free_result(pres);

    return ulRc;
}


/**
 * Writes db error (rc<0) to the log buffer.
 * @returns   Number of signals.
 * @param     pszError   Reference to the error buffer pointer.
 * @param     pszQuery   Pointer to query which was executed.
 * @remark    Uses pszError[1] == '\xFE' to detect when to insert '\n\t'.
 */
static unsigned long logDbError(char * &pszError, const char *pszQuery)
{
    if (pszError[1] == '\xFE')
    {
        strcat(pszError, "\n\t");
        pszError += 2;
    }
    sprintf(pszError, "select failed: %s - (sql=%s)", dbGetLastErrorDesc(), pszQuery);

    pszError += strlen(pszError);
    pszError[1] = '\xFE';

    return 1;
}


/**
 * Executes a give query and returns a result identifier/pointer.
 * @returns   Query result identifier/pointer. NULL on error.
 * @param     pszQuery  Pointer to query.
 * @remark    Used by and designed for kHtmlPC.
 */
void * _System dbExecuteQuery(const char *pszQuery)
{
    assert(pmysql != NULL);
    if (mysql_query(pmysql, pszQuery) >= 0)
        return mysql_store_result(pmysql);

    return NULL;
}


/**
 * Asks for the number of rows in the result.
 * @returns   Number of rows in the result. -1 on error.
 * @param     pres  Query result identifier/pointer.
 * @remark    Used by and designed for kHtmlPC.
 */
signed long _System dbQueryResultRows(void *pres)
{
    if (pres == NULL)
        return -1;
    return mysql_num_rows((MYSQL_RES*)pres);
}


/**
 * Frees the storage allocated by the given result.
 * @returns   Success indicator, TRUE/FALSE.
 * @param     pres  Query result identifier/pointer.
 * @remark    Used by and designed for kHtmlPC.
 */
BOOL _System dbFreeResult(void *pres)
{
    if (pres != NULL)
        mysql_free_result((MYSQL_RES*)pres);
    else
        return FALSE;
    return TRUE;
}


/**
 * Fetch data from a result. Returns the data by calling the given callback function.
 * @returns   Success indicator, TRUE/FALSE.
 * @param     pres             Query result identifier/pointer.
 * @param     dbFetchCallBack  Callback-function.
 * @param     pvUser           User parameter which is passed onto dbFetchCallBack.
 * @remark    Used by and designed for kHtmlPC.
 */
BOOL _System dbFetch(void *pres, DBCALLBACKFETCH dbFetchCallBack, void *pvUser)
{
    BOOL fRc = FALSE;
    MYSQL_ROW row = mysql_fetch_row((MYSQL_RES*)pres);

    if (row)
    {
        MYSQL_FIELD *pField;
        int i = 0;
        mysql_field_seek((MYSQL_RES*)pres, 0);

        while ((pField = mysql_fetch_field((MYSQL_RES*)pres)) != NULL)
            if (dbFetchCallBack(row[i++], pField->name, pvUser) != 0)
                return FALSE;

        fRc = TRUE;
    }

    return fRc;
}


/**
 * Converts an ISO date to days after Christ, year 0.
 * @returns   days. -1 on error;
 * @param     pszDate  ISO Date.
 */
signed long _System dbDateToDaysAfterChrist(const char *pszDate)
{
    signed long lRet = -1;
    char szQuery[128];

    sprintf(&szQuery[0], "SELECT to_days('%s')", pszDate);
    if (mysql_query(pmysql, &szQuery[0]) >= 0)
    {
        MYSQL_ROW  row;
        MYSQL_RES *pres = mysql_use_result(pmysql);
        row = mysql_fetch_row(pres);
        if (row != NULL)
        {
            lRet = atol(row[0]);
            do { row = mysql_fetch_row(pres); } while (row != NULL);
        }
    }

    return lRet;
}


/**
 * Converts days after Christ (year 0) to ISO date.
 * @returns   Success indicator. TRUE/FALSE;
 * @param     lDays    Days after Christ (year 0).
 * @param     pszDate  ISO Date. Result.
 */
BOOL _System dbDaysAfterChristToDate(signed long lDays, char *pszDate)
{
    BOOL fRet = FALSE;
    char szQuery[128];

    if (lDays < 0)
        return FALSE;

    sprintf(&szQuery[0], "SELECT from_days(%ld)", lDays);
    if (mysql_query(pmysql, &szQuery[0]) >= 0)
    {
        MYSQL_ROW  row;
        MYSQL_RES *pres = mysql_use_result(pmysql);
        row = mysql_fetch_row(pres);
        if (row != NULL)
        {
            fRet = strlen(row[0]) == (4+1+2+1+2) && row[0][4] == '-' && row[0][7] == '-'
                && strcmp(row[0], "0000-00-00") != 0;
            if (fRet)
                strcpy(pszDate, row[0]);
            do { row = mysql_fetch_row(pres); } while (row != NULL);
        }
    }

    return fRet;
}


/**
 * Display all functions for, the given module, that is not updated.
 * @returns   TRUE / FALSE.
 * @param     lModule      Module reference number.
 * @param     dbFetchCall  Callback function which will be called once for each
 *                         field for all the functions not updated.
 *                         pvUser is NULL, pszValue field value, pszFieldName the field name.
 */
BOOL _System dbGetNotUpdatedFunction(signed long lModule, DBCALLBACKFETCH dbFetchCallBack)
{
    BOOL        fRet = FALSE;
    void       *pres;
    char        szQuery[256];

    /* not updated names */
    sprintf(&szQuery[0], "SELECT f1.name, f1.intname, f1.updated, f1.aliasfn, d.name, f2.name, f2.intname AS last "
                         "FROM function f1 LEFT OUTER JOIN function f2 ON f1.aliasfn = f2.refcode "
                         "     LEFT JOIN module d ON f2.module = d.refcode "
                         "WHERE f1.module = %ld AND f1.updated = 0",
            lModule);
    pres = dbExecuteQuery(szQuery);
    if (pres != NULL)
    {
        BOOL f;
        do
        {
            f = dbFetch(pres, dbFetchCallBack, NULL);
        } while (f);
        dbFreeResult(pres);
        fRet = TRUE;
    }

    /* warn about updated > 1 too */
    sprintf(&szQuery[0], "SELECT f1.name, f1.intname, f1.updated, f1.aliasfn, d.name, f2.name, f2.intname AS last "
                         "FROM function f1 LEFT OUTER JOIN function f2 ON f1.aliasfn = f2.refcode "
                         "     LEFT JOIN module d ON f2.module = d.refcode "
                         "WHERE f1.module = %ld AND f1.updated > 1",
            lModule);
    pres = dbExecuteQuery(szQuery);
    if (pres != NULL)
    {
        BOOL f;
        do
        {
            f = dbFetch(pres, dbFetchCallBack, NULL);
        } while (f);
        dbFreeResult(pres);
        fRet = TRUE;
    }

    strcpy(&szQuery[0], "UPDATE function SET updated = 0");
    mysql_query(pmysql, &szQuery[0]);

    return fRet;
}


/**
 * Counts the function for the given MODULE which has been updated.
 * @returns -1 on error, number of updated function on success.
 * @param   lModule     Module reference number.
 */
signed long _System dbGetNumberOfUpdatedFunction(signed long lModule)
{
    int         rc;
    char        szQuery[128];
    MYSQL_RES * pres;

    sprintf(&szQuery[0], "SELECT count(*) FROM function WHERE module = (%ld) AND updated > 0\n", lModule);
    rc   = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);
    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) == 1)
        rc = (int)getvalue(0, mysql_fetch_row(pres));
    else
        rc = -1;
    mysql_free_result(pres);
    return (signed long)rc;
}



/**
 * Clear the update flags for all file in a module/module.
 * @returns Success indicator. (TRUE / FALSE)
 * @param   lModule     Module refcode.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Intended for use by APIImport.
 */
BOOL             _System dbClearUpdateFlagFile(signed long lModule)
{
    int         rc;
    char        szQuery[128];

    sprintf(&szQuery[0],
            "UPDATE file SET updated = 0 WHERE module = (%ld)",
            lModule);
    rc = mysql_query(pmysql, &szQuery[0]);
    return rc == 0;
}


/**
 * Clear update flag
 * @returns Success indicator.
 * @param   lModule Module refcode.
 * @param   fAll    All module. If false only APIs and Internal APIs are cleared
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Intended for use by APIImport.
 */
BOOL             _System dbClearUpdateFlagFunction(signed long lModule, BOOL fAll)
{
    int         rc;
    char        szQuery[128];

    sprintf(&szQuery[0],
            "UPDATE function SET updated = 0 WHERE module = (%ld)",
            lModule);
    if (!fAll)
        strcat(&szQuery[0], " AND type IN ('A', 'I')");
    rc = mysql_query(pmysql, &szQuery[0]);
    return rc == 0;
}



/**
 * Deletes all the files in a module/module which was not found/updated.
 * @returns     Success indicator.
 * @param       lModule    Module refcode.
 * @sketch      Select all files which is to be deleted.
 *                  Set all references to each file in function to -1.
 *              Delete all files which is to be deleted.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      Use with GRATE CARE!
 */
BOOL             _System dbDeleteNotUpdatedFiles(signed long lModule)
{
    MYSQL_RES * pres;
    int         rc;
    BOOL        fRc = TRUE;
    char        szQuery[128];

    sprintf(&szQuery[0],
            "SELECT refcode FROM file WHERE module = (%ld) AND updated = 0",
            lModule);
    rc = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);
    if (pres != NULL && mysql_num_rows(pres))
    {
        MYSQL_ROW  row;
        while ((row = mysql_fetch_row(pres)) != NULL)
        {
            sprintf(&szQuery[0],
                    "UPDATE function SET file = -1 WHERE file = %s",
                    row[0]);
            rc = mysql_query(pmysql, &szQuery[0]);
            if (rc) fRc = FALSE;
        }
    }

    sprintf(&szQuery[0],
            "DELETE FROM file WHERE module = %ld AND updated = 0",
            lModule);
    rc = mysql_query(pmysql, &szQuery[0]);
    if (rc) fRc = FALSE;

    return fRc;
}


/**
 * Deletes all the functions which haven't been updated.
 * All rows in other tables which references the functions are
 * also delete.
 *
 * @returns     Success indicator. (TRUE / FALSE)
 * @param       lModule     The refcode of the module owning the functions.
 * @param       fAll        All function. If FALSE then only APIs and Internal APIs.
 * @sketch      Select all functions which wan't updated (ie. updated = 0 and module = lModule).
 *              If anyone Then
 *                  Delete the referenced to the functions in:
 *                      parameters
 *                      fnauthor
 *                  Delete all function which wasn't updated.
 *              EndIf
 * @remark      Use with GREATE CARE!
 */
BOOL             _System dbDeleteNotUpdatedFunctions(signed long lModule, BOOL fAll)
{
    MYSQL_RES * pres;
    int         rc;
    BOOL        fRc = TRUE;
    char        szQuery[128];

    sprintf(&szQuery[0],
            "SELECT refcode FROM function WHERE module = %ld AND updated = 0",
            lModule);
    if (!fAll)
        strcat(&szQuery[0], " AND type IN ('A', 'I')");
    rc = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);

    if (pres != NULL && mysql_num_rows(pres))
    {
        MYSQL_ROW  row;
        while ((row = mysql_fetch_row(pres)) != NULL)
        {
            /* delete parameters */
            sprintf(&szQuery[0], "DELETE FROM parameter WHERE function = %s", row[0]);
            rc = mysql_query(pmysql, &szQuery[0]);
            if (rc) fRc = FALSE;

            /* author relations */
            sprintf(&szQuery[0], "DELETE FROM fnauthor WHERE function = %s", row[0]);
            rc = mysql_query(pmysql, &szQuery[0]);
            if (rc) fRc = FALSE;
        }

        /*
         * Delete the functions only if above completed without errors.
         *
         * Deleting the functions before all the references has successfully be
         * deleted causes database corruption!
         */
        if (fRc)
        {
            sprintf(&szQuery[0],
                    "DELETE FROM function WHERE module = %ld AND updated = 0",
                    lModule);
            if (!fAll)
                strcat(&szQuery[0], " AND type IN ('A', 'I')");
            rc = mysql_query(pmysql, &szQuery[0]);
            if (rc) fRc = FALSE;
        }
    }

    return fRc;
}



/**
 * Appends a set of strings to a query. The main string (pszStr) is enclosed in "'"s.
 * @returns   Pointer to end of the string.
 * @param     pszQuery   Outputbuffer
 * @param     pszBefore  Text before string, might be NULL.
 * @param     pszStr     String (NOT NULL)
 * @param     pszAfter   Text after, might be NULL.
 * @status    completely implemented
 * @author    knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 */
static char *sqlstrcat(char *pszQuery, const char *pszBefore, const char *pszStr, const char *pszAfter)
{
    char *          pszLineStart = pszQuery;
    register char   ch;

    pszQuery += strlen(pszQuery);

    /*
     * String before
     */
    if (pszBefore != NULL)
    {
        strcpy(pszQuery, pszBefore);
        pszQuery += strlen(pszQuery);
    }

    /*
     * THE String
     */
    *pszQuery++ = '\'';
    while ((ch = *pszStr++) != '\0')
    {
        switch (ch)
        {
            case '\'':
                *pszQuery++ = '\\';
                *pszQuery++ = '\'';
                break;

            case '"':
                *pszQuery++ = '\\';
                *pszQuery++ = '"';
                break;

            case '\\':
                *pszQuery++ = '\\';
                *pszQuery++ = '\\';
                break;

            case '%':
                *pszQuery++ = '\\';
                *pszQuery++ = '%';
                break;

            case '_':
                *pszQuery++ = '\\';
                *pszQuery++ = '_';
                break;

            case '\n':
                *pszQuery++ = '\\';
                *pszQuery++ = 'r';
                *pszQuery++ = '\\';
                *pszQuery++ = 'n';
                break;

            case '\t':
                *pszQuery++ = '\\';
                *pszQuery++ = 't';
                break;

            case '\r':
                break;

            default:
                *pszQuery++ = ch;
        }

        /* Add new lines every 80 chars MySql don't like long lines. */
        if (pszLineStart - pszQuery > 80)
        {
            *pszQuery = '\n';
            pszLineStart = pszQuery;
        }
    }
    *pszQuery++ = '\'';

    /*
     * String after
     */
    if (pszAfter != NULL)
    {
        strcpy(pszQuery, pszAfter);
        pszQuery += strlen(pszQuery);
    }
    else
        *pszQuery = '\0';


    return pszQuery;
}


#ifndef DLL
/**
 * Signal handler.
 * Ensures that the database connection is closed at termination.
 * @param     sig  Signal number.
 */
void  dbHandler(int sig)
{
    if (pmysql != NULL)
    {
        fprintf(stderr, "\n\t!disconnecting from database!\n");
        dbDisconnect();
    }

    flushall();
    switch (sig)
    {
        case SIGBREAK:
            printf("\nSIGBREAK\n");
            exit(-1);
            break;
        case SIGINT:
            printf("\nSIGINT\n");
            exit(-1);
            break;
        case SIGTERM:
            printf("\nSIGTERM\n");
            exit(-1);
            break;
        case SIGSEGV:
            raise(sig);
            break;
        case SIGILL:
            printf("\nSIGILL\n");
            exit(-1);
            break;
    }
}


#else
/*******/
/* DLL */
/*******/
/* prototypes used in the _DLL_InitTerm function */
extern "C"
{
    int _CRT_init(void);
    void _CRT_term(void);
    void __ctordtorInit( void );
    void __ctordtorTerm( void );
    unsigned long _System _DLL_InitTerm(unsigned long hModule, unsigned long ulFlag);
}


/**
 * Dll InitTerm function.
 * @returns   0 on success.
 *            1 on error.
 * @param     hModule
 * @param     ulFlags
 * @remark    We'll ensure that the database connection is terminated as we terminate.
 */
unsigned long _System _DLL_InitTerm(unsigned long hModule, unsigned long ulFlag)
{
    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
        case 0:
            if (_CRT_init() == -1)
                return 0;
            __ctordtorInit();
            break;

        case 1:
            /* ensure that db connection is terminated */
            if (pmysql != NULL)
            {
                fprintf(stderr, "\n\t!disconnecting from database!\n");
                dbDisconnect();
            }
            __ctordtorTerm();
            break;

        default:
            return 0;
    }
    hModule = hModule;
    return 1;
}

/*****************************************************************/
/* -why is this terminate function referenced but not defined??? */
/*  and where is it referenced???                                */
/* -Probably an export missing from the libraries.               */
/*****************************************************************/
void terminate(void)
{
    DosPutMessage(0, sizeof("terminate")-1, "terminate");
    exit(-1);
}

/****************************************/
/* EMX run-time trouble                 */
/* _environ is missing when using -Zomf */
/****************************************/
char **_environ = environ;

#endif
