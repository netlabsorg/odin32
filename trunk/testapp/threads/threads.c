/*
 * Taken from here http://msdn.microsoft.com/en-us/library/ms682516%28v=VS.85%29.aspx
 * and modified for the needs of the testcase
 */

//#define USE_TRY

#if !defined(_MSC_VER) && 1
#define INCL_DOS
#include <os2wrap2.h>
#endif

#include <windows.h>
#include <tchar.h>
#include <excpt.h>

#define MAX_THREADS 10
#define BUF_SIZE 255

#ifdef _MSC_VER
#include <strsafe.h>
#else
#include <stdarg.h>
void StringCchPrintf(LPTSTR pszDest, size_t cchDest,
                     LPCTSTR pszFormat, ...);
void StringCchLength(LPCTSTR psz, size_t cchMax, size_t *pcch);
#endif

DWORD WINAPI MyThreadFunction( LPVOID lpParam );
void ErrorHandler(LPTSTR lpszFunction);

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
    int val1;
    int val2;
} MYDATA, *PMYDATA;

#ifndef _MSC_VER

// register the EXE to cause the OS/2 exception handler setup around
// the entry point

#include <odinlx.h>

#undef _tmain
int _tmain();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    return _tmain();
}

int main(int argc, char **argv)
{
    EnableSEH();
    RegisterLxExe((WINMAIN)WinMain, NULL);
    return _tmain();
}

void StringCchPrintf(LPTSTR pszDest, size_t cchDest,
                     LPCTSTR pszFormat, ...)
{
    va_list ap;

    va_start(ap, pszFormat);
    vsnprintf(pszDest, cchDest, pszFormat, ap);
    va_end(ap);
}

void StringCchLength(LPCTSTR psz, size_t cchMax, size_t *pcch)
{
    *pcch = strnlen(psz, cchMax);
}

#endif // !_MSC_VER

int _tmain()
{
    PMYDATA pDataArray[MAX_THREADS];
    DWORD   dwThreadIdArray[MAX_THREADS];
    HANDLE  hThreadArray[MAX_THREADS];

    // Create MAX_THREADS worker threads.

    int i;
    for( i=0; i<MAX_THREADS; i++ )
    {
        // Allocate memory for thread data.

        pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                sizeof(MYDATA));

        if( pDataArray[i] == NULL )
        {
           // If the array allocation fails, the system is out of memory
           // so there is no point in trying to print an error message.
           // Just terminate execution.
            ExitProcess(2);
        }

        // Generate unique data for each thread to work with.

        pDataArray[i]->val1 = i;
        pDataArray[i]->val2 = i+100;

        // Create the thread to begin execution on its own.

        hThreadArray[i] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            MyThreadFunction,       // thread function name
            pDataArray[i],          // argument to thread function
            0,                      // use default creation flags
            &dwThreadIdArray[i]);   // returns the thread identifier


        // Check the return value for success.
        // If CreateThread fails, terminate execution.
        // This will automatically clean up threads and memory.

        if (hThreadArray[i] == NULL)
        {
           ErrorHandler(TEXT("CreateThread"));
           ExitProcess(3);
        }
    } // End of main thread creation loop.

    _tprintf(TEXT("Started %d threads. Waiting for them to terminate...\n"),
             MAX_THREADS);

    // Wait until some threads have terminated.

    WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, 2000);

    _tprintf(TEXT("Finished waiting.\n"));

    // Close all thread handles and free memory allocations.

    for(i=0; i<MAX_THREADS; i++)
    {
        CloseHandle(hThreadArray[i]);
        if(pDataArray[i] != NULL)
        {
            HeapFree(GetProcessHeap(), 0, pDataArray[i]);
            pDataArray[i] = NULL;    // Ensure address is not reused.
        }
    }

#if !defined(_MSC_VER) && 1
    DosExit(1, 0);
#endif

    return 0;
}

#ifdef USE_TRY
int exc_filter(DWORD code, PEXCEPTION_POINTERS pPtrs)
{
    PEXCEPTION_RECORD pRec = pPtrs->ExceptionRecord;

#ifndef _MSC_VER
    os2_PPIB pPib;
    os2_PTIB pTib;
    DosGetInfoBlocks(&pTib, &pPib);
    printf("TID: %d\n", pTib->tib_ptib2->tib2_ultid);
#endif

    _tprintf(TEXT("Filter: code %08lx\n"), code);
    _tprintf(TEXT("ExceptionCode %p\n"), pRec->ExceptionCode);
    _tprintf(TEXT("ExceptionAddress %p\n"), pRec->ExceptionAddress);
    _tprintf(TEXT("NumberParameters %d\n"), pRec->NumberParameters);

    return EXCEPTION_CONTINUE_SEARCH;
}
#endif

DWORD WINAPI MyThreadFunction( LPVOID lpParam )
{
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

#ifdef USE_TRY
    __try
    {
#endif
        // Cast the parameter to the correct data type.
        // The pointer is known to be valid because
        // it was checked for NULL before the thread was created.

        pDataArray = (PMYDATA)lpParam;

        // Print the parameter values using thread-safe functions.

        Sleep(1000 * pDataArray->val1 / 2);

#if 0
        StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"),
            pDataArray->val1, pDataArray->val2);
        _tprintf("%s", msgBuf);
#endif

#ifdef USE_TRY
    }
    __except(exc_filter(exception_code(), exception_info()))
    {
    }
#endif

    return 0;
}

void ErrorHandler(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

	// Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

