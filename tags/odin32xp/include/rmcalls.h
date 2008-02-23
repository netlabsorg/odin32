/**************************************************************************
 *
 * SOURCE FILE NAME =  RMCALLS.H
 *
 * DESCRIPTIVE NAME =  RM function prototypes exported to drivers
 *
 *
 * Copyright : COPYRIGHT IBM CORPORATION, 1994, 1995
 *             LICENSED MATERIAL - PROGRAM PROPERTY OF IBM
 *             REFER TO COPYRIGHT INSTRUCTION FORM#G120-2083
 *             RESTRICTED MATERIALS OF IBM
 *             IBM CONFIDENTIAL
 *
 * VERSION = V1.01
 *
 * DATE
 *
 * DESCRIPTION :
 *
 * Purpose:
 *
 *
 *
 * FUNCTIONS  :
 *
 * NOTES
 *
 *
 * STRUCTURES
 *
 * EXTERNAL REFERENCES
 *
 *
 *
 * EXTERNAL FUNCTIONS
 *
 * CHANGE ACTIVITY =
 *   DATE      FLAG        APAR   CHANGE DESCRIPTION
 *   --------  ----------  -----  --------------------------------------
 *
 ****************************************************************************/


/****************************************************************************/
/*                                                                          */
/* Function Prototypes                                                      */
/*                                                                          */
/****************************************************************************/



#ifndef __RM_CALLS__
#define __RM_CALLS__

#include <rmbase.h>
#include <rmioctl.h>
#ifdef __32BIT__
#define CDECL _Optlink
#else
#ifndef APIRET
#define APIRET USHORT
#endif
#ifndef CDECL
#define CDECL _cdecl
#endif
#endif

#ifndef __32BIT__
typedef USHORT (_cdecl FAR *PFNRM)();
#endif

APIRET CDECL FAR RMCreateDriver(PDRIVERSTRUCT pDriverStruct,
                                 PHDRIVER phDriver);

APIRET CDECL FAR RMDestroyDriver(HDRIVER hDriver);

APIRET CDECL FAR RMCreateAdapter(HDRIVER hDriver,
                                  PHADAPTER phAdapter,
                                  PADAPTERSTRUCT pAdapterStruct,
                                  HDEVICE hDevice,
                                  PAHRESOURCE pahResource);

APIRET CDECL FAR RMDestroyAdapter(HDRIVER hDriver,
                                   HADAPTER hAdapter);


APIRET CDECL FAR RMCreateDevice(HDRIVER hDriver,
                                 PHDEVICE phDevice,
                                 PDEVICESTRUCT pDeviceStruct,
                                 HADAPTER hAdapter,
                                 PAHRESOURCE pahResource);

APIRET CDECL FAR RMDestroyDevice(HDRIVER hDriver,
                                   HDEVICE hDevice);

APIRET CDECL FAR RMAllocResource(HDRIVER hDriver,
                                  PHRESOURCE phResource,
                                  PRESOURCESTRUCT pResourceStruct);


APIRET CDECL FAR RMDeallocResource(HDRIVER hDriver,
                                    HRESOURCE hResource);

APIRET CDECL FAR RMClaimResources(HDRIVER hDriver,
                                   HADAPTER hAdapter);

APIRET CDECL FAR RMReleaseResources(HDRIVER hDriver,
                                    HADAPTER hAdapter);

APIRET CDECL FAR RMCreateLDev(HDRIVER hDriver,
                                PHLDEV  phLDev,
                                HDEVICE hDevice,
                                PLDEVSTRUCT pLDevStruct);

APIRET CDECL FAR RMDestroyLDev(HDRIVER hDriver,
                                 HLDEV   hLDev);

APIRET CDECL FAR RMCreateSysName(HDRIVER hDriver,
                                  PHSYSNAME phSysName,
                                  HLDEV hLDev,
                                  PSYSNAMESTRUCT pSysNameStruct);

APIRET CDECL FAR RMDestroySysName(HDRIVER hDriver,
                                   HSYSNAME   hLDev);

APIRET CDECL FAR RMADDToHDEVICE(PHDEVICE phDevice,
                                 USHORT ADDHandle,
                                 USHORT UnitHandle);

APIRET CDECL FAR RMKeyToHandleList(RMHANDLE    hStartNode,
                                    PSZ         SearchKey,
                                    PHANDLELIST pHandleList);

APIRET CDECL FAR RMHandleToType(RMHANDLE  hHandle,
                                 PUSHORT   pHandleType );

