#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#ifndef _MSC_VER

#include <odinlx.h>

#include <minivcrt.h>

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

static void test_fullpath(const char *path)
{
    char fullPath[MAX_PATH];
    _fullpath(fullPath, path, sizeof(fullPath));
    printf("### [%s] => [%s]\n", path, fullPath);
}

static void do_test_fullpath()
{
    test_fullpath("");
    test_fullpath("\\");
    test_fullpath("\\\\\\\\");
    test_fullpath(".");
    test_fullpath(".\\");
    test_fullpath("\\.");
    test_fullpath("\\.\\");
    test_fullpath("..");
    test_fullpath("..\\");
    test_fullpath("\\..");
    test_fullpath("\\..\\");
    test_fullpath("a\\b");
    test_fullpath("\\a\\b");
    test_fullpath("a\\b\\");
    test_fullpath("\\a\\b\\");
    test_fullpath("d:\\a\\b");
    test_fullpath("d:\\a\\b\\");
    test_fullpath("d:\\a\\b\\\\");
    test_fullpath("d:\\a\\\\b\\c\\\\d");
    test_fullpath("d:\\a\\.\\.\\b\\c\\.\\.\\.\\d");
    test_fullpath("d:\\..\\..\\.\\");
    test_fullpath("d:\\a\\\\b\\c\\\\d\\..\\e");
    test_fullpath("d:\\a\\\\b\\c\\\\d\\..\\..\\e");
    test_fullpath("d:\\a\\\\b\\c\\\\d\\..\\..\\..\\e");
    test_fullpath("d:\\a\\\\b\\c\\\\d\\..\\..\\\\..\\e");
    test_fullpath("d:\\a\\\\b\\c\\\\d\\..\\..\\..\\..\\e");
    test_fullpath("d:\\a\\\\b\\c\\\\d\\..\\..\\..\\..\\..\\e");
    test_fullpath("d\\a\\\\b\\c\\\\d\\..\\..\\..\\..\\..\\e");
}

int _main(int argc, char **argv)
{
    do_test_fullpath();

    return 0;
}
