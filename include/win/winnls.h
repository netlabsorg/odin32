#ifndef __WINE_WINNLS_H
#define __WINE_WINNLS_H

#ifdef HAVE_WCTYPE_H

/* Sun Bug Workaround */
#ifdef __WINE__
# undef wsprintf
#endif
#include <wctype.h> /* needed for towupper */
#endif

#ifndef OS2_INCLUDED
#include <windef.h>
#include <winbase.h>
#endif

#define MB_PRECOMPOSED              0x00000001
#define MB_COMPOSITE                0x00000002
#define MB_USEGLYPHCHARS            0x00000004
#define MB_ERR_INVALID_CHARS        0x00000008

#define LCID_INSTALLED              0x00000001

/* flags to GetLocaleInfo */
#define  LOCALE_NOUSEROVERRIDE               0x80000000
#define  LOCALE_USE_CP_ACP                   0x40000000

#define LOCALE_LOCALEINFOFLAGSMASK  0xC0000000

/* When adding new defines, don't forget to add an entry to the
 * locale2id map in misc/ole2nls.c
 */
#define LOCALE_ILANGUAGE            0x00000001
#define LOCALE_SLANGUAGE            0x00000002
#define LOCALE_SENGLANGUAGE         0x00001001
#define LOCALE_SABBREVLANGNAME      0x00000003
#define LOCALE_SNATIVELANGNAME      0x00000004
#define LOCALE_ICOUNTRY             0x00000005
#define LOCALE_SCOUNTRY             0x00000006
#define LOCALE_SENGCOUNTRY          0x00001002
#define LOCALE_SABBREVCTRYNAME      0x00000007
#define LOCALE_SNATIVECTRYNAME      0x00000008
#define LOCALE_IDEFAULTLANGUAGE     0x00000009
#define LOCALE_IDEFAULTCOUNTRY      0x0000000A
#define LOCALE_IDEFAULTCODEPAGE     0x0000000B
#define LOCALE_IDEFAULTANSICODEPAGE 0x00001004
#define LOCALE_IDEFAULTMACCODEPAGE  0x00001011
#define LOCALE_SLIST                0x0000000C
#define LOCALE_IMEASURE             0x0000000D
#define LOCALE_SDECIMAL             0x0000000E
#define LOCALE_STHOUSAND            0x0000000F
#define LOCALE_SGROUPING            0x00000010
#define LOCALE_IDIGITS              0x00000011
#define LOCALE_ILZERO               0x00000012
#define LOCALE_INEGNUMBER           0x00001010
#define LOCALE_SNATIVEDIGITS        0x00000013
#define LOCALE_SCURRENCY            0x00000014
#define LOCALE_SINTLSYMBOL          0x00000015
#define LOCALE_SMONDECIMALSEP       0x00000016
#define LOCALE_SMONTHOUSANDSEP      0x00000017
#define LOCALE_SMONGROUPING         0x00000018
#define LOCALE_ICURRDIGITS          0x00000019
#define LOCALE_IINTLCURRDIGITS      0x0000001A
#define LOCALE_ICURRENCY            0x0000001B
#define LOCALE_INEGCURR             0x0000001C
#define LOCALE_SDATE                0x0000001D
#define LOCALE_STIME                0x0000001E
#define LOCALE_SSHORTDATE           0x0000001F
#define LOCALE_SLONGDATE            0x00000020
#define LOCALE_STIMEFORMAT          0x00001003
#define LOCALE_IDATE                0x00000021
#define LOCALE_ILDATE               0x00000022
#define LOCALE_ITIME                0x00000023
#define LOCALE_ITIMEMARKPOSN        0x00001005
#define LOCALE_ICENTURY             0x00000024
#define LOCALE_ITLZERO              0x00000025
#define LOCALE_IDAYLZERO            0x00000026
#define LOCALE_IMONLZERO            0x00000027
#define LOCALE_S1159                0x00000028
#define LOCALE_S2359                0x00000029
#define LOCALE_ICALENDARTYPE        0x00001009
#define LOCALE_IOPTIONALCALENDAR    0x0000100B
#define LOCALE_IFIRSTDAYOFWEEK      0x0000100C
#define LOCALE_IFIRSTWEEKOFYEAR     0x0000100D
#define LOCALE_SDAYNAME1            0x0000002A
#define LOCALE_SDAYNAME2            0x0000002B
#define LOCALE_SDAYNAME3            0x0000002C
#define LOCALE_SDAYNAME4            0x0000002D
#define LOCALE_SDAYNAME5            0x0000002E
#define LOCALE_SDAYNAME6            0x0000002F
#define LOCALE_SDAYNAME7            0x00000030
#define LOCALE_SABBREVDAYNAME1      0x00000031
#define LOCALE_SABBREVDAYNAME2      0x00000032
#define LOCALE_SABBREVDAYNAME3      0x00000033
#define LOCALE_SABBREVDAYNAME4      0x00000034
#define LOCALE_SABBREVDAYNAME5      0x00000035
#define LOCALE_SABBREVDAYNAME6      0x00000036
#define LOCALE_SABBREVDAYNAME7      0x00000037
#define LOCALE_SMONTHNAME1          0x00000038
#define LOCALE_SMONTHNAME2          0x00000039
#define LOCALE_SMONTHNAME3          0x0000003A
#define LOCALE_SMONTHNAME4          0x0000003B
#define LOCALE_SMONTHNAME5          0x0000003C
#define LOCALE_SMONTHNAME6          0x0000003D
#define LOCALE_SMONTHNAME7          0x0000003E
#define LOCALE_SMONTHNAME8          0x0000003F
#define LOCALE_SMONTHNAME9          0x00000040
#define LOCALE_SMONTHNAME10         0x00000041
#define LOCALE_SMONTHNAME11         0x00000042
#define LOCALE_SMONTHNAME12         0x00000043
#define LOCALE_SMONTHNAME13         0x0000100E
#define LOCALE_SABBREVMONTHNAME1    0x00000044
#define LOCALE_SABBREVMONTHNAME2    0x00000045
#define LOCALE_SABBREVMONTHNAME3    0x00000046
#define LOCALE_SABBREVMONTHNAME4    0x00000047
#define LOCALE_SABBREVMONTHNAME5    0x00000048
#define LOCALE_SABBREVMONTHNAME6    0x00000049
#define LOCALE_SABBREVMONTHNAME7    0x0000004A
#define LOCALE_SABBREVMONTHNAME8    0x0000004B
#define LOCALE_SABBREVMONTHNAME9    0x0000004C
#define LOCALE_SABBREVMONTHNAME10   0x0000004D
#define LOCALE_SABBREVMONTHNAME11   0x0000004E
#define LOCALE_SABBREVMONTHNAME12   0x0000004F
#define LOCALE_SABBREVMONTHNAME13   0x0000100F
#define LOCALE_SPOSITIVESIGN        0x00000050
#define LOCALE_SNEGATIVESIGN        0x00000051
#define LOCALE_IPOSSIGNPOSN         0x00000052
#define LOCALE_INEGSIGNPOSN         0x00000053
#define LOCALE_IPOSSYMPRECEDES      0x00000054
#define LOCALE_IPOSSEPBYSPACE       0x00000055
#define LOCALE_INEGSYMPRECEDES      0x00000056
#define LOCALE_INEGSEPBYSPACE       0x00000057
#define  LOCALE_FONTSIGNATURE        0x00000058
#define LOCALE_SISO639LANGNAME      0x00000059
#define LOCALE_SISO3166CTRYNAME     0x0000005A

