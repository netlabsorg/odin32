/*
 * dev32 - header file for 32-bit part of the driver.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 */
#ifndef _dev32_h_
#define _dev32_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Wrapper for declaration from dev1632.h used in this file.
 */
#ifndef _dev1632_h_
    #define RP32INIT void
    #define PKRNLOBJTABLE void *
#endif

/*
 * Defines from reqpkt.h.
 */
#define MAX_DISKDD_CMD      29

#define STERR               0x8000      /* Bit 15 - Error                   */
#define STINTER             0x0400      /* Bit 10 - Interim character       */
#define STBUI               0x0200      /* Bit  9 - Busy                    */
#define STDON               0x0100      /* Bit  8 - Done                    */
#define STECODE             0x00FF      /* Error code                       */
#define WRECODE             0x0000

#define STATUS_DONE         0x0100
#define STATUS_ERR_UNKCMD   0x8003


#define RPF_Int13RP         0x01        /* Int 13 Request Packet            */
#define RPF_CallOutDone     0x02        /* Int 13 Callout completed         */
#define RPF_PktDiskIOTchd   0x04        /* Disk_IO has touched this packet  */
#define RPF_CHS_ADDRESSING  0x08        /* CHS Addressing used in RBA field */
#define RPF_Internal        0x10        /* Internal request packet command  */
#define RPF_TraceComplete   0x20        /* Trace completion flag            */


/*
 * Function prototypes.
 */
#ifndef __cplusplus
USHORT _loadds _Far32 _Pascal R0Init32(RP32INIT *pRpInit);
USHORT _loadds _Far32 _Pascal GetOTEs32(PKRNLOBJTABLE pOTEBuf);
USHORT _loadds _Far32 _Pascal VerifyProcTab32(void);
#endif

/*
 * Global variables
 */
extern ULONG TKSSBase32;

/*
 * Macros
 */
#define SSToDS(a)   ((PVOID)((ULONG)(a) + TKSSBase32))

#ifdef __cplusplus
}
#endif

#endif
