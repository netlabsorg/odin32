/*
 * OS/2 Unicode API definitions
 *
 * Copyright 1999 Achim Hasenmueller
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef _ULS_UCONV
#define _ULS_UCONV

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>             /* define size_t and wchar_t types */
#include <time.h>               /* define tm struct */

#define ULS_API_ERROR_BASE                      0x00020400
#define ULS_API_ERROR(x)                        (ULS_API_ERROR_BASE | (x))

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

#define D_T_FMT    1  /* string for formatting date and time */
#define D_FMT      2  /* string for formatting date */
#define T_FMT      3  /* string for formatting time */
#define AM_STR     4  /* string for a.m. */
#define PM_STR     5  /* string for p.m. */

#define ABDAY_1    6  /* abbreviated first day of the week (Sun) */
#define ABDAY_2    7  /* abbreviated second day of the week (Mon) */
#define ABDAY_3    8  /* abbreviated third day of the week (Tue) */
#define ABDAY_4    9  /* abbreviated fourth day of the week (Wed) */
#define ABDAY_5   10  /* abbreviated fifth day of the week (Thu) */
#define ABDAY_6   11  /* abbreviated sixth day of the week (Fri) */
#define ABDAY_7   12  /* abbreviated seventh day of the week (Sat) */

#define DAY_1     13  /* name of the first day of the week (Sunday) */
#define DAY_2     14  /* name of the second day of the week (Monday) */
#define DAY_3     15  /* name of the third day of the week (Tuesday) */
#define DAY_4     16  /* name of the fourth day of the week (Wednesday) */
#define DAY_5     17  /* name of the fifth day of the week (Thursday) */
#define DAY_6     18  /* name of the sixth day of the week (Friday) */
#define DAY_7     19  /* name of the seventh day of the week (Saturday) */

#define ABMON_1   20  /* abbreviated first month (Jan) */
#define ABMON_2   21  /* abbreviated second month (Feb) */
#define ABMON_3   22  /* abbreviated third month (Mar) */
#define ABMON_4   23  /* abbreviated fourth month (Apr) */
#define ABMON_5   24  /* abbreviated fifth month (May) */
#define ABMON_6   25  /* abbreviated sixth month (Jun) */
#define ABMON_7   26  /* abbreviated seventh month (Jul) */
#define ABMON_8   27  /* abbreviated eighth month (Aug) */
#define ABMON_9   28  /* abbreviated ninth month (Sep) */
#define ABMON_10  29  /* abbreviated tenth month (Oct) */
#define ABMON_11  30  /* abbreviated eleventh month (Nov) */
#define ABMON_12  31  /* abbreviated twelveth month (Dec) */

#define MON_1     32  /* name of the first month (January) */
#define MON_2     33  /* name of the second month (February) */
#define MON_3     34  /* name of the third month (March) */
#define MON_4     35  /* name of the fourth month (April) */
#define MON_5     36  /* name of the fifth month (May) */
#define MON_6     37  /* name of the sixth month (June) */
#define MON_7     38  /* name of the seventh month (July) */
#define MON_8     39  /* name of the eighth month (August) */
#define MON_9     40  /* name of the ninth month (September) */
#define MON_10    41  /* name of the tenth month (October) */
#define MON_11    42  /* name of the eleventh month (November) */
#define MON_12    43  /* name of the twelveth month (December) */

#define RADIXCHAR 44  /* radix character */
#define THOUSEP   45  /* separator for thousands */
#define YESSTR    46  /* affiramitive response for yes/no queries */
#define NOSTR     47  /* negative response for yes/no queries */
#define CRNCYSTR  48  /* currency symbol; - leading, + trailing */
#define CODESET 49   /* codeset name */

/* Additional constants defined in XPG4 */

#define T_FMT_AMPM    55  /* am/pm time formating string */
#define ERA           56  /* era description segments */
#define ERA_D_FMT     57  /* era date format string */
#define ERA_D_T_FMT   58  /* era date and time format string */
#define ERA_T_FMT     59  /* era time format string */
#define ALT_DIGITS    60  /* alternative symbols for digits */
#define YESEXPR       61  /* affirmative response expression */
#define NOEXPR        62  /* negative response expression */

