//-----------------------------------------------------------------------------
// Freeware.  This file may be used freely to promote the ioctl90 mixer API.
//
//-----------------------------------------------------------------------------
// File: ioctl90.h                                      Date Created: 10/14/98
// Description: prototypes for cat 90 mixer IOCTLs      Last Update : 11/16/99
//-----------------------------------------------------------------------------
//
// Crystal Semiconductor defined category 0x90 OS/2 mixer IOCTLS.
// 
// STATUS: (Drivers known to implement this API)
//    Crystal Semiconductor ISA Mode 3 driver version 2.08.
//    Crystal Semiconductor PCI driver version 3.02.
//
//-----------------------------------------------------------------------------
//
// This header file contains application usable defines for calls to 
// Crystal Semiconductor device driver to manage hardware mixer.
// It is primarily used to adjust the listening level of
// mixer inputs (CD-ROM, Aux, Line, Phone...).  The API also allows
// adjustment to the record gain as an override to MMPM/2 stream 
// specific API.
//
// API is modeled off of AC97 mixer.  This does not mean that the device
// uses an AC97 mixer.  Being a relatively low-function device, the model
// should be usable on most hardware architectures.
//
// For information on AC97 hardware architecuture, consult data book
// for the CS4297 databook:
//    Crystal Clear(tm) SoundFusion(tm) Audio Codec '97 (CS4297)
// Available from http://www.cirrus.com/
//
//
// API DEFINITION
// --------------
//
// Architecture notes:
// - All volumes sent to the device driver are range 0..100.
// - Mute is a boolean stored in a ULONG (range 0..1)
// - All APIs effect the device on GLOBAL basis (not per-stream).
// - API does not control master volume or per-stream volume.
//   These are controlled by MMPM/2 MCI and AUDIODD IOCTLS (category 0x80).
// - API does permit override of MMPM/2 record gain.  The value sent is
//   global and will only be used by the device driver if the master
//   gain is set via this API.  Once set, the global value will be used
//   in preference to the MMPM/2 per-stream setting, until next reboot.
//
// IOCTL Category 0x90, functions:
// 40 - MonoInSet        60 - MonoInQuery
// 41 - PhoneSet         61 - PhoneQuery
// 42 - MicSet           62 - MicQuery
// 43 - LineSet          63 - LineQuery
// 44 - CDSet            64 - CDQuery
// 45 - VideoSet         65 - VideoQuery
// 46 - AuxSet           66 - AuxQuery
// 4C - ThreeDSet        6C - ThreeDQuery
// 4D - StreamVolSet     6D - StreamVolQuery
// 4E - RecordSrcSet     6E - RecordSrcQuery
// 4F - RecordGainSet    6F - RecordGainQuery
// 80 - ApiLevelQuery    
// 81 - GetApiMap
// 82 - CallbackReg
//
// Application passes a structure (ioctl data buffer) 
// which contains:
// ULONG Mute;     // UnMute==0, Mute==1
// ULONG ulVolL;   // Left volume in percent 0..100
// ULONG ulVolR;   // Right volume in percent 0..100
//
// With the exception of the record gain APIs, these APIs effect 
// what you hear (output bus connections).
//
//-----------------------------------------------------------------------------
//
// Calling code should query the device driver name by calling
// MMPM/2 MCI APIs to get the PDD name for the "WaveAudio" MCI device.
// Application should then issue DosOpen on the device followed
// by DosDevIOCTLs to perform operations.
//
// Here is sample code for DosOpen and DosDevIOCTL
//
// Driver name in call to the open function should include "\\DEV\\"
//    Example: hPdd = DevOpen ("\\DEV\\BSAUD1$"); or
//    Example: hPdd = DevOpen ("\\DEV\\CWCAUD1$");
//
// HFILE DevOpen (char *ddName)
// {
//    ULONG ulRC;
//    ULONG OpenFlags;
//    ULONG OpenMode;
//    ULONG ulFileSize      = 0;
//    ULONG ulFileAttribute = 0;
//    ULONG ulActionTaken   = 0;
//    HFILE hPdd            = NULL;
// 
//    OpenFlags = OPEN_ACTION_OPEN_IF_EXISTS;   // Do not create file
// 
//    OpenMode  = OPEN_ACCESS_READWRITE +       // Read/Write file
//                OPEN_SHARE_DENYNONE +         // Non-exclusive access
//                OPEN_FLAGS_FAIL_ON_ERROR;     // No system popups on errors
// 
//    ulRC = DosOpen (ddName,          // in
//                    &hPdd,           //    out (handle)
//                    &ulActionTaken,  //    out
//                    ulFileSize,      // in
//                    ulFileAttribute, // in
//                    OpenFlags,       // in
//                    OpenMode,        // in
//                    NULL);           // in
// 
//    printf ("DosOpen RC = %x\n", ulRC);
// 
//    if (ulRC != 0)
//       hPdd = NULL;
// 
//    return (hPdd);
// }
//
// ULONG SendIOCTL (HFILE hPdd, ULONG ulFunc, PMIXSTRUCT pMix)
// {
//    ULONG     ulRC;
//    ULONG     ulSizeOfStruct =  = sizeof (MixStruct);
// 
//    ulRC = DosDevIOCtl 
//       (hPdd,               // Device Handle
//        0x90,               // Category (user defined >= 0x80)
//        ulFunc,             // Function Use defines in this .H file
//        NULL,               // in      Address of parm data (not used)
//        0,                  // in      Max size of parm data structure
//        NULL,               // in out  Actual size of parm data structure
//        pMix,               // in      Address of command data
//        ulSizeOfStruct,     // in      Maximum size of command data
//        &ulSizeOfStruct);   // in out  Size of command data
// 
//    printf ("DosDevIOCtl ulRC = %d\n", ulRC);
// 
//    return (ulRC);
// }
//
// MODIFICATION HISTORY
// 14-Oct-98 Joe Nord   Create (Crystal Semiconductor, Inc).
// 05-Mar-99 Joe Nord   Added version query and API map query ioctls (80, 81)
//                      First release, Crystal PCI 3.02
// 13-Apr-99 Joe Nord   Added support for global record gain set and query
// 15-Jul-99 Joe Nord   Add callback semaphore support
// 10-Nov-99 Joe Nord   Added support for 3D sound effect set and query
// 15-Nov-99 Joe Nord   Added support for stream volume override
// 16-Nov-99 Joe Nord   Added support for message buffer return
//-----------------------------------------------------------------------------