#define LOCALE_IDEFAULTEBCDICCODEPAGE 0x00001012
#define LOCALE_IPAPERSIZE             0x0000100A
#define LOCALE_SENGCURRNAME           0x00001007
#define LOCALE_SNATIVECURRNAME        0x00001008
#define LOCALE_SYEARMONTH             0x00001006
#define LOCALE_SSORTNAME              0x00001013
#define LOCALE_IDIGITSUBSTITUTION     0x00001014

#ifdef __WIN32OS2__
#define LCID_INSTALLED            0x00000001  /* installed locale ids */
#define LCID_SUPPORTED            0x00000002  /* supported locale ids */
#endif

#define NORM_IGNORECASE                           1
#define NORM_IGNORENONSPACE                    2
#define NORM_IGNORESYMBOLS                     4
#define NORM_STRINGSORT                           0x1000
#define NORM_IGNOREKANATYPE                     0x00010000
#define NORM_IGNOREWIDTH                        0x00020000

#define CP_ACP                                  0
#define CP_OEMCP                                1
#define CP_MACCP				2
#define CP_THREAD_ACP				3
#define CP_SYMBOL				42
#define CP_UTF7					65000
#define CP_UTF8					65001

#define WC_DISCARDNS                0x00000010
#define WC_SEPCHARS                 0x00000020
#define WC_DEFAULTCHAR              0x00000040
#define WC_COMPOSITECHECK           0x00000200
#define WC_NO_BEST_FIT_CHARS        0x00000400

#define WC_DEFAULTCHECK                           0x00000100

#define MAKELCID(l, s)                      (MAKELONG(l, s))

#define LANGIDFROMLCID(lcid)	((WORD)(lcid))
#define SORTIDFROMLCID(lcid)	((WORD)((((DWORD)(lcid)) >> 16) & 0x0f))

#define MAKELANGID(p, s)                 ((((WORD)(s))<<10) | (WORD)(p))
#define PRIMARYLANGID(l)                 ((WORD)(l) & 0x3ff)
#define SUBLANGID(l)                        ((WORD)(l) >> 10)

#define LANG_SYSTEM_DEFAULT              (MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT))
#define LANG_USER_DEFAULT                (MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT))
#define LOCALE_SYSTEM_DEFAULT            (MAKELCID(LANG_SYSTEM_DEFAULT, SORT_DEFAULT))
#define LOCALE_USER_DEFAULT              (MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT))
#define LOCALE_NEUTRAL                      (MAKELCID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL),SORT_DEFAULT))


/* Language IDs (were in winnt.h,  for some reason) */


/* Language IDs */

/* FIXME: are the symbolic names correct for LIDs:  0x17, 0x20, 0x28,
 *   0x2a, 0x2b, 0x2c, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
 *   0x37, 0x39, 0x3a, 0x3b, 0x3c, 0x3e, 0x3f, 0x41, 0x43, 0x44,
 *   0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e,
 *   0x4f, 0x57
 */
