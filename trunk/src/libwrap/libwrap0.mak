# $Id: libwrap0.mak,v 1.1 2004/12/04 09:40:49 sao2l02 Exp $

#
# Odin32 API
#
#       libwrap0.lib makefile
#


#
# Tell buildenvironmet to build an object library.
#
LIBTARGET = 1
EXETARGET = 1
PUBLICLIB = 1


#
# Compiler, tools, and interference rules.
#
!include ../../makefile.inc


#
# Object files. Prefix with OBJDIR and one space before the '\'.
#
OBJS = \
$(OBJDIR)\O32_AbortDoc.obj \
$(OBJDIR)\O32_AbortPath.obj \
$(OBJDIR)\O32_AddAtom.obj \
$(OBJDIR)\O32_AdjustWindowRect.obj \
$(OBJDIR)\O32_AdjustWindowRectEx.obj \
$(OBJDIR)\O32_AngleArc.obj \
$(OBJDIR)\O32_AnimatePalette.obj \
$(OBJDIR)\O32_Arc.obj \
$(OBJDIR)\O32_ArcTo.obj \
$(OBJDIR)\O32_ArrangeIconicWindows.obj \
$(OBJDIR)\O32_Beep.obj \
$(OBJDIR)\O32_BeginDeferWindowPos.obj \
$(OBJDIR)\O32_BeginPaint.obj \
$(OBJDIR)\O32_BeginPath.obj \
$(OBJDIR)\O32_BitBlt.obj \
$(OBJDIR)\O32_BringWindowToTop.obj \
$(OBJDIR)\O32_CallMsgFilter.obj \
$(OBJDIR)\O32_CallNextHookEx.obj \
$(OBJDIR)\O32_CallWindowProc.obj \
##$(OBJDIR)\O32_ChangeClipboardChain.obj \
$(OBJDIR)\O32_CharLower.obj \
$(OBJDIR)\O32_CharLowerBuff.obj \
$(OBJDIR)\O32_CharNext.obj \
$(OBJDIR)\O32_CharPrev.obj \
$(OBJDIR)\O32_CharToOem.obj \
$(OBJDIR)\O32_CharToOemBuff.obj \
$(OBJDIR)\O32_CharUpper.obj \
$(OBJDIR)\O32_CharUpperBuff.obj \
$(OBJDIR)\O32_ChildWindowFromPoint.obj \
$(OBJDIR)\O32_ChooseColor.obj \
$(OBJDIR)\O32_ChooseFont.obj \
$(OBJDIR)\O32_Chord.obj \
$(OBJDIR)\O32_ClientToScreen.obj \
$(OBJDIR)\O32_ClipCursor.obj \
##$(OBJDIR)\O32_CloseClipboard.obj \
$(OBJDIR)\O32_CloseEnhMetaFile.obj \
$(OBJDIR)\O32_CloseFigure.obj \
$(OBJDIR)\O32_CloseHandle.obj \
$(OBJDIR)\O32_CloseMetaFile.obj \
$(OBJDIR)\O32_CloseWindow.obj \
$(OBJDIR)\O32_CommDlgExtendedError.obj \
$(OBJDIR)\O32_CompareFileTime.obj \
$(OBJDIR)\O32_CopyCursor.obj \
$(OBJDIR)\O32_CopyEnhMetaFile.obj \
$(OBJDIR)\O32_CopyFile.obj \
$(OBJDIR)\O32_CopyIcon.obj \
$(OBJDIR)\O32_CopyMetaFile.obj \
$(OBJDIR)\O32_CopyRect.obj \
##$(OBJDIR)\O32_CountClipboardFormats.obj \
$(OBJDIR)\O32_CreateAcceleratorTable.obj \
$(OBJDIR)\O32_CreateBitmap.obj \
$(OBJDIR)\O32_CreateBitmapIndirect.obj \
$(OBJDIR)\O32_CreateBrushIndirect.obj \
$(OBJDIR)\O32_CreateCaret.obj \
$(OBJDIR)\O32_CreateCompatibleBitmap.obj \
$(OBJDIR)\O32_CreateCompatibleDC.obj \
$(OBJDIR)\O32_CreateCursor.obj \
$(OBJDIR)\O32_CreateDC.obj \
$(OBJDIR)\O32_CreateDialog.obj \
$(OBJDIR)\O32_CreateDialogIndirect.obj \
$(OBJDIR)\O32_CreateDialogIndirectParam.obj \
$(OBJDIR)\O32_CreateDialogParam.obj \
$(OBJDIR)\O32_CreateDIBitmap.obj \
$(OBJDIR)\O32_CreateDIBPatternBrushPt.obj \
$(OBJDIR)\O32_CreateDirectory.obj \
$(OBJDIR)\O32_CreateEnhMetaFile.obj \
$(OBJDIR)\O32_CreateEvent.obj \
$(OBJDIR)\O32_CreateFile.obj \
$(OBJDIR)\O32_CreateFont.obj \
$(OBJDIR)\O32_CreateFontIndirect.obj \
$(OBJDIR)\O32_CreateHatchBrush.obj \
$(OBJDIR)\O32_CreateIC.obj \
$(OBJDIR)\O32_CreateMetaFile.obj \
$(OBJDIR)\O32_CreateMutex.obj \
$(OBJDIR)\O32_CreatePalette.obj \
$(OBJDIR)\O32_CreatePatternBrush.obj \
$(OBJDIR)\O32_CreatePen.obj \
$(OBJDIR)\O32_CreatePenIndirect.obj \
$(OBJDIR)\O32_CreatePopupMenu.obj \
$(OBJDIR)\O32_CreateProcess.obj \
$(OBJDIR)\O32_CreateSemaphore.obj \
$(OBJDIR)\O32_CreateSolidBrush.obj \
$(OBJDIR)\O32_CreateThread.obj \
$(OBJDIR)\O32_CreateWindowEx.obj \
$(OBJDIR)\O32_DdeAbandonTransaction.obj \
$(OBJDIR)\O32_DdeAccessData.obj \
$(OBJDIR)\O32_DdeAddData.obj \
$(OBJDIR)\O32_DdeClientTransaction.obj \
$(OBJDIR)\O32_DdeCmpStringHandles.obj \
$(OBJDIR)\O32_DdeConnect.obj \
$(OBJDIR)\O32_DdeConnectList.obj \
$(OBJDIR)\O32_DdeCreateDataHandle.obj \
$(OBJDIR)\O32_DdeCreateStringHandle.obj \
$(OBJDIR)\O32_DdeDisconnect.obj \
$(OBJDIR)\O32_DdeDisconnectList.obj \
$(OBJDIR)\O32_DdeEnableCallback.obj \
$(OBJDIR)\O32_DdeFreeDataHandle.obj \
$(OBJDIR)\O32_DdeFreeStringHandle.obj \
$(OBJDIR)\O32_DdeGetData.obj \
$(OBJDIR)\O32_DdeGetLastError.obj \
$(OBJDIR)\O32_DdeInitialize.obj \
$(OBJDIR)\O32_DdeKeepStringHandle.obj \
$(OBJDIR)\O32_DdeNameService.obj \
$(OBJDIR)\O32_DdePostAdvise.obj \
$(OBJDIR)\O32_DdeQueryConvInfo.obj \
$(OBJDIR)\O32_DdeQueryNextServer.obj \
$(OBJDIR)\O32_DdeQueryString.obj \
$(OBJDIR)\O32_DdeReconnect.obj \
$(OBJDIR)\O32_DdeSetUserHandle.obj \
$(OBJDIR)\O32_DdeUnaccessData.obj \
$(OBJDIR)\O32_DdeUninitialize.obj \
$(OBJDIR)\O32_DefDlgProc.obj \
$(OBJDIR)\O32_DeferWindowPos.obj \
$(OBJDIR)\O32_DefFrameProc.obj \
$(OBJDIR)\O32_DefMDIChildProc.obj \
$(OBJDIR)\O32_DefWindowProc.obj \
$(OBJDIR)\O32_DeleteAtom.obj \
$(OBJDIR)\O32_DeleteCriticalSection.obj \
$(OBJDIR)\O32_DeleteDC.obj \
$(OBJDIR)\O32_DeleteEnhMetaFile.obj \
$(OBJDIR)\O32_DeleteFile.obj \
$(OBJDIR)\O32_DeleteMetaFile.obj \
$(OBJDIR)\O32_DeleteObject.obj \
$(OBJDIR)\O32_DestroyAcceleratorTable.obj \
$(OBJDIR)\O32_DestroyCaret.obj \
$(OBJDIR)\O32_DestroyWindow.obj \
$(OBJDIR)\O32_DeviceCapabilities.obj \
$(OBJDIR)\O32_DialogBox.obj \
$(OBJDIR)\O32_DialogBoxIndirect.obj \
$(OBJDIR)\O32_DialogBoxIndirectParam.obj \
$(OBJDIR)\O32_DialogBoxParam.obj \
$(OBJDIR)\O32_DispatchMessage.obj \
$(OBJDIR)\O32_DlgDirList.obj \
$(OBJDIR)\O32_DlgDirListComboBox.obj \
$(OBJDIR)\O32_DlgDirSelectComboBoxEx.obj \
$(OBJDIR)\O32_DlgDirSelectEx.obj \
$(OBJDIR)\O32_DllEntryPoint.obj \
$(OBJDIR)\O32_DosDateTimeToFileTime.obj \
$(OBJDIR)\O32_DPtoLP.obj \
$(OBJDIR)\O32_DragAcceptFiles.obj \
$(OBJDIR)\O32_DragFinish.obj \
$(OBJDIR)\O32_DragQueryFile.obj \
$(OBJDIR)\O32_DragQueryPoint.obj \
$(OBJDIR)\O32_DrawFocusRect.obj \
$(OBJDIR)\O32_DrawIcon.obj \
$(OBJDIR)\O32_DrawText.obj \
$(OBJDIR)\O32_DuplicateHandle.obj \
$(OBJDIR)\O32_Ellipse.obj \
##$(OBJDIR)\O32_EmptyClipboard.obj \
$(OBJDIR)\O32_EnableWindow.obj \
$(OBJDIR)\O32_EndDeferWindowPos.obj \
$(OBJDIR)\O32_EndDialog.obj \
$(OBJDIR)\O32_EndDoc.obj \
$(OBJDIR)\O32_EndPage.obj \
$(OBJDIR)\O32_EndPaint.obj \
$(OBJDIR)\O32_EndPath.obj \
$(OBJDIR)\O32_EnterCriticalSection.obj \
$(OBJDIR)\O32_EnumChildWindows.obj \
##$(OBJDIR)\O32_EnumClipboardFormats.obj \
$(OBJDIR)\O32_EnumEnhMetaFile.obj \
$(OBJDIR)\O32_EnumFontFamilies.obj \
$(OBJDIR)\O32_EnumFonts.obj \
$(OBJDIR)\O32_EnumMetaFile.obj \
$(OBJDIR)\O32_EnumObjects.obj \
$(OBJDIR)\O32_EnumPrinters.obj \
$(OBJDIR)\O32_EnumProps.obj \
$(OBJDIR)\O32_EnumPropsEx.obj \
$(OBJDIR)\O32_EnumThreadWindows.obj \
$(OBJDIR)\O32_EnumWindows.obj \
$(OBJDIR)\O32_EqualRect.obj \
$(OBJDIR)\O32_Escape.obj \
$(OBJDIR)\O32_ExcludeClipRect.obj \
$(OBJDIR)\O32_ExcludeUpdateRgn.obj \
$(OBJDIR)\O32_ExitProcess.obj \
$(OBJDIR)\O32_ExitThread.obj \
$(OBJDIR)\O32_ExitWindows.obj \
$(OBJDIR)\O32_ExitWindowsEx.obj \
$(OBJDIR)\O32_ExtCreatePen.obj \
$(OBJDIR)\O32_ExtCreateRegion.obj \
$(OBJDIR)\O32_ExtFloodFill.obj \
$(OBJDIR)\O32_ExtSelectClipRgn.obj \
$(OBJDIR)\O32_ExtTextOut.obj \
$(OBJDIR)\O32_FatalAppExit.obj \
$(OBJDIR)\O32_FatalExit.obj \
$(OBJDIR)\O32_FileTimeToDosDateTime.obj \
$(OBJDIR)\O32_FileTimeToLocalFileTime.obj \
$(OBJDIR)\O32_FileTimeToSystemTime.obj \
$(OBJDIR)\O32_FillPath.obj \
$(OBJDIR)\O32_FillRect.obj \
$(OBJDIR)\O32_FillRgn.obj \
$(OBJDIR)\O32_FindAtom.obj \
$(OBJDIR)\O32_FindClose.obj \
$(OBJDIR)\O32_FindFirstFile.obj \
$(OBJDIR)\O32_FindNextFile.obj \
$(OBJDIR)\O32_FindResource.obj \
$(OBJDIR)\O32_FindText.obj \
$(OBJDIR)\O32_FindWindow.obj \
$(OBJDIR)\O32_FlashWindow.obj \
$(OBJDIR)\O32_FlattenPath.obj \
$(OBJDIR)\O32_FloodFill.obj \
$(OBJDIR)\O32_FlushFileBuffers.obj \
$(OBJDIR)\O32_FrameRect.obj \
$(OBJDIR)\O32_FrameRgn.obj \
$(OBJDIR)\O32_FreeDDElParam.obj \
$(OBJDIR)\O32_FreeLibrary.obj \
$(OBJDIR)\O32_GetACP.obj \
$(OBJDIR)\O32_GetActiveWindow.obj \
$(OBJDIR)\O32_GetArcDirection.obj \
$(OBJDIR)\O32_GetAspectRatioFilterEx.obj \
$(OBJDIR)\O32_GetAtomName.obj \
$(OBJDIR)\O32_GetBitmapBits.obj \
$(OBJDIR)\O32_GetBitmapDimensionEx.obj \
$(OBJDIR)\O32_GetBkColor.obj \
$(OBJDIR)\O32_GetBkMode.obj \
$(OBJDIR)\O32_GetBoundsRect.obj \
$(OBJDIR)\O32_GetBrushOrgEx.obj \
$(OBJDIR)\O32_GetCapture.obj \
$(OBJDIR)\O32_GetCaretBlinkTime.obj \
$(OBJDIR)\O32_GetCaretPos.obj \
$(OBJDIR)\O32_GetCharABCWidths.obj \
$(OBJDIR)\O32_GetCharWidth.obj \
$(OBJDIR)\O32_GetClassInfo.obj \
$(OBJDIR)\O32_GetClassLong.obj \
$(OBJDIR)\O32_GetClassName.obj \
$(OBJDIR)\O32_GetClassWord.obj \
$(OBJDIR)\O32_GetClientRect.obj \
##$(OBJDIR)\O32_GetClipboardData.obj \
##$(OBJDIR)\O32_GetClipboardFormatName.obj \
##$(OBJDIR)\O32_GetClipboardOwner.obj \
##$(OBJDIR)\O32_GetClipboardViewer.obj \
$(OBJDIR)\O32_GetClipBox.obj \
$(OBJDIR)\O32_GetClipCursor.obj \
$(OBJDIR)\O32_GetClipRgn.obj \
$(OBJDIR)\O32_GetCommandLine.obj \
$(OBJDIR)\O32_GetCurrentDirectory.obj \
$(OBJDIR)\O32_GetCurrentObject.obj \
$(OBJDIR)\O32_GetCurrentPositionEx.obj \
$(OBJDIR)\O32_GetCurrentProcess.obj \
$(OBJDIR)\O32_GetCurrentProcessId.obj \
$(OBJDIR)\O32_GetCurrentThread.obj \
$(OBJDIR)\O32_GetCurrentThreadId.obj \
$(OBJDIR)\O32_GetCurrentTime.obj \
$(OBJDIR)\O32_GetCursor.obj \
$(OBJDIR)\O32_GetCursorPos.obj \
$(OBJDIR)\O32_GetDC.obj \
$(OBJDIR)\O32_GetDCEx.obj \
$(OBJDIR)\O32_GetDCOrgEx.obj \
$(OBJDIR)\O32_GetDesktopWindow.obj \
$(OBJDIR)\O32_GetDeviceCaps.obj \
$(OBJDIR)\O32_GetDialogBaseUnits.obj \
$(OBJDIR)\O32_GetDIBits.obj \
$(OBJDIR)\O32_GetDiskFreeSpace.obj \
$(OBJDIR)\O32_GetDlgCtrlID.obj \
$(OBJDIR)\O32_GetDlgItem.obj \
$(OBJDIR)\O32_GetDlgItemInt.obj \
$(OBJDIR)\O32_GetDlgItemText.obj \
$(OBJDIR)\O32_GetDoubleClickTime.obj \
$(OBJDIR)\O32_GetDriveType.obj \
$(OBJDIR)\O32_GetEnhMetaFile.obj \
$(OBJDIR)\O32_GetEnhMetaFileBits.obj \
$(OBJDIR)\O32_GetEnhMetaFileHeader.obj \
$(OBJDIR)\O32_GetEnhMetaFilePaletteEntries.obj \
$(OBJDIR)\O32_GetEnvironmentStrings.obj \
$(OBJDIR)\O32_GetEnvironmentVariable.obj \
$(OBJDIR)\O32_GetExitCodeProcess.obj \
$(OBJDIR)\O32_GetExitCodeThread.obj \
$(OBJDIR)\O32_GetFileAttributes.obj \
$(OBJDIR)\O32_GetFileInformationByHandle.obj \
$(OBJDIR)\O32_GetFileSize.obj \
$(OBJDIR)\O32_GetFileTime.obj \
$(OBJDIR)\O32_GetFileTitle.obj \
$(OBJDIR)\O32_GetFileType.obj \
$(OBJDIR)\O32_GetFocus.obj \
$(OBJDIR)\O32_GetForegroundWindow.obj \
$(OBJDIR)\O32_GetFullPathName.obj \
$(OBJDIR)\O32_GetGraphicsMode.obj \
$(OBJDIR)\O32_GetIconInfo.obj \
$(OBJDIR)\O32_GetKerningPairs.obj \
$(OBJDIR)\O32_GetKeyboardType.obj \
$(OBJDIR)\O32_GetKeyNameText.obj \
$(OBJDIR)\O32_GetKeyState.obj \
$(OBJDIR)\O32_GetLastActivePopup.obj \
$(OBJDIR)\O32_GetLastError.obj \
$(OBJDIR)\O32_GetLocalTime.obj \
$(OBJDIR)\O32_GetLogicalDrives.obj \
$(OBJDIR)\O32_GetLogicalDriveStrings.obj \
$(OBJDIR)\O32_GetMapMode.obj \
$(OBJDIR)\O32_GetMessage.obj \
$(OBJDIR)\O32_GetMessageExtraInfo.obj \
$(OBJDIR)\O32_GetMessagePos.obj \
$(OBJDIR)\O32_GetMessageTime.obj \
$(OBJDIR)\O32_GetMetaFile.obj \
$(OBJDIR)\O32_GetMetaFileBitsEx.obj \
$(OBJDIR)\O32_GetMiterLimit.obj \
$(OBJDIR)\O32_GetModuleFileName.obj \
$(OBJDIR)\O32_GetModuleHandle.obj \
$(OBJDIR)\O32_GetNearestColor.obj \
$(OBJDIR)\O32_GetNearestPaletteIndex.obj \
$(OBJDIR)\O32_GetNextDlgGroupItem.obj \
$(OBJDIR)\O32_GetNextWindow.obj \
$(OBJDIR)\O32_GetObject.obj \
$(OBJDIR)\O32_GetObjectType.obj \
$(OBJDIR)\O32_GetOEMCP.obj \
##$(OBJDIR)\O32_GetOpenClipboardWindow.obj \
$(OBJDIR)\O32_GetOpenFileName.obj \
$(OBJDIR)\O32_GetOutlineTextMetrics.obj \
$(OBJDIR)\O32_GetOverlappedResult.obj \
$(OBJDIR)\O32_GetPaletteEntries.obj \
$(OBJDIR)\O32_GetParent.obj \
$(OBJDIR)\O32_GetPath.obj \
$(OBJDIR)\O32_GetPixel.obj \
$(OBJDIR)\O32_GetPolyFillMode.obj \
$(OBJDIR)\O32_GetPriorityClass.obj \
##$(OBJDIR)\O32_GetPriorityClipboardFormat.obj \
$(OBJDIR)\O32_GetPrivateProfileInt.obj \
$(OBJDIR)\O32_GetPrivateProfileString.obj \
$(OBJDIR)\O32_GetProcAddress.obj \
$(OBJDIR)\O32_GetProfileInt.obj \
$(OBJDIR)\O32_GetProfileString.obj \
$(OBJDIR)\O32_GetProp.obj \
$(OBJDIR)\O32_GetQueueStatus.obj \
$(OBJDIR)\O32_GetRasterizerCaps.obj \
$(OBJDIR)\O32_GetRegionData.obj \
$(OBJDIR)\O32_GetRgnBox.obj \
$(OBJDIR)\O32_GetROP2.obj \
$(OBJDIR)\O32_GetSaveFileName.obj \
$(OBJDIR)\O32_GetScrollPos.obj \
$(OBJDIR)\O32_GetScrollRange.obj \
$(OBJDIR)\O32_GetStdHandle.obj \
$(OBJDIR)\O32_GetStockObject.obj \
$(OBJDIR)\O32_GetStretchBltMode.obj \
$(OBJDIR)\O32_GetSysColor.obj \
$(OBJDIR)\O32_GetSystemDirectory.obj \
$(OBJDIR)\O32_GetSystemMetrics.obj \
$(OBJDIR)\O32_GetSystemPaletteEntries.obj \
$(OBJDIR)\O32_GetSystemTime.obj \
$(OBJDIR)\O32_GetTabbedTextExtent.obj \
$(OBJDIR)\O32_GetTempFileName.obj \
$(OBJDIR)\O32_GetTempPath.obj \
$(OBJDIR)\O32_GetTextAlign.obj \
$(OBJDIR)\O32_GetTextCharacterExtra.obj \
$(OBJDIR)\O32_GetTextColor.obj \
$(OBJDIR)\O32_GetTextExtentPoint.obj \
$(OBJDIR)\O32_GetTextExtentPoint32.obj \
$(OBJDIR)\O32_GetTextFace.obj \
$(OBJDIR)\O32_GetTextMetrics.obj \
$(OBJDIR)\O32_GetThreadPriority.obj \
$(OBJDIR)\O32_GetTickCount.obj \
$(OBJDIR)\O32_GetTimeZoneInformation.obj \
$(OBJDIR)\O32_GetTopWindow.obj \
$(OBJDIR)\O32_GetUpdateRect.obj \
$(OBJDIR)\O32_GetUpdateRgn.obj \
$(OBJDIR)\O32_GetViewportExtEx.obj \
$(OBJDIR)\O32_GetViewportOrgEx.obj \
$(OBJDIR)\O32_GetVolumeInformation.obj \
$(OBJDIR)\O32_GetWindow.obj \
$(OBJDIR)\O32_GetWindowDC.obj \
$(OBJDIR)\O32_GetWindowExtEx.obj \
$(OBJDIR)\O32_GetWindowLong.obj \
$(OBJDIR)\O32_GetWindowOrgEx.obj \
$(OBJDIR)\O32_GetWindowPlacement.obj \
$(OBJDIR)\O32_GetWindowRect.obj \
$(OBJDIR)\O32_GetWindowsDirectory.obj \
$(OBJDIR)\O32_GetWindowText.obj \
$(OBJDIR)\O32_GetWindowTextLength.obj \
$(OBJDIR)\O32_GetWindowThreadProcessId.obj \
$(OBJDIR)\O32_GetWindowWord.obj \
$(OBJDIR)\O32_GetWinMetaFileBits.obj \
$(OBJDIR)\O32_GetWorldTransform.obj \
$(OBJDIR)\O32_HideCaret.obj \
$(OBJDIR)\O32_InflateRect.obj \
$(OBJDIR)\O32_InitAtomTable.obj \
$(OBJDIR)\O32_InSendMessage.obj \
$(OBJDIR)\O32_IsBadCodePtr.obj \
$(OBJDIR)\O32_IsBadHugeReadPtr.obj \
$(OBJDIR)\O32_IsBadHugeWritePtr.obj \
$(OBJDIR)\O32_IsBadReadPtr.obj \
$(OBJDIR)\O32_IsBadStringPtr.obj \
$(OBJDIR)\O32_IsBadWritePtr.obj \
$(OBJDIR)\O32_IsCharAlpha.obj \
$(OBJDIR)\O32_IsCharAlphaNumeric.obj \
$(OBJDIR)\O32_IsCharLower.obj \
$(OBJDIR)\O32_IsCharUpper.obj \
$(OBJDIR)\O32_IsChild.obj \
##$(OBJDIR)\O32_IsClipboardFormatAvailable.obj \
$(OBJDIR)\O32_IsDBCSLeadByte.obj \
$(OBJDIR)\O32_IsDialogMessage.obj \
$(OBJDIR)\O32_IsDlgButtonChecked.obj \
$(OBJDIR)\O32_IsIconic.obj \
$(OBJDIR)\O32_IsMenu.obj \
$(OBJDIR)\O32_IsRectEmpty.obj \
$(OBJDIR)\O32_IsWindow.obj \
$(OBJDIR)\O32_IsWindowEnabled.obj \
$(OBJDIR)\O32_IsWindowVisible.obj \
$(OBJDIR)\O32_IsZoomed.obj \
$(OBJDIR)\O32_KillTimer.obj \
$(OBJDIR)\O32_LeaveCriticalSection.obj \
$(OBJDIR)\O32_LineDDA.obj \
$(OBJDIR)\O32_LineTo.obj \
$(OBJDIR)\O32_LoadAccelerators.obj \
$(OBJDIR)\O32_LoadBitmap.obj \
$(OBJDIR)\O32_LoadCursor.obj \
$(OBJDIR)\O32_LoadIcon.obj \
$(OBJDIR)\O32_LoadLibrary.obj \
$(OBJDIR)\O32_LoadMenu.obj \
$(OBJDIR)\O32_LoadMenuIndirect.obj \
$(OBJDIR)\O32_LoadModule.obj \
$(OBJDIR)\O32_LoadResource.obj \
$(OBJDIR)\O32_LoadString.obj \
$(OBJDIR)\O32_LocalAlloc.obj \
$(OBJDIR)\O32_LocalDiscard.obj \
$(OBJDIR)\O32_LocalFileTimeToFileTime.obj \
$(OBJDIR)\O32_LocalFlags.obj \
$(OBJDIR)\O32_LocalFree.obj \
$(OBJDIR)\O32_LocalHandle.obj \
$(OBJDIR)\O32_LocalLock.obj \
$(OBJDIR)\O32_LocalReAlloc.obj \
$(OBJDIR)\O32_LocalSize.obj \
$(OBJDIR)\O32_LocalUnlock.obj \
$(OBJDIR)\O32_LockFile.obj \
$(OBJDIR)\O32_LockResource.obj \
$(OBJDIR)\O32_LockWindowUpdate.obj \
$(OBJDIR)\O32_LPtoDP.obj \
$(OBJDIR)\O32_lstrcat.obj \
$(OBJDIR)\O32_lstrcmp.obj \
$(OBJDIR)\O32_lstrcmpi.obj \
$(OBJDIR)\O32_lstrcpy.obj \
$(OBJDIR)\O32_lstrlen.obj \
$(OBJDIR)\O32_MapDialogRect.obj \
$(OBJDIR)\O32_MapVirtualKey.obj \
$(OBJDIR)\O32_MapWindowPoints.obj \
$(OBJDIR)\O32_MaskBlt.obj \
$(OBJDIR)\O32_MessageBeep.obj \
$(OBJDIR)\O32_MessageBox.obj \
$(OBJDIR)\O32_ModifyMenu.obj \
$(OBJDIR)\O32_ModifyWorldTransform.obj \
$(OBJDIR)\O32_MoveFile.obj \
$(OBJDIR)\O32_MoveToEx.obj \
$(OBJDIR)\O32_MoveWindow.obj \
$(OBJDIR)\O32_MsgWaitForMultipleObjects.obj \
$(OBJDIR)\O32_MulDiv.obj \
$(OBJDIR)\O32_OemToChar.obj \
$(OBJDIR)\O32_OemToCharBuff.obj \
$(OBJDIR)\O32_OffsetClipRgn.obj \
$(OBJDIR)\O32_OffsetRect.obj \
$(OBJDIR)\O32_OffsetRgn.obj \
$(OBJDIR)\O32_OffsetViewportOrgEx.obj \
$(OBJDIR)\O32_OffsetWindowOrgEx.obj \
##$(OBJDIR)\O32_OpenClipboard.obj \
$(OBJDIR)\O32_OpenEvent.obj \
$(OBJDIR)\O32_OpenFile.obj \
$(OBJDIR)\O32_OpenMutex.obj \
$(OBJDIR)\O32_OpenProcess.obj \
$(OBJDIR)\O32_OpenSemaphore.obj \
$(OBJDIR)\O32_OutputDebugString.obj \
$(OBJDIR)\O32_PackDDElParam.obj \
$(OBJDIR)\O32_PaintRgn.obj \
$(OBJDIR)\O32_PatBlt.obj \
$(OBJDIR)\O32_PathToRegion.obj \
$(OBJDIR)\O32_PeekMessage.obj \
$(OBJDIR)\O32_Pie.obj \
$(OBJDIR)\O32_PlayEnhMetaFile.obj \
$(OBJDIR)\O32_PlayMetaFile.obj \
$(OBJDIR)\O32_PlayMetaFileRecord.obj \
$(OBJDIR)\O32_PolyBezier.obj \
$(OBJDIR)\O32_PolyBezierTo.obj \
$(OBJDIR)\O32_PolyDraw.obj \
$(OBJDIR)\O32_Polygon.obj \
$(OBJDIR)\O32_Polyline.obj \
$(OBJDIR)\O32_PolylineTo.obj \
$(OBJDIR)\O32_PolyPolygon.obj \
$(OBJDIR)\O32_PolyPolyline.obj \
$(OBJDIR)\O32_PostMessage.obj \
$(OBJDIR)\O32_PostQuitMessage.obj \
$(OBJDIR)\O32_PostThreadMessage.obj \
$(OBJDIR)\O32_PrintDlg.obj \
$(OBJDIR)\O32_PtInRect.obj \
$(OBJDIR)\O32_PtInRegion.obj \
$(OBJDIR)\O32_PtVisible.obj \
$(OBJDIR)\O32_PulseEvent.obj \
$(OBJDIR)\O32_ReadFile.obj \
$(OBJDIR)\O32_RealizePalette.obj \
$(OBJDIR)\O32_Rectangle.obj \
$(OBJDIR)\O32_RectInRegion.obj \
$(OBJDIR)\O32_RectVisible.obj \
$(OBJDIR)\O32_RedrawWindow.obj \
$(OBJDIR)\O32_RegCloseKey.obj \
$(OBJDIR)\O32_RegCreateKey.obj \
$(OBJDIR)\O32_RegCreateKeyEx.obj \
$(OBJDIR)\O32_RegDeleteKey.obj \
$(OBJDIR)\O32_RegDeleteValue.obj \
$(OBJDIR)\O32_RegEnumKey.obj \
$(OBJDIR)\O32_RegEnumKeyEx.obj \
$(OBJDIR)\O32_RegEnumValue.obj \
$(OBJDIR)\O32_RegisterClass.obj \
##$(OBJDIR)\O32_RegisterClipboardFormat.obj \
$(OBJDIR)\O32_RegisterWindowMessage.obj \
$(OBJDIR)\O32_RegOpenKey.obj \
$(OBJDIR)\O32_RegOpenKeyEx.obj \
$(OBJDIR)\O32_RegQueryInfoKey.obj \
$(OBJDIR)\O32_RegQueryValue.obj \
$(OBJDIR)\O32_RegQueryValueEx.obj \
$(OBJDIR)\O32_RegSetValue.obj \
$(OBJDIR)\O32_RegSetValueEx.obj \
$(OBJDIR)\O32_ReleaseCapture.obj \
$(OBJDIR)\O32_ReleaseDC.obj \
$(OBJDIR)\O32_ReleaseMutex.obj \
$(OBJDIR)\O32_ReleaseSemaphore.obj \
$(OBJDIR)\O32_RemoveDirectory.obj \
$(OBJDIR)\O32_RemoveFontResource.obj \
$(OBJDIR)\O32_RemoveMenu.obj \
$(OBJDIR)\O32_RemoveProp.obj \
$(OBJDIR)\O32_ReplaceText.obj \
$(OBJDIR)\O32_ReplyMessage.obj \
$(OBJDIR)\O32_ResetDC.obj \
$(OBJDIR)\O32_ResetEvent.obj \
$(OBJDIR)\O32_ResizePalette.obj \
$(OBJDIR)\O32_RestoreDC.obj \
$(OBJDIR)\O32_ResumeThread.obj \
$(OBJDIR)\O32_ReuseDDElParam.obj \
$(OBJDIR)\O32_RoundRect.obj \
$(OBJDIR)\O32_SaveDC.obj \
$(OBJDIR)\O32_ScaleViewportExtEx.obj \
$(OBJDIR)\O32_ScaleWindowExtEx.obj \
$(OBJDIR)\O32_ScreenToClient.obj \
$(OBJDIR)\O32_ScrollDC.obj \
$(OBJDIR)\O32_ScrollWindow.obj \
$(OBJDIR)\O32_ScrollWindowEx.obj \
$(OBJDIR)\O32_SearchPath.obj \
$(OBJDIR)\O32_SelectClipRgn.obj \
$(OBJDIR)\O32_SelectObject.obj \
$(OBJDIR)\O32_SelectPalette.obj \
$(OBJDIR)\O32_SendDlgItemMessage.obj \
$(OBJDIR)\O32_SendMessage.obj \
$(OBJDIR)\O32_SetActiveWindow.obj \
$(OBJDIR)\O32_SetArcDirection.obj \
$(OBJDIR)\O32_SetBitmapBits.obj \
$(OBJDIR)\O32_SetBitmapDimensionEx.obj \
$(OBJDIR)\O32_SetBkColor.obj \
$(OBJDIR)\O32_SetBkMode.obj \
$(OBJDIR)\O32_SetBoundsRect.obj \
$(OBJDIR)\O32_SetBrushOrgEx.obj \
$(OBJDIR)\O32_SetCapture.obj \
$(OBJDIR)\O32_SetCaretBlinkTime.obj \
$(OBJDIR)\O32_SetCaretPos.obj \
$(OBJDIR)\O32_SetClassLong.obj \
$(OBJDIR)\O32_SetClassWord.obj \
##$(OBJDIR)\O32_SetClipboardData.obj \
##$(OBJDIR)\O32_SetClipboardViewer.obj \
$(OBJDIR)\O32_SetCurrentDirectory.obj \
$(OBJDIR)\O32_SetCursor.obj \
$(OBJDIR)\O32_SetCursorPos.obj \
$(OBJDIR)\O32_SetDIBits.obj \
$(OBJDIR)\O32_SetDIBitsToDevice.obj \
$(OBJDIR)\O32_SetDlgItemInt.obj \
$(OBJDIR)\O32_SetDlgItemText.obj \
$(OBJDIR)\O32_SetDoubleClickTime.obj \
$(OBJDIR)\O32_SetEndOfFile.obj \
$(OBJDIR)\O32_SetEnhMetaFileBits.obj \
$(OBJDIR)\O32_SetEnvironmentVariable.obj \
$(OBJDIR)\O32_SetEvent.obj \
$(OBJDIR)\O32_SetFileAttributes.obj \
$(OBJDIR)\O32_SetFilePointer.obj \
$(OBJDIR)\O32_SetFileTime.obj \
$(OBJDIR)\O32_SetFocus.obj \
$(OBJDIR)\O32_SetForegroundWindow.obj \
$(OBJDIR)\O32_SetGraphicsMode.obj \
$(OBJDIR)\O32_SetHandleCount.obj \
$(OBJDIR)\O32_SetLastError.obj \
$(OBJDIR)\O32_SetLocalTime.obj \
$(OBJDIR)\O32_SetMapMode.obj \
$(OBJDIR)\O32_SetMapperFlags.obj \
$(OBJDIR)\O32_SetMenu.obj \
$(OBJDIR)\O32_SetMenuItemBitmaps.obj \
$(OBJDIR)\O32_SetMetaFileBitsEx.obj \
$(OBJDIR)\O32_SetMiterLimit.obj \
$(OBJDIR)\O32_SetPaletteEntries.obj \
$(OBJDIR)\O32_SetParent.obj \
$(OBJDIR)\O32_SetPixel.obj \
$(OBJDIR)\O32_SetPolyFillMode.obj \
$(OBJDIR)\O32_SetPriorityClass.obj \
$(OBJDIR)\O32_SetProp.obj \
$(OBJDIR)\O32_SetRect.obj \
$(OBJDIR)\O32_SetRectEmpty.obj \
$(OBJDIR)\O32_SetRectRgn.obj \
$(OBJDIR)\O32_SetROP2.obj \
$(OBJDIR)\O32_SetScrollPos.obj \
$(OBJDIR)\O32_SetScrollRange.obj \
$(OBJDIR)\O32_SetStdHandle.obj \
$(OBJDIR)\O32_SetStretchBltMode.obj \
$(OBJDIR)\O32_SetSysColors.obj \
$(OBJDIR)\O32_SetSystemTime.obj \
$(OBJDIR)\O32_SetTextAlign.obj \
$(OBJDIR)\O32_SetTextCharacterExtra.obj \
$(OBJDIR)\O32_SetTextColor.obj \
$(OBJDIR)\O32_SetTextJustification.obj \
$(OBJDIR)\O32_SetThreadPriority.obj \
$(OBJDIR)\O32_SetTimer.obj \
$(OBJDIR)\O32_SetTimeZoneInformation.obj \
$(OBJDIR)\O32_SetViewportExtEx.obj \
$(OBJDIR)\O32_SetViewportOrgEx.obj \
$(OBJDIR)\O32_SetVolumeLabel.obj \
$(OBJDIR)\O32_SetWindowExtEx.obj \
$(OBJDIR)\O32_SetWindowLong.obj \
$(OBJDIR)\O32_SetWindowOrgEx.obj \
$(OBJDIR)\O32_SetWindowPlacement.obj \
$(OBJDIR)\O32_SetWindowPos.obj \
$(OBJDIR)\O32_SetWindowsHookEx.obj \
$(OBJDIR)\O32_SetWindowText.obj \
$(OBJDIR)\O32_SetWindowWord.obj \
$(OBJDIR)\O32_SetWinMetaFileBits.obj \
$(OBJDIR)\O32_SetWorldTransform.obj \
$(OBJDIR)\O32_ShowCaret.obj \
$(OBJDIR)\O32_ShowCursor.obj \
$(OBJDIR)\O32_ShowOwnedPopups.obj \
$(OBJDIR)\O32_ShowScrollBar.obj \
$(OBJDIR)\O32_ShowWindow.obj \
$(OBJDIR)\O32_SizeofResource.obj \
$(OBJDIR)\O32_Sleep.obj \
$(OBJDIR)\O32_StartDoc.obj \
$(OBJDIR)\O32_StartPage.obj \
$(OBJDIR)\O32_StretchBlt.obj \
$(OBJDIR)\O32_StretchDIBits.obj \
$(OBJDIR)\O32_StrokeAndFillPath.obj \
$(OBJDIR)\O32_StrokePath.obj \
$(OBJDIR)\O32_SubtractRect.obj \
$(OBJDIR)\O32_SuspendThread.obj \
$(OBJDIR)\O32_SwapMouseButton.obj \
$(OBJDIR)\O32_SystemParametersInfo.obj \
$(OBJDIR)\O32_SystemTimeToFileTime.obj \
$(OBJDIR)\O32_SystemTimeToTzSpecificLocalTime.obj \
$(OBJDIR)\O32_TabbedTextOut.obj \
$(OBJDIR)\O32_TerminateProcess.obj \
$(OBJDIR)\O32_TerminateThread.obj \
$(OBJDIR)\O32_TextOut.obj \
$(OBJDIR)\O32_timeGetSystemTime.obj \
$(OBJDIR)\O32_timeGetTime.obj \
$(OBJDIR)\O32_TlsAlloc.obj \
$(OBJDIR)\O32_TlsFree.obj \
$(OBJDIR)\O32_TlsGetValue.obj \
$(OBJDIR)\O32_TlsSetValue.obj \
$(OBJDIR)\O32_TrackPopupMenu.obj \
$(OBJDIR)\O32_TranslateAccelerator.obj \
$(OBJDIR)\O32_TranslateMDISysAccel.obj \
$(OBJDIR)\O32_TranslateMessage.obj \
$(OBJDIR)\O32_UnhookWindowsHookEx.obj \
$(OBJDIR)\O32_UnionRect.obj \
$(OBJDIR)\O32_UnlockFile.obj \
$(OBJDIR)\O32_UnpackDDElParam.obj \
$(OBJDIR)\O32_UnrealizeObject.obj \
$(OBJDIR)\O32_UnregisterClass.obj \
$(OBJDIR)\O32_UpdateWindow.obj \
$(OBJDIR)\O32_ValidateRect.obj \
$(OBJDIR)\O32_ValidateRgn.obj \
$(OBJDIR)\O32_VkKeyScan.obj \
$(OBJDIR)\O32_WaitForMultipleObjects.obj \
$(OBJDIR)\O32_WaitForSingleObject.obj \
$(OBJDIR)\O32_WaitMessage.obj \
$(OBJDIR)\O32_WidenPath.obj \
$(OBJDIR)\O32_WinCallWinMain.obj \
$(OBJDIR)\O32_WindowFromDC.obj \
$(OBJDIR)\O32_WindowFromPoint.obj \
$(OBJDIR)\O32_WinExec.obj \
$(OBJDIR)\O32_WinHelp.obj \
$(OBJDIR)\O32_WinQueryTranslateMode.obj \
$(OBJDIR)\O32_WinSetTranslateMode.obj \
$(OBJDIR)\O32_WinTranslateDevicePoints.obj \
$(OBJDIR)\O32_WinTranslateDeviceRects.obj \
$(OBJDIR)\O32_WinTranslateGraphicsObjectHandle.obj \
$(OBJDIR)\O32_WinTranslateMnemonicString.obj \
$(OBJDIR)\O32_WriteFile.obj \
$(OBJDIR)\O32_WritePrivateProfileString.obj \
$(OBJDIR)\O32_WriteProfileString.obj \
$(OBJDIR)\O32_wvsprintf.obj \
$(OBJDIR)\O32_ZeroMemory.obj \
$(OBJDIR)\O32_GetAsyncKeyState.obj \
$(OBJDIR)\O32_InvertRect.obj \
$(OBJDIR)\O32_GetStartupInfo.obj \
$(OBJDIR)\O32_WaitForInputIdle.obj \
$(OBJDIR)\O32_CreateBitmapFromPMHandle.obj \
$(OBJDIR)\O32_CreatePaletteFromPMHandle.obj \
$(OBJDIR)\O32_GetPMHandleFromGDIHandle.obj \