/* LSA feature */
#define DATESEP       63
#define TIMESEP       64
#define LISTSEP       65

/*
*  nl_items to support implementation of NLgetenv
*/
#ifdef _AIX31
#define NLLDATE    50
#define NLTMISC    51
#define NLTSTRS    52
#define NLTUNITS   53
#define NLYEAR     54
#endif

/*
 ** if this number changes, it MUST be changed
 ** in sys/locdata.h
 */
#ifndef _NL_NUM_ITEMS
#define _NL_NUM_ITEMS 66
#endif


#if defined(__IBMCPP__) || defined(__IBMC__)
   #define CALLCONV    _System
#else
   #define CALLCONV
#endif


/*
 * ulsBool Type
 */
typedef int ulsBool;


/* LSA added TRUE/FALSE */
#ifndef TRUE
   #define TRUE      1
#endif
#ifndef FALSE
   #define FALSE     0
#endif

/*
 * UniChar Type
 */
#ifndef _ULS_UNICHAR_DEFINED
    typedef unsigned  short  UniChar ;
    #define _ULS_UNICHAR_DEFINED
#endif

/*
 * LocaleObject Type
 */
typedef void  *LocaleObject;

/*
 * LocaleToken Type
 */
typedef unsigned int LocaleToken;

/*
 * AttrObject Type
 */
typedef void *AttrObject;

/*
 * XformObject type
 */
typedef void *XformObject;

/*
 * LocaleSpecType Values
 */
#define UNI_TOKEN_POINTER       1
#define UNI_MBS_STRING_POINTER  2
#define UNI_UCS_STRING_POINTER  3


/*
 * Categories
 */
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

/*
 * UniLconv locale conventions structure
 */
struct UniLconv {
   UniChar *decimal_point;      /* non-monetary decimal point           */
   UniChar *thousands_sep;      /* non-monetary thousands separator     */
   short   *grouping;           /* non-monetary size of grouping        */
   UniChar *int_curr_symbol;    /* int'l currency symbol and separator  */
   UniChar *currency_symbol;    /* local  currency symbol               */
   UniChar *mon_decimal_point;  /* monetary decimal point               */
   UniChar *mon_thousands_sep;  /* monetary thousands separator         */
   short   *mon_grouping;       /* monetary size of grouping            */
   UniChar *positive_sign;      /* non-negative values sign             */
   UniChar *negative_sign;      /* negative values sign                 */
   short   int_frac_digits;     /* no of fractional digits int currency */
   short   frac_digits;         /* no of fractional digits loc currency */
   short   p_cs_precedes;       /* nonneg curr sym 1-precedes,0-succeeds*/
   short   p_sep_by_space;      /* nonneg curr sym 1-space,0-no space   */
   short   n_cs_precedes;       /* neg curr sym 1-precedes,0-succeeds   */
   short   n_sep_by_space;      /* neg curr sym 1-space 0-no space      */
   short   p_sign_posn;         /* positioning of nonneg monetary sign  */
   short   n_sign_posn;         /* positioning of negative monetary sign*/
   short   os2_mondecpt;        /* os2 curr sym positioning             */
   UniChar *debit_sign;         /* non-neg-valued monetary sym - "DB"   */
   UniChar *credit_sign;        /* negative-valued monetary sym - "CR"  */
   UniChar *left_parenthesis;   /* negative-valued monetary sym - "("   */
   UniChar *right_parenthesis;  /* negative-valued monetary sym - ")"   */
};

/*
 * Locale Management Function Prototypes
 */
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

/*
 * Locale Information Function Prototypes
 */
int     CALLCONV UniQueryLocaleInfo ( const LocaleObject locale_object,
                         struct UniLconv **unilconv_addr_ptr );
int     CALLCONV UniFreeLocaleInfo  ( struct UniLconv *unilconv_addr );
int     CALLCONV UniQueryLocaleItem ( const LocaleObject locale_object,
                         LocaleItem item,
                         UniChar **info_item_addr_ptr );

/*
 * Date and Time Function Prototypes
 */
size_t       CALLCONV UniStrftime ( const LocaleObject locale_object,
                       UniChar *ucs, size_t maxsize,
                       const UniChar *format, const struct tm *time_ptr );
