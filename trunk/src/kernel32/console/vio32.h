#if !defined (_KBD32_)
#define _KBD32_

#define INCL_DOS
#define INCL_PM
#define INCL_WIN
#define INCL_VIO
#define INCL_SUB
#define INCL_DOSERRORS
#include <os2.h>
#include "MonitorApi.h"

#pragma pack (1)
/** */
typedef struct _KBD32KEYINFO {
    BOOL   keyUp;
    ULONG  status;
    USHORT repeat;
    USHORT scancode;
    USHORT chr;
    USHORT vKey;
} KBD32KEYINFO;

/** */
typedef struct _VIO32MOUINFO {
    USHORT x;
    USHORT y;
    ULONG  buttonStatus;
    ULONG  status;
    ULONG  eventType;
} VIO32MOUINFO;

/** */
typedef struct _pmMsg{
    /** */
    MPARAM mp1;
    /** */
    MPARAM mp2;
} pmMsgType;

// any of the possible messages
typedef union _VIO32EVENTINFO {
    /** generic mp1 mp2 message */
    pmMsgType       pmMsg;
    /** */
    KBDKEYINFO      vioKbd;
    /** */
    MOUEVENTINFO    vioMou;
    /** */
    MonRecord       monKbd;
    /** hook intercepted data */
    CHRMSG          wmchar;
    /** */
//    MSEMSG          wmmouse;
    /** */
    KBD32KEYINFO    vio32Kbd;
    /** */
    VIO32MOUINFO    vio32Mou;
} VIO32EVENTINFO;

// the kbd + mouse status (as scan codes)
typedef struct _VIO32STATUSINFO {
    // 256 bits (scan codes) + 32 bit special scans: mouse...
    ULONG status[9];
} VIO32STATUSINFO;

/** to create hook-like plugins for vio32 */
typedef struct _vio32Filter {
    VOID *next;
    /** associated information: MUST be unique for each filter! NOT NULL!! */
    VOID *filterData;
    /** filtering function */
    BOOL (*filter)(ULONG *eventType,VIO32EVENTINFO *event,VOID *filterData);
} vio32Filter;

#pragma pack(4)

// macro to check if a key is pressed on a VIO32STATUSINFO : BOOL
#define KBD32_CHECKKEY(scanState,scanCode) ((scanState.status[scanCode >> 5] & (1 << (scanCode & 0x1F))) != 0)

// macro to mark a key as pressed on a VIO32STATUSINFO : VOID
#define KBD32_SETKEY(scanState,scanCode) (scanState.status[scanCode >> 5] |= (1 << (scanCode & 0x1F)))

// macro to mark a key as dePressed on a VIO32STATUSINFO : VOID
#define KBD32_UNSETKEY(scanState,scanCode) (scanState.status[scanCode >> 5] &= (~(1 << (scanCode & 0x1F))))

// macro to toggle a key status : VOID
#define KBD32_TOGGLEKEY(scanState,scanCode) (scanState.status[scanCode >> 5] ^= (1 << (scanCode & 0x1F)))

// macro to cleanup a scanState structure
#define KBD32_RESETKEYS(scanState) {    \
    scanState.status[0] = 0;            \
    scanState.status[1] = 0;            \
    scanState.status[2] = 0;            \
    scanState.status[3] = 0;            \
    scanState.status[4] = 0;            \
    scanState.status[5] = 0;            \
    scanState.status[6] = 0;            \
    scanState.status[7] = 0;            \
    scanState.status[8] = 0;            \
}


// the scan assigned to mouse button 1
#define KBD32_MB1_SCAN          0x101

// the scan assigned to mouse button 2
#define KBD32_MB2_SCAN          0x102

// the scan assigned to mouse button 3
#define KBD32_MB3_SCAN          0x103

// the scan assigned to mouse button 4
#define KBD32_MB4_SCAN          0x104

// the scan assigned to mouse button 5
#define KBD32_MB5_SCAN          0x105

// the scan assigned to the caps lock status
#define KBD32_CAPSLOCK_SCAN     0x106

// the scan assigned to the scroll lock status
#define KBD32_SCROLLLOCK_SCAN   0x107

// the scan assigned to the num lock status
#define KBD32_NUMLOCK_SCAN      0x108

// the scan assigned to the insert status
#define KBD32_INSERT_SCAN       0x109

//
#define KBD32_SHIFTL_SCAN       0x02A

//
#define KBD32_SHIFTR_SCAN       0x036

