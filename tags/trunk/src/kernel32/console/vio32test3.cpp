#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  INCL_DOS
#define  INCL_WIN
#define  INCL_BASE
#include <os2.h>
#include "vio32.h"


void main(int argc, char **argv){


    vio32Open();
    kbd32SetMode(KBD32MODE_DISABLEBREAK | KBD32MODE_ENABLEMOUSE | KBD32MODE_ENABLEMMOVE);


    printf("\n");
    VIO32EVENTINFO event;
    ULONG eventType;
    while(1){
        BOOL ret = vio32GetEvent(&event,&eventType,-1);
        if (ret==0){
            printf("%x\n",eventType);
        } else DosSleep(100);
    }

    vio32Close();


}
