/*
 * COMDLG32: Vista-style IFileOpenDialog / IFileSaveDialog on top of the classic
 * GetOpenFileNameW / GetSaveFileNameW (and SHBrowseForFolderW for folder picking).
 * Only what applications such as Notepad++ use is implemented: results come back
 * as plain file-system IShellItem objects created by SHCreateItemFromParsingName.
 */
#include <os2win.h>
#include <string.h>
#include <stddef.h>
#include <commdlg.h>
#include <shlobj.h>

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif
typedef struct { LPCWSTR pszName; LPCWSTR pszSpec; } FD_FILTERSPEC;
#define COMDLG_FILTERSPEC FD_FILTERSPEC

#define E_FD_NOTIMPL    ((HRESULT)0x80004001)
#define E_FD_NOINTERFACE ((HRESULT)0x80004002)
#define E_FD_POINTER    ((HRESULT)0x80004003)
#define E_FD_FAIL       ((HRESULT)0x80004005)
#define E_FD_OOM        ((HRESULT)0x8007000E)
#define E_FD_CANCELLED  ((HRESULT)0x800704C7)
#define CLASS_E_NOAGGR  ((HRESULT)0x80040110)
#define CLASS_E_NOCLASS ((HRESULT)0x80040111)

#define FOS_OVERWRITEPROMPT   0x00000002
#define FOS_NOCHANGEDIR       0x00000008
#define FOS_PICKFOLDERS       0x00000020
#define FOS_ALLOWMULTISELECT  0x00000200
#define FOS_PATHMUSTEXIST     0x00000800
#define FOS_FILEMUSTEXIST     0x00001000
#define FOS_CREATEPROMPT      0x00002000
#define FOS_NOREADONLYRETURN  0x00008000

typedef struct { DWORD d1; WORD d2, d3; BYTE d4[8]; } FGUID;

#define DEFGUID(n,a,b,c,d0,d1,d2,d3,d4,d5,d6,d7) \
    static const FGUID n = {a,b,c,{d0,d1,d2,d3,d4,d5,d6,d7}}
DEFGUID(IID_FUnknown,       0x00000000,0x0000,0x0000,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46);
DEFGUID(IID_FClassFactory,  0x00000001,0x0000,0x0000,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46);
DEFGUID(IID_FModalWindow,   0xB4DB1657,0x70D7,0x485E,0x8E,0x3E,0x6F,0xCB,0x5A,0x5C,0x18,0x02);
DEFGUID(IID_FFileDialog,    0x42F85136,0xDB7E,0x439C,0x85,0xF1,0xE4,0x07,0x5D,0x13,0x5F,0xC8);
DEFGUID(IID_FFileOpen,      0xD57C7288,0xD4AD,0x4768,0xBE,0x02,0x9D,0x96,0x95,0x32,0xD9,0x60);
DEFGUID(IID_FFileSave,      0x84BCCD23,0x5FDE,0x4CDB,0xAE,0xA4,0xAF,0x64,0xB8,0x3D,0x78,0xAB);
DEFGUID(IID_FCustomize,     0xE6FDD21A,0x163F,0x4975,0x9C,0x8C,0xA6,0x9F,0x1B,0xA3,0x70,0x34);
DEFGUID(IID_FShellItem,     0x43826D1E,0xE718,0x42EE,0xBC,0x55,0xA1,0xE2,0x61,0xC3,0x7B,0xFE);
DEFGUID(IID_FItemArray,     0xB63EA76D,0x1F85,0x456F,0xA1,0x9C,0x48,0x15,0x9E,0xFA,0x85,0x8B);
DEFGUID(CLSID_FFileOpen,    0xDC1C5A9C,0xE88A,0x4DDE,0xA5,0xA1,0x60,0xF8,0x2A,0x20,0xAE,0xF7);
DEFGUID(CLSID_FFileSave,    0xC0B4E2F3,0xBA21,0x4773,0x8D,0xBA,0x33,0x5E,0xC9,0x46,0xEB,0x8B);

static bool SameG(const void *a, const FGUID &b) { return memcmp(a, &b, sizeof(FGUID)) == 0; }

extern "C" LONG WINAPI SHCreateItemFromParsingName(LPCWSTR pszPath, LPVOID pbc, LPVOID riid, LPVOID *ppv);