#define LANG_NEUTRAL                     0x00
#define LANG_AFRIKAANS                          0x36
#define LANG_ALBANIAN                           0x1c
#define LANG_ARABIC                      0x01
#define LANG_ARMENIAN                           0x2b
#define LANG_ASSAMESE                           0x4d
#define LANG_AZERI                              0x2c
#define LANG_BASQUE                      0x2d
#define LANG_BENGALI                            0x45
#define LANG_BULGARIAN                   0x02
#define LANG_BYELORUSSIAN                0x23
#define LANG_CATALAN                     0x03
#define LANG_CHINESE                     0x04
#define LANG_SERBO_CROATIAN              0x1a
#define LANG_CROATIAN                      LANG_SERBO_CROATIAN
#define LANG_SERBIAN                       LANG_SERBO_CROATIAN
#define LANG_CZECH                       0x05
#define LANG_DANISH                      0x06
#define LANG_DUTCH                       0x13
#define LANG_ENGLISH                     0x09
#define LANG_ESTONIAN                    0x25
#define LANG_FAEROESE                    0x38
#define LANG_FARSI                       0x29
#define LANG_FINNISH                     0x0b
#define LANG_FRENCH                      0x0c
#define LANG_GAELIC                             0x3c
#define LANG_GEORGIAN                           0x37
#define LANG_GERMAN                      0x07
#define LANG_GREEK                       0x08
#define LANG_GUJARATI                           0x47
#define LANG_HEBREW                      0x0D
#define LANG_HINDI                              0x39
#define LANG_HUNGARIAN                   0x0e
#define LANG_ICELANDIC                   0x0f
#define LANG_INDONESIAN                  0x21
#define LANG_ITALIAN                     0x10
#define LANG_JAPANESE                    0x11
#define LANG_KANNADA                            0x4b
#define LANG_KAZAKH                             0x3f
#define LANG_KONKANI                            0x57
#define LANG_KOREAN                      0x12
#define LANG_LATVIAN                     0x26
#define LANG_LITHUANIAN                  0x27
#define LANG_MACEDONIAN                         0x2f
#define LANG_MALAY                              0x3e
#define LANG_MALAYALAM                          0x4c
#define LANG_MALTESE                            0x3a
#define LANG_MAORI                              0x28
#define LANG_MARATHI                            0x4e
#define LANG_NORWEGIAN                   0x14
#define LANG_ORIYA                              0x48
#define LANG_POLISH                      0x15
#define LANG_PORTUGUESE                  0x16
#define LANG_PUNJABI                            0x46
#define LANG_RHAETO_ROMANCE                  0x17
#define LANG_ROMANIAN                    0x18
#define LANG_RUSSIAN                     0x19
#define LANG_SAAMI                              0x3b
#define LANG_SANSKRIT                           0x4f
#define LANG_SLOVAK                      0x1b
#define LANG_SLOVENIAN                   0x24
#define LANG_SORBIAN                     0x2e
#define LANG_SPANISH                     0x0a
#define LANG_SUTU                            0x30
#define LANG_SWAHILI                            0x41
#define LANG_SWEDISH                     0x1d
#define LANG_TAMIL                              0x49
#define LANG_TATAR                              0x44
#define LANG_TELUGU                             0x4a
#define LANG_THAI                        0x1e
#define LANG_TSONGA                             0x31
#define LANG_TSWANA                             0x32
#define LANG_TURKISH                     0x1f
#define LANG_UKRAINIAN                   0x22
#define LANG_URDU                            0x20
#define LANG_UZBEK                              0x43
#define LANG_VENDA                              0x33
#define LANG_VIETNAMESE                         0x2a
#define LANG_XHOSA                              0x34
#define LANG_ZULU                            0x35
/* non standard; keep the number high enough (but < 0xff) */
#define LANG_ESPERANTO                          0x8f
#define LANG_WALON                              0x90
#define LANG_CORNISH                            0x91
#define LANG_WELSH                              0x92
#define LANG_BRETON                             0x93

/* Sublanguage definitions */
#define SUBLANG_NEUTRAL                  0x00    /* language neutral */
#define SUBLANG_DEFAULT                  0x01    /* user default */
#define SUBLANG_SYS_DEFAULT              0x02    /* system default */

