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
    HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
    printf("STD_INPUT_HANDLE %x = %x\n", STD_INPUT_HANDLE, h);

    DWORD type = GetFileType(h);
    printf("handle type %d\n", type);

    return 0;
}
