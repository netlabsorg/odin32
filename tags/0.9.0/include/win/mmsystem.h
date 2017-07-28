/* 
 * MMSYSTEM - Multimedia Wine Extension ... :-)
 */

#ifndef __WINE_MMSYSTEM_H
#define __WINE_MMSYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif 
#include "windef.h"

typedef LPSTR		HPSTR;          /* a huge version of LPSTR */
typedef LPCSTR		HPCSTR;         /* a huge version of LPCSTR */
   
#include "pshpack1.h"
   
#define MAXWAVEDRIVERS	10
#define MAXMIDIDRIVERS	10
#define MAXAUXDRIVERS	10
#define MAXMCIDRIVERS	32
#define MAXMIXERDRIVERS	10
   
#define MAXPNAMELEN      32     /* max product name length (including NULL) */
#define MAXERRORLENGTH   128    /* max error text length (including NULL) */
#define MAX_JOYSTICKOEMVXDNAME	260
   
typedef WORD    VERSION;        /* major (high byte), minor (low byte) */

typedef UINT16	MMVERSION16;
typedef UINT	MMVERSION;
typedef UINT16	MCIDEVICEID16;
typedef UINT	MCIDEVICEID;
typedef	UINT16	MMRESULT16;
typedef	UINT	MMRESULT;
typedef DWORD   MCIERROR;

typedef struct {
    UINT16    wType;		/* indicates the contents of the union */
    union {
	DWORD ms;		/* milliseconds */
	DWORD sample;		/* samples */
	DWORD cb;		/* byte count */
	struct {		/* SMPTE */
	    BYTE hour;		/* hours */
	    BYTE min;		/* minutes */
	    BYTE sec;		/* seconds */
	    BYTE frame;		/* frames  */
	    BYTE fps;		/* frames per second */
	    BYTE dummy;		/* pad */
	} smpte;
	struct {		/* MIDI */
	    DWORD songptrpos;	/* song pointer position */
	} midi;
    } u;
} MMTIME16,  *LPMMTIME16;

typedef struct {
    UINT    wType;
    union {
	DWORD ms;
	DWORD sample;
	DWORD cb;
        DWORD ticks;
	struct {
	    BYTE hour;
	    BYTE min;
	    BYTE sec;
	    BYTE frame;
	    BYTE fps;
	    BYTE dummy;
	    BYTE pad[2];
	} smpte;
	struct {
	    DWORD songptrpos;
	} midi;
    } u;
} MMTIME,  *LPMMTIME;

#define TIME_MS         0x0001  /* time in milliseconds */
#define TIME_SAMPLES    0x0002  /* number of wave samples */
#define TIME_BYTES      0x0004  /* current byte offset */
#define TIME_SMPTE      0x0008  /* SMPTE time */
#define TIME_MIDI       0x0010  /* MIDI time */
#define TIME_TICKS	0x0020  /* MIDI ticks */

#define MM_JOY1MOVE         0x3A0           /* joystick */
#define MM_JOY2MOVE         0x3A1
#define MM_JOY1ZMOVE        0x3A2
#define MM_JOY2ZMOVE        0x3A3
#define MM_JOY1BUTTONDOWN   0x3B5
#define MM_JOY2BUTTONDOWN   0x3B6
#define MM_JOY1BUTTONUP     0x3B7
#define MM_JOY2BUTTONUP     0x3B8

#define MM_MCINOTIFY        0x3B9           /* MCI */

#define MM_WOM_OPEN         0x3BB           /* waveform output */
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#define MM_WIM_OPEN         0x3BE           /* waveform input */
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

#define MM_MIM_OPEN         0x3C1           /* MIDI input */
#define MM_MIM_CLOSE        0x3C2
#define MM_MIM_DATA         0x3C3
#define MM_MIM_LONGDATA     0x3C4
#define MM_MIM_ERROR        0x3C5
#define MM_MIM_LONGERROR    0x3C6

#define MM_MOM_OPEN         0x3C7           /* MIDI output */
#define MM_MOM_CLOSE        0x3C8
#define MM_MOM_DONE         0x3C9
#define MM_MOM_POSITIONCB   0x3CA

#define MM_MIM_MOREDATA     0x3CC

#define MM_MIXM_LINE_CHANGE     0x3D0
#define MM_MIXM_CONTROL_CHANGE  0x3D1

#define MMSYSERR_BASE          0
#define WAVERR_BASE            32
#define MIDIERR_BASE           64
#define TIMERR_BASE            96
#define JOYERR_BASE            160
#define MCIERR_BASE            256

#define MCI_STRING_OFFSET      512
#define MCI_VD_OFFSET          1024
#define MCI_CD_OFFSET          1088
#define MCI_WAVE_OFFSET        1152
#define MCI_SEQ_OFFSET         1216

#define MMSYSERR_NOERROR      	0                    /* no error */
#define MMSYSERR_ERROR        	(MMSYSERR_BASE + 1)  /* unspecified error */
#define MMSYSERR_BADDEVICEID  	(MMSYSERR_BASE + 2)  /* device ID out of range */
#define MMSYSERR_NOTENABLED   	(MMSYSERR_BASE + 3)  /* driver failed enable */
#define MMSYSERR_ALLOCATED    	(MMSYSERR_BASE + 4)  /* device already allocated */
#define MMSYSERR_INVALHANDLE  	(MMSYSERR_BASE + 5)  /* device handle is invalid */
#define MMSYSERR_NODRIVER     	(MMSYSERR_BASE + 6)  /* no device driver present */
#define MMSYSERR_NOMEM        	(MMSYSERR_BASE + 7)  /* memory allocation error */
#define MMSYSERR_NOTSUPPORTED 	(MMSYSERR_BASE + 8)  /* function isn't supported */
#define MMSYSERR_BADERRNUM    	(MMSYSERR_BASE + 9)  /* error value out of range */
#define MMSYSERR_INVALFLAG    	(MMSYSERR_BASE + 10) /* invalid flag passed */
#define MMSYSERR_INVALPARAM   	(MMSYSERR_BASE + 11) /* invalid parameter passed */
#define MMSYSERR_HANDLEBUSY     (MMSYSERR_BASE + 12) /* handle being used */
#define MMSYSERR_INVALIDALIAS   (MMSYSERR_BASE + 13) /* specified alias not found */
#define MMSYSERR_BADDB          (MMSYSERR_BASE + 14) /* bad registry database */
#define MMSYSERR_KEYNOTFOUND    (MMSYSERR_BASE + 15) /* registry key not found */
#define MMSYSERR_READERROR      (MMSYSERR_BASE + 16) /* registry read error */
#define MMSYSERR_WRITEERROR     (MMSYSERR_BASE + 17) /* registry write error */
#define MMSYSERR_DELETEERROR    (MMSYSERR_BASE + 18) /* registry delete error */
#define MMSYSERR_VALNOTFOUND    (MMSYSERR_BASE + 19) /* registry value not found */
#define MMSYSERR_NODRIVERCB     (MMSYSERR_BASE + 20) /* driver does not call DriverCallback */
#define MMSYSERR_LASTERROR      (MMSYSERR_BASE + 20) /* last error in range */

#define CALLBACK_TYPEMASK   	0x00070000l    	/* callback type mask */
#define CALLBACK_NULL       	0x00000000l    	/* no callback */
#define CALLBACK_WINDOW     	0x00010000l    	/* dwCallback is a HWND */
#define CALLBACK_TASK       	0x00020000l    	/* dwCallback is a HTASK */
#define CALLBACK_THREAD		(CALLBACK_TASK)	/* dwCallback is a thread ID */
#define CALLBACK_FUNCTION   	0x00030000l    	/* dwCallback is a FARPROC */
#define CALLBACK_EVENT		0x00050000l	/* dwCallback is an EVENT Handler */

typedef void (* CALLBACK LPDRVCALLBACK16) (HDRVR16 h, UINT16 uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef void (* CALLBACK LPDRVCALLBACK) (HDRVR h, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);

#define DRV_LOAD                0x0001
#define DRV_ENABLE              0x0002
#define DRV_OPEN                0x0003
#define DRV_CLOSE               0x0004
#define DRV_DISABLE             0x0005
#define DRV_FREE                0x0006
#define DRV_CONFIGURE           0x0007
#define DRV_QUERYCONFIGURE      0x0008
#define DRV_INSTALL             0x0009
#define DRV_REMOVE              0x000A
#define DRV_EXITSESSION         0x000B
#define DRV_EXITAPPLICATION     0x000C
#define DRV_POWER               0x000F

#define DRV_RESERVED            0x0800
#define DRV_USER                0x4000

#define DRVCNF_CANCEL           0x0000
#define DRVCNF_OK               0x0001
#define DRVCNF_RESTART 		0x0002

#define DRVEA_NORMALEXIT  	0x0001
#define DRVEA_ABNORMALEXIT 	0x0002

#define DRV_SUCCESS		0x0001
#define DRV_FAILURE		0x0000

#define GND_FIRSTINSTANCEONLY 	0x00000001

#define GND_FORWARD  		0x00000000
#define GND_REVERSE    		0x00000002

typedef struct {
    DWORD   			dwDCISize;
    LPCWSTR  			lpszDCISectionName;
    LPCWSTR  			lpszDCIAliasName;
} DRVCONFIGINFO, *LPDRVCONFIGINFO;


LRESULT WINAPI DefDriverProc(DWORD dwDriverIdentifier, HDRVR hdrvr,
			     UINT Msg, LPARAM lParam1, LPARAM lParam2);
HDRVR 	WINAPI OpenDriverA(LPCSTR szDriverName, LPCSTR szSectionName,
			   LPARAM lParam2);
HDRVR 	WINAPI OpenDriverW(LPCWSTR szDriverName, LPCWSTR szSectionName,
			   LPARAM lParam2);
#define OpenDriver WINELIB_NAME_AW(OpenDriver)
LRESULT WINAPI CloseDriver(HDRVR hDriver, LPARAM lParam1, LPARAM lParam2);
LRESULT WINAPI SendDriverMessage(HDRVR hDriver, UINT message,
				 LPARAM lParam1, LPARAM lParam2);
HMODULE WINAPI GetDriverModuleHandle(HDRVR hDriver);

BOOL WINAPI DriverCallback(DWORD dwCallBack, UINT uFlags, HDRVR hDev, 
			   UINT wMsg, DWORD dwUser, DWORD dwParam1, 
			   DWORD dwParam2);

DWORD	WINAPI GetDriverFlags(HDRVR hDriver);
#ifdef __WINE__
/* this call (GetDriverFlags) is not documented, nor the flags returned.
 * here are Wine only definitions
 */
#define WINE_GDF_EXIST	0x80000000
#define WINE_GDF_16BIT	0x10000000
#endif

#define MM_MICROSOFT            1       /* Microsoft Corp. */

#define MM_MIDI_MAPPER          1       /* MIDI Mapper */
#define MM_WAVE_MAPPER          2       /* Wave Mapper */

#define MM_SNDBLST_MIDIOUT      3       /* Sound Blaster MIDI output port */
#define MM_SNDBLST_MIDIIN       4       /* Sound Blaster MIDI input port  */
#define MM_SNDBLST_SYNTH        5       /* Sound Blaster internal synthesizer */
#define MM_SNDBLST_WAVEOUT      6       /* Sound Blaster waveform output */
#define MM_SNDBLST_WAVEIN       7       /* Sound Blaster waveform input */

#define MM_ADLIB                9       /* Ad Lib-compatible synthesizer */

#define MM_MPU401_MIDIOUT       10      /* MPU401-compatible MIDI output port */
#define MM_MPU401_MIDIIN        11      /* MPU401-compatible MIDI input port */

#define MM_PC_JOYSTICK          12      /* Joystick adapter */


UINT16		WINAPI	mmsystemGetVersion16(void);
UINT 		WINAPI 	mmsystemGetVersion(void);
BOOL            WINAPI  sndPlaySoundA(LPCSTR pszSound, UINT fuSound);
BOOL            WINAPI  sndPlaySoundW(LPCWSTR pszSound, UINT fuSound);
#define                 sndPlaySound WINELIB_NAME_AW(sndPlaySound)
BOOL 		WINAPI 	PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
BOOL 		WINAPI 	PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound);
#define 		PlaySound WINELIB_NAME_AW(PlaySound)

#define SND_SYNC            	0x0000  /* play synchronously (default) */
#define SND_ASYNC           	0x0001  /* play asynchronously */
#define SND_NODEFAULT       	0x0002  /* don't use default sound */
#define SND_MEMORY          	0x0004  /* lpszSoundName points to a memory file */
#define SND_LOOP            	0x0008  /* loop the sound until next sndPlaySound */
#define SND_NOSTOP          	0x0010  /* don't stop any currently playing sound */

#define SND_NOWAIT		0x00002000L /* don't wait if the driver is busy */
#define SND_ALIAS       	0x00010000L /* name is a registry alias */
#define SND_ALIAS_ID		0x00110000L /* alias is a predefined ID */
#define SND_FILENAME    	0x00020000L /* name is file name */
#define SND_RESOURCE    	0x00040004L /* name is resource name or atom */
#define SND_PURGE		0x00000040L /* purge all sounds */
#define SND_APPLICATION     	0x00000080L /* look for application specific association */

/* waveform audio error return values */
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    /* unsupported wave format */
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    /* still something playing */
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    /* header not prepared */
#define WAVERR_SYNC           (WAVERR_BASE + 3)    /* device is synchronous */
#define WAVERR_LASTERROR      (WAVERR_BASE + 3)    /* last error in range */

typedef LPDRVCALLBACK16 LPWAVECALLBACK16;
typedef LPDRVCALLBACK LPWAVECALLBACK;

#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA

#define WAVE_MAPPER     (-1)

#define  WAVE_FORMAT_QUERY     		0x0001
#define  WAVE_ALLOWSYNC        		0x0002
#define  WAVE_MAPPED               	0x0004
#define  WAVE_FORMAT_DIRECT        	0x0008
#define  WAVE_FORMAT_DIRECT_QUERY  	(WAVE_FORMAT_QUERY | WAVE_FORMAT_DIRECT)

typedef struct wavehdr_tag {
    LPSTR       lpData;		/* pointer to locked data buffer */
    DWORD       dwBufferLength;	/* length of data buffer */
    DWORD       dwBytesRecorded;/* used for input only */
    DWORD       dwUser;		/* for client's use */
    DWORD       dwFlags;	/* assorted flags (see defines) */
    DWORD       dwLoops;	/* loop control counter */

    struct wavehdr_tag *lpNext;	/* reserved for driver */
    DWORD       reserved;	/* reserved for driver */
} WAVEHDR, *LPWAVEHDR;

#define WHDR_DONE       0x00000001  /* done bit */
#define WHDR_PREPARED   0x00000002  /* set if this header has been prepared */
#define WHDR_BEGINLOOP  0x00000004  /* loop start block */
#define WHDR_ENDLOOP    0x00000008  /* loop end block */
#define WHDR_INQUEUE    0x00000010  /* reserved for driver */

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION16	vDriverVersion;		/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];	/* product name (0 terminated string) */
    DWORD	dwFormats;		/* formats supported */
    WORD	wChannels;		/* number of sources supported */
    DWORD	dwSupport;		/* functionality supported by driver */
} WAVEOUTCAPS16, *LPWAVEOUTCAPS16;

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION	vDriverVersion;		/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];	/* product name (0 terminated string) */
    DWORD	dwFormats;		/* formats supported */
    WORD	wChannels;		/* number of sources supported */
    WORD	wReserved1;		/* padding */
    DWORD	dwSupport;		/* functionality supported by driver */
} WAVEOUTCAPSA, *LPWAVEOUTCAPSA;

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION	vDriverVersion;		/* version of the driver */
    WCHAR	szPname[MAXPNAMELEN];	/* product name (0 terminated string) */
    DWORD	dwFormats;		/* formats supported */
    WORD	wChannels;		/* number of sources supported */
    WORD	wReserved1;		/* padding */
    DWORD	dwSupport;		/* functionality supported by driver */
} WAVEOUTCAPSW, *LPWAVEOUTCAPSW;
DECL_WINELIB_TYPE_AW(WAVEOUTCAPS)
DECL_WINELIB_TYPE_AW(LPWAVEOUTCAPS)

#define WAVECAPS_PITCH          0x0001   /* supports pitch control */
#define WAVECAPS_PLAYBACKRATE   0x0002   /* supports playback rate control */
#define WAVECAPS_VOLUME         0x0004   /* supports volume control */
#define WAVECAPS_LRVOLUME       0x0008   /* separate left-right volume control */
#define WAVECAPS_SYNC           0x0010	 /* driver is synchrounous and playing is blocking */
#define WAVECAPS_SAMPLEACCURATE 0x0020	 /* position is sample accurate */
#define WAVECAPS_DIRECTSOUND	0x0040   /* ? */

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION16	vDriverVersion;		/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];	/* product name (0 terminated string) */
    DWORD	dwFormats;		/* formats supported */
    WORD	wChannels;		/* number of channels supported */
} WAVEINCAPS16, *LPWAVEINCAPS16;

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION	vDriverVersion;		/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];	/* product name (0 terminated string) */
    DWORD	dwFormats;		/* formats supported */
    WORD	wChannels;		/* number of channels supported */
    WORD	wReserved1;
} WAVEINCAPSA, *LPWAVEINCAPSA;
typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION	vDriverVersion;		/* version of the driver */
    WCHAR	szPname[MAXPNAMELEN];	/* product name (0 terminated string) */
    DWORD	dwFormats;		/* formats supported */
    WORD	wChannels;		/* number of channels supported */
    WORD	wReserved1;
} WAVEINCAPSW, *LPWAVEINCAPSW;
DECL_WINELIB_TYPE_AW(WAVEINCAPS)
DECL_WINELIB_TYPE_AW(LPWAVEINCAPS)