static LPVOID TaskAlloc(DWORD cb)
{
    typedef LPVOID (WINAPI *PFN)(DWORD);
    HMODULE h = LoadLibraryA("ole32.dll");
    PFN pfn = h ? (PFN)GetProcAddress(h, "CoTaskMemAlloc") : NULL;
    return pfn ? pfn(cb) : NULL;
}

//---- IUnknown-only vtable head ---------------------------------------------------------
struct IUnk;
struct IUnkVtbl {
    HRESULT (WINAPI *QueryInterface)(IUnk*, const void*, void**);
    ULONG   (WINAPI *AddRef)(IUnk*);
    ULONG   (WINAPI *Release)(IUnk*);
};
struct IUnk { const IUnkVtbl *vt; };

//---- IShellItemArray -------------------------------------------------------------------
#define MAXRES 512
struct ItemArray {
    const void * const *vt;
    LONG   ref;
    DWORD  count;
    IUnk  *items[MAXRES];
};

static HRESULT WINAPI Ia_QI(ItemArray *This, const void *riid, void **ppv)
{
    if(!ppv) return E_FD_POINTER;
    if(SameG(riid, IID_FUnknown) || SameG(riid, IID_FItemArray)) {
        This->ref++;
        *ppv = This;
        return S_OK;
    }
    *ppv = NULL;
    return E_FD_NOINTERFACE;
}
static ULONG WINAPI Ia_AddRef(ItemArray *This) { return ++This->ref; }
static ULONG WINAPI Ia_Release(ItemArray *This)
{
    ULONG r = --This->ref;
    if(r == 0) {
        for(DWORD i = 0; i < This->count; i++)
            if(This->items[i]) This->items[i]->vt->Release(This->items[i]);
        HeapFree(GetProcessHeap(), 0, This);
    }
    return r;
}
static HRESULT WINAPI Ia_Nimpl4(ItemArray*, void*, void*, void*, void*) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Ia_Nimpl2(ItemArray*, void*, void*) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Ia_Nimpl1(ItemArray*, void*) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Ia_Nimpl3(ItemArray*, void*, void*, void*) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Ia_GetAttributes(ItemArray*, DWORD, DWORD, DWORD*) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Ia_GetCount(ItemArray *This, DWORD *pc)
{
    if(!pc) return E_FD_POINTER;
    *pc = This->count;
    return S_OK;
}
static HRESULT WINAPI Ia_GetItemAt(ItemArray *This, DWORD i, IUnk **ppsi)
{
    if(!ppsi) return E_FD_POINTER;
    *ppsi = NULL;
    if(i >= This->count) return (HRESULT)0x80070057;
    This->items[i]->vt->AddRef(This->items[i]);
    *ppsi = This->items[i];
    return S_OK;
}
static const void * const g_ItemArrayVtbl[10] = {
    (void*)Ia_QI, (void*)Ia_AddRef, (void*)Ia_Release,
    (void*)Ia_Nimpl4 /*BindToHandler*/, (void*)Ia_Nimpl3 /*GetPropertyStore*/,
    (void*)Ia_Nimpl3 /*GetPropertyDescriptionList*/, (void*)Ia_GetAttributes,
    (void*)Ia_GetCount, (void*)Ia_GetItemAt, (void*)Ia_Nimpl1 /*EnumItems*/
};

//---- the file dialog object ------------------------------------------------------------
struct FDlg {
    const void * const *vtDlg;      // IFileOpenDialog / IFileSaveDialog / IFileDialog
    const void * const *vtCust;     // IFileDialogCustomize
    LONG     ref;
    bool     save;
    DWORD    options;
    UINT     nTypes;
    LPWSTR   typeNames[64];
    LPWSTR   typeSpecs[64];
    UINT     typeIndex;             // 1-based
    WCHAR    defExt[32];
    WCHAR    title[260];
    WCHAR    okLabel[64];
    WCHAR    fileName[MAX_PATH];
    WCHAR    folder[MAX_PATH];
    WCHAR    defFolder[MAX_PATH];
    DWORD    nResults;
    LPWSTR   results[MAXRES];
};

