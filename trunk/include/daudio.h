/* $Id: daudio.h,v 1.4 2001-04-27 17:34:51 sandervl Exp $ */
#ifndef __DAUDIO_H__
#define __DAUDIO_H__

#ifndef FAR
#define FAR
#endif

#define DAUDIO_IOCTL_CAT		0x91

#define DAUDIO_OPEN			0x40
#define DAUDIO_CLOSE			0x41
#define DAUDIO_QUERYFORMAT              0x42
#define DAUDIO_SETVOLUME		0x43
#define DAUDIO_GETVOLUME		0x44
#define DAUDIO_START			0x45
#define DAUDIO_STOP			0x46
#define DAUDIO_PAUSE			0x47
#define DAUDIO_RESUME			0x48
#define DAUDIO_GETPOS                   0x49
#define DAUDIO_ADDBUFFER		0x4A
#define DAUDIO_REGISTER_THREAD		0x4B
#define DAUDIO_DEREGISTER_THREAD	0x4C
#define DAUDIO_QUERYCAPS                0x4D
#define DAUDIO_QUERYVERSION             0x4E
#define DAUDIO_ADDLOOPINGBUFFER		0x4F

#define DAUDIO_VERSION                  0x00000002  //highword = major, lowword = minor version

typedef struct {
  union
    {
    struct
      {
      ULONG   VolumeL;		// left volume (0..100)
      ULONG   VolumeR;		// right volume (0..100)
      } Vol;
    struct 
      {
      ULONG   lpBuffer;
      ULONG   ulBufferLength;
      } Buffer;
    struct 
      {
      ULONG   lpBuffer;
      ULONG   ulBufferLength;
      } LoopingBuffer;
    struct 
      {
      ULONG   ulCurrentPos;
      } Pos;
    struct 
      {
      ULONG   hSemaphore;
      } Thread;
    struct 
      {
      ULONG   ulVersion;
      } Version;
    };
  
} DAUDIO_CMD, FAR *LPDAUDIO_CMD;

#define DAUDIOCAPS_PRIMARYMONO          0x00000001
#define DAUDIOCAPS_PRIMARYSTEREO        0x00000002
#define DAUDIOCAPS_PRIMARY8BIT          0x00000004
#define DAUDIOCAPS_PRIMARY16BIT         0x00000008
#define DAUDIOCAPS_CONTINUOUSRATE       0x00000010
#define DAUDIOCAPS_EMULDRIVER           0x00000020
#define DAUDIOCAPS_CERTIFIED            0x00000040
#define DAUDIOCAPS_SECONDARYMONO        0x00000100
#define DAUDIOCAPS_SECONDARYSTEREO      0x00000200
#define DAUDIOCAPS_SECONDARY8BIT        0x00000400
#define DAUDIOCAPS_SECONDARY16BIT       0x00000800

typedef struct _DAUDIOCAPS
{
    ULONG	dwSize;
    ULONG	dwFlags;
    ULONG	dwMinSecondarySampleRate;
    ULONG	dwMaxSecondarySampleRate;
    ULONG	dwPrimaryBuffers;
    ULONG	dwMaxHwMixingAllBuffers;
    ULONG	dwMaxHwMixingStaticBuffers;
    ULONG	dwMaxHwMixingStreamingBuffers;
    ULONG	dwFreeHwMixingAllBuffers;
    ULONG	dwFreeHwMixingStaticBuffers;
    ULONG	dwFreeHwMixingStreamingBuffers;
    ULONG	dwMaxHw3DAllBuffers;
    ULONG	dwMaxHw3DStaticBuffers;
    ULONG	dwMaxHw3DStreamingBuffers;
    ULONG	dwFreeHw3DAllBuffers;
    ULONG	dwFreeHw3DStaticBuffers;
    ULONG	dwFreeHw3DStreamingBuffers;
    ULONG	dwTotalHwMemBytes;
    ULONG	dwFreeHwMemBytes;
    ULONG	dwMaxContigFreeHwMemBytes;
    ULONG	dwUnlockTransferRateHwBuffers;
    ULONG	dwPlayCpuOverheadSwBuffers;
    ULONG	dwReserved1;
    ULONG	dwReserved2;
} DAUDIO_CAPS, FAR *LPDAUDIO_CAPS;

#define SBLIVECAPS_MINSAMPLERATE	5000
#define SBLIVECAPS_MAXSAMPLERATE	48000
#define SBLIVECAPS_MAXSTREAMS           32

#endif //__DAUDIO_H__
