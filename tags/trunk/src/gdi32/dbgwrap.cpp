#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winreg.h>
#include <winnt.h>
#include <wincon.h>
#include <winthunk.h>
#include <winuser.h>
#include <ddeml.h>
#include <dde.h>
#include <winnls.h>
#include <ntddk.h>
#include <heapstring.h>

#define DBG_LOCALLOG    DBG_trace
#include <dbglocal.h>

#define DBGWRAP_MODULE "GDI32"
#include <dbgwrap.h>


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_text

DEBUGWRAP32(ExtTextOutA)
DEBUGWRAP32(ExtTextOutW)
DEBUGWRAP12(PolyTextOutA)
DEBUGWRAP12(PolyTextOutW)
DEBUGWRAP20(TextOutA)
DEBUGWRAP20(TextOutW)
DEBUGWRAP28(GetTextExtentExPointA)
DEBUGWRAP28(GetTextExtentExPointW)
DEBUGWRAP16(GetTextExtentPoint32A)
DEBUGWRAP16(GetTextExtentPoint32W)
DEBUGWRAP16(GetTextExtentPointA)
DEBUGWRAP16(GetTextExtentPointW)



#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_blit


DEBUGWRAP36(BitBlt)
DEBUGWRAP44(StretchBlt)
DEBUGWRAP52(StretchDIBits)
DEBUGWRAP48(MaskBlt)
DEBUGWRAP24(PatBlt)
DEBUGWRAP40(PlgBlt)
DEBUGWRAP4(GetStretchBltMode)
DEBUGWRAP8(SetStretchBltMode)
DEBUGWRAP48(SetDIBitsToDevice)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_font

DEBUGWRAP12(GetTextFaceA)
DEBUGWRAP12(GetTextFaceW)
DEBUGWRAP56(CreateFontA)
DEBUGWRAP12(TranslateCharsetInfo)
DEBUGWRAP4(CreateFontIndirectA)
DEBUGWRAP4(CreateFontIndirectW)
DEBUGWRAP56(CreateFontW)
DEBUGWRAP16(CreateScalableFontResourceA)
DEBUGWRAP16(CreateScalableFontResourceW)
DEBUGWRAP4(AddFontResourceA)
DEBUGWRAP4(AddFontResourceW)
DEBUGWRAP16(EnumFontFamiliesA)
DEBUGWRAP20(EnumFontFamiliesExA)
DEBUGWRAP20(EnumFontFamiliesExW)
DEBUGWRAP16(EnumFontFamiliesW)
DEBUGWRAP16(EnumFontsA)
DEBUGWRAP16(EnumFontsW)
DEBUGWRAP20(GetFontData)
DEBUGWRAP4(GetFontLanguageInfo)
//;    GetFontResourceInfo        = _GetFontResourceInfo@??     @259
DEBUGWRAP28(GetGlyphOutlineA)
DEBUGWRAP28(GetGlyphOutlineW)
DEBUGWRAP4(RemoveFontResourceA)
DEBUGWRAP4(RemoveFontResourceW)
//;    SetFontEnumeration         = _SetFontEnumeration@??      @385


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_region

DEBUGWRAP20(IntersectClipRect)
DEBUGWRAP8(GetRgnBox)
DEBUGWRAP8(GetMetaRgn)
DEBUGWRAP8(GetClipRgn)
DEBUGWRAP20(FrameRgn)
DEBUGWRAP12(FillRgn)
DEBUGWRAP12(GetRandomRgn)
DEBUGWRAP12(ExtSelectClipRgn)
DEBUGWRAP8(EqualRgn)
DEBUGWRAP16(CreatePolyPolygonRgn)
DEBUGWRAP12(CreatePolygonRgn)
DEBUGWRAP16(CreateRectRgn)
DEBUGWRAP4(CreateRectRgnIndirect)
DEBUGWRAP24(CreateRoundRectRgn)
DEBUGWRAP16(CombineRgn)
DEBUGWRAP16(CreateEllipticRgn)
DEBUGWRAP4(CreateEllipticRgnIndirect)
DEBUGWRAP8(PaintRgn)
DEBUGWRAP12(ExtCreateRegion)
DEBUGWRAP12(GetRegionData)
DEBUGWRAP4(PathToRegion)
DEBUGWRAP12(PtInRegion)
DEBUGWRAP8(RectInRegion)
DEBUGWRAP20(SetRectRgn)
DEBUGWRAP8(InvertRgn)
DEBUGWRAP12(OffsetClipRgn)
DEBUGWRAP12(OffsetRgn)
DEBUGWRAP8(SelectClipRgn)
DEBUGWRAP4(SetMetaRgn)
DEBUGWRAP8(GetClipBox)



