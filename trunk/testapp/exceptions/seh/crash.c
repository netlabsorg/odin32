#include <stdio.h>
#include <windows.h>
#include <excpt.h>
#include <setjmp.h>

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

int exc_filter(PEXCEPTION_POINTERS pPtrs)
{
    PEXCEPTION_RECORD pRec = pPtrs->ExceptionRecord;

    printf("\nexc_filter():\n");
    printf("pPtrs %p\n", pPtrs);
    printf("ExceptionCode %p\n", pRec->ExceptionCode);
    printf("ExceptionAddress %p\n", pRec->ExceptionAddress);
    printf("NumberParameters %d\n", pRec->NumberParameters);
    printf("Returning EXCEPTION_CONTINUE_SEARCH.\n");

    return EXCEPTION_CONTINUE_SEARCH;
}

void throw_EXCEPTION_INT_DIVIDE_BY_ZERO()
{
    printf("Throwing EXCEPTION_INT_DIVIDE_BY_ZERO...\n");
    int x = 0;
    volatile int y = 4 / x;
}

int test_1()
{
    printf("The program should now expectedly crash "
           "(but NO POPUPLOG.OS2 entry!)...\n");
    __try
    {
        printf("In outer try...\n");

        __try
        {
            printf("In inner try...\n");

            throw_EXCEPTION_INT_DIVIDE_BY_ZERO();

            printf("FAILED: No inner exception!\n");
        }
        __except(exc_filter(exception_info()))
        {
            printf("FAILED: Inner exception handled.\n");
        }

        printf("FAILED: No outer exception!\n");
    }
    __except(exc_filter(exception_info()))
    {
        printf("FAILED: Inner exception handled.\n");
    }

    printf("FAILED: No exception at all!\n");
	return 1;
}

void foo(int code)
{
   __try
   {
       printf("In foo(%d)...\n", code);

       if (code == 5)
       {
           throw_EXCEPTION_INT_DIVIDE_BY_ZERO();
           printf("FAILED: No exception!\n");
       }
       else
       {
           foo(code + 1);
       }
   }
   __except(exc_filter(exception_info()))
   {
       printf("FAILED: foo(%d) exception handled.\n", code);
   }
}

static
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
    foo(0);
    printf("FAILED: No exception at all!\n");
    return 0;
}

int test_2()
{
    printf("The program should now expectedly crash "
           "(but NO POPUPLOG.OS2 entry!)...\n");

    HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
    if (hThread == NULL)
    {
        printf("FAILED: CreateThread().");
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);

    return 1;
}

int _main(int argc, char **argv)
{
    int rc = test_2();

    printf("Return (%d).\n", rc);

    return rc;
}
