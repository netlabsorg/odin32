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
    kbd32SetMode(KBD32MODE_ENABLEMOUSE);

    printf("\n");
    VIO32STATUSINFO status;
    KBD32KEYINFO key;
    while(1){
        BOOL ret = kbd32GetKey(&key,2000);
        if (ret==0){
            kbd32GetKeyStatus(&status);
//            for(int i=0;i<79;i++)
//                if( KBD32_CHECKKEY(status,i)) printf("1");
//                else                          printf("0");
            printf("%08x%08x%08x%08x",status.status[0],status.status[1],status.status[2],status.status[3]);
            printf("%08x%08x%08x%08x%08x\n",status.status[4],status.status[5],status.status[6],status.status[7],status.status[8]);

        } else DosSleep(100);
    }

    vio32Close();


}