#define SUBLANG_ARABIC                   0x01
#define SUBLANG_ARABIC_SAUDI_ARABIA      0x01
#define SUBLANG_ARABIC_IRAQ              0x02
#define SUBLANG_ARABIC_EGYPT             0x03
#define SUBLANG_ARABIC_LIBYA             0x04
#define SUBLANG_ARABIC_ALGERIA           0x05
#define SUBLANG_ARABIC_MOROCCO           0x06
#define SUBLANG_ARABIC_TUNISIA           0x07
#define SUBLANG_ARABIC_OMAN              0x08
#define SUBLANG_ARABIC_YEMEN             0x09
#define SUBLANG_ARABIC_SYRIA             0x0a
#define SUBLANG_ARABIC_JORDAN            0x0b
#define SUBLANG_ARABIC_LEBANON           0x0c
#define SUBLANG_ARABIC_KUWAIT            0x0d
#define SUBLANG_ARABIC_UAE               0x0e
#define SUBLANG_ARABIC_BAHRAIN           0x0f
#define SUBLANG_ARABIC_QATAR             0x10
#define SUBLANG_CHINESE_TRADITIONAL      0x01
#define SUBLANG_CHINESE_SIMPLIFIED       0x02
#define SUBLANG_CHINESE_HONGKONG         0x03
#define SUBLANG_CHINESE_SINGAPORE        0x04
#define SUBLANG_CHINESE_MACAU            0x05
#define SUBLANG_DUTCH                    0x01
#define SUBLANG_DUTCH_BELGIAN            0x02
#define SUBLANG_DUTCH_SURINAM                0x03
#define SUBLANG_ENGLISH_US               0x01
#define SUBLANG_ENGLISH_UK               0x02
#define SUBLANG_ENGLISH_AUS              0x03
#define SUBLANG_ENGLISH_CAN              0x04
#define SUBLANG_ENGLISH_NZ               0x05
#define SUBLANG_ENGLISH_EIRE             0x06
#define SUBLANG_ENGLISH_SAFRICA          0x07
#define SUBLANG_ENGLISH_JAMAICA          0x08
#define SUBLANG_ENGLISH_CARRIBEAN        0x09
#define SUBLANG_ENGLISH_BELIZE           0x0a
#define SUBLANG_ENGLISH_TRINIDAD         0x0b
#define SUBLANG_ENGLISH_ZIMBABWE         0x0c
#define SUBLANG_ENGLISH_PHILIPPINES      0x0d
#define SUBLANG_FRENCH                   0x01
#define SUBLANG_FRENCH_BELGIAN           0x02
#define SUBLANG_FRENCH_CANADIAN          0x03
#define SUBLANG_FRENCH_SWISS             0x04
#define SUBLANG_FRENCH_LUXEMBOURG        0x05
#define SUBLANG_FRENCH_MONACO            0x06
#define SUBLANG_GERMAN                   0x01
#define SUBLANG_GERMAN_SWISS             0x02
#define SUBLANG_GERMAN_AUSTRIAN          0x03
#define SUBLANG_GERMAN_LUXEMBOURG        0x04
#define SUBLANG_GERMAN_LIECHTENSTEIN     0x05
#define SUBLANG_ITALIAN                  0x01
#define SUBLANG_ITALIAN_SWISS            0x02
#define SUBLANG_KOREAN                   0x01
#define SUBLANG_KOREAN_JOHAB             0x02
#define SUBLANG_NORWEGIAN_BOKMAL         0x01
#define SUBLANG_NORWEGIAN_NYNORSK        0x02
#define SUBLANG_PORTUGUESE               0x02
#define SUBLANG_PORTUGUESE_BRAZILIAN     0x01
#define SUBLANG_SPANISH                  0x01
#define SUBLANG_SPANISH_MEXICAN          0x02
#define SUBLANG_SPANISH_MODERN           0x03
#define SUBLANG_SPANISH_GUATEMALA        0x04
#define SUBLANG_SPANISH_COSTARICA        0x05
#define SUBLANG_SPANISH_PANAMA           0x06
#define SUBLANG_SPANISH_DOMINICAN        0x07
#define SUBLANG_SPANISH_VENEZUELA        0x08
#define SUBLANG_SPANISH_COLOMBIA         0x09
#define SUBLANG_SPANISH_PERU             0x0a
#define SUBLANG_SPANISH_ARGENTINA        0x0b
#define SUBLANG_SPANISH_ECUADOR          0x0c
#define SUBLANG_SPANISH_CHILE            0x0d
#define SUBLANG_SPANISH_URUGUAY          0x0e
#define SUBLANG_SPANISH_PARAGUAY         0x0f
#define SUBLANG_SPANISH_BOLIVIA          0x10
#define SUBLANG_SPANISH_EL_SALVADOR      0x11
#define SUBLANG_SPANISH_HONDURAS         0x12
#define SUBLANG_SPANISH_NICARAGUA        0x13
#define SUBLANG_SPANISH_PUERTO_RICO      0x14
/* FIXME: I don't know the symbolic names for those */
#define SUBLANG_ROMANIAN                 0x01
#define SUBLANG_ROMANIAN_MOLDAVIA        0x02
#define SUBLANG_RUSSIAN                  0x01
#define SUBLANG_RUSSIAN_MOLDAVIA         0x02
#define SUBLANG_CROATIAN                 0x01
#define SUBLANG_SERBIAN                  0x02
#define SUBLANG_SERBIAN_LATIN            0x03
#define SUBLANG_SWEDISH                  0x01
#define SUBLANG_SWEDISH_FINLAND          0x02
#define SUBLANG_LITHUANIAN               0x01
#define SUBLANG_LITHUANIAN_CLASSIC       0x02
#define SUBLANG_AZERI                    0x01
#define SUBLANG_AZERI_CYRILLIC           0x02
#define SUBLANG_GAELIC                   0x01
#define SUBLANG_GAELIC_SCOTTISH          0x02
#define SUBLANG_GAELIC_MANX              0x03
#define SUBLANG_MALAY                    0x01
#define SUBLANG_MALAY_BRUNEI_DARUSSALAM  0x02
#define SUBLANG_UZBEK                    0x01
#define SUBLANG_UZBEK_CYRILLIC           0x02
#define SUBLANG_URDU_PAKISTAN            0x01

/* Sort definitions */
#define SORT_DEFAULT                     0x0
#define SORT_JAPANESE_XJIS               0x0
#define SORT_JAPANESE_UNICODE            0x1
#define SORT_CHINESE_BIG5                0x0
#define SORT_CHINESE_UNICODE             0x1
#define SORT_KOREAN_KSC                  0x0
#define SORT_KOREAN_UNICODE              0x1


/* Locale Dependent Mapping Flags */
#define LCMAP_LOWERCASE                  0x00000100  /* lower case letters */
#define LCMAP_UPPERCASE                  0x00000200  /* upper case letters */
#define LCMAP_SORTKEY                    0x00000400  /* WC sort key (normalize) */
#define LCMAP_BYTEREV                    0x00000800  /* byte reversal */

#define SORT_STRINGSORT 0x00001000      /* take punctuation into account */

#define LCMAP_HIRAGANA                   0x00100000  /* map katakana to hiragana */
#define LCMAP_KATAKANA                   0x00200000  /* map hiragana to katakana */
#define LCMAP_HALFWIDTH                  0x00400000  /* map double byte to single byte */
#define LCMAP_FULLWIDTH                  0x00800000  /* map single byte to double byte */

/* Date Flags for GetDateFormat. */

#define DATE_SHORTDATE         0x00000001  /* use short date picture */
#define DATE_LONGDATE          0x00000002  /* use long date picture */
#define DATE_USE_ALT_CALENDAR  0x00000004  /* use alternate calendar */
                          /* alt. calendar support is broken anyway */