static FDlg *FromCust(void *p) { return (FDlg *)((char *)p - offsetof(FDlg, vtCust)); }

static LPWSTR DupW(LPCWSTR s)
{
    DWORD n = lstrlenW(s) + 1;
    LPWSTR d = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, n * sizeof(WCHAR));
    if(d) memcpy(d, s, n * sizeof(WCHAR));
    return d;
}

static void FreeResults(FDlg *d)
{
    for(DWORD i = 0; i < d->nResults; i++) HeapFree(GetProcessHeap(), 0, d->results[i]);
    d->nResults = 0;
}

static HRESULT WINAPI Fd_QI(FDlg *This, const void *riid, void **ppv)
{
    if(!ppv) return E_FD_POINTER;
    *ppv = NULL;
    if(SameG(riid, IID_FUnknown) || SameG(riid, IID_FModalWindow) || SameG(riid, IID_FFileDialog) ||
       SameG(riid, This->save ? IID_FFileSave : IID_FFileOpen))
        *ppv = &This->vtDlg;
    else if(SameG(riid, IID_FCustomize))
        *ppv = &This->vtCust;
    else
        return E_FD_NOINTERFACE;
    This->ref++;
    return S_OK;
}
static ULONG WINAPI Fd_AddRef(FDlg *This) { return ++This->ref; }
static ULONG WINAPI Fd_Release(FDlg *This)
{
    ULONG r = --This->ref;
    if(r == 0) {
        FreeResults(This);
        for(UINT i = 0; i < This->nTypes; i++) {
            HeapFree(GetProcessHeap(), 0, This->typeNames[i]);
            HeapFree(GetProcessHeap(), 0, This->typeSpecs[i]);
        }
        HeapFree(GetProcessHeap(), 0, This);
    }
    return r;
}

static bool ItemPath(IUnk *item, WCHAR *out, DWORD cch)
{
    typedef HRESULT (WINAPI *PGDN)(IUnk*, DWORD, LPWSTR*);
    LPWSTR s = NULL;
    PGDN gdn;
    out[0] = 0;
    if(!item) return false;
    gdn = (PGDN)((void * const *)item->vt)[5];
    if(gdn(item, 0x80058000, &s) != S_OK || !s) return false;
    if((DWORD)lstrlenW(s) < cch) lstrcpyW(out, s);
    return out[0] != 0;
}

static void AddResult(FDlg *d, LPCWSTR path)
{
    if(d->nResults < MAXRES) d->results[d->nResults++] = DupW(path);
}

static HRESULT ShowFolderPicker(FDlg *d, HWND hwnd)
{
    BROWSEINFOW bi;
    WCHAR       disp[MAX_PATH], path[MAX_PATH];
    LPITEMIDLIST pidl;
    memset(&bi, 0, sizeof(bi));
    bi.hwndOwner = hwnd;
    bi.pszDisplayName = disp;
    bi.lpszTitle = d->title[0] ? d->title : NULL;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    pidl = SHBrowseForFolderW(&bi);
    if(!pidl) return E_FD_CANCELLED;
    if(!SHGetPathFromIDListW(pidl, path)) return E_FD_CANCELLED;
    AddResult(d, path);
    return S_OK;
}

