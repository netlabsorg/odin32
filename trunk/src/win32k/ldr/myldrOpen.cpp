/* $Id: myldrOpen.cpp,v 1.1 1999-09-06 02:20:01 bird Exp $
 *
 * myldrOpen - _ldrOpen.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <string.h>

#include "OS2Krnl.h"
#include "log.h"
#include "malloc.h"
#if 0
    #include "lxFile.h"
#else
    #define LXFile class { public: BOOL queryIsModuleName(const char *) {return FALSE;}}
#endif
#include "ldr.h"
#include "ldrCalls.h"



/**
 * _ldrOpen override.
 * @returns   Return code.
 * @param     phFile       Pointer to file handler. Holds filehandle on output.
 * @param     pszFilename  Pointer to filename.
 * @parma     param3       Probably some flags.
 */
ULONG LDRCALL myldrOpen(PSFN phFile, char *pszFilename, ULONG param3)
{
    ULONG rc;
    int i;

    rc = _ldrOpen(phFile, pszFilename, param3);

    if (rc == NO_ERROR)
        kprintf(("_ldrOpen:  phFile=%#.4x, flags=%#.8x, pszFn=%s\n", *phFile, param3, pszFilename));

    if (rc == NO_ERROR)
    {
        i = getFreeUncertainEntry();
        if (i != -1)
        {
            ahUncertain[i].offsetNEHdr = 0;
            ahUncertain[i].fMZ = 0;
            ahUncertain[i].fPE = 0;
            ahUncertain[i].hFile = *phFile;
            ahUncertain[i].pszName = (char*)malloc(strlen(pszFilename)+1);
            if (ahUncertain[i].pszName != NULL)
            {
                strcpy(ahUncertain[i].pszName, pszFilename);
                SetState(*phFile, HSTATE_CHECK);
            }
            else
            {
                kprintf(("myldrOpen: warning - malloc returned NULL\n"));
                freeUncertainEntry(*phFile);
                SetState(*phFile, HSTATE_NOT_PE);
            }
        }
        else
            kprintf(("getFreeUncertainEntry failed\n"));
    }
    return rc;
}
