/* $Id: aspi.h,v 1.1 2002-06-08 11:42:01 sandervl Exp $ */

/* ASPI definitions used for both WNASPI16 and WNASPI32 */

#if !defined(ASPI_H)
#define ASPI_H

#include "pshpack1.h"

#define ASPI_POSTING(prb) (prb->SRB_Flags & 0x1)

#define HOST_TO_TARGET(prb) (((prb->SRB_Flags>>3) & 0x3) == 0x2)
#define TARGET_TO_HOST(prb) (((prb->SRB_Flags>>3) & 0x3) == 0x1)
#define NO_DATA_TRANSFERED(prb) (((prb->SRB_Flags>>3) & 0x3) == 0x3)

#define SRB_EVENT_NOTIFY 0x40 /* Enable ASPI event notification */

#define INQUIRY_VENDOR          8

#define MUSTEK_SCSI_AREA_AND_WINDOWS 0x04
#define MUSTEK_SCSI_READ_SCANNED_DATA 0x08
#define MUSTEK_SCSI_GET_IMAGE_STATUS 0x0f
#define MUSTEK_SCSI_ADF_AND_BACKTRACE 0x10
#define MUSTEK_SCSI_CCD_DISTANCE 0x11
#define MUSTEK_SCSI_START_STOP 0x1b

#define CMD_TEST_UNIT_READY 0x00
#define CMD_REQUEST_SENSE 0x03
#define CMD_INQUIRY 0x12

/* scanner commands - just for debug */
#define CMD_SCAN_GET_DATA_BUFFER_STATUS 0x34
#define CMD_SCAN_GET_WINDOW 0x25
#define CMD_SCAN_OBJECT_POSITION 0x31
#define CMD_SCAN_READ 0x28
#define CMD_SCAN_RELEASE_UNIT 0x17
#define CMD_SCAN_RESERVE_UNIT 0x16
#define CMD_SCAN_SCAN 0x1b
#define CMD_SCAN_SEND 0x2a
#define CMD_SCAN_CHANGE_DEFINITION 0x40

#define INQURIY_CMDLEN 6
#define INQURIY_REPLY_LEN 96
#define INQUIRY_VENDOR 8

#define SENSE_BUFFER(prb) (&prb->CDBByte[prb->SRB_CDBLen])


/* Just a container for seeing what devices are open */
struct ASPI_DEVICE_INFO {
    struct ASPI_DEVICE_INFO *   next;
    int                         fd;
    int                         hostId;
    int                         target;
    int                         lun;
};

typedef struct ASPI_DEVICE_INFO ASPI_DEVICE_INFO;
static ASPI_DEVICE_INFO *ASPI_open_devices = NULL;

#include "poppack.h"


#endif
