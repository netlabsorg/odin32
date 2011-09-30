#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  INCL_DOS
#define  INCL_WIN
#define  INCL_BASE
#include <os2.h>
#include "vio32.h"


BOOL filter1F(ULONG *eventType,VIO32EVENTINFO *event,VOID *filterData){
    switch (*eventType){
        case WM_CHAR:
            printf("1WM_CHAR\n");
        break;
        case VIO_CHAR:
            printf("1VIO_CHAR\n");
        break;
        case MON_CHAR:
            printf("1MON_CHAR\n");
        break;
        case WM_MOVE:{
            printf("1WM_MOVE\n");
        } break;
        default:
        break;
    }
    return FALSE;
}

BOOL filter2F(ULONG *eventType,VIO32EVENTINFO *event,VOID *filterData){
    switch (*eventType){
        case WM_CHAR:
            printf("2WM_CHAR\n");
        break;
        case VIO_CHAR:
            printf("2VIO_CHAR\n");
        break;
        case MON_CHAR:
            printf("2MON_CHAR\n");
        break;
        case WM_MOVE:{
            printf("2WM_MOVE\n");
        } break;
        case KBD32_CHAR:
            printf("2KBD32_CHAR\n");
        break;
        case VIO32_MOU:
            printf("2VIO32_MOU\n");
        break;
        default:
        break;
    }
    return FALSE;
}

vio32Filter filter1={NULL,(VOID *)"filter1",filter1F};
vio32Filter filter2={NULL,(VOID *)"filter2",filter2F};


void main(int argc, char **argv){

    vio32Open();

    kbd32SetMode(0);

    ULONG where = VIO32_PRE_FIRST;
    kbd32AddFilter(&filter1,&where);
    where = VIO32_POST_FIRST;
    kbd32AddFilter(&filter2,&where);

    USHORT  fs;
    UCHAR   cRepeat;
    UCHAR   scancode;
    USHORT  chr;
    USHORT  vkey;
    while(1){
        KBD32KEYINFO key;
//    DosSleep(1000);
        BOOL ret = kbd32GetKey(&key,2000);
        if (ret==0){
            char cc = key.chr;
            if (cc == 0) cc = ' ';
            printf("ascii=(%c,%i) scan=%i status=%x vkey=%i up=%i\n",cc,key.chr & 255,key.scancode,key.status,key.vKey,key.keyUp);
        } else printf("NO\n");
    }

    vio32Close();


}