static HRESULT WINAPI Fd_Show(FDlg **pThis, HWND hwnd)
{
    FDlg *d = (FDlg *)pThis;
    OPENFILENAMEW ofn;
    LPWSTR buf, filter, p;
    DWORD  cchBuf = 32768, cchFilter = 8, flags;
    BOOL   ok;
    UINT   i;

    FreeResults(d);
    if(d->options & FOS_PICKFOLDERS) return ShowFolderPicker(d, hwnd);

    for(i = 0; i < d->nTypes; i++) cchFilter += lstrlenW(d->typeNames[i]) + lstrlenW(d->typeSpecs[i]) + 2;
    filter = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchFilter * sizeof(WCHAR));
    buf    = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchBuf * sizeof(WCHAR));
    if(!filter || !buf) {
        HeapFree(GetProcessHeap(), 0, filter);
        HeapFree(GetProcessHeap(), 0, buf);
        return E_FD_OOM;
    }
    for(p = filter, i = 0; i < d->nTypes; i++) {
        lstrcpyW(p, d->typeNames[i]); p += lstrlenW(p) + 1;
        lstrcpyW(p, d->typeSpecs[i]); p += lstrlenW(p) + 1;
    }
    lstrcpyW(buf, d->fileName);

    flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_ENABLESIZING;
    if(d->options & FOS_OVERWRITEPROMPT)  flags |= OFN_OVERWRITEPROMPT;
    if(d->options & FOS_NOCHANGEDIR)      flags |= OFN_NOCHANGEDIR;
    if(d->options & FOS_ALLOWMULTISELECT) flags |= OFN_ALLOWMULTISELECT;
    if(d->options & FOS_PATHMUSTEXIST)    flags |= OFN_PATHMUSTEXIST;
    if(d->options & FOS_FILEMUSTEXIST)    flags |= OFN_FILEMUSTEXIST;
    if(d->options & FOS_CREATEPROMPT)     flags |= OFN_CREATEPROMPT;
    if(d->options & FOS_NOREADONLYRETURN) flags |= OFN_NOREADONLYRETURN;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = hwnd;
    ofn.lpstrFilter     = d->nTypes ? filter : NULL;
    ofn.nFilterIndex    = d->nTypes ? d->typeIndex : 0;
    ofn.lpstrFile       = buf;
    ofn.nMaxFile        = cchBuf;
    ofn.lpstrInitialDir = d->folder[0] ? d->folder : (d->defFolder[0] ? d->defFolder : NULL);
    ofn.lpstrTitle      = d->title[0] ? d->title : NULL;
    ofn.lpstrDefExt     = d->defExt[0] ? d->defExt : NULL;
    ofn.Flags           = flags;

    ok = d->save ? GetSaveFileNameW(&ofn) : GetOpenFileNameW(&ofn);
    if(ok) {
        d->typeIndex = ofn.nFilterIndex ? ofn.nFilterIndex : d->typeIndex;
        if(buf[lstrlenW(buf) + 1] == 0) {
            AddResult(d, buf);                       // single full path
        }
        else {                                       // dir\0name1\0name2\0\0
            WCHAR full[MAX_PATH * 2];
            LPWSTR dir = buf, name = buf + lstrlenW(buf) + 1;
            while(*name) {
                lstrcpyW(full, dir);
                i = lstrlenW(full);
                if(i && full[i - 1] != '\\') { full[i++] = '\\'; full[i] = 0; }
                lstrcatW(full, name);
                AddResult(d, full);
                name += lstrlenW(name) + 1;
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, filter);
    HeapFree(GetProcessHeap(), 0, buf);
    return ok ? S_OK : E_FD_CANCELLED;
}

static HRESULT WINAPI Fd_SetFileTypes(FDlg **pThis, UINT c, const COMDLG_FILTERSPEC *spec)
{
    FDlg *d = (FDlg *)pThis;
    for(UINT i = 0; i < d->nTypes; i++) {
        HeapFree(GetProcessHeap(), 0, d->typeNames[i]);
        HeapFree(GetProcessHeap(), 0, d->typeSpecs[i]);
    }
    d->nTypes = 0;
    if(c > 64) c = 64;
    for(UINT i = 0; i < c; i++) {
        d->typeNames[i] = DupW(spec[i].pszName ? spec[i].pszName : L"");
        d->typeSpecs[i] = DupW(spec[i].pszSpec ? spec[i].pszSpec : L"*.*");
    }
    d->nTypes = c;
    return S_OK;
}
static HRESULT WINAPI Fd_SetFileTypeIndex(FDlg **pThis, UINT i) { ((FDlg *)pThis)->typeIndex = i ? i : 1; return S_OK; }
static HRESULT WINAPI Fd_GetFileTypeIndex(FDlg **pThis, UINT *pi)
{
    if(!pi) return E_FD_POINTER;
    *pi = ((FDlg *)pThis)->typeIndex;
    return S_OK;
}
static HRESULT WINAPI Fd_Advise(FDlg **, void *, DWORD *pdw) { if(pdw) *pdw = 1; return S_OK; }
static HRESULT WINAPI Fd_Unadvise(FDlg **, DWORD) { return S_OK; }
static HRESULT WINAPI Fd_SetOptions(FDlg **pThis, DWORD o) { ((FDlg *)pThis)->options = o; return S_OK; }
static HRESULT WINAPI Fd_GetOptions(FDlg **pThis, DWORD *po)
{
    if(!po) return E_FD_POINTER;
    *po = ((FDlg *)pThis)->options;
    return S_OK;
}
static HRESULT WINAPI Fd_SetDefaultFolder(FDlg **pThis, IUnk *psi)
{
    FDlg *d = (FDlg *)pThis;
    ItemPath(psi, d->defFolder, MAX_PATH);
    return S_OK;
}
static HRESULT WINAPI Fd_SetFolder(FDlg **pThis, IUnk *psi)
{
    FDlg *d = (FDlg *)pThis;
    ItemPath(psi, d->folder, MAX_PATH);
    return S_OK;
}
static HRESULT WINAPI Fd_GetFolder(FDlg **pThis, IUnk **ppsi)
{
    FDlg *d = (FDlg *)pThis;
    if(!ppsi) return E_FD_POINTER;
    *ppsi = NULL;
    if(!d->folder[0] && !d->defFolder[0]) return E_FD_FAIL;
    return SHCreateItemFromParsingName(d->folder[0] ? d->folder : d->defFolder, NULL, (LPVOID)&IID_FShellItem, (LPVOID *)ppsi);
}
static HRESULT WINAPI Fd_GetResult(FDlg **pThis, IUnk **ppsi)
{
    FDlg *d = (FDlg *)pThis;
    if(!ppsi) return E_FD_POINTER;
    *ppsi = NULL;
    if(!d->nResults) return E_FD_FAIL;
    return SHCreateItemFromParsingName(d->results[0], NULL, (LPVOID)&IID_FShellItem, (LPVOID *)ppsi);
}
static HRESULT WINAPI Fd_GetFileName(FDlg **pThis, LPWSTR *ppsz)
{
    FDlg *d = (FDlg *)pThis;
    const WCHAR *src = d->nResults ? d->results[0] : d->fileName, *p;
    LPWSTR out;
    if(!ppsz) return E_FD_POINTER;
    for(p = src + lstrlenW(src); p > src && p[-1] != '\\' && p[-1] != '/'; p--) ;
    out = (LPWSTR)TaskAlloc((lstrlenW(p) + 1) * sizeof(WCHAR));
    if(!out) { *ppsz = NULL; return E_FD_OOM; }
    lstrcpyW(out, p);
    *ppsz = out;
    return S_OK;
}
static HRESULT WINAPI Fd_SetFileName(FDlg **pThis, LPCWSTR s)
{
    FDlg *d = (FDlg *)pThis;
    lstrcpynW(d->fileName, s ? s : L"", MAX_PATH);
    return S_OK;
}
static HRESULT WINAPI Fd_SetTitle(FDlg **pThis, LPCWSTR s)
{
    lstrcpynW(((FDlg *)pThis)->title, s ? s : L"", 260);
    return S_OK;
}
static HRESULT WINAPI Fd_SetOkLabel(FDlg **pThis, LPCWSTR s)
{
    lstrcpynW(((FDlg *)pThis)->okLabel, s ? s : L"", 64);
    return S_OK;
}
static HRESULT WINAPI Fd_SetLabel(FDlg **, LPCWSTR) { return S_OK; }
static HRESULT WINAPI Fd_AddPlace(FDlg **, void *, DWORD) { return S_OK; }
static HRESULT WINAPI Fd_SetDefaultExtension(FDlg **pThis, LPCWSTR s)
{
    lstrcpynW(((FDlg *)pThis)->defExt, s ? s : L"", 32);
    return S_OK;
}
static HRESULT WINAPI Fd_Close(FDlg **, HRESULT) { return S_OK; }
static HRESULT WINAPI Fd_SetClientGuid(FDlg **, const void *) { return S_OK; }
static HRESULT WINAPI Fd_ClearClientData(FDlg **) { return S_OK; }
static HRESULT WINAPI Fd_SetFilter(FDlg **, void *) { return S_OK; }
static HRESULT WINAPI Fd_GetResults(FDlg **pThis, ItemArray **ppa)
{
    FDlg *d = (FDlg *)pThis;
    ItemArray *a;
    if(!ppa) return E_FD_POINTER;
    *ppa = NULL;
    if(!d->nResults) return E_FD_FAIL;
    a = (ItemArray *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ItemArray));
    if(!a) return E_FD_OOM;
    a->vt = g_ItemArrayVtbl;
    a->ref = 1;
    for(DWORD i = 0; i < d->nResults; i++) {
        IUnk *it = NULL;
        if(SHCreateItemFromParsingName(d->results[i], NULL, (LPVOID)&IID_FShellItem, (LPVOID *)&it) == S_OK)
            a->items[a->count++] = it;
    }
    *ppa = a;
    return S_OK;
}
static HRESULT WINAPI Fd_Nimpl(FDlg **) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Fd_Nimpl1(FDlg **, void *) { return E_FD_NOTIMPL; }
static HRESULT WINAPI Fd_Nimpl2(FDlg **, void *, void *) { return E_FD_NOTIMPL; }

