# $Id: makefile,v 1.5 2004-04-20 10:11:17 sandervl Exp $

#
# Odin32 API
#
#       libwrap.lib makefile
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
$(OBJDIR)\_DdfBeginList.obj \
$(OBJDIR)\_DdfBitmap.obj \
$(OBJDIR)\_DdfEndList.obj \
$(OBJDIR)\_DdfHyperText.obj \
$(OBJDIR)\_DdfInform.obj \
$(OBJDIR)\_DdfInitialize.obj \
$(OBJDIR)\_DdfListItem.obj \
$(OBJDIR)\_DdfMetafile.obj \
$(OBJDIR)\_DdfPara.obj \
$(OBJDIR)\_DdfSetColor.obj \
$(OBJDIR)\_DdfSetFont.obj \
$(OBJDIR)\_DdfSetFontStyle.obj \
$(OBJDIR)\_DdfSetFormat.obj \
$(OBJDIR)\_DdfSetTextAlign.obj \
$(OBJDIR)\_DdfText.obj \
$(OBJDIR)\_DevCloseDC.obj \
$(OBJDIR)\_DevEscape.obj \
$(OBJDIR)\_DevOpenDC.obj \
$(OBJDIR)\_DevPostDeviceModes.obj \
$(OBJDIR)\_DevPostEscape.obj \
$(OBJDIR)\_DevQueryCaps.obj \
$(OBJDIR)\_DevQueryDeviceNames.obj \
$(OBJDIR)\_DevQueryHardcopyCaps.obj \
$(OBJDIR)\_DosAcknowledgeSignalException.obj \
$(OBJDIR)\_DosAddMuxWaitSem.obj \
$(OBJDIR)\_DosAliasMem.obj \
$(OBJDIR)\_DosAllocMem.obj \
$(OBJDIR)\_DosAllocSharedMem.obj \
$(OBJDIR)\_DosAllocThreadLocalMemory.obj \
$(OBJDIR)\_DosAsyncTimer.obj \
$(OBJDIR)\_DosBeep.obj \
$(OBJDIR)\_DosCallNPipe.obj \
$(OBJDIR)\_DosCancelLockRequest.obj \
$(OBJDIR)\_DosClose.obj \
$(OBJDIR)\_DosCloseEventSem.obj \
$(OBJDIR)\_DosCloseMutexSem.obj \
$(OBJDIR)\_DosCloseMuxWaitSem.obj \
$(OBJDIR)\_DosCloseQueue.obj \
$(OBJDIR)\_DosCloseVDD.obj \
$(OBJDIR)\_DosConnectNPipe.obj \
$(OBJDIR)\_DosCopy.obj \
$(OBJDIR)\_DosCreateDir.obj \
$(OBJDIR)\_DosCreateEventSem.obj \
$(OBJDIR)\_DosCreateMutexSem.obj \
$(OBJDIR)\_DosCreateMuxWaitSem.obj \
$(OBJDIR)\_DosCreateNPipe.obj \
$(OBJDIR)\_DosCreatePipe.obj \
$(OBJDIR)\_DosCreateQueue.obj \
$(OBJDIR)\_DosCreateThread.obj \
$(OBJDIR)\_DosDebug.obj \
$(OBJDIR)\_DosDelete.obj \
$(OBJDIR)\_DosDeleteDir.obj \
$(OBJDIR)\_DosDeleteMuxWaitSem.obj \
$(OBJDIR)\_DosDevConfig.obj \
$(OBJDIR)\_DosDevIOCtl.obj \
$(OBJDIR)\_DosDisConnectNPipe.obj \
$(OBJDIR)\_DosDumpProcess.obj \
$(OBJDIR)\_DosDupHandle.obj \
$(OBJDIR)\_DosEditName.obj \
$(OBJDIR)\_DosEnterCritSec.obj \
$(OBJDIR)\_DosEnterMustComplete.obj \
$(OBJDIR)\_DosEnumAttribute.obj \
$(OBJDIR)\_DosError.obj \
$(OBJDIR)\_DosExecPgm.obj \
$(OBJDIR)\_DosExit.obj \
$(OBJDIR)\_DosExitCritSec.obj \
$(OBJDIR)\_DosExitList.obj \
$(OBJDIR)\_DosExitMustComplete.obj \
$(OBJDIR)\_DosFindClose.obj \
$(OBJDIR)\_DosFindFirst.obj \
$(OBJDIR)\_DosFindNext.obj \
$(OBJDIR)\_DosForceDelete.obj \
$(OBJDIR)\_DosForceSystemDump.obj \
$(OBJDIR)\_DosFreeMem.obj \
$(OBJDIR)\_DosFreeModule.obj \
$(OBJDIR)\_DosFreeResource.obj \
$(OBJDIR)\_DosFreeThreadLocalMemory.obj \
$(OBJDIR)\_DosFSAttach.obj \
$(OBJDIR)\_DosFSCtl.obj \
$(OBJDIR)\_DosGetDateTime.obj \
$(OBJDIR)\_DosGetInfoBlocks.obj \
$(OBJDIR)\_DosGetMessage.obj \
$(OBJDIR)\_DosGetNamedSharedMem.obj \
$(OBJDIR)\_DosGetResource.obj \
$(OBJDIR)\_DosGetSharedMem.obj \
$(OBJDIR)\_DosGiveSharedMem.obj \
$(OBJDIR)\_DosInsertMessage.obj \
$(OBJDIR)\_DosKillProcess.obj \
$(OBJDIR)\_DosKillThread.obj \
$(OBJDIR)\_DosLoadModule.obj \
$(OBJDIR)\_DosMapCase.obj \
$(OBJDIR)\_DosMove.obj \
$(OBJDIR)\_DosOpen.obj \
$(OBJDIR)\_DosOpenEventSem.obj \
$(OBJDIR)\_DosOpenMutexSem.obj \
$(OBJDIR)\_DosOpenMuxWaitSem.obj \
$(OBJDIR)\_DosOpenQueue.obj \
$(OBJDIR)\_DosOpenVDD.obj \
$(OBJDIR)\_DosPeekNPipe.obj \
$(OBJDIR)\_DosPeekQueue.obj \
$(OBJDIR)\_DosPhysicalDisk.obj \
$(OBJDIR)\_DosPostEventSem.obj \
$(OBJDIR)\_DosProtectClose.obj \
$(OBJDIR)\_DosProtectEnumAttribute.obj \
$(OBJDIR)\_DosProtectOpen.obj \
$(OBJDIR)\_DosProtectQueryFHState.obj \
$(OBJDIR)\_DosProtectQueryFileInfo.obj \
$(OBJDIR)\_DosProtectRead.obj \
$(OBJDIR)\_DosProtectSetFHState.obj \
$(OBJDIR)\_DosProtectSetFileInfo.obj \
$(OBJDIR)\_DosProtectSetFileLocks.obj \
$(OBJDIR)\_DosProtectSetFilePtr.obj \
$(OBJDIR)\_DosProtectSetFileSize.obj \
$(OBJDIR)\_DosProtectWrite.obj \
$(OBJDIR)\_DosPurgeQueue.obj \
$(OBJDIR)\_DosPutMessage.obj \
$(OBJDIR)\_DosQueryAppType.obj \
$(OBJDIR)\_DosQueryCollate.obj \
$(OBJDIR)\_DosQueryCp.obj \
$(OBJDIR)\_DosQueryCtryInfo.obj \
$(OBJDIR)\_DosQueryCurrentDir.obj \
$(OBJDIR)\_DosQueryCurrentDisk.obj \
$(OBJDIR)\_DosQueryDBCSEnv.obj \
$(OBJDIR)\_DosQueryDOSProperty.obj \
$(OBJDIR)\_DosQueryEventSem.obj \
$(OBJDIR)\_DosQueryFHState.obj \
$(OBJDIR)\_DosQueryFileInfo.obj \
$(OBJDIR)\_DosQueryFSAttach.obj \
$(OBJDIR)\_DosQueryFSInfo.obj \
$(OBJDIR)\_DosQueryHType.obj \
$(OBJDIR)\_DosQueryMem.obj \
$(OBJDIR)\_DosQueryMessageCP.obj \
$(OBJDIR)\_DosQueryModuleHandle.obj \
$(OBJDIR)\_DosQueryModuleName.obj \
$(OBJDIR)\_DosQueryMutexSem.obj \
$(OBJDIR)\_DosQueryMuxWaitSem.obj \
$(OBJDIR)\_DosQueryNPHState.obj \
$(OBJDIR)\_DosQueryNPipeInfo.obj \
$(OBJDIR)\_DosQueryNPipeSemState.obj \
$(OBJDIR)\_DosQueryPathInfo.obj \
$(OBJDIR)\_DosQueryProcAddr.obj \
$(OBJDIR)\_DosQueryProcType.obj \
$(OBJDIR)\_DosQueryQueue.obj \
$(OBJDIR)\_DosQueryRASInfo.obj \
$(OBJDIR)\_DosQueryResourceSize.obj \
$(OBJDIR)\_DosQuerySysInfo.obj \
$(OBJDIR)\_DosQueryThreadContext.obj \
$(OBJDIR)\_DosQueryVerify.obj \
$(OBJDIR)\_DosRaiseException.obj \
$(OBJDIR)\_DosRead.obj \
$(OBJDIR)\_DosReadQueue.obj \
$(OBJDIR)\_DosReleaseMutexSem.obj \
$(OBJDIR)\_DosRequestMutexSem.obj \
$(OBJDIR)\_DosRequestVDD.obj \
$(OBJDIR)\_DosResetBuffer.obj \
$(OBJDIR)\_DosResetEventSem.obj \
$(OBJDIR)\_DosResumeThread.obj \
$(OBJDIR)\_DosScanEnv.obj \
$(OBJDIR)\_DosSearchPath.obj \
$(OBJDIR)\_DosSelectSession.obj \
$(OBJDIR)\_DosSendSignalException.obj \
$(OBJDIR)\_DosSetCurrentDir.obj \
$(OBJDIR)\_DosSetDateTime.obj \
$(OBJDIR)\_DosSetDefaultDisk.obj \
$(OBJDIR)\_DosSetDOSProperty.obj \
$(OBJDIR)\_DosSetExceptionHandler.obj \
$(OBJDIR)\_DosSetFHState.obj \
$(OBJDIR)\_DosSetFileInfo.obj \
$(OBJDIR)\_DosSetFileLocks.obj \
$(OBJDIR)\_DosSetFilePtr.obj \
$(OBJDIR)\_DosSetFileSize.obj \
$(OBJDIR)\_DosSetFSInfo.obj \
$(OBJDIR)\_DosSetMaxFH.obj \
$(OBJDIR)\_DosSetMem.obj \
$(OBJDIR)\_DosSetNPHState.obj \
$(OBJDIR)\_DosSetNPipeSem.obj \
$(OBJDIR)\_DosSetPathInfo.obj \
$(OBJDIR)\_DosSetPriority.obj \
$(OBJDIR)\_DosSetProcessCp.obj \
$(OBJDIR)\_DosSetRelMaxFH.obj \
$(OBJDIR)\_DosSetSession.obj \
$(OBJDIR)\_DosSetSignalExceptionFocus.obj \
$(OBJDIR)\_DosSetVerify.obj \
$(OBJDIR)\_DosShutdown.obj \
$(OBJDIR)\_DosSleep.obj \
$(OBJDIR)\_DosStartSession.obj \
$(OBJDIR)\_DosStartTimer.obj \
$(OBJDIR)\_DosStopSession.obj \
$(OBJDIR)\_DosStopTimer.obj \
$(OBJDIR)\_DosSubAllocMem.obj \
$(OBJDIR)\_DosSubFreeMem.obj \
$(OBJDIR)\_DosSubSetMem.obj \
$(OBJDIR)\_DosSubUnsetMem.obj \
$(OBJDIR)\_DosSuppressPopUps.obj \
$(OBJDIR)\_DosSuspendThread.obj \
$(OBJDIR)\_DosTmrQueryFreq.obj \
$(OBJDIR)\_DosTmrQueryTime.obj \
$(OBJDIR)\_DosTransactNPipe.obj \
$(OBJDIR)\_DosUnsetExceptionHandler.obj \
$(OBJDIR)\_DosUnwindException.obj \
$(OBJDIR)\_DosWaitChild.obj \
$(OBJDIR)\_DosWaitEventSem.obj \
$(OBJDIR)\_DosWaitMuxWaitSem.obj \
$(OBJDIR)\_DosWaitNPipe.obj \
$(OBJDIR)\_DosWaitThread.obj \
$(OBJDIR)\_DosWrite.obj \
$(OBJDIR)\_DosWriteQueue.obj \
$(OBJDIR)\_DrgAcceptDroppedFiles.obj \
$(OBJDIR)\_DrgAccessDraginfo.obj \
$(OBJDIR)\_DrgAddStrHandle.obj \
$(OBJDIR)\_DrgAllocDraginfo.obj \
$(OBJDIR)\_DrgAllocDragtransfer.obj \
$(OBJDIR)\_DrgCancelLazyDrag.obj \
$(OBJDIR)\_DrgDeleteDraginfoStrHandles.obj \
$(OBJDIR)\_DrgDeleteStrHandle.obj \
$(OBJDIR)\_DrgDrag.obj \
$(OBJDIR)\_DrgDragFiles.obj \
$(OBJDIR)\_DrgFreeDraginfo.obj \
$(OBJDIR)\_DrgFreeDragtransfer.obj \
$(OBJDIR)\_DrgGetPS.obj \
$(OBJDIR)\_DrgLazyDrag.obj \
$(OBJDIR)\_DrgLazyDrop.obj \
$(OBJDIR)\_DrgPostTransferMsg.obj \
$(OBJDIR)\_DrgPushDraginfo.obj \
$(OBJDIR)\_DrgQueryDraginfoPtr.obj \
$(OBJDIR)\_DrgQueryDraginfoPtrFromDragitem.obj \
$(OBJDIR)\_DrgQueryDraginfoPtrFromHwnd.obj \
$(OBJDIR)\_DrgQueryDragitem.obj \
$(OBJDIR)\_DrgQueryDragitemCount.obj \
$(OBJDIR)\_DrgQueryDragitemPtr.obj \
$(OBJDIR)\_DrgQueryDragStatus.obj \
$(OBJDIR)\_DrgQueryNativeRMF.obj \
$(OBJDIR)\_DrgQueryNativeRMFLen.obj \
$(OBJDIR)\_DrgQueryStrName.obj \
$(OBJDIR)\_DrgQueryStrNameLen.obj \
$(OBJDIR)\_DrgQueryTrueType.obj \
$(OBJDIR)\_DrgQueryTrueTypeLen.obj \
$(OBJDIR)\_DrgReallocDraginfo.obj \
$(OBJDIR)\_DrgReleasePS.obj \
$(OBJDIR)\_DrgSendTransferMsg.obj \
$(OBJDIR)\_DrgSetDragImage.obj \
$(OBJDIR)\_DrgSetDragitem.obj \
$(OBJDIR)\_DrgSetDragPointer.obj \
$(OBJDIR)\_DrgVerifyNativeRMF.obj \
$(OBJDIR)\_DrgVerifyRMF.obj \
$(OBJDIR)\_DrgVerifyTrueType.obj \
$(OBJDIR)\_DrgVerifyType.obj \
$(OBJDIR)\_DrgVerifyTypeSet.obj \
$(OBJDIR)\_GpiAnimatePalette.obj \
$(OBJDIR)\_GpiAssociate.obj \
$(OBJDIR)\_GpiBeginArea.obj \
$(OBJDIR)\_GpiBeginElement.obj \
$(OBJDIR)\_GpiBeginInkPath.obj \
$(OBJDIR)\_GpiBeginPath.obj \
$(OBJDIR)\_GpiBitBlt.obj \
$(OBJDIR)\_GpiBox.obj \
$(OBJDIR)\_GpiCallSegmentMatrix.obj \
$(OBJDIR)\_GpiCharString.obj \
$(OBJDIR)\_GpiCharStringAt.obj \
$(OBJDIR)\_GpiCharStringPos.obj \
$(OBJDIR)\_GpiCharStringPosAt.obj \
$(OBJDIR)\_GpiCloseFigure.obj \
$(OBJDIR)\_GpiCloseSegment.obj \
$(OBJDIR)\_GpiCombineRegion.obj \
$(OBJDIR)\_GpiComment.obj \
$(OBJDIR)\_GpiConvert.obj \
$(OBJDIR)\_GpiConvertWithMatrix.obj \
$(OBJDIR)\_GpiCopyMetaFile.obj \
$(OBJDIR)\_GpiCorrelateChain.obj \
$(OBJDIR)\_GpiCorrelateFrom.obj \
$(OBJDIR)\_GpiCorrelateSegment.obj \
$(OBJDIR)\_GpiCreateBitmap.obj \
$(OBJDIR)\_GpiCreateEllipticRegion.obj \
$(OBJDIR)\_GpiCreateLogColorTable.obj \
$(OBJDIR)\_GpiCreateLogFont.obj \
$(OBJDIR)\_GpiCreatePalette.obj \
$(OBJDIR)\_GpiCreatePolygonRegion.obj \
$(OBJDIR)\_GpiCreatePS.obj \
$(OBJDIR)\_GpiCreateRegion.obj \
$(OBJDIR)\_GpiCreateRoundRectRegion.obj \
$(OBJDIR)\_GpiDeleteBitmap.obj \
$(OBJDIR)\_GpiDeleteElement.obj \
$(OBJDIR)\_GpiDeleteElementRange.obj \
$(OBJDIR)\_GpiDeleteElementsBetweenLabels.obj \
$(OBJDIR)\_GpiDeleteMetaFile.obj \
$(OBJDIR)\_GpiDeletePalette.obj \
$(OBJDIR)\_GpiDeleteSegment.obj \
$(OBJDIR)\_GpiDeleteSegments.obj \
$(OBJDIR)\_GpiDeleteSetId.obj \
$(OBJDIR)\_GpiDestroyPS.obj \
$(OBJDIR)\_GpiDestroyRegion.obj \
$(OBJDIR)\_GpiDrawBits.obj \
$(OBJDIR)\_GpiDrawChain.obj \
$(OBJDIR)\_GpiDrawDynamics.obj \
$(OBJDIR)\_GpiDrawFrom.obj \
$(OBJDIR)\_GpiDrawSegment.obj \
$(OBJDIR)\_GpiElement.obj \
$(OBJDIR)\_GpiEndArea.obj \
$(OBJDIR)\_GpiEndElement.obj \
$(OBJDIR)\_GpiEndInkPath.obj \
$(OBJDIR)\_GpiEndPath.obj \
$(OBJDIR)\_GpiEqualRegion.obj \
$(OBJDIR)\_GpiErase.obj \
$(OBJDIR)\_GpiErrorSegmentData.obj \
$(OBJDIR)\_GpiExcludeClipRectangle.obj \
$(OBJDIR)\_GpiFillPath.obj \
$(OBJDIR)\_GpiFloodFill.obj \
$(OBJDIR)\_GpiFrameRegion.obj \
$(OBJDIR)\_GpiFullArc.obj \
$(OBJDIR)\_GpiGetData.obj \
$(OBJDIR)\_GpiImage.obj \
$(OBJDIR)\_GpiIntersectClipRectangle.obj \
$(OBJDIR)\_GpiLabel.obj \
$(OBJDIR)\_GpiLine.obj \
$(OBJDIR)\_GpiLoadBitmap.obj \
$(OBJDIR)\_GpiLoadFonts.obj \
$(OBJDIR)\_GpiLoadMetaFile.obj \
$(OBJDIR)\_GpiLoadPublicFonts.obj \
$(OBJDIR)\_GpiMarker.obj \
$(OBJDIR)\_GpiModifyPath.obj \
$(OBJDIR)\_GpiMove.obj \
$(OBJDIR)\_GpiOffsetClipRegion.obj \
$(OBJDIR)\_GpiOffsetElementPointer.obj \
$(OBJDIR)\_GpiOffsetRegion.obj \
$(OBJDIR)\_GpiOpenSegment.obj \
$(OBJDIR)\_GpiOutlinePath.obj \
$(OBJDIR)\_GpiPaintRegion.obj \
$(OBJDIR)\_GpiPartialArc.obj \
$(OBJDIR)\_GpiPathToRegion.obj \
$(OBJDIR)\_GpiPlayMetaFile.obj \
$(OBJDIR)\_GpiPointArc.obj \
$(OBJDIR)\_GpiPolyFillet.obj \
$(OBJDIR)\_GpiPolyFilletSharp.obj \
$(OBJDIR)\_GpiPolygons.obj \
$(OBJDIR)\_GpiPolyLine.obj \
$(OBJDIR)\_GpiPolyLineDisjoint.obj \
$(OBJDIR)\_GpiPolyMarker.obj \
$(OBJDIR)\_GpiPolySpline.obj \
$(OBJDIR)\_GpiPop.obj \
$(OBJDIR)\_GpiPtInRegion.obj \
$(OBJDIR)\_GpiPtVisible.obj \
$(OBJDIR)\_GpiPutData.obj \
$(OBJDIR)\_GpiQueryArcParams.obj \
$(OBJDIR)\_GpiQueryAttrMode.obj \
$(OBJDIR)\_GpiQueryAttrs.obj \
$(OBJDIR)\_GpiQueryBackColor.obj \
$(OBJDIR)\_GpiQueryBackMix.obj \
$(OBJDIR)\_GpiQueryBitmapBits.obj \
$(OBJDIR)\_GpiQueryBitmapDimension.obj \
$(OBJDIR)\_GpiQueryBitmapHandle.obj \
$(OBJDIR)\_GpiQueryBitmapInfoHeader.obj \
$(OBJDIR)\_GpiQueryBitmapParameters.obj \
$(OBJDIR)\_GpiQueryBoundaryData.obj \
$(OBJDIR)\_GpiQueryCharAngle.obj \
$(OBJDIR)\_GpiQueryCharBox.obj \
$(OBJDIR)\_GpiQueryCharBreakExtra.obj \
$(OBJDIR)\_GpiQueryCharDirection.obj \
$(OBJDIR)\_GpiQueryCharExtra.obj \
$(OBJDIR)\_GpiQueryCharMode.obj \
$(OBJDIR)\_GpiQueryCharSet.obj \
$(OBJDIR)\_GpiQueryCharShear.obj \
$(OBJDIR)\_GpiQueryCharStringPos.obj \
$(OBJDIR)\_GpiQueryCharStringPosAt.obj \
$(OBJDIR)\_GpiQueryClipBox.obj \
$(OBJDIR)\_GpiQueryClipRegion.obj \
$(OBJDIR)\_GpiQueryColor.obj \
$(OBJDIR)\_GpiQueryColorData.obj \
$(OBJDIR)\_GpiQueryColorIndex.obj \
$(OBJDIR)\_GpiQueryCp.obj \
$(OBJDIR)\_GpiQueryCurrentPosition.obj \
$(OBJDIR)\_GpiQueryDefArcParams.obj \
$(OBJDIR)\_GpiQueryDefAttrs.obj \
$(OBJDIR)\_GpiQueryDefaultViewMatrix.obj \
$(OBJDIR)\_GpiQueryDefCharBox.obj \
$(OBJDIR)\_GpiQueryDefTag.obj \
$(OBJDIR)\_GpiQueryDefViewingLimits.obj \
$(OBJDIR)\_GpiQueryDevice.obj \
$(OBJDIR)\_GpiQueryDeviceBitmapFormats.obj \
$(OBJDIR)\_GpiQueryDrawControl.obj \
$(OBJDIR)\_GpiQueryDrawingMode.obj \
$(OBJDIR)\_GpiQueryEditMode.obj \
$(OBJDIR)\_GpiQueryElement.obj \
$(OBJDIR)\_GpiQueryElementPointer.obj \
$(OBJDIR)\_GpiQueryElementType.obj \
$(OBJDIR)\_GpiQueryFaceString.obj \
$(OBJDIR)\_GpiQueryFontAction.obj \
$(OBJDIR)\_GpiQueryFontFileDescriptions.obj \
$(OBJDIR)\_GpiQueryFontMetrics.obj \
$(OBJDIR)\_GpiQueryFonts.obj \
$(OBJDIR)\_GpiQueryFullFontFileDescs.obj \
$(OBJDIR)\_GpiQueryGraphicsField.obj \
$(OBJDIR)\_GpiQueryInitialSegmentAttrs.obj \
$(OBJDIR)\_GpiQueryKerningPairs.obj \
$(OBJDIR)\_GpiQueryLineEnd.obj \
$(OBJDIR)\_GpiQueryLineJoin.obj \
$(OBJDIR)\_GpiQueryLineType.obj \
$(OBJDIR)\_GpiQueryLineWidth.obj \
$(OBJDIR)\_GpiQueryLineWidthGeom.obj \
$(OBJDIR)\_GpiQueryLogColorTable.obj \
$(OBJDIR)\_GpiQueryLogicalFont.obj \
$(OBJDIR)\_GpiQueryMarker.obj \
$(OBJDIR)\_GpiQueryMarkerBox.obj \
$(OBJDIR)\_GpiQueryMarkerSet.obj \
$(OBJDIR)\_GpiQueryMetaFileBits.obj \
$(OBJDIR)\_GpiQueryMetaFileLength.obj \
$(OBJDIR)\_GpiQueryMix.obj \
$(OBJDIR)\_GpiQueryModelTransformMatrix.obj \
$(OBJDIR)\_GpiQueryNearestColor.obj \
$(OBJDIR)\_GpiQueryNumberSetIds.obj \
$(OBJDIR)\_GpiQueryPageViewport.obj \
$(OBJDIR)\_GpiQueryPalette.obj \
$(OBJDIR)\_GpiQueryPaletteInfo.obj \
$(OBJDIR)\_GpiQueryPattern.obj \
$(OBJDIR)\_GpiQueryPatternRefPoint.obj \
$(OBJDIR)\_GpiQueryPatternSet.obj \
$(OBJDIR)\_GpiQueryPel.obj \
$(OBJDIR)\_GpiQueryPickAperturePosition.obj \
$(OBJDIR)\_GpiQueryPickApertureSize.obj \
$(OBJDIR)\_GpiQueryPS.obj \
$(OBJDIR)\_GpiQueryRealColors.obj \
$(OBJDIR)\_GpiQueryRegionBox.obj \
$(OBJDIR)\_GpiQueryRegionRects.obj \
$(OBJDIR)\_GpiQueryRGBColor.obj \
$(OBJDIR)\_GpiQuerySegmentAttrs.obj \
$(OBJDIR)\_GpiQuerySegmentNames.obj \
$(OBJDIR)\_GpiQuerySegmentPriority.obj \
$(OBJDIR)\_GpiQuerySegmentTransformMatrix.obj \
$(OBJDIR)\_GpiQuerySetIds.obj \
$(OBJDIR)\_GpiQueryStopDraw.obj \
$(OBJDIR)\_GpiQueryTag.obj \
$(OBJDIR)\_GpiQueryTextAlignment.obj \
$(OBJDIR)\_GpiQueryTextBox.obj \
$(OBJDIR)\_GpiQueryViewingLimits.obj \
$(OBJDIR)\_GpiQueryViewingTransformMatrix.obj \
$(OBJDIR)\_GpiQueryWidthTable.obj \
$(OBJDIR)\_GpiRectInRegion.obj \
$(OBJDIR)\_GpiRectVisible.obj \
$(OBJDIR)\_GpiRemoveDynamics.obj \
$(OBJDIR)\_GpiResetBoundaryData.obj \
$(OBJDIR)\_GpiResetPS.obj \
$(OBJDIR)\_GpiRestorePS.obj \
$(OBJDIR)\_GpiRotate.obj \
$(OBJDIR)\_GpiSaveMetaFile.obj \
$(OBJDIR)\_GpiSavePS.obj \
$(OBJDIR)\_GpiScale.obj \
$(OBJDIR)\_GpiSelectPalette.obj \
$(OBJDIR)\_GpiSetArcParams.obj \
$(OBJDIR)\_GpiSetAttrMode.obj \
$(OBJDIR)\_GpiSetAttrs.obj \
$(OBJDIR)\_GpiSetBackColor.obj \
$(OBJDIR)\_GpiSetBackMix.obj \
$(OBJDIR)\_GpiSetBitmap.obj \
$(OBJDIR)\_GpiSetBitmapBits.obj \
$(OBJDIR)\_GpiSetBitmapDimension.obj \
$(OBJDIR)\_GpiSetBitmapId.obj \
$(OBJDIR)\_GpiSetCharAngle.obj \
$(OBJDIR)\_GpiSetCharBox.obj \
$(OBJDIR)\_GpiSetCharBreakExtra.obj \
$(OBJDIR)\_GpiSetCharDirection.obj \
$(OBJDIR)\_GpiSetCharExtra.obj \
$(OBJDIR)\_GpiSetCharMode.obj \
$(OBJDIR)\_GpiSetCharSet.obj \
$(OBJDIR)\_GpiSetCharShear.obj \
$(OBJDIR)\_GpiSetClipPath.obj \
$(OBJDIR)\_GpiSetClipRegion.obj \
$(OBJDIR)\_GpiSetColor.obj \
$(OBJDIR)\_GpiSetCp.obj \
$(OBJDIR)\_GpiSetCurrentPosition.obj \
$(OBJDIR)\_GpiSetDefArcParams.obj \
$(OBJDIR)\_GpiSetDefAttrs.obj \
$(OBJDIR)\_GpiSetDefaultViewMatrix.obj \
$(OBJDIR)\_GpiSetDefTag.obj \
$(OBJDIR)\_GpiSetDefViewingLimits.obj \
$(OBJDIR)\_GpiSetDrawControl.obj \
$(OBJDIR)\_GpiSetDrawingMode.obj \
$(OBJDIR)\_GpiSetEditMode.obj \
$(OBJDIR)\_GpiSetElementPointer.obj \
$(OBJDIR)\_GpiSetElementPointerAtLabel.obj \
$(OBJDIR)\_GpiSetGraphicsField.obj \
$(OBJDIR)\_GpiSetInitialSegmentAttrs.obj \
$(OBJDIR)\_GpiSetLineEnd.obj \
$(OBJDIR)\_GpiSetLineJoin.obj \
$(OBJDIR)\_GpiSetLineType.obj \
$(OBJDIR)\_GpiSetLineWidth.obj \
$(OBJDIR)\_GpiSetLineWidthGeom.obj \
$(OBJDIR)\_GpiSetMarker.obj \
$(OBJDIR)\_GpiSetMarkerBox.obj \
$(OBJDIR)\_GpiSetMarkerSet.obj \
$(OBJDIR)\_GpiSetMetaFileBits.obj \
$(OBJDIR)\_GpiSetMix.obj \
$(OBJDIR)\_GpiSetModelTransformMatrix.obj \
$(OBJDIR)\_GpiSetPageViewport.obj \
$(OBJDIR)\_GpiSetPaletteEntries.obj \
$(OBJDIR)\_GpiSetPattern.obj \
$(OBJDIR)\_GpiSetPatternRefPoint.obj \
$(OBJDIR)\_GpiSetPatternSet.obj \
$(OBJDIR)\_GpiSetPel.obj \
$(OBJDIR)\_GpiSetPickAperturePosition.obj \
$(OBJDIR)\_GpiSetPickApertureSize.obj \
$(OBJDIR)\_GpiSetPS.obj \
$(OBJDIR)\_GpiSetRegion.obj \
$(OBJDIR)\_GpiSetSegmentAttrs.obj \
$(OBJDIR)\_GpiSetSegmentPriority.obj \
$(OBJDIR)\_GpiSetSegmentTransformMatrix.obj \
$(OBJDIR)\_GpiSetStopDraw.obj \
$(OBJDIR)\_GpiSetTag.obj \
$(OBJDIR)\_GpiSetTextAlignment.obj \
$(OBJDIR)\_GpiSetViewingLimits.obj \
$(OBJDIR)\_GpiSetViewingTransformMatrix.obj \
$(OBJDIR)\_GpiStrokeInkPath.obj \
$(OBJDIR)\_GpiStrokePath.obj \
$(OBJDIR)\_GpiTranslate.obj \
$(OBJDIR)\_GpiUnloadFonts.obj \
$(OBJDIR)\_GpiUnloadPublicFonts.obj \
$(OBJDIR)\_GpiWCBitBlt.obj \
$(OBJDIR)\_PrfAddProgram.obj \
$(OBJDIR)\_PrfChangeProgram.obj \
$(OBJDIR)\_PrfCloseProfile.obj \
$(OBJDIR)\_PrfCreateGroup.obj \
$(OBJDIR)\_PrfDestroyGroup.obj \
$(OBJDIR)\_PrfOpenProfile.obj \
$(OBJDIR)\_PrfQueryDefinition.obj \
$(OBJDIR)\_PrfQueryProfile.obj \
$(OBJDIR)\_PrfQueryProfileData.obj \
$(OBJDIR)\_PrfQueryProfileInt.obj \
$(OBJDIR)\_PrfQueryProfileSize.obj \
$(OBJDIR)\_PrfQueryProfileString.obj \
$(OBJDIR)\_PrfQueryProgramCategory.obj \
$(OBJDIR)\_PrfQueryProgramHandle.obj \
$(OBJDIR)\_PrfQueryProgramTitles.obj \
$(OBJDIR)\_PrfRemoveProgram.obj \
$(OBJDIR)\_PrfReset.obj \
$(OBJDIR)\_PrfWriteProfileData.obj \
$(OBJDIR)\_PrfWriteProfileString.obj \
$(OBJDIR)\_RexxAddMacro.obj \
$(OBJDIR)\_RexxClearMacroSpace.obj \
$(OBJDIR)\_RexxDeregisterExit.obj \
$(OBJDIR)\_RexxDeregisterFunction.obj \
$(OBJDIR)\_RexxDeregisterSubcom.obj \
$(OBJDIR)\_RexxDropMacro.obj \
$(OBJDIR)\_RexxLoadMacroSpace.obj \
$(OBJDIR)\_RexxQueryExit.obj \
$(OBJDIR)\_RexxQueryFunction.obj \
$(OBJDIR)\_RexxQueryMacro.obj \
$(OBJDIR)\_RexxQuerySubcom.obj \
$(OBJDIR)\_RexxRegisterExitDll.obj \
$(OBJDIR)\_RexxRegisterExitExe.obj \
$(OBJDIR)\_RexxRegisterFunctionDll.obj \
$(OBJDIR)\_RexxRegisterFunctionExe.obj \
$(OBJDIR)\_RexxRegisterSubcomDll.obj \
$(OBJDIR)\_RexxRegisterSubcomExe.obj \
$(OBJDIR)\_RexxReorderMacro.obj \
$(OBJDIR)\_RexxResetTrace.obj \
$(OBJDIR)\_RexxSaveMacroSpace.obj \
$(OBJDIR)\_RexxSetHalt.obj \
$(OBJDIR)\_RexxSetTrace.obj \
$(OBJDIR)\_RexxStart.obj \
$(OBJDIR)\_RexxVariablePool.obj \
$(OBJDIR)\_SplControlDevice.obj \
$(OBJDIR)\_SplCopyJob.obj \
$(OBJDIR)\_SplCreateDevice.obj \
$(OBJDIR)\_SplCreatePort.obj \
$(OBJDIR)\_SplCreateQueue.obj \
$(OBJDIR)\_SplDeleteDevice.obj \
$(OBJDIR)\_SplDeleteJob.obj \
$(OBJDIR)\_SplDeletePort.obj \
$(OBJDIR)\_SplDeleteQueue.obj \
$(OBJDIR)\_SplEnumDevice.obj \
$(OBJDIR)\_SplEnumDriver.obj \
$(OBJDIR)\_SplEnumJob.obj \
$(OBJDIR)\_SplEnumPort.obj \
$(OBJDIR)\_SplEnumPrinter.obj \
$(OBJDIR)\_SplEnumQueue.obj \
$(OBJDIR)\_SplEnumQueueProcessor.obj \
$(OBJDIR)\_SplHoldJob.obj \
$(OBJDIR)\_SplHoldQueue.obj \
$(OBJDIR)\_SplMessageBox.obj \
$(OBJDIR)\_SplPurgeQueue.obj \
$(OBJDIR)\_SplQmAbort.obj \
$(OBJDIR)\_SplQmAbortDoc.obj \
$(OBJDIR)\_SplQmClose.obj \
$(OBJDIR)\_SplQmEndDoc.obj \
$(OBJDIR)\_SplQmGetJobID.obj \
$(OBJDIR)\_SplQmNewPage.obj \
$(OBJDIR)\_SplQmOpen.obj \
$(OBJDIR)\_SplQmStartDoc.obj \
$(OBJDIR)\_SplQmWrite.obj \
$(OBJDIR)\_SplQueryDevice.obj \
$(OBJDIR)\_SplQueryDriver.obj \
$(OBJDIR)\_SplQueryJob.obj \
$(OBJDIR)\_SplQueryPort.obj \
$(OBJDIR)\_SplQueryQueue.obj \
$(OBJDIR)\_SplReleaseJob.obj \
$(OBJDIR)\_SplReleaseQueue.obj \
$(OBJDIR)\_SplSetDevice.obj \
$(OBJDIR)\_SplSetDriver.obj \
$(OBJDIR)\_SplSetJob.obj \
$(OBJDIR)\_SplSetPort.obj \
$(OBJDIR)\_SplSetQueue.obj \
$(OBJDIR)\_SplStdClose.obj \
$(OBJDIR)\_SplStdDelete.obj \
$(OBJDIR)\_SplStdGetBits.obj \
$(OBJDIR)\_SplStdOpen.obj \
$(OBJDIR)\_SplStdQueryLength.obj \
$(OBJDIR)\_SplStdStart.obj \
$(OBJDIR)\_SplStdStop.obj \
$(OBJDIR)\_Win32AddClipbrdViewer.obj \
$(OBJDIR)\_Win32QueryClipbrdViewerChain.obj \
$(OBJDIR)\_Win32QueryOpenClipbrdWindow.obj \
$(OBJDIR)\_Win32RemoveClipbrdViewer.obj \
$(OBJDIR)\_WinAddAtom.obj \
$(OBJDIR)\_WinAddSwitchEntry.obj \
$(OBJDIR)\_WinAlarm.obj \
$(OBJDIR)\_WinAssociateHelpInstance.obj \
$(OBJDIR)\_WinBeginEnumWindows.obj \
$(OBJDIR)\_WinBeginPaint.obj \
$(OBJDIR)\_WinBroadcastMsg.obj \
$(OBJDIR)\_WinCalcFrameRect.obj \
$(OBJDIR)\_WinCallMsgFilter.obj \
$(OBJDIR)\_WinCancelShutdown.obj \
$(OBJDIR)\_WinChangeSwitchEntry.obj \
$(OBJDIR)\_WinCheckInput.obj \
$(OBJDIR)\_WinCloseClipbrd.obj \
$(OBJDIR)\_WinCompareStrings.obj \
$(OBJDIR)\_WinCopyAccelTable.obj \
$(OBJDIR)\_WinCopyObject.obj \
$(OBJDIR)\_WinCopyRect.obj \
$(OBJDIR)\_WinCpTranslateChar.obj \
$(OBJDIR)\_WinCpTranslateString.obj \
$(OBJDIR)\_WinCreateAccelTable.obj \
$(OBJDIR)\_WinCreateAtomTable.obj \
$(OBJDIR)\_WinCreateCursor.obj \
$(OBJDIR)\_WinCreateDlg.obj \
$(OBJDIR)\_WinCreateFrameControls.obj \
$(OBJDIR)\_WinCreateHelpInstance.obj \
$(OBJDIR)\_WinCreateHelpTable.obj \
$(OBJDIR)\_WinCreateMenu.obj \
$(OBJDIR)\_WinCreateMsgQueue.obj \
$(OBJDIR)\_WinCreateObject.obj \
$(OBJDIR)\_WinCreatePointer.obj \
$(OBJDIR)\_WinCreatePointerIndirect.obj \
$(OBJDIR)\_WinCreateShadow.obj \
$(OBJDIR)\_WinCreateStdWindow.obj \
$(OBJDIR)\_WinCreateSwitchEntry.obj \
$(OBJDIR)\_WinCreateWindow.obj \
$(OBJDIR)\_WinDdeInitiate.obj \
$(OBJDIR)\_WinDdePostMsg.obj \
$(OBJDIR)\_WinDdeRespond.obj \
$(OBJDIR)\_WinDefDlgProc.obj \
$(OBJDIR)\_WinDefFileDlgProc.obj \
$(OBJDIR)\_WinDefFontDlgProc.obj \
$(OBJDIR)\_WinDefWindowProc.obj \
$(OBJDIR)\_WinDeleteAtom.obj \
$(OBJDIR)\_WinDeleteLibrary.obj \
$(OBJDIR)\_WinDeleteProcedure.obj \
$(OBJDIR)\_WinDeregisterObjectClass.obj \
$(OBJDIR)\_WinDestroyAccelTable.obj \
$(OBJDIR)\_WinDestroyAtomTable.obj \
$(OBJDIR)\_WinDestroyCursor.obj \
$(OBJDIR)\_WinDestroyHelpInstance.obj \
$(OBJDIR)\_WinDestroyMsgQueue.obj \
$(OBJDIR)\_WinDestroyObject.obj \
$(OBJDIR)\_WinDestroyPointer.obj \
$(OBJDIR)\_WinDestroyWindow.obj \
$(OBJDIR)\_WinDismissDlg.obj \
$(OBJDIR)\_WinDispatchMsg.obj \
$(OBJDIR)\_WinDlgBox.obj \
$(OBJDIR)\_WinDrawBitmap.obj \
$(OBJDIR)\_WinDrawBorder.obj \
$(OBJDIR)\_WinDrawPointer.obj \
$(OBJDIR)\_WinDrawText.obj \
$(OBJDIR)\_WinEmptyClipbrd.obj \
$(OBJDIR)\_WinEnablePhysInput.obj \
$(OBJDIR)\_WinEnableWindow.obj \
$(OBJDIR)\_WinEnableWindowUpdate.obj \
$(OBJDIR)\_WinEndEnumWindows.obj \
$(OBJDIR)\_WinEndPaint.obj \
$(OBJDIR)\_WinEnumClipbrdFmts.obj \
$(OBJDIR)\_WinEnumDlgItem.obj \
$(OBJDIR)\_WinEnumObjectClasses.obj \
$(OBJDIR)\_WinEqualRect.obj \
$(OBJDIR)\_WinExcludeUpdateRegion.obj \
$(OBJDIR)\_WinFileDlg.obj \
$(OBJDIR)\_WinFillRect.obj \
$(OBJDIR)\_WinFindAtom.obj \
$(OBJDIR)\_WinFlashWindow.obj \
$(OBJDIR)\_WinFocusChange.obj \
$(OBJDIR)\_WinFontDlg.obj \
$(OBJDIR)\_WinFreeErrorInfo.obj \
$(OBJDIR)\_WinFreeFileDlgList.obj \
$(OBJDIR)\_WinFreeFileIcon.obj \
$(OBJDIR)\_WinGetClipPS.obj \
$(OBJDIR)\_WinGetCurrentTime.obj \
$(OBJDIR)\_WinGetDlgMsg.obj \
$(OBJDIR)\_WinGetErrorInfo.obj \
$(OBJDIR)\_WinGetKeyState.obj \
$(OBJDIR)\_WinGetLastError.obj \
$(OBJDIR)\_WinGetMaxPosition.obj \
$(OBJDIR)\_WinGetMinPosition.obj \
$(OBJDIR)\_WinGetMsg.obj \
$(OBJDIR)\_WinGetNextWindow.obj \
$(OBJDIR)\_WinGetPhysKeyState.obj \
$(OBJDIR)\_WinGetPS.obj \
$(OBJDIR)\_WinGetScreenPS.obj \
$(OBJDIR)\_WinGetSysBitmap.obj \
$(OBJDIR)\_WinInflateRect.obj \
$(OBJDIR)\_WinInitialize.obj \
$(OBJDIR)\_WinInSendMsg.obj \
$(OBJDIR)\_WinIntersectRect.obj \
$(OBJDIR)\_WinInvalidateRect.obj \
$(OBJDIR)\_WinInvalidateRegion.obj \
$(OBJDIR)\_WinInvertRect.obj \
$(OBJDIR)\_WinIsChild.obj \
$(OBJDIR)\_WinIsPhysInputEnabled.obj \
$(OBJDIR)\_WinIsRectEmpty.obj \
$(OBJDIR)\_WinIsSOMDDReady.obj \
$(OBJDIR)\_WinIsThreadActive.obj \
$(OBJDIR)\_WinIsWindow.obj \
$(OBJDIR)\_WinIsWindowEnabled.obj \
$(OBJDIR)\_WinIsWindowShowing.obj \
$(OBJDIR)\_WinIsWindowVisible.obj \
$(OBJDIR)\_WinIsWPDServerReady.obj \
$(OBJDIR)\_WinLoadAccelTable.obj \
$(OBJDIR)\_WinLoadDlg.obj \
$(OBJDIR)\_WinLoadFileIcon.obj \
$(OBJDIR)\_WinLoadHelpTable.obj \
$(OBJDIR)\_WinLoadLibrary.obj \
$(OBJDIR)\_WinLoadMenu.obj \
$(OBJDIR)\_WinLoadMessage.obj \
$(OBJDIR)\_WinLoadPointer.obj \
$(OBJDIR)\_WinLoadProcedure.obj \
$(OBJDIR)\_WinLoadString.obj \
$(OBJDIR)\_WinLockInput.obj \
$(OBJDIR)\_WinLockPointerUpdate.obj \
$(OBJDIR)\_WinLockupSystem.obj \
$(OBJDIR)\_WinLockVisRegions.obj \
$(OBJDIR)\_WinLockWindowUpdate.obj \
$(OBJDIR)\_WinMakePoints.obj \
$(OBJDIR)\_WinMakeRect.obj \
$(OBJDIR)\_WinMapDlgPoints.obj \
$(OBJDIR)\_WinMapWindowPoints.obj \
$(OBJDIR)\_WinMessageBox.obj \
$(OBJDIR)\_WinMessageBox2.obj \
$(OBJDIR)\_WinMoveObject.obj \
$(OBJDIR)\_WinMultWindowFromIDs.obj \
$(OBJDIR)\_WinNextChar.obj \
$(OBJDIR)\_WinOffsetRect.obj \
$(OBJDIR)\_WinOpenClipbrd.obj \
$(OBJDIR)\_WinOpenObject.obj \
$(OBJDIR)\_WinOpenWindowDC.obj \
$(OBJDIR)\_WinPeekMsg.obj \
$(OBJDIR)\_WinPopupMenu.obj \
$(OBJDIR)\_WinPostMsg.obj \
$(OBJDIR)\_WinPostQueueMsg.obj \
$(OBJDIR)\_WinPrevChar.obj \
$(OBJDIR)\_WinProcessDlg.obj \
$(OBJDIR)\_WinPtInRect.obj \
$(OBJDIR)\_WinQueryAccelTable.obj \
$(OBJDIR)\_WinQueryActiveDesktopPathname.obj \
$(OBJDIR)\_WinQueryActiveWindow.obj \
$(OBJDIR)\_WinQueryAnchorBlock.obj \
$(OBJDIR)\_WinQueryAtomLength.obj \
$(OBJDIR)\_WinQueryAtomName.obj \
$(OBJDIR)\_WinQueryAtomUsage.obj \
$(OBJDIR)\_WinQueryCapture.obj \
$(OBJDIR)\_WinQueryClassInfo.obj \
$(OBJDIR)\_WinQueryClassName.obj \
$(OBJDIR)\_WinQueryClassThunkProc.obj \
$(OBJDIR)\_WinQueryClipbrdData.obj \
$(OBJDIR)\_WinQueryClipbrdFmtInfo.obj \
$(OBJDIR)\_WinQueryClipbrdOwner.obj \
$(OBJDIR)\_WinQueryClipbrdViewer.obj \
$(OBJDIR)\_WinQueryControlColors.obj \
$(OBJDIR)\_WinQueryCp.obj \
$(OBJDIR)\_WinQueryCpList.obj \
$(OBJDIR)\_WinQueryCursorInfo.obj \
$(OBJDIR)\_WinQueryDesktopBkgnd.obj \
$(OBJDIR)\_WinQueryDesktopWindow.obj \
$(OBJDIR)\_WinQueryDlgItemShort.obj \
$(OBJDIR)\_WinQueryDlgItemText.obj \
$(OBJDIR)\_WinQueryDlgItemTextLength.obj \
$(OBJDIR)\_WinQueryFocus.obj \
$(OBJDIR)\_WinQueryHelpInstance.obj \
$(OBJDIR)\_WinQueryMsgPos.obj \
$(OBJDIR)\_WinQueryMsgTime.obj \
$(OBJDIR)\_WinQueryObject.obj \
$(OBJDIR)\_WinQueryObjectPath.obj \
$(OBJDIR)\_WinQueryObjectWindow.obj \
$(OBJDIR)\_WinQueryPointer.obj \
$(OBJDIR)\_WinQueryPointerInfo.obj \
$(OBJDIR)\_WinQueryPointerPos.obj \
$(OBJDIR)\_WinQueryPresParam.obj \
$(OBJDIR)\_WinQueryQueueInfo.obj \
$(OBJDIR)\_WinQueryQueueStatus.obj \
$(OBJDIR)\_WinQuerySendMsg.obj \
$(OBJDIR)\_WinQuerySessionTitle.obj \
$(OBJDIR)\_WinQuerySwitchEntry.obj \
$(OBJDIR)\_WinQuerySwitchHandle.obj \
$(OBJDIR)\_WinQuerySwitchList.obj \
$(OBJDIR)\_WinQuerySysColor.obj \
$(OBJDIR)\_WinQuerySysModalWindow.obj \
$(OBJDIR)\_WinQuerySysPointer.obj \
$(OBJDIR)\_WinQuerySysPointerData.obj \
$(OBJDIR)\_WinQuerySystemAtomTable.obj \
$(OBJDIR)\_WinQuerySysValue.obj \
$(OBJDIR)\_WinQueryTaskSizePos.obj \
$(OBJDIR)\_WinQueryTaskTitle.obj \
$(OBJDIR)\_WinQueryUpdateRect.obj \
$(OBJDIR)\_WinQueryUpdateRegion.obj \
$(OBJDIR)\_WinQueryVersion.obj \
$(OBJDIR)\_WinQueryVisibleRegion.obj \
$(OBJDIR)\_WinQueryWindow.obj \
$(OBJDIR)\_WinQueryWindowDC.obj \
$(OBJDIR)\_WinQueryWindowModel.obj \
$(OBJDIR)\_WinQueryWindowPos.obj \
$(OBJDIR)\_WinQueryWindowProcess.obj \
$(OBJDIR)\_WinQueryWindowPtr.obj \
$(OBJDIR)\_WinQueryWindowRect.obj \
$(OBJDIR)\_WinQueryWindowText.obj \
$(OBJDIR)\_WinQueryWindowTextLength.obj \
$(OBJDIR)\_WinQueryWindowThunkProc.obj \
$(OBJDIR)\_WinQueryWindowULong.obj \
$(OBJDIR)\_WinQueryWindowUShort.obj \
$(OBJDIR)\_WinQueueFromID.obj \
$(OBJDIR)\_WinRealizePalette.obj \
$(OBJDIR)\_WinRegisterClass.obj \
$(OBJDIR)\_WinRegisterObjectClass.obj \
$(OBJDIR)\_WinRegisterUserDatatype.obj \
$(OBJDIR)\_WinRegisterUserMsg.obj \
$(OBJDIR)\_WinReleaseHook.obj \
$(OBJDIR)\_WinReleasePS.obj \
$(OBJDIR)\_WinRemovePresParam.obj \
$(OBJDIR)\_WinRemoveSwitchEntry.obj \
$(OBJDIR)\_WinReplaceObjectClass.obj \
$(OBJDIR)\_WinReplyMsg.obj \
$(OBJDIR)\_WinRequestMutexSem.obj \
$(OBJDIR)\_WinRestartSOMDD.obj \
$(OBJDIR)\_WinRestartWPDServer.obj \
$(OBJDIR)\_WinRestoreWindowPos.obj \
$(OBJDIR)\_WinSaveObject.obj \
$(OBJDIR)\_WinSaveWindowPos.obj \
$(OBJDIR)\_WinScrollWindow.obj \
$(OBJDIR)\_WinSendDlgItemMsg.obj \
$(OBJDIR)\_WinSendMsg.obj \
$(OBJDIR)\_WinSetAccelTable.obj \
$(OBJDIR)\_WinSetActiveWindow.obj \
$(OBJDIR)\_WinSetCapture.obj \
$(OBJDIR)\_WinSetClassMsgInterest.obj \
$(OBJDIR)\_WinSetClassThunkProc.obj \
$(OBJDIR)\_WinSetClipbrdData.obj \
$(OBJDIR)\_WinSetClipbrdOwner.obj \
$(OBJDIR)\_WinSetClipbrdViewer.obj \
$(OBJDIR)\_WinSetControlColors.obj \
$(OBJDIR)\_WinSetCp.obj \
$(OBJDIR)\_WinSetDesktopBkgnd.obj \
$(OBJDIR)\_WinSetDlgItemShort.obj \
$(OBJDIR)\_WinSetDlgItemText.obj \
$(OBJDIR)\_WinSetFileIcon.obj \
$(OBJDIR)\_WinSetFocus.obj \
$(OBJDIR)\_WinSetHook.obj \
$(OBJDIR)\_WinSetKeyboardStateTable.obj \
$(OBJDIR)\_WinSetMsgInterest.obj \
$(OBJDIR)\_WinSetMsgMode.obj \
$(OBJDIR)\_WinSetMultWindowPos.obj \
$(OBJDIR)\_WinSetObjectData.obj \
$(OBJDIR)\_WinSetOwner.obj \
$(OBJDIR)\_WinSetParent.obj \
$(OBJDIR)\_WinSetPointer.obj \
$(OBJDIR)\_WinSetPointerOwner.obj \
$(OBJDIR)\_WinSetPointerPos.obj \
$(OBJDIR)\_WinSetPresParam.obj \
$(OBJDIR)\_WinSetRect.obj \
$(OBJDIR)\_WinSetRectEmpty.obj \
$(OBJDIR)\_WinSetSynchroMode.obj \
$(OBJDIR)\_WinSetSysColors.obj \
$(OBJDIR)\_WinSetSysModalWindow.obj \
$(OBJDIR)\_WinSetSysPointerData.obj \
$(OBJDIR)\_WinSetSysValue.obj \
$(OBJDIR)\_WinSetVisibleRegionNotify.obj \
$(OBJDIR)\_WinSetWindowBits.obj \
$(OBJDIR)\_WinSetWindowPos.obj \
$(OBJDIR)\_WinSetWindowPtr.obj \
$(OBJDIR)\_WinSetWindowText.obj \
$(OBJDIR)\_WinSetWindowThunkProc.obj \
$(OBJDIR)\_WinSetWindowULong.obj \
$(OBJDIR)\_WinSetWindowUShort.obj \
$(OBJDIR)\_WinShowCursor.obj \
$(OBJDIR)\_WinShowPointer.obj \
$(OBJDIR)\_WinShowTrackRect.obj \
$(OBJDIR)\_WinShowWindow.obj \
$(OBJDIR)\_WinShutdownSystem.obj \
$(OBJDIR)\_WinStartApp.obj \
$(OBJDIR)\_WinStartTimer.obj \
$(OBJDIR)\_WinStopTimer.obj \
$(OBJDIR)\_WinStoreWindowPos.obj \
$(OBJDIR)\_WinSubclassWindow.obj \
$(OBJDIR)\_WinSubstituteStrings.obj \
$(OBJDIR)\_WinSubtractRect.obj \
$(OBJDIR)\_WinSwitchToProgram.obj \
$(OBJDIR)\_WinTerminate.obj \
$(OBJDIR)\_WinTerminateApp.obj \
$(OBJDIR)\_WinThreadAssocQueue.obj \
$(OBJDIR)\_WinTrackRect.obj \
$(OBJDIR)\_WinTranslateAccel.obj \
$(OBJDIR)\_WinUnionRect.obj \
$(OBJDIR)\_WinUnlockSystem.obj \
$(OBJDIR)\_WinUpdateWindow.obj \
$(OBJDIR)\_WinUpper.obj \
$(OBJDIR)\_WinUpperChar.obj \
$(OBJDIR)\_WinValidateRect.obj \
$(OBJDIR)\_WinValidateRegion.obj \
$(OBJDIR)\_WinWaitEventSem.obj \
$(OBJDIR)\_WinWaitMsg.obj \
$(OBJDIR)\_WinWaitMuxWaitSem.obj \
$(OBJDIR)\_WinWakeThread.obj \
$(OBJDIR)\_WinWindowFromDC.obj \
$(OBJDIR)\_WinWindowFromID.obj \
$(OBJDIR)\_WinWindowFromPoint.obj \
$(OBJDIR)\__OpenClipbrd.obj \
$(OBJDIR)\O32_GetStartupInfo.obj \
$(OBJDIR)\O32_WaitForInputIdle.obj \
$(OBJDIR)\O32_CreateBitmapFromPMHandle.obj \
$(OBJDIR)\O32_CreatePaletteFromPMHandle.obj \
$(OBJDIR)\O32_GetPMHandleFromGDIHandle.obj \
$(OBJDIR)\_WinSetErrorInfo.obj \
$(OBJDIR)\_DosVerifyPidTid.obj


