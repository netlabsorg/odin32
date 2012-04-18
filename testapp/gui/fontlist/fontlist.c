#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#include "XFont.h"

#ifndef _MSC_VER

#include <odinlx.h>

int _main();
int _argc;
char **_argv;

int WIN32API WinMain(HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR  lpCmdLine,
                     int    nCmdShow)
{
    return _main(_argc, _argv);
}

int main(int argc, char **argv)
{
    _argc = argc;
    _argv = argv;
#ifdef ODIN_FORCE_WIN32_TIB
    ForceWin32TIB();
#endif
    RegisterLxExe(WinMain, NULL);
}

#else
#define _main main
#endif

int _main(int argc, char **argv)
{
    LPCTSTR szFontName = _T("Arial");

    TCHAR szDisplayName[1000], szFontFile[1000];

    printf("Font name : '%s'\n", szFontName);
    if (GetFontFile(szFontName,
                    szDisplayName, sizeof(szDisplayName),
                    szFontFile, sizeof(szFontFile)))
    {
        printf("  Display name : '%s'\n", szDisplayName);
        printf("  File name    : '%s'\n", szFontFile);
    }
    else
    {
        printf(_T("  <Not found>\n"));
    }

    return 0;
}