#define TIME_FORCE24HOURFORMAT 0x00000008  /* force 24 hour format*/
#define TIME_NOTIMEMARKER      0x00000004  /* show no AM/PM */
#define TIME_NOSECONDS         0x00000002  /* show no seconds */
#define TIME_NOMINUTESORSECONDS 0x0000001  /* show no minutes either */

/* internal flags for GetDateFormat system */
#define DATE_DATEVARSONLY      0x00000100  /* only date stuff: yMdg */
#define TIME_TIMEVARSONLY      0x00000200  /* only time stuff: hHmst */
/* use this in a WineLib program if you really want all types */
#define LOCALE_TIMEDATEBOTH    0x00000300  /* full set */

/* Definitions for IsTextUnicode() function */
#define IS_TEXT_UNICODE_ASCII16             0x0001
#define IS_TEXT_UNICODE_SIGNATURE        0x0008
#define IS_TEXT_UNICODE_REVERSE_ASCII16  0x0010
#define IS_TEXT_UNICODE_REVERSE_SIGNATURE 0x0080
#define IS_TEXT_UNICODE_ILLEGAL_CHARS    0x0100
#define IS_TEXT_UNICODE_ODD_LENGTH       0x0200

/* Tests that we currently implement */
#define ITU_IMPLEMENTED_TESTS \
   IS_TEXT_UNICODE_SIGNATURE| \
   IS_TEXT_UNICODE_ODD_LENGTH


/* Character Type Flags */
#ifdef CT_CTYPE1
#  undef CT_CTYPE1
#endif

#ifdef CT_CTYPE2
#  undef CT_CTYPE2
#endif

#ifdef CT_CTYPE3
#  undef CT_CTYPE3
#endif


#define  CT_CTYPE1                          0x00000001  /* usual ctype */
#define  CT_CTYPE2                          0x00000002  /* bidirectional layout info */
#define  CT_CTYPE3                          0x00000004  /* textprocessing info */



/* CType 1 Flag Bits */
#ifdef C1_UPPER
#  undef C1_UPPER
#endif

#ifdef C1_LOWER
#  undef C1_LOWER
#endif

#ifdef C1_DIGIT
#  undef C1_DIGIT
#endif

#ifdef C1_SPACE
#  undef C1_SPACE
#endif

#ifdef C1_PUNCT
#  undef C1_PUNCT
#endif

#ifdef C1_CNTRL
#  undef C1_CNTRL
#endif

#ifdef C1_BLANK
#  undef C1_BLANK
#endif

#ifdef C1_XDIGIT
#  undef C1_XDIGIT
#endif

#ifdef C1_ALPHA
#  undef C1_ALPHA
#endif


#define C1_UPPER                         0x0001
#define C1_LOWER                         0x0002
#define C1_DIGIT                         0x0004
#define C1_SPACE                         0x0008
#define C1_PUNCT                         0x0010
#define C1_CNTRL                         0x0020
#define C1_BLANK                         0x0040
#define C1_XDIGIT                        0x0080
#define C1_ALPHA                         0x0100


/* CType 2 Flag Bits */
#ifdef C2_LEFTTORIGHT
#  undef C2_LEFTTORIGHT
#endif

#ifdef C2_RIGHTTOLEFT
#  undef C2_RIGHTTOLEFT
#endif

#ifdef C2_EUROPENUMBER
#  undef C2_EUROPENUMBER
#endif

#ifdef C2_EUROPESEPARATOR
#  undef C2_EUROPESEPARATOR
#endif

#ifdef C2_EUROPETERMINATOR
#  undef C2_EUROPETERMINATOR
#endif

#ifdef C2_ARABICNUMBER
#  undef C2_ARABICNUMBER
#endif

#ifdef C2_COMMONSEPARATOR
#  undef C2_COMMONSEPARATOR
#endif

#ifdef C2_BLOCKSEPARATOR
#  undef C2_BLOCKSEPARATOR
#endif

#ifdef C2_SEGMENTSEPARATOR
#  undef C2_SEGMENTSEPARATOR
#endif

#ifdef C2_WHITESPACE
#  undef C2_WHITESPACE
#endif

#ifdef C2_OTHERNEUTRAL
#  undef C2_OTHERNEUTRAL
#endif

#ifdef C2_NOTAPPLICABLE
#  undef C2_NOTAPPLICABLE
#endif


#define  C2_LEFTTORIGHT                  0x0001
#define  C2_RIGHTTOLEFT                  0x0002
#define  C2_EUROPENUMBER                 0x0003
#define  C2_EUROPESEPARATOR              0x0004
#define  C2_EUROPETERMINATOR             0x0005
#define  C2_ARABICNUMBER                 0x0006
#define  C2_COMMONSEPARATOR              0x0007
#define  C2_BLOCKSEPARATOR               0x0008
#define  C2_SEGMENTSEPARATOR             0x0009
#define  C2_WHITESPACE                   0x000A
#define  C2_OTHERNEUTRAL                 0x000B
#define  C2_NOTAPPLICABLE                0x0000

/* CType 3 Flag Bits */
#define  C3_NONSPACING                   0x0001
#define  C3_DIACRITIC                    0x0002
#define  C3_VOWELMARK                    0x0004
#define  C3_SYMBOL                       0x0008
#define  C3_KATAKANA                     0x0010
#define  C3_HIRAGANA                     0x0020
#define  C3_HALFWIDTH                    0x0040
#define  C3_FULLWIDTH                    0x0080
#define  C3_IDEOGRAPH                    0x0100
#define  C3_KASHIDA                      0x0200
#define  C3_LEXICAL                      0x0400
#define  C3_ALPHA                        0x8000
#define  C3_NOTAPPLICABLE                0x0000