UniChar    * CALLCONV UniStrptime ( const LocaleObject locale_object,
                       const UniChar *buf, const UniChar *format,
                       struct tm *time_ptr );

/*
 * Monetary Formatting Function Prototype
 */
int          CALLCONV UniStrfmon  ( const LocaleObject locale_object,
                       UniChar *ucs, size_t maxsize,
                       const UniChar *format, ... );

/*
 * String/Character Function Prototypes
 */
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


/*
 * Character Attribute Function Prototypes
 */
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

/*
 * String Transformation Function Prototypes
 */
int   CALLCONV UniCreateTransformObject ( const LocaleObject locale_object,
                                const UniChar *xtype,
                                XformObject *xform_object_ptr );
int   CALLCONV UniTransformStr          ( XformObject xform_object,
                                const UniChar *inp_buf, int *inp_size,
                                UniChar *out_buf, int *out_size );
int   CALLCONV UniFreeTransformObject   ( XformObject xform_object );
UniChar   CALLCONV UniTransLower        ( const LocaleObject locale_object, UniChar uc );
UniChar   CALLCONV UniTransUpper        ( const LocaleObject locale_object, UniChar uc );

/*
 * String Conversion Function Prototypes
 */
int   CALLCONV UniStrtod  ( const LocaleObject locale_object, const UniChar *ucs,
                  UniChar **end_ptr, double *result_ptr );
int   CALLCONV UniStrtol  ( const LocaleObject locale_object, const UniChar *ucs,
                  UniChar **end_ptr, int base, long int *result_ptr );
int   CALLCONV UniStrtoul ( const LocaleObject locale_object, const UniChar *ucs,
                  UniChar **end_ptr, int base, unsigned long int *result_ptr );

/*
 * String Comparison Function Prototypes
 */
int      CALLCONV UniStrcoll  ( const LocaleObject locale_object,
                     const UniChar *ucs1, const UniChar *ucs2 );
size_t   CALLCONV UniStrxfrm  ( const LocaleObject locale_object,
                     UniChar *ucs1, const UniChar *ucs2, size_t n );
int      CALLCONV UniStrcmpi  ( const LocaleObject locale_object,
                     const UniChar *ucs1, const UniChar *ucs2 );
int      CALLCONV UniStrncmpi ( const LocaleObject locale_object,
                     const UniChar *ucs1, const UniChar *ucs2, const size_t n );

/*
 * Unicode Case Mapping Function Prototypes
 */
UniChar   CALLCONV UniToupper ( UniChar uc );
UniChar   CALLCONV UniTolower ( UniChar uc );
UniChar * CALLCONV UniStrupr  ( UniChar * ucs );
UniChar * CALLCONV UniStrlwr  ( UniChar * ucs );


int CALLCONV UniMapCtryToLocale( unsigned long Country,
                                 UniChar *LocaleName,
                                 size_t n
                               );

#ifndef _ULS_UCONVP
#define _ULS_UCONVP
typedef void * UconvObject;          /* uconv Type pointer            */
#endif

#ifndef _ULS_UNICHAR_DEFINED
#define _ULS_UNICHAR_DEFINED
typedef unsigned  short  UniChar ;
#endif


#ifndef ULS_ATTR
#define ULS_ATTR
/*
 * Substitution options
 */
#define UCONV_OPTION_SUBSTITUTE_FROM_UNICODE  1
#define UCONV_OPTION_SUBSTITUTE_TO_UNICODE    2
#define UCONV_OPTION_SUBSTITUTE_BOTH          3

/*
 * Conversion options
 */
#define CVTTYPE_PATH     0x00000004   /* Treat string as a path   */
#define CVTTYPE_CDRA     0x00000002   /* Use CDRA control mapping */
#define CVTTYPE_CTRL7F   0x00000001   /* Treat 0x7F as a control  */

/*
 * Conversion mask.  This is a bit mask.  If the bit is on, the
 * corresponding character 0x00-0x1f is treated as a control.
 * Otherwise it is treated as a glyph.
 */
#define DSPMASK_DATA    0xffffffff
#define DSPMASK_DISPLAY 0x00000000
#define DSPMASK_TAB     0x00000200
#define DSPMASK_LF      0x00000400
#define DSPMASK_CR      0x00002000
#define DSPMASK_CRLF    0x00002400

