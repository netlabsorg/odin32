/* $Id: testlib.cpp,v 1.1 1999-11-28 23:10:12 bird Exp $
 *
 * General test helpers.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INDENT 4


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "testlib.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static unsigned     cTestTables = 0;
static PTESTTABLE   apTestTables[100] = {0};


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void printHeadersToCurrentLevel();
static void printAtCurrentLevel(const char *pszText, int iDelta = 0);


/**
 * Registers and prints an error message.
 * @param     pszFile    Source filename - __FILE__.
 * @param     uLine      Source code line - __LINE__.
 * @param     pszFormat  Pointer to a message (printf styled) string
 * @param     ...        Additional parameters.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
void TstError(const char *pszFile, unsigned uLine, const char *pszFormat, ...)
{
    va_list arg;
    char szOutput[0x1000];
    int i;

    /* error header */
    i = sprintf(szOutput, "%s - Error(%s:%d): ",
                apTestTables[cTestTables-1]->paTest[apTestTables[cTestTables-1]->iCurrentTest].pszName,
                pszFile, uLine
                );

    /* format message */
    va_start(arg, pszFormat);
    i += vsprintf(&szOutput[i], pszFormat, arg);
    va_end(arg);

    if (szOutput[i-1] != '\n')
    {
        szOutput[i++] = '\n';
        szOutput[i] = '\0';
    }

    printHeadersToCurrentLevel();
    printAtCurrentLevel(szOutput);

    /* update statistics */
    apTestTables[cTestTables-1]->paTest[apTestTables[cTestTables-1]->iCurrentTest].cErrors++;
}


/**
 * Registers and prints a warning message.
 * @param     pszFile    Source filename - __FILE__.
 * @param     uLine      Source code line - __LINE__.
 * @param     pszFormat  Pointer to a message (printf styled) string
 * @param     ...        Additional parameters.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
void TstWarning(const char *pszFile, unsigned uLine, const char *pszFormat, ...)
{
    va_list arg;
    char szOutput[0x1000];
    int i;

    /* error header */
    i = sprintf(szOutput, "%s - Warning(%s:%d): ",
                apTestTables[cTestTables-1]->paTest[apTestTables[cTestTables-1]->iCurrentTest].pszName,
                pszFile, uLine
                );

    /* format message */
    va_start(arg, pszFormat);
    i += vsprintf(&szOutput[i], pszFormat, arg);
    va_end(arg);

    if (szOutput[i-1] != '\n')
    {
        szOutput[i++] = '\n';
        szOutput[i] = '\0';
    }

    printHeadersToCurrentLevel();
    printAtCurrentLevel(szOutput);

    /* update statistics */
    apTestTables[cTestTables-1]->paTest[apTestTables[cTestTables-1]->iCurrentTest].cWarnings++;
}


/**
 * Prints an informational message.
 * @param     pszFormat  Pointer to a message (printf styled) string
 * @param     ...        Additional parameters.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
void    TstInfo(const char *pszFormat, ...)
{
    va_list arg;
    char szOutput[0x1000];
    int i;

    /* error header */
    i = sprintf(szOutput, "%s - Info: ",
                apTestTables[cTestTables-1]->paTest[apTestTables[cTestTables-1]->iCurrentTest].pszName);

    /* format message */
    va_start(arg, pszFormat);
    i += vsprintf(&szOutput[i], pszFormat, arg);
    va_end(arg);

    if (szOutput[i-1] != '\n')
    {
        szOutput[i++] = '\n';
        szOutput[i] = '\0';
    }

    printHeadersToCurrentLevel();
    printAtCurrentLevel(szOutput);
}


/**
 * Prints an informational message.
 * @param     pszFile    Source filename - __FILE__.
 * @param     uLine      Source code line - __LINE__.
 * @param     pszFormat  Pointer to a message (printf styled) string
 * @param     ...        Additional parameters.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 * @remark    Later this message type may be filtered out by a commandline option.
 */
