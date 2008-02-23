/*
 * VARIANT
 *
 * Copyright 1998 Jean-Claude Cote
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * NOTES
 *   This implements the low-level and hi-level APIs for manipulating VARIANTs.
 *   The low-level APIs are used to do data coercion between different data types.
 *   The hi-level APIs are built on top of these low-level APIs and handle
 *   initialization, copying, destroying and changing the type of VARIANTs.
 *
 * TODO:
 *   - The Variant APIs do not support international languages, currency
 *     types, number formating and calendar.  They only support U.S. English format.
 *   - The Variant APIs do not the following types: IUknown, IDispatch, DECIMAL and SafeArray.
 *     The prototypes for these are commented out in the oleauto.h file.  They need
 *     to be implemented and cases need to be added to the switches of the  existing APIs.
 *   - The parsing of date for the VarDateFromStr is not complete.
 *   - The date manipulations do not support dates prior to 1900.
 *   - The parsing does not accept as many formats as the Windows implementation.
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#ifdef HAVE_FLOAT_H
# include <float.h>
#endif

#include "windef.h"
#include "oleauto.h"
#include "heap.h"
#include "wine/debug.h"
#include "winerror.h"
#include "parsedt.h"
#include "typelib.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole);

#define SYSDUPSTRING(str) SysAllocStringLen((str), SysStringLen(str))

#ifndef FLT_MAX
# ifdef MAXFLOAT
#  define FLT_MAX MAXFLOAT
# else
#  error "Can't find #define for MAXFLOAT/FLT_MAX"
# endif
#endif

#undef CHAR_MAX
#undef CHAR_MIN
static const char CHAR_MAX = 127;
static const char CHAR_MIN = -128;
static const BYTE UI1_MAX = 255;
static const BYTE UI1_MIN = 0;
static const unsigned short UI2_MAX = 65535;
static const unsigned short UI2_MIN = 0;
static const short I2_MAX = 32767;
static const short I2_MIN =  -32768;
static const unsigned long UI4_MAX = 4294967295U;
static const unsigned long UI4_MIN = 0;
static const long I4_MAX = 2147483647;
static const long I4_MIN = -(2147483648U);
static const DATE DATE_MIN = -657434;
static const DATE DATE_MAX = 2958465;


/* This mask is used to set a flag in wReserved1 of
 * the VARIANTARG structure. The flag indicates if
 * the API function is using an inner variant or not.
 */
#define PROCESSING_INNER_VARIANT 0x0001

/* General use buffer.
 */
#define BUFFER_MAX 1024
static char pBuffer[BUFFER_MAX];

/*
 * Note a leap year is one that is a multiple of 4
 * but not of a 100.  Except if it is a multiple of
 * 400 then it is a leap year.
 */

/*
 * Use 365 days/year and a manual calculation for leap year days
 * to keep arithmetic simple
 */
static const double DAYS_IN_ONE_YEAR = 365.0;

/*
 * Token definitions for Varient Formatting
 * Worked out by experimentation on a w2k machine. Doesnt appear to be
 *   documented anywhere obviously so keeping definitions internally
 *
 */
/* Pre defined tokens */
#define TOK_COPY 0x00
#define TOK_END  0x02
#define LARGEST_TOKENID 6

/* Mapping of token name to id put into the tokenized form
   Note testing on W2K shows aaaa and oooo are not parsed??!! */
#define TOK_COLON  0x03
#define TOK_SLASH  0x04
#define TOK_c      0x05
#define TOK_d      0x08
#define TOK_dd     0x09
#define TOK_ddd    0x0a
#define TOK_dddd   0x0b
#define TOK_ddddd  0x0c
#define TOK_dddddd 0x0d
#define TOK_w      0x0f
#define TOK_ww     0x10
#define TOK_m      0x11
#define TOK_mm     0x12
#define TOK_mmm    0x13
#define TOK_mmmm   0x14
#define TOK_q      0x06
#define TOK_y      0x15
#define TOK_yy     0x16
#define TOK_yyyy   0x18
#define TOK_h      0x1e
#define TOK_Hh     0x1f
#define TOK_N      0x1a
#define TOK_Nn     0x1b
#define TOK_S      0x1c
#define TOK_Ss     0x1d
#define TOK_ttttt  0x07
#define TOK_AMsPM  0x2f
#define TOK_amspm  0x32
#define TOK_AsP    0x30
#define TOK_asp    0x33
#define TOK_AMPM   0x2e

typedef struct tagFORMATTOKEN {
    char  *str;
    BYTE   tokenSize;
    BYTE   tokenId;
    int    varTypeRequired;
} FORMATTOKEN;

typedef struct tagFORMATHDR {
    BYTE   len;
    BYTE   hex3;
    BYTE   hex6;
    BYTE   reserved[8];
} FORMATHDR;

FORMATTOKEN formatTokens[] = {           /* FIXME: Only date formats so far */
    {":"     ,   1,  TOK_COLON  , 0},
    {"/"     ,   1,  TOK_SLASH  , 0},
    {"c"     ,   1,  TOK_c      , VT_DATE},
    {"dddddd",   6,  TOK_dddddd , VT_DATE},
    {"ddddd" ,   5,  TOK_ddddd  , VT_DATE},
    {"dddd"  ,   4,  TOK_dddd   , VT_DATE},
    {"ddd"   ,   3,  TOK_ddd    , VT_DATE},
    {"dd"    ,   2,  TOK_dd     , VT_DATE},
    {"d"     ,   1,  TOK_d      , VT_DATE},
    {"ww"    ,   2,  TOK_ww     , VT_DATE},
    {"w"     ,   1,  TOK_w      , VT_DATE},
    {"mmmm"  ,   4,  TOK_mmmm   , VT_DATE},
    {"mmm"   ,   3,  TOK_mmm    , VT_DATE},
    {"mm"    ,   2,  TOK_mm     , VT_DATE},
    {"m"     ,   1,  TOK_m      , VT_DATE},
    {"q"     ,   1,  TOK_q      , VT_DATE},
    {"yyyy"  ,   4,  TOK_yyyy   , VT_DATE},
    {"yy"    ,   2,  TOK_yy     , VT_DATE},
    {"y"     ,   1,  TOK_y      , VT_DATE},
    {"h"     ,   1,  TOK_h      , VT_DATE},
    {"Hh"    ,   2,  TOK_Hh     , VT_DATE},
    {"Nn"    ,   2,  TOK_Nn     , VT_DATE},
    {"N"     ,   1,  TOK_N      , VT_DATE},
    {"S"     ,   1,  TOK_S      , VT_DATE},
    {"Ss"    ,   2,  TOK_Ss     , VT_DATE},
    {"ttttt" ,   5,  TOK_ttttt  , VT_DATE},
    {"AM/PM" ,   5,  TOK_AMsPM  , VT_DATE},
    {"am/pm" ,   5,  TOK_amspm  , VT_DATE},
    {"A/P"   ,   3,  TOK_AsP    , VT_DATE},
    {"a/p"   ,   3,  TOK_asp    , VT_DATE},
    {"AMPM"  ,   4,  TOK_AMPM   , VT_DATE},
    {0x00    ,   0,  0          , VT_NULL}
};


/************** Form wine: oleaut32/variant.h **************/

/* Value of sign for a positive decimal number */
#define DECIMAL_POS 0

/* Native headers don't change the union ordering for DECIMAL sign/scale (duh).
 * This means that the signscale member is only useful for setting both members to 0.
 * SIGNSCALE creates endian-correct values so that we can properly set both at once
 * to values other than 0.
 */
#ifdef WORDS_BIGENDIAN
#define SIGNSCALE(sign,scale) (((scale) << 8) | sign)
#else
#define SIGNSCALE(sign,scale) (((sign) << 8) | scale)
#endif
/* Macros for getting at a DECIMAL's parts */
#define DEC_SIGN(d)      ((d)->u.s.sign)
#define DEC_SCALE(d)     ((d)->u.s.scale)
#define DEC_SIGNSCALE(d) ((d)->u.signscale)
#define DEC_HI32(d)      ((d)->Hi32)
#define DEC_MID32(d)     ((d)->u1.s1.Mid32)
#define DEC_LO32(d)      ((d)->u1.s1.Lo32)
#define DEC_LO64(d)      ((d)->u1.Lo64)

/* DECIMAL */
#define _VarDecFromStr(str,lcid,flags,out) VARIANT_NumberFromBstr(str,lcid,flags,(BYTE*)out,VT_DECIMAL)
/* R8 (double) */
#define _VarR8FromStr(str,lcid,flags,out) VARIANT_NumberFromBstr(str,lcid,flags,(BYTE*)out,VT_R8)

/* Internal flags for low level conversion functions */
#define  VAR_BOOLONOFF 0x0400 /* Convert bool to "On"/"Off" */
#define  VAR_BOOLYESNO 0x0800 /* Convert bool to "Yes"/"No" */
#define  VAR_NEGATIVE  0x1000 /* Number is negative */

/* The localised characters that make up a valid number */
typedef struct tagVARIANT_NUMBER_CHARS
{
  WCHAR cNegativeSymbol;
  WCHAR cPositiveSymbol;
  WCHAR cDecimalPoint;
  WCHAR cDigitSeperator;
  WCHAR cCurrencyLocal;
  WCHAR cCurrencyLocal2;
  WCHAR cCurrencyDecimalPoint;
  WCHAR cCurrencyDigitSeperator;
} VARIANT_NUMBER_CHARS;

void VARIANT_GetLocalisedNumberChars(VARIANT_NUMBER_CHARS*,LCID,DWORD);
/********** End of Form wine: oleaut32/variant.h ************/

/* Write a number from a UI8 and sign */
#ifdef __WIN32OS2__
static WCHAR *VARIANT_WriteNumber(ULONG ulVal, WCHAR* szOut)
{
  //FIXME("%s: Hack! No real ULONG64 support here, using ULONG instead. ulVal=%x!!\n", __FUNCTION__, ulVal);
#else 
static WCHAR *VARIANT_WriteNumber(ULONG64 ulVal, WCHAR* szOut)
  {
#endif

  do
  {
    WCHAR ulNextDigit = ulVal % 10;

    *szOut-- = '0' + ulNextDigit;
    ulVal = (ulVal - ulNextDigit) / 10;
  } while (ulVal);

  szOut++;
  return szOut;
}

/* Create a (possibly localised) BSTR from a UI8 and sign */
static BSTR VARIANT_MakeBstr(LCID lcid, DWORD dwFlags, WCHAR *szOut)
{
  WCHAR szConverted[256];

  if (dwFlags & VAR_NEGATIVE)
    *--szOut = '-';

  if (dwFlags & LOCALE_USE_NLS)
  {
    /* Format the number for the locale */
    szConverted[0] = '\0';
    GetNumberFormatW(lcid,
                     dwFlags & LOCALE_NOUSEROVERRIDE,
                     szOut, NULL, szConverted, sizeof(szConverted)/sizeof(WCHAR));
    szOut = szConverted;
  }
  return SysAllocStringByteLen((LPCSTR)szOut, strlenW(szOut) * sizeof(WCHAR));
}


/******************************************************************************
 * VarBstrFromDec (OLEAUT32.232)
 *
 * Convert a VT_DECIMAL to a VT_BSTR.
 *
 * PARAMS
 *  pDecIn   [I] Source
 *  lcid     [I] LCID for the conversion
 *  dwFlags  [I] Flags controlling the conversion (VAR_ flags from "oleauto.h")
 *  pbstrOut [O] Destination
 *
 * RETURNS
 *  Success: S_OK.
 *  Failure: E_INVALIDARG, if pbstrOut is invalid.
 *           E_OUTOFMEMORY, if memory allocation fails.
 */
HRESULT WINAPI VarBstrFromDec(DECIMAL* pDecIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
  TRACE("%s: entering function. pDecIn->lo32: %x, pDecIn->Mid32: %x, pDecIn->Hi32: %x, sign: %x, dwFlags: %x\n",
        __FUNCTION__, DEC_LO32(pDecIn), DEC_MID32(pDecIn), DEC_HI32(pDecIn), DEC_SIGN(pDecIn), dwFlags);
  if (!pbstrOut)
    return E_INVALIDARG;

  if (!DEC_SCALE(pDecIn) && !DEC_HI32(pDecIn))
  {
    WCHAR szBuff[256], *szOut = szBuff + sizeof(szBuff)/sizeof(WCHAR) - 1;

    /* Create the basic number string */
    *szOut-- = '\0';


#ifdef __WIN32OS2__
    szOut = VARIANT_WriteNumber(DEC_LO32(pDecIn), szOut);
#else
    szOut = VARIANT_WriteNumber(DEC_LO64(pDecIn), szOut);
#endif
    if (DEC_SIGN(pDecIn))
      dwFlags |= VAR_NEGATIVE;

    *pbstrOut = VARIANT_MakeBstr(lcid, dwFlags, szOut);
    TRACE("%s: returning %s\n", __FUNCTION__, debugstr_w(*pbstrOut));
    return *pbstrOut ? S_OK : E_OUTOFMEMORY;
  }
  FIXME("semi-stub\n");
  return E_INVALIDARG;
}


/* Copy data from one variant to another. */
static inline void VARIANT_CopyData(const VARIANT *srcVar, VARTYPE vt, void *pOut)
{
  //  TRACE("%s: entering function.\n", __FUNCTION__);
  switch (vt)
  {
  case VT_I1:
  case VT_UI1: memcpy(pOut, &V_UI1(srcVar), sizeof(BYTE)); break;
  case VT_BOOL:
  case VT_I2:
  case VT_UI2: memcpy(pOut, &V_UI2(srcVar), sizeof(SHORT)); break;
  case VT_R4:
  case VT_INT:
  case VT_I4:
  case VT_UINT:
  case VT_UI4: memcpy(pOut, &V_UI4(srcVar), sizeof (LONG)); break;
  case VT_R8:
  case VT_DATE:
  case VT_CY:
  case VT_I8:
  case VT_UI8: memcpy(pOut, &V_UI8(srcVar), sizeof (LONGLONG)); break;
#ifdef __WIN32OS2__
    /*  case VT_INT_PTR: memcpy(pOut, &V_INT_PTR(srcVar), sizeof (INT_PTR)); break; */
  case VT_DECIMAL: memcpy(pOut, &V_DECIMAL(srcVar), sizeof (DECIMAL)); break;
#else
  case VT_INT_PTR: memcpy(pOut, &V_INT_PTR(srcVar), sizeof (INT_PTR)); break;
  case VT_DECIMAL: memcpy(pOut, &V_DECIMAL(srcVar), sizeof (DECIMAL)); break;
#endif
  default:
    FIXME("%s: VT_ type %d unhandled, please report!\n", __FUNCTION__,  vt);
  }
}

/* Form wine: oleaut32/vartype.c */
/* Coerce VT_BSTR to a numeric type */
HRESULT VARIANT_NumberFromBstr(OLECHAR* pStrIn, LCID lcid, ULONG ulFlags,
                               void* pOut, VARTYPE vt)
{
  VARIANTARG dstVar;
  HRESULT hRet;
  NUMPARSE np;
  BYTE rgb[1024];

  TRACE("%s: (StrIn: %s, LCID: 0x%08lx, ulFlags: 0x%08lx )\n", __FUNCTION__, pStrIn, lcid, ulFlags );

  /* Use VarParseNumFromStr/VarNumFromParseNum as MSDN indicates */
  np.cDig = sizeof(rgb) / sizeof(BYTE);
  np.dwInFlags = NUMPRS_STD;

  hRet = VarParseNumFromStr(pStrIn, lcid, ulFlags, &np, rgb);

  if (SUCCEEDED(hRet))
  {
    /* 1 << vt gives us the VTBIT constant for the destination number type */
    hRet = VarNumFromParseNum(&np, rgb, 1 << vt, &dstVar);
    if (SUCCEEDED(hRet)) {
      VARIANT_CopyData(&dstVar, vt, pOut);
      TRACE("%s: Returning dstVar-> type : %x", __FUNCTION__, V_VT(&dstVar));
    }
  }
  return hRet;
}


/******************************************************************************
 *	   DateTimeStringToTm	[INTERNAL]
 *
 * Converts a string representation of a date and/or time to a tm structure.
 *
 * Note this function uses the postgresql date parsing functions found
 * in the parsedt.c file.
 *
 * Returns TRUE if successful.
 *
 * Note: This function does not parse the day of the week,
 * daylight savings time. It will only fill the followin fields in
 * the tm struct, tm_sec, tm_min, tm_hour, tm_year, tm_day, tm_mon.
 *
 ******************************************************************************/
static BOOL DateTimeStringToTm( OLECHAR* strIn, DWORD dwFlags, struct tm* pTm )
{
	BOOL res = FALSE;
	double		fsec;
	int 		tzp;
	int 		dtype;
	int 		nf;
	char	   *field[MAXDATEFIELDS];
	int 		ftype[MAXDATEFIELDS];
	char		lowstr[MAXDATELEN + 1];
	char* strDateTime = NULL;

	/* Convert the string to ASCII since this is the only format
	 * postgesql can handle.
	 */
	strDateTime = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );

	if( strDateTime != NULL )
	{
		/* Make sure we don't go over the maximum length
		 * accepted by postgesql.
		 */
		if( strlen( strDateTime ) <= MAXDATELEN )
		{
			if( ParseDateTime( strDateTime, lowstr, field, ftype, MAXDATEFIELDS, &nf) == 0 )
			{
				if( dwFlags & VAR_DATEVALUEONLY )
				{
					/* Get the date information.
					 * It returns 0 if date information was
					 * present and 1 if only time information was present.
					 * -1 if an error occures.
					 */
					if( DecodeDateTime(field, ftype, nf, &dtype, pTm, &fsec, &tzp) == 0 )
					{
						/* Eliminate the time information since we
						 * were asked to get date information only.
						 */
						pTm->tm_sec = 0;
						pTm->tm_min = 0;
						pTm->tm_hour = 0;
						res = TRUE;
					}
				}
				if( dwFlags & VAR_TIMEVALUEONLY )
				{
					/* Get time information only.
					 */
					if( DecodeTimeOnly(field, ftype, nf, &dtype, pTm, &fsec) == 0 )
					{
						res = TRUE;
					}
				}
				else
				{
					/* Get both date and time information.
					 * It returns 0 if date information was
					 * present and 1 if only time information was present.
					 * -1 if an error occures.
					 */
					if( DecodeDateTime(field, ftype, nf, &dtype, pTm, &fsec, &tzp) != -1 )
					{
						res = TRUE;
					}
				}
			}
		}
		HeapFree( GetProcessHeap(), 0, strDateTime );
	}

	return res;
}






/******************************************************************************
 *	   TmToDATE 	[INTERNAL]
 *
 * The date is implemented using an 8 byte floating-point number.
 * Days are represented by whole numbers increments starting with 0.00 has
 * being December 30 1899, midnight.
 * The hours are expressed as the fractional part of the number.
 * December 30 1899 at midnight = 0.00
 * January 1 1900 at midnight = 2.00
 * January 4 1900 at 6 AM = 5.25
 * January 4 1900 at noon = 5.50
 * December 29 1899 at midnight = -1.00
 * December 18 1899 at midnight = -12.00
 * December 18 1899 at 6AM = -12.25
 * December 18 1899 at 6PM = -12.75
 * December 19 1899 at midnight = -11.00
 * The tm structure is as follows:
 * struct tm {
 *		  int tm_sec;	   seconds after the minute - [0,59]
 *		  int tm_min;	   minutes after the hour - [0,59]
 *		  int tm_hour;	   hours since midnight - [0,23]
 *		  int tm_mday;	   day of the month - [1,31]
 *		  int tm_mon;	   months since January - [0,11]
 *		  int tm_year;	   years
 *		  int tm_wday;	   days since Sunday - [0,6]
 *		  int tm_yday;	   days since January 1 - [0,365]
 *		  int tm_isdst;    daylight savings time flag
 *		  };
 *
 * Note: This function does not use the tm_wday, tm_yday, tm_wday,
 * and tm_isdst fields of the tm structure. And only converts years
 * after 1900.
 *
 * Returns TRUE if successful.
 */
static BOOL TmToDATE( struct tm* pTm, DATE *pDateOut )
{
    int leapYear = 0;

    /* Hmmm... An uninitialized Date in VB is December 30 1899 so
       Start at 0. This is the way DATE is defined. */

    /* Start at 1. This is the way DATE is defined.
     * January 1, 1900 at Midnight is 1.00.
     * January 1, 1900 at 6AM is 1.25.
     * and so on.
     */
    *pDateOut = 1;

    if( (pTm->tm_year - 1900) >= 0 ) {

        /* Add the number of days corresponding to
         * tm_year.
         */
        *pDateOut += (pTm->tm_year - 1900) * 365;

        /* Add the leap days in the previous years between now and 1900.
         * Note a leap year is one that is a multiple of 4
         * but not of a 100.  Except if it is a multiple of
         * 400 then it is a leap year.
         * Copied + reversed functionality into TmToDate
         */
        *pDateOut += ( (pTm->tm_year - 1) / 4 ) - ( 1900 / 4 );
        *pDateOut -= ( (pTm->tm_year - 1) / 100 ) - ( 1900 / 100 );
        *pDateOut += ( (pTm->tm_year - 1) / 400 ) - ( 1900 / 400 );

        /* Set the leap year flag if the
         * current year specified by tm_year is a
         * leap year. This will be used to add a day
         * to the day count.
         */
        if( isleap( pTm->tm_year ) )
            leapYear = 1;

        /* Add the number of days corresponding to
         * the month. (remember tm_mon is 0..11)
         */
        switch( pTm->tm_mon )
        {
        case 1:
            *pDateOut += 31;
            break;
        case 2:
            *pDateOut += ( 59 + leapYear );
            break;
        case 3:
            *pDateOut += ( 90 + leapYear );
            break;
        case 4:
            *pDateOut += ( 120 + leapYear );
            break;
        case 5:
            *pDateOut += ( 151 + leapYear );
            break;
        case 6:
            *pDateOut += ( 181 + leapYear );
            break;
        case 7:
            *pDateOut += ( 212 + leapYear );
            break;
        case 8:
            *pDateOut += ( 243 + leapYear );
            break;
        case 9:
            *pDateOut += ( 273 + leapYear );
            break;
        case 10:
            *pDateOut += ( 304 + leapYear );
            break;
        case 11:
            *pDateOut += ( 334 + leapYear );
            break;
        }
        /* Add the number of days in this month.
         */
        *pDateOut += pTm->tm_mday;

        /* Add the number of seconds, minutes, and hours
         * to the DATE. Note these are the fracionnal part
         * of the DATE so seconds / number of seconds in a day.
         */
    } else {
        *pDateOut = 0;
    }

    *pDateOut += pTm->tm_hour / 24.0;
    *pDateOut += pTm->tm_min / 1440.0;
    *pDateOut += pTm->tm_sec / 86400.0;
    return TRUE;
}

/******************************************************************************
 *	   DateToTm 	[INTERNAL]
 *
 * This function converts a windows DATE to a tm structure.
 *
 * It does not fill all the fields of the tm structure.
 * Here is a list of the fields that are filled:
 * tm_sec, tm_min, tm_hour, tm_year, tm_day, tm_mon.
 *
 * Note this function does not support dates before the January 1, 1900
 * or ( dateIn < 2.0 ).
 *
 * Returns TRUE if successful.
 */
BOOL DateToTm( DATE dateIn, DWORD dwFlags, struct tm* pTm )
{
    double decimalPart = 0.0;
    double wholePart = 0.0;

    memset(pTm,0,sizeof(*pTm));

    /* Because of the nature of DATE format which
     * associates 2.0 to January 1, 1900. We will
     * remove 1.0 from the whole part of the DATE
     * so that in the following code 1.0
     * will correspond to January 1, 1900.
     * This simplifies the processing of the DATE value.
     */
    decimalPart = fmod( dateIn, 1.0 ); /* Do this before the -1, otherwise 0.xx goes negative */
    dateIn -= 1.0;
    wholePart = (double) floor( dateIn );

    if( !(dwFlags & VAR_TIMEVALUEONLY) )
    {
        unsigned int nDay = 0;
        int leapYear = 0;
        double yearsSince1900 = 0;

        /* Hard code dates smaller than January 1, 1900. */
        if( dateIn < 2.0 ) {
            pTm->tm_year = 1899;
            pTm->tm_mon  = 11; /* December as tm_mon is 0..11 */
            if( dateIn < 1.0 ) {
                pTm->tm_mday  = 30;
                dateIn = dateIn * -1.0; /* Ensure +ve for time calculation */
                decimalPart = decimalPart * -1.0; /* Ensure +ve for time calculation */
            } else {
                pTm->tm_mday  = 31;
            }

        } else {

            /* Start at 1900, this is where the DATE time 0.0 starts.
             */
            pTm->tm_year = 1900;
            /* find in what year the day in the "wholePart" falls into.
             * add the value to the year field.
             */
            yearsSince1900 = floor( (wholePart / DAYS_IN_ONE_YEAR) + 0.001 );
            pTm->tm_year += yearsSince1900;
            /* determine if this is a leap year.
             */
            if( isleap( pTm->tm_year ) )
            {
                leapYear = 1;
                wholePart++;
            }

            /* find what day of that year the "wholePart" corresponds to.
             * Note: nDay is in [1-366] format
             */
            nDay = (((unsigned int) wholePart) - ((pTm->tm_year-1900) * DAYS_IN_ONE_YEAR ));

            /* Remove the leap days in the previous years between now and 1900.
             * Note a leap year is one that is a multiple of 4
             * but not of a 100.  Except if it is a multiple of
             * 400 then it is a leap year.
             * Copied + reversed functionality from TmToDate
             */
            nDay -= ( (pTm->tm_year - 1) / 4 ) - ( 1900 / 4 );
            nDay += ( (pTm->tm_year - 1) / 100 ) - ( 1900 / 100 );
            nDay -= ( (pTm->tm_year - 1) / 400 ) - ( 1900 / 400 );

            /* Set the tm_yday value.
             * Note: The day must be converted from [1-366] to [0-365]
             */
            /*pTm->tm_yday = nDay - 1;*/
            /* find which month this day corresponds to.
             */
            if( nDay <= 31 )
            {
                pTm->tm_mday = nDay;
                pTm->tm_mon = 0;
            }
            else if( nDay <= ( 59 + leapYear ) )
            {
                pTm->tm_mday = nDay - 31;
                pTm->tm_mon = 1;
            }
            else if( nDay <= ( 90 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 59 + leapYear );
                pTm->tm_mon = 2;
            }
            else if( nDay <= ( 120 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 90 + leapYear );
                pTm->tm_mon = 3;
            }
            else if( nDay <= ( 151 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 120 + leapYear );
                pTm->tm_mon = 4;
            }
            else if( nDay <= ( 181 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 151 + leapYear );
                pTm->tm_mon = 5;
            }
            else if( nDay <= ( 212 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 181 + leapYear );
                pTm->tm_mon = 6;
            }
            else if( nDay <= ( 243 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 212 + leapYear );
                pTm->tm_mon = 7;
            }
            else if( nDay <= ( 273 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 243 + leapYear );
                pTm->tm_mon = 8;
            }
            else if( nDay <= ( 304 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 273 + leapYear );
                pTm->tm_mon = 9;
            }
            else if( nDay <= ( 334 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 304 + leapYear );
                pTm->tm_mon = 10;
            }
            else if( nDay <= ( 365 + leapYear ) )
            {
                pTm->tm_mday = nDay - ( 334 + leapYear );
                pTm->tm_mon = 11;
            }
        }
    }
    if( !(dwFlags & VAR_DATEVALUEONLY) )
    {
        /* find the number of seconds in this day.
         * fractional part times, hours, minutes, seconds.
         * Note: 0.1 is hack to ensure figures come out in whole numbers
         *   due to floating point inaccuracies
         */
        pTm->tm_hour = (int) ( decimalPart * 24 );
        pTm->tm_min = (int) ( ( ( decimalPart * 24 ) - pTm->tm_hour ) * 60 );
        /* Note: 0.1 is hack to ensure seconds come out in whole numbers
             due to floating point inaccuracies */
        pTm->tm_sec = (int) (( ( ( decimalPart * 24 * 60 ) - ( pTm->tm_hour * 60 ) - pTm->tm_min ) * 60 ) + 0.1);
    }
    return TRUE;
}



/******************************************************************************
 *	   SizeOfVariantData   	[INTERNAL]
 *
 * This function finds the size of the data referenced by a Variant based
 * the type "vt" of the Variant.
 */
static int SizeOfVariantData( VARIANT* parg )
{
    int size = 0;
    switch( V_VT(parg) & VT_TYPEMASK )
    {
    case( VT_I2 ):
        size = sizeof(short);
        break;
    case( VT_INT ):
        size = sizeof(int);
        break;
    case( VT_I4 ):
        size = sizeof(long);
        break;
    case( VT_UI1 ):
        size = sizeof(BYTE);
        break;
    case( VT_UI2 ):
        size = sizeof(unsigned short);
        break;
    case( VT_UINT ):
        size = sizeof(unsigned int);
        break;
    case( VT_UI4 ):
        size = sizeof(unsigned long);
        break;
    case( VT_R4 ):
        size = sizeof(float);
        break;
    case( VT_R8 ):
        size = sizeof(double);
        break;
    case( VT_DATE ):
        size = sizeof(DATE);
        break;
    case( VT_BOOL ):
        size = sizeof(VARIANT_BOOL);
        break;
    case( VT_BSTR ):
    case( VT_DISPATCH ):
    case( VT_UNKNOWN ):
        size = sizeof(void*);
        break;
    case( VT_CY ):
	size = sizeof(CY);
	break;
    case( VT_DECIMAL ):		/* hmm, tricky, DECIMAL is only VT_BYREF */
    default:
        FIXME("Add size information for type vt=%d\n", V_VT(parg) & VT_TYPEMASK );
        break;
    }

    return size;
}
/******************************************************************************
 *	   StringDupAtoBstr		[INTERNAL]
 *
 */
