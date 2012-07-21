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
    LPCTSTR szFile = _T("test.txt");

    HANDLE hFile = CreateFile(szFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile)
    {
        printf("hFile %08lX\n", hFile);
        BY_HANDLE_FILE_INFORMATION info;
        if (GetFileInformationByHandle(hFile, &info))
        {
            printf("dwVolumeSerialNumber %08lX\n", info.dwVolumeSerialNumber);
            printf("nFileIndexHigh       %08lX\n", info.nFileIndexHigh);
            printf("nFileIndexLow        %08lX\n", info.nFileIndexLow);
        }
    }

    return 0;
}
