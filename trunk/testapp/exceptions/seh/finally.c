#include <stdio.h>
#include <windows.h>
#include <excpt.h>

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
    EnableSEH();
    RegisterLxExe(WinMain, NULL);
}

#else
#define _main main
#endif

void throw_EXCEPTION_INT_DIVIDE_BY_ZERO()
{
    printf("Throwing EXCEPTION_INT_DIVIDE_BY_ZERO...\n");
    int x = 0;
    volatile int y = 4 / x;
}

int _main(int argc, char **argv)
{
    int code = argc > 1 ? atoi(argv[1]) : 0;

    __try
    {
        switch(code)
        {
            case 1:
            {
                throw_EXCEPTION_INT_DIVIDE_BY_ZERO();
                printf("FAILED: Exception was not thrown!\n");
                return 1;
            }
            case 2:
            {
                printf("Doing __leave...\n");
                __leave;
                printf("FAILED: __leave didn't work!\n");
                return 1;
            }
            case 3:
            default:
            {        
                printf("Not causing exceptions, not doing __leave...\n");
                break;
            }
        }
    }
    __finally
    {
        // handle exception
        printf("Finally block executed.\n");
    }

    printf("Return.\n");

    return 0;
}