APIRET CDECL FAR RMHandleToParent(RMHANDLE   hHandle,
                                   PRMHANDLE  phParent );

APIRET CDECL FAR RMUpdateAdjunct(HDRIVER  hDriver,
                                  HDEVICE  hDevice,
                                  USHORT   AdjunctIndex,
                                  PADJUNCT pAdjunct   );

APIRET CDECL FAR RMAdjToHandleList(PADJUNCT       pAdj,
                                    HADAPTER       hStartNode,
                                    PADJHANDLELIST pAdjHndList     );

APIRET CDECL FAR RMHDevToHLDev( HDEVICE  hDevice,
                                 HLDEV    hStartLDev,
                                 PHLDEV   phLDev );

APIRET CDECL FAR RMResToHandleList(PRESOURCESTRUCT   pRes,
                                    PHANDLELIST       pHndList );

APIRET CDECL FAR RMActivateAdapter(HDRIVER hDriver,
                                    HADAPTER hAdapter);

APIRET CDECL FAR RMDeactivateAdapter(HDRIVER hDriver,
                                      HADAPTER hAdapter);

APIRET CDECL FAR RMCreateLinkDevice(HDRIVER  hDriver,
                                     PHDEVICE phLinkDevice,
                                     PDEVICESTRUCT pDeviceStruct,
                                     PAHDEVICES pahDevices);

APIRET CDECL FAR RMModifyResources(HDRIVER   hDriver,
                                    HADAPTER  hAdapter,
                                    USHORT    ModifyAction,
                                    HRESOURCE hResource);


APIRET CDECL FAR RMGetNodeInfo(RMHANDLE         RMHandle,
                                PRM_GETNODE_DATA pNodeInfo,
                                USHORT           BufferSize);


APIRET CDECL FAR RMParseScsiInquiry(PVOID   pInquiry,
                                     PSZ     pDescBuffer,
                                     USHORT  BufferSize);


APIRET CDECL FAR RMCreateDetected(HDRIVER hDriver,
                                  PHDETECTED phDetected,
                                  PDETECTEDSTRUCT pDetectedStruct,
                                  PAHRESOURCE pahResource);

APIRET CDECL FAR RMDestroyDetected(HDRIVER hDriver,
                                   HDETECTED hDetected);

APIRET CDECL FAR RMDevIDToHandleList(IDTYPE IDType,
                                     DEVID DeviceID,
                                     DEVID FunctionID,
                                     DEVID CompatibleID,
												 VENDID VendorID,
												 SERNUM SerialNumber,
                                     SEARCHIDFLAGS SearchFlags,
                                     HDETECTED hStartNode,
                                     PHANDLELIST pHndList);

APIRET CDECL FAR RMHandleToResourceHandleList(RMHANDLE hHandle,
                                              PHANDLELIST pHndList);

APIRET CDECL FAR RMModifyNodeFlags(HDRIVER hDriver,
                                   RMHANDLE hNode,
                                   USHORT Flags);

APIRET CDECL FAR RMConvertID(PULONG pId, PSZ String, USHORT Direction);

APIRET CDECL FAR RMGetCommandLine(PSZ ConfigFileSpec,
                                  PSZ ConfigParms,
                                  PUSHORT ConfigParmsLen,
                                  USHORT Index,
                                  USHORT DDtype);

APIRET CDECL FAR RMGetVersion(PUSHORT pMajorVersion,
                              PUSHORT pMinorVersion);

APIRET CDECL FAR RMSetSnoopLevel(USHORT SnoopLevel,
                                 USHORT SnoopFlags);

APIRET CDECL FAR RMSaveDetectedData(USHORT SaveFlags);

APIRET CDECL FAR RMDeleteDetectedData(USHORT DeleteFlags);

APIRET CDECL FAR RMGetSnoopLevel(PUSHORT SnoopLevel,
                                 USHORT  SnoopFlags);



#ifndef __32BIT__
/*******************************************************/
/* The alloc_text pragma allows steering of a function */
/* to a specified segment.                             */
/* To override the names of these segments place a     */
/* #define RMCode MY_SEGMENT_NAME                      */
/*******************************************************/


#pragma alloc_text(RMCode,RMCreateDriver)
#pragma alloc_text(RMCode,RMDestroyDriver)

#pragma alloc_text(RMCode,RMCreateAdapter)
#pragma alloc_text(RMCode,RMDestroyAdapter)

