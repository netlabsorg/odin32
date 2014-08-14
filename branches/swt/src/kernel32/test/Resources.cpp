/* $Id: Resources.cpp,v 1.1 1999-11-28 23:10:06 bird Exp $
 *
 * Kernel32 Resource functions tests.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#ifdef __WATCOMC__
    #define ENUMRESNAMEPROCW ENUMRESNAMEPROC
    #define ENUMRESNAMEPROCA ENUMRESNAMEPROC
    #define RT_MENUW  ((LPCWSTR)RT_MENU)
    #define RT_MENUA  ((LPCTSTR)RT_MENU)
    #define RT_DIALOGW  ((LPCWSTR)RT_DIALOG)
    #define RT_DIALOGA  ((LPCTSTR)RT_DIALOG)
#endif

#define MY_LPARAM 0x12345678

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#ifndef __WATCOMC__
    #include <odin.h>
    #include <odinwrap.h>
#endif

#include <windows.h>
#include "kernel32test.h"
#include "rsrc.h"
#include "testlib.h"



/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void PosEnumResourceNamesW1(void);
static void PosEnumResourceNamesW2(void);
static void PosEnumResourceNamesW3(void);
static CALLBACK PosEnumNamesCallbackW1(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam);
static CALLBACK PosEnumNamesCallbackW2(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam);
static CALLBACK PosEnumNamesCallbackW3(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam);

static void NegEnumResourceNamesW1(void);
static void NegEnumResourceNamesW2(void);
static void NegEnumResourceNamesW3(void);
static void NegEnumResourceNamesW4(void);
static void NegEnumResourceNamesW5(void);
static CALLBACK NegEnumNamesCallbackW1(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam);
static CALLBACK NegEnumNamesCallbackW2(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam);
static CALLBACK NegEnumNamesCallbackW5(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam);

static void PosEnumResourceNamesA1(void);
static void PosEnumResourceNamesA2(void);
static void PosEnumResourceNamesA3(void);
static CALLBACK PosEnumNamesCallbackA1(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam);
static CALLBACK PosEnumNamesCallbackA2(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam);
static CALLBACK PosEnumNamesCallbackA3(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam);

static void NegEnumResourceNamesA1(void);
static void NegEnumResourceNamesA2(void);
static void NegEnumResourceNamesA3(void);
static void NegEnumResourceNamesA4(void);
static void NegEnumResourceNamesA5(void);
static CALLBACK NegEnumNamesCallbackA1(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam);
static CALLBACK NegEnumNamesCallbackA2(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam);
static CALLBACK NegEnumNamesCallbackA5(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam);


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static TESTENTRY aRsrcTests[] =
{ /* pfnTest                    pszName */
    {PosEnumResourceNamesW1,    "EnumResourceNamesW1+ ", 0, 0},
    {PosEnumResourceNamesW2,    "EnumResourceNamesW2+ false callback return", 0, 0},
    {PosEnumResourceNamesW3,    "EnumResourceNamesW3+ callback return > TRUE", 0, 0},
    {NegEnumResourceNamesW1,    "EnumResourceNamesW1- hmod", 0, 0},
    {NegEnumResourceNamesW2,    "EnumResourceNamesW2- type", 0, 0},
    {NegEnumResourceNamesW3,    "EnumResourceNamesW3- type pointer", 0, 0},
    {NegEnumResourceNamesW4,    "EnumResourceNamesW4- callback",    0, 0},
    {NegEnumResourceNamesW5,    "EnumResourceNamesW5- string standard type", 0, 0},

    {PosEnumResourceNamesA1,    "EnumResourceNamesA1+ ", 0, 0},
    {PosEnumResourceNamesA2,    "EnumResourceNamesA2+ false callback return", 0, 0},
    {PosEnumResourceNamesA3,    "EnumResourceNamesA3+ callback return > TRUE", 0, 0},
    {NegEnumResourceNamesA1,    "EnumResourceNamesA1- hmod", 0, 0},
    {NegEnumResourceNamesA2,    "EnumResourceNamesA2- type", 0, 0},
    {NegEnumResourceNamesA3,    "EnumResourceNamesA3- type pointer", 0, 0},
    {NegEnumResourceNamesA4,    "EnumResourceNamesA4- callback",    0, 0},
    {NegEnumResourceNamesA5,    "EnumResourceNamesA5- string standard type", 0, 0},

    {NULL,                      NULL, 0, 0} /* terminator entry */
};