#define WAVE_INVALIDFORMAT     0x00000000       /* invalid format */
#define WAVE_FORMAT_1M08       0x00000001       /* 11.025 kHz, Mono,   8-bit  */
#define WAVE_FORMAT_1S08       0x00000002       /* 11.025 kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_1M16       0x00000004       /* 11.025 kHz, Mono,   16-bit */
#define WAVE_FORMAT_1S16       0x00000008       /* 11.025 kHz, Stereo, 16-bit */
#define WAVE_FORMAT_2M08       0x00000010       /* 22.05  kHz, Mono,   8-bit  */
#define WAVE_FORMAT_2S08       0x00000020       /* 22.05  kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_2M16       0x00000040       /* 22.05  kHz, Mono,   16-bit */
#define WAVE_FORMAT_2S16       0x00000080       /* 22.05  kHz, Stereo, 16-bit */
#define WAVE_FORMAT_4M08       0x00000100       /* 44.1   kHz, Mono,   8-bit  */
#define WAVE_FORMAT_4S08       0x00000200       /* 44.1   kHz, Stereo, 8-bit  */
#define WAVE_FORMAT_4M16       0x00000400       /* 44.1   kHz, Mono,   16-bit */
#define WAVE_FORMAT_4S16       0x00000800       /* 44.1   kHz, Stereo, 16-bit */

/* General format structure common to all formats, same for Win16 and Win32 */
typedef struct {
    WORD	wFormatTag;	/* format type */
    WORD	nChannels;	/* number of channels */
    DWORD	nSamplesPerSec;	/* sample rate */
    DWORD	nAvgBytesPerSec;/* for buffer estimation */
    WORD	nBlockAlign; 	/* block size of data */
} WAVEFORMAT, *LPWAVEFORMAT;

#define WAVE_FORMAT_PCM     1

typedef struct {
    WAVEFORMAT	wf;
    WORD	wBitsPerSample;
} PCMWAVEFORMAT, *LPPCMWAVEFORMAT;

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
/* dito same for Win16 / Win32 */
typedef struct {
    WORD	wFormatTag;	/* format type */
    WORD	nChannels;	/* number of channels (i.e. mono, stereo...) */
    DWORD	nSamplesPerSec;	/* sample rate */
    DWORD	nAvgBytesPerSec;/* for buffer estimation */
    WORD	nBlockAlign;	/* block size of data */
    WORD	wBitsPerSample;	/* number of bits per sample of mono data */
    WORD	cbSize;		/* the count in bytes of the size of */
				/* extra information (after cbSize) */
} WAVEFORMATEX, *LPWAVEFORMATEX, *NPWAVEFORMATEX, *PWAVEFORMATEX;
#endif

UINT16 		WINAPI 	waveOutGetNumDevs16(void);
UINT 		WINAPI 	waveOutGetNumDevs(void);
UINT16 		WINAPI 	waveOutGetDevCaps16(UINT16,LPWAVEOUTCAPS16,UINT16);
UINT 		WINAPI 	waveOutGetDevCapsA(UINT,LPWAVEOUTCAPSA,UINT);
UINT 		WINAPI	waveOutGetDevCapsW(UINT,LPWAVEOUTCAPSW,UINT);
#define 		waveOutGetDevCaps WINELIB_NAME_AW(waveOutGetDevCaps)
UINT16 		WINAPI	waveOutGetVolume16(UINT16,DWORD*);
UINT 		WINAPI	waveOutGetVolume(HWAVEOUT,DWORD*);
UINT16 		WINAPI 	waveOutSetVolume16(UINT16,DWORD);
UINT 		WINAPI 	waveOutSetVolume(HWAVEOUT,DWORD);
UINT16 		WINAPI 	waveOutGetErrorText16(UINT16,LPSTR,UINT16);
UINT 		WINAPI 	waveOutGetErrorTextA(UINT,LPSTR,UINT);
UINT 		WINAPI 	waveOutGetErrorTextW(UINT,LPWSTR,UINT);
#define 	    	waveOutGetErrorText WINELIB_NAME_AW(waveOutGetErrorText)
UINT16 		WINAPI 	waveOutOpen16(HWAVEOUT16*,UINT16,const LPWAVEFORMATEX,DWORD,DWORD,DWORD);
UINT 		WINAPI 	waveOutOpen(HWAVEOUT*,UINT,const LPWAVEFORMATEX,DWORD,DWORD,DWORD);
UINT16 		WINAPI 	waveOutClose16(HWAVEOUT16);
UINT 		WINAPI 	waveOutClose(HWAVEOUT);
UINT16 		WINAPI 	waveOutPrepareHeader16(HWAVEOUT16,WAVEHDR*,UINT16);
UINT 		WINAPI 	waveOutPrepareHeader(HWAVEOUT,WAVEHDR*,UINT);
UINT16 		WINAPI 	waveOutUnprepareHeader16(HWAVEOUT16,WAVEHDR*,UINT16);
UINT 		WINAPI 	waveOutUnprepareHeader(HWAVEOUT,WAVEHDR*,UINT);
UINT16 		WINAPI 	waveOutWrite16(HWAVEOUT16,WAVEHDR*,UINT16);
UINT 		WINAPI 	waveOutWrite(HWAVEOUT,WAVEHDR*,UINT);
UINT16 		WINAPI 	waveOutPause16(HWAVEOUT16);
UINT 		WINAPI 	waveOutPause(HWAVEOUT);
UINT16 		WINAPI 	waveOutRestart16(HWAVEOUT16);
UINT 		WINAPI 	waveOutRestart(HWAVEOUT);
UINT16 		WINAPI 	waveOutReset16(HWAVEOUT16);
UINT 		WINAPI 	waveOutReset(HWAVEOUT);
UINT16 		WINAPI 	waveOutBreakLoop16(HWAVEOUT16);
UINT 		WINAPI 	waveOutBreakLoop(HWAVEOUT);
UINT16 		WINAPI 	waveOutGetPosition16(HWAVEOUT16,LPMMTIME16,UINT16);
UINT 		WINAPI 	waveOutGetPosition(HWAVEOUT,LPMMTIME,UINT);
UINT16 		WINAPI 	waveOutGetPitch16(HWAVEOUT16,DWORD*);
UINT 		WINAPI 	waveOutGetPitch(HWAVEOUT,DWORD*);
UINT16 		WINAPI 	waveOutSetPitch16(HWAVEOUT16,DWORD);
UINT 		WINAPI 	waveOutSetPitch(HWAVEOUT,DWORD);
UINT16 		WINAPI 	waveOutGetPlaybackRate16(HWAVEOUT16,DWORD*);
UINT 		WINAPI 	waveOutGetPlaybackRate(HWAVEOUT,DWORD*);
UINT16 		WINAPI 	waveOutSetPlaybackRate16(HWAVEOUT16,DWORD);
UINT 		WINAPI 	waveOutSetPlaybackRate(HWAVEOUT,DWORD);
UINT16 		WINAPI 	waveOutGetID16(HWAVEOUT16,UINT16*);
UINT 		WINAPI 	waveOutGetID(HWAVEOUT,UINT*);
UINT16 		WINAPI 	waveOutMessage16(HWAVEOUT16,UINT16,DWORD,DWORD);
UINT		WINAPI 	waveOutMessage(HWAVEOUT,UINT,DWORD,DWORD);

UINT16 		WINAPI 	waveInGetNumDevs16(void);
UINT 		WINAPI 	waveInGetNumDevs(void);
UINT16 		WINAPI 	waveInGetDevCaps16(UINT16,LPWAVEINCAPS16,UINT16);
UINT 		WINAPI 	waveInGetDevCapsA(UINT,LPWAVEINCAPSA,UINT);
UINT 		WINAPI 	waveInGetDevCapsW(UINT,LPWAVEINCAPSW,UINT);
#define 		waveInGetDevCaps WINELIB_NAME_AW(waveInGetDevCaps)
UINT16 		WINAPI 	waveInGetErrorText16(UINT16,LPSTR,UINT16);
UINT 		WINAPI 	waveInGetErrorTextA(UINT,LPSTR,UINT);
UINT 		WINAPI 	waveInGetErrorTextW(UINT,LPWSTR,UINT);
#define 		waveInGetErrorText WINELIB_NAME_AW(waveInGetErrorText)
UINT16 		WINAPI  waveInOpen16(HWAVEIN16*,UINT16,const LPWAVEFORMAT,DWORD,DWORD,DWORD);
UINT 		WINAPI  waveInOpen(HWAVEIN*,UINT,const LPWAVEFORMATEX,DWORD,DWORD,DWORD);
UINT16 		WINAPI  waveInClose16(HWAVEIN16);
UINT 		WINAPI  waveInClose(HWAVEIN);
UINT16 		WINAPI  waveInPrepareHeader16(HWAVEIN16,WAVEHDR*,UINT16);
UINT 		WINAPI  waveInPrepareHeader(HWAVEIN,WAVEHDR*,UINT);
UINT16 		WINAPI  waveInUnprepareHeader16(HWAVEIN16,WAVEHDR*,UINT16);
UINT 		WINAPI  waveInUnprepareHeader(HWAVEIN,WAVEHDR*,UINT);
UINT16 		WINAPI  waveInAddBuffer16(HWAVEIN16,WAVEHDR*,UINT16);
UINT 		WINAPI  waveInAddBuffer(HWAVEIN,WAVEHDR*,UINT);
UINT16 		WINAPI  waveInStart16(HWAVEIN16);
UINT 		WINAPI  waveInStart(HWAVEIN);
UINT16 		WINAPI  waveInStop16(HWAVEIN16);
UINT 		WINAPI  waveInStop(HWAVEIN);
UINT16 		WINAPI  waveInReset16(HWAVEIN16);
UINT 		WINAPI  waveInReset(HWAVEIN);
UINT16 		WINAPI  waveInGetPosition16(HWAVEIN16,LPMMTIME16,UINT16);
UINT 		WINAPI  waveInGetPosition(HWAVEIN,LPMMTIME,UINT);
UINT16 		WINAPI  waveInGetID16(HWAVEIN16,UINT16*);
UINT 		WINAPI  waveInGetID(HWAVEIN,UINT*);

UINT16 		WINAPI 	waveInMessage16(HWAVEIN16,UINT16,DWORD,DWORD);
UINT 		WINAPI 	waveInMessage(HWAVEIN,UINT,DWORD,DWORD);

#define MIDIERR_UNPREPARED    (MIDIERR_BASE + 0)   /* header not prepared */
#define MIDIERR_STILLPLAYING  (MIDIERR_BASE + 1)   /* still something playing */
#define MIDIERR_NOMAP         (MIDIERR_BASE + 2)   /* no current map */
#define MIDIERR_NOTREADY      (MIDIERR_BASE + 3)   /* hardware is still busy */
#define MIDIERR_NODEVICE      (MIDIERR_BASE + 4)   /* port no longer connected */
#define MIDIERR_INVALIDSETUP  (MIDIERR_BASE + 5)   /* invalid setup */
#define MIDIERR_LASTERROR     (MIDIERR_BASE + 5)   /* last error in range */

typedef LPDRVCALLBACK16 LPMIDICALLBACK16;
typedef LPDRVCALLBACK LPMIDICALLBACK;
#define MIDIPATCHSIZE   128
typedef WORD PATCHARRAY[MIDIPATCHSIZE];
typedef WORD *LPPATCHARRAY;
typedef WORD KEYARRAY[MIDIPATCHSIZE];
typedef WORD *LPKEYARRAY;

#define MIM_OPEN        MM_MIM_OPEN
#define MIM_CLOSE       MM_MIM_CLOSE
#define MIM_DATA        MM_MIM_DATA
#define MIM_LONGDATA    MM_MIM_LONGDATA
#define MIM_ERROR       MM_MIM_ERROR
#define MIM_LONGERROR   MM_MIM_LONGERROR
#define MIM_MOREDATA	MM_MIM_MOREDATA

#define MOM_OPEN        MM_MOM_OPEN
#define MOM_CLOSE       MM_MOM_CLOSE
#define MOM_DONE        MM_MOM_DONE
#define MOM_POSITIONCB	MM_MOM_POSITIONCB

/* device ID for MIDI mapper */

#define MIDIMAPPER     (-1)
#define MIDI_MAPPER    (-1)

/* Only on Win95 and up */
#define MIDI_IO_STATUS	0x00000020L

/* flags for wFlags parm of 
	midiOutCachePatches(), 
	midiOutCacheDrumPatches() */
#define MIDI_CACHE_ALL      1
#define MIDI_CACHE_BESTFIT  2
#define MIDI_CACHE_QUERY    3
#define MIDI_UNCACHE        4

typedef struct {
    WORD	wMid;		/* manufacturer ID */
    WORD	wPid;		/* product ID */
    MMVERSION16	vDriverVersion;	/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    WORD	wTechnology;	/* type of device */
    WORD	wVoices;	/* # of voices (internal synth only) */
    WORD	wNotes;		/* max # of notes (internal synth only) */
    WORD	wChannelMask;	/* channels used (internal synth only) */
    DWORD	dwSupport;	/* functionality supported by driver */
} MIDIOUTCAPS16, *LPMIDIOUTCAPS16;

typedef struct {
    WORD	wMid;		/* manufacturer ID */
    WORD	wPid;		/* product ID */
    MMVERSION	vDriverVersion;	/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    WORD	wTechnology;	/* type of device */
    WORD	wVoices;	/* # of voices (internal synth only) */
    WORD	wNotes;		/* max # of notes (internal synth only) */
    WORD	wChannelMask;	/* channels used (internal synth only) */
    DWORD	dwSupport;	/* functionality supported by driver */
} MIDIOUTCAPSA, *LPMIDIOUTCAPSA;

typedef struct {
    WORD	wMid;		/* manufacturer ID */
    WORD	wPid;		/* product ID */
    MMVERSION	vDriverVersion;	/* version of the driver */
    WCHAR	szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    WORD	wTechnology;	/* type of device */
    WORD	wVoices;	/* # of voices (internal synth only) */
    WORD	wNotes;		/* max # of notes (internal synth only) */
    WORD	wChannelMask;	/* channels used (internal synth only) */
    DWORD	dwSupport;	/* functionality supported by driver */
} MIDIOUTCAPSW, *LPMIDIOUTCAPSW;

DECL_WINELIB_TYPE_AW(MIDIOUTCAPS)
DECL_WINELIB_TYPE_AW(LPMIDIOUTCAPS)

#define MOD_MIDIPORT    1  /* output port */
#define MOD_SYNTH       2  /* generic internal synth */
#define MOD_SQSYNTH     3  /* square wave internal synth */
#define MOD_FMSYNTH     4  /* FM internal synth */
#define MOD_MAPPER      5  /* MIDI mapper */

#define MIDICAPS_VOLUME		0x0001  /* supports volume control */
#define MIDICAPS_LRVOLUME	0x0002  /* separate left-right volume control */
#define MIDICAPS_CACHE		0x0004
#define MIDICAPS_STREAM		0x0008  /* capable of supporting stream buffer */

typedef struct {
    WORD	wMid;		/* manufacturer ID */
    WORD	wPid;		/* product ID */
    MMVERSION16	vDriverVersion;	/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    DWORD	dwSupport;	/* included in win95 and higher */
} MIDIINCAPS16, *LPMIDIINCAPS16;

typedef struct {
    WORD	wMid;		/* manufacturer ID */
    WORD	wPid;		/* product ID */
    MMVERSION	vDriverVersion;	/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    DWORD	dwSupport;	/* included in win95 and higher */
} MIDIINCAPSA, *LPMIDIINCAPSA;

typedef struct {
    WORD	wMid;		/* manufacturer ID */
    WORD	wPid;		/* product ID */
    MMVERSION	vDriverVersion;	/* version of the driver */
    WCHAR	szPname[MAXPNAMELEN];/* product name (NULL terminated string) */
    DWORD	dwSupport;	/* included in win95 and higher */
} MIDIINCAPSW, *LPMIDIINCAPSW;

DECL_WINELIB_TYPE_AW(MIDIINCAPS)
DECL_WINELIB_TYPE_AW(LPMIDIINCAPS)

typedef struct {
    LPSTR	lpData;		/* pointer to locked data block */
    DWORD	dwBufferLength;	/* length of data in data block */
    DWORD	dwBytesRecorded;/* used for input only */
    DWORD	dwUser;		/* for client's use */
    DWORD	dwFlags;	/* assorted flags (see defines) */
    struct midihdr_tag *lpNext;	/* reserved for driver */
    DWORD	reserved;	/* reserved for driver */
} MIDIHDR16, *LPMIDIHDR16;

/* It seems that Win32 has a slightly different structure than Win 16.
 * sigh....
 */
typedef struct {
    LPSTR	lpData;		/* pointer to locked data block */
    DWORD	dwBufferLength;	/* length of data in data block */
    DWORD	dwBytesRecorded;/* used for input only */
    DWORD	dwUser;		/* for client's use */
    DWORD	dwFlags;	/* assorted flags (see defines) */
    struct midihdr_tag *lpNext;	/* reserved for driver */
    DWORD	reserved;	/* reserved for driver */
    DWORD	dwOffset;	/* offset of playback in case of 
				 * MIDISTRM buffer */
    DWORD	dwReserved[4];	/* reserved for driver */
} MIDIHDR, *LPMIDIHDR;

#define MHDR_DONE       0x00000001       /* done bit */
#define MHDR_PREPARED   0x00000002       /* set if header prepared */
#define MHDR_INQUEUE    0x00000004       /* reserved for driver */
#define MHDR_ISSTRM	0x00000008	 /* data is sent by Stream functions */

typedef struct {
    DWORD		cbStruct;
    DWORD		dwTempo;
} MIDIPROPTEMPO, *LPMIDIPROPTEMPO;

typedef struct {
    DWORD		cbStruct;
    DWORD		dwTimeDiv;
} MIDIPROPTIMEDIV, *LPMIDIPROPTIMEDIV;

