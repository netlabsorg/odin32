//----------------------------------------------------------------------//
//									//
//	File: CDIOCTL.H							//
//									//
//	(c) 2001 S&T Systemtechnik GmbH					//
//									//
//	erzeugt:	03.03.01	R.Ihle				//
//									//
//----------------------------------------------------------------------//

#ifndef	__CDIOCTL_H
#define	__CDIOCTL_H


// CDROM IOCTLs

#ifndef	IOCTL_CDROMDISK
  #define IOCTL_CDROMDISK               0x80

  #define CDROMDISK_RESETDRIVE		0x40
  #define CDROMDISK_EJECTDISK		0x44
  #define CDROMDISK_LOCKUNLOCKDOOR	0x46
  #define CDROMDISK_SEEK		0x50
  #define CDROMDISK_DEVICESTATUS	0x60
  #define CDROMDISK_GETDRIVER		0x61
  #define CDROMDISK_GETSECTORSIZE	0x63
  #define CDROMDISK_GETHEADLOC		0x70
  #define CDROMDISK_READLONG		0x72
  #define CDROMDISK_GETVOLUMESIZE	0x78
  #define CDROMDISK_GETUPC		0x79
#endif

#ifndef CDROMDISK_EXECMD
  #define CDROMDISK_EXECMD		0x7a
#endif


// CDROM audio IOCTLs

#ifndef	IOCTL_CDROMAUDIO
  #define IOCTL_CDROMAUDIO		0x81

  #define CDROMAUDIO_SETCHANNELCTRL	0x40
  #define CDROMAUDIO_PLAYAUDIO		0x50
  #define CDROMAUDIO_STOPAUDIO		0x51
  #define CDROMAUDIO_RESUMEAUDIO	0x52
  #define CDROMAUDIO_GETCHANNEL		0x60
  #define CDROMAUDIO_GETAUDIODISK	0x61
  #define CDROMAUDIO_GETAUDIOTRACK	0x62
  #define CDROMAUDIO_GETSUBCHANNELQ	0x63
  #define CDROMAUDIO_GETAUDIOSTATUS	0x65
#endif


// CD-ROM$2 IOCTLs

#ifndef	IOCTL_CDROMDISK2
  #define IOCTL_CDROMDISK2		0x82

  #define CDROMDISK2_DRVINFO		0x60
#endif

#ifndef CDROMDISK2_FEATURES
  #define CDROMDISK2_FEATURES		0x63
#endif


#pragma	pack(1)



/*
 * CDROMDISK2_DRIVEINFO
 *
 * Parameter packet:	none
 * Data packet:		DrvInfo structure
 */

struct DrvInfo
{
  USHORT	usDrvCount;		// number of drive letters
  USHORT	usFirstDrvNo;		// first drive letter (0 = A, 1 = B ..)
};


/*
 * CDROMDISK2_FEATURES
 *
 * Parameter packet:	none
 * Data packet:		ULONG
 */

#define	FEATURE_USB_SUPPORT	0x00000001L
#define	FEATURE_CDRW_SUPPORT	0x00000002L
#define	FEATURE_EXECMD_SUPPORT	0x00000004L




/*
 * CDROMDISK_EXECMD
 *
 * Parameter packet:	ExecCmd structure
 * Data packet:		depends on the SCSI command. Length is defined
 *			by data_length field in the Parameter Packet.
 *
 * Returns:		02 - device not ready
 *			13 - unsupported parameter
 *			14 - device already in use
 */

struct ExecCmd
{
  ULONG		ID_code;		// 'CD01'
  USHORT	data_length;		// length of the Data Packet
  USHORT	cmd_length;		// length of the Command Buffer
  USHORT	flags;			// flags
  UCHAR		cmd_buffer[16];		// Command Buffer for SCSI command
};

// flags
#define EX_DIRECTION_IN		0x0001	// 0, if transfer data to device
					// 1, if transfer data from device
#define EX_PLAYING_CHK		0x0002	// 0, if don't check playing audio
					// 1, if device plays audio return error

#pragma	pack()


#endif


