/* $Id: db.cpp,v 1.4 2000-02-10 22:10:40 bird Exp $ */
/*
 * DB - contains all database routines.
 *
 * Copyright (c) 1999 knut st. osmundsen
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
        mysql_num_rows(pres2) <= 0)    \
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
#ifdef NOTDLL
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
    #ifdef NOTDLL
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
 * Checks if dll exists. If not exists the dll is inserted.
 * @returns   Dll refcode. -1 on errors.
 * @param     pszDll  Dll name.
 * @remark    This search must be case insensitive.
 *            (In the mysql-world everything is case insensitive!)
 */
signed short _System dbCheckInsertDll(const char *pszDll)
{
    int  rc;
    char szQuery[256];
    MYSQL_RES *pres;

    /* try find match */
    sprintf(&szQuery[0], "SELECT refcode, name FROM dll WHERE name = '%s'\n", pszDll);
    rc   = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);

    /* not found? - insert dll */
    if (rc < 0 || pres == NULL || mysql_num_rows(pres) == 0)
    {
        mysql_free_result(pres);

        sprintf(&szQuery[0], "INSERT INTO dll(name) VALUES('%s')\n", pszDll);
        rc = mysql_query(pmysql, &szQuery[0]);

        /* select row to get refcode */
        sprintf(&szQuery[0], "SELECT refcode, name FROM dll WHERE name = '%s'\n", pszDll);
        rc   = mysql_query(pmysql, &szQuery[0]);
        pres = mysql_store_result(pmysql);
    }

    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) == 1)
        rc = (int)getvalue(0, mysql_fetch_row(pres));
    else
        rc = -1;
    mysql_free_result(pres);

    return (short)rc;
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

    return (short)rc;
}


/**
 * Updates or inserts a function name into the database.
 * @returns   Success indicator. TRUE / FALSE.
 * @param     usDll           Dll refcode.
 * @param     pszFunction     Functionname.
 * @param     ulOrdinal       Ordinal value.
 * @param     fIgnoreOrdinal  Do not update ordinal value.
 */