#define MIDIPROP_GET		0x40000000
#define MIDIPROP_SET		0x80000000
#define MIDIPROP_TEMPO		0x00000002
#define MIDIPROP_TIMEDIV	0x00000001

typedef struct {  
    DWORD dwDeltaTime;	/* Time, in MIDI ticks, between the previous 
			 * event and the current event. */
    DWORD dwStreamID;	/* Reserved; must be zero. */ 
    DWORD dwEvent;  	/* event => see MEVT_XXX macros */
#ifdef __WIN32OS2__
    DWORD dwParms[1];	/* extra pmts to dwEvent if F_LONG is set */
#else
    DWORD dwParms[0];	/* extra pmts to dwEvent if F_LONG is set */
#endif
} MIDIEVENT, *LPMIDIEVENT;
          
#define MEVT_EVENTTYPE(x) ((BYTE) (((x)>>24)&0xFF))
#define MEVT_EVENTPARM(x) ((DWORD) ((x)&0x00FFFFFFL))

#define MEVT_F_CALLBACK 0x40000000l
#define	MEVT_F_LONG     0x80000000l
#define	MEVT_F_SHORT    0x00000000l
#define	MEVT_COMMENT	((BYTE)0x82)
#define	MEVT_LONGMSG	((BYTE)0x80)
#define	MEVT_NOP	((BYTE)0x02)
#define	MEVT_SHORTMSG	((BYTE)0x00)
#define	MEVT_TEMPO	((BYTE)0x01)
#define	MEVT_VERSION	((BYTE)0x84)

UINT16		WINAPI	midiOutGetNumDevs16(void);
UINT		WINAPI	midiOutGetNumDevs(void);
UINT16		WINAPI	midiOutGetDevCaps16(UINT16,LPMIDIOUTCAPS16,UINT16);
UINT		WINAPI	midiOutGetDevCapsA(UINT,LPMIDIOUTCAPSA,UINT);
UINT		WINAPI	midiOutGetDevCapsW(UINT,LPMIDIOUTCAPSW,UINT);
#define 		midiOutGetDevCaps WINELIB_NAME_AW(midiOutGetDevCaps)
UINT16		WINAPI	midiOutGetVolume16(UINT16,DWORD*);
UINT		WINAPI	midiOutGetVolume(HMIDIOUT,DWORD*);
UINT16		WINAPI	midiOutSetVolume16(UINT16,DWORD);
UINT		WINAPI	midiOutSetVolume(HMIDIOUT,DWORD);
UINT16		WINAPI	midiOutGetErrorText16(UINT16,LPSTR,UINT16);
UINT		WINAPI	midiOutGetErrorTextA(UINT,LPSTR,UINT);
UINT		WINAPI	midiOutGetErrorTextW(UINT,LPWSTR,UINT);
#define 		midiOutGetErrorText WINELIB_NAME_AW(midiOutGetErrorText)
UINT16		WINAPI	midiGetErrorText(UINT16,LPSTR,UINT16);
UINT16		WINAPI	midiOutOpen16(HMIDIOUT16*,UINT16,DWORD,DWORD,DWORD);
UINT		WINAPI	midiOutOpen(HMIDIOUT*,UINT,DWORD,DWORD,DWORD);
UINT16		WINAPI	midiOutClose16(HMIDIOUT16);
UINT		WINAPI	midiOutClose(HMIDIOUT);
UINT16		WINAPI	midiOutPrepareHeader16(HMIDIOUT16,MIDIHDR16*,UINT16);
UINT		WINAPI	midiOutPrepareHeader(HMIDIOUT,MIDIHDR*,UINT);
UINT16		WINAPI	midiOutUnprepareHeader16(HMIDIOUT16,MIDIHDR16*,UINT16);
UINT		WINAPI	midiOutUnprepareHeader(HMIDIOUT,MIDIHDR*,UINT);
UINT16		WINAPI	midiOutShortMsg16(HMIDIOUT16,DWORD);
UINT		WINAPI	midiOutShortMsg(HMIDIOUT,DWORD);
UINT16		WINAPI	midiOutLongMsg16(HMIDIOUT16,MIDIHDR16*,UINT16);
UINT		WINAPI	midiOutLongMsg(HMIDIOUT,MIDIHDR*,UINT);
UINT16		WINAPI	midiOutReset16(HMIDIOUT16);
UINT		WINAPI	midiOutReset(HMIDIOUT);
UINT16		WINAPI	midiOutCachePatches16(HMIDIOUT16,UINT16,WORD*,UINT16);
UINT		WINAPI	midiOutCachePatches(HMIDIOUT,UINT,WORD*,UINT);
UINT16		WINAPI	midiOutCacheDrumPatches16(HMIDIOUT16,UINT16,WORD*,UINT16);
UINT		WINAPI	midiOutCacheDrumPatches(HMIDIOUT,UINT,WORD*,UINT);
UINT16		WINAPI	midiOutGetID16(HMIDIOUT16,UINT16*);
UINT		WINAPI	midiOutGetID(HMIDIOUT,UINT*);

UINT16		WINAPI	midiOutMessage16(HMIDIOUT16,UINT16,DWORD,DWORD);
UINT		WINAPI	midiOutMessage(HMIDIOUT,UINT,DWORD,DWORD);

UINT16		WINAPI	midiInGetNumDevs16(void);
UINT		WINAPI	midiInGetNumDevs(void);
UINT16		WINAPI	midiInGetDevCaps16(UINT16,LPMIDIINCAPS16,UINT16);
UINT		WINAPI	midiInGetDevCapsA(UINT,LPMIDIINCAPSA,UINT);
UINT		WINAPI	midiInGetDevCapsW(UINT,LPMIDIINCAPSW,UINT);
#define 		midiInGetDevCaps WINELIB_NAME_AW(midiInGetDevCaps)
UINT16		WINAPI	midiInGetErrorText16(UINT16,LPSTR,UINT16);
UINT		WINAPI	midiInGetErrorTextA(UINT,LPSTR,UINT);
UINT		WINAPI	midiInGetErrorTextW(UINT,LPWSTR,UINT);
#define 		midiInGetErrorText WINELIB_NAME_AW(midiInGetErrorText)
UINT16		WINAPI	midiInOpen16(HMIDIIN16*,UINT16,DWORD,DWORD,DWORD);
UINT		WINAPI	midiInOpen(HMIDIIN*,UINT,DWORD,DWORD,DWORD);
UINT16		WINAPI	midiInClose16(HMIDIIN16);
UINT		WINAPI	midiInClose(HMIDIIN);
UINT16		WINAPI	midiInPrepareHeader16(HMIDIIN16,MIDIHDR16*,UINT16);
UINT		WINAPI	midiInPrepareHeader(HMIDIIN,MIDIHDR*,UINT);
UINT16		WINAPI	midiInUnprepareHeader16(HMIDIIN16,MIDIHDR16*,UINT16);
UINT		WINAPI	midiInUnprepareHeader(HMIDIIN,MIDIHDR*,UINT);
UINT16		WINAPI	midiInAddBuffer16(HMIDIIN16,MIDIHDR16*,UINT16);
UINT		WINAPI	midiInAddBuffer(HMIDIIN,MIDIHDR*,UINT);
UINT16		WINAPI	midiInStart16(HMIDIIN16);
UINT		WINAPI	midiInStart(HMIDIIN);
UINT16		WINAPI	midiInStop16(HMIDIIN16);
UINT		WINAPI	midiInStop(HMIDIIN);
UINT16		WINAPI	midiInReset16(HMIDIIN16);
UINT		WINAPI	midiInReset(HMIDIIN);
UINT16		WINAPI	midiInGetID16(HMIDIIN16,UINT16*);
UINT		WINAPI	midiInGetID(HMIDIIN,UINT*);
UINT16		WINAPI	midiInMessage16(HMIDIIN16,UINT16,DWORD,DWORD);
UINT		WINAPI	midiInMessage(HMIDIIN,UINT,DWORD,DWORD);

MMRESULT16	WINAPI	midiStreamClose16(HMIDISTRM16 hms);
MMRESULT	WINAPI	midiStreamClose(HMIDISTRM hms);
MMRESULT	WINAPI	midiStreamOpen(HMIDISTRM* phms, LPUINT uDeviceID, DWORD cMidi,
				       DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen); 
MMRESULT16	WINAPI	midiStreamOpen16(HMIDISTRM16* phms, LPUINT16 devid, DWORD cMidi,
					 DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen); 
MMRESULT16	WINAPI	midiStreamOut16(HMIDISTRM16 hms, LPMIDIHDR16 lpMidiHdr, UINT16 cbMidiHdr); 
MMRESULT	WINAPI	midiStreamOut(HMIDISTRM hms, LPMIDIHDR lpMidiHdr, UINT cbMidiHdr);
MMRESULT16	WINAPI	midiStreamPause16(HMIDISTRM16 hms);
MMRESULT	WINAPI	midiStreamPause(HMIDISTRM hms);
MMRESULT16	WINAPI	midiStreamPosition16(HMIDISTRM16 hms, LPMMTIME16 lpmmt, UINT16 cbmmt);
MMRESULT	WINAPI	midiStreamPosition(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt);
MMRESULT16	WINAPI	midiStreamProperty16(HMIDISTRM16 hms, LPBYTE lpPropData, DWORD dwProperty);
MMRESULT	WINAPI	midiStreamProperty(HMIDISTRM hms, LPBYTE lpPropData, DWORD dwProperty);
MMRESULT16	WINAPI	midiStreamRestart16(HMIDISTRM16 hms);
MMRESULT	WINAPI	midiStreamRestart(HMIDISTRM hms);
MMRESULT16	WINAPI	midiStreamStop16(HMIDISTRM16 hms);
MMRESULT	WINAPI	midiStreamStop(HMIDISTRM hms);

MMRESULT        WINAPI  midiConnect(HMIDI hMidiIn, HMIDIOUT hMidiOut, LPVOID pReserved);
MMRESULT        WINAPI  midiDisconnect(HMIDI hMidiIn, HMIDIOUT hMidiOut, LPVOID pReserved);

#define AUX_MAPPER     (-1)

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION16	vDriverVersion;		/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];	/* product name (NULL terminated string) */
    WORD	wTechnology;		/* type of device */
    DWORD	dwSupport;		/* functionality supported by driver */
} AUXCAPS16, *LPAUXCAPS16;

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION	vDriverVersion;		/* version of the driver */
    CHAR	szPname[MAXPNAMELEN];	/* product name (NULL terminated string) */
    WORD	wTechnology;		/* type of device */
    WORD	wReserved1;		/* padding */
    DWORD	dwSupport;		/* functionality supported by driver */
} AUXCAPSA, *LPAUXCAPSA;

typedef struct {
    WORD	wMid;			/* manufacturer ID */
    WORD	wPid;			/* product ID */
    MMVERSION	vDriverVersion;		/* version of the driver */
    WCHAR	szPname[MAXPNAMELEN];	/* product name (NULL terminated string) */
    WORD	wTechnology;		/* type of device */
    WORD	wReserved1;		/* padding */
    DWORD	dwSupport;		/* functionality supported by driver */
} AUXCAPSW, *LPAUXCAPSW;

#define AUXCAPS_CDAUDIO    1       /* audio from internal CD-ROM drive */
#define AUXCAPS_AUXIN      2       /* audio from auxiliary input jacks */

#define AUXCAPS_VOLUME          0x0001  /* supports volume control */
#define AUXCAPS_LRVOLUME        0x0002  /* separate left-right volume control */

UINT16		WINAPI	auxGetNumDevs16(void);
UINT		WINAPI	auxGetNumDevs(void);
UINT16		WINAPI	auxGetDevCaps16 (UINT16,LPAUXCAPS16,UINT16);
UINT		WINAPI	auxGetDevCapsA(UINT,LPAUXCAPSA,UINT);
UINT		WINAPI	auxGetDevCapsW(UINT,LPAUXCAPSW,UINT);
#define 		auxGetDevCaps WINELIB_NAME_AW(auxGetDevCaps)
UINT16		WINAPI	auxSetVolume16(UINT16,DWORD);
UINT		WINAPI	auxSetVolume(UINT,DWORD);

UINT16		WINAPI	auxGetVolume16(UINT16,LPDWORD);
UINT		WINAPI	auxGetVolume(UINT,LPDWORD);

UINT16		WINAPI	auxOutMessage16(UINT16,UINT16,DWORD,DWORD);
UINT		WINAPI	auxOutMessage(UINT,UINT,DWORD,DWORD);

#define TIMERR_NOERROR        (0)                  /* no error */
#define TIMERR_NOCANDO        (TIMERR_BASE+1)      /* request not completed */
#define TIMERR_STRUCT         (TIMERR_BASE+33)     /* time struct size */

typedef void (* CALLBACK LPTIMECALLBACK16)(UINT16 uTimerID, UINT16 uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef void (* CALLBACK LPTIMECALLBACK)(UINT uTimerID, UINT uMessage, DWORD dwUser, DWORD dw1, DWORD dw2);

#define TIME_ONESHOT			0x0000	/* program timer for single event */
#define TIME_PERIODIC			0x0001	/* program for continuous periodic event */
#define TIME_CALLBACK_FUNCTION		0x0000	/* callback is function */
#define TIME_CALLBACK_EVENT_SET		0x0010	/* callback is event - use SetEvent */
#define TIME_CALLBACK_EVENT_PULSE 	0x0020	/* callback is event - use PulseEvent */

typedef struct {
    UINT16	wPeriodMin;	/* minimum period supported  */
    UINT16	wPeriodMax;	/* maximum period supported  */
} TIMECAPS16,*LPTIMECAPS16;

typedef struct {
    UINT	wPeriodMin;
    UINT	wPeriodMax;
} TIMECAPS, *LPTIMECAPS;


MMRESULT16	WINAPI	timeGetSystemTime16(LPMMTIME16,UINT16);
MMRESULT	WINAPI	timeGetSystemTime(LPMMTIME,UINT);
DWORD		WINAPI	timeGetTime(void);	/* same for win32/win16 */
MMRESULT16	WINAPI	timeSetEvent16(UINT16,UINT16,LPTIMECALLBACK16,DWORD,UINT16);
MMRESULT	WINAPI	timeSetEvent(UINT,UINT,LPTIMECALLBACK,DWORD,UINT);
MMRESULT16	WINAPI	timeKillEvent16(UINT16);
MMRESULT	WINAPI	timeKillEvent(UINT);
MMRESULT16	WINAPI	timeGetDevCaps16(LPTIMECAPS16,UINT16);
MMRESULT	WINAPI	timeGetDevCaps(LPTIMECAPS,UINT);
MMRESULT16	WINAPI	timeBeginPeriod16(UINT16);
MMRESULT	WINAPI	timeBeginPeriod(UINT);
MMRESULT16	WINAPI	timeEndPeriod16(UINT16);
MMRESULT	WINAPI	timeEndPeriod(UINT);

#define JOYERR_NOERROR        (0)                  /* no error */
#define JOYERR_PARMS          (JOYERR_BASE+5)      /* bad parameters */
#define JOYERR_NOCANDO        (JOYERR_BASE+6)      /* request not completed */
#define JOYERR_UNPLUGGED      (JOYERR_BASE+7)      /* joystick is unplugged */

/* JOYINFO, JOYINFOEX, MM_JOY* */
#define JOY_BUTTON1         	0x0001
#define JOY_BUTTON2         	0x0002
#define JOY_BUTTON3         	0x0004
#define JOY_BUTTON4         	0x0008
#define JOY_BUTTON1CHG      	0x0100
#define JOY_BUTTON2CHG      	0x0200
#define JOY_BUTTON3CHG      	0x0400
#define JOY_BUTTON4CHG      	0x0800

#define JOYSTICKID1         	0
#define JOYSTICKID2         	1

/* JOYCAPS.wCaps */
#define JOYCAPS_HASZ		0x0001
#define JOYCAPS_HASR		0x0002
#define JOYCAPS_HASU		0x0004
#define JOYCAPS_HASV		0x0008
#define JOYCAPS_HASPOV		0x0010
#define JOYCAPS_POV4DIR		0x0020
#define JOYCAPS_POVCTS		0x0040

/* JOYINFOEX stuff */
#define JOY_POVCENTERED		(WORD) -1
#define JOY_POVFORWARD		0
#define JOY_POVRIGHT		9000
#define JOY_POVBACKWARD		18000
#define JOY_POVLEFT		27000

#define JOY_RETURNX		0x00000001
#define JOY_RETURNY		0x00000002
#define JOY_RETURNZ		0x00000004
#define JOY_RETURNR		0x00000008
#define JOY_RETURNU		0x00000010
#define JOY_RETURNV		0x00000020
#define JOY_RETURNPOV		0x00000040
#define JOY_RETURNBUTTONS	0x00000080
#define JOY_RETURNRAWDATA	0x00000100
#define JOY_RETURNPOVCTS	0x00000200
#define JOY_RETURNCENTERED	0x00000400
#define JOY_USEDEADZONE		0x00000800
#define JOY_RETURNALL		(JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | \
				 JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | \
				 JOY_RETURNPOV | JOY_RETURNBUTTONS)
#define JOY_CAL_READALWAYS	0x00010000
#define JOY_CAL_READXYONLY	0x00020000
#define JOY_CAL_READ3		0x00040000
#define JOY_CAL_READ4		0x00080000
#define JOY_CAL_READXONLY	0x00100000
#define JOY_CAL_READYONLY	0x00200000
#define JOY_CAL_READ5		0x00400000
#define JOY_CAL_READ6		0x00800000
#define JOY_CAL_READZONLY	0x01000000
#define JOY_CAL_READRONLY	0x02000000
#define JOY_CAL_READUONLY	0x04000000
#define JOY_CAL_READVONLY	0x08000000

