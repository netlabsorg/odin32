#include <stdio.h>
#include <windows.h>
#include <tchar.h>

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
    LPCTSTR szFile = _T("..");

    HANDLE hFile = CreateFile(szFile, 0,
                              FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, 0, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        printf("szFile %s\n", szFile);
        printf("hFile %08lX\n", hFile);
        BY_HANDLE_FILE_INFORMATION info;
        if (GetFileInformationByHandle(hFile, &info))
        {
            printf("dwVolumeSerialNumber %08lX\n", info.dwVolumeSerialNumber);
            printf("nFileIndexHigh       %08lX\n", info.nFileIndexHigh);
            printf("nFileIndexLow        %08lX\n", info.nFileIndexLow);
        }
    }
    else
    {
        printf("Error %d opening file '%s'\n", GetLastError(), szFile);
    }

    return 0;
}
