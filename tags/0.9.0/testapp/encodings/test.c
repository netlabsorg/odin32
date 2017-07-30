#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#ifndef _MSC_VER

#include <odinlx.h>

int _main(int argc, char **argv);

int WIN32API WinMain(HANDLE hInstance,
                     HANDLE hPrevInstance,
                     LPSTR  lpCmdLine,
                     int    nCmdShow)
{
    return _main(__argcA, __argvA);
}

int main(int argc, char **argv)
{
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
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL rc;
    char *cmd;

	/* ANSI encoding */
    MessageBoxA(NULL,
                "ANSI: ������� 1251, ���᪨� 866",
                "ANSI: ������� 1251, ���᪨� 866",
                MB_ICONINFORMATION | MB_OK);

    /* Unicode */
    MessageBoxW(NULL,
                L"Unicode: \x0420\x0443\x0441\x0441\x043a\x0438\x0439",
                L"Unicode: \x0420\x0443\x0441\x0441\x043a\x0438\x0439",
                MB_ICONINFORMATION | MB_OK);

    /* Command line */
    printf("1st Command Line argument: \"%s\"\n", argc > 1 ? argv[1] : NULL);
    MessageBoxA(NULL,
                argc > 1 ? argv[1] : NULL,
                "1st Command Line argument",
                MB_ICONINFORMATION | MB_OK);

    /* Start a command */
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    cmd = getenv("COMSPEC");
    rc = CreateProcess(cmd, "cmd.exe /c ����.cmd", NULL, NULL, FALSE, 0,
                       NULL, NULL, &si, &pi);
    printf("CreateProcess returned %d (error %x)\n", rc, GetLastError());
    if (rc) {
        printf("dwProcessId %d\n", pi.dwProcessId);
    }

    return 0;
}
