/*
 * windowscodecs - Windows Imaging Component stub: WICConvertBitmapSource fails.
 */
#include <os2win.h>

#define FF_E_NOTIMPL ((HRESULT)0x80004001)

extern "C" {

HRESULT WINAPI WICConvertBitmapSource(PVOID dstFormat, PVOID src, PVOID *dst)
{
    if(dst)
        *dst = NULL;
    return FF_E_NOTIMPL;
}

} // extern "C"