/* Code page information.
 */
#define MAX_LEADBYTES     12
#define MAX_DEFAULTCHAR   2

/* Defines for calendar handling */
#define CAL_ICALINTVALUE          0x00000001
#define CAL_SCALNAME              0x00000002
#define CAL_IYEAROFFSETRANGE      0x00000003
#define CAL_SERASTRING            0x00000004
#define CAL_SSHORTDATE            0x00000005
#define CAL_SLONGDATE             0x00000006
#define CAL_SDAYNAME1             0x00000007
#define CAL_SDAYNAME2             0x00000008
#define CAL_SDAYNAME3             0x00000009
#define CAL_SDAYNAME4             0x0000000a
#define CAL_SDAYNAME5             0x0000000b
#define CAL_SDAYNAME6             0x0000000c
#define CAL_SDAYNAME7             0x0000000d
#define CAL_SABBREVDAYNAME1       0x0000000e
#define CAL_SABBREVDAYNAME2       0x0000000f
#define CAL_SABBREVDAYNAME3       0x00000010
#define CAL_SABBREVDAYNAME4       0x00000011
#define CAL_SABBREVDAYNAME5       0x00000012
#define CAL_SABBREVDAYNAME6       0x00000013
#define CAL_SABBREVDAYNAME7       0x00000014
#define CAL_SMONTHNAME1           0x00000015
#define CAL_SMONTHNAME2           0x00000016
#define CAL_SMONTHNAME3           0x00000017
#define CAL_SMONTHNAME4           0x00000018
#define CAL_SMONTHNAME5           0x00000019
#define CAL_SMONTHNAME6           0x0000001a
#define CAL_SMONTHNAME7           0x0000001b
#define CAL_SMONTHNAME8           0x0000001c
#define CAL_SMONTHNAME9           0x0000001d
#define CAL_SMONTHNAME10          0x0000001e
#define CAL_SMONTHNAME11          0x0000001f
#define CAL_SMONTHNAME12          0x00000020
#define CAL_SMONTHNAME13          0x00000021
#define CAL_SABBREVMONTHNAME1     0x00000022
#define CAL_SABBREVMONTHNAME2     0x00000023
#define CAL_SABBREVMONTHNAME3     0x00000024
#define CAL_SABBREVMONTHNAME4     0x00000025
#define CAL_SABBREVMONTHNAME5     0x00000026
#define CAL_SABBREVMONTHNAME6     0x00000027
#define CAL_SABBREVMONTHNAME7     0x00000028
#define CAL_SABBREVMONTHNAME8     0x00000029
#define CAL_SABBREVMONTHNAME9     0x0000002a
#define CAL_SABBREVMONTHNAME10    0x0000002b
#define CAL_SABBREVMONTHNAME11    0x0000002c
#define CAL_SABBREVMONTHNAME12    0x0000002d
#define CAL_SABBREVMONTHNAME13    0x0000002e
#define CAL_SYEARMONTH            0x0000002f
#define CAL_GREGORIAN                  1
#define CAL_GREGORIAN_US               2
#define CAL_JAPAN                      3
#define CAL_TAIWAN                     4
#define CAL_KOREA                      5
#define CAL_HIJRI                      6
#define CAL_THAI                       7
#define CAL_HEBREW                     8
#define CAL_GREGORIAN_ME_FRENCH        9
#define CAL_GREGORIAN_ARABIC           10
#define CAL_GREGORIAN_XLIT_ENGLISH     11
#define CAL_GREGORIAN_XLIT_FRENCH      12

typedef DWORD CALTYPE;
typedef DWORD CALID;


#ifndef OS2_INCLUDED

/* Define a bunch of callback types */

#if defined(STRICT) || defined(__WINE__)
typedef BOOL    (* CALLBACK CALINFO_ENUMPROCA)(LPSTR);
typedef BOOL    (* CALLBACK CALINFO_ENUMPROCW)(LPWSTR);
typedef BOOL    (* CALLBACK CALINFO_ENUMPROCEXA)(LPSTR,CALID);
typedef BOOL    (* CALLBACK CALINFO_ENUMPROCEXW)(LPWSTR,CALID);
typedef BOOL    (* CALLBACK CODEPAGE_ENUMPROCA)(LPSTR);
typedef BOOL    (* CALLBACK CODEPAGE_ENUMPROCW)(LPWSTR);
typedef BOOL    (* CALLBACK DATEFMT_ENUMPROCA)(LPSTR);
typedef BOOL    (* CALLBACK DATEFMT_ENUMPROCW)(LPWSTR);
typedef BOOL    (* CALLBACK DATEFMT_ENUMPROCEXA)(LPSTR,CALID);
typedef BOOL    (* CALLBACK DATEFMT_ENUMPROCEXW)(LPWSTR,CALID);
typedef BOOL    (* CALLBACK LOCALE_ENUMPROCA)(LPSTR);
typedef BOOL    (* CALLBACK LOCALE_ENUMPROCW)(LPWSTR);
typedef BOOL    (* CALLBACK TIMEFMT_ENUMPROCA)(LPSTR);
typedef BOOL    (* CALLBACK TIMEFMT_ENUMPROCW)(LPWSTR);
#else
typedef FARPROC CALINFO_ENUMPROCA;
typedef FARPROC CALINFO_ENUMPROCW;
typedef FARPROC CALINFO_ENUMPROCEXA;
typedef FARPROC CALINFO_ENUMPROCEXW;
typedef FARPROC CODEPAGE_ENUMPROCA;
typedef FARPROC CODEPAGE_ENUMPROCW;
typedef FARPROC DATEFMT_ENUMPROCA;
typedef FARPROC DATEFMT_ENUMPROCW;
typedef FARPROC DATEFMT_ENUMPROCEXA;
typedef FARPROC DATEFMT_ENUMPROCEXW;
typedef FARPROC LOCALE_ENUMPROCA;
typedef FARPROC LOCALE_ENUMPROCW;
typedef FARPROC TIMEFMT_ENUMPROCA;
typedef FARPROC TIMEFMT_ENUMPROCW;
#endif /* STRICT || __WINE__ */

