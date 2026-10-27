/*
 * SHELL32 shims for the Vista+ shell item / PIDL helpers.  The OS/2 host has
 * no shell namespace of this kind, so they fail with E_NOTIMPL and callers
 * take their fallback (plain file operations).
 */
#include <os2win.h>
#include <string.h>

#define VC_E_NOTIMPL    ((LONG)0x80004001)

extern "C" {

/* ---- minimal file-system IShellItem ------------------------------------------------ */
typedef struct { DWORD d1; WORD d2, d3; BYTE d4[8]; } SGUID;
static const SGUID IID_SIUnknown   = {0x00000000,0x0000,0x0000,{0xC0,0,0,0,0,0,0,0x46}};
static const SGUID IID_SIShellItem = {0x43826D1E,0xE718,0x42EE,{0xBC,0x55,0xA1,0xE2,0x61,0xC3,0x7B,0xFE}};

struct SItem;
struct SItemVtbl {
    LONG  (WINAPI *QueryInterface)(SItem*, const SGUID*, void**);
    ULONG (WINAPI *AddRef)(SItem*);
    ULONG (WINAPI *Release)(SItem*);
    LONG  (WINAPI *BindToHandler)(SItem*, void*, const SGUID*, const SGUID*, void**);
    LONG  (WINAPI *GetParent)(SItem*, SItem**);
    LONG  (WINAPI *GetDisplayName)(SItem*, DWORD, LPWSTR*);
    LONG  (WINAPI *GetAttributes)(SItem*, DWORD, DWORD*);
    LONG  (WINAPI *Compare)(SItem*, SItem*, DWORD, int*);
};
struct SItem { const SItemVtbl *vt; LONG ref; WCHAR path[MAX_PATH + 1]; };

LONG WINAPI SHCreateItemFromParsingName(LPCWSTR pszPath, LPVOID pbc, LPVOID riid, LPVOID *ppv);
static bool SameGuid(const SGUID *a, const SGUID *b) { return memcmp(a, b, sizeof(SGUID)) == 0; }

static LPWSTR SiAlloc(DWORD cb)
{
    typedef LPVOID (WINAPI *PFN)(DWORD);
    HMODULE h = LoadLibraryA("ole32.dll");
    PFN pfn = h ? (PFN)GetProcAddress(h, "CoTaskMemAlloc") : NULL;
    return pfn ? (LPWSTR)pfn(cb) : NULL;
}

static LONG WINAPI Si_QI(SItem *This, const SGUID *riid, void **ppv)
{
    if(!ppv) return (LONG)0x80004003;
    if(SameGuid(riid, &IID_SIUnknown) || SameGuid(riid, &IID_SIShellItem)) {
        This->vt->AddRef(This);
        *ppv = This;
        return 0;
    }
    *ppv = NULL;
    return (LONG)0x80004002;
}
static ULONG WINAPI Si_AddRef(SItem *This) { return ++This->ref; }
static ULONG WINAPI Si_Release(SItem *This)
{
    ULONG r = --This->ref;
    if(r == 0) HeapFree(GetProcessHeap(), 0, This);
    return r;
}
static LONG WINAPI Si_Bind(SItem*, void*, const SGUID*, const SGUID*, void **ppv) { if(ppv) *ppv = NULL; return VC_E_NOTIMPL; }
static LONG WINAPI Si_GetParent(SItem *This, SItem **ppsi)
{
    WCHAR dir[MAX_PATH + 1];
    int   i;
    if(!ppsi) return (LONG)0x80004003;
    *ppsi = NULL;
    lstrcpyW(dir, This->path);
    for(i = lstrlenW(dir) - 1; i > 2; i--)
        if(dir[i] == '\\' || dir[i] == '/') break;
    if(i <= 2) return (LONG)0x80070002;
    dir[i] = 0;
    return SHCreateItemFromParsingName(dir, NULL, (LPVOID)&IID_SIShellItem, (LPVOID*)ppsi);
}
static LONG WINAPI Si_GetDisplayName(SItem *This, DWORD sigdn, LPWSTR *ppsz)
{
    const WCHAR *src = This->path;
    LPWSTR out;
    if(!ppsz) return (LONG)0x80004003;
    *ppsz = NULL;
    if(sigdn == 0 || sigdn == 0x8001c001 /*PARENTRELATIVE*/ || sigdn == 0x80018001 /*PARENTRELATIVEPARSING*/
       || sigdn == 0x80031001 /*PARENTRELATIVEEDITING*/ || sigdn == 0x8007c001 /*FORADDRESSBAR*/) {
        const WCHAR *p;
        for(p = src + lstrlenW(src); p > src && p[-1] != '\\' && p[-1] != '/'; p--) ;
        if(*p) src = p;
    }
    out = SiAlloc((lstrlenW(src) + 1) * sizeof(WCHAR));
    if(!out) return (LONG)0x8007000E;
    lstrcpyW(out, src);
    *ppsz = out;
    return 0;
}
static LONG WINAPI Si_GetAttributes(SItem *This, DWORD mask, DWORD *pattr)
{
    DWORD a = GetFileAttributesW(This->path);
    DWORD r = 0;
    if(!pattr) return (LONG)0x80004003;
    if(a == 0xFFFFFFFF) { *pattr = 0; return 1; }
    r |= 0x40000000 | 0x20000000;                     /* SFGAO_FILESYSTEM | SFGAO_STREAM-ish */
    if(a & FILE_ATTRIBUTE_DIRECTORY) r |= 0x20000000 | 0x00000020; /* FOLDER | CANRENAME-ish */
    *pattr = r & mask;
    return (*pattr == mask) ? 0 : 1;
}
static LONG WINAPI Si_Compare(SItem *This, SItem *other, DWORD, int *piOrder)
{
    LPWSTR n = NULL;
    int c;
    if(!other || !piOrder) return (LONG)0x80004003;
    if(other->vt->GetDisplayName(other, 0x80058000, &n) != 0 || !n) return (LONG)0x80004005;
    c = lstrcmpiW(This->path, n);
    *piOrder = c;
    return c == 0 ? 0 : 1;
}
static const SItemVtbl g_SItemVtbl = { Si_QI, Si_AddRef, Si_Release, Si_Bind, Si_GetParent,
                                       Si_GetDisplayName, Si_GetAttributes, Si_Compare };

LONG WINAPI SHCreateItemFromParsingName(LPCWSTR pszPath, LPVOID pbc, LPVOID riid, LPVOID *ppv)
{
    SItem *it;
    LONG   hr;
    if(!ppv) return (LONG)0x80004003;
    *ppv = NULL;
    if(!pszPath || lstrlenW(pszPath) > MAX_PATH) return (LONG)0x80070057;
    it = (SItem *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SItem));
    if(!it) return (LONG)0x8007000E;
    it->vt = &g_SItemVtbl;
    it->ref = 1;
    lstrcpyW(it->path, pszPath);
    hr = Si_QI(it, riid ? (const SGUID *)riid : &IID_SIShellItem, ppv);
    Si_Release(it);
    return hr;
}

LONG WINAPI SHParseDisplayName(LPCWSTR pszName, LPVOID pbc, LPVOID *ppidl,
                               DWORD sfgaoIn, DWORD *psfgaoOut)
{
    if(ppidl) *ppidl = NULL;
    return VC_E_NOTIMPL;
}

LONG WINAPI SHOpenFolderAndSelectItems(LPVOID pidlFolder, UINT cidl, LPVOID *apidl, DWORD dwFlags)
{
    return VC_E_NOTIMPL;
}

} // extern "C"
