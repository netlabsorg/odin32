#if !defined (OS2_MONITOR_API)
#define OS2_MONITOR_API

#pragma pack(1)

struct MonRecord {
    USHORT MonFlagWord;
    BYTE   XlatedChar;
    BYTE   XlatedScan;
    BYTE   DBCS_Status;
    BYTE   DBCS_Shift;
    USHORT Shift_State;
    ULONG  Milliseconds;
    USHORT KbdDDFlagWord;
    USHORT extra;
};

#define MONSIZE (118)
typedef struct _MONIN {    /* mnin */
    USHORT cb;
    BYTE   abReserved[18];
    BYTE   abBuffer[MONSIZE];
} MONIN;

typedef struct _MONOUT {    /* mnout */
    USHORT cb;
    BYTE   abReserved[18];
    BYTE   abBuffer[MONSIZE];
} MONOUT;
#pragma pack(4)


typedef SHANDLE HMONITOR;       /* hmon */
typedef HMONITOR FAR *PHMONITOR;

#define DosMonOpen              DOSMONOPEN
#define DosMonClose             DOSMONCLOSE
#define DosMonRead              DOSMONREAD
#define DosMonWrite             DOSMONWRITE
#define DosMonReg               DOSMONREG

APIRET16 APIENTRY16  DosMonOpen(PSZ pszDevName, PHMONITOR phmon);
APIRET16 APIENTRY16  DosMonClose(HMONITOR hmon);
APIRET16 APIENTRY16  DosMonReg(HMONITOR hmon, PBYTE pbInBuf, PBYTE pbOutBuf,
                          USHORT fPosition, USHORT usIndex);
APIRET16 APIENTRY16  DosMonRead(PBYTE pbInBuf, USHORT fWait, PBYTE pbDataBuf,
                           PUSHORT pcbData);
APIRET16 APIENTRY16  DosMonWrite(PBYTE pbOutBuf, PBYTE pbDataBuf, USHORT cbData);


#endif
