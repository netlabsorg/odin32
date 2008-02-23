#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_DOS
#define INCL_PM
#define INCL_WIN
#define INCL_VIO
#define INCL_SUB
#define INCL_DOSERRORS
#include <os2.h>
#include "vio32.h"

extern "C" {
    //
    BOOL EXPENTRY InstallHook();
    //
    BOOL EXPENTRY DeInstallHook();
}


ULONG _System MyTermHandler(  PEXCEPTIONREPORTRECORD       p1,
                              PEXCEPTIONREGISTRATIONRECORD p2,
                              PCONTEXTRECORD               p3,
                              PVOID                        pv ){
    switch( p1->ExceptionNum) {
        case XCPT_PROCESS_TERMINATE:
        case XCPT_ASYNC_PROCESS_TERMINATE: {
printf("termination handler called \n");
            DeInstallHook();
        }
        default:;
    }

   return XCPT_CONTINUE_SEARCH;          /* Exception not resolved... */
}


void main(int argc, char **argv){

    EXCEPTIONREGISTRATIONRECORD RegRec;
    RegRec.prev_structure = NULL;
    RegRec.ExceptionHandler = (ERR)MyTermHandler;


    DosSetExceptionHandler(&RegRec);
    InstallHook();

    DosSleep(-1);

    DeInstallHook();
    DosUnsetExceptionHandler(&RegRec);

}
