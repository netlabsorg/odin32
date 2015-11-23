
/*****************************************************************************\
* PmApiTest.cpp                                                               *
* --------------------------------------------------------------------------- *
* This file is generated from an xmind-file !                                 *
* It uses the newly designed transformation-sheet.                            *
* While this sheet is still file-based, it will be moved to an Ant Property   *
* later so it can be constructed from parts.                                  *
\*****************************************************************************/


/*
// Include the standard C/C++ headers.
*/
#include    <stdlib.h>
#include    <stdio.h>
#include    <string.h>

/*
// Include the Platform headers.
*/
#define     INCL_DOS
#define     INCL_WIN
#include    <os2.h>


/*
// Minimal Window Procedure.
*/
MRESULT EXPENTRY    MyWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
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
                    123,                        // Window ID
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
                /* This is a message from our button, we post a message to close the window */
                case 123:
                    WinPostMsg(hwnd, WM_CLOSE, NULL, NULL);
                    break;
            }
            break;

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
    PSZ     pszTitle        = (PSZ) "PmApiTest :: Main Window"; // Window Title
    ULONG   flStyleClient   = 0;                                // Style for Client Window
    HWND    hwndFrame       = NULL;                             // Receives handle for Frame Window
    HWND    hwndClient      = NULL;                             // Receives handle for Client Window



    /* Initialize the Graphics System */
    hab = WinInitialize(NULL);

    /* Create the message-queue for this (main) thread */
    hmq = WinCreateMsgQueue(hab, 0);

    /* Register the class of the Main Window */
    brc = WinRegisterClass(hab, pszClassClient, MyWindowProc, CS_SIZEREDRAW,  0);

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

    /* Return our reply-code */
    return 0;
}


/*
// This is the standard C/C++ EntryPoint.
*/
int     main(int argc, char* argv[]) {
    printf("\n");
    printf("%s\n","###############################################################################");
    printf("%s\n","# This is the PM version of ApiTest                      version.201511231348 #");
    printf("%s\n","###############################################################################");
    printf("\n");

    /*
    // The graphical part is encapsulated in a separate function so we can
    // easily fiddle with it.
    */
    PmMain(argc, argv);
    return 0;
}