static TESTTABLE RsrcTestTable =
{
    "ResourceTests",
    0,
    0,
    &aRsrcTests[0],
    -1,
    -1,
    -1,
};

static int iPosW1 = 0;
static int iPosW2 = 0;
static int iPosA1 = 0;
static int iPosA2 = 0;



/**
 * Tests resource functions.
 * @returns
 * @status    stub
 * @author    knut st. osmundsen
 * @remark
 */
void TestResources(void)
{
    TstProcessTestTable(&RsrcTestTable);
}






/*******************************************************************************
*                                                                              *
*  EnumResourceNamesW  EnumResourceNamesW  EnumResourceNamesW                  *
*  EnumResourceNamesW  EnumResourceNamesW  EnumResourceNamesW                  *
*  EnumResourceNamesW  EnumResourceNamesW  EnumResourceNamesW                  *
*  EnumResourceNamesW  EnumResourceNamesW  EnumResourceNamesW                  *
*  EnumResourceNamesW  EnumResourceNamesW  EnumResourceNamesW                  *
*                                                                              *
*******************************************************************************/

/**
 * Positive - This function tests the EnumResourceNamesW function.
 */
static void PosEnumResourceNamesW1(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW(0, RT_MENUW,
                              (ENUMRESNAMEPROCW)PosEnumNamesCallbackW1,
                              MY_LPARAM);
    if (!fRet)
    {
        TstError(__FILE__, __LINE__, "failed with fRet=%d and LastError=%d\n", fRet, GetLastError());
    }
    TSTCHECKLASTERROR(NO_ERROR);
    TSTCHECK1D(iPosW1 == 4, iPosW1);
}
static CALLBACK PosEnumNamesCallbackW1(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam)
{
    static LPCWSTR aiRes[] = {(LPCWSTR)IDM_1, (LPCWSTR)IDM_2, (LPCWSTR)IDM_3, (LPCWSTR)IDM_4};

    TSTCHECK(hmod == 0);
    TSTCHECK(lpszType == RT_MENUW);
    TSTCHECK(lParam == MY_LPARAM);
    TSTCHECK(aiRes[iPosW1] == lpszName);
    iPosW1++;
    return TRUE;
}


/**
 * Positive - This function tests the EnumResourceNamesW, negative callback return.
 */
static void PosEnumResourceNamesW2(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW((HMODULE)0, RT_MENUW,
                              (ENUMRESNAMEPROCW)PosEnumNamesCallbackW2,
                              MY_LPARAM);
    if (fRet)
        TstError(__FILE__, __LINE__, "didn't fail, fRet=%d and LastError=%d\n", fRet, GetLastError());

    TSTINFOLASTERROR();
    TSTCHECKLASTERROR(NO_ERROR);
    TSTCHECK1D(iPosW2 == 2, iPosW2);
}
static CALLBACK PosEnumNamesCallbackW2(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam)
{
    static LPCWSTR aiRes[] = {(LPCWSTR)IDM_1, (LPCWSTR)IDM_2, (LPCWSTR)IDM_3, (LPCWSTR)IDM_4};

    TSTCHECK(hmod == (HMODULE)0);
    TSTCHECK(lpszType == RT_MENUW);
    TSTCHECK(lParam == MY_LPARAM);
    TSTCHECK(aiRes[iPosW2] == lpszName);
    iPosW2++;
    return iPosW2 < 2 ? TRUE : FALSE;
}


/**
 * Positive - This function tests EnumResourceNamesW return value
 */
static void PosEnumResourceNamesW3(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW((HMODULE)0, RT_MENUW,
                              (ENUMRESNAMEPROCW)PosEnumNamesCallbackW3,
                              MY_LPARAM);
    if (fRet != 1)
        TstError(__FILE__, __LINE__, "invalid return, fRet=%d and LastError=%d\n", fRet, GetLastError());

    TSTCHECKLASTERROR(NO_ERROR);
}
static CALLBACK PosEnumNamesCallbackW3(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam)
{

    TSTCHECK(hmod == (HMODULE)0);
    TSTCHECK(lpszType == RT_MENUW);
    TSTCHECK(lParam == MY_LPARAM);
    lpszName = lpszName;
    return 0xBEEF0000;
}


/**
 * Negative - invalid handle
 */
