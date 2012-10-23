/* $Id: PrfTiming.h,v 1.1 2003-05-01 11:20:51 bird Exp $
 *
 * Common timing code.
 *
 * Copyright (c) 2001-2003 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */

#include <stdio.h>
#if !defined(__WINNT__)
#include <sys/time.h>
#endif
#ifndef __NOTPC__
#include <process.h>
#endif

#ifdef __OS2__
#define INCL_DOSPROFILE
#include <os2.h>

long double gettime(void)
{
    QWORD   qw;
    DosTmrQueryTime(&qw);
    return (long double)qw.ulHi * (4294967296.00) + qw.ulLo;
}

unsigned getHz(void)
{
    ULONG ul = -1;
    DosTmrQueryFreq(&ul);
    return ul;
}

void printSystemInfo(void)
{
}

#elif defined(__WINNT__)
#include <windows.h>
/*
 * Windows
 */
unsigned long __stdcall GetTickCount(void);

long double gettime(void)
{
    //return (long double)GetTickCount();
    LARGE_INTEGER ullCounter;
    ullCounter.QuadPart = 0;
    QueryPerformanceCounter(&ullCounter);
    return (long double)ullCounter.QuadPart;
}

unsigned getHz(void)
{
    //return 1000;
    LARGE_INTEGER ullFrequency;
    ullFrequency.QuadPart = 0;
    QueryPerformanceFrequency(&ullFrequency);
    return (unsigned)ullFrequency.QuadPart;
}

void printSystemInfo(void)
{
    LONG        lrc;
    SYSTEM_INFO si;
    HKEY        hProcessor0;
    char        szMhz[16];
    szMhz[0] = '\0';

    lrc = RegOpenKey(HKEY_LOCAL_MACHINE,
                     "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                     &hProcessor0);
    if (!lrc)
    {
        LONG    cchMhz = sizeof(szMhz);
        DWORD   iKey;
        CHAR    szValueName[256];
        DWORD   cchValueName = sizeof(szValueName);
        DWORD   dwType;
        BYTE    abData[256];
        DWORD   cbData;

        iKey = 0;
        szMhz[0] = '\0';
        while ((lrc = RegEnumValue(hProcessor0,
                                   iKey,
                                   szValueName,
                                   &cchValueName,
                                   NULL,
                                   &dwType,
                                   &abData,
                                   &cbData)) == 0
               || lrc == ERROR_MORE_DATA)
        {
            switch (dwType)
            {
                case REG_SZ:
                case REG_EXPAND_SZ:
                    /*
                    printf("%-24s =  type: %1x  namesize: %2d  data size: %3d bytes\n          %s\n",
                           szValueName, dwType, cchValueName, cbData, &abData[0]);
                    */
                    break;

                default:
                {
                    /*
                    int i,j;
                    printf("%-24s =  type: %1x  namesize: %2d  data size: %3d bytes\n",
                           szValueName, dwType, cchValueName, cbData);
                    for (i = 0; i < cbData; i += 16)
                    {
                        printf("    %04x ", i);
                        for (j = 0; j < 16; j++)
                        {
                            if (j + i < cbData)
                                printf(j == 8 ? "-  %02x " : "%02x ",
                                       (BYTE)abData[i+j]);
                            else
                                printf(j == 8 ? "    " : "   ");
                        }
                        putc(' ', stdout);
                        for (j = 0; j < 16; j++)
                        {
                            if (j+i < cbData)
                                putc(isprint(abData[j]) ? abData[j] : '.', stdout);
                            else
                                putc(' ', stdout);
                        }
                        putc('\n', stdout);
                    }
                    */
                    if (   !szMhz[0]
                        && stricmp(szValueName, "~MHz") == 0
                        && dwType == REG_DWORD
                        && cbData == sizeof(DWORD))
                        sprintf(szMhz, "%d", *(PDWORD)&abData[0]);
                }
            }

            /* next */
            iKey++;
            dwType = 0;
            cchValueName = sizeof(szValueName);
            szValueName[0] = '\0';
        }
        RegCloseKey(hProcessor0);
    }

    GetSystemInfo(&si);
    printf("  %d %d CPUs  %s Mhz\n",
           si.dwNumberOfProcessors,
           si.dwProcessorType,
           szMhz
           );
}

#else

long double gettime(void)
{
    struct  timeval tp;
    long    sec = 0L;

    if (gettimeofday(&tp, NULL))
        return -1;
    return tp.tv_usec / 1000000.00 + tp.tv_sec;
}

unsigned getHz(void)
{
    return 1;//000000;
}

void printSystemInfo(void)
{
}

#endif