TARGET  = libwrap


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
_DdfBeginList.asm
_DdfBitmap.asm
_DdfEndList.asm
_DdfHyperText.asm
_DdfInform.asm
_DdfInitialize.asm
_DdfListItem.asm
_DdfMetafile.asm
_DdfPara.asm
_DdfSetColor.asm
_DdfSetFont.asm
_DdfSetFontStyle.asm
_DdfSetFormat.asm
_DdfSetTextAlign.asm
_DdfText.asm
_DevCloseDC.asm
_DevEscape.asm
_DevOpenDC.asm
_DevPostDeviceModes.asm
_DevPostEscape.asm
_DevQueryCaps.asm
_DevQueryDeviceNames.asm
_DevQueryHardcopyCaps.asm
_DosAcknowledgeSignalException.asm
_DosAddMuxWaitSem.asm
_DosAliasMem.asm
_DosAllocMem.asm
_DosAllocSharedMem.asm
_DosAllocThreadLocalMemory.asm
_DosAsyncTimer.asm
_DosBeep.asm
_DosCallNPipe.asm
_DosCancelLockRequest.asm
_DosClose.asm
_DosCloseEventSem.asm
_DosCloseMutexSem.asm
_DosCloseMuxWaitSem.asm
_DosCloseQueue.asm
_DosCloseVDD.asm
_DosConnectNPipe.asm
_DosCopy.asm
_DosCreateDir.asm
_DosCreateEventSem.asm
_DosCreateMutexSem.asm
_DosCreateMuxWaitSem.asm
_DosCreateNPipe.asm
_DosCreatePipe.asm
_DosCreateQueue.asm
_DosCreateThread.asm
_DosDebug.asm
_DosDelete.asm
_DosDeleteDir.asm
_DosDeleteMuxWaitSem.asm
_DosDevConfig.asm
_DosDevIOCtl.asm
_DosDisConnectNPipe.asm
_DosDumpProcess.asm
_DosDupHandle.asm
_DosEditName.asm
_DosEnterCritSec.asm
_DosEnterMustComplete.asm
_DosEnumAttribute.asm
_DosError.asm
_DosExecPgm.asm
_DosExit.asm
_DosExitCritSec.asm
_DosExitList.asm
_DosExitMustComplete.asm
_DosFindClose.asm
_DosFindFirst.asm
_DosFindNext.asm
_DosForceDelete.asm
_DosForceSystemDump.asm
_DosFreeMem.asm
_DosFreeModule.asm
_DosFreeResource.asm
_DosFreeThreadLocalMemory.asm
_DosFSAttach.asm
_DosFSCtl.asm
_DosGetDateTime.asm
_DosGetInfoBlocks.asm
_DosGetMessage.asm
_DosGetNamedSharedMem.asm
_DosGetResource.asm
_DosGetSharedMem.asm
_DosGiveSharedMem.asm
_DosInsertMessage.asm
_DosKillProcess.asm
_DosKillThread.asm
_DosLoadModule.asm
_DosMapCase.asm
_DosMove.asm
_DosOpen.asm
_DosOpenEventSem.asm
_DosOpenMutexSem.asm
_DosOpenMuxWaitSem.asm
_DosOpenQueue.asm
_DosOpenVDD.asm
_DosPeekNPipe.asm
_DosPeekQueue.asm
_DosPhysicalDisk.asm
_DosPostEventSem.asm
_DosProtectClose.asm
_DosProtectEnumAttribute.asm
_DosProtectOpen.asm
_DosProtectQueryFHState.asm
_DosProtectQueryFileInfo.asm
_DosProtectRead.asm
_DosProtectSetFHState.asm
_DosProtectSetFileInfo.asm
_DosProtectSetFileLocks.asm
_DosProtectSetFilePtr.asm
_DosProtectSetFileSize.asm
_DosProtectWrite.asm
_DosPurgeQueue.asm
_DosPutMessage.asm
_DosQueryAppType.asm
_DosQueryCollate.asm
_DosQueryCp.asm
_DosQueryCtryInfo.asm
_DosQueryCurrentDir.asm
_DosQueryCurrentDisk.asm
_DosQueryDBCSEnv.asm
_DosQueryDOSProperty.asm
_DosQueryEventSem.asm
_DosQueryFHState.asm
_DosQueryFileInfo.asm
_DosQueryFSAttach.asm
_DosQueryFSInfo.asm
_DosQueryHType.asm
_DosQueryMem.asm
_DosQueryMessageCP.asm
_DosQueryModuleHandle.asm
_DosQueryModuleName.asm
_DosQueryMutexSem.asm
_DosQueryMuxWaitSem.asm
_DosQueryNPHState.asm
_DosQueryNPipeInfo.asm
_DosQueryNPipeSemState.asm
_DosQueryPathInfo.asm
_DosQueryProcAddr.asm
_DosQueryProcType.asm
_DosQueryQueue.asm
_DosQueryRASInfo.asm
_DosQueryResourceSize.asm
_DosQuerySysInfo.asm
_DosQueryThreadContext.asm
_DosQueryVerify.asm
_DosRaiseException.asm
_DosRead.asm
_DosReadQueue.asm
_DosReleaseMutexSem.asm
_DosRequestMutexSem.asm
_DosRequestVDD.asm
_DosResetBuffer.asm
_DosResetEventSem.asm
_DosResumeThread.asm
_DosScanEnv.asm
_DosSearchPath.asm
_DosSelectSession.asm
_DosSendSignalException.asm
_DosSetCurrentDir.asm
_DosSetDateTime.asm
_DosSetDefaultDisk.asm
_DosSetDOSProperty.asm
_DosSetExceptionHandler.asm
_DosSetFHState.asm
_DosSetFileInfo.asm
_DosSetFileLocks.asm
_DosSetFilePtr.asm
_DosSetFileSize.asm
_DosSetFSInfo.asm
_DosSetMaxFH.asm
_DosSetMem.asm
_DosSetNPHState.asm
_DosSetNPipeSem.asm
_DosSetPathInfo.asm
_DosSetPriority.asm
_DosSetProcessCp.asm
_DosSetRelMaxFH.asm
_DosSetSession.asm
_DosSetSignalExceptionFocus.asm
_DosSetVerify.asm
_DosShutdown.asm
_DosSleep.asm
_DosStartSession.asm
_DosStartTimer.asm
_DosStopSession.asm
_DosStopTimer.asm
_DosSubAllocMem.asm
_DosSubFreeMem.asm
_DosSubSetMem.asm
_DosSubUnsetMem.asm
_DosSuppressPopUps.asm
_DosSuspendThread.asm
_DosTmrQueryFreq.asm
_DosTmrQueryTime.asm
_DosTransactNPipe.asm
_DosUnsetExceptionHandler.asm
_DosUnwindException.asm
_DosWaitChild.asm
_DosWaitEventSem.asm
_DosWaitMuxWaitSem.asm
_DosWaitNPipe.asm
_DosWaitThread.asm
_DosWrite.asm
_DosWriteQueue.asm
_DrgAcceptDroppedFiles.asm
_DrgAccessDraginfo.asm
_DrgAddStrHandle.asm
_DrgAllocDraginfo.asm
_DrgAllocDragtransfer.asm
_DrgCancelLazyDrag.asm
_DrgDeleteDraginfoStrHandles.asm
_DrgDeleteStrHandle.asm
_DrgDrag.asm
_DrgDragFiles.asm
_DrgFreeDraginfo.asm
_DrgFreeDragtransfer.asm
_DrgGetPS.asm
_DrgLazyDrag.asm
_DrgLazyDrop.asm
_DrgPostTransferMsg.asm
_DrgPushDraginfo.asm
_DrgQueryDraginfoPtr.asm
_DrgQueryDraginfoPtrFromDragitem.asm
_DrgQueryDraginfoPtrFromHwnd.asm
_DrgQueryDragitem.asm
_DrgQueryDragitemCount.asm
_DrgQueryDragitemPtr.asm
_DrgQueryDragStatus.asm
_DrgQueryNativeRMF.asm
_DrgQueryNativeRMFLen.asm
_DrgQueryStrName.asm
_DrgQueryStrNameLen.asm
_DrgQueryTrueType.asm
_DrgQueryTrueTypeLen.asm
_DrgReallocDraginfo.asm
_DrgReleasePS.asm
_DrgSendTransferMsg.asm
_DrgSetDragImage.asm
_DrgSetDragitem.asm
_DrgSetDragPointer.asm
_DrgVerifyNativeRMF.asm
_DrgVerifyRMF.asm
_DrgVerifyTrueType.asm
_DrgVerifyType.asm
_DrgVerifyTypeSet.asm
_GpiAnimatePalette.asm
_GpiAssociate.asm
_GpiBeginArea.asm
_GpiBeginElement.asm
_GpiBeginInkPath.asm
_GpiBeginPath.asm
_GpiBitBlt.asm
_GpiBox.asm
_GpiCallSegmentMatrix.asm
_GpiCharString.asm
_GpiCharStringAt.asm
_GpiCharStringPos.asm
_GpiCharStringPosAt.asm
_GpiCloseFigure.asm
_GpiCloseSegment.asm
_GpiCombineRegion.asm
_GpiComment.asm
_GpiConvert.asm
_GpiConvertWithMatrix.asm
_GpiCopyMetaFile.asm
_GpiCorrelateChain.asm
_GpiCorrelateFrom.asm
_GpiCorrelateSegment.asm
_GpiCreateBitmap.asm
_GpiCreateEllipticRegion.asm
_GpiCreateLogColorTable.asm
_GpiCreateLogFont.asm
_GpiCreatePalette.asm
_GpiCreatePolygonRegion.asm
_GpiCreatePS.asm
_GpiCreateRegion.asm
_GpiCreateRoundRectRegion.asm
_GpiDeleteBitmap.asm
_GpiDeleteElement.asm
_GpiDeleteElementRange.asm
_GpiDeleteElementsBetweenLabels.asm
_GpiDeleteMetaFile.asm
_GpiDeletePalette.asm
_GpiDeleteSegment.asm
_GpiDeleteSegments.asm
_GpiDeleteSetId.asm
_GpiDestroyPS.asm
_GpiDestroyRegion.asm
_GpiDrawBits.asm
_GpiDrawChain.asm
_GpiDrawDynamics.asm
_GpiDrawFrom.asm
_GpiDrawSegment.asm
_GpiElement.asm
_GpiEndArea.asm
_GpiEndElement.asm
_GpiEndInkPath.asm
_GpiEndPath.asm
_GpiEqualRegion.asm
_GpiErase.asm
_GpiErrorSegmentData.asm
_GpiExcludeClipRectangle.asm
_GpiFillPath.asm
_GpiFloodFill.asm
_GpiFrameRegion.asm
_GpiFullArc.asm
_GpiGetData.asm
_GpiImage.asm
_GpiIntersectClipRectangle.asm
_GpiLabel.asm
_GpiLine.asm
_GpiLoadBitmap.asm
_GpiLoadFonts.asm
_GpiLoadMetaFile.asm
_GpiLoadPublicFonts.asm
_GpiMarker.asm
_GpiModifyPath.asm
_GpiMove.asm
_GpiOffsetClipRegion.asm
_GpiOffsetElementPointer.asm
_GpiOffsetRegion.asm
_GpiOpenSegment.asm
_GpiOutlinePath.asm
_GpiPaintRegion.asm
_GpiPartialArc.asm
_GpiPathToRegion.asm
_GpiPlayMetaFile.asm
_GpiPointArc.asm
_GpiPolyFillet.asm
_GpiPolyFilletSharp.asm
_GpiPolygons.asm
_GpiPolyLine.asm
_GpiPolyLineDisjoint.asm
_GpiPolyMarker.asm
_GpiPolySpline.asm
_GpiPop.asm
_GpiPtInRegion.asm
_GpiPtVisible.asm
_GpiPutData.asm
_GpiQueryArcParams.asm
_GpiQueryAttrMode.asm
_GpiQueryAttrs.asm
_GpiQueryBackColor.asm
_GpiQueryBackMix.asm
_GpiQueryBitmapBits.asm
_GpiQueryBitmapDimension.asm
_GpiQueryBitmapHandle.asm
_GpiQueryBitmapInfoHeader.asm
_GpiQueryBitmapParameters.asm
_GpiQueryBoundaryData.asm
_GpiQueryCharAngle.asm
_GpiQueryCharBox.asm
_GpiQueryCharBreakExtra.asm
_GpiQueryCharDirection.asm
_GpiQueryCharExtra.asm
_GpiQueryCharMode.asm
_GpiQueryCharSet.asm
_GpiQueryCharShear.asm
_GpiQueryCharStringPos.asm
_GpiQueryCharStringPosAt.asm
_GpiQueryClipBox.asm
_GpiQueryClipRegion.asm
_GpiQueryColor.asm
_GpiQueryColorData.asm
_GpiQueryColorIndex.asm
_GpiQueryCp.asm
_GpiQueryCurrentPosition.asm
_GpiQueryDefArcParams.asm
_GpiQueryDefAttrs.asm
_GpiQueryDefaultViewMatrix.asm
_GpiQueryDefCharBox.asm
_GpiQueryDefTag.asm
_GpiQueryDefViewingLimits.asm
_GpiQueryDevice.asm
_GpiQueryDeviceBitmapFormats.asm
_GpiQueryDrawControl.asm
_GpiQueryDrawingMode.asm
_GpiQueryEditMode.asm
_GpiQueryElement.asm
_GpiQueryElementPointer.asm
_GpiQueryElementType.asm
_GpiQueryFaceString.asm
_GpiQueryFontAction.asm
_GpiQueryFontFileDescriptions.asm
_GpiQueryFontMetrics.asm
_GpiQueryFonts.asm
_GpiQueryFullFontFileDescs.asm
_GpiQueryGraphicsField.asm
_GpiQueryInitialSegmentAttrs.asm
_GpiQueryKerningPairs.asm
_GpiQueryLineEnd.asm
_GpiQueryLineJoin.asm
_GpiQueryLineType.asm
_GpiQueryLineWidth.asm
_GpiQueryLineWidthGeom.asm
_GpiQueryLogColorTable.asm
_GpiQueryLogicalFont.asm
_GpiQueryMarker.asm
_GpiQueryMarkerBox.asm
_GpiQueryMarkerSet.asm
_GpiQueryMetaFileBits.asm
_GpiQueryMetaFileLength.asm
_GpiQueryMix.asm
_GpiQueryModelTransformMatrix.asm
_GpiQueryNearestColor.asm
_GpiQueryNumberSetIds.asm
_GpiQueryPageViewport.asm
_GpiQueryPalette.asm
_GpiQueryPaletteInfo.asm
_GpiQueryPattern.asm
_GpiQueryPatternRefPoint.asm
_GpiQueryPatternSet.asm
_GpiQueryPel.asm
_GpiQueryPickAperturePosition.asm
_GpiQueryPickApertureSize.asm
_GpiQueryPS.asm
_GpiQueryRealColors.asm
_GpiQueryRegionBox.asm
_GpiQueryRegionRects.asm
_GpiQueryRGBColor.asm
_GpiQuerySegmentAttrs.asm
_GpiQuerySegmentNames.asm
_GpiQuerySegmentPriority.asm
_GpiQuerySegmentTransformMatrix.asm
_GpiQuerySetIds.asm
_GpiQueryStopDraw.asm
_GpiQueryTag.asm
_GpiQueryTextAlignment.asm
_GpiQueryTextBox.asm
_GpiQueryViewingLimits.asm
_GpiQueryViewingTransformMatrix.asm
_GpiQueryWidthTable.asm
_GpiRectInRegion.asm
_GpiRectVisible.asm
_GpiRemoveDynamics.asm
_GpiResetBoundaryData.asm
_GpiResetPS.asm
_GpiRestorePS.asm
_GpiRotate.asm
_GpiSaveMetaFile.asm
_GpiSavePS.asm
_GpiScale.asm
_GpiSelectPalette.asm
_GpiSetArcParams.asm
_GpiSetAttrMode.asm
_GpiSetAttrs.asm
_GpiSetBackColor.asm
_GpiSetBackMix.asm
_GpiSetBitmap.asm
_GpiSetBitmapBits.asm
_GpiSetBitmapDimension.asm
_GpiSetBitmapId.asm
_GpiSetCharAngle.asm
_GpiSetCharBox.asm
_GpiSetCharBreakExtra.asm
_GpiSetCharDirection.asm
_GpiSetCharExtra.asm
_GpiSetCharMode.asm
_GpiSetCharSet.asm
_GpiSetCharShear.asm
_GpiSetClipPath.asm
_GpiSetClipRegion.asm
_GpiSetColor.asm
_GpiSetCp.asm
_GpiSetCurrentPosition.asm
_GpiSetDefArcParams.asm
_GpiSetDefAttrs.asm
_GpiSetDefaultViewMatrix.asm
_GpiSetDefTag.asm
_GpiSetDefViewingLimits.asm
_GpiSetDrawControl.asm
_GpiSetDrawingMode.asm
_GpiSetEditMode.asm
_GpiSetElementPointer.asm
_GpiSetElementPointerAtLabel.asm
_GpiSetGraphicsField.asm
_GpiSetInitialSegmentAttrs.asm
_GpiSetLineEnd.asm
_GpiSetLineJoin.asm
_GpiSetLineType.asm
_GpiSetLineWidth.asm
_GpiSetLineWidthGeom.asm
_GpiSetMarker.asm
_GpiSetMarkerBox.asm
_GpiSetMarkerSet.asm
_GpiSetMetaFileBits.asm
_GpiSetMix.asm
_GpiSetModelTransformMatrix.asm
_GpiSetPageViewport.asm
_GpiSetPaletteEntries.asm
_GpiSetPattern.asm
_GpiSetPatternRefPoint.asm
_GpiSetPatternSet.asm
_GpiSetPel.asm
_GpiSetPickAperturePosition.asm
_GpiSetPickApertureSize.asm
_GpiSetPS.asm
_GpiSetRegion.asm
_GpiSetSegmentAttrs.asm
_GpiSetSegmentPriority.asm
_GpiSetSegmentTransformMatrix.asm
_GpiSetStopDraw.asm
_GpiSetTag.asm
_GpiSetTextAlignment.asm
_GpiSetViewingLimits.asm
_GpiSetViewingTransformMatrix.asm
_GpiStrokeInkPath.asm
_GpiStrokePath.asm
_GpiTranslate.asm
_GpiUnloadFonts.asm
_GpiUnloadPublicFonts.asm
_GpiWCBitBlt.asm
_PrfAddProgram.asm
_PrfChangeProgram.asm
_PrfCloseProfile.asm
_PrfCreateGroup.asm
_PrfDestroyGroup.asm
_PrfOpenProfile.asm
_PrfQueryDefinition.asm
_PrfQueryProfile.asm
_PrfQueryProfileData.asm
_PrfQueryProfileInt.asm
_PrfQueryProfileSize.asm
_PrfQueryProfileString.asm
_PrfQueryProgramCategory.asm
_PrfQueryProgramHandle.asm
_PrfQueryProgramTitles.asm
_PrfRemoveProgram.asm
_PrfReset.asm
_PrfWriteProfileData.asm
_PrfWriteProfileString.asm
_RexxAddMacro.asm
_RexxClearMacroSpace.asm
_RexxDeregisterExit.asm
_RexxDeregisterFunction.asm
_RexxDeregisterSubcom.asm
_RexxDropMacro.asm
_RexxLoadMacroSpace.asm
_RexxQueryExit.asm
_RexxQueryFunction.asm
_RexxQueryMacro.asm
_RexxQuerySubcom.asm
_RexxRegisterExitDll.asm
_RexxRegisterExitExe.asm
_RexxRegisterFunctionDll.asm
_RexxRegisterFunctionExe.asm
_RexxRegisterSubcomDll.asm
_RexxRegisterSubcomExe.asm
_RexxReorderMacro.asm
_RexxResetTrace.asm
_RexxSaveMacroSpace.asm
_RexxSetHalt.asm
_RexxSetTrace.asm
_RexxStart.asm
_RexxVariablePool.asm
_SplControlDevice.asm
_SplCopyJob.asm
_SplCreateDevice.asm
_SplCreatePort.asm
_SplCreateQueue.asm
_SplDeleteDevice.asm
_SplDeleteJob.asm
_SplDeletePort.asm
_SplDeleteQueue.asm
_SplEnumDevice.asm
_SplEnumDriver.asm
_SplEnumJob.asm
_SplEnumPort.asm
_SplEnumPrinter.asm
_SplEnumQueue.asm
_SplEnumQueueProcessor.asm
_SplHoldJob.asm
_SplHoldQueue.asm
_SplMessageBox.asm
_SplPurgeQueue.asm
_SplQmAbort.asm
_SplQmAbortDoc.asm
_SplQmClose.asm
_SplQmEndDoc.asm
_SplQmGetJobID.asm
_SplQmNewPage.asm
_SplQmOpen.asm
_SplQmStartDoc.asm
_SplQmWrite.asm
_SplQueryDevice.asm
_SplQueryDriver.asm
_SplQueryJob.asm
_SplQueryPort.asm
_SplQueryQueue.asm
_SplReleaseJob.asm
_SplReleaseQueue.asm
_SplSetDevice.asm
_SplSetDriver.asm
_SplSetJob.asm
_SplSetPort.asm
_SplSetQueue.asm
_SplStdClose.asm
_SplStdDelete.asm
_SplStdGetBits.asm
_SplStdOpen.asm
_SplStdQueryLength.asm
_SplStdStart.asm
_SplStdStop.asm
_Win32AddClipbrdViewer.asm
_Win32QueryClipbrdViewerChain.asm
_Win32QueryOpenClipbrdWindow.asm
_Win32RemoveClipbrdViewer.asm
_WinAddAtom.asm
_WinAddSwitchEntry.asm
_WinAlarm.asm
_WinAssociateHelpInstance.asm
_WinBeginEnumWindows.asm
_WinBeginPaint.asm
_WinBroadcastMsg.asm
_WinCalcFrameRect.asm
_WinCallMsgFilter.asm
_WinCancelShutdown.asm
_WinChangeSwitchEntry.asm
_WinCheckInput.asm
_WinCloseClipbrd.asm
_WinCompareStrings.asm
_WinCopyAccelTable.asm
_WinCopyObject.asm
_WinCopyRect.asm
_WinCpTranslateChar.asm
_WinCpTranslateString.asm
_WinCreateAccelTable.asm
_WinCreateAtomTable.asm
_WinCreateCursor.asm
_WinCreateDlg.asm
_WinCreateFrameControls.asm
_WinCreateHelpInstance.asm
_WinCreateHelpTable.asm
_WinCreateMenu.asm
_WinCreateMsgQueue.asm
_WinCreateObject.asm
_WinCreatePointer.asm
_WinCreatePointerIndirect.asm
_WinCreateShadow.asm
_WinCreateStdWindow.asm
_WinCreateSwitchEntry.asm
_WinCreateWindow.asm
_WinDdeInitiate.asm
_WinDdePostMsg.asm
_WinDdeRespond.asm
_WinDefDlgProc.asm
_WinDefFileDlgProc.asm
_WinDefFontDlgProc.asm
_WinDefWindowProc.asm
_WinDeleteAtom.asm
_WinDeleteLibrary.asm
_WinDeleteProcedure.asm
_WinDeregisterObjectClass.asm
_WinDestroyAccelTable.asm
_WinDestroyAtomTable.asm
_WinDestroyCursor.asm
_WinDestroyHelpInstance.asm
_WinDestroyMsgQueue.asm
_WinDestroyObject.asm
_WinDestroyPointer.asm
_WinDestroyWindow.asm
_WinDismissDlg.asm
_WinDispatchMsg.asm
_WinDlgBox.asm
_WinDrawBitmap.asm
_WinDrawBorder.asm
_WinDrawPointer.asm
_WinDrawText.asm
_WinEmptyClipbrd.asm
_WinEnablePhysInput.asm
_WinEnableWindow.asm
_WinEnableWindowUpdate.asm
_WinEndEnumWindows.asm
_WinEndPaint.asm
_WinEnumClipbrdFmts.asm
_WinEnumDlgItem.asm
_WinEnumObjectClasses.asm
_WinEqualRect.asm
_WinExcludeUpdateRegion.asm
_WinFileDlg.asm
_WinFillRect.asm
_WinFindAtom.asm
_WinFlashWindow.asm
_WinFocusChange.asm
_WinFontDlg.asm
_WinFreeErrorInfo.asm
_WinFreeFileDlgList.asm
_WinFreeFileIcon.asm
_WinGetClipPS.asm
_WinGetCurrentTime.asm
_WinGetDlgMsg.asm
_WinGetErrorInfo.asm
_WinGetKeyState.asm
_WinGetLastError.asm
_WinGetMaxPosition.asm
_WinGetMinPosition.asm
_WinGetMsg.asm
_WinGetNextWindow.asm
_WinGetPhysKeyState.asm
_WinGetPS.asm
_WinGetScreenPS.asm
_WinGetSysBitmap.asm
_WinInflateRect.asm
_WinInitialize.asm
_WinInSendMsg.asm
_WinIntersectRect.asm
_WinInvalidateRect.asm
_WinInvalidateRegion.asm
_WinInvertRect.asm
_WinIsChild.asm
_WinIsPhysInputEnabled.asm
_WinIsRectEmpty.asm
_WinIsSOMDDReady.asm
_WinIsThreadActive.asm
_WinIsWindow.asm
_WinIsWindowEnabled.asm
_WinIsWindowShowing.asm
_WinIsWindowVisible.asm
_WinIsWPDServerReady.asm
_WinLoadAccelTable.asm
_WinLoadDlg.asm
_WinLoadFileIcon.asm
_WinLoadHelpTable.asm
_WinLoadLibrary.asm
_WinLoadMenu.asm
_WinLoadMessage.asm
_WinLoadPointer.asm
_WinLoadProcedure.asm
_WinLoadString.asm
_WinLockInput.asm
_WinLockPointerUpdate.asm
_WinLockupSystem.asm
_WinLockVisRegions.asm
_WinLockWindowUpdate.asm
_WinMakePoints.asm
_WinMakeRect.asm
_WinMapDlgPoints.asm
_WinMapWindowPoints.asm
_WinMessageBox.asm
_WinMessageBox2.asm
_WinMoveObject.asm
_WinMultWindowFromIDs.asm
_WinNextChar.asm
_WinOffsetRect.asm
_WinOpenClipbrd.asm
_WinOpenObject.asm
_WinOpenWindowDC.asm
_WinPeekMsg.asm
_WinPopupMenu.asm
_WinPostMsg.asm
_WinPostQueueMsg.asm
_WinPrevChar.asm
_WinProcessDlg.asm
_WinPtInRect.asm
_WinQueryAccelTable.asm
_WinQueryActiveDesktopPathname.asm
_WinQueryActiveWindow.asm
_WinQueryAnchorBlock.asm
_WinQueryAtomLength.asm
_WinQueryAtomName.asm
_WinQueryAtomUsage.asm
_WinQueryCapture.asm
_WinQueryClassInfo.asm
_WinQueryClassName.asm
_WinQueryClassThunkProc.asm
_WinQueryClipbrdData.asm
_WinQueryClipbrdFmtInfo.asm
_WinQueryClipbrdOwner.asm
_WinQueryClipbrdViewer.asm
_WinQueryControlColors.asm
_WinQueryCp.asm
_WinQueryCpList.asm
_WinQueryCursorInfo.asm
_WinQueryDesktopBkgnd.asm
_WinQueryDesktopWindow.asm
_WinQueryDlgItemShort.asm
_WinQueryDlgItemText.asm
_WinQueryDlgItemTextLength.asm
_WinQueryFocus.asm
_WinQueryHelpInstance.asm
_WinQueryMsgPos.asm
_WinQueryMsgTime.asm
_WinQueryObject.asm
_WinQueryObjectPath.asm
_WinQueryObjectWindow.asm
_WinQueryPointer.asm
_WinQueryPointerInfo.asm
_WinQueryPointerPos.asm
_WinQueryPresParam.asm
_WinQueryQueueInfo.asm
_WinQueryQueueStatus.asm
_WinQuerySendMsg.asm
_WinQuerySessionTitle.asm
_WinQuerySwitchEntry.asm
_WinQuerySwitchHandle.asm
_WinQuerySwitchList.asm
_WinQuerySysColor.asm
_WinQuerySysModalWindow.asm
_WinQuerySysPointer.asm
_WinQuerySysPointerData.asm
_WinQuerySystemAtomTable.asm
_WinQuerySysValue.asm
_WinQueryTaskSizePos.asm
_WinQueryTaskTitle.asm
_WinQueryUpdateRect.asm
_WinQueryUpdateRegion.asm
_WinQueryVersion.asm
_WinQueryVisibleRegion.asm
_WinQueryWindow.asm
_WinQueryWindowDC.asm
_WinQueryWindowModel.asm
_WinQueryWindowPos.asm
_WinQueryWindowProcess.asm
_WinQueryWindowPtr.asm
_WinQueryWindowRect.asm
_WinQueryWindowText.asm
_WinQueryWindowTextLength.asm
_WinQueryWindowThunkProc.asm
_WinQueryWindowULong.asm
_WinQueryWindowUShort.asm
_WinQueueFromID.asm
_WinRealizePalette.asm
_WinRegisterClass.asm
_WinRegisterObjectClass.asm
_WinRegisterUserDatatype.asm
_WinRegisterUserMsg.asm
_WinReleaseHook.asm
_WinReleasePS.asm
_WinRemovePresParam.asm
_WinRemoveSwitchEntry.asm
_WinReplaceObjectClass.asm
_WinReplyMsg.asm
_WinRequestMutexSem.asm
_WinRestartSOMDD.asm
_WinRestartWPDServer.asm
_WinRestoreWindowPos.asm
_WinSaveObject.asm
_WinSaveWindowPos.asm
_WinScrollWindow.asm
_WinSendDlgItemMsg.asm
_WinSendMsg.asm
_WinSetAccelTable.asm
_WinSetActiveWindow.asm
_WinSetCapture.asm
_WinSetClassMsgInterest.asm
_WinSetClassThunkProc.asm
_WinSetClipbrdData.asm
_WinSetClipbrdOwner.asm
_WinSetClipbrdViewer.asm
_WinSetControlColors.asm
_WinSetCp.asm
_WinSetDesktopBkgnd.asm
_WinSetDlgItemShort.asm
_WinSetDlgItemText.asm
_WinSetFileIcon.asm
_WinSetFocus.asm
_WinSetHook.asm
_WinSetKeyboardStateTable.asm
_WinSetMsgInterest.asm
_WinSetMsgMode.asm
_WinSetMultWindowPos.asm
_WinSetObjectData.asm
_WinSetOwner.asm
_WinSetParent.asm
_WinSetPointer.asm
_WinSetPointerOwner.asm
_WinSetPointerPos.asm
_WinSetPresParam.asm
_WinSetRect.asm
_WinSetRectEmpty.asm
_WinSetSynchroMode.asm
_WinSetSysColors.asm
_WinSetSysModalWindow.asm
_WinSetSysPointerData.asm
_WinSetSysValue.asm
_WinSetVisibleRegionNotify.asm
_WinSetWindowBits.asm
_WinSetWindowPos.asm
_WinSetWindowPtr.asm
_WinSetWindowText.asm
_WinSetWindowThunkProc.asm
_WinSetWindowULong.asm
_WinSetWindowUShort.asm
_WinShowCursor.asm
_WinShowPointer.asm
_WinShowTrackRect.asm
_WinShowWindow.asm
_WinShutdownSystem.asm
_WinStartApp.asm
_WinStartTimer.asm
_WinStopTimer.asm
_WinStoreWindowPos.asm
_WinSubclassWindow.asm
_WinSubstituteStrings.asm
_WinSubtractRect.asm
_WinSwitchToProgram.asm
_WinTerminate.asm
_WinTerminateApp.asm
_WinThreadAssocQueue.asm
_WinTrackRect.asm
_WinTranslateAccel.asm
_WinUnionRect.asm
_WinUnlockSystem.asm
_WinUpdateWindow.asm
_WinUpper.asm
_WinUpperChar.asm
_WinValidateRect.asm
_WinValidateRegion.asm
_WinWaitEventSem.asm
_WinWaitMsg.asm
_WinWaitMuxWaitSem.asm
_WinWakeThread.asm
_WinWindowFromDC.asm
_WinWindowFromID.asm
_WinWindowFromPoint.asm
__OpenClipbrd.asm
O32_GetStartupInfo.asm
O32_WaitForInputIdle.asm
O32_CreateBitmapFromPMHandle.asm
O32_CreatePaletteFromPMHandle.asm
O32_GetPMHandleFromGDIHandle.asm
_WinSetErrorInfo.asm
_DosVerifyPidTid.asm
<<KEEP	