DECL_WINELIB_TYPE_AW(CALINFO_ENUMPROC)
DECL_WINELIB_TYPE_AW(CALINFO_ENUMPROCEX)
DECL_WINELIB_TYPE_AW(CODEPAGE_ENUMPROC)
DECL_WINELIB_TYPE_AW(DATEFMT_ENUMPROC)
DECL_WINELIB_TYPE_AW(DATEFMT_ENUMPROCEX)
DECL_WINELIB_TYPE_AW(LOCALE_ENUMPROC)
DECL_WINELIB_TYPE_AW(TIMEFMT_ENUMPROC)


typedef struct
{
    UINT MaxCharSize;
    BYTE   DefaultChar[MAX_DEFAULTCHAR];
    BYTE   LeadByte[MAX_LEADBYTES];
} CPINFO, *LPCPINFO;

typedef struct
{
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
    WCHAR UnicodeDefaultChar;
    UINT CodePage;
    CHAR CodePageName[MAX_PATH];
} CPINFOEXA, *LPCPINFOEXA;

typedef struct
{
    UINT MaxCharSize;
    BYTE DefaultChar[MAX_DEFAULTCHAR];
    BYTE LeadByte[MAX_LEADBYTES];
    WCHAR UnicodeDefaultChar;
    UINT CodePage;
    WCHAR CodePageName[MAX_PATH];
} CPINFOEXW, *LPCPINFOEXW;

DECL_WINELIB_TYPE_AW(CPINFOEX)
DECL_WINELIB_TYPE_AW(LPCPINFOEX)

typedef struct _numberfmtA {
    UINT NumDigits;
    UINT LeadingZero;
    UINT Grouping;
    LPSTR lpDecimalSep;
    LPSTR lpThousandSep;
    UINT NegativeOrder;
} NUMBERFMTA, *LPNUMBERFMTA;

typedef struct _numberfmtW {
    UINT NumDigits;
    UINT LeadingZero;
    UINT Grouping;
    LPWSTR lpDecimalSep;
    LPWSTR lpThousandSep;
    UINT NegativeOrder;
} NUMBERFMTW, *LPNUMBERFMTW;

DECL_WINELIB_TYPE_AW(NUMBERFMT)
DECL_WINELIB_TYPE_AW(LPNUMBERFMT)

typedef struct _currencyfmtA {
    UINT    NumDigits;
    UINT    LeadingZero;
    UINT    Grouping;
    LPSTR   lpDecimalSep;
    LPSTR   lpThousandSep;
    UINT    NegativeOrder;
    UINT    PositiveOrder;
    LPSTR   lpCurrencySymbol;
} CURRENCYFMTA, *LPCURRENCYFMTA;

typedef struct _currencyfmtW {
    UINT    NumDigits;
    UINT    LeadingZero;
    UINT    Grouping;
    LPWSTR  lpDecimalSep;
    LPWSTR  lpThousandSep;
    UINT    NegativeOrder;
    UINT    PositiveOrder;
    LPWSTR  lpCurrencySymbol;
} CURRENCYFMTW, *LPCURRENCYFMTW;

/* APIs
 */

