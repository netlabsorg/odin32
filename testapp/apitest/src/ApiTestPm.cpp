
/*****************************************************************************\
* ApiTestPm.cpp                                                               *
* --------------------------------------------------------------------------- *
* This is the PM version of the ApiTest program.                              *
* It is used as a casco to construct the basic structure.                     *
* Note that this is not the program of focus, that would be the Odin32 and    *
* Win32 variants which will use the Odin32-API and will be added in upcoming  *
* commits.                                                                    *
\*****************************************************************************/


/*
// Include the standard C/C++ headers.
*/
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/*
// Include the Platform headers for OS/2.
*/
#define     INCL_DOS
#define     INCL_WIN
#include    <os2.h>


/*
// Module related include-files.
*/
#include    "ids.h"

/*
// Minimal Window Procedure.
*/
MRESULT EXPENTRY    PmWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    /*
    // Local Variables follow here.
    */
    MRESULT mres    = (MRESULT) 0;
    ERRORID err     = -1;

    /*
    // The Message Switch
    */
    switch (msg) {

        /*
        // Initial Window Creation.
        */
        case WM_CREATE:
            printf("WM_CREATE received\n");
            mres = WinDefWindowProc(hwnd, msg, mp1, mp2);
            /*
            // Create a button on the client-window
            // ------------------------------------
            // So we can quickly exit the application by just pressing
            // the space-bar.
            */
            do {
                HWND hwndButton = NULLHANDLE;
                //break;
                /* Create the button */
                hwndButton = WinCreateWindow(
                    hwnd,                       // Parent (client-window)
                    WC_BUTTON,                  // We want a window of class WC_BUTTON
                    (PSZ)"Bye",                 // The button-text
                    WS_VISIBLE|BS_PUSHBUTTON,   // Make it visible
                    20,                         // The x-pos from llc
                    20,                         // The y-pos from llc
                    100,                        // Width of the button
                    50,                         // Height of the button
                    hwnd,                       // Owner (client-window)
                    HWND_TOP,                   // Z-order
                    ID_EXIT,                    // Window ID
                    NULL,                       // Control Data (none)
                    NULL                        // Presentation Parameters (none)
                );
                /* Give the focus to the button */
                WinSetFocus(HWND_DESKTOP, hwndButton);
            } while (0);
            break;

        /*
        // Commands sent by Controls.
        */
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1)) {

                /* Message from the button, we post a message to close the window */
                case ID_EXIT:
                    WinPostMsg(hwnd, WM_CLOSE, NULL, NULL);
                    break;

                /* Exit Message from the File Menu, forward it to ID_EXIT */
                case ID_FILE_EXIT:
                    WinPostMsg(hwnd, WM_COMMAND, (MPARAM) ID_EXIT, NULL);
                    break;

                /* Messages from the Test Menu */
                case ID_TEST1:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;
                case ID_TEST2:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;
                case ID_TEST3:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;
                case ID_TEST4:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;
                case ID_TEST5:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;
                case ID_TEST6:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;
                case ID_TEST7:
                    printf("WM_COMMAND received, id: %04d\n", SHORT1FROMMP(mp1));
                    break;

            }
            break;  /*WM_COMMAND*/

        /*
        // Background Erasure.
        */
        case WM_ERASEBACKGROUND:
            printf("WM_ERASEBACKGROUND received\n");
            mres = (MRESULT) TRUE;
            break;

        /*
        // Request to close the application.
        */
        case WM_CLOSE:
            printf("WM_CLOSE received\n");
            mres = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;

        /*
        // Window is being destroyed, time to cleanup resources allocated.
        */
        case WM_DESTROY:
            printf("WM_DESTROY received\n");
            mres = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;

        /*
        // Pass all unhandled messages to the default handler.
        */
        default: {
            mres = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }
    }

    /*
    // Return whatever was composed as a return-value in the switch above.
    */
    return mres;
}



