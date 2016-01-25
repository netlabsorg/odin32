//! GENINFO :: platform:OS/2, version:20.45, target:ApiTest.generate
/*****************************************************************************\
* ApiTestOdin.cpp :: This is the Odin Based variant of ApiTest                *
* --------------------------------------------------------------------------- *
* This is the main variant of interest as it uses the Odin32-API while being  *
* a Native OS/2 application and thus in LX-format. One could say this variant *
* is 'Odin Aware' because it is built against Odin32 from the source-level.   *
* Another term is 'Odin Based'.                                               *
\*****************************************************************************/


/*
// Standard C/C++ Headers
*/
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>



/*
// Platform Headers for Odin32
*/
//#include  <os2wrap.h>
#include    <windows.h>
//#include  <wingdi.h>
#include <odinlx.h>



/*
// Module Headers
*/
#include    "ids.h"
#include    "ApiTestOdin.hpp"



/*
// Window Procedure
*/
LRESULT CALLBACK Odin32WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    /*
    // Local Variables follow here.
    */
    LRESULT lres    = (LRESULT) 0;
    //ERRORID   err     = -1;

    /*
    // The Message Switch
    */
    switch (msg) {

        /*
        // Initial Window Creation.
        */
        case WM_CREATE:
            printf("WM_CREATE received\n");
            lres = DefWindowProc(hwnd, msg, wparam, lparam);
            /*
            // Create a button on the client-window
            */
            do {
                HWND hwndButton = NULL;
                //break;
                /* Create the button */
                hwndButton = CreateWindow(
                    "BUTTON",                           // Window Class
                    "Bye",                              // The button-text
                    WS_VISIBLE|WS_CHILD|BS_PUSHBUTTON,  // The button-style
                    20,                                 // The x-pos from ulc
                    600-20-50-50,                       // The y-pos from ulc
                    100,                                // Width of the button
                    50,                                 // Height of the button
                    hwnd,                               // Owner Window
                    (HMENU)ID_EXIT,                     // Button ID
                    (HANDLE)NULL,                       // Module Instance
                    (LPVOID)NULL                        // Create Structure (NA)
                );

                /* Give the focus to the button */
                SetFocus(hwndButton);
            } while (0);
            break;

        /*
        // Commands sent by Controls.
        */
        case WM_COMMAND:
            switch (LOWORD(wparam)) {

                /* Message from the button, we post a message to close the window */
                case ID_EXIT:
                    PostQuitMessage(0);
                    break;

                /* Exit Message from the File Menu, forward it to ID_EXIT */
                case ID_FILE_EXIT:
                    PostMessage(hwnd, WM_COMMAND, (UINT) ID_EXIT, NULL);
                    break;

                /* Messages from the Test Menu */
                case ID_TEST1:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                case ID_TEST2:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                case ID_TEST3:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                case ID_TEST4:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                case ID_TEST5:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                case ID_TEST6:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                case ID_TEST7:
                    printf("WM_COMMAND received, id: %04d\n", LOWORD(wparam));
                    break;
                default:
                    lres = DefWindowProc(hwnd, msg, wparam, lparam);
                    break;
            }
            break;  /*WM_COMMAND*/

        /*
        // Request to quit the application.
        */
        case WM_QUIT:
            printf("WM_QUIT received\n");
            lres = DefWindowProc(hwnd, msg, wparam, lparam);
            break;

        /*
        // Request to close the application.
        // On Win32 WM_CLOSE does a DestroyWindow() but does not post a WM_QUIT
        // message and thus does not terminate the message-loop.
        */
        case WM_CLOSE:
            printf("WM_CLOSE received\n");
            lres = DefWindowProc(hwnd, msg, wparam, lparam);
            PostQuitMessage(0);
            break;

        /*
        // Window is being destroyed, time to cleanup resources allocated.
        */
        case WM_DESTROY:
            printf("WM_DESTROY received\n");
            lres = DefWindowProc(hwnd, msg, wparam, lparam);
            break;

        /*
        // Show some info.
        */
        case WM_PAINT:
            printf("WM_PAINT received\n");
            do {
                BOOL    brc = FALSE;
                PAINTSTRUCT ps;
                CHAR    buf[512] = {0};
                CHAR    buf2[512] = {0};
                HDC     hdc = NULL;
                int     y = 0;
                int     dy = 20;
                hdc = BeginPaint(hwnd, &ps);
                brc = TextOut(hdc, 0, 0, "Hello !!", strlen("Hello !!"));
                GetWindowsDirectory(buf2, sizeof(buf2));
                sprintf(buf, "WindowsDir: %s", buf2);
                brc = TextOut(hdc, 0, y+=dy, buf, strlen(buf));
                GetSystemDirectory(buf2, sizeof(buf2));
                sprintf(buf, "SystemDir: %s", buf2);
                brc = TextOut(hdc, 0, y+=dy, buf, strlen(buf));
                EndPaint(hwnd, &ps);
                lres = 0;
            } while (0);
                
            //lres = DefWindowProc(hwnd, msg, wparam, lparam);
            break;

        /*
        // Pass all unhandled messages to the default handler.
        */
        default: {
            lres = DefWindowProc(hwnd, msg, wparam, lparam);
            break;
        }

    }
    // Return result.
    return(lres);
}



