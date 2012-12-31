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
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD tin = GetFileType(hin);
    printf("STD_INPUT_HANDLE %x = %x, type %d\n", STD_INPUT_HANDLE, hin, tin);

    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD tout = GetFileType(hout);
    printf("STD_OUTPUT_HANDLE %x = %x, type %d\n", STD_OUTPUT_HANDLE, hout, tout);

    HANDLE herr = GetStdHandle(STD_ERROR_HANDLE);
    DWORD terr = GetFileType(herr);
    printf("STD_ERROR_HANDLE %x = %x, type %d\n", STD_ERROR_HANDLE, herr, terr);

	const char *str = "beef\nfish\n";
    DWORD written;
	WriteFile(hout, str, strlen(str), &written, NULL);

    return 0;
}