static BSTR StringDupAtoBstr( char* strIn )
{
	BSTR bstr = NULL;
	OLECHAR* pNewString = NULL;
	pNewString = HEAP_strdupAtoW( GetProcessHeap(), 0, strIn );
	bstr = SysAllocString( pNewString );
	HeapFree( GetProcessHeap(), 0, pNewString );
	return bstr;
}

/******************************************************************************
 *		round		[INTERNAL]
 *
 * Round the double value to the nearest integer value.
 */
static double round( double d )
{
   double decimals = 0.0, integerValue = 0.0, roundedValue = 0.0;
    BOOL bEvenNumber = FALSE;
    int nSign = 0;

    /* Save the sign of the number
     */
   nSign = (d >= 0.0) ? 1 : -1;
    d = fabs( d );

	/* Remove the decimals.
	 */
   integerValue = floor( d );

    /* Set the Even flag.  This is used to round the number when
     * the decimals are exactly 1/2.  If the integer part is
     * odd the number is rounded up. If the integer part
     * is even the number is rounded down.  Using this method
     * numbers are rounded up|down half the time.
     */
   bEvenNumber = (((short)fmod(integerValue, 2)) == 0) ? TRUE : FALSE;

    /* Remove the integral part of the number.
     */
    decimals = d - integerValue;

	/* Note: Ceil returns the smallest integer that is greater that x.
	 * and floor returns the largest integer that is less than or equal to x.
	 */
    if( decimals > 0.5 )
    {
        /* If the decimal part is greater than 1/2
         */
        roundedValue = ceil( d );
    }
    else if( decimals < 0.5 )
    {
        /* If the decimal part is smaller than 1/2
         */
        roundedValue = floor( d );
    }
    else
    {
        /* the decimals are exactly 1/2 so round according to
         * the bEvenNumber flag.
         */
        if( bEvenNumber )
        {
            roundedValue = floor( d );
        }
        else
        {
            roundedValue = ceil( d );
        }
    }

	return roundedValue * nSign;
}

/******************************************************************************
 *		RemoveCharacterFromString		[INTERNAL]
 *
 * Removes any of the characters in "strOfCharToRemove" from the "str" argument.
 */
static void RemoveCharacterFromString( LPSTR str, LPSTR strOfCharToRemove )
{
	LPSTR pNewString = NULL;
	LPSTR strToken = NULL;

	/* Check if we have a valid argument
	 */
	if( str != NULL )
	{
		pNewString = strdup( str );
		str[0] = '\0';
		strToken = strtok( pNewString, strOfCharToRemove );
		while( strToken != NULL ) {
			strcat( str, strToken );
			strToken = strtok( NULL, strOfCharToRemove );
		}
		free( pNewString );
	}
	return;
}

/******************************************************************************
 *		GetValidRealString		[INTERNAL]
 *
 * Checks if the string is of proper format to be converted to a real value.
 */
static BOOL IsValidRealString( LPSTR strRealString )
{
	/* Real values that have a decimal point are required to either have
	 * digits before or after the decimal point.  We will assume that
	 * we do not have any digits at either position. If we do encounter
	 * some we will disable this flag.
	 */
	BOOL bDigitsRequired = TRUE;
	/* Processed fields in the string representation of the real number.
	 */
	BOOL bWhiteSpaceProcessed = FALSE;
	BOOL bFirstSignProcessed = FALSE;
	BOOL bFirstDigitsProcessed = FALSE;
	BOOL bDecimalPointProcessed = FALSE;
	BOOL bSecondDigitsProcessed = FALSE;
	BOOL bExponentProcessed = FALSE;
	BOOL bSecondSignProcessed = FALSE;
	BOOL bThirdDigitsProcessed = FALSE;
	/* Assume string parameter "strRealString" is valid and try to disprove it.
	 */
	BOOL bValidRealString = TRUE;

	/* Used to count the number of tokens in the "strRealString".
	 */
	LPSTR strToken = NULL;
	int nTokens = 0;
	LPSTR pChar = NULL;

	/* Check if we have a valid argument
	 */
	if( strRealString == NULL )
	{
		bValidRealString = FALSE;
	}

	if( bValidRealString == TRUE )
	{
		/* Make sure we only have ONE token in the string.
		 */
		strToken = strtok( strRealString, " " );
		while( strToken != NULL ) {
			nTokens++;
			strToken = strtok( NULL, " " );
		}

		if( nTokens != 1 )
		{
			bValidRealString = FALSE;
		}
	}


	/* Make sure this token contains only valid characters.
	 * The string argument to atof has the following form:
	 * [whitespace] [sign] [digits] [.digits] [ {d | D | e | E }[sign]digits]
	 * Whitespace consists of space and|or <TAB> characters, which are ignored.
     * Sign is either plus '+' or minus '-'.
     * Digits are one or more decimal digits.
     * Note: If no digits appear before the decimal point, at least one must
     * appear after the decimal point.
     * The decimal digits may be followed by an exponent.
     * An Exponent consists of an introductory letter ( D, d, E, or e) and
	 * an optionally signed decimal integer.
	 */
	pChar = strRealString;
	while( bValidRealString == TRUE && *pChar != '\0' )
	{
		switch( *pChar )
		{
		/* If whitespace...
		 */
		case ' ':
		case '\t':
			if( bWhiteSpaceProcessed ||
				bFirstSignProcessed ||
				bFirstDigitsProcessed ||
				bDecimalPointProcessed ||
				bSecondDigitsProcessed ||
				bExponentProcessed ||
				bSecondSignProcessed ||
				bThirdDigitsProcessed )
			{
				bValidRealString = FALSE;
			}
			break;
		/* If sign...
		 */
		case '+':
		case '-':
			if( bFirstSignProcessed == FALSE )
			{
				if( bFirstDigitsProcessed ||
					bDecimalPointProcessed ||
					bSecondDigitsProcessed ||
					bExponentProcessed ||
					bSecondSignProcessed ||
					bThirdDigitsProcessed )
				{
					bValidRealString = FALSE;
				}
				bWhiteSpaceProcessed = TRUE;
				bFirstSignProcessed = TRUE;
			}
			else if( bSecondSignProcessed == FALSE )
			{
                /* Note: The exponent must be present in
				 * order to accept the second sign...
				 */
				if( bExponentProcessed == FALSE ||
					bThirdDigitsProcessed ||
					bDigitsRequired )
				{
					bValidRealString = FALSE;
				}
				bFirstSignProcessed = TRUE;
				bWhiteSpaceProcessed = TRUE;
				bFirstDigitsProcessed = TRUE;
				bDecimalPointProcessed = TRUE;
				bSecondDigitsProcessed = TRUE;
				bSecondSignProcessed = TRUE;
			}
			break;

		/* If decimals...
		 */
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if( bFirstDigitsProcessed == FALSE )
			{
				if( bDecimalPointProcessed ||
					bSecondDigitsProcessed ||
					bExponentProcessed ||
					bSecondSignProcessed ||
					bThirdDigitsProcessed )
				{
					bValidRealString = FALSE;
				}
				bFirstSignProcessed = TRUE;
				bWhiteSpaceProcessed = TRUE;
				/* We have found some digits before the decimal point
				 * so disable the "Digits required" flag.
				 */
				bDigitsRequired = FALSE;
			}
			else if( bSecondDigitsProcessed == FALSE )
			{
				if( bExponentProcessed ||
					bSecondSignProcessed ||
					bThirdDigitsProcessed )
				{
					bValidRealString = FALSE;
				}
				bFirstSignProcessed = TRUE;
				bWhiteSpaceProcessed = TRUE;
				bFirstDigitsProcessed = TRUE;
				bDecimalPointProcessed = TRUE;
				/* We have found some digits after the decimal point
				 * so disable the "Digits required" flag.
				 */
				bDigitsRequired = FALSE;
			}
			else if( bThirdDigitsProcessed == FALSE )
			{
				/* Getting here means everything else should be processed.
                 * If we get anything else than a decimal following this
                 * digit it will be flagged by the other cases, so
				 * we do not really need to do anything in here.
				 */
			}
			break;
		/* If DecimalPoint...
		 */
		case '.':
			if( bDecimalPointProcessed ||
				bSecondDigitsProcessed ||
				bExponentProcessed ||
				bSecondSignProcessed ||
				bThirdDigitsProcessed )
			{
				bValidRealString = FALSE;
			}
			bFirstSignProcessed = TRUE;
			bWhiteSpaceProcessed = TRUE;
			bFirstDigitsProcessed = TRUE;
			bDecimalPointProcessed = TRUE;
			break;
		/* If Exponent...
		 */
		case 'e':
		case 'E':
		case 'd':
		case 'D':
			if( bExponentProcessed ||
				bSecondSignProcessed ||
				bThirdDigitsProcessed ||
				bDigitsRequired )
			{
				bValidRealString = FALSE;
			}
			bFirstSignProcessed = TRUE;
			bWhiteSpaceProcessed = TRUE;
			bFirstDigitsProcessed = TRUE;
			bDecimalPointProcessed = TRUE;
			bSecondDigitsProcessed = TRUE;
			bExponentProcessed = TRUE;
			break;
		default:
			bValidRealString = FALSE;
			break;
		}
		/* Process next character.
		 */
		pChar++;
	}

	/* If the required digits were not present we have an invalid
	 * string representation of a real number.
	 */
	if( bDigitsRequired == TRUE )
	{
		bValidRealString = FALSE;
	}

	return bValidRealString;
}


/******************************************************************************
 *		Coerce	[INTERNAL]
 *
 * This function dispatches execution to the proper conversion API
 * to do the necessary coercion.
 *
 * FIXME: Passing down dwFlags to the conversion functions is wrong, this
 * 	  is a different flagmask. Check MSDN.
 */