UINT        WINAPI CompareStringA(LCID,DWORD,LPCSTR,DWORD,LPCSTR,DWORD);
UINT        WINAPI CompareStringW(LCID,DWORD,LPCWSTR,DWORD,LPCWSTR,DWORD);
#define     CompareString WINELIB_NAME_AW(CompareString)
LCID        WINAPI ConvertDefaultLocale(LCID);
BOOL        WINAPI EnumCalendarInfoA(CALINFO_ENUMPROCA,LCID,CALID,CALTYPE);
BOOL        WINAPI EnumCalendarInfoW(CALINFO_ENUMPROCW,LCID,CALID,CALTYPE);
#define     EnumCalendarInfo WINELIB_NAME_AW(EnumCalendarInfo)
BOOL        WINAPI EnumCalendarInfoExA(CALINFO_ENUMPROCEXA,LCID,CALID,CALTYPE);
BOOL        WINAPI EnumCalendarInfoExW(CALINFO_ENUMPROCEXW,LCID,CALID,CALTYPE);
#define     EnumCalendarInfoEx WINELIB_NAME_AW(EnumCalendarInfoEx)
BOOL        WINAPI EnumDateFormatsA(DATEFMT_ENUMPROCA,LCID,DWORD);
BOOL        WINAPI EnumDateFormatsW(DATEFMT_ENUMPROCW,LCID,DWORD);
#define     EnumDateFormats WINELIB_NAME_AW(EnumDateFormats)
BOOL        WINAPI EnumDateFormatsExA(DATEFMT_ENUMPROCEXA,LCID,DWORD);
BOOL        WINAPI EnumDateFormatsExW(DATEFMT_ENUMPROCEXW,LCID,DWORD);
#define     EnumDateFormatsEx WINELIB_NAME_AW(EnumDateFormatsEx)
BOOL        WINAPI EnumSystemCodePagesA(CODEPAGE_ENUMPROCA,DWORD);
BOOL        WINAPI EnumSystemCodePagesW(CODEPAGE_ENUMPROCW,DWORD);
#define     EnumSystemCodePages WINELIB_NAME_AW(EnumSystemCodePages)
BOOL        WINAPI EnumSystemLocalesA(LOCALE_ENUMPROCA,DWORD);
BOOL        WINAPI EnumSystemLocalesW(LOCALE_ENUMPROCW,DWORD);
#define     EnumSystemLocales WINELIB_NAME_AW(EnumSystemLocales)
BOOL        WINAPI EnumTimeFormatsA(TIMEFMT_ENUMPROCA,LCID,DWORD);
BOOL        WINAPI EnumTimeFormatsW(TIMEFMT_ENUMPROCW,LCID,DWORD);
#define     EnumTimeFormats WINELIB_NAME_AW(EnumTimeFormats)
int         WINAPI FoldStringA(DWORD,LPCSTR,int,LPSTR,int);
int         WINAPI FoldStringW(DWORD,LPCWSTR,int,LPWSTR,int);
#define     FoldString WINELIB_NAME_AW(FoldString)
UINT        WINAPI GetACP(void);
BOOL        WINAPI GetCPInfo(UINT,LPCPINFO);
BOOL        WINAPI GetCPInfoExA(UINT,DWORD,LPCPINFOEXA);
BOOL        WINAPI GetCPInfoExW(UINT,DWORD,LPCPINFOEXW);
#define     GetCPInfoEx WINELIB_NAME_AW(GetCPInfoEx)
INT         WINAPI GetCurrencyFormatA(LCID,DWORD,LPCSTR,const CURRENCYFMTA*,LPSTR,int);
INT         WINAPI GetCurrencyFormatW(LCID,DWORD,LPCWSTR,const CURRENCYFMTW*,LPWSTR,int);
#define     GetCurrencyFormat WINELIB_NAME_AW(GetCurrencyFormat)
INT         WINAPI GetDateFormatA(LCID,DWORD,CONST SYSTEMTIME*,LPCSTR,LPSTR,INT);
INT         WINAPI GetDateFormatW(LCID,DWORD,CONST SYSTEMTIME*,LPCWSTR,LPWSTR,INT);
#define     GetDateFormat WINELIB_NAME_AW(GetDateFormat)
INT         WINAPI GetLocaleInfoA(LCID,LCTYPE,LPSTR,INT);
INT         WINAPI GetLocaleInfoW(LCID,LCTYPE,LPWSTR,INT);
#define     GetLocaleInfo WINELIB_NAME_AW(GetLocaleInfo)
INT         WINAPI GetNumberFormatA(LCID,DWORD,LPCSTR,const NUMBERFMTA*,LPSTR,int);
INT         WINAPI GetNumberFormatW(LCID,DWORD,LPCWSTR,const NUMBERFMTW*,LPWSTR,int);
#define     GetNumberFormat WINELIB_NAME_AW(GetNumberFormat)
UINT        WINAPI GetOEMCP(void);
BOOL        WINAPI GetStringTypeA(LCID,DWORD,LPCSTR,INT,LPWORD);
BOOL        WINAPI GetStringTypeW(DWORD,LPCWSTR,INT,LPWORD);
BOOL        WINAPI GetStringTypeExA(LCID,DWORD,LPCSTR,INT,LPWORD);
BOOL        WINAPI GetStringTypeExW(LCID,DWORD,LPCWSTR,INT,LPWORD);
#define     GetStringTypeEx WINELIB_NAME_AW(GetStringTypeEx)
LANGID      WINAPI GetSystemDefaultLangID(void);
LCID        WINAPI GetSystemDefaultLCID(void);
LCID        WINAPI GetThreadLocale(void);
INT         WINAPI GetTimeFormatA(LCID,DWORD,CONST SYSTEMTIME*,LPCSTR,LPSTR,INT);
INT         WINAPI GetTimeFormatW(LCID,DWORD,CONST SYSTEMTIME*,LPCWSTR,LPWSTR,INT);
#define     GetTimeFormat WINELIB_NAME_AW(GetTimeFormat)
LANGID      WINAPI GetUserDefaultLangID(void);
LCID        WINAPI GetUserDefaultLCID(void);
BOOL        WINAPI IsDBCSLeadByte(BYTE);
BOOL        WINAPI IsDBCSLeadByteEx(UINT,BYTE);
BOOL        WINAPI IsValidCodePage(UINT);
BOOL        WINAPI IsValidLocale(LCID,DWORD);
INT         WINAPI LCMapStringA(LCID,DWORD,LPCSTR,INT,LPSTR,INT);
INT         WINAPI LCMapStringW(LCID,DWORD,LPCWSTR,INT,LPWSTR,INT);
#define     LCMapString WINELIB_NAME_AW(LCMapString)
INT         WINAPI MultiByteToWideChar(UINT,DWORD,LPCSTR,INT,LPWSTR,INT);
BOOL        WINAPI SetLocaleInfoA(LCID,LCTYPE,LPCSTR);
BOOL        WINAPI SetLocaleInfoW(LCID,LCTYPE,LPCWSTR);
#define     SetLocaleInfo WINELIB_NAME_AW(SetLocaleInfo)
BOOL        WINAPI SetThreadLocale(LCID);
INT         WINAPI WideCharToMultiByte(UINT,DWORD,LPCWSTR,INT,LPSTR,INT,LPCSTR,LPBOOL);

#endif //#ifndef OS2_INCLUDED

#endif  /* __WINE_WINNLS_H */