// This prototype is used in the device driver only (common header)
//USHORT ioctlmixMain (USHORT Function, 
//                     USHORT SysFileNum,
//                     ULONG  ulpvData, 
//                     USHORT usDLength);

// This structure is passed to the device driver using DosDevIOCTL.
typedef struct
{
   ULONG Mute;                  // UnMute==0, Mute==1
   ULONG VolumeL;               // 0..100 percent
   ULONG VolumeR;               // 0..100 percent
} MIXSTRUCT, *PMIXSTRUCT;

typedef struct
{
   ULONG pBuffer;     // Application linear address to message buffer (in)
   ULONG ulSize;      // Size of buffer (in).  Count of chars (out).
   ULONG fClear;      // PDD should clear buffer after copy (in)
   ULONG fError;      // Message buffer includes error message (out)
   ULONG fNewInfo;    // Message buffer has new text since last read (out)
   ULONG ulCharsLost; // Messages lost - circular queue wrap around (out)
} MIXMSGBUF, *PMIXMSGBUF;


//
// Defines for use by applications
//
#define MONOINSET       0x40    // SET functions in the 0x40 range
#define PHONESET        0x41
#define MICSET          0x42
#define LINESET         0x43
#define CDSET           0x44
#define VIDEOSET        0x45
#define AUXSET          0x46
#define BASSTREBLESET   0x4B
#define THREEDSET       0x4C
#define STREAMVOLSET    0x4D
#define RECORDSRCSET    0x4E
#define RECORDGAINSET   0x4F

#define MONOINQUERY     0x60    // Query functions in the 0x60 range
#define PHONEQUERY      0x61
#define MICQUERY        0x62
#define LINEQUERY       0x63
#define CDQUERY         0x64
#define VIDEOQUERY      0x65
#define AUXQUERY        0x66
#define BASSTREBLEQUERY 0x6B
#define THREEDQUERY     0x6C
#define STREAMVOLQUERY  0x6D
#define RECORDSRCQUERY  0x6E
#define RECORDGAINQUERY 0x6F

#define APILEVELQUERY   0x80
#define GETAPIMAP       0x81    // Get 256 byte BOOL list of supported IOCTLs
#define CALLBACKREG     0x82    // Provide HEV for mixer change callbacks
#define MSGBUF          0x83    // Get PDD error log message buffer

// RECORDSRCSET
// Support mixer application override of record gain values.
// Only use the global values when a mixer app has instructed to do so.
// This allows override of settings from MMPM/2 applications.

#define I90SRC_BASE     0x00000010      // 0..F are reserved (MMPM/2 space)        
#define I90SRC_MIC      0x00000010
#define I90SRC_CD       0x00000020
#define I90SRC_VIDEO    0x00000040
#define I90SRC_AUX      0x00000080
#define I90SRC_LINE     0x00000100
#define I90SRC_RES5     0x00000200
#define I90SRC_RES6     0x00000400
#define I90SRC_PHONE    0x00000800


// In first release, the APILEVELQUERY function returns a ULONG, 0x01.  
// As significant changes are made to the ioctlmix API, the return value
// of this function will be incremented.

// GETAPIMAP
// To help an application know what APIs are supported by this device driver, 
// return an array of booleans (BYTES).  TRUE => Supported.
// The array must be declared as 256 bytes.  This is larger than necessary as 
// the first 64 entries, range 0x00..0x39, are guaranteed to be zero.
// The extra bytes allows the array to be indexed using the IOCTL function
// defines in this file.
//
// Notice that a FALSE value in an array position (for example "video set") 
// can be used to inform the mixer application that this device driver does
// not support a "video" connection to the audio device.
// Where a device driver is called for an input that it does not support, the
// DosDevIOCTL will fail with return code RPGENFAIL.