static void NegEnumResourceNamesW1(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW((HMODULE)0xBEEFBEEF, RT_MENUW,
                              (ENUMRESNAMEPROCW)NegEnumNamesCallbackW1,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");
    TSTCHECKLASTERROR(ERROR_RESOURCE_DATA_NOT_FOUND);
}
static CALLBACK NegEnumNamesCallbackW1(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam)
{
    hmod = hmod; lpszType = lpszType; lpszName = lpszName; lParam = lParam;
    return TRUE;
}


/*
 * invalid resource type
 */
static void NegEnumResourceNamesW2(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW(0, RT_DIALOGW, /* no dialogs resources */
                              (ENUMRESNAMEPROCW)NegEnumNamesCallbackW2,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");
    TSTCHECKLASTERROR(ERROR_RESOURCE_TYPE_NOT_FOUND);
}
static CALLBACK NegEnumNamesCallbackW2(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam)
{
    hmod = hmod; lpszType = lpszType; lpszName = lpszName; lParam = lParam;
    return TRUE;
}


/*
 * invalid lpszType pointer
 */
static void NegEnumResourceNamesW3(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW(0, (LPCWSTR)0x80000000,
                              (ENUMRESNAMEPROCW)0UL,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");

    TSTCHECKLASTERROR(ERROR_INVALID_PARAMETER);
}


/*
 * invalid callback
 */
static void NegEnumResourceNamesW4(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW(0, RT_MENUW,
                              (ENUMRESNAMEPROCW)0UL,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");

    TSTCHECKLASTERROR(ERROR_NOACCESS);
}


/**
 * Negative - This function tests EnumResourceNamesW String Type for standard resource type.
 */
static void NegEnumResourceNamesW5(void)
{
    BOOL fRet;
    WCHAR szType[] = {'M', 'E', 'N', 'U', 0};

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesW((HMODULE)0, szType,
                              (ENUMRESNAMEPROCW)NegEnumNamesCallbackW5,
                              MY_LPARAM);
    TSTCHECK1D(!fRet, fRet);
    TSTCHECKLASTERROR(ERROR_RESOURCE_TYPE_NOT_FOUND);
}
static CALLBACK NegEnumNamesCallbackW5(HMODULE hmod, LPCWSTR lpszType, LPCWSTR lpszName, LONG lParam)
{

    TSTCHECK(hmod == (HMODULE)0);
    TSTCHECK(lpszType == RT_MENUW);
    TSTCHECK(lParam == MY_LPARAM);
    lpszName = lpszName;
    return TRUE;
}







/*******************************************************************************
*                                                                              *
*  EnumResourceNamesA  EnumResourceNamesA  EnumResourceNamesA                  *
*  EnumResourceNamesA  EnumResourceNamesA  EnumResourceNamesA                  *
*  EnumResourceNamesA  EnumResourceNamesA  EnumResourceNamesA                  *
*  EnumResourceNamesA  EnumResourceNamesA  EnumResourceNamesA                  *
*  EnumResourceNamesA  EnumResourceNamesA  EnumResourceNamesA                  *
*                                                                              *
*******************************************************************************/

/**
 * Positive - This function tests the EnumResourceNamesA function.
 */
static void PosEnumResourceNamesA1(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA(0, RT_MENUA,
                              (ENUMRESNAMEPROCA)PosEnumNamesCallbackA1,
                              MY_LPARAM);
    if (!fRet)
    {
        TstError(__FILE__, __LINE__, "failed with fRet=%d and LastError=%d\n", fRet, GetLastError());
    }
    TSTCHECKLASTERROR(NO_ERROR);
    TSTCHECK1D(iPosA1 == 4, iPosA1);
}
static CALLBACK PosEnumNamesCallbackA1(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam)
{
    static LPCTSTR aiRes[] = {(LPCTSTR)IDM_1, (LPCTSTR)IDM_2, (LPCTSTR)IDM_3, (LPCTSTR)IDM_4};

    TSTCHECK(hmod == 0);
    TSTCHECK(lpszType == RT_MENUA);
    TSTCHECK(lParam == MY_LPARAM);
    TSTCHECK(aiRes[iPosA1] == lpszName);
    iPosA1++;
    return TRUE;
}


/**
 * Positive - This function tests the EnumResourceNamesA, negative callback return.
 */
static void PosEnumResourceNamesA2(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA((HMODULE)0, RT_MENUA,
                              (ENUMRESNAMEPROCA)PosEnumNamesCallbackA2,
                              MY_LPARAM);
    if (fRet)
        TstError(__FILE__, __LINE__, "didn't fail, fRet=%d and LastError=%d\n", fRet, GetLastError());

    TSTINFOLASTERROR();
    TSTCHECKLASTERROR(NO_ERROR);
    TSTCHECK1D(iPosA2 == 2, iPosA2);
}
static CALLBACK PosEnumNamesCallbackA2(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam)
{
    static LPCTSTR aiRes[] = {(LPCTSTR)IDM_1, (LPCTSTR)IDM_2, (LPCTSTR)IDM_3, (LPCTSTR)IDM_4};

    TSTCHECK(hmod == (HMODULE)0);
    TSTCHECK(lpszType == RT_MENUA);
    TSTCHECK(lParam == MY_LPARAM);
    TSTCHECK(aiRes[iPosA2] == lpszName);
    iPosA2++;
    return iPosA2 < 2 ? TRUE : FALSE;
}


/**
 * Positive - This function tests EnumResourceNamesA return value
 */
static void PosEnumResourceNamesA3(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA((HMODULE)0, RT_MENUA,
                              (ENUMRESNAMEPROCA)PosEnumNamesCallbackA3,
                              MY_LPARAM);
    if (fRet != 1)
        TstError(__FILE__, __LINE__, "invalid return, fRet=%d and LastError=%d\n", fRet, GetLastError());

    TSTCHECKLASTERROR(NO_ERROR);
}
static CALLBACK PosEnumNamesCallbackA3(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam)
{

    TSTCHECK(hmod == (HMODULE)0);
    TSTCHECK(lpszType == RT_MENUA);
    TSTCHECK(lParam == MY_LPARAM);
    lpszName = lpszName;
    return 0xBEEF0000;
}


/**
 * Negative - invalid handle
 */
static void NegEnumResourceNamesA1(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA((HMODULE)0xBEEFBEEF, RT_MENUA,
                              (ENUMRESNAMEPROCA)NegEnumNamesCallbackA1,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");
    TSTCHECKLASTERROR(ERROR_RESOURCE_DATA_NOT_FOUND);
}
static CALLBACK NegEnumNamesCallbackA1(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam)
{
    hmod = hmod; lpszType = lpszType; lpszName = lpszName; lParam = lParam;
    return TRUE;
}


/*
 * invalid resource type
 */
static void NegEnumResourceNamesA2(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA(0, RT_DIALOGA, /* no dialogs resources */
                              (ENUMRESNAMEPROCA)NegEnumNamesCallbackA2,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");
    TSTCHECKLASTERROR(ERROR_RESOURCE_TYPE_NOT_FOUND);
}
static CALLBACK NegEnumNamesCallbackA2(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam)
{
    hmod = hmod; lpszType = lpszType; lpszName = lpszName; lParam = lParam;
    return TRUE;
}


/*
 * invalid lpszType pointer
 */
static void NegEnumResourceNamesA3(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA(0, (LPCTSTR)0x80000000,
                              (ENUMRESNAMEPROCA)0UL,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");

    TSTCHECKLASTERROR(ERROR_INVALID_PARAMETER);
}


/*
 * invalid callback
 */
static void NegEnumResourceNamesA4(void)
{
    BOOL fRet;

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA(0, RT_MENUA,
                              (ENUMRESNAMEPROCA)0UL,
                              MY_LPARAM);
    if (fRet == TRUE)
        TstError(__FILE__, __LINE__, "invalid return code, !FALSE");

    TSTCHECKLASTERROR(ERROR_NOACCESS);
}


/**
 * Negative - This function tests EnumResourceNamesA String Type for standard resource type.
 */
static void NegEnumResourceNamesA5(void)
{
    BOOL fRet;
    CHAR szType[] = {'M', 'E', 'N', 'U', 0};

    SetLastError(NO_ERROR);

    fRet = EnumResourceNamesA((HMODULE)0, szType,
                              (ENUMRESNAMEPROCA)NegEnumNamesCallbackA5,
                              MY_LPARAM);
    TSTCHECK1D(!fRet, fRet);
    TSTCHECKLASTERROR(ERROR_RESOURCE_TYPE_NOT_FOUND);
}
static CALLBACK NegEnumNamesCallbackA5(HMODULE hmod, LPCTSTR lpszType, LPCTSTR lpszName, LONG lParam)
{

    TSTCHECK(hmod == (HMODULE)0);
    TSTCHECK(lpszType == RT_MENUA);
    TSTCHECK(lParam == MY_LPARAM);
    lpszName = lpszName;
    return TRUE;
}

