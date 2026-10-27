/*
 * COMCTL32 version 6 entry points that applications import by ORDINAL:
 *   #345  TaskDialogIndirect
 *   #381  LoadIconWithScaleDown
 * (ordinals as in the Windows comctl32 v6 DLL; they are unused in Odin's v5.8)
 *
 * TaskDialogIndirect is emulated with MessageBoxW: main instruction + content,
 * the common buttons and the standard icons are honoured; custom buttons,
 * radio buttons, verification box and callbacks are not.
 */
#include <os2win.h>
#include <string.h>

#define VC_E_NOTIMPL    ((LONG)0x80004001)
#define VC_E_INVALIDARG ((LONG)0x80070057)

//TASKDIALOGCONFIG prefix (the SDK packs it to 1 byte; every field is 4 bytes wide)
struct VC_TDBUTTON {
    int     nButtonID;
    LPCWSTR pszButtonText;
};

struct VC_TDCONFIG {
    UINT         cbSize;
    HWND         hwndParent;
    HINSTANCE    hInstance;
    DWORD        dwFlags;
    DWORD        dwCommonButtonFlags;
    LPCWSTR      pszWindowTitle;
    LPCWSTR      pszMainIcon;
    LPCWSTR      pszMainInstruction;
    LPCWSTR      pszContent;
    UINT         cButtons;
    VC_TDBUTTON *pButtons;
};

static LPCWSTR vc_text(HINSTANCE hinst, LPCWSTR p, WCHAR *buf, int cch)
{
    if(p == NULL) {
        return NULL;
    }
    if(((DWORD)p >> 16) == 0) {                 //MAKEINTRESOURCE string id
        if(LoadStringW(hinst, (UINT)(DWORD)p, buf, cch) <= 0) {
            buf[0] = 0;
        }
        return buf;
    }
    return p;
}

extern "C" {

LONG WINAPI TaskDialogIndirect(const VC_TDCONFIG *cfg, int *pnButton, int *pnRadioButton,
                               BOOL *pfVerificationFlagChecked)
{
    WCHAR   title[256], mainText[512], content[1024], text[1600];
    LPCWSTR t, m, c;
    UINT    type;
    DWORD   cb;
    int     id;

    if(cfg == NULL || cfg->cbSize < sizeof(VC_TDCONFIG)) {
        return VC_E_INVALIDARG;
    }
    t = vc_text(cfg->hInstance, cfg->pszWindowTitle, title, 256);
    m = vc_text(cfg->hInstance, cfg->pszMainInstruction, mainText, 512);
    c = vc_text(cfg->hInstance, cfg->pszContent, content, 1024);

    text[0] = 0;
    if(m) lstrcpynW(text, m, 1600);
    if(m && c && *m && *c) lstrcatW(text, L"\n\n");
    if(c) lstrcpynW(text + lstrlenW(text), c, 1599 - lstrlenW(text));

    cb = cfg->dwCommonButtonFlags;      //TDCBF: OK 1, YES 2, NO 4, CANCEL 8, RETRY 0x10, CLOSE 0x20
    if((cb & 0x0E) == 0x0E)             type = MB_YESNOCANCEL;
    else if((cb & 0x06) == 0x06)        type = MB_YESNO;
    else if((cb & 0x11) == 0x11)        type = MB_OKCANCEL;
    else if((cb & 0x18) == 0x18)        type = MB_RETRYCANCEL;
    else if((cb & 0x09) == 0x09)        type = MB_OKCANCEL;
    else                                type = MB_OK;

    switch((DWORD)cfg->pszMainIcon & 0xFFFF) {
    case 0xFFFF: type |= MB_ICONWARNING;     break;     //TD_WARNING_ICON
    case 0xFFFE: type |= MB_ICONERROR;       break;     //TD_ERROR_ICON
    case 0xFFFD: type |= MB_ICONINFORMATION; break;     //TD_INFORMATION_ICON
    default: break;
    }
    id = MessageBoxW(cfg->hwndParent, text, t ? t : L"", type);
    if(pnButton) {
        *pnButton = (cb == 0 && cfg->cButtons && cfg->pButtons) ? cfg->pButtons[0].nButtonID : id;
    }
    if(pnRadioButton)               *pnRadioButton = 0;
    if(pfVerificationFlagChecked)   *pfVerificationFlagChecked = FALSE;
    return 0;
}

LONG WINAPI LoadIconWithScaleDown(HINSTANCE hinst, LPCWSTR pszName, int cx, int cy, HICON *phico)
{
    HICON h;

    if(phico == NULL) {
        return VC_E_INVALIDARG;
    }
    *phico = NULL;
    h = (HICON)LoadImageW(hinst, pszName, IMAGE_ICON, cx, cy, LR_DEFAULTCOLOR);
    if(h == NULL) {
        DWORD err = GetLastError();
        return err ? (LONG)((err & 0xFFFF) | 0x80070000) : VC_E_NOTIMPL;
    }
    *phico = h;
    return 0;
}

} // extern "C"
