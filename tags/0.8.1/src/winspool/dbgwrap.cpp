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
#include <winspool.h>

#define DBGWRAP_MODULE "WINSPOOL"
#include <dbgwrap.h>


DEBUGWRAP4(AbortPrinter)
DEBUGWRAP12(AddFormA)
DEBUGWRAP12(AddFormW)
DEBUGWRAP20(AddJobA)
DEBUGWRAP20(AddJobW)
DEBUGWRAP12(AddMonitorA)
DEBUGWRAP12(AddPortA)
// AddPortExA									@113
// AddPortExW									@114
DEBUGWRAP12(AddPortW)
DEBUGWRAP16(AddPrintProcessorA)
DEBUGWRAP16(AddPrintProcessorW)
DEBUGWRAP12(AddPrintProvidorA)
DEBUGWRAP12(AddPrintProvidorW)
DEBUGWRAP12(AddPrinterA)
DEBUGWRAP4(AddPrinterConnectionA)
// AddPrinterConnectionUI							@122
DEBUGWRAP4(AddPrinterConnectionW)
DEBUGWRAP12(AddPrinterDriverA)
DEBUGWRAP12(AddPrinterDriverW)
DEBUGWRAP12(AddPrinterW)
DEBUGWRAP20(AdvancedDocumentPropertiesA)
// AdvancedSetupDialog								@129	
DEBUGWRAP4(ClosePrinter)
DEBUGWRAP12(ConfigurePortA)
DEBUGWRAP12(ConfigurePortW)
DEBUGWRAP8(ConnectToPrinterDlg)
// ConvertAnsiDevModeToUnicodeDevmode						@134
// ConvertUnicodeDevModeToAnsiDevmode						@135
// CreatePrinterIC								@136
// DEVICECAPABILITIES								@137
// DEVICEMODE									@138
DEBUGWRAP8(DeleteFormA)
DEBUGWRAP8(DeleteFormW)
DEBUGWRAP12(DeleteMonitorA)
DEBUGWRAP12(DeleteMonitorW)
DEBUGWRAP12(DeletePortA)
DEBUGWRAP12(DeletePortW)
DEBUGWRAP12(DeletePrintProcessorA)
DEBUGWRAP12(DeletePrintProcessorW)
DEBUGWRAP12(DeletePrintProvidorA)
DEBUGWRAP12(DeletePrintProvidorW)
DEBUGWRAP4(DeletePrinter)
DEBUGWRAP4(DeletePrinterConnectionA)
DEBUGWRAP4(DeletePrinterConnectionW)
DEBUGWRAP8(DeletePrinterDataA)
DEBUGWRAP8(DeletePrinterDataW)
DEBUGWRAP12(DeletePrinterDriverA)
DEBUGWRAP12(DeletePrinterDriverW)
// DeletePrinterIC								@156
// DevQueryPrint									@157
// DevQueryPrintEx								@158
DEBUGWRAP20(DeviceCapabilitiesA)
DEBUGWRAP20(DeviceCapabilitiesW)
// DeviceMode									@162
// DevicePropertySheets								@163
// DocumentEvent									@164
DEBUGWRAP24(DocumentPropertiesA)
DEBUGWRAP24(DocumentPropertiesW)
// DocumentPropertySheets							@167
// EXTDEVICEMODE									@168
DEBUGWRAP4(EndDocPrinter)
DEBUGWRAP4(EndPagePrinter)
DEBUGWRAP24(EnumFormsA)
DEBUGWRAP24(EnumFormsW)
DEBUGWRAP32(EnumJobsA)
DEBUGWRAP32(EnumJobsW)
DEBUGWRAP24(EnumMonitorsA)
DEBUGWRAP24(EnumMonitorsW)
DEBUGWRAP24(EnumPortsA)
DEBUGWRAP24(EnumPortsW)
DEBUGWRAP28(EnumPrintProcessorDatatypesA)
DEBUGWRAP28(EnumPrintProcessorDatatypesW)
DEBUGWRAP28(EnumPrintProcessorsA)
DEBUGWRAP28(EnumPrintProcessorsW)
DEBUGWRAP36(EnumPrinterDataA)
DEBUGWRAP36(EnumPrinterDataW)
DEBUGWRAP28(EnumPrinterDriversA)
DEBUGWRAP28(EnumPrintersA)
DEBUGWRAP28(EnumPrintersW)
// ExtDeviceMode									@189
DEBUGWRAP4(FindClosePrinterChangeNotification)
DEBUGWRAP16(FindFirstPrinterChangeNotification)
DEBUGWRAP16(FindNextPrinterChangeNotification)
DEBUGWRAP4(FreePrinterNotifyInfo)
DEBUGWRAP24(GetFormA)
DEBUGWRAP24(GetFormW)
DEBUGWRAP24(GetJobA)
DEBUGWRAP24(GetJobW)
DEBUGWRAP24(GetPrintProcessorDirectoryA)
DEBUGWRAP24(GetPrintProcessorDirectoryW)
DEBUGWRAP20(GetPrinterA)
DEBUGWRAP24(GetPrinterDataA)
DEBUGWRAP24(GetPrinterDataW)
DEBUGWRAP24(GetPrinterDriverA)
DEBUGWRAP24(GetPrinterDriverDirectoryA)
DEBUGWRAP24(GetPrinterDriverDirectoryW)
DEBUGWRAP24(GetPrinterDriverW)
DEBUGWRAP20(GetPrinterW)
// InitializeDll									@208
DEBUGWRAP12(OpenPrinterA)
DEBUGWRAP12(OpenPrinterW)
// PlayGdiScriptOnPrinterIC							@211
DEBUGWRAP24(PrinterMessageBoxA)
DEBUGWRAP24(PrinterMessageBoxW)
DEBUGWRAP8(PrinterProperties)
// QueryRemoteFonts								@215
// QuerySpoolMode								@216
DEBUGWRAP16(ReadPrinter)
DEBUGWRAP8(ResetPrinterA)
DEBUGWRAP8(ResetPrinterW)
DEBUGWRAP8(ScheduleJob)
// SetAllocFailCount								@221
DEBUGWRAP16(SetFormA)
DEBUGWRAP16(SetFormW)
DEBUGWRAP20(SetJobA)
DEBUGWRAP16(SetPortA)
DEBUGWRAP16(SetPortW)
DEBUGWRAP16(SetPrinterA)
DEBUGWRAP20(SetPrinterDataA)
DEBUGWRAP20(SetPrinterDataW)
DEBUGWRAP16(SetPrinterW)
// SpoolerDevQueryPrintW								@232
// SpoolerInit									@233
// SpoolerPrinterEvent								@234
// StartDocDlgA									@235
// StartDocDlgW									@236
DEBUGWRAP12(StartDocPrinterA)
DEBUGWRAP12(StartDocPrinterW)
DEBUGWRAP4(StartPagePrinter)
// WaitForPrinterChange								@240
DEBUGWRAP16(WritePrinter)