#pragma alloc_text(RMCode,RMCreateDevice)
#pragma alloc_text(RMCode,RMDestroyDevice)

#pragma alloc_text(RMCode,RMAllocResource)
#pragma alloc_text(RMCode,RMDeallocResource)

#pragma alloc_text(RMCode,RMClaimResources)
#pragma alloc_text(RMCode,RMReleaseResources)

#pragma alloc_text(RMCode,RMCreateLDev)
#pragma alloc_text(RMCode,RMDestroyLDev)

#pragma alloc_text(RMCode,RMCreateSysName)
#pragma alloc_text(RMCode,RMDestroySysName)

#pragma alloc_text(RMCode,RMADDToHDEVICE)
#pragma alloc_text(RMCode,RMActivateAdapter)
#pragma alloc_text(RMCode,RMDeactivateAdapter)

#pragma alloc_text(RMCode,RMKeyToHandleList)
#pragma alloc_text(RMCode,RMHandleToType)
#pragma alloc_text(RMCode,RMHandleToParent)

#pragma alloc_text(RMCode,RMCreateLinkDevice)
#pragma alloc_text(RMCode,RMModifyResources)
#pragma alloc_text(RMCode,RMParseScsiInquiry)

#pragma alloc_text(RMCode,RMUpdateAdjunct)
#pragma alloc_text(RMCode,RMAdjToHandleList)
#pragma alloc_text(RMCode,RMHDevToHLDev)
#pragma alloc_text(RMCode,RMResToHandleList)
#pragma alloc_text(RMCode,RMGetNodeInfo)

#pragma alloc_text(RMCode,RMCreateDetected)
#pragma alloc_text(RMCode,RMDestroyDetected)
#pragma alloc_text(RMCode,RMDevIDToHandleList)
#pragma alloc_text(RMCode,RMHandleToResourceHandleList)
#pragma alloc_text(RMCode,RMModifyNodeFlags)
#pragma alloc_text(RMCode,RMConvertID)
#pragma alloc_text(RMCode,RMGetCommandLine)
#pragma alloc_text(RMCode,RMGetVersion)

#pragma alloc_text(RMCode,RMSetSnoopLevel)
#pragma alloc_text(RMCode,RMSaveDetectedData)
#pragma alloc_text(RMCode,RMDeleteDetectedData)
#endif

/****************************************************************************/
/*                                                                          */
/* Resource Manager Return Codes                                            */
/*                                                                          */
/****************************************************************************/
#define RMRC_SUCCESS               0x0000
#define RMRC_NOTINITIALIZED        0x0001
#define RMRC_BAD_DRIVERHANDLE      0x0002
#define RMRC_BAD_ADAPTERHANDLE     0x0003
#define RMRC_BAD_DEVICEHANDLE      0x0004
#define RMRC_BAD_RESOURCEHANDLE    0x0005
#define RMRC_BAD_LDEVHANDLE        0x0006
#define RMRC_BAD_SYSNAMEHANDLE     0x0007
#define RMRC_BAD_DEVHELP           0x0008
#define RMRC_NULL_POINTER          0x0009
#define RMRC_NULL_STRINGS          0x000a
#define RMRC_BAD_VERSION           0x000b
#define RMRC_RES_ALREADY_CLAIMED   0x000c
#define RMRC_DEV_ALREADY_CLAIMED   0x000d
#define RMRC_INVALID_PARM_VALUE    0x000e
#define RMRC_OUT_OF_MEMORY         0x000f
#define RMRC_SEARCH_FAILED         0x0010
#define RMRC_BUFFER_TOO_SMALL      0x0011
#define RMRC_GENERAL_FAILURE       0x0012
#define RMRC_IRQ_ENTRY_ILLEGAL     0x0013
#define RMRC_NOT_IMPLEMENTED       0x0014
#define RMRC_NOT_INSTALLED         0x0015
#define RMRC_BAD_DETECTHANDLE      0x0016
#define RMRC_BAD_RMHANDLE          0x0017
#define RMRC_BAD_FLAGS             0x0018
#define RMRC_NO_DETECTED_DATA      0x0019


/*********************/
/*  RM Types         */
/*********************/

typedef struct {
   UCHAR      MajorVersion;
   UCHAR      MinorVersion;
   USHORT     NumRSMEntries;
} RSMVERSTRUCT, FAR *PRSMVERSTRUCT, NEAR *NPRSMVERSTRUCT;


#endif /* __RM_CALLS__ */