#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_printer


DEBUGWRAP8(SetAbortProc)
DEBUGWRAP4(AbortDoc)
DEBUGWRAP8(StartDocA)
DEBUGWRAP8(StartDocW)
DEBUGWRAP4(StartPage)
DEBUGWRAP4(EndDoc)
DEBUGWRAP4(EndPage)



#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_palette

DEBUGWRAP16(AnimatePalette)
DEBUGWRAP4(CreateHalftonePalette)
DEBUGWRAP4(CreatePalette)
DEBUGWRAP8(GetNearestPaletteIndex)
DEBUGWRAP16(GetPaletteEntries)
DEBUGWRAP16(GetSystemPaletteEntries)
DEBUGWRAP4(GetSystemPaletteUse)
DEBUGWRAP8(SetSystemPaletteUse)
DEBUGWRAP4(RealizePalette)
DEBUGWRAP8(ResizePalette)
DEBUGWRAP12(SelectPalette)
DEBUGWRAP16(SetPaletteEntries)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_icm

DEBUGWRAP12(EnumICMProfilesA)
DEBUGWRAP12(EnumICMProfilesW)
NODEF_DEBUGWRAP12(GetICMProfileA)
NODEF_DEBUGWRAP12(GetICMProfileW)
DEBUGWRAP8(SetICMMode)
NODEF_DEBUGWRAP8(SetICMProfileA)
NODEF_DEBUGWRAP8(SetICMProfileW)
NODEF_DEBUGWRAP16(UpdateICMRegKeyA)
NODEF_DEBUGWRAP16(UpdateICMRegKeyW)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_metafile

DEBUGWRAP4(CloseMetaFile)
DEBUGWRAP8(CopyEnhMetaFileA)
DEBUGWRAP8(CopyEnhMetaFileW)
DEBUGWRAP8(CopyMetaFileA)
DEBUGWRAP8(CopyMetaFileW)
DEBUGWRAP4(CloseEnhMetaFile)
DEBUGWRAP16(CreateEnhMetaFileA)
DEBUGWRAP16(CreateEnhMetaFileW)
DEBUGWRAP4(CreateMetaFileA)
DEBUGWRAP4(CreateMetaFileW)
DEBUGWRAP4(DeleteEnhMetaFile)
DEBUGWRAP4(DeleteMetaFile)
DEBUGWRAP20(EnumEnhMetaFile)
DEBUGWRAP16(EnumMetaFile)
DEBUGWRAP4(GetEnhMetaFileA)
DEBUGWRAP12(GetEnhMetaFileBits)
DEBUGWRAP12(GetEnhMetaFileDescriptionA)
DEBUGWRAP12(GetEnhMetaFileDescriptionW)
DEBUGWRAP12(GetEnhMetaFileHeader)
DEBUGWRAP12(GetEnhMetaFilePaletteEntries)
DEBUGWRAP4(GetEnhMetaFileW)
DEBUGWRAP4(GetMetaFileA)
DEBUGWRAP12(GetMetaFileBitsEx)
DEBUGWRAP4(GetMetaFileW)
DEBUGWRAP20(GetWinMetaFileBits)
DEBUGWRAP12(PlayEnhMetaFile)
DEBUGWRAP16(PlayEnhMetaFileRecord)
DEBUGWRAP8(PlayMetaFile)
DEBUGWRAP16(PlayMetaFileRecord)
DEBUGWRAP8(SetEnhMetaFileBits)
DEBUGWRAP8(SetMetaFileBitsEx)
DEBUGWRAP16(SetWinMetaFileBits)



#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_objhandle

DEBUGWRAP4(UnrealizeObject)
DEBUGWRAP4(DeleteObject)
DEBUGWRAP16(EnumObjects)
DEBUGWRAP8(GetCurrentObject)
DEBUGWRAP12(GetObjectA)
DEBUGWRAP_LVL2_4(GetObjectType)
DEBUGWRAP12(GetObjectW)
DEBUGWRAP_LVL2_4(GetStockObject)
DEBUGWRAP_LVL2_8(SelectObject)
NODEF_DEBUGWRAP8(SetObjectOwner)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_dibitmap

DEBUGWRAP20(CreateBitmap)
DEBUGWRAP4(CreateBitmapIndirect)
DEBUGWRAP12(CreateCompatibleBitmap)
DEBUGWRAP24(CreateDIBitmap)
DEBUGWRAP12(CreateDiscardableBitmap)
DEBUGWRAP12(GetBitmapBits)
DEBUGWRAP8(GetBitmapDimensionEx)
DEBUGWRAP12(SetBitmapBits)
DEBUGWRAP16(SetBitmapDimensionEx)
DEBUGWRAP24(CreateDIBSection)
DEBUGWRAP16(GetDIBColorTable)
DEBUGWRAP28(GetDIBits)
DEBUGWRAP16(SetDIBColorTable)
DEBUGWRAP28(SetDIBits)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_line


