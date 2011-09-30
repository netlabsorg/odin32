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
	$(AS) $(ASFLAGS) -Od -Fdo:$(OBJDIR) $(@B)