void    TstInfo(const char *pszFile, unsigned uLine, const char *pszFormat, ...)
{
    va_list arg;
    char szOutput[0x1000];
    int i;

    /* error header */
    i = sprintf(szOutput, "%s - Info(%s:%d): ",
                apTestTables[cTestTables-1]->paTest[apTestTables[cTestTables-1]->iCurrentTest].pszName,
                pszFile, uLine
                );

    /* format message */
    va_start(arg, pszFormat);
    i += vsprintf(&szOutput[i], pszFormat, arg);
    va_end(arg);

    if (szOutput[i-1] != '\n')
    {
        szOutput[i++] = '\n';
        szOutput[i] = '\0';
    }

    printHeadersToCurrentLevel();
    printAtCurrentLevel(szOutput);
}


/**
 * This functions processes a testtable. (A test table is a set of tests.)
 * @param     pTestTable  Pointer to a testtable struct.
 * @sketch    Initiate internal datamembers in the testtable struct and add it to the array.
 *
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
void TstProcessTestTable(PTESTTABLE pTestTable)
{
    assert(pTestTable != NULL);

    /* init and insert */
    pTestTable->fPrinted = 0;
    pTestTable->uLevel = cTestTables;
    pTestTable->cErrors = 0;
    pTestTable->cWarnings = 0;
    pTestTable->iCurrentTest = 0;
    apTestTables[cTestTables++] = pTestTable;

    /* execute tests */
    for (; pTestTable->paTest[pTestTable->iCurrentTest].pfnTest != NULL; pTestTable->iCurrentTest++)
    {
        pTestTable->paTest[pTestTable->iCurrentTest].cErrors = 0;
        pTestTable->paTest[pTestTable->iCurrentTest].cWarnings = 0;

        pTestTable->paTest[pTestTable->iCurrentTest].pfnTest();
        if (pTestTable->paTest[pTestTable->iCurrentTest].cErrors > 0
            || pTestTable->paTest[pTestTable->iCurrentTest].cWarnings > 0)
        {
            pTestTable->cErrors += pTestTable->paTest[pTestTable->iCurrentTest].cErrors;
            pTestTable->cWarnings += pTestTable->paTest[pTestTable->iCurrentTest].cWarnings;
        }
    }

    /* if errors or warnings print summary. */
    if (pTestTable->cErrors > 0 || pTestTable->cWarnings > 0 || cTestTables == 1)
    {
        char szText[128];
        sprintf(szText, "--- Summary ---\n"
                        "Errors   %2d\n"
                        "Warnings %2d\n",
                pTestTable->cErrors,
                pTestTable->cWarnings
                );
        printAtCurrentLevel(szText, -1);
    }

    /* update parent test if any */
    if (cTestTables > 1)
    {
        apTestTables[cTestTables - 2]->cErrors += pTestTable->cErrors;
        apTestTables[cTestTables - 2]->cWarnings += pTestTable->cWarnings;
    }

    /* remove */
    apTestTables[cTestTables--] = NULL;
}


/**
 * Prints unprinted TestTable headers up to the current level.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static void printHeadersToCurrentLevel()
{
    int i;
    for (i = 0; i < cTestTables; i++)
        if (!apTestTables[i]->fPrinted)
        {
            printf("%*s%s\n", INDENT * apTestTables[i]->uLevel, "", apTestTables[i]->pszTableDescription);
            apTestTables[i]->fPrinted = 1;
        }
}


/**
 * Indents the specified textstring to the current level.
 * @param     pszText  Pointer to text.
 * @param     iDelta   Level modifier.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static void printAtCurrentLevel(const char *pszText, int iDelta)
{
    int i;

    for (i = 0; pszText[i] != '\0'; i++)
    {
        int j = 0;

        while (pszText[i+j] != '\n' && pszText[i+j+1] != '\0')
            j++;

        printf("%*s%.*s\n",
               INDENT * (cTestTables + iDelta), "",
               j, &pszText[i]);

        i += j;
    }
}

