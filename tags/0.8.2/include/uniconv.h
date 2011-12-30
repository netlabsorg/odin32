/* $Id: uniconv.h,v 1.8 2001-05-19 08:23:41 sandervl Exp $ */
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
#define IFIRSTDAYOFWEEK   81
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

#ifdef __cplusplus

//Wrappers for unicode api
#include <os2sel.h>

inline int _UniCreateLocaleObject(int a, const void *b, LocaleObject *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniCreateLocaleObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniCreateLocaleObject
#define UniCreateLocaleObject _UniCreateLocaleObject

inline int _UniQueryLocaleObject(const LocaleObject a, int b, int c, void **d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryLocaleObject(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryLocaleObject
#define UniQueryLocaleObject _UniQueryLocaleObject

inline int _UniFreeLocaleObject(LocaleObject a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniFreeLocaleObject(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniFreeLocaleObject
#define UniFreeLocaleObject _UniFreeLocaleObject

inline int _UniFreeMem(void *a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniFreeMem(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniFreeMem
#define UniFreeMem _UniFreeMem

inline int _UniLocaleStrToToken(int a, const void *b, LocaleToken *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniLocaleStrToToken(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniLocaleStrToToken
#define UniLocaleStrToToken _UniLocaleStrToToken

inline int _UniLocaleTokenToStr(const LocaleToken a, int b, void **c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniLocaleTokenToStr(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniLocaleTokenToStr
#define UniLocaleTokenToStr _UniLocaleTokenToStr

inline int _UniQueryLocaleInfo(const LocaleObject a, UniLconv **b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryLocaleInfo(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryLocaleInfo
#define UniQueryLocaleInfo _UniQueryLocaleInfo

inline int _UniFreeLocaleInfo(struct UniLconv *a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniFreeLocaleInfo(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniFreeLocaleInfo
#define UniFreeLocaleInfo _UniFreeLocaleInfo

inline int _UniQueryLocaleItem(const LocaleObject a, LocaleItem b, UniChar **c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryLocaleItem(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryLocaleItem
#define UniQueryLocaleItem _UniQueryLocaleItem

inline size_t _UniStrftime(const LocaleObject a, UniChar *b, size_t c, const UniChar *d, const struct tm *e)
{
 size_t yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrftime(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrftime
#define UniStrftime _UniStrftime

inline UniChar *_UniStrptime(const LocaleObject a, const UniChar *b, const UniChar *c, struct tm *d)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrptime(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrptime
#define UniStrptime _UniStrptime

inline UniChar *_UniStrcat(UniChar *a, const UniChar *b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrcat(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrcat
#define UniStrcat _UniStrcat

inline UniChar *_UniStrchr(const UniChar *a, UniChar b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrchr(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrchr
#define UniStrchr _UniStrchr

inline int _UniStrcmp(const UniChar *a, const UniChar *b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrcmp(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrcmp
#define UniStrcmp _UniStrcmp

inline UniChar *_UniStrcpy(UniChar *a, const UniChar *b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrcpy(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrcpy
#define UniStrcpy _UniStrcpy

inline size_t _UniStrcspn(const UniChar *a, const UniChar *b)
{
 size_t yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrcspn(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrcspn
#define UniStrcspn _UniStrcspn

inline size_t _UniStrlen(const UniChar *a)
{
 size_t yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrlen(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrlen
#define UniStrlen _UniStrlen

inline UniChar *_UniStrncat(UniChar *a, const UniChar *b, size_t c)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrncat(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrncat
#define UniStrncat _UniStrncat

inline int _UniStrncmp(const UniChar *a, const UniChar *b, size_t c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrncmp(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrncmp
#define UniStrncmp _UniStrncmp

inline UniChar *_UniStrncpy(UniChar *a, const UniChar *b, size_t c)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrncpy(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrncpy
#define UniStrncpy _UniStrncpy

inline UniChar *_UniStrpbrk(const UniChar *a, const UniChar *b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrpbrk(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrpbrk
#define UniStrpbrk _UniStrpbrk

inline UniChar *_UniStrrchr(const UniChar *a, UniChar b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrrchr(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrrchr
#define UniStrrchr _UniStrrchr

inline size_t _UniStrspn(const UniChar *a, const UniChar *b)
{
 size_t yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrspn(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrspn
#define UniStrspn _UniStrspn

inline UniChar *_UniStrstr(const UniChar *a, const UniChar *b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrstr(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrstr
#define UniStrstr _UniStrstr

inline UniChar *_UniStrtok(UniChar *a, const UniChar *b)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrtok(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrtok
#define UniStrtok _UniStrtok

inline int _UniCreateAttrObject(const LocaleObject a, const UniChar *b, AttrObject *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniCreateAttrObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniCreateAttrObject
#define UniCreateAttrObject _UniCreateAttrObject

inline int _UniQueryCharAttr(AttrObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryCharAttr(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryCharAttr
#define UniQueryCharAttr _UniQueryCharAttr

inline int _UniScanForAttr(AttrObject a, const UniChar *b, size_t c, ulsBool d, size_t *e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniScanForAttr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  UniScanForAttr
#define UniScanForAttr _UniScanForAttr

inline int _UniFreeAttrObject(AttrObject a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniFreeAttrObject(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniFreeAttrObject
#define UniFreeAttrObject _UniFreeAttrObject

inline int _UniQueryAlnum(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryAlnum(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryAlnum
#define UniQueryAlnum _UniQueryAlnum

inline int _UniQueryAlpha(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryAlpha(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryAlpha
#define UniQueryAlpha _UniQueryAlpha

inline int _UniQueryBlank(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryBlank(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryBlank
#define UniQueryBlank _UniQueryBlank

inline int _UniQueryCntrl(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryCntrl(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryCntrl
#define UniQueryCntrl _UniQueryCntrl

inline int _UniQueryDigit(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryDigit(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryDigit
#define UniQueryDigit _UniQueryDigit

inline int _UniQueryGraph(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryGraph(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryGraph
#define UniQueryGraph _UniQueryGraph

inline int _UniQueryLower(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryLower(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryLower
#define UniQueryLower _UniQueryLower

inline int _UniQueryPrint(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryPrint(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryPrint
#define UniQueryPrint _UniQueryPrint

inline int _UniQueryPunct(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryPunct(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryPunct
#define UniQueryPunct _UniQueryPunct

inline int _UniQuerySpace(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQuerySpace(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQuerySpace
#define UniQuerySpace _UniQuerySpace

inline int _UniQueryUpper(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryUpper(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryUpper
#define UniQueryUpper _UniQueryUpper

inline int _UniQueryXdigit(const LocaleObject a, UniChar b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryXdigit(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryXdigit
#define UniQueryXdigit _UniQueryXdigit

inline int _UniCreateTransformObject(const LocaleObject a, const UniChar *b, XformObject *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniCreateTransformObject(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniCreateTransformObject
#define UniCreateTransformObject _UniCreateTransformObject

inline int _UniTransformStr(XformObject a, const UniChar *b, int *c, UniChar *d, int *e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniTransformStr(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  UniTransformStr
#define UniTransformStr _UniTransformStr

inline int _UniFreeTransformObject(XformObject a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniFreeTransformObject(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniFreeTransformObject
#define UniFreeTransformObject _UniFreeTransformObject

inline UniChar _UniTransLower(const LocaleObject a, UniChar b)
{
 UniChar yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniTransLower(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniTransLower
#define UniTransLower _UniTransLower

inline UniChar _UniTransUpper(const LocaleObject a, UniChar b)
{
 UniChar yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniTransUpper(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniTransUpper
#define UniTransUpper _UniTransUpper

inline int _UniStrtod(const LocaleObject a, const UniChar *b, UniChar **c, double *d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrtod(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrtod
#define UniStrtod _UniStrtod

inline int _UniStrtol(const LocaleObject a, const UniChar *b, UniChar **c, int d, long *e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrtol(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrtol
#define UniStrtol _UniStrtol

inline int _UniStrtoul(const LocaleObject a, const UniChar *b, UniChar **c, int d, unsigned long *e)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrtoul(a, b, c, d, e);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrtoul
#define UniStrtoul _UniStrtoul

inline int _UniStrcoll(const LocaleObject a, const UniChar *b, const UniChar *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrcoll(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrcoll
#define UniStrcoll _UniStrcoll

inline size_t _UniStrxfrm(const LocaleObject a, UniChar *b, const UniChar *c, size_t d)
{
 size_t yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrxfrm(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrxfrm
#define UniStrxfrm _UniStrxfrm

inline int _UniStrcmpi(const LocaleObject a, const UniChar *b, const UniChar *c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrcmpi(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrcmpi
#define UniStrcmpi _UniStrcmpi

inline int _UniStrncmpi(const LocaleObject a, const UniChar *b, const UniChar *c, const size_t d)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrncmpi(a, b, c, d);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrncmpi
#define UniStrncmpi _UniStrncmpi

inline UniChar _UniToupper(UniChar a)
{
 UniChar yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniToupper(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniToupper
#define UniToupper _UniToupper

inline UniChar _UniTolower(UniChar a)
{
 UniChar yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniTolower(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniTolower
#define UniTolower _UniTolower

inline UniChar *_UniStrupr(UniChar *a)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrupr(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrupr
#define UniStrupr _UniStrupr

inline UniChar *_UniStrlwr(UniChar *a)
{
 UniChar * yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniStrlwr(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniStrlwr
#define UniStrlwr _UniStrlwr

inline int _UniMapCtryToLocale(long a, UniChar *b, size_t c)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniMapCtryToLocale(a, b, c);
    SetFS(sel);

    return yyrc;
} 

#undef  UniMapCtryToLocale
#define UniMapCtryToLocale _UniMapCtryToLocale

inline int _UniCreateUconvObject(UniChar *a, UconvObject *b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniCreateUconvObject(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniCreateUconvObject
#define UniCreateUconvObject _UniCreateUconvObject

inline int _UniQueryUconvObject(UconvObject a, uconv_attribute_t *b, size_t c, char d[256], char e[256], udcrange_t f[32])
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniQueryUconvObject(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  UniQueryUconvObject
#define UniQueryUconvObject _UniQueryUconvObject

inline int _UniSetUconvObject(UconvObject a, uconv_attribute_t *b)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniSetUconvObject(a, b);
    SetFS(sel);

    return yyrc;
} 

#undef  UniSetUconvObject
#define UniSetUconvObject _UniSetUconvObject

inline int _UniUconvToUcs(UconvObject a, void **b, size_t *c, UniChar **d, size_t *e, size_t *f)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniUconvToUcs(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  UniUconvToUcs
#define UniUconvToUcs _UniUconvToUcs

inline int _UniUconvFromUcs(UconvObject a, UniChar **b, size_t *c, void **d, size_t *e, size_t *f)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniUconvFromUcs(a, b, c, d, e, f);
    SetFS(sel);

    return yyrc;
} 

#undef  UniUconvFromUcs
#define UniUconvFromUcs _UniUconvFromUcs

inline int _UniFreeUconvObject(UconvObject a)
{
 int yyrc;
 USHORT sel = RestoreOS2FS();

    yyrc = UniFreeUconvObject(a);
    SetFS(sel);

    return yyrc;
} 

#undef  UniFreeUconvObject
#define UniFreeUconvObject _UniFreeUconvObject
#endif //__cplusplus

#endif