BOOL _System dbInsertUpdateFunction(unsigned short usDll, const char *pszFunction,
                                    unsigned long ulOrdinal, BOOL fIgnoreOrdinal)
{
    int  rc;
    long lFunction = -1;
    char szQuery[256];
    MYSQL_RES *pres;

    /* try find function */
    sprintf(&szQuery[0], "SELECT refcode FROM function WHERE dll = %d AND name = '%s'", usDll, pszFunction);
    rc = mysql_query(pmysql, &szQuery[0]);
    pres = mysql_store_result(pmysql);
    if (rc >= 0 && pres != NULL && mysql_num_rows(pres) > 0)
    {   /* update function (function is found) */
        MYSQL_ROW parow;
        if (mysql_num_rows(pres) > 1)
        {
            fprintf(stderr, "internal database integrity error(%s): More function by the same name for the same dll. "
                    "usDll = %d, pszFunction = %s\n", __FUNCTION__, usDll, pszFunction);
            return FALSE;
        }

        parow = mysql_fetch_row(pres);
        if (parow != NULL)
            lFunction = getvalue(0, parow);
        mysql_free_result(pres);

        if (!fIgnoreOrdinal)
        {
            sprintf(&szQuery[0], "UPDATE function SET ordinal = %ld WHERE refcode = %ld",
                    ulOrdinal, lFunction);
            rc = mysql_query(pmysql, &szQuery[0]);
        }
    }
    else
    {   /* insert */
        sprintf(&szQuery[0], "INSERT INTO function(dll, name, ordinal) VALUES(%d, '%s', %ld)",
                usDll, pszFunction, ulOrdinal);
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


/**
 * Find occurences of a function, given by name.
 * @returns   success indicator, TRUE / FALSE.
 * @param     pszFunctionName
 * @param     pFnFindBuf
 */
BOOL _System dbFindFunction(const char *pszFunctionName, PFNFINDBUF pFnFindBuf)
{
    MYSQL_RES   *pres;
    MYSQL_ROW    row;
    int          rc;
    char         szQuery[256];

    sprintf(&szQuery[0], "SELECT refcode, dll FROM function WHERE name = '%s'", pszFunctionName);

    rc = mysql_query(pmysql, &szQuery[0]);
    if (rc >= 0)
    {
        pres = mysql_store_result(pmysql);
        if (pres != NULL)
        {
            pFnFindBuf->cFns = 0;
            while ((row = mysql_fetch_row(pres)) != NULL)
            {
                pFnFindBuf->alRefCode[pFnFindBuf->cFns] = atol(row[0]);
                pFnFindBuf->alDllRefCode[pFnFindBuf->cFns] = atol(row[1]);

                /* next */
                pFnFindBuf->cFns++;
            }
            mysql_free_result(pres);
        }
        else
            rc = -1;
    }

    return rc >= 0;
}


/**
 * Finds the refcode for an author, if the author exists.
 * @returns   Author 'refcode'.
 * @param     pszAuthor  String which holds the identifier of an author.
 *                       This doesn't have to be the name. Initials, alias and email
 *                       is also searched.
 */
signed long _System dbFindAuthor(const char *pszAuthor)
{
    signed long refcode = -1;
    MYSQL_RES *pres;
    char szQuery[256];

    sprintf(&szQuery[0],
            "SELECT refcode FROM author "
            "WHERE name     = '%s' OR "
            "      initials = '%s' OR "
            "      alias    = '%s' OR "
            "      email    = '%s'",
            pszAuthor, pszAuthor, pszAuthor, pszAuthor);
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


/**
 * Updates function information.
 * @returns   number of errors.
 * @param     pFnDesc
 * @param     pszError
 * @result    on error(s) pszError will hold information about the error(s).
 */
unsigned long _System dbUpdateFunction(PFNDESC pFnDesc, char *pszError)
{
    char  szQuery[256];
    char *pszQuery = &szQuery[0];
    long  lCurrentState;
    int   i,rc;
    unsigned long ulRc = 0;
    BOOL          f = FALSE;

    lCurrentState = dbGetFunctionState(pFnDesc->lRefCode);
    if (lCurrentState == -1)
    {
        strcpy(pszError, dbGetLastErrorDesc());
        return 1;
    }

    /* update function table first */
    strcpy(pszQuery, "UPDATE function SET ");
    pszQuery += strlen(pszQuery);
    if (pFnDesc->lStatus != 99 || lCurrentState == 0)
    {
        sprintf(pszQuery, "state = %ld ", pFnDesc->lStatus);
        f = TRUE;
    }
    pszQuery += strlen(pszQuery);

    if (pFnDesc->pszReturnType != NULL)
    {
        if (f)
        {
            strcat(pszQuery, ", ");
            pszQuery += strlen(pszQuery);
        }
        sprintf(pszQuery, "return = '%s' ", pFnDesc->pszReturnType);
        pszQuery += strlen(pszQuery);
        f = TRUE;
    }

    if (f)
    {
        sprintf(pszQuery, "WHERE refcode = %ld", pFnDesc->lRefCode);
        rc = mysql_query(pmysql, &szQuery[0]);
        if (rc < 0)
        {
            sprintf(pszError, "Updating functiontable failed with error: %s - (sql=%s) ",
                    dbGetLastErrorDesc(), &szQuery[0]);
            pszError += strlen(pszError) - 1;
            ulRc++;
        }
    }

    /* parameters */
    pszQuery = &szQuery[0];
    sprintf(pszQuery, "DELETE FROM parameter WHERE function = %ld", pFnDesc->lRefCode);
    rc = mysql_query(pmysql, &szQuery[0]);
    if (rc < 0)
    {
        if (*pszError == ' ')
            strcpy(pszError++, "\n\t");
        sprintf(pszError, "Deleting old parameters failed with error: %s - (sql=%s) ",
                dbGetLastErrorDesc(), &szQuery[0]);
        pszError += strlen(pszError) - 1;
        ulRc++;
    }

    for (i = 0; i < pFnDesc->cParams; i++)
    {
        sprintf(pszQuery, "INSERT INTO parameter(function, sequencenbr, type, name) "
                "VALUES (%ld, %d, '%s', '%s')",
                pFnDesc->lRefCode, i,
                pFnDesc->apszParamType[i] != NULL ? pFnDesc->apszParamType[i] : "",
                pFnDesc->apszParamName[i] != NULL ? pFnDesc->apszParamName[i] : ""
                );
        rc = mysql_query(pmysql, pszQuery);
        if (rc < 0)
        {
            if (*pszError == ' ')
                strcpy(pszError++, "\n\t");
            sprintf(pszError, "Inserting parameter %i failed with error: %s - (sql=%s) ",
                    i, dbGetLastErrorDesc(), &szQuery[0]);
            pszError += strlen(pszError) - 1;
            ulRc++;
        }
    }

    /* authors */
    pszQuery = &szQuery[0];
    sprintf(pszQuery, "DELETE FROM fnauthor WHERE function = %ld", pFnDesc->lRefCode);
    rc = mysql_query(pmysql, &szQuery[0]);
    if (rc < 0)
    {
        if (*pszError == ' ')
            strcpy(pszError++, "\n\t");
        sprintf(pszError, "Deleting old authors failed with error: %s - (sql=%s) ",
                dbGetLastErrorDesc(), &szQuery[0]);
        pszError += strlen(pszError) - 1;
        ulRc++;
    }

    for (i = 0; i < pFnDesc->cAuthors; i++)
    {
        if (pFnDesc->alAuthorRefCode[i] == -1)
            continue;
        sprintf(pszQuery, "INSERT INTO fnauthor(author, function) "
                "VALUES (%ld, %ld)",
                pFnDesc->alAuthorRefCode[i], pFnDesc->lRefCode);
        rc = mysql_query(pmysql, pszQuery);
        if (rc < 0)
        {
            if (*pszError == ' ')
                strcpy(pszError++, "\n\t");
            sprintf(pszError, "Inserting parameter %i failed with error: %s - (sql=%s) ",
                    i, dbGetLastErrorDesc(), &szQuery[0]);
            pszError += strlen(pszError) - 1;
            ulRc++;
        }
    }

    return ulRc;
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
            sprintf(pszQuery, "DELETE FROM historydll WHERE date = '%s'", &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "DELETE FROM historyapigroup WHERE date = '%s'", &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "DELETE FROM historydlltotal WHERE date = '%s'", &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "DELETE FROM historyapigrouptotal WHERE date = '%s'", &szCurDt[0]);
            CheckLogContinue((pszError, "error removing old history rows: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            /* insert new stats */
            sprintf(pszQuery, "INSERT INTO historydll(dll, state, date, count) "
                    "SELECT dll, state, '%s', count(*) FROM function GROUP BY dll, state",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "INSERT INTO historyapigroup(apigroup, state, date, count) "
                    "SELECT dll, state, '%s', count(*) FROM function WHERE apigroup IS NOT NULL "
                    "GROUP BY apigroup, state",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            /* inserting new totals */
            sprintf(pszQuery, "INSERT INTO historydlltotal(dll, date, totalcount) "
                    "SELECT dll, '%s', count(*) FROM function GROUP BY dll",
                    &szCurDt[0]);
            rc = mysql_query(pmysql, pszQuery);
            CheckLogContinue((pszError, "error inserting: %s - (sql=%s) ", dbGetLastErrorDesc(), pszQuery));

            sprintf(pszQuery, "INSERT INTO historyapigrouptotal(apigroup, date, totalcount) "
                    "SELECT apigroup, '%s', count(*) FROM function WHERE apigroup IS NOT NULL "
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
    strcpy(pszQuery, "SELECT refcode, dll, state, apigroup FROM function");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check dll */
                sprintf(pszQuery, "SELECT name FROM dll WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("function/dll", "Foreign key 'dll' not found in the dll table");

                /* check state */
                sprintf(pszQuery, "SELECT name FROM state WHERE refcode = %s", row1[2]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("function/state", "Foreign key 'state' not found in the state table");

                /* check apigroup */
                if (row1[3] != NULL)
                {
                    sprintf(pszQuery, "SELECT name FROM apigroup WHERE refcode = %s", row1[3]);
                    rc = mysql_query(pmysql, pszQuery);
                    CheckFKError("function/state", "Foreign key 'state' not found in the state table");
                }
            }
            mysql_free_result(pres1);
        }
    }
    else
        ulRc += logDbError(pszError, pszQuery);

    /* foreign keys in apigroup */
    strcpy(pszQuery, "SELECT refcode, dll FROM apigroup");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check dll */
                sprintf(pszQuery, "SELECT refcode FROM dll WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("apigroup/dll", "Foreign key 'dll' not found in the dll table");
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

    /* foreign keys in historydll table */
    strcpy(pszQuery, "SELECT date, dll, state FROM historydll");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check dll */
                sprintf(pszQuery, "SELECT refcode FROM dll WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historydll/dll", "Foreign key 'dll' not found in the dll table");

                /* check state */
                sprintf(pszQuery, "SELECT refcode FROM state WHERE refcode = %s", row1[2]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historydll/state", "Foreign key 'state' not found in the state table");
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
                /* check dll */
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

    /* foreign keys in historydlltotal table */
    strcpy(pszQuery, "SELECT date, dll FROM historydlltotal");
    rc = mysql_query(pmysql, pszQuery);
    if (rc >= 0)
    {
        pres1 = mysql_store_result(pmysql);
        if (pres1 != NULL)
        {
            while ((row1 = mysql_fetch_row(pres1)) != NULL)
            {
                /* check dll */
                sprintf(pszQuery, "SELECT refcode FROM dll WHERE refcode = %s", row1[1]);
                rc = mysql_query(pmysql, pszQuery);
                CheckFKError("historydlltotal/dll", "Foreign key 'dll' not found in the dll table");
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
                /* check dll */
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
    if (rc < 0 || (pres != NULL && mysql_num_rows(pres) > 0))
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


#ifdef NOTDLL
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