typedef struct {
    WORD wMid;                  /* manufacturer ID */
    WORD wPid;                  /* product ID */
    char szPname[MAXPNAMELEN];	/* product name (NULL terminated string) */
    UINT16 wXmin;               /* minimum x position value */
    UINT16 wXmax;               /* maximum x position value */
    UINT16 wYmin;               /* minimum y position value */
    UINT16 wYmax;               /* maximum y position value */
    UINT16 wZmin;               /* minimum z position value */
    UINT16 wZmax;               /* maximum z position value */
    UINT16 wNumButtons;         /* number of buttons */
    UINT16 wPeriodMin;          /* minimum message period when captured */
    UINT16 wPeriodMax;          /* maximum message period when captured */
                                /* win95,nt4 additions: */
    UINT16 wRmin;		/* minimum r position value */
    UINT16 wRmax;		/* maximum r position value */
    UINT16 wUmin;		/* minimum u (5th axis) position value */
    UINT16 wUmax;		/* maximum u (5th axis) position value */
    UINT16 wVmin;		/* minimum v (6th axis) position value */
    UINT16 wVmax;		/* maximum v (6th axis) position value */
    UINT16 wCaps;		/* joystick capabilites */
    UINT16 wMaxAxes;		/* maximum number of axes supported */
    UINT16 wNumAxes;		/* number of axes in use */
    UINT16 wMaxButtons;		/* maximum number of buttons supported */
    CHAR szRegKey[MAXPNAMELEN]; /* registry key */
    CHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME]; /* OEM VxD in use */
} JOYCAPS16, *LPJOYCAPS16;