// the normal scan code produced by the api, as a status it is the left ALT
#define KBD32_ALTL_SCAN         0x038

// only valid as status: right ALT
#define KBD32_ALTR_SCAN         0x10A

// the normal scan code produced by the api, as a status it is the left ALT
#define KBD32_CTRLL_SCAN        0x01D

// only valid as status: right ALT
#define KBD32_CTRLR_SCAN        0x10B

// only valid as status: Mouse button 1
#define VIO32_MB1_SCAN          0x10C

// only valid as status: Mouse button 2
#define VIO32_MB2_SCAN          0x10D

// only valid as status: Mouse button 3
#define VIO32_MB3_SCAN          0x10E

// only valid as status: Mouse button 4
#define VIO32_MB4_SCAN          0x10F

// only valid as status: Mouse button 5
#define VIO32_MB5_SCAN          0x110

// a message coming from the KbdCharIn call
#define VIO_CHAR WM_USER

// a message coming from the monitor routine
#define MON_CHAR WM_USER+1

// a char message translated to the KBD32KEYINFO
#define KBD32_CHAR WM_USER+2

// a message coming from the mouse calls
#define VIO_MOU WM_USER+3

// the translated mouse messages
#define VIO32_MOU WM_USER+4

// CTRL-BREAK is disallowed to break the program
#define KBD32MODE_DISABLEBREAK 0x1

// enable mouse click events
#define KBD32MODE_ENABLEMOUSE  0x2

// enable mouse move events
#define KBD32MODE_ENABLEMMOVE  0x4

// compatible with win32
#define KBD32_SHIFT              0x00000010
#define KBD32_CTRL_L             0x00000008
#define KBD32_CTRL_R             0x00000004
#define KBD32_ALT_L              0x00000002
#define KBD32_ALT_R              0x00000001
#define KBD32_NUMLOCK            0x00000020
#define KBD32_CAPSLOCK           0x00000080
#define KBD32_SCROLLLOCK         0x00000040
#define KBD32_ENHANCED_KEY       0x00000100

// compatible with win32: mouse button status
#define VIO32_MB1                0x0001
#define VIO32_MB2                0x0002
#define VIO32_MB3                0x0004
#define VIO32_MB4                0x0008
#define VIO32_MB5                0x0010
// compatible with win32: mouse event types
#define VIO32_MOUSEMOVE          0x0001
#define VIO32_DOUBLECLICK        0x0002

// Filter where modes
// first of the preprocessing filters
#define VIO32_PRE_FIRST         0x000000000
// last of the preprocessing filters
#define VIO32_PRE_LAST          0x00000FFFF
// first of the postprocessing filters (activated on read)
#define VIO32_POST_FIRST        0x000010000
// last of the postprocessing filters  (activated on read)
#define VIO32_POST_LAST         0x00001FFFF



/** for client use */
extern "C" {
    /** */
    APIRET EXPENTRY vio32Open();
    /** */
    APIRET EXPENTRY vio32Close();
    /** */
    APIRET EXPENTRY kbd32GetKey(KBD32KEYINFO *key,ULONG msecTimeout);
    /** */
    APIRET EXPENTRY vio32GetMouse(VIO32MOUINFO *mou,ULONG msecTimeout);
    /** */
    APIRET EXPENTRY vio32GetEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG msecTimeout);
    /** */
    APIRET EXPENTRY vio32GetEventQSize(ULONG *size,ULONG msecTimeout);
    /** */
    APIRET EXPENTRY vio32WriteEvent(VIO32EVENTINFO *event, ULONG *eventType);
    /** inconsistency problems with the output filters To be Solved*/
    APIRET EXPENTRY vio32PeekEvent(VIO32EVENTINFO *event,ULONG *eventType,ULONG position);
    /** inconsistency problems with the output filters To be Solved*/
    APIRET EXPENTRY vio32FlushEventQ();
    /** */
    APIRET EXPENTRY kbd32SetMode(ULONG mode);
    /** */
    APIRET EXPENTRY kbd32GetMode(ULONG *mode);
    /** where is the position on the LIST returns the actual position 0x10000 ored indicates the output list */
    APIRET EXPENTRY kbd32AddFilter(vio32Filter *filter,ULONG *where);
    /** the filter is identified by its data field */
    APIRET EXPENTRY kbd32RemoveFilter(VOID *filterData);
    /** */
    APIRET EXPENTRY kbd32GetKeyStatus(VIO32STATUSINFO *status);

}







#endif
