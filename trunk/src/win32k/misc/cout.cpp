/* $Id: cout.cpp,v 1.3 2000-09-02 21:08:13 bird Exp $
 *
 * cout - cout replacement.
 *
 * Note: use of cout is discurraged.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <builtin.h>
#include <stdarg.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "cout.h"
#include "options.h"
#include "sprintf.h"
#include "yield.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
kLog _log_;


/** Constructor -> constructor(..) */
kLog::kLog()
{
    constructor();
}


/** Destructor */
kLog::~kLog()
{
}


/**
 * Manual constructor.
 * @param    ulPort   Portaddress for the (com)port to use. default: OUTPUT_COM2
 * @remark   (OUTPUT_COM1 and OUTPUT_COM2 are defined in the header file.)
 */
void kLog::constructor(ULONG ulPort /*= OUTPUT_COM2*/)
{
    fHex = TRUE;
    this->ulPort = ulPort;
}


/**
 * Put a single char to the com-port.
 * @param     ch  Char to put.
 */
void kLog::putch(char ch)
{
    if (!options.fLogging)
        return;

    #if !defined(RING0) || defined(RING0_DEBUG_IN_RING3)
        putc(ch, stdout);
    #else
        while (!(_inp(ulPort + 5) & 0x20));     /* waits for port To Be ready */
        _outp(ulPort, ch);                      /* put char */
    #endif
}


/**
 * Puts a string to the comport.
 * @param     psz  String to put.
 * @remark    Note that this function may yield the CPU.
 */
void kLog::putstr(const char *psz)
{
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


/**
 * Put an unsigned long value to the comport.
 * @returns   Reference to this object.
 * @param     ul  Number to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(unsigned long ul)
{
    sprintf(szBuffer, fHex ? "%lx" : "%lu", ul);
    putstr(szBuffer);
    return *this;
}


/**
 * Put a pointer value to the comport.
 * @returns   Reference to this object.
 * @param     p  Pointer to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(const void *pv)
{
    if (pv != NULL)
    {
        sprintf(szBuffer,"%#.8lx", pv);
        putstr(szBuffer);
    }
    else
        putstr("(null)");
    return *this;
}



/**
 * Put a signed long value to the comport.
 * @returns   Reference to this object.
 * @param     l  Number to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(long l)
{
    sprintf(szBuffer, fHex ? "%lx" : "%ld", l);
    putstr(szBuffer);
    return *this;
}


/**
 * Set format.
 * @param     hex      Hex indicator.
 * @param     ignored  This parameter is ignored.
 */
void kLog::setf(int fHex, int ignored)
{
    if (fHex == 1 || fHex == 0)
        this->fHex = fHex;
}


/**
 * Put a char to the comport.
 * @returns   Reference to this object.
 * @param     c  Char to put.
 */
kLog& kLog::operator<<(char c)
{
    kLog::putch(c);
    return *this;
}


/**
 * Put a string to the COM-port.
 * @returns   Reference to this object.
 * @param     psz  Pointer to string.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(const char *psz)
{
    kLog::putstr(psz);
    return *this;
}


/**
 * Put a string to the COM-port.
 * @returns   Reference to this object.
 * @param     psz  Pointer to string.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(const signed char *psz)
{
    kLog::putstr((char*)psz);
    return *this;
}


/**
 * Put a string to the COM-port.
 * @returns   Reference to this object.
 * @param     psz  Pointer to string.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(const unsigned char *psz)
{
    kLog::putstr((char*)psz);
    return *this;
}


/**
 * Put an (signed) int to the COM-port.
 * @returns   Reference to this object.
 * @param     a  Number to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(int a)
{
    return *this << (long)a;
}


/**
 * Put an unsigned int to the COM-port.
 * @returns   Reference to this object.
 * @param     a  Number to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(unsigned int a)
{
    return *this << (unsigned long)a;
}


/**
 * Put a signed short int to the COM-port.
 * @returns   Reference to this object.
 * @param     a  Number to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(short i)
{
    return *this << (long)i;
}


/**
 * Put a unsigned short int to the COM-port.
 * @returns   Reference to this object.
 * @param     a  Number to put.
 * @remark    May yield the CPU.
 */
kLog& kLog::operator<<(unsigned short i)
{
    return *this << (unsigned long)i;
}


/**
 * Init function for cout.
 * @param    ulPort   Portaddress for the (com)port to use. default: OUTPUT_COM2
 * @remark   (OUTPUT_COM1 and OUTPUT_COM2 are defined in the header file.)
 */
void coutInit(USHORT usCom)
{
    _log_.constructor(usCom);
}