typedef struct {
    WORD wMid;
    WORD wPid;
    CHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    CHAR szRegKey[MAXPNAMELEN];
    CHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYCAPSA, *LPJOYCAPSA;

typedef struct {
    WORD wMid;
    WORD wPid;
    WCHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    WCHAR szRegKey[MAXPNAMELEN];
    WCHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYCAPSW, *LPJOYCAPSW;
DECL_WINELIB_TYPE_AW(JOYCAPS)
DECL_WINELIB_TYPE_AW(LPJOYCAPS)

typedef struct {
    UINT16 wXpos;                 /* x position */
    UINT16 wYpos;                 /* y position */
    UINT16 wZpos;                 /* z position */
    UINT16 wButtons;              /* button states */
} JOYINFO16, *LPJOYINFO16;

typedef struct {
    UINT wXpos;
    UINT wYpos;
    UINT wZpos;
    UINT wButtons;
} JOYINFO, *LPJOYINFO;

typedef struct {
    DWORD	dwSize;		/* size of structure */
    DWORD	dwFlags;	/* flags to indicate what to return */
    DWORD	dwXpos;		/* x position */
    DWORD	dwYpos;		/* y position */
    DWORD	dwZpos;		/* z position */
    DWORD	dwRpos;		/* rudder/4th axis position */
    DWORD	dwUpos;		/* 5th axis position */
    DWORD	dwVpos;		/* 6th axis position */
    DWORD	dwButtons;	/* button states */
    DWORD	dwButtonNumber;	/* current button number pressed */
    DWORD	dwPOV;		/* point of view state */
    DWORD	dwReserved1;	/* reserved for communication between winmm & driver */
    DWORD	dwReserved2;	/* reserved for future expansion */
} JOYINFOEX,*LPJOYINFOEX;


MMRESULT16	WINAPI	joyGetDevCaps16 (UINT16,LPJOYCAPS16 ,UINT16);
MMRESULT	WINAPI	joyGetDevCapsA(UINT,LPJOYCAPSA,UINT);
MMRESULT	WINAPI	joyGetDevCapsW(UINT,LPJOYCAPSW,UINT);
#define joyGetDevCaps WINELIB_NAME_AW(joyGetDevCaps)
UINT16		WINAPI	joyGetNumDevs16(void);
UINT		WINAPI	joyGetNumDevs(void);
MMRESULT16	WINAPI	joyGetPos16(UINT16,LPJOYINFO16);
MMRESULT	WINAPI	joyGetPos(UINT,LPJOYINFO);
MMRESULT16	WINAPI	joyGetPosEx16(UINT16,LPJOYINFOEX);
MMRESULT	WINAPI	joyGetPosEx(UINT,LPJOYINFOEX);
MMRESULT16	WINAPI	joyGetThreshold16(UINT16,UINT16*);
MMRESULT	WINAPI	joyGetThreshold(UINT,UINT*);
MMRESULT16	WINAPI	joyReleaseCapture16(UINT16);
MMRESULT	WINAPI	joyReleaseCapture(UINT);
MMRESULT16	WINAPI	joySetCapture16(HWND16,UINT16,UINT16,BOOL16);
MMRESULT	WINAPI	joySetCapture(HWND,UINT,UINT,BOOL);
MMRESULT16	WINAPI	joySetThreshold16(UINT16,UINT16);
MMRESULT	WINAPI	joySetThreshold(UINT,UINT);
MMRESULT        WINAPI  joyConfigChanged(DWORD dwFlags);

#define	MIXERR_BASE		1024
#define	MIXERR_INVALLINE	(MIXERR_BASE + 0)
#define MIXERR_INVALCONTROL	(MIXERR_BASE + 1)
#define MIXERR_INVALVALUE	(MIXERR_BASE + 2)
#define MIXERR_LASTERROR	(MIXERR_BASE + 2)

typedef struct {
	WORD		wMid;			/* manufacturer id */
	WORD		wPid;			/* product id */
	MMVERSION16	vDriverVersion;		/* version of the driver */
	CHAR		szPname[MAXPNAMELEN];	/* product name */
	DWORD		fdwSupport;		/* misc. support bits */
	DWORD		cDestinations;		/* count of destinations */
} MIXERCAPS16,*LPMIXERCAPS16;

typedef struct {
	WORD		wMid;
	WORD		wPid;
	MMVERSION	vDriverVersion;
	CHAR		szPname[MAXPNAMELEN];
	DWORD		fdwSupport;
	DWORD		cDestinations;
} MIXERCAPSA,*LPMIXERCAPSA;

typedef struct {
	WORD		wMid;
	WORD		wPid;
	MMVERSION	vDriverVersion;
	WCHAR		szPname[MAXPNAMELEN];
	DWORD		fdwSupport;
	DWORD		cDestinations;
} MIXERCAPSW,*LPMIXERCAPSW;

DECL_WINELIB_TYPE_AW(MIXERCAPS)
DECL_WINELIB_TYPE_AW(LPMIXERCAPS)

#define MIXER_SHORT_NAME_CHARS		16
#define MIXER_LONG_NAME_CHARS		64

/*  MIXERLINE.fdwLine */
#define	MIXERLINE_LINEF_ACTIVE		0x00000001
#define	MIXERLINE_LINEF_DISCONNECTED	0x00008000
#define	MIXERLINE_LINEF_SOURCE		0x80000000

/* Mixer flags */
#define MIXER_OBJECTF_HANDLE	0x80000000L
#define MIXER_OBJECTF_MIXER	0x00000000L
#define MIXER_OBJECTF_HMIXER	(MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_MIXER)
#define MIXER_OBJECTF_WAVEOUT	0x10000000L
#define MIXER_OBJECTF_HWAVEOUT	(MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_WAVEOUT)
#define MIXER_OBJECTF_WAVEIN	0x20000000L
#define MIXER_OBJECTF_HWAVEIN	(MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_WAVEIN)
#define MIXER_OBJECTF_MIDIOUT	0x30000000L
#define MIXER_OBJECTF_HMIDIOUT	(MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_MIDIOUT)
#define MIXER_OBJECTF_MIDIIN	0x40000000L
#define MIXER_OBJECTF_HMIDIIN	(MIXER_OBJECTF_HANDLE|MIXER_OBJECTF_MIDIIN)
#define MIXER_OBJECTF_AUX	0x50000000L

/*  MIXERLINE.dwComponentType */
/*  component types for destinations and sources */
#define	MIXERLINE_COMPONENTTYPE_DST_FIRST	0x00000000L
#define	MIXERLINE_COMPONENTTYPE_DST_UNDEFINED	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 0)
#define	MIXERLINE_COMPONENTTYPE_DST_DIGITAL	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 1)
#define	MIXERLINE_COMPONENTTYPE_DST_LINE	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 2)
#define	MIXERLINE_COMPONENTTYPE_DST_MONITOR	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 3)
#define	MIXERLINE_COMPONENTTYPE_DST_SPEAKERS	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 4)
#define	MIXERLINE_COMPONENTTYPE_DST_HEADPHONES	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 5)
#define	MIXERLINE_COMPONENTTYPE_DST_TELEPHONE	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 6)
#define	MIXERLINE_COMPONENTTYPE_DST_WAVEIN	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 7)
#define	MIXERLINE_COMPONENTTYPE_DST_VOICEIN	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 8)
#define	MIXERLINE_COMPONENTTYPE_DST_LAST	(MIXERLINE_COMPONENTTYPE_DST_FIRST + 8)

#define	MIXERLINE_COMPONENTTYPE_SRC_FIRST	0x00001000L
#define	MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 0)
#define	MIXERLINE_COMPONENTTYPE_SRC_DIGITAL	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 1)
#define	MIXERLINE_COMPONENTTYPE_SRC_LINE	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 2)
#define	MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 3)
#define	MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 4)
#define	MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 5)
#define	MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 6)
#define	MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 7)
#define	MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 8)
#define	MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 9)
#define	MIXERLINE_COMPONENTTYPE_SRC_ANALOG	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 10)
#define	MIXERLINE_COMPONENTTYPE_SRC_LAST	(MIXERLINE_COMPONENTTYPE_SRC_FIRST + 10)

/*  MIXERLINE.Target.dwType */
#define	MIXERLINE_TARGETTYPE_UNDEFINED	0
#define	MIXERLINE_TARGETTYPE_WAVEOUT	1
#define	MIXERLINE_TARGETTYPE_WAVEIN	2
#define	MIXERLINE_TARGETTYPE_MIDIOUT	3
#define	MIXERLINE_TARGETTYPE_MIDIIN	4
#define MIXERLINE_TARGETTYPE_AUX	5

typedef struct {
    DWORD	cbStruct;		/* size of MIXERLINE structure */
    DWORD	dwDestination;		/* zero based destination index */
    DWORD	dwSource;		/* zero based source index (if source) */
    DWORD	dwLineID;		/* unique line id for mixer device */
    DWORD	fdwLine;		/* state/information about line */
    DWORD	dwUser;			/* driver specific information */
    DWORD	dwComponentType;	/* component type line connects to */
    DWORD	cChannels;		/* number of channels line supports */
    DWORD	cConnections;		/* number of connections [possible] */
    DWORD	cControls;		/* number of controls at this line */
    CHAR	szShortName[MIXER_SHORT_NAME_CHARS];
    CHAR	szName[MIXER_LONG_NAME_CHARS];
    struct {
	DWORD	dwType;			/* MIXERLINE_TARGETTYPE_xxxx */
	DWORD	dwDeviceID;		/* target device ID of device type */
	WORD	wMid;			/* of target device */
	WORD	wPid;			/*      " */
	MMVERSION16	vDriverVersion;	/*      " */
	CHAR	szPname[MAXPNAMELEN];	/*      " */
    } Target;
} MIXERLINE16, *LPMIXERLINE16;

typedef struct {
    DWORD	cbStruct;
    DWORD	dwDestination;
    DWORD	dwSource;
    DWORD	dwLineID;
    DWORD	fdwLine;
    DWORD	dwUser;
    DWORD	dwComponentType;
    DWORD	cChannels;
    DWORD	cConnections;
    DWORD	cControls;
    CHAR	szShortName[MIXER_SHORT_NAME_CHARS];
    CHAR	szName[MIXER_LONG_NAME_CHARS];
    struct {
	DWORD	dwType;
	DWORD	dwDeviceID;
	WORD	wMid;
	WORD	wPid;
	MMVERSION	vDriverVersion;
	CHAR	szPname[MAXPNAMELEN];
    } Target;
} MIXERLINEA, *LPMIXERLINEA;

typedef struct {
    DWORD	cbStruct;
    DWORD	dwDestination;
    DWORD	dwSource;
    DWORD	dwLineID;
    DWORD	fdwLine;
    DWORD	dwUser;
    DWORD	dwComponentType;
    DWORD	cChannels;
    DWORD	cConnections;
    DWORD	cControls;
    WCHAR	szShortName[MIXER_SHORT_NAME_CHARS];
    WCHAR	szName[MIXER_LONG_NAME_CHARS];
    struct {
	DWORD	dwType;
	DWORD	dwDeviceID;
	WORD	wMid;
	WORD	wPid;
	MMVERSION	vDriverVersion;
	WCHAR	szPname[MAXPNAMELEN];
    } Target;
} MIXERLINEW, *LPMIXERLINEW;

DECL_WINELIB_TYPE_AW(MIXERLINE)
DECL_WINELIB_TYPE_AW(LPMIXERLINE)

/*  MIXERCONTROL.fdwControl */
#define	MIXERCONTROL_CONTROLF_UNIFORM		0x00000001L
#define	MIXERCONTROL_CONTROLF_MULTIPLE		0x00000002L
#define	MIXERCONTROL_CONTROLF_DISABLED		0x80000000L

/*  MIXERCONTROL_CONTROLTYPE_xxx building block defines */
#define	MIXERCONTROL_CT_CLASS_MASK		0xF0000000L
#define	MIXERCONTROL_CT_CLASS_CUSTOM		0x00000000L
#define	MIXERCONTROL_CT_CLASS_METER		0x10000000L
#define	MIXERCONTROL_CT_CLASS_SWITCH		0x20000000L
#define	MIXERCONTROL_CT_CLASS_NUMBER		0x30000000L
#define	MIXERCONTROL_CT_CLASS_SLIDER		0x40000000L
#define	MIXERCONTROL_CT_CLASS_FADER		0x50000000L
#define	MIXERCONTROL_CT_CLASS_TIME		0x60000000L
#define	MIXERCONTROL_CT_CLASS_LIST		0x70000000L

#define	MIXERCONTROL_CT_SUBCLASS_MASK		0x0F000000L

#define	MIXERCONTROL_CT_SC_SWITCH_BOOLEAN	0x00000000L
#define	MIXERCONTROL_CT_SC_SWITCH_BUTTON	0x01000000L

#define	MIXERCONTROL_CT_SC_METER_POLLED		0x00000000L

#define	MIXERCONTROL_CT_SC_TIME_MICROSECS	0x00000000L
#define	MIXERCONTROL_CT_SC_TIME_MILLISECS	0x01000000L

#define	MIXERCONTROL_CT_SC_LIST_SINGLE		0x00000000L
#define	MIXERCONTROL_CT_SC_LIST_MULTIPLE	0x01000000L

#define	MIXERCONTROL_CT_UNITS_MASK		0x00FF0000L
#define	MIXERCONTROL_CT_UNITS_CUSTOM		0x00000000L
#define	MIXERCONTROL_CT_UNITS_BOOLEAN		0x00010000L
#define	MIXERCONTROL_CT_UNITS_SIGNED		0x00020000L
#define	MIXERCONTROL_CT_UNITS_UNSIGNED		0x00030000L
#define	MIXERCONTROL_CT_UNITS_DECIBELS		0x00040000L /* in 10ths */
#define	MIXERCONTROL_CT_UNITS_PERCENT		0x00050000L /* in 10ths */

/*  Commonly used control types for specifying MIXERCONTROL.dwControlType */
#define MIXERCONTROL_CONTROLTYPE_CUSTOM		(MIXERCONTROL_CT_CLASS_CUSTOM | MIXERCONTROL_CT_UNITS_CUSTOM)
#define MIXERCONTROL_CONTROLTYPE_BOOLEANMETER	(MIXERCONTROL_CT_CLASS_METER | MIXERCONTROL_CT_SC_METER_POLLED | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_SIGNEDMETER	(MIXERCONTROL_CT_CLASS_METER | MIXERCONTROL_CT_SC_METER_POLLED | MIXERCONTROL_CT_UNITS_SIGNED)
#define MIXERCONTROL_CONTROLTYPE_PEAKMETER	(MIXERCONTROL_CONTROLTYPE_SIGNEDMETER + 1)
#define MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER	(MIXERCONTROL_CT_CLASS_METER | MIXERCONTROL_CT_SC_METER_POLLED | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_BOOLEAN	(MIXERCONTROL_CT_CLASS_SWITCH | MIXERCONTROL_CT_SC_SWITCH_BOOLEAN | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_ONOFF		(MIXERCONTROL_CONTROLTYPE_BOOLEAN + 1)
#define MIXERCONTROL_CONTROLTYPE_MUTE		(MIXERCONTROL_CONTROLTYPE_BOOLEAN + 2)
#define MIXERCONTROL_CONTROLTYPE_MONO		(MIXERCONTROL_CONTROLTYPE_BOOLEAN + 3)
#define MIXERCONTROL_CONTROLTYPE_LOUDNESS	(MIXERCONTROL_CONTROLTYPE_BOOLEAN + 4)
#define MIXERCONTROL_CONTROLTYPE_STEREOENH	(MIXERCONTROL_CONTROLTYPE_BOOLEAN + 5)
#define MIXERCONTROL_CONTROLTYPE_BUTTON		(MIXERCONTROL_CT_CLASS_SWITCH | MIXERCONTROL_CT_SC_SWITCH_BUTTON | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_DECIBELS	(MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_DECIBELS)
#define MIXERCONTROL_CONTROLTYPE_SIGNED		(MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_SIGNED)
#define MIXERCONTROL_CONTROLTYPE_UNSIGNED	(MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_PERCENT	(MIXERCONTROL_CT_CLASS_NUMBER | MIXERCONTROL_CT_UNITS_PERCENT)
#define MIXERCONTROL_CONTROLTYPE_SLIDER		(MIXERCONTROL_CT_CLASS_SLIDER | MIXERCONTROL_CT_UNITS_SIGNED)
#define MIXERCONTROL_CONTROLTYPE_PAN		(MIXERCONTROL_CONTROLTYPE_SLIDER + 1)
#define MIXERCONTROL_CONTROLTYPE_QSOUNDPAN	(MIXERCONTROL_CONTROLTYPE_SLIDER + 2)
#define MIXERCONTROL_CONTROLTYPE_FADER		(MIXERCONTROL_CT_CLASS_FADER | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_VOLUME		(MIXERCONTROL_CONTROLTYPE_FADER + 1)
#define MIXERCONTROL_CONTROLTYPE_BASS		(MIXERCONTROL_CONTROLTYPE_FADER + 2)
#define MIXERCONTROL_CONTROLTYPE_TREBLE		(MIXERCONTROL_CONTROLTYPE_FADER + 3)
#define MIXERCONTROL_CONTROLTYPE_EQUALIZER	(MIXERCONTROL_CONTROLTYPE_FADER + 4)
#define MIXERCONTROL_CONTROLTYPE_SINGLESELECT	(MIXERCONTROL_CT_CLASS_LIST | MIXERCONTROL_CT_SC_LIST_SINGLE | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_MUX		(MIXERCONTROL_CONTROLTYPE_SINGLESELECT + 1)
#define MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT	(MIXERCONTROL_CT_CLASS_LIST | MIXERCONTROL_CT_SC_LIST_MULTIPLE | MIXERCONTROL_CT_UNITS_BOOLEAN)
#define MIXERCONTROL_CONTROLTYPE_MIXER		(MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT + 1)
#define MIXERCONTROL_CONTROLTYPE_MICROTIME	(MIXERCONTROL_CT_CLASS_TIME | MIXERCONTROL_CT_SC_TIME_MICROSECS | MIXERCONTROL_CT_UNITS_UNSIGNED)
#define MIXERCONTROL_CONTROLTYPE_MILLITIME	(MIXERCONTROL_CT_CLASS_TIME | MIXERCONTROL_CT_SC_TIME_MILLISECS | MIXERCONTROL_CT_UNITS_UNSIGNED)


typedef struct {
    DWORD		cbStruct;           /* size in bytes of MIXERCONTROL */
    DWORD		dwControlID;        /* unique control id for mixer device */
    DWORD		dwControlType;      /* MIXERCONTROL_CONTROLTYPE_xxx */
    DWORD		fdwControl;         /* MIXERCONTROL_CONTROLF_xxx */
    DWORD		cMultipleItems;     /* if MIXERCONTROL_CONTROLF_MULTIPLE set */
    CHAR		szShortName[MIXER_SHORT_NAME_CHARS];
    CHAR		szName[MIXER_LONG_NAME_CHARS];
    union {
	struct {
	    LONG	lMinimum;	/* signed minimum for this control */
	    LONG	lMaximum;	/* signed maximum for this control */
	} DUMMYSTRUCTNAME;
	struct {
	    DWORD	dwMinimum;	/* unsigned minimum for this control */
	    DWORD	dwMaximum;	/* unsigned maximum for this control */
	} DUMMYSTRUCTNAME1;
	DWORD       	dwReserved[6];
    } Bounds;
    union {
	DWORD		cSteps;		/* # of steps between min & max */
	DWORD		cbCustomData;	/* size in bytes of custom data */
	DWORD		dwReserved[6];	/* !!! needed? we have cbStruct.... */
    } Metrics;
} MIXERCONTROL16, *LPMIXERCONTROL16;

typedef struct {
    DWORD		cbStruct;
    DWORD		dwControlID;
    DWORD		dwControlType;
    DWORD		fdwControl;
    DWORD		cMultipleItems;
    CHAR		szShortName[MIXER_SHORT_NAME_CHARS];
    CHAR		szName[MIXER_LONG_NAME_CHARS];
    union {
	struct {
	    LONG	lMinimum;
	    LONG	lMaximum;
	} DUMMYSTRUCTNAME;
	struct {
	    DWORD	dwMinimum;
	    DWORD	dwMaximum;
	} DUMMYSTRUCTNAME1;
	DWORD       	dwReserved[6];
    } Bounds;
    union {
	DWORD		cSteps;
	DWORD		cbCustomData;
	DWORD		dwReserved[6];
    } Metrics;
} MIXERCONTROLA, *LPMIXERCONTROLA;

typedef struct {
    DWORD		cbStruct;
    DWORD		dwControlID;
    DWORD		dwControlType;
    DWORD		fdwControl;
    DWORD		cMultipleItems;
    WCHAR		szShortName[MIXER_SHORT_NAME_CHARS];
    WCHAR		szName[MIXER_LONG_NAME_CHARS];
    union {
	struct {
	    LONG	lMinimum;
	    LONG	lMaximum;
	} DUMMYSTRUCTNAME;
	struct {
	    DWORD	dwMinimum;
	    DWORD	dwMaximum;
	} DUMMYSTRUCTNAME1;
	DWORD       	dwReserved[6];
    } Bounds;
    union {
	DWORD		cSteps;
	DWORD		cbCustomData;
	DWORD		dwReserved[6];
    } Metrics;
} MIXERCONTROLW, *LPMIXERCONTROLW;

DECL_WINELIB_TYPE_AW(MIXERCONTROL)
DECL_WINELIB_TYPE_AW(LPMIXERCONTROL)

typedef struct {
    DWORD	cbStruct;	/* size in bytes of MIXERLINECONTROLS */
    DWORD	dwLineID;	/* line id (from MIXERLINE.dwLineID) */
    union {
	DWORD	dwControlID;	/* MIXER_GETLINECONTROLSF_ONEBYID */
	DWORD	dwControlType;	/* MIXER_GETLINECONTROLSF_ONEBYTYPE */
    } DUMMYUNIONNAME;
    DWORD	cControls;	/* count of controls pmxctrl points to */
    DWORD	cbmxctrl;	/* size in bytes of _one_ MIXERCONTROL */
    LPMIXERCONTROL16	pamxctrl;/* pointer to first MIXERCONTROL array */
} MIXERLINECONTROLS16, *LPMIXERLINECONTROLS16;

typedef struct {
    DWORD	cbStruct;
    DWORD	dwLineID;
    union {
	DWORD	dwControlID;
	DWORD	dwControlType;
    } DUMMYUNIONNAME;
    DWORD	cControls;
    DWORD	cbmxctrl;
    LPMIXERCONTROLA	pamxctrl;
} MIXERLINECONTROLSA, *LPMIXERLINECONTROLSA;

typedef struct {
    DWORD	cbStruct;
    DWORD	dwLineID;
    union {
	DWORD	dwControlID;
	DWORD	dwControlType;
    } DUMMYUNIONNAME;
    DWORD	cControls;
    DWORD	cbmxctrl;
    LPMIXERCONTROLW	pamxctrl;
} MIXERLINECONTROLSW, *LPMIXERLINECONTROLSW;

DECL_WINELIB_TYPE_AW(MIXERLINECONTROLS)
DECL_WINELIB_TYPE_AW(LPMIXERLINECONTROLS)

typedef struct {
    DWORD	cbStruct;	/* size in bytes of MIXERCONTROLDETAILS */
    DWORD	dwControlID;	/* control id to get/set details on */
    DWORD	cChannels;	/* number of channels in paDetails array */
    union {
        HWND16	hwndOwner;	/* for MIXER_SETCONTROLDETAILSF_CUSTOM */
        DWORD	cMultipleItems;	/* if _MULTIPLE, the number of items per channel */
    } DUMMYUNIONNAME;
    DWORD	cbDetails;	/* size of _one_ details_XX struct */
    LPVOID	paDetails;	/* pointer to array of details_XX structs */
} MIXERCONTROLDETAILS16,*LPMIXERCONTROLDETAILS16;

typedef struct {
    DWORD	cbStruct;
    DWORD	dwControlID;
    DWORD	cChannels;
    union {
        HWND	hwndOwner;
        DWORD	cMultipleItems;
    } DUMMYUNIONNAME;
    DWORD	cbDetails;
    LPVOID	paDetails;
} MIXERCONTROLDETAILS,*LPMIXERCONTROLDETAILS;


typedef struct {
    DWORD	dwParam1;
    DWORD	dwParam2;
    CHAR	szName[MIXER_LONG_NAME_CHARS];
} MIXERCONTROLDETAILS_LISTTEXT16,*LPMIXERCONTROLDETAILS_LISTTEXT16;

typedef struct {
    DWORD	dwParam1;
    DWORD	dwParam2;
    CHAR	szName[MIXER_LONG_NAME_CHARS];
} MIXERCONTROLDETAILS_LISTTEXTA,*LPMIXERCONTROLDETAILS_LISTTEXTA;

typedef struct {
    DWORD	dwParam1;
    DWORD	dwParam2;
    WCHAR	szName[MIXER_LONG_NAME_CHARS];
} MIXERCONTROLDETAILS_LISTTEXTW,*LPMIXERCONTROLDETAILS_LISTTEXTW;

DECL_WINELIB_TYPE_AW(MIXERCONTROLDETAILS_LISTTEXT)
DECL_WINELIB_TYPE_AW(LPMIXERCONTROLDETAILS_LISTTEXT)

/*  MIXER_GETCONTROLDETAILSF_VALUE */
typedef struct {
	LONG	fValue;
} MIXERCONTROLDETAILS_BOOLEAN,*LPMIXERCONTROLDETAILS_BOOLEAN;

typedef struct {
	LONG	lValue;
} MIXERCONTROLDETAILS_SIGNED,*LPMIXERCONTROLDETAILS_SIGNED;

typedef struct {
	DWORD	dwValue;
} MIXERCONTROLDETAILS_UNSIGNED,*LPMIXERCONTROLDETAILS_UNSIGNED;

/* bits passed to mixerGetLineInfo.fdwInfo */
#define MIXER_GETLINEINFOF_DESTINATION		0x00000000L
#define MIXER_GETLINEINFOF_SOURCE		0x00000001L
#define MIXER_GETLINEINFOF_LINEID		0x00000002L
#define MIXER_GETLINEINFOF_COMPONENTTYPE	0x00000003L
#define MIXER_GETLINEINFOF_TARGETTYPE		0x00000004L
#define MIXER_GETLINEINFOF_QUERYMASK		0x0000000FL

/* bitmask passed to mixerGetLineControl */
#define MIXER_GETLINECONTROLSF_ALL		0x00000000L
#define MIXER_GETLINECONTROLSF_ONEBYID		0x00000001L
#define MIXER_GETLINECONTROLSF_ONEBYTYPE	0x00000002L
#define MIXER_GETLINECONTROLSF_QUERYMASK	0x0000000FL

/* bitmask passed to mixerGetControlDetails */
#define MIXER_GETCONTROLDETAILSF_VALUE		0x00000000L
#define MIXER_GETCONTROLDETAILSF_LISTTEXT	0x00000001L
#define MIXER_GETCONTROLDETAILSF_QUERYMASK	0x0000000FL

/* bitmask passed to mixerSetControlDetails */
#define	MIXER_SETCONTROLDETAILSF_VALUE		0x00000000L
#define	MIXER_SETCONTROLDETAILSF_CUSTOM		0x00000001L
#define	MIXER_SETCONTROLDETAILSF_QUERYMASK	0x0000000FL

UINT16		WINAPI	mixerGetNumDevs16(void);
UINT		WINAPI	mixerGetNumDevs(void);
UINT16		WINAPI	mixerOpen16(LPHMIXER16,UINT16,DWORD,DWORD,DWORD);
UINT		WINAPI	mixerOpen(LPHMIXER,UINT,DWORD,DWORD,DWORD);
UINT16		WINAPI	mixerClose16(HMIXER16);
UINT		WINAPI	mixerClose(HMIXER);
UINT16		WINAPI	mixerMessage16(HMIXER16,UINT16,DWORD,DWORD);
UINT		WINAPI	mixerMessage(HMIXER,UINT,DWORD,DWORD);
UINT16		WINAPI	mixerGetDevCaps16(UINT16,LPMIXERCAPS16,UINT16);
UINT		WINAPI	mixerGetDevCapsA(UINT,LPMIXERCAPSA,UINT);
UINT		WINAPI	mixerGetDevCapsW(UINT,LPMIXERCAPSW,UINT);
#define 		mixerGetDevCaps WINELIB_NAME_AW(mixerGetDevCaps)
UINT16		WINAPI	mixerGetLineInfo16(HMIXEROBJ16,LPMIXERLINE16,DWORD);
UINT		WINAPI	mixerGetLineInfoA(HMIXEROBJ,LPMIXERLINEA,DWORD);
UINT		WINAPI	mixerGetLineInfoW(HMIXEROBJ,LPMIXERLINEW,DWORD);
#define 		mixerGetLineInfo WINELIB_NAME_AW(mixerGetLineInfo)
UINT16		WINAPI	mixerGetID16(HMIXEROBJ16,LPUINT16,DWORD);
UINT		WINAPI	mixerGetID(HMIXEROBJ,LPUINT,DWORD);
UINT16		WINAPI	mixerGetLineControls16(HMIXEROBJ16,LPMIXERLINECONTROLS16,DWORD);
UINT		WINAPI	mixerGetLineControlsA(HMIXEROBJ,LPMIXERLINECONTROLSA,DWORD);
UINT		WINAPI	mixerGetLineControlsW(HMIXEROBJ,LPMIXERLINECONTROLSW,DWORD);
#define 		mixerGetLineControls WINELIB_NAME_AW(mixerGetLineControls)
UINT16		WINAPI	mixerGetControlDetails16(HMIXEROBJ16,LPMIXERCONTROLDETAILS16,DWORD);
UINT		WINAPI	mixerGetControlDetailsA(HMIXEROBJ,LPMIXERCONTROLDETAILS,DWORD);
UINT		WINAPI	mixerGetControlDetailsW(HMIXEROBJ,LPMIXERCONTROLDETAILS,DWORD);
#define 		mixerGetControlDetails WINELIB_NAME_AW(mixerGetControlDetails)
UINT16		WINAPI	mixerSetControlDetails16(HMIXEROBJ16,LPMIXERCONTROLDETAILS16,DWORD);
UINT		WINAPI	mixerSetControlDetails(HMIXEROBJ,LPMIXERCONTROLDETAILS,DWORD);

#define MMIOERR_BASE            256
#define MMIOERR_FILENOTFOUND    (MMIOERR_BASE + 1)  /* file not found */
#define MMIOERR_OUTOFMEMORY     (MMIOERR_BASE + 2)  /* out of memory */
#define MMIOERR_CANNOTOPEN      (MMIOERR_BASE + 3)  /* cannot open */
#define MMIOERR_CANNOTCLOSE     (MMIOERR_BASE + 4)  /* cannot close */
#define MMIOERR_CANNOTREAD      (MMIOERR_BASE + 5)  /* cannot read */
#define MMIOERR_CANNOTWRITE     (MMIOERR_BASE + 6)  /* cannot write */
#define MMIOERR_CANNOTSEEK      (MMIOERR_BASE + 7)  /* cannot seek */
#define MMIOERR_CANNOTEXPAND    (MMIOERR_BASE + 8)  /* cannot expand file */
#define MMIOERR_CHUNKNOTFOUND   (MMIOERR_BASE + 9)  /* chunk not found */
#define MMIOERR_UNBUFFERED      (MMIOERR_BASE + 10) /* file is unbuffered */

#define CFSEPCHAR       '+'             /* compound file name separator char. */

typedef DWORD           FOURCC;         /* a four character code */
typedef LONG (* CALLBACK LPMMIOPROC16)(LPSTR lpmmioinfo, UINT16 uMessage,
                                      LPARAM lParam1, LPARAM lParam2);
typedef LONG (* CALLBACK LPMMIOPROC)(LPSTR lpmmioinfo, UINT uMessage,
				    LPARAM lParam1, LPARAM lParam2);

typedef struct {
        DWORD		dwFlags;	/* general status flags */
        FOURCC		fccIOProc;	/* pointer to I/O procedure */
        LPMMIOPROC16	pIOProc;	/* pointer to I/O procedure */
        UINT16		wErrorRet;	/* place for error to be returned */
        HTASK16		htask;		/* alternate local task */
        /* fields maintained by MMIO functions during buffered I/O */
        LONG		cchBuffer;	/* size of I/O buffer (or 0L) */
        HPSTR		pchBuffer;	/* start of I/O buffer (or NULL) */
        HPSTR		pchNext;	/* pointer to next byte to read/write */
        HPSTR		pchEndRead;	/* pointer to last valid byte to read */
        HPSTR		pchEndWrite;	/* pointer to last byte to write */
        LONG		lBufOffset;	/* disk offset of start of buffer */
        /* fields maintained by I/O procedure */
        LONG		lDiskOffset;	/* disk offset of next read or write */
        DWORD		adwInfo[3];	/* data specific to type of MMIOPROC */
        /* other fields maintained by MMIO */
        DWORD		dwReserved1;	/* reserved for MMIO use */
        DWORD		dwReserved2;	/* reserved for MMIO use */
        HMMIO16		hmmio;		/* handle to open file */
} MMIOINFO16, *LPMMIOINFO16;

typedef struct {
        DWORD		dwFlags;
        FOURCC		fccIOProc;
        LPMMIOPROC	pIOProc;
        UINT		wErrorRet;
        HTASK		htask;
        /* fields maintained by MMIO functions during buffered I/O */
        LONG		cchBuffer;
        HPSTR		pchBuffer;
        HPSTR		pchNext;
        HPSTR		pchEndRead;
        HPSTR		pchEndWrite;
        LONG		lBufOffset;
        /* fields maintained by I/O procedure */
        LONG		lDiskOffset;
        DWORD		adwInfo[3];
        /* other fields maintained by MMIO */
        DWORD		dwReserved1;
        DWORD		dwReserved2;
        HMMIO		hmmio;
} MMIOINFO, *PMMIOINFO, *LPMMIOINFO;


typedef struct _MMCKINFO
{
        FOURCC          ckid;           /* chunk ID */
        DWORD           cksize;         /* chunk size */
        FOURCC          fccType;        /* form type or list type */
        DWORD           dwDataOffset;   /* offset of data portion of chunk */
        DWORD           dwFlags;        /* flags used by MMIO functions */
} MMCKINFO, *LPMMCKINFO;

#define MMIO_RWMODE     0x00000003      /* open file for reading/writing/both */
#define MMIO_SHAREMODE  0x00000070      /* file sharing mode number */

#define MMIO_CREATE     0x00001000      /* create new file (or truncate file) */
#define MMIO_PARSE      0x00000100      /* parse new file returning path */
#define MMIO_DELETE     0x00000200      /* create new file (or truncate file) */
#define MMIO_EXIST      0x00004000      /* checks for existence of file */
#define MMIO_ALLOCBUF   0x00010000      /* mmioOpen() should allocate a buffer */
#define MMIO_GETTEMP    0x00020000      /* mmioOpen() should retrieve temp name */

#define MMIO_DIRTY      0x10000000      /* I/O buffer is dirty */

#define MMIO_READ       0x00000000      /* open file for reading only */
#define MMIO_WRITE      0x00000001      /* open file for writing only */
#define MMIO_READWRITE  0x00000002      /* open file for reading and writing */

#define MMIO_COMPAT     0x00000000      /* compatibility mode */
#define MMIO_EXCLUSIVE  0x00000010      /* exclusive-access mode */
#define MMIO_DENYWRITE  0x00000020      /* deny writing to other processes */
#define MMIO_DENYREAD   0x00000030      /* deny reading to other processes */
#define MMIO_DENYNONE   0x00000040      /* deny nothing to other processes */

#define MMIO_FHOPEN             0x0010  /* mmioClose: keep file handle open */
#define MMIO_EMPTYBUF           0x0010  /* mmioFlush: empty the I/O buffer */
#define MMIO_TOUPPER            0x0010  /* mmioStringToFOURCC: to u-case */
#define MMIO_INSTALLPROC    0x00010000  /* mmioInstallIOProc: install MMIOProc */
#define MMIO_GLOBALPROC     0x10000000  /* mmioInstallIOProc: install globally */
#define MMIO_REMOVEPROC     0x00020000  /* mmioInstallIOProc: remove MMIOProc */
#define MMIO_FINDPROC       0x00040000  /* mmioInstallIOProc: find an MMIOProc */
#define MMIO_FINDCHUNK          0x0010  /* mmioDescend: find a chunk by ID */
#define MMIO_FINDRIFF           0x0020  /* mmioDescend: find a LIST chunk */
#define MMIO_FINDLIST           0x0040  /* mmioDescend: find a RIFF chunk */
#define MMIO_CREATERIFF         0x0020  /* mmioCreateChunk: make a LIST chunk */
#define MMIO_CREATELIST         0x0040  /* mmioCreateChunk: make a RIFF chunk */

#define SEEK_SET   0
#define SEEK_CUR   1
#define SEEK_END   2

#define MMIOM_READ      MMIO_READ       /* read */
#define MMIOM_WRITE    MMIO_WRITE       /* write */
#define MMIOM_SEEK              2       /* seek to a new position in file */
#define MMIOM_OPEN              3       /* open file */
#define MMIOM_CLOSE             4       /* close file */
#define MMIOM_WRITEFLUSH        5       /* write and flush */

#define MMIOM_RENAME            6       /* rename specified file */

#define MMIOM_USER         0x8000       /* beginning of user-defined messages */

#define FOURCC_RIFF     mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC('L', 'I', 'S', 'T')

#define FOURCC_DOS      mmioFOURCC('D', 'O', 'S', ' ')
#define FOURCC_MEM      mmioFOURCC('M', 'E', 'M', ' ')

#define MMIO_DEFAULTBUFFER      8192    /* default buffer size */

#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

LPMMIOPROC16	WINAPI 	mmioInstallIOProc16(FOURCC,LPMMIOPROC16,DWORD);
LPMMIOPROC 	WINAPI 	mmioInstallIOProcA(FOURCC,LPMMIOPROC,DWORD);
LPMMIOPROC 	WINAPI 	mmioInstallIOProcW(FOURCC,LPMMIOPROC,DWORD);
#define      		mmioInstallIOProc WINELIB_NAME_AW(mmioInstallIOProc)

FOURCC 		WINAPI	mmioStringToFOURCC16(LPCSTR,UINT16);
FOURCC 		WINAPI	mmioStringToFOURCCA(LPCSTR,UINT);
FOURCC 		WINAPI	mmioStringToFOURCCW(LPCWSTR,UINT);
#define 		mmioStringToFOURCC WINELIB_NAME_AW(mmioStringToFOURCC)
HMMIO16		WINAPI	mmioOpen16 (LPSTR ,MMIOINFO16*,DWORD);
HMMIO		WINAPI	mmioOpenA(LPSTR ,MMIOINFO*,DWORD);
HMMIO		WINAPI	mmioOpenW(LPWSTR,MMIOINFO*,DWORD);
#define			mmioOpen WINELIB_NAME_AW(mmioOpen)

UINT16 		WINAPI	mmioRename16(LPCSTR szFileName, LPCSTR szNewFileName,
				     MMIOINFO16 * lpmmioinfo, DWORD dwRenameFlags);
UINT 		WINAPI	mmioRenameA(LPCSTR szFileName, LPCSTR szNewFileName,
				    MMIOINFO * lpmmioinfo, DWORD dwRenameFlags);
UINT 		WINAPI	mmioRenameW(LPCWSTR szFileName, LPCWSTR szNewFileName,
				    MMIOINFO * lpmmioinfo, DWORD dwRenameFlags);
#define 		mmioRename WINELIB_NAME_AW(mmioRename)

MMRESULT16 	WINAPI	mmioClose16(HMMIO16,UINT16);
MMRESULT 	WINAPI	mmioClose(HMMIO,UINT);
LONG 		WINAPI	mmioRead16(HMMIO16,HPSTR,LONG);
LONG 		WINAPI	mmioRead(HMMIO,HPSTR,LONG);
LONG 		WINAPI	mmioWrite16(HMMIO16,HPCSTR,LONG);
LONG 		WINAPI	mmioWrite(HMMIO,HPCSTR,LONG);
LONG 		WINAPI	mmioSeek16(HMMIO16,LONG,INT16);
LONG 		WINAPI	mmioSeek(HMMIO,LONG,INT);
MMRESULT16	WINAPI	mmioGetInfo16(HMMIO16,MMIOINFO16*,UINT16);
MMRESULT 	WINAPI	mmioGetInfo(HMMIO,MMIOINFO*,UINT);
MMRESULT16 	WINAPI	mmioSetInfo16(HMMIO16,const MMIOINFO16*,UINT16);
MMRESULT 	WINAPI	mmioSetInfo(HMMIO,const MMIOINFO*,UINT);
UINT16 		WINAPI	mmioSetBuffer16(HMMIO16,LPSTR,LONG,UINT16);
UINT 		WINAPI	mmioSetBuffer(HMMIO,LPSTR,LONG,UINT);
UINT16 		WINAPI	mmioFlush16(HMMIO16,UINT16);
UINT 		WINAPI	mmioFlush(HMMIO,UINT);
UINT16 		WINAPI	mmioAdvance16(HMMIO16,MMIOINFO16*,UINT16);
UINT 		WINAPI	mmioAdvance(HMMIO,MMIOINFO*,UINT);
LONG 		WINAPI	mmioSendMessage(HMMIO,UINT,LPARAM,LPARAM);
UINT		WINAPI	mmioDescend(HMMIO,MMCKINFO*,const MMCKINFO*,UINT);

UINT16		WINAPI	mmioAscend16(HMMIO16,MMCKINFO*,UINT16);
UINT		WINAPI	mmioAscend(HMMIO,MMCKINFO*,UINT);
UINT16		WINAPI 	mmioCreateChunk16(HMMIO16,MMCKINFO*,UINT16);
UINT		WINAPI	mmioCreateChunk(HMMIO,MMCKINFO*,UINT);

typedef UINT16 (* CALLBACK YIELDPROC)(UINT16,DWORD);

DWORD		WINAPI	mciSendCommand16(UINT16,UINT16,DWORD,DWORD);
MCIERROR	WINAPI	mciSendCommandA(MCIDEVICEID,UINT,DWORD,DWORD);
MCIERROR	WINAPI	mciSendCommandW(MCIDEVICEID,UINT,DWORD,DWORD);
#define 		mciSendCommand WINELIB_NAME_AW(mciSendCommand)

DWORD		WINAPI	mciSendString16(LPCSTR,LPSTR,UINT16,HWND16);
DWORD		WINAPI	mciSendStringA(LPCSTR,LPSTR,UINT,HWND);
DWORD		WINAPI	mciSendStringW(LPCWSTR,LPWSTR,UINT,HWND);
#define 		mciSendString WINELIB_NAME_AW(mciSendString)

UINT16		WINAPI	mciGetDeviceID16(LPCSTR);
UINT		WINAPI	mciGetDeviceIDA(LPCSTR);
UINT		WINAPI	mciGetDeviceIDW(LPCWSTR);
#define 		mciGetDeviceID WINELIB_NAME_AW(mciGetDeviceID)

UINT16		WINAPI	mciGetDeviceIDFromElementID16(DWORD,LPCSTR);

BOOL16		WINAPI	mciGetErrorString16 (DWORD,LPSTR,UINT16);
BOOL		WINAPI	mciGetErrorStringA(DWORD,LPSTR,UINT);
BOOL		WINAPI	mciGetErrorStringW(DWORD,LPWSTR,UINT);
#define 		mciGetErrorString WINELIB_NAME_AW(mciGetErrorString)

BOOL16		WINAPI	mciSetYieldProc16(UINT16,YIELDPROC,DWORD);
BOOL		WINAPI	mciSetYieldProc(UINT,YIELDPROC,DWORD);

HTASK16		WINAPI	mciGetCreatorTask16(UINT16);
HTASK		WINAPI	mciGetCreatorTask(UINT);

YIELDPROC	WINAPI	mciGetYieldProc16(UINT16,DWORD*);
YIELDPROC	WINAPI	mciGetYieldProc(UINT,DWORD*);

BOOL            WINAPI  mciDriverNotify(HWND hwndCallback, UINT uDeviceID, UINT uStatus);
UINT            WINAPI  mciDriverYield(UINT uDeviceID);
BOOL            WINAPI  mciExecute(LPCSTR pszCommand);
BOOL            WINAPI  mciFreeCommandResource(UINT uTable);
MCIDEVICEID     WINAPI  mciGetDeviceIDFromElementIDW(DWORD dwElementID, LPCWSTR lpstrType);
DWORD           WINAPI  mciGetDriverData(UINT uDeviceID);
UINT            WINAPI  mciLoadCommandResource(HINSTANCE hInstance, LPCWSTR lpResName, UINT uType);
BOOL            WINAPI  mciSetDriverData(UINT uDeviceID, DWORD dwData);

#define MCIERR_INVALID_DEVICE_ID        (MCIERR_BASE + 1)
#define MCIERR_UNRECOGNIZED_KEYWORD     (MCIERR_BASE + 3)
#define MCIERR_UNRECOGNIZED_COMMAND     (MCIERR_BASE + 5)
#define MCIERR_HARDWARE                 (MCIERR_BASE + 6)
#define MCIERR_INVALID_DEVICE_NAME      (MCIERR_BASE + 7)
#define MCIERR_OUT_OF_MEMORY            (MCIERR_BASE + 8)
#define MCIERR_DEVICE_OPEN              (MCIERR_BASE + 9)
#define MCIERR_CANNOT_LOAD_DRIVER       (MCIERR_BASE + 10)
#define MCIERR_MISSING_COMMAND_STRING   (MCIERR_BASE + 11)
#define MCIERR_PARAM_OVERFLOW           (MCIERR_BASE + 12)
#define MCIERR_MISSING_STRING_ARGUMENT  (MCIERR_BASE + 13)
#define MCIERR_BAD_INTEGER              (MCIERR_BASE + 14)
#define MCIERR_PARSER_INTERNAL          (MCIERR_BASE + 15)
#define MCIERR_DRIVER_INTERNAL          (MCIERR_BASE + 16)
#define MCIERR_MISSING_PARAMETER        (MCIERR_BASE + 17)
#define MCIERR_UNSUPPORTED_FUNCTION     (MCIERR_BASE + 18)
#define MCIERR_FILE_NOT_FOUND           (MCIERR_BASE + 19)
#define MCIERR_DEVICE_NOT_READY         (MCIERR_BASE + 20)
#define MCIERR_INTERNAL                 (MCIERR_BASE + 21)
#define MCIERR_DRIVER                   (MCIERR_BASE + 22)
#define MCIERR_CANNOT_USE_ALL           (MCIERR_BASE + 23)
#define MCIERR_MULTIPLE                 (MCIERR_BASE + 24)
#define MCIERR_EXTENSION_NOT_FOUND      (MCIERR_BASE + 25)
#define MCIERR_OUTOFRANGE               (MCIERR_BASE + 26)
#define MCIERR_FLAGS_NOT_COMPATIBLE     (MCIERR_BASE + 28)
#define MCIERR_FILE_NOT_SAVED           (MCIERR_BASE + 30)
#define MCIERR_DEVICE_TYPE_REQUIRED     (MCIERR_BASE + 31)
#define MCIERR_DEVICE_LOCKED            (MCIERR_BASE + 32)
#define MCIERR_DUPLICATE_ALIAS          (MCIERR_BASE + 33)
#define MCIERR_BAD_CONSTANT             (MCIERR_BASE + 34)
#define MCIERR_MUST_USE_SHAREABLE       (MCIERR_BASE + 35)
#define MCIERR_MISSING_DEVICE_NAME      (MCIERR_BASE + 36)
#define MCIERR_BAD_TIME_FORMAT          (MCIERR_BASE + 37)
#define MCIERR_NO_CLOSING_QUOTE         (MCIERR_BASE + 38)
#define MCIERR_DUPLICATE_FLAGS          (MCIERR_BASE + 39)
#define MCIERR_INVALID_FILE             (MCIERR_BASE + 40)
#define MCIERR_NULL_PARAMETER_BLOCK     (MCIERR_BASE + 41)
#define MCIERR_UNNAMED_RESOURCE         (MCIERR_BASE + 42)
#define MCIERR_NEW_REQUIRES_ALIAS       (MCIERR_BASE + 43)
#define MCIERR_NOTIFY_ON_AUTO_OPEN      (MCIERR_BASE + 44)
#define MCIERR_NO_ELEMENT_ALLOWED       (MCIERR_BASE + 45)
#define MCIERR_NONAPPLICABLE_FUNCTION   (MCIERR_BASE + 46)
#define MCIERR_ILLEGAL_FOR_AUTO_OPEN    (MCIERR_BASE + 47)
#define MCIERR_FILENAME_REQUIRED        (MCIERR_BASE + 48)
#define MCIERR_EXTRA_CHARACTERS         (MCIERR_BASE + 49)
#define MCIERR_DEVICE_NOT_INSTALLED     (MCIERR_BASE + 50)
#define MCIERR_GET_CD                   (MCIERR_BASE + 51)
#define MCIERR_SET_CD                   (MCIERR_BASE + 52)
#define MCIERR_SET_DRIVE                (MCIERR_BASE + 53)
#define MCIERR_DEVICE_LENGTH            (MCIERR_BASE + 54)
#define MCIERR_DEVICE_ORD_LENGTH        (MCIERR_BASE + 55)
#define MCIERR_NO_INTEGER               (MCIERR_BASE + 56)

#define MCIERR_WAVE_OUTPUTSINUSE        (MCIERR_BASE + 64)
#define MCIERR_WAVE_SETOUTPUTINUSE      (MCIERR_BASE + 65)
#define MCIERR_WAVE_INPUTSINUSE         (MCIERR_BASE + 66)
#define MCIERR_WAVE_SETINPUTINUSE       (MCIERR_BASE + 67)
#define MCIERR_WAVE_OUTPUTUNSPECIFIED   (MCIERR_BASE + 68)
#define MCIERR_WAVE_INPUTUNSPECIFIED    (MCIERR_BASE + 69)
#define MCIERR_WAVE_OUTPUTSUNSUITABLE   (MCIERR_BASE + 70)
#define MCIERR_WAVE_SETOUTPUTUNSUITABLE (MCIERR_BASE + 71)
#define MCIERR_WAVE_INPUTSUNSUITABLE    (MCIERR_BASE + 72)
#define MCIERR_WAVE_SETINPUTUNSUITABLE  (MCIERR_BASE + 73)

#define MCIERR_SEQ_DIV_INCOMPATIBLE     (MCIERR_BASE + 80)
#define MCIERR_SEQ_PORT_INUSE           (MCIERR_BASE + 81)
#define MCIERR_SEQ_PORT_NONEXISTENT     (MCIERR_BASE + 82)
#define MCIERR_SEQ_PORT_MAPNODEVICE     (MCIERR_BASE + 83)
#define MCIERR_SEQ_PORT_MISCERROR       (MCIERR_BASE + 84)
#define MCIERR_SEQ_TIMER                (MCIERR_BASE + 85)
#define MCIERR_SEQ_PORTUNSPECIFIED      (MCIERR_BASE + 86)
#define MCIERR_SEQ_NOMIDIPRESENT        (MCIERR_BASE + 87)

#define MCIERR_NO_WINDOW                (MCIERR_BASE + 90)
#define MCIERR_CREATEWINDOW             (MCIERR_BASE + 91)
#define MCIERR_FILE_READ                (MCIERR_BASE + 92)
#define MCIERR_FILE_WRITE               (MCIERR_BASE + 93)

#define MCIERR_NO_IDENTITY              (MCIERR_BASE + 94)

#define MCIERR_CUSTOM_DRIVER_BASE       (MCIERR_BASE + 256)

#define MCI_OPEN_DRIVER			0x0801
#define MCI_CLOSE_DRIVER		0x0802
#define MCI_OPEN			0x0803
#define MCI_CLOSE			0x0804
#define MCI_ESCAPE                      0x0805
#define MCI_PLAY                        0x0806
#define MCI_SEEK                        0x0807
#define MCI_STOP                        0x0808
#define MCI_PAUSE                       0x0809
#define MCI_INFO                        0x080A
#define MCI_GETDEVCAPS                  0x080B
#define MCI_SPIN                        0x080C
#define MCI_SET                         0x080D
#define MCI_STEP                        0x080E
#define MCI_RECORD                      0x080F
#define MCI_SYSINFO                     0x0810
#define MCI_BREAK                       0x0811
#define MCI_SOUND                       0x0812
#define MCI_SAVE                        0x0813
#define MCI_STATUS                      0x0814
#define MCI_CUE                         0x0830
#define MCI_REALIZE                     0x0840
#define MCI_WINDOW                      0x0841
#define MCI_PUT                         0x0842
#define MCI_WHERE                       0x0843
#define MCI_FREEZE                      0x0844
#define MCI_UNFREEZE                    0x0845
#define MCI_LOAD                        0x0850
#define MCI_CUT                         0x0851
#define MCI_COPY                        0x0852
#define MCI_PASTE                       0x0853
#define MCI_UPDATE                      0x0854
#define MCI_RESUME                      0x0855
#define MCI_DELETE                      0x0856

#define MCI_USER_MESSAGES               (0x400 + DRV_MCI_FIRST)

#define MCI_ALL_DEVICE_ID               0xFFFF

#define MCI_DEVTYPE_VCR                 (MCI_STRING_OFFSET + 1)
#define MCI_DEVTYPE_VIDEODISC           (MCI_STRING_OFFSET + 2)
#define MCI_DEVTYPE_OVERLAY             (MCI_STRING_OFFSET + 3)
#define MCI_DEVTYPE_CD_AUDIO            (MCI_STRING_OFFSET + 4)
#define MCI_DEVTYPE_DAT                 (MCI_STRING_OFFSET + 5)
#define MCI_DEVTYPE_SCANNER             (MCI_STRING_OFFSET + 6)
#define MCI_DEVTYPE_ANIMATION           (MCI_STRING_OFFSET + 7)
#define MCI_DEVTYPE_DIGITAL_VIDEO       (MCI_STRING_OFFSET + 8)
#define MCI_DEVTYPE_OTHER               (MCI_STRING_OFFSET + 9)
#define MCI_DEVTYPE_WAVEFORM_AUDIO      (MCI_STRING_OFFSET + 10)
#define MCI_DEVTYPE_SEQUENCER           (MCI_STRING_OFFSET + 11)

#define MCI_DEVTYPE_FIRST               MCI_DEVTYPE_VCR
#define MCI_DEVTYPE_LAST                MCI_DEVTYPE_SEQUENCER

#define MCI_MODE_NOT_READY              (MCI_STRING_OFFSET + 12)
#define MCI_MODE_STOP                   (MCI_STRING_OFFSET + 13)
#define MCI_MODE_PLAY                   (MCI_STRING_OFFSET + 14)
#define MCI_MODE_RECORD                 (MCI_STRING_OFFSET + 15)
#define MCI_MODE_SEEK                   (MCI_STRING_OFFSET + 16)
#define MCI_MODE_PAUSE                  (MCI_STRING_OFFSET + 17)
#define MCI_MODE_OPEN                   (MCI_STRING_OFFSET + 18)

#define MCI_FORMAT_MILLISECONDS         0
#define MCI_FORMAT_HMS                  1
#define MCI_FORMAT_MSF                  2
#define MCI_FORMAT_FRAMES               3
#define MCI_FORMAT_SMPTE_24             4
#define MCI_FORMAT_SMPTE_25             5
#define MCI_FORMAT_SMPTE_30             6
#define MCI_FORMAT_SMPTE_30DROP         7
#define MCI_FORMAT_BYTES                8
#define MCI_FORMAT_SAMPLES              9
#define MCI_FORMAT_TMSF                 10

#define MCI_MSF_MINUTE(msf)             ((BYTE)(msf))
#define MCI_MSF_SECOND(msf)             ((BYTE)(((WORD)(msf)) >> 8))
#define MCI_MSF_FRAME(msf)              ((BYTE)((msf)>>16))

#define MCI_MAKE_MSF(m, s, f)           ((DWORD)(((BYTE)(m) | \
                                                  ((WORD)(s)<<8)) | \
                                                 (((DWORD)(BYTE)(f))<<16)))