// IUnknown adapters for the customize sub-object
static HRESULT WINAPI Cu_QI(void **pThis, const void *riid, void **ppv) { return Fd_QI(FromCust(pThis), riid, ppv); }
static ULONG   WINAPI Cu_AddRef(void **pThis) { return Fd_AddRef(FromCust(pThis)); }
static ULONG   WINAPI Cu_Release(void **pThis) { return Fd_Release(FromCust(pThis)); }
static HRESULT WINAPI Cu_Ok(void **) { return S_OK; }
static HRESULT WINAPI Cu_Ok1(void **, DWORD) { return S_OK; }
static HRESULT WINAPI Cu_Ok2(void **, DWORD, DWORD) { return S_OK; }
static HRESULT WINAPI Cu_Ok3(void **, DWORD, DWORD, DWORD) { return S_OK; }
static HRESULT WINAPI Cu_Ok4(void **, DWORD, DWORD, DWORD, DWORD) { return S_OK; }
static HRESULT WINAPI Cu_GetState(void **, DWORD, DWORD *pst) { if(pst) *pst = 3; return S_OK; }
static HRESULT WINAPI Cu_GetCheck(void **, DWORD, BOOL *pb) { if(pb) *pb = FALSE; return S_OK; }
static HRESULT WINAPI Cu_GetEdit(void **, DWORD, LPWSTR *ps)
{
    if(ps) { *ps = (LPWSTR)TaskAlloc(sizeof(WCHAR)); if(*ps) **ps = 0; }
    return S_OK;
}
static HRESULT WINAPI Cu_GetState3(void **, DWORD, DWORD, DWORD *pst) { if(pst) *pst = 3; return S_OK; }
static HRESULT WINAPI Cu_GetSel(void **, DWORD, DWORD *pi) { if(pi) *pi = 0; return S_OK; }