/*
// PmMain Selective EntryPoint.
*/
int     PmMain(int argc, char* argv[]) {
    BOOL    brc     = FALSE;    // Boolean return values
    APIRET  ulrc    = -1;       // Numeric return values
    HAB     hab     = NULL;     // Handle Anchor Block
    HMQ     hmq     = NULL;     // Handle Message Queue
    QMSG    qmsg    = {0};      // Message Queue Structure
    ULONG   flStyle = {0};      // Style Flags
    ULONG   flCreateFlags = {
                FCF_SYSMENU     |   // Create a system-menu button
                FCF_TITLEBAR    |   // Create a title-bar
                FCF_MINMAX      |   // Create the min-max buttons (and close)
                FCF_CLOSEBUTTON |   // Actually redundant, implied by FCF_MINMAX
                FCF_SIZEBORDER  |   // Draw a sizing-border
                FCF_NOBYTEALIGN |   // Make horizontal positioning smooth
                FCF_MENU        |   // Load a menu
                FCF_TASKLIST        // Put the beast in the task-list
            };
    PSZ     pszClassClient  = (PSZ) "PmMainWindow";             // Window Class Name
    PSZ     pszTitle        = (PSZ) "ApiTestPm :: Main Window"; // Window Title
    ULONG   flStyleClient   = 0;                                // Style for Client Window
    HWND    hwndFrame       = NULL;                             // Receives handle for Frame Window
    HWND    hwndClient      = NULL;                             // Receives handle for Client Window



    /*
    // Switch the process type to PM so the command line app can create PM
    // windows. This makes it possible to use printf() to stdout.
    */
    do {
        PTIB   ptib;
        PPIB   ppib;
        //break;
        if(DosGetInfoBlocks(&ptib, &ppib) == 0) {
            ppib->pib_ultype = 3;
        }
    } while (0);



    /* Initialize the PM Graphics System */
    hab = WinInitialize(NULL);

    /* Create the message-queue for this (main) thread */
    hmq = WinCreateMsgQueue(hab, 0);

    /* Register the class of the Main Window */
    brc = WinRegisterClass(hab, pszClassClient, PmWindowProc, CS_SIZEREDRAW,  0);

    /* Create the Main Window */
    hwndFrame = WinCreateStdWindow(
        HWND_DESKTOP,       // Desktop is parent so this is a top-level Window
        flStyle,            // Style stuff
        &flCreateFlags,     // Creation options like min-max buttonts, etc.
        pszClassClient,     // Classname for the Client
        pszTitle,           // Title for the Window
        flStyleClient,      // Style stuff
        NULL,               // Module handle (null is current module)
        1,                  // Window-ID
        &hwndClient         // Storage for Client Handle
    );



    /*
    // Show the handles of the windows created.
    // The application has to be built as WINDOWCOMPAT and the code to change
    // the application-type to PM has to be active.
    */
    do {
        int delay = 1000;
        int i = 0;
        //break;
        printf("hwndFrame=%08X\n", hwndFrame);
        printf("hwndClient=%08X\n", hwndClient);
        break;
        for (i=5; i>0; i--) {
            i > 1 ? printf("Pausing for %d seconds...\n", i) : printf("Pausing for %d second...\n", i);
            DosSleep(delay);
        }
        printf("Continuing program...\n");
    } while (0);



    /* Set the size and position */
    brc = WinSetWindowPos(
        hwndFrame,      // Window to position (FrameWindow)
        HWND_TOP,       // Z-order
        100,            // The x-pos from llc
        100,            // The y-pos from llc
        800,            // Width
        600,            // Height
        SWP_MOVE    |   // Do the move
        SWP_SIZE    |   // Do the sizing
        SWP_ZORDER  |   // Do the z-ordering
        SWP_SHOW    |   // Make window visible
        SWP_ACTIVATE    // Activate it
    );

    /* Enter the message-processing loop */
    while (WinGetMsg(hab, &qmsg, 0, 0, 0))
        WinDispatchMsg(hab, &qmsg);

    /* Destroy the message-queue for this (main) thread */
    brc = WinDestroyMsgQueue(hmq);

    /* Release the Graphics System */
    brc = WinTerminate(hab);

    /* App is terminating */
    printf("ApiTestPm is terminating...\n");

    /* Return our reply-code */
    return 0;
}


/*
// This is the standard C/C++ EntryPoint.
*/
int     main(int argc, char* argv[]) {
    printf("\n");
    printf("%s\n","###############################################################################");
    printf("%s\n","# This is the PM version of ApiTest                      version.201512030711 #");
    printf("%s\n","###############################################################################");
    printf("\n");
    printf("%s\n","Switching to Graphical Mode with this Window as a Console Log...");

    /*
    // The graphical part is encapsulated in a separate function so we can
    // easily fiddle with it.
    */
    PmMain(argc, argv);
    return 0;
}