#define MCI_TMSF_TRACK(tmsf)            ((BYTE)(tmsf))
#define MCI_TMSF_MINUTE(tmsf)           ((BYTE)(((WORD)(tmsf)) >> 8))
#define MCI_TMSF_SECOND(tmsf)           ((BYTE)((tmsf)>>16))
#define MCI_TMSF_FRAME(tmsf)            ((BYTE)((tmsf)>>24))

#define MCI_MAKE_TMSF(t, m, s, f)       ((DWORD)(((BYTE)(t) | \
                                                  ((WORD)(m)<<8)) | \
                                                 (((DWORD)(BYTE)(s) | \
                                                   ((WORD)(f)<<8))<<16)))

#define MCI_HMS_HOUR(hms)               ((BYTE)(hms))
#define MCI_HMS_MINUTE(hms)             ((BYTE)(((WORD)(hms)) >> 8))
#define MCI_HMS_SECOND(hms)             ((BYTE)((hms)>>16))

#define MCI_MAKE_HMS(h, m, s)           ((DWORD)(((BYTE)(h) | \
                                                  ((WORD)(m)<<8)) | \
                                                 (((DWORD)(BYTE)(s))<<16)))

#define MCI_NOTIFY_SUCCESSFUL           0x0001
#define MCI_NOTIFY_SUPERSEDED           0x0002
#define MCI_NOTIFY_ABORTED              0x0004
#define MCI_NOTIFY_FAILURE              0x0008