static HRESULT Coerce( VARIANTARG* pd, LCID lcid, ULONG dwFlags, VARIANTARG* ps, VARTYPE vt )
{
	HRESULT res = S_OK;
	unsigned short vtFrom = 0;
	vtFrom = V_VT(ps) & VT_TYPEMASK;


	/* Note: Since "long" and "int" values both have 4 bytes and are
	 * both signed integers "int" will be treated as "long" in the
	 * following code.
	 * The same goes for their unsigned versions.
	 */

	/* Trivial Case: If the coercion is from two types that are
	 * identical then we can blindly copy from one argument to another.*/
	if ((vt==vtFrom))
	{
	   return VariantCopy(pd,ps);
	}

	/* Cases requiring thought*/
	switch( vt )
	{

    case( VT_EMPTY ):
        res = VariantClear( pd );
        break;
    case( VT_NULL ):
        res = VariantClear( pd );
        if( res == S_OK )
        {
            V_VT(pd) = VT_NULL;
        }
        break;
	case( VT_I1 ):
		switch( vtFrom )
        {
        case( VT_I1 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_I2 ):
			res = VarI1FromI2( V_UNION(ps,iVal), &V_UNION(pd,cVal) );
			break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarI1FromI4( V_UNION(ps,lVal), &V_UNION(pd,cVal) );
			break;
		case( VT_UI1 ):
			res = VarI1FromUI1( V_UNION(ps,bVal), &V_UNION(pd,cVal) );
			break;
		case( VT_UI2 ):
			res = VarI1FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,cVal) );
			break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarI1FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,cVal) );
			break;
		case( VT_R4 ):
			res = VarI1FromR4( V_UNION(ps,fltVal), &V_UNION(pd,cVal) );
			break;
		case( VT_R8 ):
			res = VarI1FromR8( V_UNION(ps,dblVal), &V_UNION(pd,cVal) );
			break;
		case( VT_DATE ):
			res = VarI1FromDate( V_UNION(ps,date), &V_UNION(pd,cVal) );
			break;
		case( VT_BOOL ):
			res = VarI1FromBool( V_UNION(ps,boolVal), &V_UNION(pd,cVal) );
			break;
		case( VT_BSTR ):
			res = VarI1FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,cVal) );
			break;
		case( VT_CY ):
			res = VarI1FromCy( V_UNION(ps,cyVal), &V_UNION(pd,cVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarI1FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,cVal) );*/
		case( VT_DECIMAL ):
			/*res = VarI1FromDec( V_UNION(ps,decVal), &V_UNION(pd,cVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_I2 ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarI2FromI1( V_UNION(ps,cVal), &V_UNION(pd,iVal) );
			break;
        case( VT_I2 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarI2FromI4( V_UNION(ps,lVal), &V_UNION(pd,iVal) );
			break;
		case( VT_UI1 ):
			res = VarI2FromUI1( V_UNION(ps,bVal), &V_UNION(pd,iVal) );
			break;
		case( VT_UI2 ):
			res = VarI2FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,iVal) );
			break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarI2FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,iVal) );
			break;
		case( VT_R4 ):
			res = VarI2FromR4( V_UNION(ps,fltVal), &V_UNION(pd,iVal) );
			break;
		case( VT_R8 ):
			res = VarI2FromR8( V_UNION(ps,dblVal), &V_UNION(pd,iVal) );
			break;
		case( VT_DATE ):
			res = VarI2FromDate( V_UNION(ps,date), &V_UNION(pd,iVal) );
			break;
		case( VT_BOOL ):
			res = VarI2FromBool( V_UNION(ps,boolVal), &V_UNION(pd,iVal) );
			break;
		case( VT_BSTR ):
			res = VarI2FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,iVal) );
			break;
		case( VT_CY ):
			res = VarI2FromCy( V_UNION(ps,cyVal), &V_UNION(pd,iVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarI2FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,iVal) );*/
		case( VT_DECIMAL ):
			/*res = VarI2FromDec( V_UNION(ps,deiVal), &V_UNION(pd,iVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_INT ):
	case( VT_I4 ):
		switch( vtFrom )
		{
		case( VT_EMPTY ):
		        V_UNION(pd,lVal) = 0;
		        res = S_OK;
		    	break;
		case( VT_I1 ):
			res = VarI4FromI1( V_UNION(ps,cVal), &V_UNION(pd,lVal) );
			break;
		case( VT_I2 ):
			res = VarI4FromI2( V_UNION(ps,iVal), &V_UNION(pd,lVal) );
            break;
        case( VT_INT ):
        case( VT_I4 ):
#ifdef __WIN32OS2__
        case( VT_HRESULT ):
#endif
            res = VariantCopy( pd, ps );
            break;
		case( VT_UI1 ):
			res = VarI4FromUI1( V_UNION(ps,bVal), &V_UNION(pd,lVal) );
			break;
		case( VT_UI2 ):
			res = VarI4FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,lVal) );
			break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarI4FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,lVal) );
			break;
		case( VT_R4 ):
			res = VarI4FromR4( V_UNION(ps,fltVal), &V_UNION(pd,lVal) );
			break;
		case( VT_R8 ):
			res = VarI4FromR8( V_UNION(ps,dblVal), &V_UNION(pd,lVal) );
			break;
		case( VT_DATE ):
			res = VarI4FromDate( V_UNION(ps,date), &V_UNION(pd,lVal) );
			break;
		case( VT_BOOL ):
			res = VarI4FromBool( V_UNION(ps,boolVal), &V_UNION(pd,lVal) );
			break;
		case( VT_BSTR ):
			res = VarI4FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,lVal) );
			break;
		case( VT_CY ):
			res = VarI4FromCy( V_UNION(ps,cyVal), &V_UNION(pd,lVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarI4FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,lVal) );*/
		case( VT_DECIMAL ):
			/*res = VarI4FromDec( V_UNION(ps,deiVal), &V_UNION(pd,lVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_UI1 ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarUI1FromI1( V_UNION(ps,cVal), &V_UNION(pd,bVal) );
			break;
		case( VT_I2 ):
			res = VarUI1FromI2( V_UNION(ps,iVal), &V_UNION(pd,bVal) );
			break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarUI1FromI4( V_UNION(ps,lVal), &V_UNION(pd,bVal) );
			break;
        case( VT_UI1 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_UI2 ):
			res = VarUI1FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,bVal) );
			break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarUI1FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,bVal) );
			break;
		case( VT_R4 ):
			res = VarUI1FromR4( V_UNION(ps,fltVal), &V_UNION(pd,bVal) );
			break;
		case( VT_R8 ):
			res = VarUI1FromR8( V_UNION(ps,dblVal), &V_UNION(pd,bVal) );
			break;
		case( VT_DATE ):
			res = VarUI1FromDate( V_UNION(ps,date), &V_UNION(pd,bVal) );
			break;
		case( VT_BOOL ):
			res = VarUI1FromBool( V_UNION(ps,boolVal), &V_UNION(pd,bVal) );
			break;
		case( VT_BSTR ):
			res = VarUI1FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,bVal) );
			break;
		case( VT_CY ):
			res = VarUI1FromCy( V_UNION(ps,cyVal), &V_UNION(pd,bVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarUI1FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,bVal) );*/
		case( VT_DECIMAL ):
			/*res = VarUI1FromDec( V_UNION(ps,deiVal), &V_UNION(pd,bVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_UI2 ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarUI2FromI1( V_UNION(ps,cVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_I2 ):
			res = VarUI2FromI2( V_UNION(ps,iVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarUI2FromI4( V_UNION(ps,lVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_UI1 ):
			res = VarUI2FromUI1( V_UNION(ps,bVal), &V_UNION(pd,uiVal) );
			break;
        case( VT_UI2 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarUI2FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_R4 ):
			res = VarUI2FromR4( V_UNION(ps,fltVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_R8 ):
			res = VarUI2FromR8( V_UNION(ps,dblVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_DATE ):
			res = VarUI2FromDate( V_UNION(ps,date), &V_UNION(pd,uiVal) );
			break;
		case( VT_BOOL ):
			res = VarUI2FromBool( V_UNION(ps,boolVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_BSTR ):
			res = VarUI2FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,uiVal) );
			break;
		case( VT_CY ):
			res = VarUI2FromCy( V_UNION(ps,cyVal), &V_UNION(pd,uiVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarUI2FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,uiVal) );*/
		case( VT_DECIMAL ):
			/*res = VarUI2FromDec( V_UNION(ps,deiVal), &V_UNION(pd,uiVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_UINT ):
	case( VT_UI4 ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarUI4FromI1( V_UNION(ps,cVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_I2 ):
			res = VarUI4FromI2( V_UNION(ps,iVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarUI4FromI4( V_UNION(ps,lVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_UI1 ):
			res = VarUI4FromUI1( V_UNION(ps,bVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_UI2 ):
			res = VarUI4FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,ulVal) );
			break;
        case( VT_UI4 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_R4 ):
			res = VarUI4FromR4( V_UNION(ps,fltVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_R8 ):
			res = VarUI4FromR8( V_UNION(ps,dblVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_DATE ):
			res = VarUI4FromDate( V_UNION(ps,date), &V_UNION(pd,ulVal) );
			break;
		case( VT_BOOL ):
			res = VarUI4FromBool( V_UNION(ps,boolVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_BSTR ):
			res = VarUI4FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,ulVal) );
			break;
		case( VT_CY ):
			res = VarUI4FromCy( V_UNION(ps,cyVal), &V_UNION(pd,ulVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarUI4FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,ulVal) );*/
		case( VT_DECIMAL ):
			/*res = VarUI4FromDec( V_UNION(ps,deiVal), &V_UNION(pd,ulVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_R4 ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarR4FromI1( V_UNION(ps,cVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_I2 ):
			res = VarR4FromI2( V_UNION(ps,iVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarR4FromI4( V_UNION(ps,lVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_UI1 ):
			res = VarR4FromUI1( V_UNION(ps,bVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_UI2 ):
			res = VarR4FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarR4FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,fltVal) );
			break;
        case( VT_R4 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_R8 ):
			res = VarR4FromR8( V_UNION(ps,dblVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_DATE ):
			res = VarR4FromDate( V_UNION(ps,date), &V_UNION(pd,fltVal) );
			break;
		case( VT_BOOL ):
			res = VarR4FromBool( V_UNION(ps,boolVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_BSTR ):
			res = VarR4FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,fltVal) );
			break;
		case( VT_CY ):
			res = VarR4FromCy( V_UNION(ps,cyVal), &V_UNION(pd,fltVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarR4FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,fltVal) );*/
		case( VT_DECIMAL ):
			/*res = VarR4FromDec( V_UNION(ps,deiVal), &V_UNION(pd,fltVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_R8 ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarR8FromI1( V_UNION(ps,cVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_I2 ):
			res = VarR8FromI2( V_UNION(ps,iVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_INT ):
		case( VT_I4 ):
			res = VarR8FromI4( V_UNION(ps,lVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_UI1 ):
			res = VarR8FromUI1( V_UNION(ps,bVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_UI2 ):
			res = VarR8FromUI2( V_UNION(ps,uiVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_UINT ):
		case( VT_UI4 ):
			res = VarR8FromUI4( V_UNION(ps,ulVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_R4 ):
			res = VarR8FromR4( V_UNION(ps,fltVal), &V_UNION(pd,dblVal) );
			break;
        case( VT_R8 ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_DATE ):
			res = VarR8FromDate( V_UNION(ps,date), &V_UNION(pd,dblVal) );
			break;
		case( VT_BOOL ):
			res = VarR8FromBool( V_UNION(ps,boolVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_BSTR ):
			res = VarR8FromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,dblVal) );
			break;
		case( VT_CY ):
			res = VarR8FromCy( V_UNION(ps,cyVal), &V_UNION(pd,dblVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarR8FromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,dblVal) );*/
		case( VT_DECIMAL ):
			/*res = VarR8FromDec( V_UNION(ps,deiVal), &V_UNION(pd,dblVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_DATE ):
		switch( vtFrom )
		{
		case( VT_I1 ):
			res = VarDateFromI1( V_UNION(ps,cVal), &V_UNION(pd,date) );
			break;
		case( VT_I2 ):
			res = VarDateFromI2( V_UNION(ps,iVal), &V_UNION(pd,date) );
			break;
		case( VT_INT ):
			res = VarDateFromInt( V_UNION(ps,intVal), &V_UNION(pd,date) );
			break;
		case( VT_I4 ):
			res = VarDateFromI4( V_UNION(ps,lVal), &V_UNION(pd,date) );
			break;
		case( VT_UI1 ):
			res = VarDateFromUI1( V_UNION(ps,bVal), &V_UNION(pd,date) );
			break;
		case( VT_UI2 ):
			res = VarDateFromUI2( V_UNION(ps,uiVal), &V_UNION(pd,date) );
			break;
		case( VT_UINT ):
			res = VarDateFromUint( V_UNION(ps,uintVal), &V_UNION(pd,date) );
			break;
		case( VT_UI4 ):
			res = VarDateFromUI4( V_UNION(ps,ulVal), &V_UNION(pd,date) );
			break;
		case( VT_R4 ):
			res = VarDateFromR4( V_UNION(ps,fltVal), &V_UNION(pd,date) );
			break;
		case( VT_R8 ):
			res = VarDateFromR8( V_UNION(ps,dblVal), &V_UNION(pd,date) );
			break;
        case( VT_DATE ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_BOOL ):
			res = VarDateFromBool( V_UNION(ps,boolVal), &V_UNION(pd,date) );
			break;
		case( VT_BSTR ):
			res = VarDateFromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,date) );
			break;
		case( VT_CY ):
			res = VarDateFromCy( V_UNION(ps,cyVal), &V_UNION(pd,date) );
			break;
		case( VT_DISPATCH ):
			/*res = VarDateFromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,date) );*/
		case( VT_DECIMAL ):
			/*res = VarDateFromDec( V_UNION(ps,deiVal), &V_UNION(pd,date) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_BOOL ):
		switch( vtFrom )
		{
		case( VT_EMPTY ):
		    	res = S_OK;
			V_UNION(pd,boolVal) = VARIANT_FALSE;
			break;
		case( VT_I1 ):
			res = VarBoolFromI1( V_UNION(ps,cVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_I2 ):
			res = VarBoolFromI2( V_UNION(ps,iVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_INT ):
			res = VarBoolFromInt( V_UNION(ps,intVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_I4 ):
			res = VarBoolFromI4( V_UNION(ps,lVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_UI1 ):
			res = VarBoolFromUI1( V_UNION(ps,bVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_UI2 ):
			res = VarBoolFromUI2( V_UNION(ps,uiVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_UINT ):
			res = VarBoolFromUint( V_UNION(ps,uintVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_UI4 ):
			res = VarBoolFromUI4( V_UNION(ps,ulVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_R4 ):
			res = VarBoolFromR4( V_UNION(ps,fltVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_R8 ):
			res = VarBoolFromR8( V_UNION(ps,dblVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_DATE ):
			res = VarBoolFromDate( V_UNION(ps,date), &V_UNION(pd,boolVal) );
			break;
        case( VT_BOOL ):
            res = VariantCopy( pd, ps );
            break;
		case( VT_BSTR ):
			res = VarBoolFromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,boolVal) );
			break;
		case( VT_CY ):
			res = VarBoolFromCy( V_UNION(ps,cyVal), &V_UNION(pd,boolVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarBoolFromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,boolVal) );*/
		case( VT_DECIMAL ):
			/*res = VarBoolFromDec( V_UNION(ps,deiVal), &V_UNION(pd,boolVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

	case( VT_BSTR ):
		switch( vtFrom )
		{
		case( VT_EMPTY ):
			if ((V_UNION(pd,bstrVal) = SysAllocStringLen(NULL, 0)))
				res = S_OK;
			else
				res = E_OUTOFMEMORY;
			break;
		case( VT_I1 ):
			res = VarBstrFromI1( V_UNION(ps,cVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_I2 ):
			res = VarBstrFromI2( V_UNION(ps,iVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_INT ):
			res = VarBstrFromInt( V_UNION(ps,intVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_I4 ):
			res = VarBstrFromI4( V_UNION(ps,lVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_UI1 ):
			res = VarBstrFromUI1( V_UNION(ps,bVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_UI2 ):
			res = VarBstrFromUI2( V_UNION(ps,uiVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_UINT ):
			res = VarBstrFromUint( V_UNION(ps,uintVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_UI4 ):
			res = VarBstrFromUI4( V_UNION(ps,ulVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_R4 ):
			res = VarBstrFromR4( V_UNION(ps,fltVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_R8 ):
			res = VarBstrFromR8( V_UNION(ps,dblVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_DATE ):
			res = VarBstrFromDate( V_UNION(ps,date), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_BOOL ):
			res = VarBstrFromBool( V_UNION(ps,boolVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_BSTR ):
			res = VariantCopy( pd, ps );
			break;
		case( VT_CY ):
			res = VarBstrFromCy( V_UNION(ps,cyVal), lcid, 0, &V_UNION(pd,bstrVal) );
			break;
		case( VT_DISPATCH ):
			/*res = VarBstrFromDisp( V_UNION(ps,pdispVal), lcid, 0, &(pd,bstrVal) );*/
		case( VT_DECIMAL ):
			/*res = VarBstrFromDec( V_UNION(ps,deiVal), lcid, 0, &(pd,bstrVal) );*/
		case( VT_UNKNOWN ):
		default:
			res = DISP_E_TYPEMISMATCH;
			FIXME("Coercion from %d to %d\n", vtFrom, vt );
			break;
		}
		break;

     case( VT_CY ):
	switch( vtFrom )
	  {
	  case( VT_I1 ):
	     res = VarCyFromI1( V_UNION(ps,cVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_I2 ):
	     res = VarCyFromI2( V_UNION(ps,iVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_INT ):
	     res = VarCyFromInt( V_UNION(ps,intVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_I4 ):
	     res = VarCyFromI4( V_UNION(ps,lVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_UI1 ):
	     res = VarCyFromUI1( V_UNION(ps,bVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_UI2 ):
	     res = VarCyFromUI2( V_UNION(ps,uiVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_UINT ):
	     res = VarCyFromUint( V_UNION(ps,uintVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_UI4 ):
	     res = VarCyFromUI4( V_UNION(ps,ulVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_R4 ):
	     res = VarCyFromR4( V_UNION(ps,fltVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_R8 ):
	     res = VarCyFromR8( V_UNION(ps,dblVal), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_DATE ):
	     res = VarCyFromDate( V_UNION(ps,date), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_BOOL ):
	     res = VarCyFromBool( V_UNION(ps,date), &V_UNION(pd,cyVal) );
	     break;
	  case( VT_CY ):
	     res = VariantCopy( pd, ps );
	     break;
	  case( VT_BSTR ):
	     res = VarCyFromStr( V_UNION(ps,bstrVal), lcid, 0, &V_UNION(pd,cyVal) );
	     break;
	  case( VT_DISPATCH ):
	     /*res = VarCyFromDisp( V_UNION(ps,pdispVal), lcid, &V_UNION(pd,cyVal) );*/
	  case( VT_DECIMAL ):
	     /*res = VarCyFromDec( V_UNION(ps,deiVal), &V_UNION(pd,cyVal) );*/
	     break;
	  case( VT_UNKNOWN ):
	  default:
	     res = DISP_E_TYPEMISMATCH;
	     FIXME("Coercion from %d to %d\n", vtFrom, vt );
	     break;
	  }
	break;

	case( VT_UNKNOWN ):
	    if (vtFrom == VT_DISPATCH)
	    {
		res = IDispatch_QueryInterface(V_DISPATCH(ps), &IID_IUnknown, (LPVOID*)&V_UNKNOWN(pd));
	    }
	    else
	    {
		res = DISP_E_TYPEMISMATCH;
		FIXME("Coercion from %d to %d\n", vtFrom, vt );
	    }
	    break;

	default:
		res = DISP_E_TYPEMISMATCH;
		FIXME("Coercion from %d to %d\n", vtFrom, vt );
		break;
	}

	return res;
}

/******************************************************************************
 *		ValidateVtRange	[INTERNAL]
 *
 * Used internally by the hi-level Variant API to determine
 * if the vartypes are valid.
 */
static HRESULT WINAPI ValidateVtRange( VARTYPE vt )
{
    /* if by value we must make sure it is in the
     * range of the valid types.
     */
    if( ( vt & VT_TYPEMASK ) > VT_MAXVALIDTYPE )
    {
        return DISP_E_BADVARTYPE;
    }
    return S_OK;
}


/******************************************************************************
 *		ValidateVartype	[INTERNAL]
 *
 * Used internally by the hi-level Variant API to determine
 * if the vartypes are valid.
 */
static HRESULT WINAPI ValidateVariantType( VARTYPE vt )
{
	HRESULT res = S_OK;

	/* check if we have a valid argument.
	 */
	if( vt & VT_BYREF )
    {
        /* if by reference check that the type is in
         * the valid range and that it is not of empty or null type
         */
        if( ( vt & VT_TYPEMASK ) == VT_EMPTY ||
            ( vt & VT_TYPEMASK ) == VT_NULL ||
			( vt & VT_TYPEMASK ) > VT_MAXVALIDTYPE )
		{
			res = DISP_E_BADVARTYPE;
		}

    }
    else
    {
        res = ValidateVtRange( vt );
    }

	return res;
}

/******************************************************************************
 *		ValidateVt	[INTERNAL]
 *
 * Used internally by the hi-level Variant API to determine
 * if the vartypes are valid.
 */
static HRESULT WINAPI ValidateVt( VARTYPE vt )
{
	HRESULT res = S_OK;

	/* check if we have a valid argument.
	 */
	if( vt & VT_BYREF )
    {
        /* if by reference check that the type is in
         * the valid range and that it is not of empty or null type
         */
        if( ( vt & VT_TYPEMASK ) == VT_EMPTY ||
            ( vt & VT_TYPEMASK ) == VT_NULL ||
			( vt & VT_TYPEMASK ) > VT_MAXVALIDTYPE )
		{
			res = DISP_E_BADVARTYPE;
		}

    }
    else
    {
        res = ValidateVtRange( vt );
    }

	return res;
}





/******************************************************************************
 *		VariantInit	[OLEAUT32.8]
 *
 * Initializes the Variant.  Unlike VariantClear it does not interpret
 * the current contents of the Variant.
 */
void WINAPI VariantInit(VARIANTARG* pvarg)
{
  TRACE("%s: (%p)\n", __FUNCTION__, pvarg);

  memset(pvarg, 0, sizeof (VARIANTARG));
  V_VT(pvarg) = VT_EMPTY;

  return;
}

/******************************************************************************
 *		VariantClear	[OLEAUT32.9]
 *
 * This function clears the VARIANT by setting the vt field to VT_EMPTY. It also
 * sets the wReservedX field to 0.	The current contents of the VARIANT are
 * freed.  If the vt is VT_BSTR the string is freed. If VT_DISPATCH the object is
 * released. If VT_ARRAY the array is freed.
 */
HRESULT WINAPI VariantClear(VARIANTARG* pvarg)
{
  HRESULT res = S_OK;
  TRACE("%s: (%p)\n",__FUNCTION__, pvarg);

  res = ValidateVariantType( V_VT(pvarg) );
  if( res == S_OK )
  {
    if( !( V_VT(pvarg) & VT_BYREF ) )
    {
      /*
       * The VT_ARRAY flag is a special case of a safe array.
       */
      if ( (V_VT(pvarg) & VT_ARRAY) != 0)
      {
	SafeArrayDestroy(V_UNION(pvarg,parray));
      }
      else
      {
	switch( V_VT(pvarg) & VT_TYPEMASK )
	{
	  case( VT_BSTR ):
	    SysFreeString( V_UNION(pvarg,bstrVal) );
	    break;
	  case( VT_DISPATCH ):
	    if(V_UNION(pvarg,pdispVal)!=NULL)
	      ICOM_CALL(Release,V_UNION(pvarg,pdispVal));
	    break;
	  case( VT_VARIANT ):
	    VariantClear(V_UNION(pvarg,pvarVal));
	    break;
	  case( VT_UNKNOWN ):
	    if(V_UNION(pvarg,punkVal)!=NULL)
	      ICOM_CALL(Release,V_UNION(pvarg,punkVal));
	    break;
	  case( VT_SAFEARRAY ):
	    SafeArrayDestroy(V_UNION(pvarg,parray));
	    break;
	  default:
	    break;
	}
      }
    }

    /*
     * Empty all the fields and mark the type as empty.
     */
    memset(pvarg, 0, sizeof (VARIANTARG));
    V_VT(pvarg) = VT_EMPTY;
  }

  return res;
}

/******************************************************************************
 *		VariantCopy	[OLEAUT32.10]
 *
 * Frees up the designation variant and makes a copy of the source.
 */
HRESULT WINAPI VariantCopy(VARIANTARG* pvargDest, VARIANTARG* pvargSrc)
{
  HRESULT res = S_OK;

  TRACE("%s: (Dest: %p, Src: %p), vt=%d\n", __FUNCTION__, pvargDest, pvargSrc, V_VT(pvargSrc));

  res = ValidateVariantType( V_VT(pvargSrc) );

  /* If the pointer are to the same variant we don't need
   * to do anything.
   */
  if( pvargDest != pvargSrc && res == S_OK )
  {
    res = VariantClear( pvargDest );

    if( res == S_OK )
    {
      if( V_VT(pvargSrc) & VT_BYREF )
      {
	/* In the case of byreference we only need
	 * to copy the pointer.
	 */
	pvargDest->n1.n2.n3 = pvargSrc->n1.n2.n3;
	V_VT(pvargDest) = V_VT(pvargSrc);
      }
      else
      {
	/*
	 * The VT_ARRAY flag is another way to designate a safe array.
	 */
	if (V_VT(pvargSrc) & VT_ARRAY)
	{
	  SafeArrayCopy(V_UNION(pvargSrc,parray), &V_UNION(pvargDest,parray));
	}
	else
	{
	  /* In the case of by value we need to
	   * copy the actual value. In the case of
	   * VT_BSTR a copy of the string is made,
	   * if VT_DISPATCH or VT_IUNKNOWN AddRef is
	   * called to increment the object's reference count.
	   */
	  switch( V_VT(pvargSrc) & VT_TYPEMASK )
	  {
	    case( VT_BSTR ):
	      V_UNION(pvargDest,bstrVal) = SYSDUPSTRING( V_UNION(pvargSrc,bstrVal) );
	      break;
	    case( VT_DISPATCH ):
	      V_UNION(pvargDest,pdispVal) = V_UNION(pvargSrc,pdispVal);
	      if (V_UNION(pvargDest,pdispVal)!=NULL)
		ICOM_CALL(AddRef,V_UNION(pvargDest,pdispVal));
	      break;
	    case( VT_VARIANT ):
	      VariantCopy(V_UNION(pvargDest,pvarVal),V_UNION(pvargSrc,pvarVal));
	      break;
	    case( VT_UNKNOWN ):
	      V_UNION(pvargDest,punkVal) = V_UNION(pvargSrc,punkVal);
	      if (V_UNION(pvargDest,pdispVal)!=NULL)
		ICOM_CALL(AddRef,V_UNION(pvargDest,punkVal));
	      break;
	    case( VT_SAFEARRAY ):
	      SafeArrayCopy(V_UNION(pvargSrc,parray), &V_UNION(pvargDest,parray));
	      break;
	    default:
	      pvargDest->n1.n2.n3 = pvargSrc->n1.n2.n3;
	      break;
	  }
	}

	V_VT(pvargDest) = V_VT(pvargSrc);
      }
    }
  }

  return res;
}


/******************************************************************************
 *		VariantCopyInd	[OLEAUT32.11]
 *
 * Frees up the destination variant and makes a copy of the source.  If
 * the source is of type VT_BYREF it performs the necessary indirections.
 */
HRESULT WINAPI VariantCopyInd(VARIANT* pvargDest, VARIANTARG* pvargSrc)
{
  HRESULT res = S_OK;

  TRACE("(%p, %p)\n", pvargDest, pvargSrc);

  res = ValidateVariantType( V_VT(pvargSrc) );

  if( res != S_OK )
    return res;

  if( V_VT(pvargSrc) & VT_BYREF )
  {
    VARIANTARG varg;
    VariantInit( &varg );

    /* handle the in place copy.
     */
    if( pvargDest == pvargSrc )
    {
      /* we will use a copy of the source instead.
       */
      res = VariantCopy( &varg, pvargSrc );
      pvargSrc = &varg;
    }

    if( res == S_OK )
    {
      res = VariantClear( pvargDest );

      if( res == S_OK )
      {
	/*
	 * The VT_ARRAY flag is another way to designate a safearray variant.
	 */
	if ( V_VT(pvargSrc) & VT_ARRAY)
	{
	  SafeArrayCopy(*V_UNION(pvargSrc,pparray), &V_UNION(pvargDest,parray));
	}
	else
	{
	  /* In the case of by reference we need
	   * to copy the date pointed to by the variant.
	   */

	  /* Get the variant type.
	   */
	  switch( V_VT(pvargSrc) & VT_TYPEMASK )
	  {
	    case( VT_BSTR ):
	      V_UNION(pvargDest,bstrVal) = SYSDUPSTRING( *(V_UNION(pvargSrc,pbstrVal)) );
	      break;
	    case( VT_DISPATCH ):
	      break;
	    case( VT_VARIANT ):
	      {
		/* Prevent from cycling.  According to tests on
		 * VariantCopyInd in Windows and the documentation
		 * this API dereferences the inner Variants to only one depth.
		 * If the inner Variant itself contains an
		 * other inner variant the E_INVALIDARG error is
		 * returned.
		 */
		if( pvargSrc->n1.n2.wReserved1 & PROCESSING_INNER_VARIANT )
		{
		  /* If we get here we are attempting to deference
		   * an inner variant that that is itself contained
		   * in an inner variant so report E_INVALIDARG error.
		   */
		  res = E_INVALIDARG;
		}
		else
		{
		  /* Set the processing inner variant flag.
		   * We will set this flag in the inner variant
		   * that will be passed to the VariantCopyInd function.
		   */
		  (V_UNION(pvargSrc,pvarVal))->n1.n2.wReserved1 |= PROCESSING_INNER_VARIANT;

		  /* Dereference the inner variant.
		   */
		  res = VariantCopyInd( pvargDest, V_UNION(pvargSrc,pvarVal) );
		  /* We must also copy its type, I think.
		   */
		  V_VT(pvargSrc) = V_VT(V_UNION(pvargSrc,pvarVal));
		}
	      }
	      break;
	    case( VT_UNKNOWN ):
	      break;
	    case( VT_SAFEARRAY ):
	      SafeArrayCopy(*V_UNION(pvargSrc,pparray), &V_UNION(pvargDest,parray));
	      break;
	    default:
	      /* This is a by reference Variant which means that the union
	       * part of the Variant contains a pointer to some data of
	       * type "V_VT(pvargSrc) & VT_TYPEMASK".
	       * We will deference this data in a generic fashion using
	       * the void pointer "Variant.u.byref".
	       * We will copy this data into the union of the destination
	       * Variant.
	       */
	      memcpy( &pvargDest->n1.n2.n3, V_UNION(pvargSrc,byref), SizeOfVariantData( pvargSrc ) );
	      break;
	  }
	}

        if (res == S_OK) V_VT(pvargDest) = V_VT(pvargSrc) & VT_TYPEMASK;
      }
    }

    /* this should not fail.
     */
    VariantClear( &varg );
  }
  else
  {
    res = VariantCopy( pvargDest, pvargSrc );
  }

  return res;
}

/******************************************************************************
 *		VariantChangeType	[OLEAUT32.12]
 */
HRESULT WINAPI VariantChangeType(VARIANTARG* pvargDest, VARIANTARG* pvargSrc,
							USHORT wFlags, VARTYPE vt)
{
	return VariantChangeTypeEx( pvargDest, pvargSrc, 0, wFlags, vt );
}

/******************************************************************************
 *		VariantChangeTypeEx	[OLEAUT32.147]
 */
HRESULT WINAPI VariantChangeTypeEx(VARIANTARG* pvargDest, VARIANTARG* pvargSrc,
							  LCID lcid, USHORT wFlags, VARTYPE vt)
{
	HRESULT res = S_OK;
	VARIANTARG varg;
	VariantInit( &varg );
	TRACE("%s: (Dest: %p, source: %p, LCID: %ld, wFlags: %u, VARTYPE: %u) vt=%d\n", __FUNCTION__, pvargDest, pvargSrc, lcid, wFlags, vt, V_VT(pvargSrc));
    TRACE("Src Var:\n");
    dump_Variant(pvargSrc);

	/* validate our source argument.
	 */
	res = ValidateVariantType( V_VT(pvargSrc) );

	/* validate the vartype.
	 */
	if( res == S_OK )
	{
		res = ValidateVt( vt );
	}

	/* if we are doing an in-place conversion make a copy of the source.
	 */
	if( res == S_OK && pvargDest == pvargSrc )
	{
		res = VariantCopy( &varg, pvargSrc );
		pvargSrc = &varg;
	}

	if( res == S_OK )
	{
		/* free up the destination variant.
		 */
		res = VariantClear( pvargDest );
	}

	if( res == S_OK )
	{
		if( V_VT(pvargSrc) & VT_BYREF )
		{
			/* Convert the source variant to a "byvalue" variant.
			 */
			VARIANTARG Variant;

			VariantInit( &Variant );
			res = VariantCopyInd( &Variant, pvargSrc );
			if( res == S_OK )
			{
				res = Coerce( pvargDest, lcid, wFlags, &Variant, vt );
				/* this should not fail.
				 */
				VariantClear( &Variant );
			}
		}
		else
		{
			/* Use the current "byvalue" source variant.
			 */
			res = Coerce( pvargDest, lcid, wFlags, pvargSrc, vt );
		}
	}
	/* this should not fail.
	 */
	VariantClear( &varg );

	/* set the type of the destination
	 */
	if ( res == S_OK )
		V_VT(pvargDest) = vt;

    TRACE("Dest Var (re: %x)\n", res);
    dump_Variant(pvargDest);

	return res;
}




/******************************************************************************
 *		VarUI1FromI2		[OLEAUT32.130]
 */
HRESULT WINAPI VarUI1FromI2(short sIn, BYTE* pbOut)
{
	TRACE("( %d, %p ), stub\n", sIn, pbOut );

	/* Check range of value.
	 */
	if( sIn < UI1_MIN || sIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) sIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromI4		[OLEAUT32.131]
 */
HRESULT WINAPI VarUI1FromI4(LONG lIn, BYTE* pbOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, pbOut );

	/* Check range of value.
	 */
	if( lIn < UI1_MIN || lIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) lIn;

	return S_OK;
}


/******************************************************************************
 *		VarUI1FromR4		[OLEAUT32.132]
 */
HRESULT WINAPI VarUI1FromR4(FLOAT fltIn, BYTE* pbOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, pbOut );

	/* Check range of value.
     */
    fltIn = round( fltIn );
	if( fltIn < UI1_MIN || fltIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromR8		[OLEAUT32.133]
 */
HRESULT WINAPI VarUI1FromR8(double dblIn, BYTE* pbOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, pbOut );

	/* Check range of value.
     */
    dblIn = round( dblIn );
	if( dblIn < UI1_MIN || dblIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromDate		[OLEAUT32.135]
 */
HRESULT WINAPI VarUI1FromDate(DATE dateIn, BYTE* pbOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, pbOut );

	/* Check range of value.
     */
    dateIn = round( dateIn );
	if( dateIn < UI1_MIN || dateIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromBool		[OLEAUT32.138]
 */
HRESULT WINAPI VarUI1FromBool(VARIANT_BOOL boolIn, BYTE* pbOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, pbOut );

	*pbOut = (BYTE) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromI1		[OLEAUT32.237]
 */
HRESULT WINAPI VarUI1FromI1(CHAR cIn, BYTE* pbOut)
{
	TRACE("( %c, %p ), stub\n", cIn, pbOut );

	*pbOut = cIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromUI2		[OLEAUT32.238]
 */
HRESULT WINAPI VarUI1FromUI2(USHORT uiIn, BYTE* pbOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pbOut );

	/* Check range of value.
	 */
	if( uiIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI1FromUI4		[OLEAUT32.239]
 */
HRESULT WINAPI VarUI1FromUI4(ULONG ulIn, BYTE* pbOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, pbOut );

	/* Check range of value.
	 */
	if( ulIn > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) ulIn;

	return S_OK;
}


/******************************************************************************
 *		VarUI1FromStr		[OLEAUT32.136]
 */
HRESULT WINAPI VarUI1FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, BYTE* pbOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("( %p, 0x%08lx, 0x%08lx, %p ), stub\n", strIn, lcid, dwFlags, pbOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0 , pNewString );

	/* Check range of value.
     */
    dValue = round( dValue );
	if( dValue < UI1_MIN || dValue > UI1_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pbOut = (BYTE) dValue;

	return S_OK;
}

/**********************************************************************
 *              VarUI1FromCy [OLEAUT32.134]
 * Convert currency to unsigned char
 */
HRESULT WINAPI VarUI1FromCy(CY cyIn, BYTE* pbOut) {
   double t = round((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (t > UI1_MAX || t < UI1_MIN) return DISP_E_OVERFLOW;

   *pbOut = (BYTE)t;
   return S_OK;
}

/******************************************************************************
 *		VarI2FromUI1		[OLEAUT32.48]
 */
HRESULT WINAPI VarI2FromUI1(BYTE bIn, short* psOut)
{
	TRACE("( 0x%08x, %p ), stub\n", bIn, psOut );

	*psOut = (short) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromI4		[OLEAUT32.49]
 */
HRESULT WINAPI VarI2FromI4(LONG lIn, short* psOut)
{
	TRACE("( %lx, %p ), stub\n", lIn, psOut );

	/* Check range of value.
	 */
	if( lIn < I2_MIN || lIn > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromR4		[OLEAUT32.50]
 */
HRESULT WINAPI VarI2FromR4(FLOAT fltIn, short* psOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, psOut );

	/* Check range of value.
     */
    fltIn = round( fltIn );
	if( fltIn < I2_MIN || fltIn > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromR8		[OLEAUT32.51]
 */
HRESULT WINAPI VarI2FromR8(double dblIn, short* psOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, psOut );

	/* Check range of value.
     */
    dblIn = round( dblIn );
	if( dblIn < I2_MIN || dblIn > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromDate		[OLEAUT32.53]
 */
HRESULT WINAPI VarI2FromDate(DATE dateIn, short* psOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, psOut );

	/* Check range of value.
     */
    dateIn = round( dateIn );
	if( dateIn < I2_MIN || dateIn > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromBool		[OLEAUT32.56]
 */
HRESULT WINAPI VarI2FromBool(VARIANT_BOOL boolIn, short* psOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, psOut );

	*psOut = (short) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromI1		[OLEAUT32.205]
 */
HRESULT WINAPI VarI2FromI1(CHAR cIn, short* psOut)
{
	TRACE("( %c, %p ), stub\n", cIn, psOut );

	*psOut = (short) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromUI2		[OLEAUT32.206]
 */
HRESULT WINAPI VarI2FromUI2(USHORT uiIn, short* psOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, psOut );

	/* Check range of value.
	 */
	if( uiIn > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromUI4		[OLEAUT32.207]
 */
HRESULT WINAPI VarI2FromUI4(ULONG ulIn, short* psOut)
{
	TRACE("( %lx, %p ), stub\n", ulIn, psOut );

	/* Check range of value.
	 */
	if( ulIn < I2_MIN || ulIn > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short) ulIn;

	return S_OK;
}

/******************************************************************************
 *		VarI2FromStr		[OLEAUT32.54]
 */
HRESULT WINAPI VarI2FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, short* psOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("( %p, 0x%08lx, 0x%08lx, %p ), stub\n", strIn, lcid, dwFlags, psOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	/* Check range of value.
     */
    dValue = round( dValue );
	if( dValue < I2_MIN || dValue > I2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*psOut = (short)  dValue;

	return S_OK;
}

/**********************************************************************
 *              VarI2FromCy [OLEAUT32.52]
 * Convert currency to signed short
 */
HRESULT WINAPI VarI2FromCy(CY cyIn, short* psOut) {
   double t = round((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (t > I2_MAX || t < I2_MIN) return DISP_E_OVERFLOW;

   *psOut = (SHORT)t;
   return S_OK;
}

/******************************************************************************
 *		VarI4FromUI1		[OLEAUT32.58]
 */
HRESULT WINAPI VarI4FromUI1(BYTE bIn, LONG* plOut)
{
	TRACE("( %X, %p ), stub\n", bIn, plOut );

	*plOut = (LONG) bIn;

	return S_OK;
}


/******************************************************************************
 *		VarI4FromR4		[OLEAUT32.60]
 */
HRESULT WINAPI VarI4FromR4(FLOAT fltIn, LONG* plOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, plOut );

	/* Check range of value.
     */
    fltIn = round( fltIn );
	if( fltIn < I4_MIN || fltIn > I4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*plOut = (LONG) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromR8		[OLEAUT32.61]
 */
HRESULT WINAPI VarI4FromR8(double dblIn, LONG* plOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, plOut );

	/* Check range of value.
     */
    dblIn = round( dblIn );
	if( dblIn < I4_MIN || dblIn > I4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*plOut = (LONG) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromDate		[OLEAUT32.63]
 */
HRESULT WINAPI VarI4FromDate(DATE dateIn, LONG* plOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, plOut );

	/* Check range of value.
     */
    dateIn = round( dateIn );
	if( dateIn < I4_MIN || dateIn > I4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*plOut = (LONG) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromBool		[OLEAUT32.66]
 */
HRESULT WINAPI VarI4FromBool(VARIANT_BOOL boolIn, LONG* plOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, plOut );

	*plOut = (LONG) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromI1		[OLEAUT32.209]
 */
HRESULT WINAPI VarI4FromI1(CHAR cIn, LONG* plOut)
{
	TRACE("( %c, %p ), stub\n", cIn, plOut );

	*plOut = (LONG) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromUI2		[OLEAUT32.210]
 */
HRESULT WINAPI VarI4FromUI2(USHORT uiIn, LONG* plOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, plOut );

	*plOut = (LONG) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromUI4		[OLEAUT32.211]
 */
HRESULT WINAPI VarI4FromUI4(ULONG ulIn, LONG* plOut)
{
	TRACE("( %lx, %p ), stub\n", ulIn, plOut );

	/* Check range of value.
	 */
	if( ulIn < I4_MIN || ulIn > I4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*plOut = (LONG) ulIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromI2		[OLEAUT32.59]
 */
HRESULT WINAPI VarI4FromI2(short sIn, LONG* plOut)
{
	TRACE("( %d, %p ), stub\n", sIn, plOut );

	*plOut = (LONG) sIn;

	return S_OK;
}

/******************************************************************************
 *		VarI4FromStr		[OLEAUT32.64]
 */
HRESULT WINAPI VarI4FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, LONG* plOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("%s: ( strIn: %p -> %s, lcid: 0x%08lx, dwFlags: 0x%08lx, plOut: %p ), stub\n", __FUNCTION__, strIn, debugstr_w(strIn), lcid, dwFlags, plOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	/* Check range of value.
     */
    dValue = round( dValue );
	if( dValue < I4_MIN || dValue > I4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*plOut = (LONG) dValue;

	return S_OK;
}

/**********************************************************************
 *              VarI4FromCy [OLEAUT32.62]
 * Convert currency to signed long
 */
HRESULT WINAPI VarI4FromCy(CY cyIn, LONG* plOut) {
   double t = round((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (t > I4_MAX || t < I4_MIN) return DISP_E_OVERFLOW;

   *plOut = (LONG)t;
   return S_OK;
}

/******************************************************************************
 *		VarR4FromUI1		[OLEAUT32.68]
 */
HRESULT WINAPI VarR4FromUI1(BYTE bIn, FLOAT* pfltOut)
{
	TRACE("( %X, %p ), stub\n", bIn, pfltOut );

	*pfltOut = (FLOAT) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromI2		[OLEAUT32.69]
 */
HRESULT WINAPI VarR4FromI2(short sIn, FLOAT* pfltOut)
{
	TRACE("( %d, %p ), stub\n", sIn, pfltOut );

	*pfltOut = (FLOAT) sIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromI4		[OLEAUT32.70]
 */
HRESULT WINAPI VarR4FromI4(LONG lIn, FLOAT* pfltOut)
{
	TRACE("( %lx, %p ), stub\n", lIn, pfltOut );

	*pfltOut = (FLOAT) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromR8		[OLEAUT32.71]
 */
HRESULT WINAPI VarR4FromR8(double dblIn, FLOAT* pfltOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, pfltOut );

	/* Check range of value.
	 */
	if( dblIn < -(FLT_MAX) || dblIn > FLT_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pfltOut = (FLOAT) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromDate		[OLEAUT32.73]
 */
HRESULT WINAPI VarR4FromDate(DATE dateIn, FLOAT* pfltOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, pfltOut );

	/* Check range of value.
	 */
	if( dateIn < -(FLT_MAX) || dateIn > FLT_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pfltOut = (FLOAT) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromBool		[OLEAUT32.76]
 */
HRESULT WINAPI VarR4FromBool(VARIANT_BOOL boolIn, FLOAT* pfltOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, pfltOut );

	*pfltOut = (FLOAT) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromI1		[OLEAUT32.213]
 */
HRESULT WINAPI VarR4FromI1(CHAR cIn, FLOAT* pfltOut)
{
	TRACE("( %c, %p ), stub\n", cIn, pfltOut );

	*pfltOut = (FLOAT) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromUI2		[OLEAUT32.214]
 */
HRESULT WINAPI VarR4FromUI2(USHORT uiIn, FLOAT* pfltOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pfltOut );

	*pfltOut = (FLOAT) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromUI4		[OLEAUT32.215]
 */
HRESULT WINAPI VarR4FromUI4(ULONG ulIn, FLOAT* pfltOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, pfltOut );

	*pfltOut = (FLOAT) ulIn;

	return S_OK;
}

/******************************************************************************
 *		VarR4FromStr		[OLEAUT32.74]
 */
HRESULT WINAPI VarR4FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, FLOAT* pfltOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("( %p, %ld, %ld, %p ), stub\n", strIn, lcid, dwFlags, pfltOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	/* Check range of value.
	 */
	if( dValue < -(FLT_MAX) || dValue > FLT_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pfltOut = (FLOAT) dValue;

	return S_OK;
}

/**********************************************************************
 *              VarR4FromCy [OLEAUT32.72]
 * Convert currency to float
 */
HRESULT WINAPI VarR4FromCy(CY cyIn, FLOAT* pfltOut) {
   *pfltOut = (FLOAT)((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   return S_OK;
}

/******************************************************************************
 *		VarR8FromUI1		[OLEAUT32.78]
 */
HRESULT WINAPI VarR8FromUI1(BYTE bIn, double* pdblOut)
{
	TRACE("( %d, %p ), stub\n", bIn, pdblOut );

	*pdblOut = (double) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromI2		[OLEAUT32.79]
 */
HRESULT WINAPI VarR8FromI2(short sIn, double* pdblOut)
{
	TRACE("( %d, %p ), stub\n", sIn, pdblOut );

	*pdblOut = (double) sIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromI4		[OLEAUT32.80]
 */
HRESULT WINAPI VarR8FromI4(LONG lIn, double* pdblOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, pdblOut );

	*pdblOut = (double) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromR4		[OLEAUT32.81]
 */
HRESULT WINAPI VarR8FromR4(FLOAT fltIn, double* pdblOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, pdblOut );

	*pdblOut = (double) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromDate		[OLEAUT32.83]
 */
HRESULT WINAPI VarR8FromDate(DATE dateIn, double* pdblOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, pdblOut );

	*pdblOut = (double) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromBool		[OLEAUT32.86]
 */
HRESULT WINAPI VarR8FromBool(VARIANT_BOOL boolIn, double* pdblOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, pdblOut );

	*pdblOut = (double) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromI1		[OLEAUT32.217]
 */
HRESULT WINAPI VarR8FromI1(CHAR cIn, double* pdblOut)
{
	TRACE("( %c, %p ), stub\n", cIn, pdblOut );

	*pdblOut = (double) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromUI2		[OLEAUT32.218]
 */
HRESULT WINAPI VarR8FromUI2(USHORT uiIn, double* pdblOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pdblOut );

	*pdblOut = (double) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarR8FromUI4		[OLEAUT32.219]
 */
HRESULT WINAPI VarR8FromUI4(ULONG ulIn, double* pdblOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, pdblOut );

	*pdblOut = (double) ulIn;

	return S_OK;
}

#if 0
/******************************************************************************
 *		VarR8FromStr		[OLEAUT32.84]
 */
HRESULT WINAPI VarR8FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, double* pdblOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	TRACE("( %s, %ld, %ld, %p ), stub\n", pNewString, lcid, dwFlags, pdblOut );

	/* Check if we have a valid argument
	 */
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	*pdblOut = dValue;

	return S_OK;
}
#endif

/************************************************************************
 * VarR8FromStr (OLEAUT32.84)
 *
 * Convert a VT_BSTR to a VT_R8.
 *
 * PARAMS
 *  strIn   [I] Source
 *  lcid    [I] LCID for the conversion
 *  dwFlags [I] Flags controlling the conversion (VAR_ flags from "oleauto.h")
 *  pDblOut [O] Destination
 *
 * RETURNS
 *  Success: S_OK.
 *  Failure: E_INVALIDARG, if strIn or pDblOut is invalid.
 *           DISP_E_TYPEMISMATCH, if the type cannot be converted
 */
HRESULT WINAPI VarR8FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, double *pDblOut)
{
  return _VarR8FromStr(strIn, lcid, dwFlags, pDblOut);
}

/**********************************************************************
 *              VarR8FromCy [OLEAUT32.82]
 * Convert currency to double
 */
HRESULT WINAPI VarR8FromCy(CY cyIn, double* pdblOut) {
   *pdblOut = (double)((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);
   TRACE("%lu %ld -> %f\n", cyIn.s.Hi, cyIn.s.Lo, *pdblOut);
   return S_OK;
}

/******************************************************************************
 *		VarDateFromUI1		[OLEAUT32.88]
 */
HRESULT WINAPI VarDateFromUI1(BYTE bIn, DATE* pdateOut)
{
	TRACE("( %d, %p ), stub\n", bIn, pdateOut );

	*pdateOut = (DATE) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromI2		[OLEAUT32.89]
 */
HRESULT WINAPI VarDateFromI2(short sIn, DATE* pdateOut)
{
	TRACE("( %d, %p ), stub\n", sIn, pdateOut );

	*pdateOut = (DATE) sIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromI4		[OLEAUT32.90]
 */
HRESULT WINAPI VarDateFromI4(LONG lIn, DATE* pdateOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, pdateOut );

	if( lIn < DATE_MIN || lIn > DATE_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pdateOut = (DATE) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromR4		[OLEAUT32.91]
 */
HRESULT WINAPI VarDateFromR4(FLOAT fltIn, DATE* pdateOut)
{
    TRACE("( %f, %p ), stub\n", fltIn, pdateOut );

    if( ceil(fltIn) < DATE_MIN || floor(fltIn) > DATE_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pdateOut = (DATE) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromR8		[OLEAUT32.92]
 */
HRESULT WINAPI VarDateFromR8(double dblIn, DATE* pdateOut)
{
    TRACE("( %f, %p ), stub\n", dblIn, pdateOut );

	if( ceil(dblIn) < DATE_MIN || floor(dblIn) > DATE_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pdateOut = (DATE) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromStr		[OLEAUT32.94]
 * The string representing the date is composed of two parts, a date and time.
 *
 * The format of the time is has follows:
 * hh[:mm][:ss][AM|PM]
 * Whitespace can be inserted anywhere between these tokens.  A whitespace consists
 * of space and/or tab characters, which are ignored.
 *
 * The formats for the date part are has follows:
 * mm/[dd/][yy]yy
 * [dd/]mm/[yy]yy
 * [yy]yy/mm/dd
 * January dd[,] [yy]yy
 * dd January [yy]yy
 * [yy]yy January dd
 * Whitespace can be inserted anywhere between these tokens.
 *
 * The formats for the date and time string are has follows.
 * date[whitespace][time]
 * [time][whitespace]date
 *
 * These are the only characters allowed in a string representing a date and time:
 * [A-Z] [a-z] [0-9] ':' '-' '/' ',' ' ' '\t'
 */
HRESULT WINAPI VarDateFromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, DATE* pdateOut)
{
    HRESULT ret = S_OK;
    struct tm TM;

    memset( &TM, 0, sizeof(TM) );

    TRACE("( %p, %lx, %lx, %p ), stub\n", strIn, lcid, dwFlags, pdateOut );

    if( DateTimeStringToTm( strIn, dwFlags, &TM ) )
    {
        if( TmToDATE( &TM, pdateOut ) == FALSE )
        {
            ret = E_INVALIDARG;
        }
    }
    else
    {
        ret = DISP_E_TYPEMISMATCH;
    }
    TRACE("Return value %f\n", *pdateOut);
	return ret;
}

/******************************************************************************
 *		VarDateFromI1		[OLEAUT32.221]
 */
HRESULT WINAPI VarDateFromI1(CHAR cIn, DATE* pdateOut)
{
	TRACE("( %c, %p ), stub\n", cIn, pdateOut );

	*pdateOut = (DATE) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromUI2		[OLEAUT32.222]
 */
HRESULT WINAPI VarDateFromUI2(USHORT uiIn, DATE* pdateOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pdateOut );

	if( uiIn > DATE_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pdateOut = (DATE) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromUI4		[OLEAUT32.223]
 */
HRESULT WINAPI VarDateFromUI4(ULONG ulIn, DATE* pdateOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, pdateOut );

	if( ulIn < DATE_MIN || ulIn > DATE_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pdateOut = (DATE) ulIn;

	return S_OK;
}

/******************************************************************************
 *		VarDateFromBool		[OLEAUT32.96]
 */
HRESULT WINAPI VarDateFromBool(VARIANT_BOOL boolIn, DATE* pdateOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, pdateOut );

	*pdateOut = (DATE) boolIn;

	return S_OK;
}

/**********************************************************************
 *              VarDateFromCy [OLEAUT32.93]
 * Convert currency to date
 */
HRESULT WINAPI VarDateFromCy(CY cyIn, DATE* pdateOut) {
   *pdateOut = (DATE)((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (*pdateOut > DATE_MAX || *pdateOut < DATE_MIN) return DISP_E_TYPEMISMATCH;
   return S_OK;
}



/******************************************************************************
 *		VarBstrFromUI1		[OLEAUT32.108]
 */
HRESULT WINAPI VarBstrFromUI1(BYTE bVal, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %d, %ld, %ld, %p ), stub\n", bVal, lcid, dwFlags, pbstrOut );
	sprintf( pBuffer, "%d", bVal );

	*pbstrOut =  StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromI2		[OLEAUT32.109]
 */
HRESULT WINAPI VarBstrFromI2(short iVal, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %d, %ld, %ld, %p ), stub\n", iVal, lcid, dwFlags, pbstrOut );
	sprintf( pBuffer, "%d", iVal );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromI4		[OLEAUT32.110]
 */
HRESULT WINAPI VarBstrFromI4(LONG lIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %ld, %ld, %ld, %p ), stub\n", lIn, lcid, dwFlags, pbstrOut );

	sprintf( pBuffer, "%ld", lIn );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromR4		[OLEAUT32.111]
 */
HRESULT WINAPI VarBstrFromR4(FLOAT fltIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %f, %ld, %ld, %p ), stub\n", fltIn, lcid, dwFlags, pbstrOut );

	sprintf( pBuffer, "%.7g", fltIn );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromR8		[OLEAUT32.112]
 */
HRESULT WINAPI VarBstrFromR8(double dblIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %f, %ld, %ld, %p ), stub\n", dblIn, lcid, dwFlags, pbstrOut );

	sprintf( pBuffer, "%.15g", dblIn );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *    VarBstrFromCy   [OLEAUT32.113]
 */
HRESULT WINAPI VarBstrFromCy(CY cyIn, LCID lcid, ULONG dwFlags, BSTR *pbstrOut) {
    HRESULT rc = S_OK;
    double curVal = 0.0;

	TRACE("([cyIn], %08lx, %08lx, %p), partial stub (no flags handled).\n", lcid, dwFlags, pbstrOut);

    /* Firstly get the currency in a double, then put it in a buffer */
    rc = VarR8FromCy(cyIn, &curVal);
    if (rc == S_OK) {
        sprintf(pBuffer, "%g", curVal);
        *pbstrOut = StringDupAtoBstr( pBuffer );
    }
	return rc;
}


/******************************************************************************
 *		VarBstrFromDate		[OLEAUT32.114]
 *
 * The date is implemented using an 8 byte floating-point number.
 * Days are represented by whole numbers increments starting with 0.00 as
 * being December 30 1899, midnight.
 * The hours are expressed as the fractional part of the number.
 * December 30 1899 at midnight = 0.00
 * January 1 1900 at midnight = 2.00
 * January 4 1900 at 6 AM = 5.25
 * January 4 1900 at noon = 5.50
 * December 29 1899 at midnight = -1.00
 * December 18 1899 at midnight = -12.00
 * December 18 1899 at 6AM = -12.25
 * December 18 1899 at 6PM = -12.75
 * December 19 1899 at midnight = -11.00
 * The tm structure is as follows:
 * struct tm {
 *		  int tm_sec;	   seconds after the minute - [0,59]
 *		  int tm_min;	   minutes after the hour - [0,59]
 *		  int tm_hour;	   hours since midnight - [0,23]
 *		  int tm_mday;	   day of the month - [1,31]
 *		  int tm_mon;	   months since January - [0,11]
 *		  int tm_year;	   years
 *		  int tm_wday;	   days since Sunday - [0,6]
 *		  int tm_yday;	   days since January 1 - [0,365]
 *		  int tm_isdst;    daylight savings time flag
 *		  };
 */
HRESULT WINAPI VarBstrFromDate(DATE dateIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
    struct tm TM;
    memset( &TM, 0, sizeof(TM) );

    TRACE("( %20.20f, %ld, %ld, %p ), stub\n", dateIn, lcid, dwFlags, pbstrOut );

    if( DateToTm( dateIn, dwFlags, &TM ) == FALSE )
			{
        return E_INVALIDARG;
		}

    if( dwFlags & VAR_DATEVALUEONLY )
			strftime( pBuffer, BUFFER_MAX, "%x", &TM );
    else if( dwFlags & VAR_TIMEVALUEONLY )
			strftime( pBuffer, BUFFER_MAX, "%X", &TM );
		else
        strftime( pBuffer, BUFFER_MAX, "%x %X", &TM );

        TRACE("result: %s\n", pBuffer);
		*pbstrOut = StringDupAtoBstr( pBuffer );
	return S_OK;
}

/******************************************************************************
 *		VarBstrFromBool		[OLEAUT32.116]
 */
HRESULT WINAPI VarBstrFromBool(VARIANT_BOOL boolIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %d, %ld, %ld, %p ), stub\n", boolIn, lcid, dwFlags, pbstrOut );

	sprintf( pBuffer, (boolIn == VARIANT_FALSE) ? "False" : "True" );

	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromI1		[OLEAUT32.229]
 */
HRESULT WINAPI VarBstrFromI1(CHAR cIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %c, %ld, %ld, %p ), stub\n", cIn, lcid, dwFlags, pbstrOut );
	sprintf( pBuffer, "%d", cIn );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromUI2		[OLEAUT32.230]
 */
HRESULT WINAPI VarBstrFromUI2(USHORT uiIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %d, %ld, %ld, %p ), stub\n", uiIn, lcid, dwFlags, pbstrOut );
	sprintf( pBuffer, "%d", uiIn );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBstrFromUI4		[OLEAUT32.231]
 */
HRESULT WINAPI VarBstrFromUI4(ULONG ulIn, LCID lcid, ULONG dwFlags, BSTR* pbstrOut)
{
	TRACE("( %ld, %ld, %ld, %p ), stub\n", ulIn, lcid, dwFlags, pbstrOut );
	sprintf( pBuffer, "%ld", ulIn );
	*pbstrOut = StringDupAtoBstr( pBuffer );

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromUI1		[OLEAUT32.118]
 */
HRESULT WINAPI VarBoolFromUI1(BYTE bIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %d, %p ), stub\n", bIn, pboolOut );

	if( bIn == 0 )
	{
		*pboolOut = VARIANT_FALSE;
	}
	else
	{
		*pboolOut = VARIANT_TRUE;
	}

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromI2		[OLEAUT32.119]
 */
HRESULT WINAPI VarBoolFromI2(short sIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %d, %p ), stub\n", sIn, pboolOut );

	*pboolOut = (sIn) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromI4		[OLEAUT32.120]
 */
HRESULT WINAPI VarBoolFromI4(LONG lIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, pboolOut );

	*pboolOut = (lIn) ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromR4		[OLEAUT32.121]
 */
HRESULT WINAPI VarBoolFromR4(FLOAT fltIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, pboolOut );

	*pboolOut = (fltIn == 0.0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromR8		[OLEAUT32.122]
 */
HRESULT WINAPI VarBoolFromR8(double dblIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, pboolOut );

	*pboolOut = (dblIn == 0.0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromDate		[OLEAUT32.123]
 */
HRESULT WINAPI VarBoolFromDate(DATE dateIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, pboolOut );

	*pboolOut = (dateIn == 0.0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromStr		[OLEAUT32.125]
 */
HRESULT WINAPI VarBoolFromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, VARIANT_BOOL* pboolOut)
{
	HRESULT ret = S_OK;
	char* pNewString = NULL;

	TRACE("( %p, %ld, %ld, %p ), stub\n", strIn, lcid, dwFlags, pboolOut );

    pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );

	if( pNewString == NULL || strlen( pNewString ) == 0 )
	{
		ret = DISP_E_TYPEMISMATCH;
	}

	if( ret == S_OK )
	{
		if( strncasecmp( pNewString, "True", strlen( pNewString ) ) == 0 )
		{
			*pboolOut = VARIANT_TRUE;
		}
		else if( strncasecmp( pNewString, "False", strlen( pNewString ) ) == 0 )
		{
			*pboolOut = VARIANT_FALSE;
		}
		else
		{
			/* Try converting the string to a floating point number.
			 */
			double dValue = 0.0;
			HRESULT res = VarR8FromStr( strIn, lcid, dwFlags, &dValue );
			if( res != S_OK )
			{
				ret = DISP_E_TYPEMISMATCH;
			}
			else
				*pboolOut = (dValue == 0.0) ?
						VARIANT_FALSE : VARIANT_TRUE;
		}
	}

	HeapFree( GetProcessHeap(), 0, pNewString );

	return ret;
}

/******************************************************************************
 *		VarBoolFromI1		[OLEAUT32.233]
 */
HRESULT WINAPI VarBoolFromI1(CHAR cIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %c, %p ), stub\n", cIn, pboolOut );

	*pboolOut = (cIn == 0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromUI2		[OLEAUT32.234]
 */
HRESULT WINAPI VarBoolFromUI2(USHORT uiIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pboolOut );

	*pboolOut = (uiIn == 0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

/******************************************************************************
 *		VarBoolFromUI4		[OLEAUT32.235]
 */
HRESULT WINAPI VarBoolFromUI4(ULONG ulIn, VARIANT_BOOL* pboolOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, pboolOut );

	*pboolOut = (ulIn == 0) ? VARIANT_FALSE : VARIANT_TRUE;

	return S_OK;
}

/**********************************************************************
 *              VarBoolFromCy [OLEAUT32.124]
 * Convert currency to boolean
 */
HRESULT WINAPI VarBoolFromCy(CY cyIn, VARIANT_BOOL* pboolOut) {
      if (cyIn.s.Hi || cyIn.s.Lo) *pboolOut = -1;
      else *pboolOut = 0;

      return S_OK;
}

/******************************************************************************
 *		VarI1FromUI1		[OLEAUT32.244]
 */
HRESULT WINAPI VarI1FromUI1(BYTE bIn, CHAR* pcOut)
{
	TRACE("( %d, %p ), stub\n", bIn, pcOut );

	/* Check range of value.
	 */
	if( bIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromI2		[OLEAUT32.245]
 */
HRESULT WINAPI VarI1FromI2(short uiIn, CHAR* pcOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pcOut );

	if( uiIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromI4		[OLEAUT32.246]
 */
HRESULT WINAPI VarI1FromI4(LONG lIn, CHAR* pcOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, pcOut );

	if( lIn < CHAR_MIN || lIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromR4		[OLEAUT32.247]
 */
HRESULT WINAPI VarI1FromR4(FLOAT fltIn, CHAR* pcOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, pcOut );

    fltIn = round( fltIn );
	if( fltIn < CHAR_MIN || fltIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromR8		[OLEAUT32.248]
 */
HRESULT WINAPI VarI1FromR8(double dblIn, CHAR* pcOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, pcOut );

    dblIn = round( dblIn );
    if( dblIn < CHAR_MIN || dblIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromDate		[OLEAUT32.249]
 */
HRESULT WINAPI VarI1FromDate(DATE dateIn, CHAR* pcOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, pcOut );

    dateIn = round( dateIn );
	if( dateIn < CHAR_MIN || dateIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromStr		[OLEAUT32.251]
 */
HRESULT WINAPI VarI1FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, CHAR* pcOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("( %p, %ld, %ld, %p ), stub\n", strIn, lcid, dwFlags, pcOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	/* Check range of value.
     */
    dValue = round( dValue );
	if( dValue < CHAR_MIN || dValue > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) dValue;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromBool		[OLEAUT32.253]
 */
HRESULT WINAPI VarI1FromBool(VARIANT_BOOL boolIn, CHAR* pcOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, pcOut );

	*pcOut = (CHAR) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromUI2		[OLEAUT32.254]
 */
HRESULT WINAPI VarI1FromUI2(USHORT uiIn, CHAR* pcOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pcOut );

	if( uiIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarI1FromUI4		[OLEAUT32.255]
 */
HRESULT WINAPI VarI1FromUI4(ULONG ulIn, CHAR* pcOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, pcOut );

	if( ulIn > CHAR_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pcOut = (CHAR) ulIn;

	return S_OK;
}

/**********************************************************************
 *              VarI1FromCy [OLEAUT32.250]
 * Convert currency to signed char
 */
HRESULT WINAPI VarI1FromCy(CY cyIn, CHAR* pcOut) {
   double t = round((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (t > CHAR_MAX || t < CHAR_MIN) return DISP_E_OVERFLOW;

   *pcOut = (CHAR)t;
   return S_OK;
}

/******************************************************************************
 *		VarUI2FromUI1		[OLEAUT32.257]
 */
HRESULT WINAPI VarUI2FromUI1(BYTE bIn, USHORT* puiOut)
{
	TRACE("( %d, %p ), stub\n", bIn, puiOut );

	*puiOut = (USHORT) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromI2		[OLEAUT32.258]
 */
HRESULT WINAPI VarUI2FromI2(short uiIn, USHORT* puiOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, puiOut );

	if( uiIn < UI2_MIN )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromI4		[OLEAUT32.259]
 */
HRESULT WINAPI VarUI2FromI4(LONG lIn, USHORT* puiOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, puiOut );

	if( lIn < UI2_MIN || lIn > UI2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromR4		[OLEAUT32.260]
 */
HRESULT WINAPI VarUI2FromR4(FLOAT fltIn, USHORT* puiOut)
{
	TRACE("( %f, %p ), stub\n", fltIn, puiOut );

    fltIn = round( fltIn );
	if( fltIn < UI2_MIN || fltIn > UI2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromR8		[OLEAUT32.261]
 */
HRESULT WINAPI VarUI2FromR8(double dblIn, USHORT* puiOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, puiOut );

    dblIn = round( dblIn );
    if( dblIn < UI2_MIN || dblIn > UI2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromDate		[OLEAUT32.262]
 */
HRESULT WINAPI VarUI2FromDate(DATE dateIn, USHORT* puiOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, puiOut );

    dateIn = round( dateIn );
	if( dateIn < UI2_MIN || dateIn > UI2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromStr		[OLEAUT32.264]
 */
HRESULT WINAPI VarUI2FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, USHORT* puiOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("( %p, %ld, %ld, %p ), stub\n", strIn, lcid, dwFlags, puiOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	/* Check range of value.
     */
    dValue = round( dValue );
	if( dValue < UI2_MIN || dValue > UI2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) dValue;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromBool		[OLEAUT32.266]
 */
HRESULT WINAPI VarUI2FromBool(VARIANT_BOOL boolIn, USHORT* puiOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, puiOut );

	*puiOut = (USHORT) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromI1		[OLEAUT32.267]
 */
HRESULT WINAPI VarUI2FromI1(CHAR cIn, USHORT* puiOut)
{
	TRACE("( %c, %p ), stub\n", cIn, puiOut );

	*puiOut = (USHORT) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI2FromUI4		[OLEAUT32.268]
 */
HRESULT WINAPI VarUI2FromUI4(ULONG ulIn, USHORT* puiOut)
{
	TRACE("( %ld, %p ), stub\n", ulIn, puiOut );

	if( ulIn > UI2_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*puiOut = (USHORT) ulIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromStr		[OLEAUT32.277]
 */
HRESULT WINAPI VarUI4FromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, ULONG* pulOut)
{
	double dValue = 0.0;
	LPSTR pNewString = NULL;

	TRACE("( %p, %ld, %ld, %p ), stub\n", strIn, lcid, dwFlags, pulOut );

	/* Check if we have a valid argument
	 */
	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	RemoveCharacterFromString( pNewString, "," );
	if( IsValidRealString( pNewString ) == FALSE )
	{
		return DISP_E_TYPEMISMATCH;
	}

	/* Convert the valid string to a floating point number.
	 */
	dValue = atof( pNewString );

	/* We don't need the string anymore so free it.
	 */
	HeapFree( GetProcessHeap(), 0, pNewString );

	/* Check range of value.
     */
    dValue = round( dValue );
	if( dValue < UI4_MIN || dValue > UI4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pulOut = (ULONG) dValue;

	return S_OK;
}

/**********************************************************************
 *              VarUI2FromCy [OLEAUT32.263]
 * Convert currency to unsigned short
 */
HRESULT WINAPI VarUI2FromCy(CY cyIn, USHORT* pusOut) {
   double t = round((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (t > UI2_MAX || t < UI2_MIN) return DISP_E_OVERFLOW;

   *pusOut = (USHORT)t;

   return S_OK;
}

/******************************************************************************
 *		VarUI4FromUI1		[OLEAUT32.270]
 */
HRESULT WINAPI VarUI4FromUI1(BYTE bIn, ULONG* pulOut)
{
	TRACE("( %d, %p ), stub\n", bIn, pulOut );

	*pulOut = (USHORT) bIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromI2		[OLEAUT32.271]
 */
HRESULT WINAPI VarUI4FromI2(short uiIn, ULONG* pulOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pulOut );

	if( uiIn < UI4_MIN )
	{
		return DISP_E_OVERFLOW;
	}

	*pulOut = (ULONG) uiIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromI4		[OLEAUT32.272]
 */
HRESULT WINAPI VarUI4FromI4(LONG lIn, ULONG* pulOut)
{
	TRACE("( %ld, %p ), stub\n", lIn, pulOut );

	if( lIn < 0 )
	{
		return DISP_E_OVERFLOW;
	}

	*pulOut = (ULONG) lIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromR4		[OLEAUT32.273]
 */
HRESULT WINAPI VarUI4FromR4(FLOAT fltIn, ULONG* pulOut)
{
    fltIn = round( fltIn );
    if( fltIn < UI4_MIN || fltIn > UI4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pulOut = (ULONG) fltIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromR8		[OLEAUT32.274]
 */
HRESULT WINAPI VarUI4FromR8(double dblIn, ULONG* pulOut)
{
	TRACE("( %f, %p ), stub\n", dblIn, pulOut );

	dblIn = round( dblIn );
	if( dblIn < UI4_MIN || dblIn > UI4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pulOut = (ULONG) dblIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromDate		[OLEAUT32.275]
 */
HRESULT WINAPI VarUI4FromDate(DATE dateIn, ULONG* pulOut)
{
	TRACE("( %f, %p ), stub\n", dateIn, pulOut );

	dateIn = round( dateIn );
	if( dateIn < UI4_MIN || dateIn > UI4_MAX )
	{
		return DISP_E_OVERFLOW;
	}

	*pulOut = (ULONG) dateIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromBool		[OLEAUT32.279]
 */
HRESULT WINAPI VarUI4FromBool(VARIANT_BOOL boolIn, ULONG* pulOut)
{
	TRACE("( %d, %p ), stub\n", boolIn, pulOut );

	*pulOut = (ULONG) boolIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromI1		[OLEAUT32.280]
 */
HRESULT WINAPI VarUI4FromI1(CHAR cIn, ULONG* pulOut)
{
	TRACE("( %c, %p ), stub\n", cIn, pulOut );

	*pulOut = (ULONG) cIn;

	return S_OK;
}

/******************************************************************************
 *		VarUI4FromUI2		[OLEAUT32.281]
 */
HRESULT WINAPI VarUI4FromUI2(USHORT uiIn, ULONG* pulOut)
{
	TRACE("( %d, %p ), stub\n", uiIn, pulOut );

	*pulOut = (ULONG) uiIn;

	return S_OK;
}

/**********************************************************************
 *              VarUI4FromCy [OLEAUT32.276]
 * Convert currency to unsigned long
 */
HRESULT WINAPI VarUI4FromCy(CY cyIn, ULONG* pulOut) {
   double t = round((((double)cyIn.s.Hi * 4294967296.0) + (double)cyIn.s.Lo) / 10000);

   if (t > UI4_MAX || t < UI4_MIN) return DISP_E_OVERFLOW;

   *pulOut = (ULONG)t;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromUI1 [OLEAUT32.98]
 * Convert unsigned char to currency
 */
HRESULT WINAPI VarCyFromUI1(BYTE bIn, CY* pcyOut) {
    pcyOut->s.Hi = 0;
    pcyOut->s.Lo = ((ULONG)bIn) * 10000;

    return S_OK;
}

/**********************************************************************
 *              VarCyFromI2 [OLEAUT32.99]
 * Convert signed short to currency
 */
HRESULT WINAPI VarCyFromI2(short sIn, CY* pcyOut) {
    if (sIn < 0) pcyOut->s.Hi = -1;
    else pcyOut->s.Hi = 0;
    pcyOut->s.Lo = ((ULONG)sIn) * 10000;

    return S_OK;
}

/**********************************************************************
 *              VarCyFromI4 [OLEAUT32.100]
 * Convert signed long to currency
 */
HRESULT WINAPI VarCyFromI4(LONG lIn, CY* pcyOut) {
      double t = (double)lIn * (double)10000;
      pcyOut->s.Hi = (LONG)(t / (double)4294967296.0);
      pcyOut->s.Lo = (ULONG)fmod(t, (double)4294967296.0);
      if (lIn < 0) pcyOut->s.Hi--;

      return S_OK;
}

/**********************************************************************
 *              VarCyFromR4 [OLEAUT32.101]
 * Convert float to currency
 */
HRESULT WINAPI VarCyFromR4(FLOAT fltIn, CY* pcyOut) {
   double t = round((double)fltIn * (double)10000);
   pcyOut->s.Hi = (LONG)(t / (double)4294967296.0);
   pcyOut->s.Lo = (ULONG)fmod(t, (double)4294967296.0);
   if (fltIn < 0) pcyOut->s.Hi--;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromR8 [OLEAUT32.102]
 * Convert double to currency
 */
HRESULT WINAPI VarCyFromR8(double dblIn, CY* pcyOut) {
   double t = round(dblIn * (double)10000);
   pcyOut->s.Hi = (LONG)(t / (double)4294967296.0);
   pcyOut->s.Lo = (ULONG)fmod(t, (double)4294967296.0);
   if (dblIn < 0) pcyOut->s.Hi--;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromDate [OLEAUT32.103]
 * Convert date to currency
 */
HRESULT WINAPI VarCyFromDate(DATE dateIn, CY* pcyOut) {
   double t = round((double)dateIn * (double)10000);
   pcyOut->s.Hi = (LONG)(t / (double)4294967296.0);
   pcyOut->s.Lo = (ULONG)fmod(t, (double)4294967296.0);
   if (dateIn < 0) pcyOut->s.Hi--;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromStr [OLEAUT32.104]
 * FIXME: Never tested with decimal seperator other than '.'
 */
HRESULT WINAPI VarCyFromStr(OLECHAR *strIn, LCID lcid, ULONG dwFlags, CY *pcyOut) {

	LPSTR   pNewString      = NULL;
    char   *decSep          = NULL;
    char   *strPtr,*curPtr  = NULL;
    int size, rc;
    double currencyVal = 0.0;


	pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, strIn );
	TRACE("( '%s', 0x%08lx, 0x%08lx, %p )\n", pNewString, lcid, dwFlags, pcyOut );

    /* Get locale information - Decimal Seperator (size includes 0x00) */
    size = GetLocaleInfoA(lcid, LOCALE_SDECIMAL, NULL, 0);
    decSep = (char *) malloc(size);
    rc = GetLocaleInfoA(lcid, LOCALE_SDECIMAL, decSep, size);
    TRACE("Decimal Seperator is '%s'\n", decSep);

    /* Now copy to temporary buffer, skipping any character except 0-9 and
       the decimal seperator */
    curPtr = pBuffer;      /* Current position in string being built       */
    strPtr = pNewString;   /* Current position in supplied currenct string */

    while (*strPtr) {
        /* If decimal seperator, skip it and put '.' in string */
        if (strncmp(strPtr, decSep, (size-1)) == 0) {
            strPtr = strPtr + (size-1);
            *curPtr = '.';
            curPtr++;
        } else if ((*strPtr == '+' || *strPtr == '-') ||
                   (*strPtr >= '0' && *strPtr <= '9')) {
            *curPtr = *strPtr;
            strPtr++;
            curPtr++;
        } else strPtr++;
    }
    *curPtr = 0x00;

    /* Try to get currency into a double */
    currencyVal = atof(pBuffer);
    TRACE("Converted string '%s' to %f\n", pBuffer, currencyVal);

    /* Free allocated storage */
    HeapFree( GetProcessHeap(), 0, pNewString );
    free(decSep);

    /* Convert double -> currency using internal routine */
	return VarCyFromR8(currencyVal, pcyOut);
}


/**********************************************************************
 *              VarCyFromBool [OLEAUT32.106]
 * Convert boolean to currency
 */
HRESULT WINAPI VarCyFromBool(VARIANT_BOOL boolIn, CY* pcyOut) {
   if (boolIn < 0) pcyOut->s.Hi = -1;
   else pcyOut->s.Hi = 0;
   pcyOut->s.Lo = (ULONG)boolIn * (ULONG)10000;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromI1 [OLEAUT32.225]
 * Convert signed char to currency
 */
HRESULT WINAPI VarCyFromI1(signed char cIn, CY* pcyOut) {
   if (cIn < 0) pcyOut->s.Hi = -1;
   else pcyOut->s.Hi = 0;
   pcyOut->s.Lo = (ULONG)cIn * (ULONG)10000;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromUI2 [OLEAUT32.226]
 * Convert unsigned short to currency
 */
HRESULT WINAPI VarCyFromUI2(USHORT usIn, CY* pcyOut) {
   pcyOut->s.Hi = 0;
   pcyOut->s.Lo = (ULONG)usIn * (ULONG)10000;

   return S_OK;
}

/**********************************************************************
 *              VarCyFromUI4 [OLEAUT32.227]
 * Convert unsigned long to currency
 */
HRESULT WINAPI VarCyFromUI4(ULONG ulIn, CY* pcyOut) {
   double t = (double)ulIn * (double)10000;
   pcyOut->s.Hi = (LONG)(t / (double)4294967296.0);
   pcyOut->s.Lo = (ULONG)fmod(t, (double)4294967296.0);

   return S_OK;
}


/**********************************************************************
 *              DosDateTimeToVariantTime [OLEAUT32.14]
 * Convert dos representation of time to the date and time representation
 * stored in a variant.
 */
INT WINAPI DosDateTimeToVariantTime(USHORT wDosDate, USHORT wDosTime,
                                    DATE *pvtime)
{
    struct tm t;

    TRACE("( 0x%x, 0x%x, %p ), stub\n", wDosDate, wDosTime, pvtime );

    t.tm_sec = (wDosTime & 0x001f) * 2;
    t.tm_min = (wDosTime & 0x07e0) >> 5;
    t.tm_hour = (wDosTime & 0xf800) >> 11;

    t.tm_mday = (wDosDate & 0x001f);
    t.tm_mon = (wDosDate & 0x01e0) >> 5;
    t.tm_year = ((wDosDate & 0xfe00) >> 9) + 1980;

    return TmToDATE( &t, pvtime );
}


#define GET_NUMBER_TEXT(fld,name) \
  buff[0] = 0; \
  if (!GetLocaleInfoW(lcid, lctype|fld, buff, 2)) \
    WARN("%s: buffer too small for " #fld "\n", __FUNCTION__); \
  else \
    if (buff[0]) lpChars->name = buff[0]; /* \
  TRACE("lcid 0x%lx, " #name "=%d '%c'\n", lcid, lpChars->name, lpChars->name)*/

/* Get the valid number characters for an lcid */
void VARIANT_GetLocalisedNumberChars(VARIANT_NUMBER_CHARS *lpChars, LCID lcid, DWORD dwFlags)
{
  static const VARIANT_NUMBER_CHARS defaultChars = { '-','+','.',',','$',0,'.',',' };
  LCTYPE lctype = dwFlags & LOCALE_NOUSEROVERRIDE;
  WCHAR buff[4];

  memcpy(lpChars, &defaultChars, sizeof(defaultChars));
  GET_NUMBER_TEXT(LOCALE_SNEGATIVESIGN, cNegativeSymbol);
  GET_NUMBER_TEXT(LOCALE_SPOSITIVESIGN, cPositiveSymbol);
  GET_NUMBER_TEXT(LOCALE_SDECIMAL, cDecimalPoint);
  GET_NUMBER_TEXT(LOCALE_STHOUSAND, cDigitSeperator);
  GET_NUMBER_TEXT(LOCALE_SMONDECIMALSEP, cCurrencyDecimalPoint);
  GET_NUMBER_TEXT(LOCALE_SMONTHOUSANDSEP, cCurrencyDigitSeperator);

  /* Local currency symbols are often 2 characters */
  lpChars->cCurrencyLocal2 = '\0';
  switch(GetLocaleInfoW(lcid, lctype|LOCALE_SCURRENCY, buff, sizeof(buff)/sizeof(WCHAR)))
  {
    case 3: lpChars->cCurrencyLocal2 = buff[1]; /* Fall through */
    case 2: lpChars->cCurrencyLocal  = buff[0];
            break;
    default: WARN("buffer too small for LOCALE_SCURRENCY\n");
  }
  TRACE("lcid 0x%lx, cCurrencyLocal =%d,%d '%c','%c'\n", lcid, lpChars->cCurrencyLocal,
        lpChars->cCurrencyLocal2, lpChars->cCurrencyLocal, lpChars->cCurrencyLocal2);
}

/* Number Parsing States */
#define B_PROCESSING_EXPONENT 0x1
#define B_NEGATIVE_EXPONENT   0x2
#define B_EXPONENT_START      0x4
#define B_INEXACT_ZEROS       0x8
#define B_LEADING_ZERO        0x10
#define B_PROCESSING_HEX      0x20
#define B_PROCESSING_OCT      0x40
/**********************************************************************
 *              VarParseNumFromStr [OLEAUT32.46]
 *
 * Parse a string containing a number into a NUMPARSE structure.
 *
 * PARAMS
 *  lpszStr [I]   String to parse number from
 *  lcid    [I]   Locale Id for the conversion
 *  dwFlags [I]   0, or LOCALE_NOUSEROVERRIDE to use system default number chars
 *  pNumprs [I/O] Destination for parsed number
 *  rgbDig  [O]   Destination for digits read in
 *
 * RETURNS
 *  Success: S_OK. pNumprs and rgbDig contain the parsed representation of
 *           the number.
 *  Failure: E_INVALIDARG, if any parameter is invalid.
 *           DISP_E_TYPEMISMATCH, if the string is not a number or is formatted
 *           incorrectly.
 *           DISP_E_OVERFLOW, if rgbDig is too small to hold the number.
 *
 * NOTES
 *  pNumprs must have the following fields set:
 *   cDig: Set to the size of rgbDig.
 *   dwInFlags: Set to the allowable syntax of the number using NUMPRS_ flags
 *            from "oleauto.h".
 *
 * FIXME
 *  - I am unsure if this function should parse non-arabic (e.g. Thai)
 *   numerals, so this has not been implemented.
 */
HRESULT WINAPI VarParseNumFromStr(OLECHAR *lpszStr, LCID lcid, ULONG dwFlags,
                                  NUMPARSE *pNumprs, BYTE *rgbDig)
{
  VARIANT_NUMBER_CHARS chars;
  BYTE rgbTmp[1024];
  DWORD dwState = B_EXPONENT_START|B_INEXACT_ZEROS;
  int iMaxDigits = sizeof(rgbTmp) / sizeof(BYTE);
  int cchUsed = 0;

  TRACE("%s: (%s, %ld, dwFlags: 0x%08lx,%p,%p, NUMPARSE->dwInFlags: %x)\n", __FUNCTION__, debugstr_w(lpszStr), lcid, dwFlags, pNumprs, rgbDig, pNumprs->dwInFlags);

  if (!pNumprs || !rgbDig)
    return E_INVALIDARG;

  if (pNumprs->cDig < iMaxDigits)
    iMaxDigits = pNumprs->cDig;

  pNumprs->cDig = 0;
  pNumprs->dwOutFlags = 0;
  pNumprs->cchUsed = 0;
  pNumprs->nBaseShift = 0;
  pNumprs->nPwr10 = 0;

  if (!lpszStr)
    return DISP_E_TYPEMISMATCH;

  VARIANT_GetLocalisedNumberChars(&chars, lcid, dwFlags);

  /* First consume all the leading symbols and space from the string */
  while (1)
  {
    if (pNumprs->dwInFlags & NUMPRS_LEADING_WHITE && isspaceW(*lpszStr))
    {
      pNumprs->dwOutFlags |= NUMPRS_LEADING_WHITE;
      do
      {
        cchUsed++;
        lpszStr++;
      } while (isspaceW(*lpszStr));
    }
    else if (pNumprs->dwInFlags & NUMPRS_LEADING_PLUS &&
             *lpszStr == chars.cPositiveSymbol &&
             !(pNumprs->dwOutFlags & NUMPRS_LEADING_PLUS))
    {
      pNumprs->dwOutFlags |= NUMPRS_LEADING_PLUS;
      cchUsed++;
      lpszStr++;
    }
    else if (pNumprs->dwInFlags & NUMPRS_LEADING_MINUS &&
             *lpszStr == chars.cNegativeSymbol &&
             !(pNumprs->dwOutFlags & NUMPRS_LEADING_MINUS))
    {
      pNumprs->dwOutFlags |= (NUMPRS_LEADING_MINUS|NUMPRS_NEG);
      cchUsed++;
      lpszStr++;
    }
    else if (pNumprs->dwInFlags & NUMPRS_CURRENCY &&
             !(pNumprs->dwOutFlags & NUMPRS_CURRENCY) &&
             *lpszStr == chars.cCurrencyLocal &&
             (!chars.cCurrencyLocal2 || lpszStr[1] == chars.cCurrencyLocal2))
    {
      pNumprs->dwOutFlags |= NUMPRS_CURRENCY;
      cchUsed++;
      lpszStr++;
      /* Only accept currency characters */
      chars.cDecimalPoint = chars.cCurrencyDecimalPoint;
      chars.cDigitSeperator = chars.cCurrencyDigitSeperator;
    }
    else if (pNumprs->dwInFlags & NUMPRS_PARENS && *lpszStr == '(' &&
             !(pNumprs->dwOutFlags & NUMPRS_PARENS))
    {
      pNumprs->dwOutFlags |= NUMPRS_PARENS;
      cchUsed++;
      lpszStr++;
    }
    else
      break;
  } /* End of while */

  if(pNumprs->dwOutFlags & (1<<VT_LPWSTR))
    {
 TRACE("  I'm here");     
    }
  if (!(pNumprs->dwOutFlags & NUMPRS_CURRENCY))
  {
    /* Only accept non-currency characters */
    chars.cCurrencyDecimalPoint = chars.cDecimalPoint;
    chars.cCurrencyDigitSeperator = chars.cDigitSeperator;
  }

  if ((*lpszStr == '&' && (*(lpszStr+1) == 'H' || *(lpszStr+1) == 'h')) &&
    pNumprs->dwInFlags & NUMPRS_HEX_OCT)
  {
      dwState |= B_PROCESSING_HEX;
      pNumprs->dwOutFlags |= NUMPRS_HEX_OCT;
      cchUsed=cchUsed+2;
      lpszStr=lpszStr+2;
  }
  else if ((*lpszStr == '&' && (*(lpszStr+1) == 'O' || *(lpszStr+1) == 'o')) &&
    pNumprs->dwInFlags & NUMPRS_HEX_OCT)
  {
      dwState |= B_PROCESSING_OCT;
      pNumprs->dwOutFlags |= NUMPRS_HEX_OCT;
      cchUsed=cchUsed+2;
      lpszStr=lpszStr+2;
  }

  /* Strip Leading zeros */
  while (*lpszStr == '0')
  {
    dwState |= B_LEADING_ZERO;
    cchUsed++;
    lpszStr++;
  }

  while (*lpszStr)
  {
    if (isdigitW(*lpszStr))
    {
      if (dwState & B_PROCESSING_EXPONENT)
      {
        int exponentSize = 0;
        if (dwState & B_EXPONENT_START)
        {
          while (*lpszStr == '0')
          {
            /* Skip leading zero's in the exponent */
            cchUsed++;
            lpszStr++;
          }
          if (!isdigitW(*lpszStr))
            break; /* No exponent digits - invalid */
        }

        while (isdigitW(*lpszStr))
        {
          exponentSize *= 10;
          exponentSize += *lpszStr - '0';
          cchUsed++;
          lpszStr++;
        }
        if (dwState & B_NEGATIVE_EXPONENT)
          exponentSize = -exponentSize;
        /* Add the exponent into the powers of 10 */
        pNumprs->nPwr10 += exponentSize;
        dwState &= ~(B_PROCESSING_EXPONENT|B_EXPONENT_START);
        lpszStr--; /* back up to allow processing of next char */
      }
      else
      {
        if ((pNumprs->cDig >= iMaxDigits) && !(dwState & B_PROCESSING_HEX)
          && !(dwState & B_PROCESSING_OCT))
        {
          pNumprs->dwOutFlags |= NUMPRS_INEXACT;

          if (*lpszStr != '0')
            dwState &= ~B_INEXACT_ZEROS; /* Inexact number with non-trailing zeros */

          /* This digit can't be represented, but count it in nPwr10 */
          if (pNumprs->dwOutFlags & NUMPRS_DECIMAL)
            pNumprs->nPwr10--;
          else
            pNumprs->nPwr10++;
        }
        else
        {
          if ((dwState & B_PROCESSING_OCT) && ((*lpszStr == '8') || (*lpszStr == '9'))) {
            return DISP_E_TYPEMISMATCH;
          }

          if (pNumprs->dwOutFlags & NUMPRS_DECIMAL)
            pNumprs->nPwr10--; /* Count decimal points in nPwr10 */

          rgbTmp[pNumprs->cDig] = *lpszStr - '0';
        }
        pNumprs->cDig++;
        cchUsed++;
      }
    }
    else if (*lpszStr == chars.cDigitSeperator && pNumprs->dwInFlags & NUMPRS_THOUSANDS)
    {
      pNumprs->dwOutFlags |= NUMPRS_THOUSANDS;
      cchUsed++;
    }
    else if (*lpszStr == chars.cDecimalPoint &&
             pNumprs->dwInFlags & NUMPRS_DECIMAL &&
             !(pNumprs->dwOutFlags & (NUMPRS_DECIMAL|NUMPRS_EXPONENT)))
    {
      pNumprs->dwOutFlags |= NUMPRS_DECIMAL;
      cchUsed++;

      /* Remove trailing zeros from the whole number part */
      while (pNumprs->cDig > 1 && !rgbTmp[pNumprs->cDig - 1])
      {
        pNumprs->nPwr10++;
        pNumprs->cDig--;
      }

      /* If we have no digits so far, skip leading zeros */
      if (!pNumprs->cDig)
      {
        while (lpszStr[1] == '0')
        {
          dwState |= B_LEADING_ZERO;
          cchUsed++;
          lpszStr++;
        }
      }
    }
    else if ((*lpszStr == 'e' || *lpszStr == 'E') &&
             pNumprs->dwInFlags & NUMPRS_EXPONENT &&
             !(pNumprs->dwOutFlags & NUMPRS_EXPONENT))
    {
      dwState |= B_PROCESSING_EXPONENT;
      pNumprs->dwOutFlags |= NUMPRS_EXPONENT;
      cchUsed++;
    }
    else if (dwState & B_PROCESSING_EXPONENT && *lpszStr == chars.cPositiveSymbol)
    {
      cchUsed++; /* Ignore positive exponent */
    }
    else if (dwState & B_PROCESSING_EXPONENT && *lpszStr == chars.cNegativeSymbol)
    {
      dwState |= B_NEGATIVE_EXPONENT;
      cchUsed++;
    }
    else if (((*lpszStr >= 'a' && *lpszStr <= 'f') ||
             (*lpszStr >= 'A' && *lpszStr <= 'F')) &&
             dwState & B_PROCESSING_HEX)
    {
      if (pNumprs->cDig >= iMaxDigits)
      {
        return DISP_E_OVERFLOW;
      }
      else
      {
        if (*lpszStr >= 'a')
          rgbTmp[pNumprs->cDig] = *lpszStr - 'a' + 10;
        else
          rgbTmp[pNumprs->cDig] = *lpszStr - 'A' + 10;
      }
      pNumprs->cDig++;
      cchUsed++;
    }
    else
      break; /* Stop at an unrecognised character */

    lpszStr++;
  }

  if (!pNumprs->cDig && dwState & B_LEADING_ZERO)
  {
    /* Ensure a 0 on its own gets stored */
    pNumprs->cDig = 1;
    rgbTmp[0] = 0;
  }

  if (pNumprs->dwOutFlags & NUMPRS_EXPONENT && dwState & B_PROCESSING_EXPONENT)
  {
    pNumprs->cchUsed = cchUsed;
    return DISP_E_TYPEMISMATCH; /* Failed to completely parse the exponent */
  }

  if (pNumprs->dwOutFlags & NUMPRS_INEXACT)
  {
    if (dwState & B_INEXACT_ZEROS)
      pNumprs->dwOutFlags &= ~NUMPRS_INEXACT; /* All zeros doesn't set NUMPRS_INEXACT */
  } else if(pNumprs->dwInFlags & NUMPRS_HEX_OCT)
  {
    /* copy all of the digits into the output digit buffer */
    /* this is exactly what windows does although it also returns */
    /* cDig of X and writes X+Y where Y>=0 number of digits to rgbDig */
    memcpy(rgbDig, rgbTmp, pNumprs->cDig * sizeof(BYTE));

    if (dwState & B_PROCESSING_HEX) {
      /* hex numbers have always the same format */
      pNumprs->nPwr10=0;
      pNumprs->nBaseShift=4;
    } else {
      if (dwState & B_PROCESSING_OCT) {
        /* oct numbers have always the same format */
        pNumprs->nPwr10=0;
        pNumprs->nBaseShift=3;
      } else {
        while (pNumprs->cDig > 1 && !rgbTmp[pNumprs->cDig - 1])
        {
          if (pNumprs->dwOutFlags & NUMPRS_DECIMAL)
            pNumprs->nPwr10--;
          else
            pNumprs->nPwr10++;

          pNumprs->cDig--;
        }
      }
    }
  } else
  {
    /* Remove trailing zeros from the last (whole number or decimal) part */
    while (pNumprs->cDig > 1 && !rgbTmp[pNumprs->cDig - 1])
    {
      if (pNumprs->dwOutFlags & NUMPRS_DECIMAL)
        pNumprs->nPwr10--;
      else
        pNumprs->nPwr10++;

      pNumprs->cDig--;
    }
  }

  if (pNumprs->cDig <= iMaxDigits)
    pNumprs->dwOutFlags &= ~NUMPRS_INEXACT; /* Ignore stripped zeros for NUMPRS_INEXACT */
  else
    pNumprs->cDig = iMaxDigits; /* Only return iMaxDigits worth of digits */

  /* Copy the digits we processed into rgbDig */
  memcpy(rgbDig, rgbTmp, pNumprs->cDig * sizeof(BYTE));

  /* Consume any trailing symbols and space */
  while (1)
  {
    if ((pNumprs->dwInFlags & NUMPRS_TRAILING_WHITE) && isspaceW(*lpszStr))
    {
      pNumprs->dwOutFlags |= NUMPRS_TRAILING_WHITE;
      do
      {
        cchUsed++;
        lpszStr++;
      } while (isspaceW(*lpszStr));
    }
    else if (pNumprs->dwInFlags & NUMPRS_TRAILING_PLUS &&
             !(pNumprs->dwOutFlags & NUMPRS_LEADING_PLUS) &&
             *lpszStr == chars.cPositiveSymbol)
    {
      pNumprs->dwOutFlags |= NUMPRS_TRAILING_PLUS;
      cchUsed++;
      lpszStr++;
    }
    else if (pNumprs->dwInFlags & NUMPRS_TRAILING_MINUS &&
             !(pNumprs->dwOutFlags & NUMPRS_LEADING_MINUS) &&
             *lpszStr == chars.cNegativeSymbol)
    {
      pNumprs->dwOutFlags |= (NUMPRS_TRAILING_MINUS|NUMPRS_NEG);
      cchUsed++;
      lpszStr++;
    }
    else if (pNumprs->dwInFlags & NUMPRS_PARENS && *lpszStr == ')' &&
             pNumprs->dwOutFlags & NUMPRS_PARENS)
    {
      cchUsed++;
      lpszStr++;
      pNumprs->dwOutFlags |= NUMPRS_NEG;
    }
    else
      break;
  }

  if (pNumprs->dwOutFlags & NUMPRS_PARENS && !(pNumprs->dwOutFlags & NUMPRS_NEG))
  {
    pNumprs->cchUsed = cchUsed;
    return DISP_E_TYPEMISMATCH; /* Opening parenthesis not matched */
  }

  if (pNumprs->dwInFlags & NUMPRS_USE_ALL && *lpszStr != '\0')
    return DISP_E_TYPEMISMATCH; /* Not all chars were consumed */

  if (!pNumprs->cDig)
    return DISP_E_TYPEMISMATCH; /* No Number found */

  pNumprs->cchUsed = cchUsed;
  TRACE("%s: Returning, NUMPARS->cchUsed: %x, NUMPARS->cDig: %x, pNumprs->dwOutFlags: %x, NUMPARS->nBaseShift: %x,, NUMPARS->nPwr10: %x\n",
        __FUNCTION__, cchUsed, pNumprs->cDig, pNumprs->dwOutFlags, pNumprs->nBaseShift, pNumprs->nPwr10);
  return S_OK;
}

#if 0
/**********************************************************************
 *              VarParseNumFromStr [OLEAUT32.46]
 */
HRESULT WINAPI VarParseNumFromStr(OLECHAR * strIn, LCID lcid, ULONG dwFlags,
                                  NUMPARSE * pnumprs, BYTE * rgbDig)
{
    int i,lastent=0;
    int cDig;
    BOOL foundNum=FALSE;

    FIXME("(%s,flags=%lx,....), partial stub!\n",debugstr_w(strIn),dwFlags);
    FIXME("numparse: cDig=%d, InFlags=%lx\n",pnumprs->cDig,pnumprs->dwInFlags);

    /* The other struct components are to be set by us */
    memset(rgbDig,0,pnumprs->cDig);

    /* FIXME: Just patching some values in */
    pnumprs->nPwr10	= 0;
    pnumprs->nBaseShift	= 0;
    pnumprs->cchUsed	= lastent;
    pnumprs->dwOutFlags	= NUMPRS_DECIMAL;

    cDig = 0;
    for (i=0; strIn[i] ;i++) {
	if ((strIn[i]>='0') && (strIn[i]<='9')) {
            foundNum = TRUE;
	    if (pnumprs->cDig > cDig) {
		*(rgbDig++)=strIn[i]-'0';
		cDig++;
		lastent = i;
	    }
	} else if ((strIn[i]=='-') && (foundNum==FALSE)) {
            pnumprs->dwOutFlags	|= NUMPRS_NEG;
        }
    }
    pnumprs->cDig	= cDig;
    TRACE("numparse out: cDig=%d, OutFlags=%lx\n",pnumprs->cDig,pnumprs->dwOutFlags);
    return S_OK;
}
#endif


/**********************************************************************
 *              VarNumFromParseNum [OLEAUT32.47]
 *
 * Convert a NUMPARSE structure into a numeric Variant type.
 *
 * PARAMS
 *  pNumprs  [I] Source for parsed number. cDig must be set to the size of rgbDig
 *  rgbDig   [I] Source for the numbers digits
 *  dwVtBits [I] VTBIT_ flags from "oleauto.h" indicating the acceptable dest types
 *  pVarDst  [O] Destination for the converted Variant value.
 *
 * RETURNS
 *  Success: S_OK. pVarDst contains the converted value.
 *  Failure: E_INVALIDARG, if any parameter is invalid.
 *           DISP_E_OVERFLOW, if the number is too big for the types set in dwVtBits.
 *
 * NOTES
 *  - The smallest favoured type present in dwVtBits that can represent the
 *    number in pNumprs without losing precision is used.
 *  - Signed types are preferrred over unsigned types of the same size.
 *  - Preferred types in order are: integer, float, double, currency then decimal.
 *  - Rounding (dropping of decimal points) occurs without error. See VarI8FromR8()
 *    for details of the rounding method.
 *  - pVarDst is not cleared before the result is stored in it.
 */
HRESULT WINAPI VarNumFromParseNum(NUMPARSE * pnumprs, BYTE * rgbDig,
                                  ULONG dwVtBits, VARIANT * pvar)
{
#ifdef __WIN32OS2__
    DWORD xint;
    int i;

    TRACE("%s: NUMPARSE->cDig: %x, NUMPARSE->nBaseShift: %x, NUMPARSE->dwOutFlags: %x dwVtBits=%lx. rgbDig: \n",
          __FUNCTION__, pnumprs->cDig, pnumprs->nBaseShift, pnumprs->dwOutFlags, dwVtBits);

    xint = 0;
    for (i=0;i<pnumprs->cDig;i++)
      xint = xint*10 + rgbDig[i];

    if (pnumprs->dwOutFlags & NUMPRS_NEG) {
        xint = xint * -1;
    }
    TRACE("%s: xint: %x (%ld)\n", __FUNCTION__, xint, xint);

    VariantInit(pvar);
    if (dwVtBits & VTBIT_I4) {
	V_VT(pvar) = VT_I4;
	V_UNION(pvar,intVal) = xint;
	return S_OK;
    }
    if (dwVtBits & VTBIT_R8) {
	V_VT(pvar) = VT_R8;
	V_UNION(pvar,dblVal) = xint;
	return S_OK;
    }
    if (dwVtBits & VTBIT_R4) {
	V_VT(pvar) = VT_R4;
	V_UNION(pvar,fltVal) = xint;
	return S_OK;
    }
    if (dwVtBits & VTBIT_I2) {
        V_VT(pvar) = VT_I2;
        V_UNION(pvar,iVal) = xint;
        return S_OK;
    }
    /* FIXME: Currency should be from a double */
    if (dwVtBits & VTBIT_CY) {
        V_VT(pvar) = VT_CY;
        TRACE("Calculated currency is xint=%ld\n", xint);
        VarCyFromInt( (int) xint, &V_UNION(pvar,cyVal) );
        TRACE("Calculated cy is %ld,%lu\n", V_UNION(pvar,cyVal).s.Hi, V_UNION(pvar,cyVal).s.Lo);
        return VarCyFromInt( (int) xint, &V_UNION(pvar,cyVal) );
    }

    if (dwVtBits & VTBIT_DECIMAL)
      {
        int i;
        ULONG carry;
        /*    ULONG64 tmp; */
        ULONG tmp;
        DECIMAL* pDec = &V_DECIMAL(pvar);
        
        V_VT(pvar) = VT_DECIMAL;
        
        DECIMAL_SETZERO(pDec);
        DEC_LO32(pDec) = 0;
        
        if (pnumprs->dwOutFlags & NUMPRS_NEG)
          DEC_SIGN(pDec) = DECIMAL_NEG;
        else
          DEC_SIGN(pDec) = DECIMAL_POS;

        /* Factor the significant digits */
        for (i = 0; i < pnumprs->cDig; i++)
          {
            if(0xffffffff /10 - 9 < DEC_LO32(pDec) ) {
              /* Overflow expected. This code only support 32Bit numbers for now */
              FIXME("%s: Converted number will be  > 32bit! Only partial implementation. Returning DISP_E_OVERFLOW.", __FUNCTION__);
              DEC_LO32(pDec) =  UI4_MAX;
              return DISP_E_OVERFLOW;
            }

            tmp = DEC_LO32(pDec) * 10 + rgbDig[i];

            /* No support for numbers > 32 bit for now. */           
            DEC_LO32(pDec) = (ULONG)(tmp & UI4_MAX);
            DEC_MID32(pDec) = (ULONG)0;
            DEC_HI32(pDec) = (ULONG)0;

            /* The following is not working because of missing ULONG64 !! */
#if 0
            carry = (ULONG)(tmp >> 32);
            DEC_LO64(pDec)=tmp;
            DEC_LO32(pDec) = (ULONG)(tmp & UI4_MAX);
            tmp = (ULONG)DEC_MID32(pDec) * 10 + carry;
            carry = (ULONG)(tmp >> 32);
            DEC_MID32(pDec) = (ULONG)(tmp & UI4_MAX);
            tmp = (ULONG)DEC_HI32(pDec) * 10 + carry;
            DEC_HI32(pDec) = (ULONG)(tmp & UI4_MAX);
#endif

            if (tmp >> 32 & UI4_MAX)
              {
              VarNumFromParseNum_DecOverflow:
                TRACE("Overflow\n");
                DEC_LO32(pDec) = DEC_MID32(pDec) = DEC_HI32(pDec) = UI4_MAX;
                return DISP_E_OVERFLOW;
              }
          }
        
        TRACE("Done... LO32: %x, MID32: %x, HI32: %x, DEC_LO64: %x, SignScale: %x, sign: %x\n",
              DEC_LO32(pDec), DEC_MID32(pDec), DEC_HI32(pDec), DEC_LO64(pDec), DEC_SIGNSCALE(pDec), DEC_SIGN(pDec) );
        
        return S_OK;
      }
    
    FIXME("%s: (..,dwVtBits=%lx,....), partial stub!\n", __FUNCTION__, dwVtBits);
	FIXME("vtbitmask is unsupported %lx, int=%d\n",dwVtBits, (int) xint);
	return E_FAIL;
} /* End of __WIN32OS2__ */

#else

  /* Scale factors and limits for double arithmetic */
  static const double dblMultipliers[11] = {
    1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0,
    1000000.0, 10000000.0, 100000000.0, 1000000000.0, 10000000000.0
  };
  static const double dblMinimums[11] = {
    R8_MIN, R8_MIN*10.0, R8_MIN*100.0, R8_MIN*1000.0, R8_MIN*10000.0,
    R8_MIN*100000.0, R8_MIN*1000000.0, R8_MIN*10000000.0,
    R8_MIN*100000000.0, R8_MIN*1000000000.0, R8_MIN*10000000000.0
  };
  static const double dblMaximums[11] = {
    R8_MAX, R8_MAX/10.0, R8_MAX/100.0, R8_MAX/1000.0, R8_MAX/10000.0,
    R8_MAX/100000.0, R8_MAX/1000000.0, R8_MAX/10000000.0,
    R8_MAX/100000000.0, R8_MAX/1000000000.0, R8_MAX/10000000000.0
  };

  int wholeNumberDigits, fractionalDigits, divisor10 = 0, multiplier10 = 0;

  TRACE("(%p,%p,0x%lx,%p)\n", pNumprs, rgbDig, dwVtBits, pVarDst);

  if (pNumprs->nBaseShift)
  {
    /* nBaseShift indicates a hex or octal number */
    ULONG64 ul64 = 0;
    LONG64 l64;
    int i;

    /* Convert the hex or octal number string into a UI64 */
    for (i = 0; i < pNumprs->cDig; i++)
    {
      if (ul64 > ((UI8_MAX>>pNumprs->nBaseShift) - rgbDig[i]))
      {
        TRACE("Overflow multiplying digits\n");
        return DISP_E_OVERFLOW;
      }
      ul64 = (ul64<<pNumprs->nBaseShift) + rgbDig[i];
    }

    /* also make a negative representation */
    l64=-ul64;

    /* Try signed and unsigned types in size order */
    if (dwVtBits & VTBIT_I1 && ((ul64 <= I1_MAX)||(l64 >= I1_MIN)))
    {
      V_VT(pVarDst) = VT_I1;
      if (ul64 <= I1_MAX)
          V_I1(pVarDst) = ul64;
      else
          V_I1(pVarDst) = l64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_UI1 && ul64 <= UI1_MAX)
    {
      V_VT(pVarDst) = VT_UI1;
      V_UI1(pVarDst) = ul64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_I2 && ((ul64 <= I2_MAX)||(l64 >= I2_MIN)))
    {
      V_VT(pVarDst) = VT_I2;
      if (ul64 <= I2_MAX)
          V_I2(pVarDst) = ul64;
      else
          V_I2(pVarDst) = l64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_UI2 && ul64 <= UI2_MAX)
    {
      V_VT(pVarDst) = VT_UI2;
      V_UI2(pVarDst) = ul64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_I4 && ((ul64 <= I4_MAX)||(l64 >= I4_MIN)))
    {
      V_VT(pVarDst) = VT_I4;
      if (ul64 <= I4_MAX)
          V_I4(pVarDst) = ul64;
      else
          V_I4(pVarDst) = l64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_UI4 && ul64 <= UI4_MAX)
    {
      V_VT(pVarDst) = VT_UI4;
      V_UI4(pVarDst) = ul64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_I8 && ((ul64 <= I4_MAX)||(l64>=I4_MIN)))
    {
      V_VT(pVarDst) = VT_I8;
      V_I8(pVarDst) = ul64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_UI8)
    {
      V_VT(pVarDst) = VT_UI8;
      V_UI8(pVarDst) = ul64;
      return S_OK;
    }
    else if ((dwVtBits & REAL_VTBITS) == VTBIT_DECIMAL)
    {
      V_VT(pVarDst) = VT_DECIMAL;
      DEC_SIGNSCALE(&V_DECIMAL(pVarDst)) = SIGNSCALE(DECIMAL_POS,0);
      DEC_HI32(&V_DECIMAL(pVarDst)) = 0;
      DEC_LO64(&V_DECIMAL(pVarDst)) = ul64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_R4 && ((ul64 <= I4_MAX)||(l64 >= I4_MIN)))
    {
      V_VT(pVarDst) = VT_R4;
      if (ul64 <= I4_MAX)
          V_R4(pVarDst) = ul64;
      else
          V_R4(pVarDst) = l64;
      return S_OK;
    }
    else if (dwVtBits & VTBIT_R8 && ((ul64 <= I4_MAX)||(l64 >= I4_MIN)))
    {
      V_VT(pVarDst) = VT_R8;
      if (ul64 <= I4_MAX)
          V_R8(pVarDst) = ul64;
      else
          V_R8(pVarDst) = l64;
      return S_OK;
    }
    TRACE("Overflow: possible return types: 0x%lx, value: %s\n", dwVtBits, wine_dbgstr_longlong(ul64));
    return DISP_E_OVERFLOW;
  }

  /* Count the number of relevant fractional and whole digits stored,
   * And compute the divisor/multiplier to scale the number by.
   */
  if (pNumprs->nPwr10 < 0)
  {
    if (-pNumprs->nPwr10 >= pNumprs->cDig)
    {
      /* A real number < +/- 1.0 e.g. 0.1024 or 0.01024 */
      wholeNumberDigits = 0;
      fractionalDigits = pNumprs->cDig;
      divisor10 = -pNumprs->nPwr10;
    }
    else
    {
      /* An exactly represented real number e.g. 1.024 */
      wholeNumberDigits = pNumprs->cDig + pNumprs->nPwr10;
      fractionalDigits = pNumprs->cDig - wholeNumberDigits;
      divisor10 = pNumprs->cDig - wholeNumberDigits;
    }
  }
  else if (pNumprs->nPwr10 == 0)
  {
    /* An exactly represented whole number e.g. 1024 */
    wholeNumberDigits = pNumprs->cDig;
    fractionalDigits = 0;
  }
  else /* pNumprs->nPwr10 > 0 */
  {
    /* A whole number followed by nPwr10 0's e.g. 102400 */
    wholeNumberDigits = pNumprs->cDig;
    fractionalDigits = 0;
    multiplier10 = pNumprs->nPwr10;
  }

  TRACE("cDig %d; nPwr10 %d, whole %d, frac %d ", pNumprs->cDig,
        pNumprs->nPwr10, wholeNumberDigits, fractionalDigits);
  TRACE("mult %d; div %d\n", multiplier10, divisor10);

  if (dwVtBits & (INTEGER_VTBITS|VTBIT_DECIMAL) &&
      (!fractionalDigits || !(dwVtBits & (REAL_VTBITS|VTBIT_CY|VTBIT_DECIMAL))))
  {
    /* We have one or more integer output choices, and either:
     *  1) An integer input value, or
     *  2) A real number input value but no floating output choices.
     * Alternately, we have a DECIMAL output available and an integer input.
     *
     * So, place the integer value into pVarDst, using the smallest type
     * possible and preferring signed over unsigned types.
     */
    BOOL bOverflow = FALSE, bNegative;
    ULONG64 ul64 = 0;
    int i;

    /* Convert the integer part of the number into a UI8 */
    for (i = 0; i < wholeNumberDigits; i++)
    {
      if (ul64 > (UI8_MAX / 10 - rgbDig[i]))
      {
        TRACE("Overflow multiplying digits\n");
        bOverflow = TRUE;
        break;
      }
      ul64 = ul64 * 10 + rgbDig[i];
    }

    /* Account for the scale of the number */
    if (!bOverflow && multiplier10)
    {
      for (i = 0; i < multiplier10; i++)
      {
        if (ul64 > (UI8_MAX / 10))
        {
          TRACE("Overflow scaling number\n");
          bOverflow = TRUE;
          break;
        }
        ul64 = ul64 * 10;
      }
    }

    /* If we have any fractional digits, round the value.
     * Note we don't have to do this if divisor10 is < 1,
     * because this means the fractional part must be < 0.5
     */
    if (!bOverflow && fractionalDigits && divisor10 > 0)
    {
      const BYTE* fracDig = rgbDig + wholeNumberDigits;
      BOOL bAdjust = FALSE;

      TRACE("first decimal value is %d\n", *fracDig);

      if (*fracDig > 5)
        bAdjust = TRUE; /* > 0.5 */
      else if (*fracDig == 5)
      {
        for (i = 1; i < fractionalDigits; i++)
        {
          if (fracDig[i])
          {
            bAdjust = TRUE; /* > 0.5 */
            break;
          }
        }
        /* If exactly 0.5, round only odd values */
        if (i == fractionalDigits && (ul64 & 1))
          bAdjust = TRUE;
      }

      if (bAdjust)
      {
        if (ul64 == UI8_MAX)
        {
          TRACE("Overflow after rounding\n");
          bOverflow = TRUE;
        }
        ul64++;
      }
    }

    /* Zero is not a negative number */
    bNegative = pNumprs->dwOutFlags & NUMPRS_NEG && ul64 ? TRUE : FALSE;

    TRACE("Integer value is %lld, bNeg %d\n", ul64, bNegative);

    /* For negative integers, try the signed types in size order */
    if (!bOverflow && bNegative)
    {
      if (dwVtBits & (VTBIT_I1|VTBIT_I2|VTBIT_I4|VTBIT_I8))
      {
        if (dwVtBits & VTBIT_I1 && ul64 <= -I1_MIN)
        {
          V_VT(pVarDst) = VT_I1;
          V_I1(pVarDst) = -ul64;
          return S_OK;
        }
        else if (dwVtBits & VTBIT_I2 && ul64 <= -I2_MIN)
        {
          V_VT(pVarDst) = VT_I2;
          V_I2(pVarDst) = -ul64;
          return S_OK;
        }
        else if (dwVtBits & VTBIT_I4 && ul64 <= -((LONGLONG)I4_MIN))
        {
          V_VT(pVarDst) = VT_I4;
          V_I4(pVarDst) = -ul64;
          return S_OK;
        }
        else if (dwVtBits & VTBIT_I8 && ul64 <= (ULONGLONG)I8_MAX + 1)
        {
          V_VT(pVarDst) = VT_I8;
          V_I8(pVarDst) = -ul64;
          return S_OK;
        }
        else if ((dwVtBits & REAL_VTBITS) == VTBIT_DECIMAL)
        {
          /* Decimal is only output choice left - fast path */
          V_VT(pVarDst) = VT_DECIMAL;
          DEC_SIGNSCALE(&V_DECIMAL(pVarDst)) = SIGNSCALE(DECIMAL_NEG,0);
          DEC_HI32(&V_DECIMAL(pVarDst)) = 0;
          DEC_LO64(&V_DECIMAL(pVarDst)) = -ul64;
          return S_OK;
        }
      }
    }
    else if (!bOverflow)
    {
      /* For positive integers, try signed then unsigned types in size order */
      if (dwVtBits & VTBIT_I1 && ul64 <= I1_MAX)
      {
        V_VT(pVarDst) = VT_I1;
        V_I1(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_UI1 && ul64 <= UI1_MAX)
      {
        V_VT(pVarDst) = VT_UI1;
        V_UI1(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_I2 && ul64 <= I2_MAX)
      {
        V_VT(pVarDst) = VT_I2;
        V_I2(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_UI2 && ul64 <= UI2_MAX)
      {
        V_VT(pVarDst) = VT_UI2;
        V_UI2(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_I4 && ul64 <= I4_MAX)
      {
        V_VT(pVarDst) = VT_I4;
        V_I4(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_UI4 && ul64 <= UI4_MAX)
      {
        V_VT(pVarDst) = VT_UI4;
        V_UI4(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_I8 && ul64 <= I8_MAX)
      {
        V_VT(pVarDst) = VT_I8;
        V_I8(pVarDst) = ul64;
        return S_OK;
      }
      else if (dwVtBits & VTBIT_UI8)
      {
        V_VT(pVarDst) = VT_UI8;
        V_UI8(pVarDst) = ul64;
        return S_OK;
      }
      else if ((dwVtBits & REAL_VTBITS) == VTBIT_DECIMAL)
      {
        /* Decimal is only output choice left - fast path */
        V_VT(pVarDst) = VT_DECIMAL;
        DEC_SIGNSCALE(&V_DECIMAL(pVarDst)) = SIGNSCALE(DECIMAL_POS,0);
        DEC_HI32(&V_DECIMAL(pVarDst)) = 0;
        DEC_LO64(&V_DECIMAL(pVarDst)) = ul64;
        return S_OK;
      }
    }
  }

  if (dwVtBits & REAL_VTBITS)
  {
    /* Try to put the number into a float or real */
    BOOL bOverflow = FALSE, bNegative = pNumprs->dwOutFlags & NUMPRS_NEG;
    double whole = 0.0;
    int i;

    /* Convert the number into a double */
    for (i = 0; i < pNumprs->cDig; i++)
      whole = whole * 10.0 + rgbDig[i];

    TRACE("Whole double value is %16.16g\n", whole);

    /* Account for the scale */
    while (multiplier10 > 10)
    {
      if (whole > dblMaximums[10])
      {
        dwVtBits &= ~(VTBIT_R4|VTBIT_R8|VTBIT_CY);
        bOverflow = TRUE;
        break;
      }
      whole = whole * dblMultipliers[10];
      multiplier10 -= 10;
    }
    if (multiplier10)
    {
      if (whole > dblMaximums[multiplier10])
      {
        dwVtBits &= ~(VTBIT_R4|VTBIT_R8|VTBIT_CY);
        bOverflow = TRUE;
      }
      else
        whole = whole * dblMultipliers[multiplier10];
    }

    TRACE("Scaled double value is %16.16g\n", whole);

    while (divisor10 > 10)
    {
      if (whole < dblMinimums[10])
      {
        dwVtBits &= ~(VTBIT_R4|VTBIT_R8|VTBIT_CY); /* Underflow */
        bOverflow = TRUE;
        break;
      }
      whole = whole / dblMultipliers[10];
      divisor10 -= 10;
    }
    if (divisor10)
    {
      if (whole < dblMinimums[divisor10])
      {
        dwVtBits &= ~(VTBIT_R4|VTBIT_R8|VTBIT_CY); /* Underflow */
        bOverflow = TRUE;
      }
      else
        whole = whole / dblMultipliers[divisor10];
    }
    if (!bOverflow)
      TRACE("Final double value is %16.16g\n", whole);

    if (dwVtBits & VTBIT_R4 &&
        ((whole <= R4_MAX && whole >= R4_MIN) || whole == 0.0))
    {
      TRACE("Set R4 to final value\n");
      V_VT(pVarDst) = VT_R4; /* Fits into a float */
      V_R4(pVarDst) = pNumprs->dwOutFlags & NUMPRS_NEG ? -whole : whole;
      return S_OK;
    }

    if (dwVtBits & VTBIT_R8)
    {
      TRACE("Set R8 to final value\n");
      V_VT(pVarDst) = VT_R8; /* Fits into a double */
      V_R8(pVarDst) = pNumprs->dwOutFlags & NUMPRS_NEG ? -whole : whole;
      return S_OK;
    }

    if (dwVtBits & VTBIT_CY)
    {
      if (SUCCEEDED(VarCyFromR8(bNegative ? -whole : whole, &V_CY(pVarDst))))
      {
        V_VT(pVarDst) = VT_CY; /* Fits into a currency */
        TRACE("Set CY to final value\n");
        return S_OK;
      }
      TRACE("Value Overflows CY\n");
    }
  }

  if (dwVtBits & VTBIT_DECIMAL)
  {
    int i;
    ULONG carry;
    ULONG64 tmp;
    DECIMAL* pDec = &V_DECIMAL(pVarDst);

    DECIMAL_SETZERO(pDec);
    DEC_LO32(pDec) = 0;

    if (pNumprs->dwOutFlags & NUMPRS_NEG)
      DEC_SIGN(pDec) = DECIMAL_NEG;
    else
      DEC_SIGN(pDec) = DECIMAL_POS;

    /* Factor the significant digits */
    for (i = 0; i < pNumprs->cDig; i++)
    {
      tmp = (ULONG64)DEC_LO32(pDec) * 10 + rgbDig[i];
      carry = (ULONG)(tmp >> 32);
      DEC_LO32(pDec) = (ULONG)(tmp & UI4_MAX);
      tmp = (ULONG64)DEC_MID32(pDec) * 10 + carry;
      carry = (ULONG)(tmp >> 32);
      DEC_MID32(pDec) = (ULONG)(tmp & UI4_MAX);
      tmp = (ULONG64)DEC_HI32(pDec) * 10 + carry;
      DEC_HI32(pDec) = (ULONG)(tmp & UI4_MAX);

      if (tmp >> 32 & UI4_MAX)
      {
VarNumFromParseNum_DecOverflow:
        TRACE("Overflow\n");
        DEC_LO32(pDec) = DEC_MID32(pDec) = DEC_HI32(pDec) = UI4_MAX;
        return DISP_E_OVERFLOW;
      }
    }

    /* Account for the scale of the number */
    while (multiplier10 > 0)
    {
      tmp = (ULONG64)DEC_LO32(pDec) * 10;
      carry = (ULONG)(tmp >> 32);
      DEC_LO32(pDec) = (ULONG)(tmp & UI4_MAX);
      tmp = (ULONG64)DEC_MID32(pDec) * 10 + carry;
      carry = (ULONG)(tmp >> 32);
      DEC_MID32(pDec) = (ULONG)(tmp & UI4_MAX);
      tmp = (ULONG64)DEC_HI32(pDec) * 10 + carry;
      DEC_HI32(pDec) = (ULONG)(tmp & UI4_MAX);

      if (tmp >> 32 & UI4_MAX)
        goto VarNumFromParseNum_DecOverflow;
      multiplier10--;
    }
    DEC_SCALE(pDec) = divisor10;

    V_VT(pVarDst) = VT_DECIMAL;
    return S_OK;
  }
  return DISP_E_OVERFLOW; /* No more output choices */
}
#endif

/**********************************************************************
 *              VarFormatDateTime [OLEAUT32.97]
 */
HRESULT WINAPI VarFormatDateTime(LPVARIANT var, INT format, ULONG dwFlags, BSTR *out)
{
    FIXME("%p %d %lx %p\n", var, format, dwFlags, out);
    return E_NOTIMPL;
}

/**********************************************************************
 *              VarFormatCurrency [OLEAUT32.127]
 */
HRESULT WINAPI VarFormatCurrency(LPVARIANT var, INT digits, INT lead, INT paren, INT group, ULONG dwFlags, BSTR *out)
{
    FIXME("%p %d %d %d %d %lx %p\n", var, digits, lead, paren, group, dwFlags, out);
    return E_NOTIMPL;
}

/**********************************************************************
 *              VarFormatNumber [OLEAUT32.107]
 */
HRESULT WINAPI VarFormatNumber(LPVARIANT pvarIn, int iNumDig, int iIncLead, int iUseParens, int iGroup, ULONG dwFlags, BSTR* pbstrOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}


/**********************************************************************
 *              VarFormatPercent [OLEAUT32.117]
 */
HRESULT WINAPI VarFormatPercent(LPVARIANT pvarIn, int iNumDig, int iIncLead, int iUseParens, int iGroup, ULONG dwFlags, BSTR* pbstrOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}


/**********************************************************************
 *              VarWeekdayName [OLEAUT32.128]
 */
HRESULT WINAPI VarWeekdayName(int iWeekday, int fAbbrey, int iFirstDay, ULONG dwFlags, BSTR* pbstrOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}


/**********************************************************************
 *              VarMonthName [OLEAUT32.129]
 */
HRESULT WINAPI VarMonthName(int iMonth, int fAbbrey, ULONG dwFlags, BSTR* pbstrOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}


/**********************************************************************
 *              VariantTimeToDosDateTime [OLEAUT32.13]
 * Convert variant representation of time to the date and time representation
 * stored in dos.
 */
INT WINAPI VariantTimeToDosDateTime(DATE pvtime, USHORT *wDosDate, USHORT *wDosTime)
{
    struct tm t;
    *wDosTime = 0;
    *wDosDate = 0;

    TRACE("( 0x%x, 0x%x, %p ), stub\n", *wDosDate, *wDosTime, &pvtime );

    if (DateToTm(pvtime, 0, &t) < 0) return 0;

    *wDosTime = *wDosTime | (t.tm_sec / 2);
    *wDosTime = *wDosTime | (t.tm_min << 5);
    *wDosTime = *wDosTime | (t.tm_hour << 11);

    *wDosDate = *wDosDate | t.tm_mday ;
    *wDosDate = *wDosDate | t.tm_mon << 5;
    *wDosDate = *wDosDate | ((t.tm_year - 1980) << 9) ;

    return 1;
}


/***********************************************************************
 *              SystemTimeToVariantTime [OLEAUT32.184]
 */
HRESULT WINAPI SystemTimeToVariantTime( LPSYSTEMTIME  lpSystemTime, double *pvtime )
{
    struct tm t;

    TRACE(" %d/%d/%d %d:%d:%d\n",
          lpSystemTime->wMonth, lpSystemTime->wDay,
          lpSystemTime->wYear, lpSystemTime->wHour,
          lpSystemTime->wMinute, lpSystemTime->wSecond);

    if (lpSystemTime->wYear >= 1900)
    {
        t.tm_sec = lpSystemTime->wSecond;
        t.tm_min = lpSystemTime->wMinute;
        t.tm_hour = lpSystemTime->wHour;

        t.tm_mday = lpSystemTime->wDay;
        t.tm_mon = lpSystemTime->wMonth - 1; /* tm_mon is 0..11, wMonth is 1..12 */
        t.tm_year = lpSystemTime->wYear;

        return TmToDATE( &t, pvtime );
    }
    else
    {
        double tmpDate;
        long firstDayOfNextYear;
        long thisDay;
        long leftInYear;
        long result;

        double decimalPart = 0.0;

        t.tm_sec = lpSystemTime->wSecond;
        t.tm_min = lpSystemTime->wMinute;
        t.tm_hour = lpSystemTime->wHour;

        /* Step year forward the same number of years before 1900 */
        t.tm_year = 1900 + 1899 - lpSystemTime->wYear;
        t.tm_mon = lpSystemTime->wMonth - 1;
        t.tm_mday = lpSystemTime->wDay;

        /* Calculate date */
        TmToDATE( &t, pvtime );

        thisDay = (double) floor( *pvtime );
        decimalPart = fmod( *pvtime, thisDay );

        /* Now, calculate the same time for the first of Jan that year */
        t.tm_mon = 0;
        t.tm_mday = 1;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_year = t.tm_year+1;
        TmToDATE( &t, &tmpDate );
        firstDayOfNextYear = (long) floor(tmpDate);

        /* Finally since we know the size of the year, subtract the two to get
           remaining time in the year                                          */
        leftInYear = firstDayOfNextYear - thisDay;

        /* Now we want full years up to the year in question, and remainder of year
           of the year in question */
        if (isleap(lpSystemTime->wYear) ) {
           TRACE("Extra day due to leap year\n");
           result = 2.0 - ((firstDayOfNextYear - 366) + leftInYear - 2.0);
        } else {
           result = 2.0 - ((firstDayOfNextYear - 365) + leftInYear - 2.0);
        }
        *pvtime = (double) result + decimalPart;
        TRACE("<1899 support: returned %f, 1st day %ld, thisday %ld, left %ld\n", *pvtime, firstDayOfNextYear, thisDay, leftInYear);

        return 1;
    }

    return 0;
}

/***********************************************************************
 *              VariantTimeToSystemTime [OLEAUT32.185]
 */
HRESULT WINAPI VariantTimeToSystemTime( double vtime, LPSYSTEMTIME  lpSystemTime )
{
    double t = 0, timeofday = 0;

    static const BYTE Days_Per_Month[] =    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const BYTE Days_Per_Month_LY[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* The Month_Code is used to find the Day of the Week (LY = LeapYear)*/
    static const BYTE Month_Code[] =    {0, 1, 4, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};
    static const BYTE Month_Code_LY[] = {0, 0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6};

    /* The Century_Code is used to find the Day of the Week */
    static const BYTE Century_Code[]  = {0, 6, 4, 2};

    struct tm r;

    TRACE(" Variant = %f SYSTEMTIME ptr %p\n", vtime, lpSystemTime);

    if (vtime >= 0)
    {

        if (DateToTm(vtime, 0, &r ) <= 0) return 0;

        lpSystemTime->wSecond = r.tm_sec;
        lpSystemTime->wMinute = r.tm_min;
        lpSystemTime->wHour = r.tm_hour;
        lpSystemTime->wDay = r.tm_mday;
        lpSystemTime->wMonth = r.tm_mon;

        if (lpSystemTime->wMonth == 12)
            lpSystemTime->wMonth = 1;
        else
            lpSystemTime->wMonth++;

        lpSystemTime->wYear = r.tm_year;
    }
    else
    {
        vtime = -1*vtime;

        if (DateToTm(vtime, 0, &r ) <= 0) return 0;

        lpSystemTime->wSecond = r.tm_sec;
        lpSystemTime->wMinute = r.tm_min;
        lpSystemTime->wHour = r.tm_hour;

        lpSystemTime->wMonth = 13 - r.tm_mon;

        if (lpSystemTime->wMonth == 1)
            lpSystemTime->wMonth = 12;
        else
            lpSystemTime->wMonth--;

        lpSystemTime->wYear = 1899 - (r.tm_year - 1900);

        if (!isleap(lpSystemTime->wYear) )
            lpSystemTime->wDay = Days_Per_Month[13 - lpSystemTime->wMonth] - r.tm_mday;
        else
            lpSystemTime->wDay = Days_Per_Month_LY[13 - lpSystemTime->wMonth] - r.tm_mday;


    }

    if (!isleap(lpSystemTime->wYear))
    {
        /*
          (Century_Code+Month_Code+Year_Code+Day) % 7

          The century code repeats every 400 years , so the array
          works out like this,

          Century_Code[0] is for 16th/20th Centry
          Century_Code[1] is for 17th/21th Centry
          Century_Code[2] is for 18th/22th Centry
          Century_Code[3] is for 19th/23th Centry

          The year code is found with the formula (year + (year / 4))
          the "year" must be between 0 and 99 .

          The Month Code (Month_Code[1]) starts with January and
          ends with December.
        */

        lpSystemTime->wDayOfWeek = (
            Century_Code[(( (lpSystemTime->wYear+100) - lpSystemTime->wYear%100) /100) %4]+
            ((lpSystemTime->wYear%100)+(lpSystemTime->wYear%100)/4)+
            Month_Code[lpSystemTime->wMonth]+
            lpSystemTime->wDay) % 7;

        if (lpSystemTime->wDayOfWeek == 0) lpSystemTime->wDayOfWeek = 7;
        else lpSystemTime->wDayOfWeek -= 1;
    }
    else
    {
        lpSystemTime->wDayOfWeek = (
            Century_Code[(((lpSystemTime->wYear+100) - lpSystemTime->wYear%100)/100)%4]+
            ((lpSystemTime->wYear%100)+(lpSystemTime->wYear%100)/4)+
            Month_Code_LY[lpSystemTime->wMonth]+
            lpSystemTime->wDay) % 7;

        if (lpSystemTime->wDayOfWeek == 0) lpSystemTime->wDayOfWeek = 7;
        else lpSystemTime->wDayOfWeek -= 1;
    }

    t = floor(vtime);
    timeofday = vtime - t;

    lpSystemTime->wMilliseconds = (timeofday
                                   - lpSystemTime->wHour*(1/24)
                                   - lpSystemTime->wMinute*(1/1440)
                                   - lpSystemTime->wSecond*(1/86400) )*(1/5184000);

    return 1;
}

/***********************************************************************
 *              VarUdateFromDate [OLEAUT32.331]
 */
HRESULT WINAPI VarUdateFromDate( DATE datein, ULONG dwFlags, UDATE *pudateout)
{
    HRESULT i = 0;
    static const BYTE Days_Per_Month[] =    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static const BYTE Days_Per_Month_LY[] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    TRACE("DATE = %f\n", (double)datein);
    i = VariantTimeToSystemTime(datein, &(pudateout->st) );

    if (i)
    {
        pudateout->wDayOfYear = 0;

        if (isleap(pudateout->st.wYear))
        {
            for (i =1; i<pudateout->st.wMonth; i++)
                pudateout->wDayOfYear += Days_Per_Month[i];
        }
        else
        {
            for (i =1; i<pudateout->st.wMonth; i++)
                pudateout->wDayOfYear += Days_Per_Month_LY[i];
        }

        pudateout->wDayOfYear += pudateout->st.wDay;
        dwFlags = 0; /*VAR_VALIDDATE*/
    }
    else dwFlags = 0;

    return i;
}

/***********************************************************************
 *              VarDateFromUdate [OLEAUT32.330]
 */
HRESULT WINAPI VarDateFromUdate(UDATE *pudateout,
                                ULONG dwFlags, DATE *datein)
{
    HRESULT i;
    double t = 0;
    TRACE(" %d/%d/%d %d:%d:%d\n",
          pudateout->st.wMonth, pudateout->st.wDay,
          pudateout->st.wYear, pudateout->st.wHour,
          pudateout->st.wMinute, pudateout->st.wSecond);


    i = SystemTimeToVariantTime(&(pudateout->st), &t);
    *datein = t;

    if (i) return S_OK;
    else return E_INVALIDARG;
}


/**********************************************************************
 *              VarBstrCmp [OLEAUT32.314]
 *
 * flags can be:
 *   NORM_IGNORECASE, NORM_IGNORENONSPACE, NORM_IGNORESYMBOLS
 *   NORM_IGNORESTRINGWIDTH, NORM_IGNOREKANATYPE, NORM_IGNOREKASHIDA
 *
 */
HRESULT WINAPI VarBstrCmp(BSTR left, BSTR right, LCID lcid, DWORD flags)
{
    INT r;

    TRACE("( %s %s %ld %lx ) partial stub\n", debugstr_w(left), debugstr_w(right), lcid, flags);

    /* Contrary to the MSDN, this returns eq for null vs null, null vs L"" and L"" vs NULL */
    if((!left) || (!right)) {

        if (!left && (!right || *right==0)) return VARCMP_EQ;
        else if (!right && (!left || *left==0)) return VARCMP_EQ;
        else return VARCMP_NULL;
    }

    if(flags&NORM_IGNORECASE)
        r = lstrcmpiW(left,right);
    else
        r = lstrcmpW(left,right);

    if(r<0)
        return VARCMP_LT;
    if(r>0)
        return VARCMP_GT;

    return VARCMP_EQ;
}

/**********************************************************************
 *              VarBstrCat [OLEAUT32.313]
 */
HRESULT WINAPI VarBstrCat(BSTR left, BSTR right, BSTR *out)
{
    BSTR result;
    int size = 0;

    TRACE("( %s %s %p )\n", debugstr_w(left), debugstr_w(right), out);

    /* On Windows, NULL parms are still handled (as empty strings) */
    if (left)  size=size + lstrlenW(left);
    if (right) size=size + lstrlenW(right);

    if (out) {
        result = SysAllocStringLen(NULL, size);
        *out = result;
        if (left) lstrcatW(result,left);
        if (right) lstrcatW(result,right);
        TRACE("result = %s, [%p]\n", debugstr_w(result), result);
    }
    return S_OK;
}

/**********************************************************************
 *              VarCat [OLEAUT32.318]
 */
HRESULT WINAPI VarCat(LPVARIANT left, LPVARIANT right, LPVARIANT out)
{
    /* Should we VariantClear out? */
    /* Can we handle array, vector, by ref etc. */
    if ((V_VT(left)&VT_TYPEMASK) == VT_NULL &&
        (V_VT(right)&VT_TYPEMASK) == VT_NULL)
    {
        V_VT(out) = VT_NULL;
        return S_OK;
    }
    else if (V_VT(left) == VT_BSTR && V_VT(right) == VT_BSTR)
    {
        V_VT(out) = VT_BSTR;
        VarBstrCat (V_BSTR(left), V_BSTR(right), &V_BSTR(out));
        return S_OK;
    }
    else
        FIXME ("types not supported\n");
    return S_OK;
}

/**********************************************************************
 *              VarCmp [OLEAUT32.176]
 *
 * flags can be:
 *   NORM_IGNORECASE, NORM_IGNORENONSPACE, NORM_IGNORESYMBOLS
 *   NORM_IGNOREWIDTH, NORM_IGNOREKANATYPE, NORM_IGNOREKASHIDA
 *
 */
HRESULT WINAPI VarCmp(LPVARIANT left, LPVARIANT right, LCID lcid, DWORD flags)
{


    BOOL         lOk        = TRUE;
    BOOL         rOk        = TRUE;
    LONGLONG     lVal = -1;
    LONGLONG     rVal = -1;

    TRACE("Left Var:\n");
    dump_Variant(left);
    TRACE("Right Var:\n");
    dump_Variant(right);

    /* If either are null, then return VARCMP_NULL */
    if ((V_VT(left)&VT_TYPEMASK) == VT_NULL ||
        (V_VT(right)&VT_TYPEMASK) == VT_NULL)
        return VARCMP_NULL;

    /* Strings - use VarBstrCmp */
    if ((V_VT(left)&VT_TYPEMASK) == VT_BSTR &&
        (V_VT(right)&VT_TYPEMASK) == VT_BSTR) {
        return VarBstrCmp(V_BSTR(left), V_BSTR(right), lcid, flags);
    }

    /* Integers - Ideally like to use VarDecCmp, but no Dec support yet
           Use LONGLONG to maximize ranges                              */
    lOk = TRUE;
    switch (V_VT(left)&VT_TYPEMASK) {
    case VT_I1   : lVal = V_UNION(left,cVal); break;
    case VT_I2   : lVal = V_UNION(left,iVal); break;
    case VT_I4   : lVal = V_UNION(left,lVal); break;
    case VT_INT  : lVal = V_UNION(left,lVal); break;
    case VT_UI1  : lVal = V_UNION(left,bVal); break;
    case VT_UI2  : lVal = V_UNION(left,uiVal); break;
    case VT_UI4  : lVal = V_UNION(left,ulVal); break;
    case VT_UINT : lVal = V_UNION(left,ulVal); break;
    default: lOk = FALSE;
    }

    rOk = TRUE;
    switch (V_VT(right)&VT_TYPEMASK) {
    case VT_I1   : rVal = V_UNION(right,cVal); break;
    case VT_I2   : rVal = V_UNION(right,iVal); break;
    case VT_I4   : rVal = V_UNION(right,lVal); break;
    case VT_INT  : rVal = V_UNION(right,lVal); break;
    case VT_UI1  : rVal = V_UNION(right,bVal); break;
    case VT_UI2  : rVal = V_UNION(right,uiVal); break;
    case VT_UI4  : rVal = V_UNION(right,ulVal); break;
    case VT_UINT : rVal = V_UNION(right,ulVal); break;
#ifdef __WIN32OS2__
    case VT_R8   : rVal = V_UNION(right,dblVal); break;
#endif
    default: rOk = FALSE;
    }

    if (lOk && rOk) {
        if (lVal < rVal) {
            return VARCMP_LT;
        } else if (lVal > rVal) {
            return VARCMP_GT;
        } else {
            return VARCMP_EQ;
        }
    }

    /* Strings - use VarBstrCmp */
    if ((V_VT(left)&VT_TYPEMASK) == VT_DATE &&
        (V_VT(right)&VT_TYPEMASK) == VT_DATE) {

        if (floor(V_UNION(left,date)) == floor(V_UNION(right,date))) {
            /* Due to floating point rounding errors, calculate varDate in whole numbers) */
            double wholePart = 0.0;
            double leftR;
            double rightR;

            /* Get the fraction * 24*60*60 to make it into whole seconds */
            wholePart = (double) floor( V_UNION(left,date) );
            if (wholePart == 0) wholePart = 1;
            leftR = floor(fmod( V_UNION(left,date), wholePart ) * (24*60*60));

            wholePart = (double) floor( V_UNION(right,date) );
            if (wholePart == 0) wholePart = 1;
            rightR = floor(fmod( V_UNION(right,date), wholePart ) * (24*60*60));

            if (leftR < rightR) {
                return VARCMP_LT;
            } else if (leftR > rightR) {
                return VARCMP_GT;
            } else {
                return VARCMP_EQ;
            }

        } else if (V_UNION(left,date) < V_UNION(right,date)) {
            return VARCMP_LT;
        } else if (V_UNION(left,date) > V_UNION(right,date)) {
            return VARCMP_GT;
        }
    }


    FIXME("VarCmp partial implementation, doesnt support these pair of variant types");
    return E_FAIL;
}

/**********************************************************************
 *              VarAnd [OLEAUT32.142]
 *
 */
HRESULT WINAPI VarAnd(LPVARIANT left, LPVARIANT right, LPVARIANT result)
{
    HRESULT rc = E_FAIL;


    TRACE("Left Var:\n");
    dump_Variant(left);
    TRACE("Right Var:\n");
    dump_Variant(right);

    if ((V_VT(left)&VT_TYPEMASK) == VT_BOOL &&
        (V_VT(right)&VT_TYPEMASK) == VT_BOOL) {

        V_VT(result) = VT_BOOL;
        if (V_BOOL(left) && V_BOOL(right)) {
            V_BOOL(result) = VARIANT_TRUE;
        } else {
            V_BOOL(result) = VARIANT_FALSE;
        }
        rc = S_OK;

    } else {
        FIXME("VarAnd stub\n");
    }

    TRACE("rc=%d, Result:\n", (int) rc);
    dump_Variant(result);
    return rc;
}

/**********************************************************************
 *              VarOr [OLEAUT32.157]
 *
 * Perform a logical or (OR) operation on two variants.
 *
 * PARAMS
 *  pVarLeft  [I] First variant
 *  pVarRight [I] Variant to OR with pVarLeft
 *  pVarOut   [O] Destination for OR result
 *
 * RETURNS
 *  Success: S_OK. pVarOut contains the result of the operation with its type
 *           taken from the table listed under VarXor().
 *  Failure: An HRESULT error code indicating the error.
 *
 * NOTES
 *  See the Notes section of VarXor() for further information.
 */
HRESULT WINAPI VarOr(LPVARIANT pVarLeft, LPVARIANT pVarRight, LPVARIANT pVarOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;


#if 0
    VARTYPE vt = VT_I4;
    VARIANT varLeft, varRight, varStr;
    HRESULT hRet;

#if 0
    TRACE("(%p->(%s%s),%p->(%s%s),%p)\n", pVarLeft, debugstr_VT(pVarLeft),
          debugstr_VF(pVarLeft), pVarRight, debugstr_VT(pVarRight),
          debugstr_VF(pVarRight), pVarOut);
#endif
    if (V_EXTRA_TYPE(pVarLeft) || V_EXTRA_TYPE(pVarRight) ||
        V_VT(pVarLeft) == VT_UNKNOWN || V_VT(pVarRight) == VT_UNKNOWN ||
        V_VT(pVarLeft) == VT_DISPATCH || V_VT(pVarRight) == VT_DISPATCH ||
        V_VT(pVarLeft) == VT_RECORD || V_VT(pVarRight) == VT_RECORD)
        return DISP_E_BADVARTYPE;

    V_VT(&varLeft) = V_VT(&varRight) = V_VT(&varStr) = VT_EMPTY;

    if (V_VT(pVarLeft) == VT_NULL || V_VT(pVarRight) == VT_NULL)
    {
        /* NULL OR Zero is NULL, NULL OR value is value */
        if (V_VT(pVarLeft) == VT_NULL)
            pVarLeft = pVarRight; /* point to the non-NULL var */

        V_VT(pVarOut) = VT_NULL;
        V_I4(pVarOut) = 0;

        switch (V_VT(pVarLeft))
        {
        case VT_DATE: case VT_R8:
            if (V_R8(pVarLeft))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_BOOL:
            if (V_BOOL(pVarLeft))
                *pVarOut = *pVarLeft;
            return S_OK;
         case VT_I2: case VT_UI2:
            if (V_I2(pVarLeft))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_I1:
            if (V_I1(pVarLeft))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_UI1:
            if (V_UI1(pVarLeft))
                *pVarOut = *pVarLeft;
            return S_OK;
        case VT_R4:
            if (V_R4(pVarLeft))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_I4: case VT_UI4: case VT_INT: case VT_UINT:
            if (V_I4(pVarLeft))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_CY:
            if (V_CY(pVarLeft).int64)
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_I8: case VT_UI8:
            if (V_I8(pVarLeft))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_DECIMAL:
            if (DEC_HI32(&V_DECIMAL(pVarLeft)) || DEC_LO64(&V_DECIMAL(pVarLeft)))
                goto VarOr_AsEmpty;
            return S_OK;
        case VT_BSTR:
        {
            VARIANT_BOOL b;

            if (!V_BSTR(pVarLeft))
                return DISP_E_BADVARTYPE;

            hRet = VarBoolFromStr(V_BSTR(pVarLeft), LOCALE_USER_DEFAULT, VAR_LOCALBOOL, &b);
            if (SUCCEEDED(hRet) && b)
            {
                V_VT(pVarOut) = VT_BOOL;
                V_BOOL(pVarOut) = b;
            }
            return hRet;
        }
        case VT_NULL: case VT_EMPTY:
            V_VT(pVarOut) = VT_NULL;
            return S_OK;
        default:
            return DISP_E_BADVARTYPE;
        }
    }

    if (V_VT(pVarLeft) == VT_EMPTY || V_VT(pVarRight) == VT_EMPTY)
    {
        if (V_VT(pVarLeft) == VT_EMPTY)
            pVarLeft = pVarRight; /* point to the non-EMPTY var */

VarOr_AsEmpty:
        /* Since one argument is empty (0), OR'ing it with the other simply
         * gives the others value (as 0|x => x). So just convert the other
         * argument to the required result type.
         */
        switch (V_VT(pVarLeft))
        {
        case VT_BSTR:
            if (!V_BSTR(pVarLeft))
                return DISP_E_BADVARTYPE;

            hRet = VariantCopy(&varStr, pVarLeft);
            if (FAILED(hRet))
                goto VarOr_Exit;
            pVarLeft = &varStr;
            hRet = VariantChangeType(pVarLeft, pVarLeft, 0, VT_BOOL);
            if (FAILED(hRet))
                goto VarOr_Exit;
            /* Fall Through ... */
        case VT_EMPTY: case VT_UI1: case VT_BOOL: case VT_I2:
            V_VT(pVarOut) = VT_I2;
            break;
        case VT_DATE: case VT_CY: case VT_DECIMAL: case VT_R4: case VT_R8:
        case VT_I1: case VT_UI2: case VT_I4: case VT_UI4:
        case VT_INT: case VT_UINT: case VT_UI8:
            V_VT(pVarOut) = VT_I4;
            break;
        case VT_I8:
            V_VT(pVarOut) = VT_I8;
            break;
        default:
            return DISP_E_BADVARTYPE;
        }
        hRet = VariantCopy(&varLeft, pVarLeft);
        if (FAILED(hRet))
            goto VarOr_Exit;
        pVarLeft = &varLeft;
        hRet = VariantChangeType(pVarOut, pVarLeft, 0, V_VT(pVarOut));
        goto VarOr_Exit;
    }

    if (V_VT(pVarLeft) == VT_BOOL && V_VT(pVarRight) == VT_BOOL)
    {
        V_VT(pVarOut) = VT_BOOL;
        V_BOOL(pVarOut) = V_BOOL(pVarLeft) | V_BOOL(pVarRight);
        return S_OK;
    }

    if (V_VT(pVarLeft) == VT_UI1 && V_VT(pVarRight) == VT_UI1)
    {
        V_VT(pVarOut) = VT_UI1;
        V_UI1(pVarOut) = V_UI1(pVarLeft) | V_UI1(pVarRight);
        return S_OK;
    }

    if (V_VT(pVarLeft) == VT_BSTR)
    {
        hRet = VariantCopy(&varStr, pVarLeft);
        if (FAILED(hRet))
            goto VarOr_Exit;
        pVarLeft = &varStr;
        hRet = VariantChangeType(pVarLeft, pVarLeft, 0, VT_BOOL);
        if (FAILED(hRet))
            goto VarOr_Exit;
    }

    if (V_VT(pVarLeft) == VT_BOOL &&
        (V_VT(pVarRight) == VT_BOOL || V_VT(pVarRight) == VT_BSTR))
    {
        vt = VT_BOOL;
    }
    else if ((V_VT(pVarLeft) == VT_BOOL || V_VT(pVarLeft) == VT_UI1 ||
        V_VT(pVarLeft) == VT_I2 || V_VT(pVarLeft) == VT_BSTR) &&
        (V_VT(pVarRight) == VT_BOOL || V_VT(pVarRight) == VT_UI1 ||
        V_VT(pVarRight) == VT_I2 || V_VT(pVarRight) == VT_BSTR))
    {
        vt = VT_I2;
    }
    else if (V_VT(pVarLeft) == VT_I8 || V_VT(pVarRight) == VT_I8)
    {
        if (V_VT(pVarLeft) == VT_INT || V_VT(pVarRight) == VT_INT)
            return DISP_E_TYPEMISMATCH;
        vt = VT_I8;
    }

    hRet = VariantCopy(&varLeft, pVarLeft);
    if (FAILED(hRet))
        goto VarOr_Exit;

    hRet = VariantCopy(&varRight, pVarRight);
    if (FAILED(hRet))
        goto VarOr_Exit;

    if (vt == VT_I4 && V_VT(&varLeft) == VT_UI4)
        V_VT(&varLeft) = VT_I4; /* Don't overflow */
    else
    {
        double d;

        if (V_VT(&varLeft) == VT_BSTR &&
            FAILED(VarR8FromStr(V_BSTR(&varLeft), LOCALE_USER_DEFAULT, 0, &d)))
            hRet = VariantChangeType(&varLeft, &varLeft, VARIANT_LOCALBOOL, VT_BOOL);
        if (SUCCEEDED(hRet) && V_VT(&varLeft) != vt)
            hRet = VariantChangeType(&varLeft, &varLeft, 0, vt);
        if (FAILED(hRet))
            goto VarOr_Exit;
    }

    if (vt == VT_I4 && V_VT(&varRight) == VT_UI4)
        V_VT(&varRight) = VT_I4; /* Don't overflow */
    else
    {
        double d;

        if (V_VT(&varRight) == VT_BSTR &&
            FAILED(VarR8FromStr(V_BSTR(&varRight), LOCALE_USER_DEFAULT, 0, &d)))
            hRet = VariantChangeType(&varRight, &varRight, VARIANT_LOCALBOOL, VT_BOOL);
        if (SUCCEEDED(hRet) && V_VT(&varRight) != vt)
            hRet = VariantChangeType(&varRight, &varRight, 0, vt);
        if (FAILED(hRet))
            goto VarOr_Exit;
    }

    V_VT(pVarOut) = vt;
    if (vt == VT_I8)
    {
        V_I8(pVarOut) = V_I8(&varLeft) | V_I8(&varRight);
    }
    else if (vt == VT_I4)
    {
        V_I4(pVarOut) = V_I4(&varLeft) | V_I4(&varRight);
    }
    else
    {
        V_I2(pVarOut) = V_I2(&varLeft) | V_I2(&varRight);
    }

VarOr_Exit:
    VariantClear(&varStr);
    VariantClear(&varLeft);
    VariantClear(&varRight);
    return hRet;
#endif
}


/**********************************************************************
 *              VarXor [OLEAUT32.167]
 */
HRESULT WINAPI VarXor(LPVARIANT pvarLeft, LPVARIANT pvarRight, LPVARIANT pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);

  return E_NOTIMPL;
}

/**********************************************************************
 *              VarNot [OLEAUT32.174]
 *
 */
HRESULT WINAPI VarNot(LPVARIANT in, LPVARIANT result)
{
    HRESULT rc = E_FAIL;

    TRACE("Var In:\n");
    dump_Variant(in);

    if ((V_VT(in)&VT_TYPEMASK) == VT_BOOL) {

        V_VT(result) = VT_BOOL;
        if (V_BOOL(in)) {
            V_BOOL(result) = VARIANT_FALSE;
        } else {
            V_BOOL(result) = VARIANT_TRUE;
        }
        rc = S_OK;

    } else {
        FIXME("VarNot stub\n");
    }

    TRACE("rc=%d, Result:\n", (int) rc);
    dump_Variant(result);
    return rc;
}

/**********************************************************************
 *              VarNeg [OLEAUT32.173]
 *
 * Negate the value of a variant.
 *
 * PARAMS
 *  pVarIn  [I] Source variant
 *  pVarOut [O] Destination for converted value
 *
 * RETURNS
 *  Success: S_OK. pVarOut contains the converted value.
 *  Failure: An HRESULT error code indicating the error.
 *
 * NOTES
 *  - The type of the value stored in pVarOut depends on the type of pVarIn,
 *    according to the following table:
 *| Input Type       Output Type
 *| ----------       -----------
 *|  VT_EMPTY         VT_I2
 *|  VT_UI1           VT_I2
 *|  VT_BOOL          VT_I2
 *|  VT_BSTR          VT_R8
 *|  All Others       Unchanged (unless promoted)
 *  - Where the negated value of a variant does not fit in its base type, the type
 *    is promoted according to the following table:
 *| Input Type       Promoted To
 *| ----------       -----------
 *|   VT_I2            VT_I4
 *|   VT_I4            VT_R8
 *|   VT_I8            VT_R8
 *  - The native version of this function returns DISP_E_BADVARTYPE for valid
 *    variant types that cannot be negated, and returns DISP_E_TYPEMISMATCH
 *    for types which are not valid. Since this is in contravention of the
 *    meaning of those error codes and unlikely to be relied on by applications,
 *    this implementation returns errors consistent with the other high level
 *    variant math functions.
 */
HRESULT WINAPI VarNeg(LPVARIANT pVarIn, LPVARIANT pVarOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 * VarAbs [OLEAUT32.168]
 *
 * Convert a variant to its absolute value.
 *
 * PARAMS
 *  pVarIn  [I] Source variant
 *  pVarOut [O] Destination for converted value
 *
 * RETURNS
 *  Success: S_OK. pVarOut contains the absolute value of pVarIn.
 *  Failure: An HRESULT error code indicating the error.
 *
 * NOTES
 *  - This function does not process by-reference variants.
 *  - The type of the value stored in pVarOut depends on the type of pVarIn,
 *    according to the following table:
 *| Input Type       Output Type
 *| ----------       -----------
 *| VT_BOOL          VT_I2
 *| VT_BSTR          VT_R8
 *| (All others)     Unchanged
 */
HRESULT WINAPI VarAbs(LPVARIANT pVarIn, LPVARIANT pVarOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarFix [OLEAUT32.169]
 */
HRESULT WINAPI VarFix(LPVARIANT pvarIn, LPVARIANT pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarInt [OLEAUT32.172]
 */
HRESULT WINAPI VarInt(LPVARIANT pvarIn, LPVARIANT pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarImp [OLEAUT32.154]
 */
HRESULT WINAPI VarImp(LPVARIANT pvarLeft, LPVARIANT pvarRight, LPVARIANT pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarMod [OLEAUT32.155]
 */
HRESULT WINAPI VarMod(LPVARIANT pvarLeft, LPVARIANT pvarRight, LPVARIANT pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarEqv [OLEAUT32.152]
 *
 * Determine if two variants contain the same value.
 *
 * PARAMS
 *  pVarLeft  [I] First variant to compare
 *  pVarRight [I] Variant to compare to pVarLeft
 *  pVarOut   [O] Destination for comparison result
 *
 * RETURNS
 *  Success: S_OK. pVarOut contains the result of the comparison (VARIANT_TRUE
 *           if equivalent or non-zero otherwise.
 *  Failure: An HRESULT error code indicating the error.
 *
 * NOTES
 *  - This function simply calls VarXor() on pVarLeft and pVarRight and inverts
 *    the result.
 */
HRESULT WINAPI VarEqv(LPVARIANT pVarLeft, LPVARIANT pVarRight, LPVARIANT pVarOut)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}


/**********************************************************************
 *              VarIdiv [OLEAUT32.153]
 */
HRESULT WINAPI VarIdiv(LPVARIANT pvarLeft, LPVARIANT pvarRight, LPVARIANT pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);

  return E_NOTIMPL;
}

/**********************************************************************
 *              VarRound [OLEAUT32.175]
 */
HRESULT VarRound(LPVARIANT  pvarIn, INT cDecimals, LPVARIANT  pvarResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);

  return E_NOTIMPL;
}

/**********************************************************************
 *              VarPow [OLEAUT32.158]
 *
 */
HRESULT WINAPI VarPow(LPVARIANT left, LPVARIANT right, LPVARIANT result)
{
    HRESULT hr;
    VARIANT dl,dr;

#if 0
    TRACE("(%p->(%s%s),%p->(%s%s),%p)\n", left, debugstr_VT(left), debugstr_VF(left),
          right, debugstr_VT(right), debugstr_VF(right), result);
#endif

    hr = VariantChangeType(&dl,left,0,VT_R8);
    if (!SUCCEEDED(hr)) {
        ERR("Could not change passed left argument to VT_R8, handle it differently.\n");
        return E_FAIL;
    }
    hr = VariantChangeType(&dr,right,0,VT_R8);
    if (!SUCCEEDED(hr)) {
        ERR("Could not change passed right argument to VT_R8, handle it differently.\n");
        return E_FAIL;
    }
    V_VT(result) = VT_R8;
    V_R8(result) = pow(V_R8(&dl),V_R8(&dr));
    return S_OK;
}

/**********************************************************************
 *              VarAdd [OLEAUT32.141]
 * FIXME: From MSDN: If ... Then
 * Both expressions are of the string type Concatenated.
 * One expression is a string type and the other a character Addition.
 * One expression is numeric and the other is a string Addition.
 * Both expressions are numeric Addition.
 * Either expression is NULL NULL is returned.
 * Both expressions are empty  Integer subtype is returned.
 *
 */
HRESULT WINAPI VarAdd(LPVARIANT left, LPVARIANT right, LPVARIANT result)
{
    HRESULT rc = E_FAIL;

#if 0
    TRACE("(%p->(%s%s),%p->(%s%s),%p)\n", left, debugstr_VT(left),
          debugstr_VF(left), right, debugstr_VT(right), debugstr_VF(right), result);
#endif

    if ((V_VT(left)&VT_TYPEMASK) == VT_EMPTY)
    	return VariantCopy(result,right);

    if ((V_VT(right)&VT_TYPEMASK) == VT_EMPTY)
    	return VariantCopy(result,left);

    /* check if we add doubles */
    if (((V_VT(left)&VT_TYPEMASK) == VT_R8) || ((V_VT(right)&VT_TYPEMASK) == VT_R8)) {
        BOOL         lOk        = TRUE;
        BOOL         rOk        = TRUE;
        double       lVal = -1;
        double       rVal = -1;
        double       res  = -1;

        lOk = TRUE;
        switch (V_VT(left)&VT_TYPEMASK) {
        case VT_I1   : lVal = V_UNION(left,cVal);   break;
        case VT_I2   : lVal = V_UNION(left,iVal);   break;
        case VT_I4   : lVal = V_UNION(left,lVal);   break;
        case VT_INT  : lVal = V_UNION(left,lVal);   break;
        case VT_UI1  : lVal = V_UNION(left,bVal);   break;
        case VT_UI2  : lVal = V_UNION(left,uiVal);  break;
        case VT_UI4  : lVal = V_UNION(left,ulVal);  break;
        case VT_UINT : lVal = V_UNION(left,ulVal);  break;
        case VT_R4   : lVal = V_UNION(left,fltVal);  break;
        case VT_R8   : lVal = V_UNION(left,dblVal);  break;
	case VT_NULL : lVal = 0.0;  break;
        default: lOk = FALSE;
        }

        rOk = TRUE;
        switch (V_VT(right)&VT_TYPEMASK) {
        case VT_I1   : rVal = V_UNION(right,cVal);  break;
        case VT_I2   : rVal = V_UNION(right,iVal);  break;
        case VT_I4   : rVal = V_UNION(right,lVal);  break;
        case VT_INT  : rVal = V_UNION(right,lVal);  break;
        case VT_UI1  : rVal = V_UNION(right,bVal);  break;
        case VT_UI2  : rVal = V_UNION(right,uiVal); break;
        case VT_UI4  : rVal = V_UNION(right,ulVal); break;
        case VT_UINT : rVal = V_UNION(right,ulVal); break;
        case VT_R4   : rVal = V_UNION(right,fltVal);break;
        case VT_R8   : rVal = V_UNION(right,dblVal);break;
	case VT_NULL : rVal = 0.0; break;
        default: rOk = FALSE;
        }

        if (lOk && rOk) {
            res = (lVal + rVal);
            V_VT(result) = VT_R8;
            V_UNION(result,dblVal)  = res;
            rc = S_OK;
        } else {
	    FIXME("Unhandled type pair %d / %d in double addition.\n",
        	(V_VT(left)&VT_TYPEMASK),
        	(V_VT(right)&VT_TYPEMASK)
	    );
	}
	return rc;
    }

    /* now check if we add floats. VT_R8 can no longer happen here! */
    if (((V_VT(left)&VT_TYPEMASK) == VT_R4) || ((V_VT(right)&VT_TYPEMASK) == VT_R4)) {
        BOOL         lOk        = TRUE;
        BOOL         rOk        = TRUE;
        float        lVal = -1;
        float        rVal = -1;
        float        res  = -1;

        lOk = TRUE;
        switch (V_VT(left)&VT_TYPEMASK) {
        case VT_I1   : lVal = V_UNION(left,cVal);   break;
        case VT_I2   : lVal = V_UNION(left,iVal);   break;
        case VT_I4   : lVal = V_UNION(left,lVal);   break;
        case VT_INT  : lVal = V_UNION(left,lVal);   break;
        case VT_UI1  : lVal = V_UNION(left,bVal);   break;
        case VT_UI2  : lVal = V_UNION(left,uiVal);  break;
        case VT_UI4  : lVal = V_UNION(left,ulVal);  break;
        case VT_UINT : lVal = V_UNION(left,ulVal);  break;
        case VT_R4   : lVal = V_UNION(left,fltVal);  break;
	case VT_NULL : lVal = 0.0;  break;
        default: lOk = FALSE;
        }

        rOk = TRUE;
        switch (V_VT(right)&VT_TYPEMASK) {
        case VT_I1   : rVal = V_UNION(right,cVal);  break;
        case VT_I2   : rVal = V_UNION(right,iVal);  break;
        case VT_I4   : rVal = V_UNION(right,lVal);  break;
        case VT_INT  : rVal = V_UNION(right,lVal);  break;
        case VT_UI1  : rVal = V_UNION(right,bVal);  break;
        case VT_UI2  : rVal = V_UNION(right,uiVal); break;
        case VT_UI4  : rVal = V_UNION(right,ulVal); break;
        case VT_UINT : rVal = V_UNION(right,ulVal); break;
        case VT_R4   : rVal = V_UNION(right,fltVal);break;
	case VT_NULL : rVal = 0.0; break;
        default: rOk = FALSE;
        }

        if (lOk && rOk) {
            res = (lVal + rVal);
            V_VT(result) = VT_R4;
            V_UNION(result,fltVal)  = res;
            rc = S_OK;
        } else {
	    FIXME("Unhandled type pair %d / %d in float addition.\n",
        	(V_VT(left)&VT_TYPEMASK),
        	(V_VT(right)&VT_TYPEMASK)
	    );
	}
	return rc;
    }

    /* Handle strings as concat */
    if ((V_VT(left)&VT_TYPEMASK) == VT_BSTR &&
        (V_VT(right)&VT_TYPEMASK) == VT_BSTR) {
        V_VT(result) = VT_BSTR;
        return VarBstrCat(V_BSTR(left), V_BSTR(right), &V_BSTR(result));
    } else {

        /* Integers */
        BOOL         lOk        = TRUE;
        BOOL         rOk        = TRUE;
        LONGLONG     lVal = -1;
        LONGLONG     rVal = -1;
        LONGLONG     res  = -1;
        int          resT = 0; /* Testing has shown I2 + I2 == I2, all else
                                  becomes I4                                */

        lOk = TRUE;
        switch (V_VT(left)&VT_TYPEMASK) {
        case VT_I1   : lVal = V_UNION(left,cVal);  resT=VT_I4; break;
        case VT_I2   : lVal = V_UNION(left,iVal);  resT=VT_I2; break;
        case VT_I4   : lVal = V_UNION(left,lVal);  resT=VT_I4; break;
        case VT_INT  : lVal = V_UNION(left,lVal);  resT=VT_I4; break;
        case VT_UI1  : lVal = V_UNION(left,bVal);  resT=VT_I4; break;
        case VT_UI2  : lVal = V_UNION(left,uiVal); resT=VT_I4; break;
        case VT_UI4  : lVal = V_UNION(left,ulVal); resT=VT_I4; break;
        case VT_UINT : lVal = V_UNION(left,ulVal); resT=VT_I4; break;
	case VT_NULL : lVal = 0; resT = VT_I4; break;
        default: lOk = FALSE;
        }

        rOk = TRUE;
        switch (V_VT(right)&VT_TYPEMASK) {
        case VT_I1   : rVal = V_UNION(right,cVal);  resT=VT_I4; break;
        case VT_I2   : rVal = V_UNION(right,iVal);  resT=max(VT_I2, resT); break;
        case VT_I4   : rVal = V_UNION(right,lVal);  resT=VT_I4; break;
        case VT_INT  : rVal = V_UNION(right,lVal);  resT=VT_I4; break;
        case VT_UI1  : rVal = V_UNION(right,bVal);  resT=VT_I4; break;
        case VT_UI2  : rVal = V_UNION(right,uiVal); resT=VT_I4; break;
        case VT_UI4  : rVal = V_UNION(right,ulVal); resT=VT_I4; break;
        case VT_UINT : rVal = V_UNION(right,ulVal); resT=VT_I4; break;
	case VT_NULL : rVal = 0; resT=VT_I4; break;
        default: rOk = FALSE;
        }

        if (lOk && rOk) {
            res = (lVal + rVal);
            V_VT(result) = resT;
            switch (resT) {
            case VT_I2   : V_UNION(result,iVal)  = res; break;
            case VT_I4   : V_UNION(result,lVal)  = res; break;
            default:
                FIXME("Unexpected result variant type %x\n", resT);
                V_UNION(result,lVal)  = res;
            }
            rc = S_OK;

        } else {
            FIXME("unimplemented part (0x%x + 0x%x)\n",V_VT(left), V_VT(right));
        }
    }
#if 0
    TRACE("returning 0x%8lx (%s%s),%ld\n", rc, debugstr_VT(result),
          debugstr_VF(result), V_VT(result) == VT_I4 ? V_I4(result) : V_I2(result));
#endif
    return rc;
}

/**********************************************************************
 *              VarSub [OLEAUT32.159]
 *
 */
HRESULT WINAPI VarSub(LPVARIANT left, LPVARIANT right, LPVARIANT result)
{
    HRESULT rc = E_FAIL;
    VARTYPE lvt,rvt,resvt;
    VARIANT lv,rv;
    BOOL found;

#if 0
    TRACE("(%p->(%s%s),%p->(%s%s),%p)\n", left, debugstr_VT(left),
          debugstr_VF(left), right, debugstr_VT(right), debugstr_VF(right), result);
#endif

    VariantInit(&lv);VariantInit(&rv);
    lvt = V_VT(left)&VT_TYPEMASK;
    rvt = V_VT(right)&VT_TYPEMASK;
    found = FALSE;resvt = VT_VOID;
    if (((1<<lvt) | (1<<rvt)) & ((1<<VT_DATE)|(1<<VT_R4)|(1<<VT_R8))) {
	found = TRUE;
	resvt = VT_R8;
    }
    if (!found && (((1<<lvt) | (1<<rvt)) & ((1<<VT_I1)|(1<<VT_I2)|(1<<VT_UI1)|(1<<VT_UI2)|(1<<VT_I4)|(1<<VT_UI4)|(1<<VT_INT)|(1<<VT_UINT)))) {
	found = TRUE;
	resvt = VT_I4;
    }
    if (!found) {
	FIXME("can't expand vt %d vs %d to a target type.\n",lvt,rvt);
	return E_FAIL;
    }
    rc = VariantChangeType(&lv, left, 0, resvt);
    if (FAILED(rc)) {
	FIXME("Could not convert 0x%x to %d?\n",V_VT(left),resvt);
	return rc;
    }
    rc = VariantChangeType(&rv, right, 0, resvt);
    if (FAILED(rc)) {
	FIXME("Could not convert 0x%x to %d?\n",V_VT(right),resvt);
	return rc;
    }
    switch (resvt) {
    case VT_R8:
	V_VT(result) = resvt;
	V_R8(result) = V_R8(&lv) - V_R8(&rv);
	rc = S_OK;
	break;
    case VT_I4:
	V_VT(result) = resvt;
	V_I4(result) = V_I4(&lv) - V_I4(&rv);
	rc = S_OK;
	break;
    }
#if 0
    TRACE("returning 0x%8lx (%s%s),%g\n", rc, debugstr_VT(result),
          debugstr_VF(result), V_VT(result) == VT_R8 ? V_R8(result) : (double)V_I4(result));
#endif
    return rc;
}


/**********************************************************************
 *              VarDiv [OLEAUT32.143]
 *
 */
HRESULT WINAPI VarDiv(LPVARIANT left, LPVARIANT right, LPVARIANT result)
{
    HRESULT rc = E_FAIL;
    VARTYPE lvt,rvt,resvt;
    VARIANT lv,rv;
    BOOL found;

#if 0
    TRACE("(%p->(%s%s),%p->(%s%s),%p)\n", left, debugstr_VT(left),
          debugstr_VF(left), right, debugstr_VT(right), debugstr_VF(right), result);
#endif

    VariantInit(&lv);VariantInit(&rv);
    lvt = V_VT(left)&VT_TYPEMASK;
    rvt = V_VT(right)&VT_TYPEMASK;
    found = FALSE;resvt = VT_VOID;
    if (((1<<lvt) | (1<<rvt)) & ((1<<VT_R4)|(1<<VT_R8))) {
	found = TRUE;
	resvt = VT_R8;
    }
    if (!found && (((1<<lvt) | (1<<rvt)) & ((1<<VT_I1)|(1<<VT_I2)|(1<<VT_UI1)|(1<<VT_UI2)|(1<<VT_I4)|(1<<VT_UI4)|(1<<VT_INT)|(1<<VT_UINT)))) {
	found = TRUE;
	resvt = VT_I4;
    }
    if (!found) {
	FIXME("can't expand vt %d vs %d to a target type.\n",lvt,rvt);
	return E_FAIL;
    }
    rc = VariantChangeType(&lv, left, 0, resvt);
    if (FAILED(rc)) {
	FIXME("Could not convert 0x%x to %d?\n",V_VT(left),resvt);
	return rc;
    }
    rc = VariantChangeType(&rv, right, 0, resvt);
    if (FAILED(rc)) {
	FIXME("Could not convert 0x%x to %d?\n",V_VT(right),resvt);
	return rc;
    }
    switch (resvt) {
    case VT_R8:
	V_VT(result) = resvt;
	V_R8(result) = V_R8(&lv) / V_R8(&rv);
	rc = S_OK;
	break;
    case VT_I4:
	V_VT(result) = resvt;
	V_I4(result) = V_I4(&lv) / V_I4(&rv);
	rc = S_OK;
	break;
    }
#if 0
    TRACE("returning 0x%8lx (%s%s),%g\n", rc, debugstr_VT(result),
          debugstr_VF(result), V_VT(result) == VT_R8 ? V_R8(result) : (double)V_I4(result));
#endif
    return rc;
}

/**********************************************************************
 *              VarMul [OLEAUT32.156]
 *
 */
HRESULT WINAPI VarMul(LPVARIANT left, LPVARIANT right, LPVARIANT result)
{
    HRESULT rc = E_FAIL;
    VARTYPE lvt,rvt,resvt;
    VARIANT lv,rv;
    BOOL found;
#if 0
    TRACE("(%p->(%s%s),%p->(%s%s),%p)\n", left, debugstr_VT(left),
          debugstr_VF(left), right, debugstr_VT(right), debugstr_VF(right), result);
#endif
    VariantInit(&lv);VariantInit(&rv);
    lvt = V_VT(left)&VT_TYPEMASK;
    rvt = V_VT(right)&VT_TYPEMASK;
    found = FALSE;resvt=VT_VOID;
    if (((1<<lvt) | (1<<rvt)) & ((1<<VT_R4)|(1<<VT_R8))) {
	found = TRUE;
	resvt = VT_R8;
    }
    if (!found && (((1<<lvt) | (1<<rvt)) & ((1<<VT_I1)|(1<<VT_I2)|(1<<VT_UI1)|(1<<VT_UI2)|(1<<VT_I4)|(1<<VT_UI4)|(1<<VT_INT)|(1<<VT_UINT)))) {
	found = TRUE;
	resvt = VT_I4;
    }
    if (!found) {
	FIXME("can't expand vt %d vs %d to a target type.\n",lvt,rvt);
	return E_FAIL;
    }
    rc = VariantChangeType(&lv, left, 0, resvt);
    if (FAILED(rc)) {
	FIXME("Could not convert 0x%x to %d?\n",V_VT(left),resvt);
	return rc;
    }
    rc = VariantChangeType(&rv, right, 0, resvt);
    if (FAILED(rc)) {
	FIXME("Could not convert 0x%x to %d?\n",V_VT(right),resvt);
	return rc;
    }
    switch (resvt) {
    case VT_R8:
	V_VT(result) = resvt;
	V_R8(result) = V_R8(&lv) * V_R8(&rv);
	rc = S_OK;
	break;
    case VT_I4:
	V_VT(result) = resvt;
	V_I4(result) = V_I4(&lv) * V_I4(&rv);
	rc = S_OK;
	break;
    }
#if 0
    TRACE("returning 0x%8lx (%s%s),%g\n", rc, debugstr_VT(result),
          debugstr_VF(result), V_VT(result) == VT_R8 ? V_R8(result) : (double)V_I4(result));
#endif
    return rc;
}

/**********************************************************************
 *              VarTokenizeFormatString [OLEAUT32.140]
 *
 * From investigation on W2K, a list is built up which is:
 *
 * <0x00> AA BB - Copy from AA for BB chars (Note 1 byte with wrap!)
 * <token> - Insert appropriate token
 *
 */
HRESULT WINAPI VarTokenizeFormatString(LPOLESTR  format, LPBYTE rgbTok,
                     int   cbTok, int iFirstDay, int iFirstWeek,
                     LCID  lcid, int *pcbActual) {

    FORMATHDR *hdr;
    int        realLen, formatLeft;
    BYTE      *pData;
    LPSTR      pFormatA, pStart;
    int        checkStr;
    BOOL       insertCopy = FALSE;
    LPSTR      copyFrom = NULL;

    TRACE("'%s', %p %d %d %d only date support\n", debugstr_w(format), rgbTok, cbTok,
                   iFirstDay, iFirstWeek);

    /* Big enough for header? */
    if (cbTok < sizeof(FORMATHDR)) {
        return TYPE_E_BUFFERTOOSMALL;
    }

    /* Insert header */
    hdr = (FORMATHDR *) rgbTok;
    memset(hdr, 0x00, sizeof(FORMATHDR));
    hdr->hex3 = 0x03; /* No idea what these are */
    hdr->hex6 = 0x06;

    /* Start parsing string */
    realLen    = sizeof(FORMATHDR);
    pData      = rgbTok + realLen;
    pFormatA   = HEAP_strdupWtoA( GetProcessHeap(), 0, format );
    pStart     = pFormatA;
    formatLeft = strlen(pFormatA);

    /* Work through the format */
    while (*pFormatA != 0x00) {

        checkStr = 0;
        while (checkStr>=0 && (formatTokens[checkStr].tokenSize != 0x00)) {
            if (formatLeft >= formatTokens[checkStr].tokenSize &&
                strncmp(formatTokens[checkStr].str, pFormatA,
                        formatTokens[checkStr].tokenSize) == 0) {
                TRACE("match on '%s'\n", formatTokens[checkStr].str);

                /* Found Match! */

                /* If we have skipped chars, insert the copy */
                if (insertCopy == TRUE) {

                    if ((realLen + 3) > cbTok) {
                        HeapFree( GetProcessHeap(), 0, pFormatA );
                        return TYPE_E_BUFFERTOOSMALL;
                    }
                    insertCopy = FALSE;
                    *pData = TOK_COPY;
                    pData++;
                    *pData = (BYTE)(copyFrom - pStart);
                    pData++;
                    *pData = (BYTE)(pFormatA - copyFrom);
                    pData++;
                    realLen = realLen + 3;
                }


                /* Now insert the token itself */
                if ((realLen + 1) > cbTok) {
                    HeapFree( GetProcessHeap(), 0, pFormatA );
                    return TYPE_E_BUFFERTOOSMALL;
                }
                *pData = formatTokens[checkStr].tokenId;
                pData = pData + 1;
                realLen = realLen + 1;

                pFormatA = pFormatA + formatTokens[checkStr].tokenSize;
                formatLeft = formatLeft - formatTokens[checkStr].tokenSize;
                checkStr = -1; /* Flag as found and break out of while loop */
            } else {
                checkStr++;
            }
        }

        /* Did we ever match a token? */
        if (checkStr != -1 && insertCopy == FALSE) {
            TRACE("No match - need to insert copy from %p [%p]\n", pFormatA, pStart);
            insertCopy = TRUE;
            copyFrom   = pFormatA;
        } else if (checkStr != -1) {
            pFormatA = pFormatA + 1;
        }

    }

    /* Finally, if we have skipped chars, insert the copy */
    if (insertCopy == TRUE) {

        TRACE("Chars left over, so still copy %p,%p,%p\n", copyFrom, pStart, pFormatA);
        if ((realLen + 3) > cbTok) {
            HeapFree( GetProcessHeap(), 0, pFormatA );
            return TYPE_E_BUFFERTOOSMALL;
        }
        insertCopy = FALSE;
        *pData = TOK_COPY;
        pData++;
        *pData = (BYTE)(copyFrom - pStart);
        pData++;
        *pData = (BYTE)(pFormatA - copyFrom);
        pData++;
        realLen = realLen + 3;
    }

    /* Finally insert the terminator */
    if ((realLen + 1) > cbTok) {
        HeapFree( GetProcessHeap(), 0, pFormatA );
        return TYPE_E_BUFFERTOOSMALL;
    }
    *pData++ = TOK_END;
    realLen = realLen + 1;

    /* Finally fill in the length */
    hdr->len = realLen;
    *pcbActual = realLen;

#if 0
    { int i,j;
      for (i=0; i<realLen; i=i+0x10) {
          printf(" %4.4x : ", i);
          for (j=0; j<0x10 && (i+j < realLen); j++) {
              printf("%2.2x ", rgbTok[i+j]);
          }
          printf("\n");
      }
    }
#endif
    HeapFree( GetProcessHeap(), 0, pFormatA );

    return S_OK;
}

/**********************************************************************
 *              VarFormatFromTokens [OLEAUT32.139]
 * FIXME: No account of flags or iFirstDay etc
 */
HRESULT WINAPI VarFormatFromTokens(LPVARIANT varIn, LPOLESTR format,
                            LPBYTE pbTokCur, ULONG dwFlags, BSTR *pbstrOut,
                            LCID  lcid) {

    FORMATHDR   *hdr = (FORMATHDR *)pbTokCur;
    BYTE        *pData    = pbTokCur + sizeof (FORMATHDR);
    LPSTR        pFormatA = HEAP_strdupWtoA( GetProcessHeap(), 0, format );
    char         output[BUFFER_MAX];
    char        *pNextPos;
    int          size, whichToken;
    VARIANTARG   Variant;
    struct tm    TM;



    TRACE("'%s', %p %lx %p only date support\n", pFormatA, pbTokCur, dwFlags, pbstrOut);
    TRACE("varIn:\n");
    dump_Variant(varIn);

    memset(output, 0x00, BUFFER_MAX);
    pNextPos = output;

    while (*pData != TOK_END && ((pData - pbTokCur) <= (hdr->len))) {

        TRACE("Output looks like : '%s'\n", output);

        /* Convert varient to appropriate data type */
        whichToken = 0;
        while ((formatTokens[whichToken].tokenSize != 0x00) &&
               (formatTokens[whichToken].tokenId   != *pData)) {
            whichToken++;
        }

        /* Use Variant local from here downwards as always correct type */
        if (formatTokens[whichToken].tokenSize > 0 &&
            formatTokens[whichToken].varTypeRequired != 0) {
			VariantInit( &Variant );
            if (Coerce( &Variant, lcid, dwFlags, varIn,
                        formatTokens[whichToken].varTypeRequired ) != S_OK) {
                HeapFree( GetProcessHeap(), 0, pFormatA );
                return DISP_E_TYPEMISMATCH;
            } else if (formatTokens[whichToken].varTypeRequired == VT_DATE) {
                if( DateToTm( V_UNION(&Variant,date), dwFlags, &TM ) == FALSE ) {
                    HeapFree( GetProcessHeap(), 0, pFormatA );
                    return E_INVALIDARG;
                }
            }
        }

        TRACE("Looking for match on token '%x'\n", *pData);
        switch (*pData) {
        case TOK_COPY:
            TRACE("Copy from %d for %d bytes\n", *(pData+1), *(pData+2));
            memcpy(pNextPos, &pFormatA[*(pData+1)], *(pData+2));
            pNextPos = pNextPos + *(pData+2);
            pData = pData + 3;
            break;

        case TOK_COLON   :
            /* Get locale information - Time Seperator */
            size = GetLocaleInfoA(lcid, LOCALE_STIME, NULL, 0);
            GetLocaleInfoA(lcid, LOCALE_STIME, pNextPos, size);
            TRACE("TOK_COLON Time seperator is '%s'\n", pNextPos);
            pNextPos = pNextPos + size;
            pData = pData + 1;
            break;

        case TOK_SLASH   :
            /* Get locale information - Date Seperator */
            size = GetLocaleInfoA(lcid, LOCALE_SDATE, NULL, 0);
            GetLocaleInfoA(lcid, LOCALE_SDATE, pNextPos, size);
            TRACE("TOK_COLON Time seperator is '%s'\n", pNextPos);
            pNextPos = pNextPos + size;
            pData = pData + 1;
            break;

        case TOK_d       :
            sprintf(pNextPos, "%d", TM.tm_mday);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_dd      :
            sprintf(pNextPos, "%2.2d", TM.tm_mday);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_w       :
            sprintf(pNextPos, "%d", TM.tm_wday+1);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_m       :
            sprintf(pNextPos, "%d", TM.tm_mon+1);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_mm      :
            sprintf(pNextPos, "%2.2d", TM.tm_mon+1);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_q       :
            sprintf(pNextPos, "%d", ((TM.tm_mon+1)/4)+1);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_y       :
            sprintf(pNextPos, "%2.2d", TM.tm_yday+1);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_yy      :
            sprintf(pNextPos, "%2.2d", TM.tm_year);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_yyyy    :
            sprintf(pNextPos, "%4.4d", TM.tm_year);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_h       :
            sprintf(pNextPos, "%d", TM.tm_hour);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_Hh      :
            sprintf(pNextPos, "%2.2d", TM.tm_hour);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_N       :
            sprintf(pNextPos, "%d", TM.tm_min);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_Nn      :
            sprintf(pNextPos, "%2.2d", TM.tm_min);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_S       :
            sprintf(pNextPos, "%d", TM.tm_sec);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        case TOK_Ss      :
            sprintf(pNextPos, "%2.2d", TM.tm_sec);
            pNextPos = pNextPos + strlen(pNextPos);
            pData = pData + 1;
            break;

        /* FIXME: To Do! */
        case TOK_ttttt   :
        case TOK_AMsPM   :
        case TOK_amspm   :
        case TOK_AsP     :
        case TOK_asp     :
        case TOK_AMPM    :
        case TOK_c       :
        case TOK_ddd     :
        case TOK_dddd    :
        case TOK_ddddd   :
        case TOK_dddddd  :
        case TOK_ww      :
        case TOK_mmm     :
        case TOK_mmmm    :
        default:
            FIXME("Unhandled token for VarFormat %d\n", *pData);
            HeapFree( GetProcessHeap(), 0, pFormatA );
            return E_INVALIDARG;
        }

    }

    *pbstrOut = StringDupAtoBstr( output );
    HeapFree( GetProcessHeap(), 0, pFormatA );
    return S_OK;
}

/**********************************************************************
 *              VarFormat [OLEAUT32.87]
 *
 */
HRESULT WINAPI VarFormat(LPVARIANT varIn, LPOLESTR format,
                         int firstDay, int firstWeek, ULONG dwFlags,
                         BSTR *pbstrOut) {

    LPSTR   pNewString = NULL;
    HRESULT rc = S_OK;

    TRACE("mostly stub! format='%s' day=%d, wk=%d, flags=%ld\n",
          debugstr_w(format), firstDay, firstWeek, dwFlags);
    TRACE("varIn:\n");
    dump_Variant(varIn);

    /* Note: Must Handle references type Variants (contain ptrs
          to values rather than values */

    /* Get format string */
    pNewString = HEAP_strdupWtoA( GetProcessHeap(), 0, format );

    /* FIXME: Handle some simple pre-definted format strings : */
    if (((V_VT(varIn)&VT_TYPEMASK) == VT_CY) && (lstrcmpiA(pNewString, "Currency") == 0)) {

        /* Can't use VarBstrFromCy as it does not put currency sign on nor decimal places */
        double curVal;


        /* Handle references type Variants (contain ptrs to values rather than values */
        if (V_VT(varIn)&VT_BYREF) {
            rc = VarR8FromCy(*(CY *)V_UNION(varIn,byref), &curVal);
        } else {
            rc = VarR8FromCy(V_UNION(varIn,cyVal), &curVal);
        }

        if (rc == S_OK) {
            char tmpStr[BUFFER_MAX];
            sprintf(tmpStr, "%f", curVal);
            if (GetCurrencyFormatA(GetUserDefaultLCID(), dwFlags, tmpStr, NULL, pBuffer, BUFFER_MAX) == 0) {
                return E_FAIL;
            } else {
                *pbstrOut = StringDupAtoBstr( pBuffer );
            }
        }

    } else if ((V_VT(varIn)&VT_TYPEMASK) == VT_DATE) {

        /* Attempt to do proper formatting! */
        int firstToken = -1;

        rc = VarTokenizeFormatString(format, pBuffer, sizeof(pBuffer), firstDay,
                                  firstWeek, GetUserDefaultLCID(), &firstToken);
        if (rc==S_OK) {
            rc = VarFormatFromTokens(varIn, format, pBuffer, dwFlags, pbstrOut, GetUserDefaultLCID());
        }

    } else if ((V_VT(varIn)&VT_TYPEMASK) == VT_R8) {
        if (V_VT(varIn)&VT_BYREF) {
            sprintf(pBuffer, "%f", *(double *)V_UNION(varIn,byref));
        } else {
            sprintf(pBuffer, "%f", V_UNION(varIn,dblVal));
        }

        *pbstrOut = StringDupAtoBstr( pBuffer );

    } else {
        FIXME("VarFormat: Unsupported format %d!\n", V_VT(varIn)&VT_TYPEMASK);
        *pbstrOut = StringDupAtoBstr( "??" );
    }

    /* Free allocated storage */
    HeapFree( GetProcessHeap(), 0, pNewString );
    TRACE("result: '%s'\n", debugstr_w(*pbstrOut));
    return rc;
}

/**********************************************************************
 *              VarCyMulI4 [OLEAUT32.304]
 * Multiply currency value by integer
 */
HRESULT WINAPI VarCyMulI4(CY cyIn, LONG mulBy, CY *pcyOut) {

    double cyVal = 0;
    HRESULT rc = S_OK;

    rc = VarR8FromCy(cyIn, &cyVal);
    if (rc == S_OK) {
        rc = VarCyFromR8((cyVal * (double) mulBy), pcyOut);
        TRACE("Multiply %f by %ld = %f [%ld,%lu]\n", cyVal, mulBy, (cyVal * (double) mulBy),
                    pcyOut->s.Hi, pcyOut->s.Lo);
    }
    return rc;
}


/**********************************************************************
 *              VarDecAdd [OLEAUT32.177]
 */
HRESULT WINAPI VarDecAdd(LPDECIMAL pdecLeft, LPDECIMAL  pdecRight, LPDECIMAL  pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecSub [OLEAUT32.181]
 */
HRESULT WINAPI VarDecSub(LPDECIMAL pdecLeft, LPDECIMAL  pdecRight, LPDECIMAL  pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecMul [OLEAUT32.179]
 */
HRESULT WINAPI VarDecMul(LPDECIMAL pdecLeft, LPDECIMAL  pdecRight, LPDECIMAL  pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecRound [OLEAUT32.203]
 */
HRESULT WINAPI VarDecRound(LPDECIMAL pdecIn, int iDecimals, LPDECIMAL  pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecDiv [OLEAUT32.178]
 */
HRESULT WINAPI VarDecDiv(LPDECIMAL pdecLeft, LPDECIMAL  pdecRight, LPDECIMAL  pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecCmp [OLEAUT32.204]
 */
HRESULT WINAPI VarDecCmp(LPDECIMAL pdecLeft, LPDECIMAL  pdecRight)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecAbs [OLEAUT32.182]
 */
HRESULT WINAPI VarDecAbs(LPDECIMAL pdecIn, LPDECIMAL pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecFix [OLEAUT32.187]
 */
HRESULT WINAPI VarDecFix(LPDECIMAL pdecIn, LPDECIMAL pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecInt [OLEAUT32.188]
 */
HRESULT WINAPI VarDecInt(LPDECIMAL pdecIn, LPDECIMAL pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}


/**********************************************************************
 *              VarDecNeg [OLEAUT32.189]
 */
HRESULT WINAPI VarDecNeg(LPDECIMAL pdecIn, LPDECIMAL pdecResult)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/**********************************************************************
 *              VarDecCmpR8 [OLEAUT32.298]
 */
HRESULT WINAPI VarDecCmpR8(LPDECIMAL pdecLeft, LPDECIMAL  pdecRight)
{
  FIXME("%s: Not implemented! (returning E_NOTIMPL)", __FUNCTION__);
  return E_NOTIMPL;
}

/************************************************************************
 * VarDecFromStr (OLEAUT32.197)
 *
 * Convert a VT_BSTR to a DECIMAL.
 *
 * PARAMS
 *  strIn   [I] Source
 *  lcid    [I] LCID for the conversion
 *  dwFlags [I] Flags controlling the conversion (VAR_ flags from "oleauto.h")
 *  pDecOut [O] Destination
 *
 * RETURNS
 *  Success: S_OK.
 *  Failure: DISP_E_OVERFLOW, if the value will not fit in the destination
 */
HRESULT WINAPI VarDecFromStr(OLECHAR* strIn, LCID lcid, ULONG dwFlags, DECIMAL* pDecOut)
{
  TRACE("%s: now calling VARIANT_NumberFromBstr\n", __FUNCTION__);
  return _VarDecFromStr(strIn, lcid, dwFlags, pDecOut);
}

