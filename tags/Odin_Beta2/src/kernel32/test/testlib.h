/* $Id: testlib.h,v 1.1 1999-11-28 23:10:13 bird Exp $
 *
 * General test helpers and structures.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _TESTLIB_H_
#define _TESTLIB_H_

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
/*
 * LastError macros
 */
#define TSTCHECKLASTERROR(errorno) \
        (GetLastError() != (errorno) \
         ? TstError(__FILE__, __LINE__, "LastError %d != %d", \
                    GetLastError(), (errorno)) \
         : (void)0)

#define TSTINFOLASTERROR() \
        TstInfo(__FILE__, __LINE__, "LastError %d", GetLastError())

/*
 * Assert like macros...
 */
#define TSTCHECK(expr) \
        (!(expr) \
        ? TstError(__FILE__, __LINE__, #expr) \
        : (void)0)

#define TSTCHECK1D(expr,d1) \
        (!(expr) \
        ? TstError(__FILE__, __LINE__, #expr "; "#d1"=%d", d1) \
        : (void)0)

#define TSTCHECK1S(expr,s1) \
        (!(expr) \
        ? TstError(__FILE__, __LINE__, #expr "; "#s1"='%s'", s1) \
        : (void)0)

#define TSTCHECK2D(expr,d1,d2) \
        (!(expr) \
        ? TstError(__FILE__, __LINE__, #expr "; "#d1"=%d, "#d2"=%d", d1,d2) \
        : (void)0)


#define TSTCHECKW(expr) \
        (!(expr) \
        ? TstWarning(__FILE__, __LINE__, #expr) \
        : (void)0)

#define TSTCHECKW1D(expr,d1) \
        (!(expr) \
        ? TstWarning(__FILE__, __LINE__, #expr "; "#d1"=%d", d1) \
        : (void)0)

#define TSTCHECKW1S(expr,s1) \
        (!(expr) \
        ? TstWarning(__FILE__, __LINE__, #expr "; "#s1"='%s'", s1) \
        : (void)0)

#define TSTCHECKW2D(expr,d1,d2) \
        (!(expr) \
        ? TstWarning(__FILE__, __LINE__, #expr "; "#d1"=%d, "#d2"=%d", d1, d2) \
        : (void)0)


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
/*
 * Returns a boolean: TRUE no errors or warnings, FALSE warnings and/or errors.
 *                    (informational only)
 */
typedef void (* PTESTFUNC)(void);


/*
 * An entry in a test table.
 */
typedef struct _TestEntry
{
    PTESTFUNC   pfnTest;                /* Pointer to test function. */
    char *      pszName;                /* Pointer to string containing a name for the test. (not too long, please.) */
    unsigned    cErrors;                /* Number of errors registered in this test. */
    unsigned    cWarnings;              /* Number of warnings registered in this test. */
} TESTENTRY, *PTESTENTRY;

/*
 * Test table
 */
typedef struct _TestTable
{
    char *      pszTableDescription;    /* Pointer to string describing the this testtable. */
    unsigned    cErrors;                /* Number of errors accumulated. */
    unsigned    cWarnings;              /* Number of warnings accumulated. */
    PTESTENTRY  paTest;                 /* Pointer to array of TestEntries. */
    unsigned    uLevel;                 /* !internal! Nesting of testtables in processing */
    int         fPrinted;               /* !internal! TRUE: description is printed. FALSE: not printed. */
    int         iCurrentTest;           /* !internal! Index of the entry currently being executed. */
} TESTTABLE, *PTESTTABLE;


/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
void    TstProcessTestTable(PTESTTABLE pTestTable);
void    TstError(const char *pszFile, unsigned uLine, const char *pszFormat, ...);
void    TstWarning(const char *pszFile, unsigned uLine, const char *pszFormat, ...);
void    TstInfo(const char *pszFormat, ...);
void    TstInfo(const char *pszFile, unsigned uLine, const char *pszFormat, ...);

#endif