/*
// Gui EntryPoint for Odin32
*/
int WINAPI OdinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS    wc;                                 // Window Class Structure
    LPCSTR      MainWndClass    = "ApiTestOdin";    // Window Class Name
    HWND        hwnd            = NULL;             // Receives Window Handle
    HACCEL      hAccelerators   = NULL;             // Handle for Accelerators
    HMENU       hSysMenu        = NULL;             // Handle for System Menu
    MSG         msg;                                // Message Structure

    /* Window Class Structure */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = Odin32WindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = hInstance;
    wc.hIcon            = (HICON)0;
    wc.hCursor          = LoadCursor((HINSTANCE)0, IDC_ARROW );
    wc.hbrBackground    = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName     = MAKEINTRESOURCE(IDR_MAINMENU);
    wc.lpszClassName    = MainWndClass;

    /* Window Class Structure (Ex) */
//  wc.cbSize           = sizeof(wc);
//  wc.style            = CS_HREDRAW | CS_VREDRAW;
//  wc.lpfnWndProc      = Odin32WindowProc;
//  wc.cbClsExtra       = 0;
//  wc.cbWndExtra       = 0;
//  wc.hInstance        = hInstance;
//  wc.hIcon            = (HICON) NULL;
//  wc.hCursor          = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
//  wc.hbrBackground    = (HBRUSH) (COLOR_BTNFACE + 1);
//  wc.lpszMenuName     = NULL;
//  wc.lpszClassName    = MainWndClass;
//  wc.hIconSm          = (HICON) NULL;

    /*
    // Show some info.
    // This does not work under Win32s because the Win16 system has no stdout.
    */
    do {
        char buf[512] = {0};
        break;
        GetCurrentDirectory(sizeof(buf), buf);
        printf("Creating Window...\n");
        GetCurrentDirectory(sizeof(buf), buf);
        printf("CurDir      : %s\n", buf);
        GetWindowsDirectory(buf, sizeof(buf));
        printf("WindowsDir  : %s\n", buf);
        GetSystemDirectory(buf, sizeof(buf));
        printf("SystemDir   : %s\n", buf);
        printf("WINDOWSPATH : %s\n", getenv("WINDOWSPATH"));
        printf("INCLUDE     : %s\n", getenv("INCLUDE"));
        printf("CommandLine : %s\n", lpCmdLine);
    } while (0);


    /* Register the class of the Main Window */
    RegisterClass(&wc);
  
    /* Create the Main Window */
    hwnd = CreateWindow(
        MainWndClass,                   // Our Window Class
        "ApiTestOdin :: Main Window [generated:201601252233]",  // Caption Text
        WS_OVERLAPPEDWINDOW,            // Window Stype
        100,                            // The x-pos from ulc
        100,                            // The y-pos from ulc
        800,                            // Width of the window
        600,                            // Height of the window
        (HWND)0,                        // Owner Window
        (HMENU)0,                       // Menu Handle
        hInstance,                      // Module Instance
        NULL                            // Create Structure
    );

    /* Show the Window */
    ShowWindow(hwnd, nCmdShow);

    /* Force a Paint */
    UpdateWindow(hwnd);

    /* Enter the message-processing loop */
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        if (! TranslateAccelerator(msg.hwnd, hAccelerators, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    /* App is terminating */
    printf("ApiTestOdin is terminating...\n");

    /* Return our reply-code */
    return (int) msg.wParam;
}



/*
// This is the standard C/C++ EntryPoint
*/
int     main(int argc, char* argv[]) {
    printf("\n");
    printf("%s\n","###############################################################################");
    printf("%s\n","# This is the Odin variant of ApiTest                    version.201601252233 #");
    printf("%s\n","###############################################################################");
    printf("\n");
    printf("%s\n","Switching to Graphical Mode with this Window as a Console Log...");


    /*
    // The graphical part is encapsulated in a separate function so we can
    // easily fiddle with it.
    */
//  OdinMain(NULL, NULL, NULL, NULL);
    RegisterLxExe((WINMAIN) OdinMain, (PVOID) &Resource_PEResTab);
    return 0;
}

