/* $Id: daudio.h,v 1.2 2001-03-24 15:42:20 sandervl Exp $ */
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

typedef struct {
  union
    {
    struct
      {
      ULONG   Volume;		// left & right volume (high/low word)
      } Vol;
    struct 
      {
      ULONG   lpBuffer;
      ULONG   ulBufferLength;
      } Buffer;
    struct 
      {
      ULONG   ulCurrentPos;
      } Pos;
    struct 
      {
      ULONG   hSemaphore;
      } Thread;
    };
  
} DAUDIO_CMD, FAR *LPDAUDIO_CMD;

#endif //__DAUDIO_H__