TARGET  = libwrap0


#
# Includes the common rules.
#
!include $(ODIN32_POST_INC)

	
#
# The fast way
#
# This rule will make all objects in one go, something which is much faster
# than doing them one by one. However, this is at the cost of doing of single
# changes causing everything to be compiled.
#
# We disable debug info for these object to save the debugger a thousand
# object file entries.
#
$(OBJS):
	$(AS) $(ASFLAGS) -Od -Fdo:$(OBJDIR) @<<$(OBJDIR)\alp.rsp
O32_AbortDoc.asm
O32_AbortPath.asm
O32_AddAtom.asm
O32_AdjustWindowRect.asm
O32_AdjustWindowRectEx.asm
O32_AngleArc.asm
O32_AnimatePalette.asm
O32_Arc.asm
O32_ArcTo.asm
O32_ArrangeIconicWindows.asm
O32_Beep.asm
O32_BeginDeferWindowPos.asm
O32_BeginPaint.asm
O32_BeginPath.asm
O32_BitBlt.asm
O32_BringWindowToTop.asm
O32_CallMsgFilter.asm
O32_CallNextHookEx.asm
O32_CallWindowProc.asm
O32_ChangeClipboardChain.asm
O32_CharLower.asm
O32_CharLowerBuff.asm
O32_CharNext.asm
O32_CharPrev.asm
O32_CharToOem.asm
O32_CharToOemBuff.asm
O32_CharUpper.asm
O32_CharUpperBuff.asm
O32_ChildWindowFromPoint.asm
O32_ChooseColor.asm
O32_ChooseFont.asm
O32_Chord.asm
O32_ClientToScreen.asm
O32_ClipCursor.asm
O32_CloseClipboard.asm
O32_CloseEnhMetaFile.asm
O32_CloseFigure.asm
O32_CloseHandle.asm
O32_CloseMetaFile.asm
O32_CloseWindow.asm
O32_CommDlgExtendedError.asm
O32_CompareFileTime.asm
O32_CopyCursor.asm
O32_CopyEnhMetaFile.asm
O32_CopyFile.asm
O32_CopyIcon.asm
O32_CopyMetaFile.asm
O32_CopyRect.asm
O32_CountClipboardFormats.asm
O32_CreateAcceleratorTable.asm
O32_CreateBitmap.asm
O32_CreateBitmapIndirect.asm
O32_CreateBrushIndirect.asm
O32_CreateCaret.asm
O32_CreateCompatibleBitmap.asm
O32_CreateCompatibleDC.asm
O32_CreateCursor.asm
O32_CreateDC.asm
O32_CreateDialog.asm
O32_CreateDialogIndirect.asm
O32_CreateDialogIndirectParam.asm
O32_CreateDialogParam.asm
O32_CreateDIBitmap.asm
O32_CreateDIBPatternBrushPt.asm
O32_CreateDirectory.asm
O32_CreateEnhMetaFile.asm
O32_CreateEvent.asm
O32_CreateFile.asm
O32_CreateFont.asm
O32_CreateFontIndirect.asm
O32_CreateHatchBrush.asm
O32_CreateIC.asm
O32_CreateMetaFile.asm
O32_CreateMutex.asm
O32_CreatePalette.asm
O32_CreatePatternBrush.asm
O32_CreatePen.asm
O32_CreatePenIndirect.asm
O32_CreatePopupMenu.asm
O32_CreateProcess.asm
O32_CreateSemaphore.asm
O32_CreateSolidBrush.asm
O32_CreateThread.asm
O32_CreateWindowEx.asm
O32_DdeAbandonTransaction.asm
O32_DdeAccessData.asm
O32_DdeAddData.asm
O32_DdeClientTransaction.asm
O32_DdeCmpStringHandles.asm
O32_DdeConnect.asm
O32_DdeConnectList.asm
O32_DdeCreateDataHandle.asm
O32_DdeCreateStringHandle.asm
O32_DdeDisconnect.asm
O32_DdeDisconnectList.asm
O32_DdeEnableCallback.asm
O32_DdeFreeDataHandle.asm
O32_DdeFreeStringHandle.asm
O32_DdeGetData.asm
O32_DdeGetLastError.asm
O32_DdeInitialize.asm
O32_DdeKeepStringHandle.asm
O32_DdeNameService.asm
O32_DdePostAdvise.asm
O32_DdeQueryConvInfo.asm
O32_DdeQueryNextServer.asm
O32_DdeQueryString.asm
O32_DdeReconnect.asm
O32_DdeSetUserHandle.asm
O32_DdeUnaccessData.asm
O32_DdeUninitialize.asm
O32_DefDlgProc.asm
O32_DeferWindowPos.asm
O32_DefFrameProc.asm
O32_DefMDIChildProc.asm
O32_DefWindowProc.asm
O32_DeleteAtom.asm
O32_DeleteCriticalSection.asm
O32_DeleteDC.asm
O32_DeleteEnhMetaFile.asm
O32_DeleteFile.asm
O32_DeleteMetaFile.asm
O32_DeleteObject.asm
O32_DestroyAcceleratorTable.asm
O32_DestroyCaret.asm
O32_DestroyWindow.asm
O32_DeviceCapabilities.asm
O32_DialogBox.asm
O32_DialogBoxIndirect.asm
O32_DialogBoxIndirectParam.asm
O32_DialogBoxParam.asm
O32_DispatchMessage.asm
O32_DlgDirList.asm
O32_DlgDirListComboBox.asm
O32_DlgDirSelectComboBoxEx.asm
O32_DlgDirSelectEx.asm
O32_DllEntryPoint.asm
O32_DosDateTimeToFileTime.asm
O32_DPtoLP.asm
O32_DragAcceptFiles.asm
O32_DragFinish.asm
O32_DragQueryFile.asm
O32_DragQueryPoint.asm
O32_DrawFocusRect.asm
O32_DrawIcon.asm
O32_DrawText.asm
O32_DuplicateHandle.asm
O32_Ellipse.asm
O32_EmptyClipboard.asm
O32_EnableWindow.asm
O32_EndDeferWindowPos.asm
O32_EndDialog.asm
O32_EndDoc.asm
O32_EndPage.asm
O32_EndPaint.asm
O32_EndPath.asm
O32_EnterCriticalSection.asm
O32_EnumChildWindows.asm
O32_EnumClipboardFormats.asm
O32_EnumEnhMetaFile.asm
O32_EnumFontFamilies.asm
O32_EnumFonts.asm
O32_EnumMetaFile.asm
O32_EnumObjects.asm
O32_EnumPrinters.asm
O32_EnumProps.asm
O32_EnumPropsEx.asm
O32_EnumThreadWindows.asm
O32_EnumWindows.asm
O32_EqualRect.asm
O32_Escape.asm
O32_ExcludeClipRect.asm
O32_ExcludeUpdateRgn.asm
O32_ExitProcess.asm
O32_ExitThread.asm
O32_ExitWindows.asm
O32_ExitWindowsEx.asm
O32_ExtCreatePen.asm
O32_ExtCreateRegion.asm
O32_ExtFloodFill.asm
O32_ExtSelectClipRgn.asm
O32_ExtTextOut.asm
O32_FatalAppExit.asm
O32_FatalExit.asm
O32_FileTimeToDosDateTime.asm
O32_FileTimeToLocalFileTime.asm
O32_FileTimeToSystemTime.asm
O32_FillPath.asm
O32_FillRect.asm
O32_FillRgn.asm
O32_FindAtom.asm
O32_FindClose.asm
O32_FindFirstFile.asm
O32_FindNextFile.asm
O32_FindResource.asm
O32_FindText.asm
O32_FindWindow.asm
O32_FlashWindow.asm
O32_FlattenPath.asm
O32_FloodFill.asm
O32_FlushFileBuffers.asm
O32_FrameRect.asm
O32_FrameRgn.asm
O32_FreeDDElParam.asm
O32_FreeLibrary.asm
O32_GetACP.asm
O32_GetActiveWindow.asm
O32_GetArcDirection.asm
O32_GetAspectRatioFilterEx.asm
O32_GetAtomName.asm
O32_GetBitmapBits.asm
O32_GetBitmapDimensionEx.asm
O32_GetBkColor.asm
O32_GetBkMode.asm
O32_GetBoundsRect.asm
O32_GetBrushOrgEx.asm
O32_GetCapture.asm
O32_GetCaretBlinkTime.asm
O32_GetCaretPos.asm
O32_GetCharABCWidths.asm
O32_GetCharWidth.asm
O32_GetClassInfo.asm
O32_GetClassLong.asm
O32_GetClassName.asm
O32_GetClassWord.asm
O32_GetClientRect.asm
O32_GetClipboardData.asm
O32_GetClipboardFormatName.asm
O32_GetClipboardOwner.asm
O32_GetClipboardViewer.asm
O32_GetClipBox.asm
O32_GetClipCursor.asm
O32_GetClipRgn.asm
O32_GetCommandLine.asm
O32_GetCurrentDirectory.asm
O32_GetCurrentObject.asm
O32_GetCurrentPositionEx.asm
O32_GetCurrentProcess.asm
O32_GetCurrentProcessId.asm
O32_GetCurrentThread.asm
O32_GetCurrentThreadId.asm
O32_GetCurrentTime.asm
O32_GetCursor.asm
O32_GetCursorPos.asm
O32_GetDC.asm
O32_GetDCEx.asm
O32_GetDCOrgEx.asm
O32_GetDesktopWindow.asm
O32_GetDeviceCaps.asm
O32_GetDialogBaseUnits.asm
O32_GetDIBits.asm
O32_GetDiskFreeSpace.asm
O32_GetDlgCtrlID.asm
O32_GetDlgItem.asm
O32_GetDlgItemInt.asm
O32_GetDlgItemText.asm
O32_GetDoubleClickTime.asm
O32_GetDriveType.asm
O32_GetEnhMetaFile.asm
O32_GetEnhMetaFileBits.asm
O32_GetEnhMetaFileHeader.asm
O32_GetEnhMetaFilePaletteEntries.asm
O32_GetEnvironmentStrings.asm
O32_GetEnvironmentVariable.asm
O32_GetExitCodeProcess.asm
O32_GetExitCodeThread.asm
O32_GetFileAttributes.asm
O32_GetFileInformationByHandle.asm
O32_GetFileSize.asm
O32_GetFileTime.asm
O32_GetFileTitle.asm
O32_GetFileType.asm
O32_GetFocus.asm
O32_GetForegroundWindow.asm
O32_GetFullPathName.asm
O32_GetGraphicsMode.asm
O32_GetIconInfo.asm
O32_GetKerningPairs.asm
O32_GetKeyboardType.asm
O32_GetKeyNameText.asm
O32_GetKeyState.asm
O32_GetLastActivePopup.asm
O32_GetLastError.asm
O32_GetLocalTime.asm
O32_GetLogicalDrives.asm
O32_GetLogicalDriveStrings.asm
O32_GetMapMode.asm
O32_GetMessage.asm
O32_GetMessageExtraInfo.asm
O32_GetMessagePos.asm
O32_GetMessageTime.asm
O32_GetMetaFile.asm
O32_GetMetaFileBitsEx.asm
O32_GetMiterLimit.asm
O32_GetModuleFileName.asm
O32_GetModuleHandle.asm
O32_GetNearestColor.asm
O32_GetNearestPaletteIndex.asm
O32_GetNextDlgGroupItem.asm
O32_GetNextWindow.asm
O32_GetObject.asm
O32_GetObjectType.asm
O32_GetOEMCP.asm
O32_GetOpenClipboardWindow.asm
O32_GetOpenFileName.asm
O32_GetOutlineTextMetrics.asm
O32_GetOverlappedResult.asm
O32_GetPaletteEntries.asm
O32_GetParent.asm
O32_GetPath.asm
O32_GetPixel.asm
O32_GetPolyFillMode.asm
O32_GetPriorityClass.asm
O32_GetPriorityClipboardFormat.asm
O32_GetPrivateProfileInt.asm
O32_GetPrivateProfileString.asm
O32_GetProcAddress.asm
O32_GetProfileInt.asm
O32_GetProfileString.asm
O32_GetProp.asm
O32_GetQueueStatus.asm
O32_GetRasterizerCaps.asm
O32_GetRegionData.asm
O32_GetRgnBox.asm
O32_GetROP2.asm
O32_GetSaveFileName.asm
O32_GetScrollPos.asm
O32_GetScrollRange.asm
O32_GetStdHandle.asm
O32_GetStockObject.asm
O32_GetStretchBltMode.asm
O32_GetSysColor.asm
O32_GetSystemDirectory.asm
O32_GetSystemMetrics.asm
O32_GetSystemPaletteEntries.asm
O32_GetSystemTime.asm
O32_GetTabbedTextExtent.asm
O32_GetTempFileName.asm
O32_GetTempPath.asm
O32_GetTextAlign.asm
O32_GetTextCharacterExtra.asm
O32_GetTextColor.asm
O32_GetTextExtentPoint.asm
O32_GetTextExtentPoint32.asm
O32_GetTextFace.asm
O32_GetTextMetrics.asm
O32_GetThreadPriority.asm
O32_GetTickCount.asm
O32_GetTimeZoneInformation.asm
O32_GetTopWindow.asm
O32_GetUpdateRect.asm
O32_GetUpdateRgn.asm
O32_GetViewportExtEx.asm
O32_GetViewportOrgEx.asm
O32_GetVolumeInformation.asm
O32_GetWindow.asm
O32_GetWindowDC.asm
O32_GetWindowExtEx.asm
O32_GetWindowLong.asm
O32_GetWindowOrgEx.asm
O32_GetWindowPlacement.asm
O32_GetWindowRect.asm
O32_GetWindowsDirectory.asm
O32_GetWindowText.asm
O32_GetWindowTextLength.asm
O32_GetWindowThreadProcessId.asm
O32_GetWindowWord.asm
O32_GetWinMetaFileBits.asm
O32_GetWorldTransform.asm
O32_HideCaret.asm
O32_InflateRect.asm
O32_InitAtomTable.asm
O32_InSendMessage.asm
O32_IsBadCodePtr.asm
O32_IsBadHugeReadPtr.asm
O32_IsBadHugeWritePtr.asm
O32_IsBadReadPtr.asm
O32_IsBadStringPtr.asm
O32_IsBadWritePtr.asm
O32_IsCharAlpha.asm
O32_IsCharAlphaNumeric.asm
O32_IsCharLower.asm
O32_IsCharUpper.asm
O32_IsChild.asm
O32_IsClipboardFormatAvailable.asm
O32_IsDBCSLeadByte.asm
O32_IsDialogMessage.asm
O32_IsDlgButtonChecked.asm
O32_IsIconic.asm
O32_IsMenu.asm
O32_IsRectEmpty.asm
O32_IsWindow.asm
O32_IsWindowEnabled.asm
O32_IsWindowVisible.asm
O32_IsZoomed.asm
O32_KillTimer.asm
O32_LeaveCriticalSection.asm
O32_LineDDA.asm
O32_LineTo.asm
O32_LoadAccelerators.asm
O32_LoadBitmap.asm
O32_LoadCursor.asm
O32_LoadIcon.asm
O32_LoadLibrary.asm
O32_LoadMenu.asm
O32_LoadMenuIndirect.asm
O32_LoadModule.asm
O32_LoadResource.asm
O32_LoadString.asm
O32_LocalAlloc.asm
O32_LocalDiscard.asm
O32_LocalFileTimeToFileTime.asm
O32_LocalFlags.asm
O32_LocalFree.asm
O32_LocalHandle.asm
O32_LocalLock.asm
O32_LocalReAlloc.asm
O32_LocalSize.asm
O32_LocalUnlock.asm
O32_LockFile.asm
O32_LockResource.asm
O32_LockWindowUpdate.asm
O32_LPtoDP.asm
O32_lstrcat.asm
O32_lstrcmp.asm
O32_lstrcmpi.asm
O32_lstrcpy.asm
O32_lstrlen.asm
O32_MapDialogRect.asm
O32_MapVirtualKey.asm
O32_MapWindowPoints.asm
O32_MaskBlt.asm
O32_MessageBeep.asm
O32_MessageBox.asm
O32_ModifyMenu.asm
O32_ModifyWorldTransform.asm
O32_MoveFile.asm
O32_MoveToEx.asm
O32_MoveWindow.asm
O32_MsgWaitForMultipleObjects.asm
O32_MulDiv.asm
O32_OemToChar.asm
O32_OemToCharBuff.asm
O32_OffsetClipRgn.asm
O32_OffsetRect.asm
O32_OffsetRgn.asm
O32_OffsetViewportOrgEx.asm
O32_OffsetWindowOrgEx.asm
O32_OpenClipboard.asm
O32_OpenEvent.asm
O32_OpenFile.asm
O32_OpenMutex.asm
O32_OpenProcess.asm
O32_OpenSemaphore.asm
O32_OutputDebugString.asm
O32_PackDDElParam.asm
O32_PaintRgn.asm
O32_PatBlt.asm
O32_PathToRegion.asm
O32_PeekMessage.asm
O32_Pie.asm
O32_PlayEnhMetaFile.asm
O32_PlayMetaFile.asm
O32_PlayMetaFileRecord.asm
O32_PolyBezier.asm
O32_PolyBezierTo.asm
O32_PolyDraw.asm
O32_Polygon.asm
O32_Polyline.asm
O32_PolylineTo.asm
O32_PolyPolygon.asm
O32_PolyPolyline.asm
O32_PostMessage.asm
O32_PostQuitMessage.asm
O32_PostThreadMessage.asm
O32_PrintDlg.asm
O32_PtInRect.asm
O32_PtInRegion.asm
O32_PtVisible.asm
O32_PulseEvent.asm
O32_ReadFile.asm
O32_RealizePalette.asm
O32_Rectangle.asm
O32_RectInRegion.asm
O32_RectVisible.asm
O32_RedrawWindow.asm
O32_RegCloseKey.asm
O32_RegCreateKey.asm
O32_RegCreateKeyEx.asm
O32_RegDeleteKey.asm
O32_RegDeleteValue.asm
O32_RegEnumKey.asm
O32_RegEnumKeyEx.asm
O32_RegEnumValue.asm
O32_RegisterClass.asm
O32_RegisterClipboardFormat.asm
O32_RegisterWindowMessage.asm
O32_RegOpenKey.asm
O32_RegOpenKeyEx.asm
O32_RegQueryInfoKey.asm
O32_RegQueryValue.asm
O32_RegQueryValueEx.asm
O32_RegSetValue.asm
O32_RegSetValueEx.asm
O32_ReleaseCapture.asm
O32_ReleaseDC.asm
O32_ReleaseMutex.asm
O32_ReleaseSemaphore.asm
O32_RemoveDirectory.asm
O32_RemoveFontResource.asm
O32_RemoveMenu.asm
O32_RemoveProp.asm
O32_ReplaceText.asm
O32_ReplyMessage.asm
O32_ResetDC.asm
O32_ResetEvent.asm
O32_ResizePalette.asm
O32_RestoreDC.asm
O32_ResumeThread.asm
O32_ReuseDDElParam.asm
O32_RoundRect.asm
O32_SaveDC.asm
O32_ScaleViewportExtEx.asm
O32_ScaleWindowExtEx.asm
O32_ScreenToClient.asm
O32_ScrollDC.asm
O32_ScrollWindow.asm
O32_ScrollWindowEx.asm
O32_SearchPath.asm
O32_SelectClipRgn.asm
O32_SelectObject.asm
O32_SelectPalette.asm
O32_SendDlgItemMessage.asm
O32_SendMessage.asm
O32_SetActiveWindow.asm
O32_SetArcDirection.asm
O32_SetBitmapBits.asm
O32_SetBitmapDimensionEx.asm
O32_SetBkColor.asm
O32_SetBkMode.asm
O32_SetBoundsRect.asm
O32_SetBrushOrgEx.asm
O32_SetCapture.asm
O32_SetCaretBlinkTime.asm
O32_SetCaretPos.asm
O32_SetClassLong.asm
O32_SetClassWord.asm
O32_SetClipboardData.asm
O32_SetClipboardViewer.asm
O32_SetCurrentDirectory.asm
O32_SetCursor.asm
O32_SetCursorPos.asm
O32_SetDIBits.asm
O32_SetDIBitsToDevice.asm
O32_SetDlgItemInt.asm
O32_SetDlgItemText.asm
O32_SetDoubleClickTime.asm
O32_SetEndOfFile.asm
O32_SetEnhMetaFileBits.asm
O32_SetEnvironmentVariable.asm
O32_SetEvent.asm
O32_SetFileAttributes.asm
O32_SetFilePointer.asm
O32_SetFileTime.asm
O32_SetFocus.asm
O32_SetForegroundWindow.asm
O32_SetGraphicsMode.asm
O32_SetHandleCount.asm
O32_SetLastError.asm
O32_SetLocalTime.asm
O32_SetMapMode.asm
O32_SetMapperFlags.asm
O32_SetMenu.asm
O32_SetMenuItemBitmaps.asm
O32_SetMetaFileBitsEx.asm
O32_SetMiterLimit.asm
O32_SetPaletteEntries.asm
O32_SetParent.asm
O32_SetPixel.asm
O32_SetPolyFillMode.asm
O32_SetPriorityClass.asm
O32_SetProp.asm
O32_SetRect.asm
O32_SetRectEmpty.asm
O32_SetRectRgn.asm
O32_SetROP2.asm
O32_SetScrollPos.asm
O32_SetScrollRange.asm
O32_SetStdHandle.asm
O32_SetStretchBltMode.asm
O32_SetSysColors.asm
O32_SetSystemTime.asm
O32_SetTextAlign.asm
O32_SetTextCharacterExtra.asm
O32_SetTextColor.asm
O32_SetTextJustification.asm
O32_SetThreadPriority.asm
O32_SetTimer.asm
O32_SetTimeZoneInformation.asm
O32_SetViewportExtEx.asm
O32_SetViewportOrgEx.asm
O32_SetVolumeLabel.asm
O32_SetWindowExtEx.asm
O32_SetWindowLong.asm
O32_SetWindowOrgEx.asm
O32_SetWindowPlacement.asm
O32_SetWindowPos.asm
O32_SetWindowsHookEx.asm
O32_SetWindowText.asm
O32_SetWindowWord.asm
O32_SetWinMetaFileBits.asm
O32_SetWorldTransform.asm
O32_ShowCaret.asm
O32_ShowCursor.asm
O32_ShowOwnedPopups.asm
O32_ShowScrollBar.asm
O32_ShowWindow.asm
O32_SizeofResource.asm
O32_Sleep.asm
O32_StartDoc.asm
O32_StartPage.asm
O32_StretchBlt.asm
O32_StretchDIBits.asm
O32_StrokeAndFillPath.asm
O32_StrokePath.asm
O32_SubtractRect.asm
O32_SuspendThread.asm
O32_SwapMouseButton.asm
O32_SystemParametersInfo.asm
O32_SystemTimeToFileTime.asm
O32_SystemTimeToTzSpecificLocalTime.asm
O32_TabbedTextOut.asm
O32_TerminateProcess.asm
O32_TerminateThread.asm
O32_TextOut.asm
O32_timeGetSystemTime.asm
O32_timeGetTime.asm
O32_TlsAlloc.asm
O32_TlsFree.asm
O32_TlsGetValue.asm
O32_TlsSetValue.asm
O32_TrackPopupMenu.asm
O32_TranslateAccelerator.asm
O32_TranslateMDISysAccel.asm
O32_TranslateMessage.asm
O32_UnhookWindowsHookEx.asm
O32_UnionRect.asm
O32_UnlockFile.asm
O32_UnpackDDElParam.asm
O32_UnrealizeObject.asm
O32_UnregisterClass.asm
O32_UpdateWindow.asm
O32_ValidateRect.asm
O32_ValidateRgn.asm
O32_VkKeyScan.asm
O32_WaitForMultipleObjects.asm
O32_WaitForSingleObject.asm
O32_WaitMessage.asm
O32_WidenPath.asm
O32_WinCallWinMain.asm
O32_WindowFromDC.asm
O32_WindowFromPoint.asm
O32_WinExec.asm
O32_WinHelp.asm
O32_WinQueryTranslateMode.asm
O32_WinSetTranslateMode.asm
O32_WinTranslateDevicePoints.asm
O32_WinTranslateDeviceRects.asm
O32_WinTranslateGraphicsObjectHandle.asm
O32_WinTranslateMnemonicString.asm
O32_WriteFile.asm
O32_WritePrivateProfileString.asm
O32_WriteProfileString.asm
O32_wvsprintf.asm
O32_ZeroMemory.asm
O32_GetAsyncKeyState.asm
O32_InvertRect.asm
O32_GetStartupInfo.asm
O32_WaitForInputIdle.asm
O32_CreateBitmapFromPMHandle.asm
O32_CreatePaletteFromPMHandle.asm
O32_GetPMHandleFromGDIHandle.asm
<<KEEP	