DEBUGWRAP24(LineDDA)
DEBUGWRAP12(LineTo)
DEBUGWRAP16(MoveToEx)
DEBUGWRAP16(PolyPolyline)
DEBUGWRAP12(Polyline)
DEBUGWRAP12(PolylineTo)



#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_transform

DEBUGWRAP4(GetGraphicsMode)
DEBUGWRAP8(SetWorldTransform)
DEBUGWRAP16(SetViewportExtEx)
DEBUGWRAP16(SetViewportOrgEx)
DEBUGWRAP16(SetWindowExtEx)
DEBUGWRAP16(SetWindowOrgEx)
DEBUGWRAP8(GetViewportExtEx)
DEBUGWRAP8(GetViewportOrgEx)
DEBUGWRAP8(GetWindowExtEx)
DEBUGWRAP8(GetWindowOrgEx)
DEBUGWRAP8(GetWorldTransform)
DEBUGWRAP8(SetMapMode)
DEBUGWRAP8(SetGraphicsMode)
DEBUGWRAP4(GetMapMode)
DEBUGWRAP12(CombineTransform)
DEBUGWRAP12(ModifyWorldTransform)
DEBUGWRAP16(OffsetViewportOrgEx)
DEBUGWRAP16(OffsetWindowOrgEx)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_gdi32


DEBUGWRAP12(FillRect)
DEBUGWRAP12(FrameRect)
DEBUGWRAP8(InvertRect)
DEBUGWRAP4(AbortPath)
DEBUGWRAP24(AngleArc)
DEBUGWRAP36(Arc)
DEBUGWRAP36(ArcTo)
DEBUGWRAP4(BeginPath)
NODEF_DEBUGWRAP4(CancelDC)
NODEF_DEBUGWRAP16(CheckColorsInGamut)
DEBUGWRAP8(ChoosePixelFormat)
DEBUGWRAP36(Chord)
DEBUGWRAP4(CloseFigure)
NODEF_DEBUGWRAP12(ColorMatchToTarget)
DEBUGWRAP4(CreateBrushIndirect)
DEBUGWRAP4(CreateColorSpaceA)
DEBUGWRAP4(CreateColorSpaceW)
DEBUGWRAP8(CreateDIBPatternBrush)
DEBUGWRAP8(CreateDIBPatternBrushPt)
DEBUGWRAP8(CreateHatchBrush)
DEBUGWRAP4(CreatePatternBrush)
DEBUGWRAP12(CreatePen)
DEBUGWRAP4(CreatePenIndirect)
DEBUGWRAP4(CreateSolidBrush)
DEBUGWRAP12(DPtoLP)
DEBUGWRAP4(DeleteColorSpace)
DEBUGWRAP16(DescribePixelFormat)
//;    DeviceCapabilitiesEx       = _DeviceCapabilitiesEx@??    @177
//;    DeviceCapabilitiesExA      = _DeviceCapabilitiesExA@??   @178
//;    DeviceCapabilitiesExW      = _DeviceCapabilitiesExW@??   @179
DEBUGWRAP16(DrawEscape)
DEBUGWRAP20(Ellipse)
DEBUGWRAP4(EndPath)
DEBUGWRAP20(Escape)
DEBUGWRAP20(ExcludeClipRect)
DEBUGWRAP20(ExtCreatePen)
DEBUGWRAP24(ExtEscape)
DEBUGWRAP20(ExtFloodFill)
DEBUGWRAP4(FillPath)
DEBUGWRAP16(FixBrushOrgEx)
DEBUGWRAP4(FlattenPath)
DEBUGWRAP16(FloodFill)
DEBUGWRAP12(GdiComment)
DEBUGWRAP0(GdiFlush)
NODEF_DEBUGWRAP0(GdiGetBatchLimit)
//;    GdiPlayDCScript            = _GdiPlayDCScript@??         @215
//;    GdiPlayJournal             = _GdiPlayJournal@??          @216
//;    GdiPlayScript              = _GdiPlayScript@??           @217
NODEF_DEBUGWRAP4(GdiSetBatchLimit)
DEBUGWRAP4(GetArcDirection)
DEBUGWRAP8(GetAspectRatioFilterEx)
DEBUGWRAP4(GetBkColor)
DEBUGWRAP4(GetBkMode)
DEBUGWRAP12(GetBoundsRect)
DEBUGWRAP8(GetBrushOrgEx)
DEBUGWRAP16(GetCharABCWidthsA)
DEBUGWRAP16(GetCharABCWidthsFloatA)
DEBUGWRAP16(GetCharABCWidthsFloatW)
DEBUGWRAP16(GetCharABCWidthsW)
DEBUGWRAP16(GetCharWidth32A)
DEBUGWRAP16(GetCharWidth32W)
DEBUGWRAP16(GetCharWidthFloatA)
DEBUGWRAP16(GetCharWidthFloatW)
DEBUGWRAP24(GetCharacterPlacementA)
DEBUGWRAP24(GetCharacterPlacementW)
DEBUGWRAP8(GetColorAdjustment)
NODEF_DEBUGWRAP4(GetColorSpace)
DEBUGWRAP8(GetCurrentPositionEx)
DEBUGWRAP8(GetDCOrgEx)
DEBUGWRAP8(GetDeviceCaps)
DEBUGWRAP8(GetDeviceGammaRamp)
DEBUGWRAP12(GetKerningPairsA)
DEBUGWRAP12(GetLogColorSpaceA)
DEBUGWRAP12(GetLogColorSpaceW)
DEBUGWRAP8(GetMiterLimit)
DEBUGWRAP8(GetNearestColor)
DEBUGWRAP12(GetOutlineTextMetricsA)
DEBUGWRAP12(GetOutlineTextMetricsW)
DEBUGWRAP16(GetPath)
DEBUGWRAP_LVL2_12(GetPixel)
DEBUGWRAP4(GetPixelFormat)
DEBUGWRAP4(GetPolyFillMode)
DEBUGWRAP4(GetROP2)
DEBUGWRAP8(GetRasterizerCaps)