/*
 * Encoding schemes.  This is the primary item used to check if a
 * a codepage is valid for a particular purpose.  A length check
 * may also be made in some cases.
 */
enum uconv_esid {                     /* Process Display  VIO    GPI   */
    ESID_sbcs_data        = 0x2100,   /*    x      x      x       x    */
    ESID_sbcs_pc          = 0x3100,   /*    x      x      x       x    */
    ESID_sbcs_ebcdic      = 0x1100,   /*           x      x       x    */
    ESID_sbcs_iso         = 0x4100,   /*    x      x      x       x    */
    ESID_sbcs_windows     = 0x4105,   /*    x      x      x       x    */
    ESID_sbcs_alt         = 0xF100,   /*           x      x       x    */
    ESID_dbcs_data        = 0x2200,   /*           x              x    */
    ESID_dbcs_pc          = 0x3200,   /*    x      x      x       x    */
    ESID_dbcs_ebcdic      = 0x1200,   /*                          x    */
    ESID_mbcs_data        = 0x2300,   /*           x      x       x    */
    ESID_mbcs_pc          = 0x3300,   /*           x              x    */
    ESID_mbcs_ebcdic      = 0x1301,   /*                               */
    ESID_ucs_2            = 0x7200,   /*                               */
    ESID_ugl              = 0x72FF,   /*                               */
    ESID_utf_8            = 0x7807,   /*           x      x       x    */
    ESID_upf_8            = 0x78FF    /*    x      x      x       x    */
};

#ifndef __conv_endian_t
#define __conv_endian_t
#define     ENDIAN_SYSTEM   0x0000
#define     ENDIAN_BIG      0xfeff
#define     ENDIAN_LITTLE   0xfffe
typedef struct _conv_endian_rec {
   unsigned short  source;   /* Used by FromUcs                   */
   unsigned short  target;   /* Used by ToUcs                     */
} conv_endian_t;
#endif

typedef struct _uconv_attribute_t {
   unsigned long  version;       /* Q/S Version (must be zero)        */
   char           mb_min_len;    /* Q   Minimum char size             */
   char           mb_max_len;    /* Q   Maximum char size             */
   char           usc_min_len;   /* Q   UCS min size                  */
   char           usc_max_len;   /* Q   UCS max size                  */
   unsigned short esid;          /* Q   Encoding scheme ID            */
   char           options;       /* Q/S Substitution options          */
   char           state;         /* Q/S State for stateful convert    */
   conv_endian_t  endian;        /* Q/S Source and target endian      */
   unsigned long  displaymask;   /* Q/S Display/data mask             */
   unsigned long  converttype;   /* Q/S Conversion type               */
   unsigned short subchar_len;   /* Q/S MBCS sub len      0=table     */
   unsigned short subuni_len;    /* Q/S Unicode sub len   0=table     */
   char           subchar[16];   /* Q/S MBCS sub characters           */
   UniChar        subuni[8];     /* Q/S Unicode sub characters        */
} uconv_attribute_t;

/*
 * User defined character range
 */
typedef struct {                /* User Defined character range      */
    unsigned short   first;     /* First codepoint                   */
    unsigned short   last;      /* Last codepoint                    */
} udcrange_t;
#endif  /* ULS_ATTR */
#define uconv_error_t int

/*
 * UniCreateUconvObject: Create Unicode Conversion Object
 *
 * return code:
 *    0             - Conversion Object sucessfully initialized
 *    UCONV_EMFILE  - Maximum file descriptors are opened
 *    UCONV_ENFILE  - Too many files currently open in the system
 *    UCONV_ENOMEM  - Insuffcient memory
 *    UCONV_EINVAL  - The conversion specified by code_set or the
 *                    modifier supplied is not recognized by the
 *                    implementation.
 *
 */
int CALLCONV UniCreateUconvObject(
          UniChar     * code_set,   /* I  - Unicode name of uconv table */
          UconvObject * uobj  );    /* O  - Uconv object handle         */
/*
 * UniQueryUconvObject: Query Unicode Coversion Object
 *
 * return code:
 *    0             - Operation successful
 *    UCONV_EBADF   - The conversion object specified is not
 *                    recognized by the implementation.
 */
