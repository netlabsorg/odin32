/* $Id: log.c,v 1.2 2000-09-02 21:08:13 bird Exp $
 *
 * log - C-style logging - kprintf.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */


/*@Const************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*@Header***********************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <string.h>
#include <stdarg.h>
#include <builtin.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include "options.h"
#include "log.h"
#include "sprintf.h"
#include "yield.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static char  achBuffer[512]; /* buffer for kprintf */


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static void write(char *psz);


/**
 * Writes a string to logBuffer and to comport.
 * @param     psz   Pointer to zero-string to be put to com-port.
 * @param     Port  Port number of the com-port.
 * @remark    A lonely '\n' will result in a '\r\n' sequence.
 */
static void write(char *psz)
{
    /* if logging (to com port) enabled - log to COM-port */
    if (options.fLogging)
    {
        ULONG ulPort = options.usCom;
        int   cch = 0;

        while (*psz != '\0')
        {
            while (!(_inp(ulPort + 5) & 0x20));  /* waits for port To Be ready */
            _outp(ulPort, *psz);                 /* put char and inc pointer */

            /* new line fix */
            if (psz[1] == '\n' && psz[0] != '\r')
            {
                while (!(_inp(ulPort + 5) & 0x20));  /* waits for port To Be ready */
                _outp(ulPort, '\r');                 /* put char and inc pointer */
                cch++;
            }
            psz++;
            if ((++cch % 3) == 0) /* 9600 bit/s -> 1066 bytes (9bit) -> 3 chars takes 3.2 ms to send */
                Yield();
        }
    }
}


/**
 * printf to log.
 * @param   pFormat  Formatstring
 * @param   ...      Other stuff used in pFormat.
 * @remark  This functions may yield the CPU!
 */
void _kprintf(const char *pszFormat, ...)
{
    va_list arg;

    /* check pFormat */
    if (pszFormat == NULL)
    {
        write("\nWarning: Null pointer detected in kprint\n");
        return;
    }

    /* build string */
    va_start(arg, pszFormat);
    vsprintf(achBuffer, pszFormat, arg);
    va_end(arg);

    /* write string */
    write(achBuffer);
}
