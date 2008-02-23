# $Id: libwrap1.mak,v 1.1 2004-12-04 09:40:50 sao2l02 Exp $

#
# Odin32 API
#
#       libwrap1.lib makefile
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
$(OBJDIR)\_DosVerifyPidTid.obj \
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
$(OBJDIR)\_WinSetErrorInfo.obj \
$(OBJDIR)\__OpenClipbrd.obj


TARGET  = libwrap1


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
_DosVerifyPidTid.asm
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
_WinSetErrorInfo.asm
__OpenClipbrd.asm
<<KEEP	
