/* $Id: uniconv.h,v 1.6 2000-04-28 15:15:18 achimha Exp $ */
/*
 * OS/2 Unicode API definitions
 *
 * Copyright 1999 Achim Hasenmueller
 *           1999 Przemyslaw Dobrowolski
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _ULS_UCONV
#define _ULS_UCONV
#ifdef __cplusplus
extern "C" {
#endif
//@@@AH workaround for wrc bug
#ifndef RC_INVOKED
#include <stddef.h>
#include <time.h>
#endif
#define ULS_API_ERROR_BASE 0x00020400
#define ULS_API_ERROR(x)   (ULS_API_ERROR_BASE | (x))
typedef enum _uls_return_codes {
     ULS_SUCCESS           =  0x00000000,
     ULS_OTHER             =  ULS_API_ERROR(1),
     ULS_ILLEGALSEQUENCE   =  ULS_API_ERROR(2),
     ULS_MAXFILESPERPROC   =  ULS_API_ERROR(3),
     ULS_MAXFILES          =  ULS_API_ERROR(4),
     ULS_NOOP              =  ULS_API_ERROR(5),
     ULS_TOOMANYKBD        =  ULS_API_ERROR(6),
     ULS_KBDNOTFOUND       =  ULS_API_ERROR(7),
     ULS_BADHANDLE         =  ULS_API_ERROR(8),
     ULS_NODEAD            =  ULS_API_ERROR(9),
     ULS_NOSCAN            =  ULS_API_ERROR(10),
     ULS_INVALIDSCAN       =  ULS_API_ERROR(11),
     ULS_NOTIMPLEMENTED    =  ULS_API_ERROR(12),
     ULS_NOMEMORY          =  ULS_API_ERROR(13),
     ULS_INVALID           =  ULS_API_ERROR(14),
     ULS_BADOBJECT         =  ULS_API_ERROR(15),
     ULS_NOTOKEN           =  ULS_API_ERROR(16),
     ULS_NOMATCH           =  ULS_API_ERROR(17),
     ULS_BUFFERFULL        =  ULS_API_ERROR(18),
     ULS_RANGE             =  ULS_API_ERROR(19),
     ULS_UNSUPPORTED       =  ULS_API_ERROR(20),
     ULS_BADATTR           =  ULS_API_ERROR(21),
     ULS_VERSION           =  ULS_API_ERROR(22)
} uls_error_t;
#ifndef UNI_SUCCESS
  #define UNI_SUCCESS                         ULS_SUCCESS
#endif
#ifndef NO_ERROR
  #define NO_ERROR                            0
#endif
#ifndef UNI_UNSUPPORTED
  #define UNI_UNSUPPORTED                     ULS_UNSUPPORTED
#endif
#ifndef UNI_NOMEMORY
  #define UNI_NOMEMORY                        ULS_NOMEMORY
#endif
#ifndef UNI_INVALID
  #define UNI_INVALID                         ULS_INVALID
#endif
#ifndef UNI_BADOBJ
  #define UNI_BADOBJ                          ULS_BADOBJECT
#endif
#ifndef UNI_NOTOKEN
  #define UNI_NOTOKEN                         ULS_NOTOKEN
#endif
#ifndef UNI_NOMATCH
  #define UNI_NOMATCH                         ULS_NOMATCH
#endif
#ifndef UNI_BUFFER_FULL
  #define UNI_BUFFER_FULL                     ULS_BUFFERFULL
#endif
#ifndef UNI_RANGE
  #define UNI_RANGE                           ULS_RANGE
#endif
#ifndef ERR_NOOP
  #define ERR_NOOP                            ULS_NOOP
#endif
#ifndef ERR_TOO_MANY_KBD
  #define ERR_TOO_MANY_KBD                    ULS_TOOMANYKBD
#endif
#ifndef ERR_KBD_NOT_FOUND
  #define ERR_KBD_NOT_FOUND                   ULS_KBDNOTFOUND
#endif
#ifndef ERR_KBD_INVALID
  #define ERR_KBD_INVALID                     ULS_INVALID
#endif
#ifndef ERR_BAD_HANDLE
  #define ERR_BAD_HANDLE                      ULS_BADHANDLE
#endif
#ifndef ERR_NO_DEAD
  #define ERR_NO_DEAD                         ULS_NODEAD
#endif
#ifndef ERR_NO_SCAN
  #define ERR_NO_SCAN                         ULS_NOSCAN
#endif
#ifndef ERR_INVALID_SCAN
  #define ERR_INVALID_SCAN                    ULS_INVALIDSCAN
#endif
#ifndef ERR_NOT_IMPLEMENTED
  #define ERR_NOT_IMPLEMENTED                 ULS_NOTIMPLEMENTED
#endif
#ifdef ERR_NO_MEMORY
#undef ERR_NO_MEMORY
#endif
#define ERR_NO_MEMORY                       ULS_NOMEMORY
typedef int LocaleItem;
#define D_T_FMT    1
#define D_FMT      2
#define T_FMT      3
#define AM_STR     4
#define PM_STR     5
#define ABDAY_1    6
#define ABDAY_2    7
#define ABDAY_3    8
#define ABDAY_4    9
#define ABDAY_5   10
#define ABDAY_6   11
#define ABDAY_7   12
#define DAY_1     13
#define DAY_2     14
#define DAY_3     15
#define DAY_4     16
#define DAY_5     17
#define DAY_6     18
#define DAY_7     19
#define ABMON_1   20
#define ABMON_2   21
#define ABMON_3   22
#define ABMON_4   23
#define ABMON_5   24
#define ABMON_6   25
#define ABMON_7   26
#define ABMON_8   27
#define ABMON_9   28
#define ABMON_10  29
#define ABMON_11  30
#define ABMON_12  31
#define MON_1     32
#define MON_2     33
#define MON_3     34
#define MON_4     35
#define MON_5     36
#define MON_6     37
#define MON_7     38
#define MON_8     39
#define MON_9     40
#define MON_10    41
#define MON_11    42
#define MON_12    43
#define RADIXCHAR 44
#define THOUSEP   45
#define YESSTR    46
#define NOSTR     47
#define CRNCYSTR  48
#define CODESET 49  
#define T_FMT_AMPM    55
#define ERA           56
#define ERA_D_FMT     57
#define ERA_D_T_FMT   58
#define ERA_T_FMT     59
#define ALT_DIGITS    60
#define YESEXPR       61
#define NOEXPR        62
#define DATESEP       63
#define TIMESEP       64
#define LISTSEP       65
#ifndef _NL_NUM_ITEMS
#define _NL_NUM_ITEMS 66
#endif
// Evil UNI Items hackeckeur ;-)
// PD 21-07-99: Undefined Uni items!!!! Thanx IBM for undocumented items :-P
// Names like regedit2 uses:
#define XLOCALETOKEN      50
#define XWINLOCALE        51
#define ICURRENCY         75
#define INEGCURR          83
#define SINTLSYMBOL       87
#define SABBREVLANGNAME   88 
#define SLANGUAGE        104
#define WTIMEFORMAT      123 
#define WSHORTDATE       124
#define WLONGDATE        125
#define SENGCOUNTRYNAME  105
#define SENGLANGUAGE     103 
#define SCOUNTRY         106 // Fixme or 107?
#define IANSICODEPAGE    110
#define ICODEPAGE        111
#define IALTCODEPAGE     112
#define IMACCODEPAGE     113
#define ICOUNTRY         108
#define SISO639LANGNAME  101
#define SISO3166CTRYNAME 102
#define SABBREVCTRYNAME  102
#define SNATIVEDIGITS     53


#if defined(__IBMCPP__) || defined(__IBMC__)
   #define CALLCONV    _System
#else
   #define CALLCONV
#endif
typedef int ulsBool;
#ifndef TRUE
   #define TRUE      1
#endif
#ifndef FALSE
   #define FALSE     0
#endif
#ifndef _ULS_UNICHAR_DEFINED
    typedef unsigned  short  UniChar ;
    #define _ULS_UNICHAR_DEFINED
#endif
typedef void  *LocaleObject;
typedef unsigned int LocaleToken;
typedef void *AttrObject;
typedef void *XformObject;
#define UNI_TOKEN_POINTER       1
#define UNI_MBS_STRING_POINTER  2
#define UNI_UCS_STRING_POINTER  3
#undef LANG
#undef LC_ALL
#undef LC_COLLATE
#undef LC_CTYPE
#undef LC_NUMERIC
#undef LC_MONETARY
#undef LC_TIME
#undef LC_MESSAGES
#define LANG         (-2)
#define LC_ALL       (-1)
#define LC_COLLATE    0
#define LC_CTYPE      1
#define LC_NUMERIC    2
#define LC_MONETARY   3
#define LC_TIME       4
#define LC_MESSAGES   5
struct UniLconv {
   UniChar *decimal_point;
   UniChar *thousands_sep;
   short   *grouping;
   UniChar *int_curr_symbol;
   UniChar *currency_symbol;
   UniChar *mon_decimal_point;
   UniChar *mon_thousands_sep;
   short   *mon_grouping;
   UniChar *positive_sign;
   UniChar *negative_sign;
   short   int_frac_digits;
   short   frac_digits;
   short   p_cs_precedes;
   short   p_sep_by_space;
   short   n_cs_precedes;
   short   n_sep_by_space;
   short   p_sign_posn;
   short   n_sign_posn;
   short   os2_mondecpt;
   UniChar *debit_sign;
   UniChar *credit_sign;
   UniChar *left_parenthesis;
   UniChar *right_parenthesis;
};
int     CALLCONV UniCreateLocaleObject ( int locale_spec_type, const void *locale_spec,
                            LocaleObject *locale_object_ptr );
int     CALLCONV UniQueryLocaleObject  ( const LocaleObject locale_object, int category,
                            int locale_spec_type, void **locale_spec_ptr );
int     CALLCONV UniFreeLocaleObject   ( LocaleObject locale_object );
int     CALLCONV UniFreeMem            ( void *memory_ptr );
int     CALLCONV UniLocaleStrToToken   ( int locale_string_type, const void *locale_string,
                            LocaleToken *locale_token_ptr );
int     CALLCONV UniLocaleTokenToStr   ( const LocaleToken locale_token,
                            int locale_string_type, void **locale_string_ptr );
int     CALLCONV UniQueryLocaleInfo ( const LocaleObject locale_object,
                         struct UniLconv **unilconv_addr_ptr );
int     CALLCONV UniFreeLocaleInfo  ( struct UniLconv *unilconv_addr );
int     CALLCONV UniQueryLocaleItem ( const LocaleObject locale_object,
                         LocaleItem item,
                         UniChar **info_item_addr_ptr );
size_t       CALLCONV UniStrftime ( const LocaleObject locale_object,
                       UniChar *ucs, size_t maxsize,
                       const UniChar *format, const struct tm *time_ptr );
UniChar    * CALLCONV UniStrptime ( const LocaleObject locale_object,
                       const UniChar *buf, const UniChar *format,
                       struct tm *time_ptr );
int          CALLCONV UniStrfmon  ( const LocaleObject locale_object,
                       UniChar *ucs, size_t maxsize,
                       const UniChar *format, ... );
UniChar    * CALLCONV UniStrcat  ( UniChar *ucs1, const UniChar *ucs2 );
UniChar    * CALLCONV UniStrchr  ( const UniChar *ucs, UniChar uc );
int          CALLCONV UniStrcmp  ( const UniChar *ucs1, const UniChar *ucs2 );
UniChar    * CALLCONV UniStrcpy  ( UniChar *ucs1, const UniChar *ucs2 );
size_t       CALLCONV UniStrcspn ( const UniChar *ucs1, const UniChar *ucs2 );
size_t       CALLCONV UniStrlen  ( const UniChar *ucs1 );
UniChar    * CALLCONV UniStrncat ( UniChar *ucs1, const UniChar *ucs2, size_t n );
int          CALLCONV UniStrncmp ( const UniChar *ucs1, const UniChar *ucs2, size_t n );
UniChar    * CALLCONV UniStrncpy ( UniChar *ucs1, const UniChar *ucs2, size_t n );
UniChar    * CALLCONV UniStrpbrk ( const UniChar *ucs1, const UniChar *ucs2 );
UniChar    * CALLCONV UniStrrchr ( const UniChar *ucs, UniChar uc );
size_t       CALLCONV UniStrspn  ( const UniChar *ucs1, const UniChar *ucs2 );
UniChar    * CALLCONV UniStrstr  ( const UniChar *ucs1, const UniChar *ucs2 );
UniChar    * CALLCONV UniStrtok  ( UniChar *ucs1, const UniChar *ucs2 );
int   CALLCONV UniCreateAttrObject ( const LocaleObject locale_object,
                           const UniChar *attr_name,
                           AttrObject *attr_object_ptr );
int   CALLCONV UniQueryCharAttr    ( AttrObject attr_object, UniChar uc );
int   CALLCONV UniScanForAttr      ( AttrObject attr_object, const UniChar *ucs,
                           size_t num_elems, ulsBool inverse_op,
                           size_t *offset_ptr );
int   CALLCONV UniFreeAttrObject   ( AttrObject attr_object );
int   CALLCONV UniQueryAlnum       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryAlpha       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryBlank       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryCntrl       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryDigit       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryGraph       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryLower       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryPrint       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryPunct       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQuerySpace       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryUpper       ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniQueryXdigit      ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniCreateTransformObject ( const LocaleObject locale_object,
                                const UniChar *xtype,
                                XformObject *xform_object_ptr );
int   CALLCONV UniTransformStr          ( XformObject xform_object,
                                const UniChar *inp_buf, int *inp_size,
                                UniChar *out_buf, int *out_size );
int   CALLCONV UniFreeTransformObject   ( XformObject xform_object );
UniChar   CALLCONV UniTransLower        ( const LocaleObject locale_object, UniChar uc );
UniChar   CALLCONV UniTransUpper        ( const LocaleObject locale_object, UniChar uc );
int   CALLCONV UniStrtod  ( const LocaleObject locale_object, const UniChar *ucs,
                  UniChar **end_ptr, double *result_ptr );
int   CALLCONV UniStrtol  ( const LocaleObject locale_object, const UniChar *ucs,
                  UniChar **end_ptr, int base, long int *result_ptr );
int   CALLCONV UniStrtoul ( const LocaleObject locale_object, const UniChar *ucs,
                  UniChar **end_ptr, int base, unsigned long int *result_ptr );
int      CALLCONV UniStrcoll  ( const LocaleObject locale_object,
                     const UniChar *ucs1, const UniChar *ucs2 );
size_t   CALLCONV UniStrxfrm  ( const LocaleObject locale_object,
                     UniChar *ucs1, const UniChar *ucs2, size_t n );
int      CALLCONV UniStrcmpi  ( const LocaleObject locale_object,
                     const UniChar *ucs1, const UniChar *ucs2 );
int      CALLCONV UniStrncmpi ( const LocaleObject locale_object,
                     const UniChar *ucs1, const UniChar *ucs2, const size_t n );
UniChar   CALLCONV UniToupper (UniChar uc);
UniChar   CALLCONV UniTolower (UniChar uc);
UniChar * CALLCONV UniStrupr  (UniChar *ucs);
UniChar * CALLCONV UniStrlwr  (UniChar *ucs);
int CALLCONV UniMapCtryToLocale(unsigned long Country,
                                UniChar *LocaleName,
                                size_t n);
#ifndef _ULS_UCONVP
#define _ULS_UCONVP
typedef void * UconvObject;
#endif
#ifndef _ULS_UNICHAR_DEFINED
#define _ULS_UNICHAR_DEFINED
typedef unsigned  short  UniChar ;
#endif
#ifndef ULS_ATTR
#define ULS_ATTR
#define UCONV_OPTION_SUBSTITUTE_FROM_UNICODE  1
#define UCONV_OPTION_SUBSTITUTE_TO_UNICODE    2
#define UCONV_OPTION_SUBSTITUTE_BOTH          3
#define CVTTYPE_PATH     0x00000004   /* Treat string as a path   */
#define CVTTYPE_CDRA     0x00000002   /* Use CDRA control mapping */
#define CVTTYPE_CTRL7F   0x00000001   /* Treat 0x7F as a control  */
#define DSPMASK_DATA    0xffffffff
#define DSPMASK_DISPLAY 0x00000000
#define DSPMASK_TAB     0x00000200
#define DSPMASK_LF      0x00000400
#define DSPMASK_CR      0x00002000
#define DSPMASK_CRLF    0x00002400
enum uconv_esid {
    ESID_sbcs_data        = 0x2100,
    ESID_sbcs_pc          = 0x3100,
    ESID_sbcs_ebcdic      = 0x1100,
    ESID_sbcs_iso         = 0x4100,
    ESID_sbcs_windows     = 0x4105,
    ESID_sbcs_alt         = 0xF100,
    ESID_dbcs_data        = 0x2200,
    ESID_dbcs_pc          = 0x3200,
    ESID_dbcs_ebcdic      = 0x1200,
    ESID_mbcs_data        = 0x2300,
    ESID_mbcs_pc          = 0x3300,
    ESID_mbcs_ebcdic      = 0x1301,
    ESID_ucs_2            = 0x7200,
    ESID_ugl              = 0x72FF,
    ESID_utf_8            = 0x7807,
    ESID_upf_8            = 0x78FF 
};
#ifndef __conv_endian_t
#define __conv_endian_t
#define     ENDIAN_SYSTEM   0x0000
#define     ENDIAN_BIG      0xfeff
#define     ENDIAN_LITTLE   0xfffe
typedef struct _conv_endian_rec {
   unsigned short  source;
   unsigned short  target;
} conv_endian_t;
#endif
typedef struct _uconv_attribute_t {
   unsigned long  version;
   char           mb_min_len;
   char           mb_max_len;
   char           usc_min_len;
   char           usc_max_len;
   unsigned short esid;
   char           options;
   char           state;
   conv_endian_t  endian;
   unsigned long  displaymask;
   unsigned long  converttype;
   unsigned short subchar_len;
   unsigned short subuni_len;
   char           subchar[16];
   UniChar        subuni[8];
} uconv_attribute_t;
typedef struct {
    unsigned short   first;
    unsigned short   last;
} udcrange_t;
#endif
#define uconv_error_t int
int CALLCONV UniCreateUconvObject(
          UniChar     * code_set,
          UconvObject * uobj  );
