#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>

#ifndef _MSC_VER

#include <odinlx.h>

#define WC_EDIT TEXT("Edit")

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

int main()
{
#ifdef ODIN_FORCE_WIN32_TIB
    ForceWin32TIB();
#endif
    RegisterLxExe(WinMain, NULL);
    return 0;
}

#else

int main()
{
    return WinMain(NULL, NULL, 0, SW_SHOWNORMAL);
}

#endif

#define MY_IDC_EDIT 100

HINSTANCE ghInst;

LRESULT CALLBACK EditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC wpOld = (WNDPROC)GetWindowLong(hWnd, GWL_USERDATA);
    LRESULT lrResult = 0;

    if (wpOld)
    {
        switch (uMsg)
        {
            case WM_KEYDOWN:
            {
                printf("WM_KEYDOWN = %08x (%04x)\n", (int)wParam,
                       (!!GetKeyState(VK_SHIFT) << 2) |
                       (!!GetKeyState(VK_CONTROL) << 1) |
                       (!!GetKeyState(VK_MENU) << 0));
                break;
            }

            case WM_CHAR:
            {
                printf("WM_CHAR = %08x\n", (int)wParam,
                       (!!GetKeyState(VK_SHIFT) << 2) |
                       (!!GetKeyState(VK_CONTROL) << 1) |
                       (!!GetKeyState(VK_MENU) << 0));
                break;
            }
        }

        return CallWindowProc(wpOld, hWnd, uMsg, wParam, lParam);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            HWND hWndChild;
            hWndChild = CreateWindowEx(WS_EX_CLIENTEDGE, WC_EDIT, NULL,
                                       ES_MULTILINE | ES_WANTRETURN |
                                       ES_AUTOVSCROLL | ES_NOHIDESEL |
                                       WS_VSCROLL | WS_CHILD |
                                       WS_TABSTOP | WS_VISIBLE,
                                       0, 0, 0, 0, hWnd,
                                       (HMENU)MY_IDC_EDIT, ghInst, NULL);
            printf("hWndChild = %08x\n", hWndChild);
            if (!hWndChild)
                return -1;

            SetWindowLong(hWndChild, GWL_USERDATA,
                          GetWindowLong(hWndChild, GWL_WNDPROC));
            SetWindowLong(hWndChild, GWL_WNDPROC, (LONG)EditProc);
            SetFocus(hWndChild);

            return 0;
        }

        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
        {
            HDWP hDWP;
            RECT rc;

            if (hDWP = BeginDeferWindowPos(5))
            {
                GetClientRect(hWnd, &rc);

                hDWP = DeferWindowPos(hDWP, GetDlgItem(hWnd, MY_IDC_EDIT), NULL,
                    10, 70, rc.right - 20, rc.bottom - 80,
                    SWP_NOZORDER | SWP_NOREDRAW);

                EndDeferWindowPos(hDWP);

                RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN |
                    RDW_ERASE | RDW_NOFRAME | RDW_UPDATENOW);
            }

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    WNDCLASSEX wcex;
    DWORD dwExStyle;
    HWND hWnd;
    MSG msg;

    InitCommonControls();

    ZeroMemory(&msg,  sizeof(MSG));
    ZeroMemory(&wcex, sizeof(WNDCLASSEX));

    ghInst = hInstance;

    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.hInstance     = hInstance;
    wcex.lpszClassName = TEXT("MainWindow");
    wcex.lpfnWndProc   = MainWindowProc;
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hIconSm       = wcex.hIcon;
    wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    if(!RegisterClassEx(&wcex))
        return 1;

    dwExStyle = WS_EX_APPWINDOW;

    hWnd = CreateWindowEx(dwExStyle, wcex.lpszClassName,
#ifdef UNICODE    
                          TEXT("Edit Control Test (oe lat: \x00F6) (yo rus: \x0451) [unicode]"),
#else                          
                          TEXT("Edit Control Test (oe lat: \xF6) (yo rus: \xB8) [ascii]"),
#endif                          
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT, CW_USEDEFAULT, 450, 330,
                          HWND_DESKTOP, NULL, ghInst, NULL);
    printf("hWnd = %08x\n", hWnd);

    if (hWnd) {
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);
        while (GetMessage(&msg, NULL, 0, 0))
        {
            printf("GetMessage       = %08x %08x %08x %08x\n",
                   hWnd, msg.message, msg.wParam, msg.lParam);
            TranslateMessage(&msg);
            printf("TranslateMessage = %08x %08x %08x %08x\n",
                   hWnd, msg.message, msg.wParam, msg.lParam);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}