int CALLCONV UniQueryUconvObject(
          UconvObject         uobj, /* I  - Uconv object handle         */
          uconv_attribute_t * attr, /* O  - Uconv attributes            */
          size_t              size, /* I  - Size of attribute structure */
          char          first[256], /* O  - First byte of multibyte     */
          char          other[256], /* O  - Other byte of multibyte     */
          udcrange_t udcrange[32]); /* O  - User defined char range     */
/*
 * UniSetUconvObject:  Set Unicode Conversion Object
 *
 * return code:
 *    0             - Operation successful
 *    UCONV_EBADF   - The conversion object specified is not
 *                    recognized by the implementation.
 *    UCONV_BADATTR - attribute supplied contains invalid attribute
 *                    for the conversion object
 */
int CALLCONV UniSetUconvObject(
          UconvObject         uobj, /* I  - Uconv object handle         */
          uconv_attribute_t * attr); /* I  - Uconv attributes            */
/*
 * UniUconvToUcs:  Convert string to unicode
 *
 * return code:
 *    0             - Operation successful
 *    UCONV_EBADF   - The conversion object specified is not
 *                    recognized by the implementation.
 *    UCONV_E2BIG   - Input conversion stopped due to lack of space
 *                    in the output buffer
 *    UCONV_EINVAL  - Input conversion stopped due to incomplete
 *                    character or shift sequence at the end of the
 *                    input buffer.
 *    UCONV_EILSEQ  - Input conversion stopped due to an input byte
 *                    that does not belong to the input code set.
 */
int CALLCONV UniUconvToUcs(
          UconvObject uobj,         /* I  - Uconv object handle         */
          void    * * inbuf,        /* IO - Input buffer                */
          size_t    * inbytes,      /* IO - Input buffer size (bytes)   */
          UniChar * * outbuf,       /* IO - Output buffer size          */
          size_t    * outchars,     /* IO - Output size (chars)         */
          size_t    * subst  );     /* IO - Substitution count          */
/*
 * UniUconvFromUcs:  Convert string from unicode
 *    0             - Operation successful
 *    UCONV_EBADF   - The conversion object specified is not
 *                    recognized by the implementation.
 *    UCONV_E2BIG   - Input conversion stopped due to lack of space
 *                    in the output buffer
 *    UCONV_EINVAL  - Input conversion stopped due to incomplete
 *                    character or shift sequence at the end of the
 *                    input buffer.
 *    UCONV_EILSEQ  - Input conversion stopped due to an input byte
 *                    that does not belong to the input code set.
 */
int CALLCONV UniUconvFromUcs(
          UconvObject uobj,         /* I  - Uconv object handle         */
          UniChar * * inbuf,        /* IO - Input buffer                */
          size_t    * inchars,      /* IO - Input buffer size (bytes)   */
          void    * * outbuf,       /* IO - Output buffer size          */
          size_t    * outbytes,     /* IO - Output size (chars)         */
          size_t    * subst   );    /* IO - Substitution count          */
/*
 * UniFreeUconvObject: Close Unicode Conversion Object
 *    0             - Operation successful
 *    UCONV_EBADF   - The conversion object specified is not
 *                    recognized by the implementation.
 */
int CALLCONV UniFreeUconvObject(
          UconvObject   uobj   );   /* I  - Uconv object handle         */
/*
 * Function: UniMapCpToUcsCp
 *
 * description:
 * given a code page represented as an unsigned long, gives back the
 * corresponding Unicode string representing the code page, suitable
 * for input to UniCreateUconvObject().
 * A buffer for ucsCodePage must be given as input. The ucsCodePage
 * will contain a codepage string (Unicode string) upon successful exit.
 * The size of the ucsCodePage buffer must be given as input.
 *
 * Returns 0 upon success. If there is an error, the contents
 * of ucsCodePage are undefined.
 * If an error occures, the contents of ucsCodePage are undefined and
 * one of the following error codes is returned:
 * UCONV_E2BIG - The buffer is not large enough to hold the codepage name
 * Also see errors from UniUcsidToTag and UniUcsidFromTag
 */
int CALLCONV UniMapCpToUcsCp( unsigned long ulCodePage, UniChar *ucsCodePage, size_t n );

/*
 * for compatibility with old return codes for ULS
 */
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