int CALLCONV UniQueryUconvObject(
          UconvObject         uobj,
          uconv_attribute_t * attr,
          size_t              size,
          char          first[256],
          char          other[256],
          udcrange_t udcrange[32]);
int CALLCONV UniSetUconvObject(
          UconvObject         uobj,
          uconv_attribute_t * attr);
int CALLCONV UniUconvToUcs(
          UconvObject uobj,
          void    * * inbuf,
          size_t    * inbytes,
          UniChar * * outbuf,
          size_t    * outchars,
          size_t    * subst  );
int CALLCONV UniUconvFromUcs(
          UconvObject uobj,
          UniChar * * inbuf,
          size_t    * inchars,
          void    * * outbuf,
          size_t    * outbytes,
          size_t    * subst   );
int CALLCONV UniFreeUconvObject(
          UconvObject   uobj   );
int CALLCONV UniMapCpToUcsCp( unsigned long ulCodePage, UniChar *ucsCodePage, size_t n );
#define UCONV_EOTHER           ULS_OTHER
#define UCONV_EILSEQ           ULS_ILLEGALSEQUENCE
#define UCONV_EINVAL           ULS_INVALID
#define UCONV_E2BIG            ULS_BUFFERFULL
#define UCONV_ENOMEM           ULS_NOMEMORY
#define UCONV_EBADF            ULS_BADOBJECT
#define UCONV_BADATTR          ULS_BADATTR
#define UCONV_EMFILE           ULS_MAXFILESPERPROC
#define UCONV_ENFILE           ULS_MAXFILES
#define UCONV_NOTIMPLEMENTED   ULS_NOTIMPLEMENTED
#define IBM_437         (UniChar *)L"IBM-437"
#define IBM_819         (UniChar *)L"IBM-819"
#define IBM_850         (UniChar *)L"IBM-850"
#define UTF_8           (UniChar *)L"IBM-1208"
#define UCS_2           (UniChar *)L"IBM-1200"
#define ISO8859_1       (UniChar *)L"IBM-819"
#ifdef __cplusplus
}
#endif
#endif