// slot order of IFileOpenDialog / IFileSaveDialog (prefix shared; Open uses first 29, Save all 32)
static const void * const g_FileDlgVtbl[32] = {
    (void*)Fd_QI, (void*)Fd_AddRef, (void*)Fd_Release,
    (void*)Fd_Show,
    (void*)Fd_SetFileTypes, (void*)Fd_SetFileTypeIndex, (void*)Fd_GetFileTypeIndex,
    (void*)Fd_Advise, (void*)Fd_Unadvise, (void*)Fd_SetOptions, (void*)Fd_GetOptions,
    (void*)Fd_SetDefaultFolder, (void*)Fd_SetFolder, (void*)Fd_GetFolder,
    (void*)Fd_GetResult /*GetCurrentSelection*/, (void*)Fd_SetFileName, (void*)Fd_GetFileName,
    (void*)Fd_SetTitle, (void*)Fd_SetOkLabel, (void*)Fd_SetLabel,
    (void*)Fd_GetResult, (void*)Fd_AddPlace, (void*)Fd_SetDefaultExtension, (void*)Fd_Close,
    (void*)Fd_SetClientGuid, (void*)Fd_ClearClientData, (void*)Fd_SetFilter,
    (void*)Fd_GetResults, (void*)Fd_GetResults /*GetSelectedItems*/,
    (void*)Fd_Nimpl1 /*SetSaveAsItem / SetProperties*/, (void*)Fd_Nimpl1, (void*)Fd_Nimpl2
};