#define MCI_NOTIFY                      0x00000001L
#define MCI_WAIT                        0x00000002L
#define MCI_FROM                        0x00000004L
#define MCI_TO                          0x00000008L
#define MCI_TRACK                       0x00000010L

#define MCI_OPEN_SHAREABLE              0x00000100L
#define MCI_OPEN_ELEMENT                0x00000200L
#define MCI_OPEN_ALIAS                  0x00000400L
#define MCI_OPEN_ELEMENT_ID             0x00000800L
#define MCI_OPEN_TYPE_ID                0x00001000L
#define MCI_OPEN_TYPE                   0x00002000L

#define MCI_SEEK_TO_START               0x00000100L
#define MCI_SEEK_TO_END                 0x00000200L

#define MCI_STATUS_ITEM                 0x00000100L
#define MCI_STATUS_START                0x00000200L

#define MCI_STATUS_LENGTH               0x00000001L
#define MCI_STATUS_POSITION             0x00000002L
#define MCI_STATUS_NUMBER_OF_TRACKS     0x00000003L
#define MCI_STATUS_MODE                 0x00000004L
#define MCI_STATUS_MEDIA_PRESENT        0x00000005L
#define MCI_STATUS_TIME_FORMAT          0x00000006L
#define MCI_STATUS_READY                0x00000007L
#define MCI_STATUS_CURRENT_TRACK        0x00000008L

#define MCI_INFO_PRODUCT                0x00000100L
#define MCI_INFO_FILE                   0x00000200L
#define MCI_INFO_MEDIA_UPC              0x00000400L
#define MCI_INFO_MEDIA_IDENTITY         0x00000800L
#define MCI_INFO_NAME                   0x00001000L
#define MCI_INFO_COPYRIGHT              0x00002000L


#define MCI_GETDEVCAPS_ITEM             0x00000100L

#define MCI_GETDEVCAPS_CAN_RECORD       0x00000001L
#define MCI_GETDEVCAPS_HAS_AUDIO        0x00000002L
#define MCI_GETDEVCAPS_HAS_VIDEO        0x00000003L
#define MCI_GETDEVCAPS_DEVICE_TYPE      0x00000004L
#define MCI_GETDEVCAPS_USES_FILES       0x00000005L
#define MCI_GETDEVCAPS_COMPOUND_DEVICE  0x00000006L
#define MCI_GETDEVCAPS_CAN_EJECT        0x00000007L
#define MCI_GETDEVCAPS_CAN_PLAY         0x00000008L
#define MCI_GETDEVCAPS_CAN_SAVE         0x00000009L

#define MCI_SYSINFO_QUANTITY            0x00000100L
#define MCI_SYSINFO_OPEN                0x00000200L
#define MCI_SYSINFO_NAME                0x00000400L
#define MCI_SYSINFO_INSTALLNAME         0x00000800L

#define MCI_SET_DOOR_OPEN               0x00000100L
#define MCI_SET_DOOR_CLOSED             0x00000200L
#define MCI_SET_TIME_FORMAT             0x00000400L
#define MCI_SET_AUDIO                   0x00000800L
#define MCI_SET_VIDEO                   0x00001000L
#define MCI_SET_ON                      0x00002000L
#define MCI_SET_OFF                     0x00004000L

#define MCI_SET_AUDIO_ALL               0x00000000L
#define MCI_SET_AUDIO_LEFT              0x00000001L
#define MCI_SET_AUDIO_RIGHT             0x00000002L

#define MCI_BREAK_KEY                   0x00000100L
#define MCI_BREAK_HWND                  0x00000200L
#define MCI_BREAK_OFF                   0x00000400L

#define MCI_RECORD_INSERT               0x00000100L
#define MCI_RECORD_OVERWRITE            0x00000200L

#define MCI_SOUND_NAME                  0x00000100L

#define MCI_SAVE_FILE                   0x00000100L

#define MCI_LOAD_FILE                   0x00000100L

typedef struct {
	DWORD   dwCallback;
} MCI_GENERIC_PARMS, *LPMCI_GENERIC_PARMS;

typedef struct {
	DWORD	dwCallback;
	WORD	wDeviceID;
	WORD	wReserved0;
	LPSTR	lpstrDeviceType;
	LPSTR	lpstrElementName;
	LPSTR	lpstrAlias;
} MCI_OPEN_PARMS16, *LPMCI_OPEN_PARMS16;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCSTR		lpstrDeviceType;
	LPCSTR		lpstrElementName;
	LPCSTR		lpstrAlias;
} MCI_OPEN_PARMSA, *LPMCI_OPEN_PARMSA;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCWSTR		lpstrDeviceType;
	LPCWSTR		lpstrElementName;
	LPCWSTR		lpstrAlias;
} MCI_OPEN_PARMSW, *LPMCI_OPEN_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_OPEN_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_OPEN_PARMS)

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrom;
	DWORD   dwTo;
} MCI_PLAY_PARMS, *LPMCI_PLAY_PARMS;

typedef struct {
	DWORD   dwCallback;
	DWORD   dwTo;
} MCI_SEEK_PARMS, *LPMCI_SEEK_PARMS;

typedef struct {
	DWORD   dwCallback;
	DWORD   dwReturn;
	DWORD   dwItem;
	DWORD   dwTrack;
} MCI_STATUS_PARMS, *LPMCI_STATUS_PARMS;

typedef struct {
	DWORD   dwCallback;
	LPSTR   lpstrReturn;
	DWORD   dwRetSize;
} MCI_INFO_PARMS16, *LPMCI_INFO_PARMS16;

typedef struct {
	DWORD   dwCallback;
	LPSTR   lpstrReturn;
	DWORD   dwRetSize;
} MCI_INFO_PARMSA, *LPMCI_INFO_PARMSA;

typedef struct {
	DWORD   dwCallback;
	LPSTR   lpstrReturn;
	DWORD   dwRetSize;
} MCI_INFO_PARMSW, *LPMCI_INFO_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_INFO_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_INFO_PARMS)

typedef struct {
	DWORD   dwCallback;
	DWORD   dwReturn;
	DWORD   dwItem;
} MCI_GETDEVCAPS_PARMS, *LPMCI_GETDEVCAPS_PARMS;

typedef struct {
	DWORD	dwCallback;
	LPSTR	lpstrReturn;
	DWORD	dwRetSize;
	DWORD	dwNumber;
	WORD	wDeviceType;
	WORD	wReserved0;
} MCI_SYSINFO_PARMS16, *LPMCI_SYSINFO_PARMS16;

typedef struct {
	DWORD	dwCallback;
	LPSTR	lpstrReturn;
	DWORD	dwRetSize;
	DWORD	dwNumber;
	UINT	wDeviceType;
} MCI_SYSINFO_PARMSA, *LPMCI_SYSINFO_PARMSA;

typedef struct {
	DWORD	dwCallback;
	LPWSTR	lpstrReturn;
	DWORD	dwRetSize;
	DWORD	dwNumber;
	UINT	wDeviceType;
} MCI_SYSINFO_PARMSW, *LPMCI_SYSINFO_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_SYSINFO_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_SYSINFO_PARMS)

typedef struct {
	DWORD   dwCallback;
	DWORD   dwTimeFormat;
	DWORD   dwAudio;
} MCI_SET_PARMS, *LPMCI_SET_PARMS;

typedef struct {
	DWORD	dwCallback;
	UINT16	nVirtKey;
	WORD	wReserved0;
	HWND16	hwndBreak;
	WORD	wReserved1;
} MCI_BREAK_PARMS16, *LPMCI_BREAK_PARMS16;

typedef struct {
	DWORD	dwCallback;
	INT	nVirtKey;
	HWND	hwndBreak;
} MCI_BREAK_PARMS, *LPMCI_BREAK_PARMS;


typedef struct {
	DWORD   dwCallback;
	LPCSTR  lpstrSoundName;
} MCI_SOUND_PARMS, *LPMCI_SOUND_PARMS;

typedef struct {
	DWORD   dwCallback;
	LPCSTR  lpfilename;
} MCI_SAVE_PARMS, *LPMCI_SAVE_PARMS;

typedef struct {
	DWORD	dwCallback;
	LPCSTR	lpfilename;
} MCI_LOAD_PARMS16, *LPMCI_LOAD_PARMS16;

typedef struct {
	DWORD	dwCallback;
	LPCSTR	lpfilename;
} MCI_LOAD_PARMSA, *LPMCI_LOAD_PARMSA;

typedef struct {
	DWORD	dwCallback;
	LPCWSTR	lpfilename;
} MCI_LOAD_PARMSW, *LPMCI_LOAD_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_LOAD_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_LOAD_PARMS)

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrom;
	DWORD   dwTo;
} MCI_RECORD_PARMS, *LPMCI_RECORD_PARMS;

#define MCI_CDA_STATUS_TYPE_TRACK 	0x00004001

#define MCI_CDA_TRACK_AUDIO		(MCI_CD_OFFSET + 0)
#define MCI_CDA_TRACK_OTHER		(MCI_CD_OFFSET + 1)

#define MCI_VD_MODE_PARK                (MCI_VD_OFFSET + 1)

#define MCI_VD_MEDIA_CLV                (MCI_VD_OFFSET + 2)
#define MCI_VD_MEDIA_CAV                (MCI_VD_OFFSET + 3)
#define MCI_VD_MEDIA_OTHER              (MCI_VD_OFFSET + 4)

#define MCI_VD_FORMAT_TRACK             0x4001

#define MCI_VD_PLAY_REVERSE             0x00010000L
#define MCI_VD_PLAY_FAST                0x00020000L
#define MCI_VD_PLAY_SPEED               0x00040000L
#define MCI_VD_PLAY_SCAN                0x00080000L
#define MCI_VD_PLAY_SLOW                0x00100000L

#define MCI_VD_SEEK_REVERSE             0x00010000L

#define MCI_VD_STATUS_SPEED             0x00004002L
#define MCI_VD_STATUS_FORWARD           0x00004003L
#define MCI_VD_STATUS_MEDIA_TYPE        0x00004004L
#define MCI_VD_STATUS_SIDE              0x00004005L
#define MCI_VD_STATUS_DISC_SIZE         0x00004006L

#define MCI_VD_GETDEVCAPS_CLV           0x00010000L
#define MCI_VD_GETDEVCAPS_CAV           0x00020000L

#define MCI_VD_SPIN_UP                  0x00010000L
#define MCI_VD_SPIN_DOWN                0x00020000L

#define MCI_VD_GETDEVCAPS_CAN_REVERSE   0x00004002L
#define MCI_VD_GETDEVCAPS_FAST_RATE     0x00004003L
#define MCI_VD_GETDEVCAPS_SLOW_RATE     0x00004004L
#define MCI_VD_GETDEVCAPS_NORMAL_RATE   0x00004005L

#define MCI_VD_STEP_FRAMES              0x00010000L
#define MCI_VD_STEP_REVERSE             0x00020000L

#define MCI_VD_ESCAPE_STRING            0x00000100L

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrom;
	DWORD   dwTo;
	DWORD   dwSpeed;
} MCI_VD_PLAY_PARMS, *LPMCI_VD_PLAY_PARMS;

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrames;
} MCI_VD_STEP_PARMS, *LPMCI_VD_STEP_PARMS;

typedef struct {
	DWORD	dwCallback;
	LPCSTR	lpstrCommand;
} MCI_VD_ESCAPE_PARMS16, *LPMCI_VD_ESCAPE_PARMS16;

typedef struct {
	DWORD	dwCallback;
	LPCSTR	lpstrCommand;
} MCI_VD_ESCAPE_PARMSA, *LPMCI_VD_ESCAPE_PARMSA;

typedef struct {
	DWORD	dwCallback;
	LPCWSTR	lpstrCommand;
} MCI_VD_ESCAPE_PARMSW, *LPMCI_VD_ESCAPE_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_VD_ESCAPE_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_VD_ESCAPE_PARMS)

