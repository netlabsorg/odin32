/*
 * GDI32: GdiAlphaBlend (the engine behind MSIMG32!AlphaBlend).
 *
 * Software implementation on 32 bpp top-down DIB sections: the source and
 * destination rectangles are copied into temporary DIBs (stretching the source
 * to the destination size), blended per pixel, and copied back.  Only
 * BlendOp AC_SRC_OVER is defined by Windows.  A constant alpha of 255 without
 * per-pixel alpha degenerates to a plain (stretch) blit.
 *
 * LIMITATION: per-pixel alpha (AC_SRC_ALPHA) relies on the 32 bpp DIB
 * section keeping the alpha byte when blitted from the source DC.
 */
#include <os2win.h>
#include <string.h>

static HBITMAP vc_make_dib(HDC hdcRef, int w, int h, DWORD **ppBits)
{
    BITMAPINFO bmi;
    void *bits = NULL;

    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;           //top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    HBITMAP hbm = CreateDIBSection(hdcRef, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    *ppBits = (DWORD *)bits;
    return hbm;
}

extern "C" BOOL WINAPI GdiAlphaBlend(HDC hdcDst, int xDst, int yDst, int wDst, int hDst,
                                     HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc,
                                     DWORD blendFunction)
{
    BYTE op    = (BYTE)(blendFunction);
    BYTE flags = (BYTE)(blendFunction >> 8);
    BYTE sca   = (BYTE)(blendFunction >> 16);
    BYTE fmt   = (BYTE)(blendFunction >> 24);   //AC_SRC_ALPHA = 1

    if(op != 0 || flags != 0 || (fmt & ~1) != 0 || wDst <= 0 || hDst <= 0 || wSrc <= 0 || hSrc <= 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if(sca == 255 && fmt == 0) {
        return StretchBlt(hdcDst, xDst, yDst, wDst, hDst, hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
    }
    if(sca == 0 && fmt == 0) {
        return TRUE;                            //fully transparent: nothing to draw
    }

    DWORD  *pSrc = NULL, *pDst = NULL;
    HDC     hdcS = CreateCompatibleDC(hdcDst), hdcD = CreateCompatibleDC(hdcDst);
    HBITMAP hbmS = vc_make_dib(hdcDst, wDst, hDst, &pSrc);
    HBITMAP hbmD = vc_make_dib(hdcDst, wDst, hDst, &pDst);
    BOOL    ok   = FALSE;

    if(hdcS && hdcD && hbmS && hbmD && pSrc && pDst) {
        HGDIOBJ oldS = SelectObject(hdcS, hbmS);
        HGDIOBJ oldD = SelectObject(hdcD, hbmD);

        StretchBlt(hdcS, 0, 0, wDst, hDst, hdcSrc, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
        BitBlt(hdcD, 0, 0, wDst, hDst, hdcDst, xDst, yDst, SRCCOPY);
        GdiFlush();

        for(int i = 0; i < wDst * hDst; i++) {
            DWORD s = pSrc[i], d = pDst[i];
            DWORD a = (fmt & 1) ? ((s >> 24) & 0xFF) : 255;
            DWORD sr = (s >> 16) & 0xFF, sg = (s >> 8) & 0xFF, sb = s & 0xFF;

            if(fmt & 1) {                               //source is premultiplied
                a  = (a  * sca + 127) / 255;
                sr = (sr * sca + 127) / 255;
                sg = (sg * sca + 127) / 255;
                sb = (sb * sca + 127) / 255;
            }
            else {
                a  = sca;
                sr = (sr * a + 127) / 255;
                sg = (sg * a + 127) / 255;
                sb = (sb * a + 127) / 255;
            }
            DWORD ia = 255 - a;
            DWORD dr = (((d >> 16) & 0xFF) * ia + 127) / 255 + sr;
            DWORD dg = (((d >> 8) & 0xFF) * ia + 127) / 255 + sg;
            DWORD db = ((d & 0xFF) * ia + 127) / 255 + sb;
            pDst[i] = ((dr > 255 ? 255 : dr) << 16) | ((dg > 255 ? 255 : dg) << 8) | (db > 255 ? 255 : db);
        }
        ok = BitBlt(hdcDst, xDst, yDst, wDst, hDst, hdcD, 0, 0, SRCCOPY);
        SelectObject(hdcS, oldS);
        SelectObject(hdcD, oldD);
    }
    if(hbmS) DeleteObject(hbmS);
    if(hbmD) DeleteObject(hbmD);
    if(hdcS) DeleteDC(hdcS);
    if(hdcD) DeleteDC(hdcD);
    return ok;
}