// IFileDialogCustomize: 3 + 27 slots, all harmless
static const void * const g_CustVtbl[30] = {
    (void*)Cu_QI, (void*)Cu_AddRef, (void*)Cu_Release,
    (void*)Cu_Ok1 /*EnableOpenDropDown*/, (void*)Cu_Ok2 /*AddMenu*/, (void*)Cu_Ok2 /*AddPushButton*/,
    (void*)Cu_Ok1 /*AddComboBox*/, (void*)Cu_Ok1 /*AddRadioButtonList*/, (void*)Cu_Ok3 /*AddCheckButton*/,
    (void*)Cu_Ok2 /*AddEditBox*/, (void*)Cu_Ok1 /*AddSeparator*/, (void*)Cu_Ok2 /*AddText*/,
    (void*)Cu_Ok2 /*SetControlLabel*/, (void*)Cu_GetState, (void*)Cu_Ok2 /*SetControlState*/,
    (void*)Cu_GetEdit, (void*)Cu_Ok2 /*SetEditBoxText*/, (void*)Cu_GetCheck,
    (void*)Cu_Ok2 /*SetCheckButtonState*/, (void*)Cu_Ok3 /*AddControlItem*/, (void*)Cu_Ok2 /*RemoveControlItem*/,
    (void*)Cu_Ok1 /*RemoveAllControlItems*/, (void*)Cu_GetState3 /*GetControlItemState*/,
    (void*)Cu_Ok3 /*SetControlItemState*/, (void*)Cu_GetSel, (void*)Cu_Ok2 /*SetSelectedControlItem*/,
    (void*)Cu_Ok2 /*StartVisualGroup*/, (void*)Cu_Ok /*EndVisualGroup*/, (void*)Cu_Ok1 /*MakeProminent*/,
    (void*)Cu_Ok3 /*SetControlItemText*/
};

//---- class factory ---------------------------------------------------------------------
struct Factory { const void * const *vt; bool save; };

static HRESULT WINAPI Cf_QI(Factory *This, const void *riid, void **ppv)
{
    if(!ppv) return E_FD_POINTER;
    if(SameG(riid, IID_FUnknown) || SameG(riid, IID_FClassFactory)) { *ppv = This; return S_OK; }
    *ppv = NULL;
    return E_FD_NOINTERFACE;
}
static ULONG WINAPI Cf_AddRef(Factory *) { return 2; }
static ULONG WINAPI Cf_Release(Factory *) { return 1; }
static HRESULT WINAPI Cf_CreateInstance(Factory *This, void *pOuter, const void *riid, void **ppv)
{
    FDlg *d;
    HRESULT hr;
    if(!ppv) return E_FD_POINTER;
    *ppv = NULL;
    if(pOuter) return CLASS_E_NOAGGR;
    d = (FDlg *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FDlg));
    if(!d) return E_FD_OOM;
    d->vtDlg = g_FileDlgVtbl;
    d->vtCust = g_CustVtbl;
    d->ref = 1;
    d->save = This->save;
    d->typeIndex = 1;
    d->options = d->save ? (FOS_OVERWRITEPROMPT | FOS_PATHMUSTEXIST) : (FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST);
    hr = Fd_QI(d, riid, ppv);
    Fd_Release(d);
    return hr;
}
static HRESULT WINAPI Cf_LockServer(Factory *, BOOL) { return S_OK; }

static const void * const g_FactoryVtbl[5] = {
    (void*)Cf_QI, (void*)Cf_AddRef, (void*)Cf_Release, (void*)Cf_CreateInstance, (void*)Cf_LockServer
};
static Factory g_OpenFactory = { g_FactoryVtbl, false };
static Factory g_SaveFactory = { g_FactoryVtbl, true };

extern "C" HRESULT WINAPI ItemDlgGetClassObject(const void *rclsid, const void *riid, void **ppv)
{
    Factory *f;
    if(!ppv) return E_FD_POINTER;
    *ppv = NULL;
    if(SameG(rclsid, CLSID_FFileOpen)) f = &g_OpenFactory;
    else if(SameG(rclsid, CLSID_FFileSave)) f = &g_SaveFactory;
    else return CLASS_E_NOCLASS;
    return Cf_QI(f, riid, ppv);
}