#define MCI_WAVE_OPEN_BUFFER            0x00010000L

#define MCI_WAVE_SET_FORMATTAG          0x00010000L
#define MCI_WAVE_SET_CHANNELS           0x00020000L
#define MCI_WAVE_SET_SAMPLESPERSEC      0x00040000L
#define MCI_WAVE_SET_AVGBYTESPERSEC     0x00080000L
#define MCI_WAVE_SET_BLOCKALIGN         0x00100000L
#define MCI_WAVE_SET_BITSPERSAMPLE      0x00200000L

#define MCI_WAVE_INPUT                  0x00400000L
#define MCI_WAVE_OUTPUT                 0x00800000L

#define MCI_WAVE_STATUS_FORMATTAG       0x00004001L
#define MCI_WAVE_STATUS_CHANNELS        0x00004002L
#define MCI_WAVE_STATUS_SAMPLESPERSEC   0x00004003L
#define MCI_WAVE_STATUS_AVGBYTESPERSEC  0x00004004L
#define MCI_WAVE_STATUS_BLOCKALIGN      0x00004005L
#define MCI_WAVE_STATUS_BITSPERSAMPLE   0x00004006L
#define MCI_WAVE_STATUS_LEVEL           0x00004007L

#define MCI_WAVE_SET_ANYINPUT           0x04000000L
#define MCI_WAVE_SET_ANYOUTPUT          0x08000000L

#define MCI_WAVE_GETDEVCAPS_INPUTS      0x00004001L
#define MCI_WAVE_GETDEVCAPS_OUTPUTS     0x00004002L

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID16	wDeviceID;
	WORD		wReserved0;
	SEGPTR		lpstrDeviceType;
	SEGPTR		lpstrElementName;
	SEGPTR		lpstrAlias;
	DWORD		dwBufferSeconds;
} MCI_WAVE_OPEN_PARMS16, *LPMCI_WAVE_OPEN_PARMS16;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCSTR		lpstrDeviceType;
	LPCSTR		lpstrElementName;
	LPCSTR		lpstrAlias;
	DWORD   	dwBufferSeconds;
} MCI_WAVE_OPEN_PARMSA, *LPMCI_WAVE_OPEN_PARMSA;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCWSTR		lpstrDeviceType;
	LPCWSTR		lpstrElementName;
	LPCWSTR		lpstrAlias;
	DWORD   	dwBufferSeconds;
} MCI_WAVE_OPEN_PARMSW, *LPMCI_WAVE_OPEN_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_WAVE_OPEN_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_WAVE_OPEN_PARMS)

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrom;
	DWORD   dwTo;
} MCI_WAVE_DELETE_PARMS, *LPMCI_WAVE_DELETE_PARMS;

typedef struct {
	DWORD	dwCallback;
	DWORD	dwTimeFormat;
	DWORD	dwAudio;
	UINT16	wInput;
	UINT16	wReserved0;
	UINT16	wOutput;
	UINT16	wReserved1;
	UINT16	wFormatTag;
	UINT16	wReserved2;
	UINT16	nChannels;
	UINT16	wReserved3;
	DWORD	nSamplesPerSec;
	DWORD	nAvgBytesPerSec;
	UINT16	nBlockAlign;
	UINT16	wReserved4;
	UINT16	wBitsPerSample;
	UINT16	wReserved5;
} MCI_WAVE_SET_PARMS16, * LPMCI_WAVE_SET_PARMS16;

typedef struct {
	DWORD	dwCallback;
	DWORD	dwTimeFormat;
	DWORD	dwAudio;
	UINT	wInput;
	UINT	wOutput;
	UINT	wFormatTag;
	UINT	nChannels;
	DWORD	nSamplesPerSec;
	DWORD	nAvgBytesPerSec;
	UINT	nBlockAlign;
	UINT	wBitsPerSample;
} MCI_WAVE_SET_PARMS, * LPMCI_WAVE_SET_PARMS;


#define     MCI_SEQ_DIV_PPQN            (0 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_24        (1 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_25        (2 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_30DROP    (3 + MCI_SEQ_OFFSET)
#define     MCI_SEQ_DIV_SMPTE_30        (4 + MCI_SEQ_OFFSET)

#define     MCI_SEQ_FORMAT_SONGPTR      0x4001
#define     MCI_SEQ_FILE                0x4002
#define     MCI_SEQ_MIDI                0x4003
#define     MCI_SEQ_SMPTE               0x4004
#define     MCI_SEQ_NONE                65533

#define MCI_SEQ_STATUS_TEMPO            0x00004002L
#define MCI_SEQ_STATUS_PORT             0x00004003L
#define MCI_SEQ_STATUS_SLAVE            0x00004007L
#define MCI_SEQ_STATUS_MASTER           0x00004008L
#define MCI_SEQ_STATUS_OFFSET           0x00004009L
#define MCI_SEQ_STATUS_DIVTYPE          0x0000400AL

#define MCI_SEQ_SET_TEMPO               0x00010000L
#define MCI_SEQ_SET_PORT                0x00020000L
#define MCI_SEQ_SET_SLAVE               0x00040000L
#define MCI_SEQ_SET_MASTER              0x00080000L
#define MCI_SEQ_SET_OFFSET              0x01000000L

typedef struct {
	DWORD   dwCallback;
	DWORD   dwTimeFormat;
	DWORD   dwAudio;
	DWORD   dwTempo;
	DWORD   dwPort;
	DWORD   dwSlave;
	DWORD   dwMaster;
	DWORD   dwOffset;
} MCI_SEQ_SET_PARMS, *LPMCI_SEQ_SET_PARMS;

#define MCI_ANIM_OPEN_WS                0x00010000L
#define MCI_ANIM_OPEN_PARENT            0x00020000L
#define MCI_ANIM_OPEN_NOSTATIC          0x00040000L

#define MCI_ANIM_PLAY_SPEED             0x00010000L
#define MCI_ANIM_PLAY_REVERSE           0x00020000L
#define MCI_ANIM_PLAY_FAST              0x00040000L
#define MCI_ANIM_PLAY_SLOW              0x00080000L
#define MCI_ANIM_PLAY_SCAN              0x00100000L

#define MCI_ANIM_STEP_REVERSE           0x00010000L
#define MCI_ANIM_STEP_FRAMES            0x00020000L

#define MCI_ANIM_STATUS_SPEED           0x00004001L
#define MCI_ANIM_STATUS_FORWARD         0x00004002L
#define MCI_ANIM_STATUS_HWND            0x00004003L
#define MCI_ANIM_STATUS_HPAL            0x00004004L
#define MCI_ANIM_STATUS_STRETCH         0x00004005L

#define MCI_ANIM_INFO_TEXT              0x00010000L

#define MCI_ANIM_GETDEVCAPS_CAN_REVERSE 0x00004001L
#define MCI_ANIM_GETDEVCAPS_FAST_RATE   0x00004002L
#define MCI_ANIM_GETDEVCAPS_SLOW_RATE   0x00004003L
#define MCI_ANIM_GETDEVCAPS_NORMAL_RATE 0x00004004L
#define MCI_ANIM_GETDEVCAPS_PALETTES    0x00004006L
#define MCI_ANIM_GETDEVCAPS_CAN_STRETCH 0x00004007L
#define MCI_ANIM_GETDEVCAPS_MAX_WINDOWS 0x00004008L

#define MCI_ANIM_REALIZE_NORM           0x00010000L
#define MCI_ANIM_REALIZE_BKGD           0x00020000L

#define MCI_ANIM_WINDOW_HWND            0x00010000L
#define MCI_ANIM_WINDOW_STATE           0x00040000L
#define MCI_ANIM_WINDOW_TEXT            0x00080000L
#define MCI_ANIM_WINDOW_ENABLE_STRETCH  0x00100000L
#define MCI_ANIM_WINDOW_DISABLE_STRETCH 0x00200000L

#define MCI_ANIM_WINDOW_DEFAULT         0x00000000L

#define MCI_ANIM_RECT                   0x00010000L
#define MCI_ANIM_PUT_SOURCE             0x00020000L
#define MCI_ANIM_PUT_DESTINATION        0x00040000L

#define MCI_ANIM_WHERE_SOURCE           0x00020000L
#define MCI_ANIM_WHERE_DESTINATION      0x00040000L

#define MCI_ANIM_UPDATE_HDC             0x00020000L

typedef struct {
	DWORD   dwCallback;
	UINT16  wDeviceID;
	UINT16  wReserved0;
	SEGPTR  lpstrDeviceType;
	SEGPTR  lpstrElementName;
	SEGPTR  lpstrAlias;
	DWORD   dwStyle;
	HWND16  hWndParent;
	UINT16  wReserved1;
} MCI_ANIM_OPEN_PARMS16, *LPMCI_ANIM_OPEN_PARMS16;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCSTR		lpstrDeviceType;
	LPCSTR		lpstrElementName;
	LPCSTR		lpstrAlias;
	DWORD		dwStyle;
	HWND		hWndParent;
} MCI_ANIM_OPEN_PARMSA, *LPMCI_ANIM_OPEN_PARMSA;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCWSTR		lpstrDeviceType;
	LPCWSTR		lpstrElementName;
	LPCWSTR		lpstrAlias;
	DWORD		dwStyle;
	HWND		hWndParent;
} MCI_ANIM_OPEN_PARMSW, *LPMCI_ANIM_OPEN_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_ANIM_OPEN_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_ANIM_OPEN_PARMS)

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrom;
	DWORD   dwTo;
	DWORD   dwSpeed;
} MCI_ANIM_PLAY_PARMS, *LPMCI_ANIM_PLAY_PARMS;

typedef struct {
	DWORD   dwCallback;
	DWORD   dwFrames;
} MCI_ANIM_STEP_PARMS, *LPMCI_ANIM_STEP_PARMS;

typedef struct {
	DWORD	dwCallback;
	HWND16	hWnd;
	WORD	wReserved1;
	WORD	nCmdShow;
	WORD	wReserved2;
	LPCSTR	lpstrText;
} MCI_ANIM_WINDOW_PARMS16, *LPMCI_ANIM_WINDOW_PARMS16;

typedef struct {
	DWORD	dwCallback;
	HWND	hWnd;
	UINT	nCmdShow;
	LPCSTR	lpstrText;
} MCI_ANIM_WINDOW_PARMSA, *LPMCI_ANIM_WINDOW_PARMSA;

typedef struct {
	DWORD	dwCallback;
	HWND	hWnd;
	UINT	nCmdShow;
	LPCWSTR	lpstrText;
} MCI_ANIM_WINDOW_PARMSW, *LPMCI_ANIM_WINDOW_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_ANIM_WINDOW_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_ANIM_WINDOW_PARMS)

typedef struct {
	DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
	POINT16 ptOffset;
	POINT16 ptExtent;
#else   /* ifdef MCI_USE_OFFEXT */
	RECT16  rc;
#endif  /* ifdef MCI_USE_OFFEXT */
} MCI_ANIM_RECT_PARMS16, *LPMCI_ANIM_RECT_PARMS16;

typedef struct {
	DWORD	dwCallback;
#ifdef MCI_USE_OFFEXT
	POINT	ptOffset;
	POINT	ptExtent;
#else   /* ifdef MCI_USE_OFFEXT */
	RECT	rc;
#endif  /* ifdef MCI_USE_OFFEXT */
} MCI_ANIM_RECT_PARMS, *LPMCI_ANIM_RECT_PARMS;


typedef struct {
	DWORD   dwCallback;
	RECT16  rc;
	HDC16   hDC;
} MCI_ANIM_UPDATE_PARMS16, *LPMCI_ANIM_UPDATE_PARMS16;

typedef struct {
	DWORD   dwCallback;
	RECT  rc;
	HDC   hDC;
} MCI_ANIM_UPDATE_PARMS, *LPMCI_ANIM_UPDATE_PARMS;


#define MCI_OVLY_OPEN_WS                0x00010000L
#define MCI_OVLY_OPEN_PARENT            0x00020000L

#define MCI_OVLY_STATUS_HWND            0x00004001L
#define MCI_OVLY_STATUS_STRETCH         0x00004002L

#define MCI_OVLY_INFO_TEXT              0x00010000L

#define MCI_OVLY_GETDEVCAPS_CAN_STRETCH 0x00004001L
#define MCI_OVLY_GETDEVCAPS_CAN_FREEZE  0x00004002L
#define MCI_OVLY_GETDEVCAPS_MAX_WINDOWS 0x00004003L

#define MCI_OVLY_WINDOW_HWND            0x00010000L
#define MCI_OVLY_WINDOW_STATE           0x00040000L
#define MCI_OVLY_WINDOW_TEXT            0x00080000L
#define MCI_OVLY_WINDOW_ENABLE_STRETCH  0x00100000L
#define MCI_OVLY_WINDOW_DISABLE_STRETCH 0x00200000L

#define MCI_OVLY_WINDOW_DEFAULT         0x00000000L

#define MCI_OVLY_RECT                   0x00010000L
#define MCI_OVLY_PUT_SOURCE             0x00020000L
#define MCI_OVLY_PUT_DESTINATION        0x00040000L
#define MCI_OVLY_PUT_FRAME              0x00080000L
#define MCI_OVLY_PUT_VIDEO              0x00100000L

#define MCI_OVLY_WHERE_SOURCE           0x00020000L
#define MCI_OVLY_WHERE_DESTINATION      0x00040000L
#define MCI_OVLY_WHERE_FRAME            0x00080000L
#define MCI_OVLY_WHERE_VIDEO            0x00100000L

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID16	wDeviceID;
	WORD		wReserved0;
	LPCSTR		lpstrDeviceType;
	LPCSTR		lpstrElementName;
	LPCSTR		lpstrAlias;
	DWORD		dwStyle;
	HWND16		hWndParent;
	WORD		wReserved1;
} MCI_OVLY_OPEN_PARMS16, *LPMCI_OVLY_OPEN_PARMS16;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCSTR		lpstrDeviceType;
	LPCSTR		lpstrElementName;
	LPCSTR		lpstrAlias;
	DWORD		dwStyle;
	HWND		hWndParent;
} MCI_OVLY_OPEN_PARMSA, *LPMCI_OVLY_OPEN_PARMSA;

typedef struct {
	DWORD		dwCallback;
	MCIDEVICEID	wDeviceID;
	LPCWSTR		lpstrDeviceType;
	LPCWSTR		lpstrElementName;
	LPCWSTR		lpstrAlias;
	DWORD		dwStyle;
	HWND		hWndParent;
} MCI_OVLY_OPEN_PARMSW, *LPMCI_OVLY_OPEN_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_OVLY_OPEN_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_OVLY_OPEN_PARMS)

typedef struct {
	DWORD	dwCallback;
	HWND16	hWnd;
	WORD	wReserved1;
	UINT16	nCmdShow;
	WORD	wReserved2;
	LPCSTR	lpstrText;
} MCI_OVLY_WINDOW_PARMS16, *LPMCI_OVLY_WINDOW_PARMS16;

typedef struct {
	DWORD	dwCallback;
	HWND	hWnd;
	UINT	nCmdShow;
	LPCSTR	lpstrText;
} MCI_OVLY_WINDOW_PARMSA, *LPMCI_OVLY_WINDOW_PARMSA;

typedef struct {
	DWORD	dwCallback;
	HWND	hWnd;
	UINT	nCmdShow;
	LPCWSTR	lpstrText;
} MCI_OVLY_WINDOW_PARMSW, *LPMCI_OVLY_WINDOW_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_OVLY_WINDOW_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_OVLY_WINDOW_PARMS)

typedef struct {
	DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
	POINT16 ptOffset;
	POINT16 ptExtent;
#else   /* ifdef MCI_USE_OFFEXT */
	RECT16  rc;
#endif  /* ifdef MCI_USE_OFFEXT */
} MCI_OVLY_RECT_PARMS16, *LPMCI_OVLY_RECT_PARMS16;

typedef struct {
	DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
	POINT ptOffset;
	POINT ptExtent;
#else   /* ifdef MCI_USE_OFFEXT */
	RECT  rc;
#endif  /* ifdef MCI_USE_OFFEXT */
} MCI_OVLY_RECT_PARMS, *LPMCI_OVLY_RECT_PARMS;


typedef struct {
	DWORD   dwCallback;
	LPCSTR  lpfilename;
	RECT16  rc;
} MCI_OVLY_SAVE_PARMS16, *LPMCI_OVLY_SAVE_PARMS16;

typedef struct {
	DWORD   dwCallback;
	LPCSTR  lpfilename;
	RECT  rc;
} MCI_OVLY_SAVE_PARMSA, *LPMCI_OVLY_SAVE_PARMSA;

typedef struct {
	DWORD   dwCallback;
	LPCWSTR  lpfilename;
	RECT  rc;
} MCI_OVLY_SAVE_PARMSW, *LPMCI_OVLY_SAVE_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_OVLY_SAVE_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_OVLY_SAVE_PARMS)

typedef struct {
	DWORD	dwCallback;
	LPCSTR	lpfilename;
	RECT16	rc;
} MCI_OVLY_LOAD_PARMS16, *LPMCI_OVLY_LOAD_PARMS16;

typedef struct {
	DWORD	dwCallback;
	LPCSTR	lpfilename;
	RECT	rc;
} MCI_OVLY_LOAD_PARMSA, *LPMCI_OVLY_LOAD_PARMSA;

typedef struct {
	DWORD	dwCallback;
	LPCWSTR	lpfilename;
	RECT	rc;
} MCI_OVLY_LOAD_PARMSW, *LPMCI_OVLY_LOAD_PARMSW;

DECL_WINELIB_TYPE_AW(MCI_OVLY_LOAD_PARMS)
DECL_WINELIB_TYPE_AW(LPMCI_OVLY_LOAD_PARMS)

#include "poppack.h"

#ifdef __cplusplus
}
#endif 

#endif /* __WINE_MMSYSTEM_H */
