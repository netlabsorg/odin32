/* $Id: kError.cpp,v 1.1 2002-02-24 02:47:24 bird Exp $
 *
 * Error Wrapper.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMISC


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#ifdef __EMX__
#define __OS2DEF__
#define OS2EMX_PLAIN_CHAR
#endif
#include <os2.h>

#include <string.h>
#include <stdio.h>

#include <kTypes.h>
#include <kError.h>


/**
 * Gets the message text for a given error number.
 * @return  Message text. Caller must delete it.
 */
char *kError::getText(int iErrorNo)
{
    char *psz;

    if (iErrorNo >= 0 && iErrorNo <= 0x10000)
    {
        char    szErrorMsg[1024];
        ULONG   cbMsg = sizeof(szErrorMsg);
        APIRET  rc;

        rc = DosGetMessage(NULL, 0, &szErrorMsg[0], cbMsg, iErrorNo, "OSO001.MSG", &cbMsg);
        if (rc || cbMsg == 0)
            cbMsg = sprintf(szErrorMsg,
                            "No error message for error %d. (DosGetMessage -> %d)",
                            iErrorNo, rc);
        psz = new char[cbMsg + 1];
        strcpy(psz, szErrorMsg);
    }
    else
    {   /* User defined message */
        char    szErrorMsg[50];
        char *  psz2;

        switch (iErrorNo)
        {

            default:
                sprintf(szErrorMsg, "No error message for error %d.", iErrorNo);
                psz2 = szErrorMsg;
        }
        psz = new char[strlen(psz2) + 1];
        strcpy(psz, psz2);
    }

    return psz;
}

