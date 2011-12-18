#define INCL_DOS
#define INCL_PM
#define INCL_WIN
#define INCL_VIO
#define INCL_SUB
#define INCL_DOSERRORS
#define INCL_DOSDEVICES
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <MonitorApi.h>

#pragma pack(1)
#define MOUSE_MONSIZE (1024 - 20)
typedef struct _MOUSE_MONIN {    /* mnin */
    USHORT cb;
    BYTE   abReserved[18];
    BYTE   abBuffer[MOUSE_MONSIZE];
} MOUSE_MONIN;

typedef struct _MOUSE_MONOUT {    /* mnout */
    USHORT cb;
    BYTE   abReserved[18];
    BYTE   abBuffer[MOUSE_MONSIZE];
} MOUSE_MONOUT;
#pragma pack(4)

void main(){

    HMONITOR mon;
    APIRET rc = DosMonOpen("MOUSE$",&mon);
printf("rc = %i\n",rc);
    if (rc != 0){
        DosMonClose(mon);
        return;
    }

    MOUSE_MONIN monin;
    monin.cb = sizeof(monin);
    MOUSE_MONOUT monout;
    monout.cb = sizeof(monout);
    rc = DosMonReg(mon,(BYTE *)&monin,(BYTE *)&monout,0,-1);
printf("rc = %i\n",rc);
    if (rc != 0) {
        DosMonClose(mon);
        return;
    }

    DosSetPriority(PRTYS_THREAD,PRTYC_FOREGROUNDSERVER,10,0);

    while(1){
        ULONG buffer[8];
        USHORT bufSize = sizeof(buffer);
        rc = DosMonRead((PBYTE)&monin,DCWW_WAIT,(PBYTE)&buffer,&bufSize);
printf("rc = %i bs =%i\n",rc,bufSize);
        if (rc != 0) break;
        printf(">>%08x %08x %08x %08x %08x %08x %08x %08x\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);
        rc = DosMonWrite((PBYTE)&monout,(PBYTE)&buffer,bufSize);

        if (rc != 0) break;
    }
    DosMonClose(mon);
}