DEBUGWRAP4(GetTextAlign)
DEBUGWRAP4(GetTextCharacterExtra)
DEBUGWRAP4(GetTextCharset)
DEBUGWRAP12(GetTextCharsetInfo)
DEBUGWRAP4(GetTextColor)
DEBUGWRAP8(GetTextMetricsA)
DEBUGWRAP8(GetTextMetricsW)
DEBUGWRAP12(LPtoDP)
DEBUGWRAP36(Pie)
DEBUGWRAP12(PolyBezier)
DEBUGWRAP12(PolyBezierTo)
DEBUGWRAP16(PolyDraw)
DEBUGWRAP16(PolyPolygon)
DEBUGWRAP12(Polygon)
DEBUGWRAP12(PtVisible)
DEBUGWRAP8(RectVisible)
DEBUGWRAP20(Rectangle)
DEBUGWRAP8(ResetDCA)
DEBUGWRAP8(ResetDCW)
DEBUGWRAP8(RestoreDC)
DEBUGWRAP28(RoundRect)
DEBUGWRAP4(SaveDC)
DEBUGWRAP24(ScaleViewportExtEx)
DEBUGWRAP24(ScaleWindowExtEx)
DEBUGWRAP8(SelectClipPath)
DEBUGWRAP8(SetArcDirection)
DEBUGWRAP8(SetBkColor)
DEBUGWRAP8(SetBkMode)
DEBUGWRAP12(SetBoundsRect)
DEBUGWRAP16(SetBrushOrgEx)
DEBUGWRAP8(SetColorAdjustment)
DEBUGWRAP8(SetColorSpace)
DEBUGWRAP8(SetDeviceGammaRamp)
DEBUGWRAP8(SetMapperFlags)
DEBUGWRAP12(SetMiterLimit)
DEBUGWRAP_LVL2_16(SetPixel)
DEBUGWRAP12(SetPixelFormat)
DEBUGWRAP_LVL2_16(SetPixelV)
DEBUGWRAP8(SetPolyFillMode)
DEBUGWRAP8(SetROP2)



DEBUGWRAP8(SetTextAlign)
DEBUGWRAP8(SetTextCharacterExtra)
DEBUGWRAP8(SetTextColor)
DEBUGWRAP12(SetTextJustification)
DEBUGWRAP4(StrokeAndFillPath)
DEBUGWRAP4(StrokePath)
DEBUGWRAP4(SwapBuffers)
DEBUGWRAP4(UpdateColors)
DEBUGWRAP4(WidenPath)


#undef DBG_LOCALLOG
#define DBG_LOCALLOG    DBG_devcontext

DEBUGWRAP4(CreateCompatibleDC)
DEBUGWRAP16(CreateDCA)
DEBUGWRAP16(CreateDCW)
DEBUGWRAP16(CreateICA)
DEBUGWRAP16(CreateICW)
DEBUGWRAP4(DeleteDC)
