/* $Id: url.cpp,v 1.4 2000-07-18 18:00:01 bird Exp $ */

/*
 * Win32 Lightweight SHELL32 API for OS/2
 *
 * 2000/04/20 Patrick Haller (haller@zebra.fh-weingarten.de)
 *
 * @(#) url.cpp                 1.0.0   2000/04/20 PH Start from scratch
 */


/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <wcstr.h>
#define HAVE_WCTYPE_H

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>

#include "shlwapi.h"

ODINDEBUGCHANNEL(SHLWAPI-URL)


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


typedef struct tagURLSCHEME
{
  LPSTR pszName;       /* such as http://                    */
  BOOL  flagOpaque;    /* has a double-slash '//             */
  BOOL  flagNoHistory; /* usually not in history of browsers */
  DWORD dwType;        /* URL type                           */

} URLSCHEME, *PURLSCHEME;


typedef enum tagURLIS
{
  URLIS_APPLIABLE,   /* Attempt to determine a valid scheme for the URL. */
  URLIS_DIRECTORY,   /* Does the URL string end with a directory?        */
  URLIS_FILEURL,     /* Is the URL a file URL?                           */
  URLIS_HASQUERY,    /* Does the URL have an appended query string?      */
  URLIS_NOHISTORY,   /* Is the URL a "No History" URL?                   */
  URLIS_OPAQUE,      /* Is the URL opaque?                               */
  URLIS_URL          /* Is the URL valid?                                */
} URLIS;


static URLSCHEME arrUrlSchemes[] =
{ /* scheme     opaque hist   type */
  {"http://",   FALSE, FALSE, 0},
  {"https://",  FALSE, FALSE, 0},
  {"shttp://",  FALSE, FALSE, 0},
  {"file://",   FALSE, FALSE, 0},
  {"ftp://",    FALSE, FALSE, 0},
  {"telnet://", FALSE, TRUE,  0},
  {"news://",   FALSE, TRUE,  0},
  {"snews://",  FALSE, TRUE,  0},
  {"mailto:",   TRUE,  TRUE,  0},
  {"gopher://", FALSE, FALSE, 0},
  {"wais://",   FALSE, FALSE, 0},
};




/*****************************************************************************
 * Name      : UrlApplyScheme
 * Purpose   : Takes a URL string, determines a scheme for it, and returns a
 *             string with an appropriate prefix.
 * Parameters: pszIn   [in]     A NULL-terminated URL string.
 *             pszOut  [out]    A buffer to receive a NULL-terminated string,
 *                              set to the URL specified by pszIn, converted
 *                              to the standard scheme://URL_string format.
 *             pcchOut [in/out] Address of a value set to the number of
 *                              characters in the pszOut buffer. When the
 *                              function returns, the value depends on whether
 *                              the function is successful or returns
 *                              E_POINTER. For other return values, the value
 *                              of this parameter is meaningless.
 *             dwFlags [in]     Flags that specify how to determine the
 *                              scheme. The following flags can be combined.
 *                              URL_APPLY_DEFAULT
 *                                Apply the default scheme if UrlApplyScheme
 *                                can't determine one. The default prefix is
 *                                stored in the registry but is typically "http".
 *                              URL_APPLY_GUESSSCHEME
 *                                Attempt to determine the scheme by examining
 *                                pszIn.
 *                              URL_APPLY_GUESSFILE
 *                                Attempt to determine a file URL from pszIn.
 *                              URL_APPLY_FORCEAPPLY
 *                                Force UrlApplyScheme to determine a scheme
 *                                for pszIn.
 * Variables :
 * Result    : S_OK      A scheme was determined. pszOut points to a string
 *                       containing the URL with the scheme's prefix. The value
 *                       of pcchOut is set to the number of characters in the
 *                       string, not counting the terminating NULL character.
 *             S_FALSE   There were no errors, but no prefix was prepended.
 *             E_POINTER The buffer was too small. The value of pcchOut is
 *                       set to the minimum number of characters that the
 *                       buffer must be able to contain, including the
 *                       terminating NULL character.
 *             Other errors - A standard OLE error value is returned.
 * Remark    : If the URL has a valid scheme, the string will not be modified.
 *             However, almost any combination of two or more characters
 *             followed by a colon will be parsed as a scheme. Valid
 *             characters include some common punctuation marks, such as ".".
 *             If your input string fits this description, UrlApplyScheme may
 *             treat it as valid and not apply a scheme. To force the function
 *             to apply a scheme to a URL, set the URL_APPLY_FORCEAPPLY and
 *             URL_APPLY_DEFAULT flags in dwFlags. This combination of flags
 *             forces the function to apply a scheme to the URL. Typically,
 *             the function will not be able to determine a valid scheme. The
 *             second flag guarantees that, if no valid scheme can be
 *             determined, the function will apply the default scheme to the URL.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Thu, 2000/04/20 19:46]
 *****************************************************************************/

ODINFUNCTION4(HRESULT, UrlApplySchemeA,
              LPCSTR,  pszIn,
              LPSTR,   pszOut,
              LPDWORD, pcchOut,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  strncpy(pszOut,
          pszIn,
          *pcchOut);
  *pcchOut = 0;

  return S_OK;
}


ODINFUNCTION4(HRESULT, UrlApplySchemeW,
              LPCWSTR, pszIn,
              LPWSTR,  pszOut,
              LPDWORD, pcchOut,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  wcsncpy((wchar_t*)pszOut,
          (wchar_t*)pszIn,
          *pcchOut);
  *pcchOut = 0;

  return S_OK;
}


/*****************************************************************************
 * Name      : UrlCanonicalize
 * Purpose   : Takes a URL string and converts it into canonical form.
 * Parameters: pszUrl [in]     Pointer to a URL string. If it does not refer
 *                             to a file, it must include a valid scheme such
 *                             as "http://".
 *             pszCanonicalized  [out] A pointer to a NULL-terminated string
 *                                     used to return the converted URL.
 *             pcchCanonicalized [out] The number of characters in
 *                                     pszCanonicalized.
 *             dwFlags           [in]  Flags that specify how the URL will be
 *                                     converted to canonical form. The
 *                                     following flags can be combined.
 *                                     URL_DONT_SIMPLIFY
 *                                       Treat '/./' and '/../' in a URL
 *                                       string as literal characters, not as
 *                                       shorthand for navigation. See Remarks
 *                                       for further discussion.
 *                                     URL_ESCAPE_PERCENT
 *                                       Convert any occurrence of '%' to
 *                                       its escape sequence.
 *                                     URL_ESCAPE_SPACES_ONLY
 *                                       Replace only spaces with escape
 *                                       sequences. This flag takes precedence
 *                                       over URL_ESCAPE_UNSAFE, but does not
 *                                       apply to opaque URLs.
 *                                     URL_ESCAPE_UNSAFE
 *                                       Replace unsafe values with their escape
 *                                       sequences. This flag applies to all URLs,
 *                                       including opaque URLs.
 *                                     URL_PLUGGABLE_PROTOCOL
 *                                       Combine URLs with client-defined
 *                                       pluggable protocols, according to the W3C
 *                                       specification. This flag does not apply
 *                                       to standard protocols such as ftp, http,
 *                                       gopher, and so on. If this flag is set,
 *                                       UrlCombine will not simplify URLs, so
 *                                       there is no need to also set
 *                                       URL_DONT_SIMPLIFY.
 *                                     URL_UNESCAPE
 *                                       Unescape any escape sequences that the
 *                                       URLs contain, with two exceptions. The
 *                                       escape sequences for '?' and '#' will
 *                                       not be unescaped. If one of the
 *                                       URL_ESCAPE_XXX flags is also set, the two
 *                                       URLs will unescaped, then combined, then
 *                                       escaped.
 * Variables :
 * Result    : Returns S_OK if successful, or a standard OLE error
 *             value otherwise.
 * Remark    : This function will do such tasks as replacing unsafe characters
 *             with their escape sequences and collapsing sequences like "..\...".
 *             If a URL string contains '/../' or '/./', UrlCanonicalize
 *             will normally treat the characters as indicating navigation in
 *             the URL hierarchy. The function will simplify the URLs before
 *             combining them. For instance "/hello/cruel/../world" will be
 *             simplified to "/hello/world". If the URL_DONT_SIMPLIFY flag is
 *             set in dwFlags, the function will not simplify URLs. In this
 *             case, "/hello/cruel/../world" will be left as is.
 * Status    :
 *
 * Author    : Patrick Haller [Thu, 2000/04/20 19:49]
 *****************************************************************************/

ODINFUNCTION4(HRESULT, UrlCanonicalizeA,
              LPCSTR,  pszUrl,
              LPSTR,   pszCanonicalized,
              LPDWORD, pcchCanonicalized,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}


ODINFUNCTION4(HRESULT, UrlCanonicalizeW,
              LPCWSTR, pszUrl,
              LPWSTR,  pszCanonicalized,
              LPDWORD, pcchCanonicalized,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}



/*****************************************************************************
 * Name      : UrlCombine
 * Purpose   : Takes a relative URL and its base and returns a URL in canonical form.
 * Parameters: pszBase     [in]  Pointer to a string with the base URL.
 *             pszRelative [in]  Pointer to a string with the relative URL.
 *             pszCombined [out] Pointer to a buffer to receive a
 *                               NULL-terminated string containing the combined URL.
 *             pcchCombined [in/out] Pointer to a value set to the number of
 *                               characters in the pszCombined buffer. When the
 *                               function returns, the value depends on whether
 *                               the function is successful or returns E_POINTER.
 *                               For other return values, the value of this
 *                               parameter is meaningless.
 *             dwFlags     [in]  Flags that specify how the URL will be converted
 *                               to canonical form. The following flags can be combined.
 *                               URL_DONT_SIMPLIFY
 *                                 Treat '/./' and '/../' in a URL string as literal
 *                                 characters, not as shorthand for
 *                                 navigation. See Remarks for further discussion.
 *                               URL_ESCAPE_PERCENT
 *                                 Convert any occurrence of '%' to its escape sequence.
 *                               URL_ESCAPE_SPACES_ONLY
 *                                 Replace only spaces with escape sequences. This flag
 *                                 takes precedence over
 *                                 URL_ESCAPE_UNSAFE, but does not apply to opaque URLs.
 *                               URL_ESCAPE_UNSAFE
 *                                 Replace unsafe values with their escape sequences.
 *                                 This flag applies to all URLs,
 *                                 including opaque URLs.
 *                               URL_PLUGGABLE_PROTOCOL
 *                                 Combine URLs with client-defined pluggable protocols,
 *                                 according to the W3C specification. This flag does not
 *                                 apply to standard protocols such as ftp, http,
 *                                 gopher, and so on. If this flag is set,
 *                                 UrlCombine will not simplify URLs, so there is
 *                                 no need to also set URL_DONT_SIMPLIFY.
 *                               URL_UNESCAPE
 *                                 Unescape any escape sequences that the URLs contain,
 *                                 with two exceptions. The escape sequences
 *                                 for '?' and '#' will not be unescaped.
 *                                 If one of the URL_ESCAPE_XXX flags is also
 *                                 set, the two URLs will unescaped, then
 *                                 combined, then escaped.
 * Variables :
 * Result    : S_OK      pszCombined points to a string containing the
 *                       combined URLs. The value of pcchCombined is set to
 *                       the number of characters in the string, not counting
 *                       the terminating NULL character.
 *             E_POINTER The buffer was too small. The value of pcchCombined
 *                       is set to the minimum number of characters that the
 *                       buffer must be able to contain, including the
 *                       terminating NULL character.
 *             Other errors A standard OLE error value is returned.
 * Remark    : SHLWAPI.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION5(HRESULT,UrlCombineA,
              LPCSTR, pszBase,
              LPCSTR, pszRelative,
              LPSTR,  pszCombined,
              LPDWORD,pcchCombined,
              DWORD,  dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}


ODINFUNCTION5(HRESULT, UrlCombineW,
              LPCWSTR, pszBase,
              LPCWSTR, pszRelative,
              LPWSTR,  pszCombined,
              LPDWORD, pcchCombined,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}


/*****************************************************************************
 * Name      : UrlCompare
 * Purpose   : Does a case-sensitive comparison of two URL strings.
 * Parameters: pszURL1      [in] NULL-terminated string with the first URL.
 *             pszURL2      [in] NULL-terminated string with the second URL.
 *             fIgnoreSlash [in] Value that is set to TRUE to have UrlCompare
 *                               ignore a trailing '/' character on either or
 *                               both URLs.
 * Variables :
 * Result    : Returns zero if the two strings are equal, apart from a
 *             trailing '\' character if fIgnoreSlash is set to TRUE.
 *             Returns a negative integer if the string pointed to by pszURL1
 *             is less than the string pointed to by pszURL2. Otherwise, it
 *             returns a positive integer.
 * Remark    : SHLWAPI.
 *             For the best results, you should first canonicalize the URLs
 *             with UrlCanonicalize. Then, compare the canonicalized URLs with
 *             UrlCompare.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION3(int,    UrlCompareA,
              LPCSTR, pszURL1,
              LPCSTR, pszURL2,
              BOOL,   fIgnoreSlash)
{
  dprintf(("not correctly implemented."));

  return strcmp(pszURL1,
                pszURL2);
}

ODINFUNCTION3(int,     UrlCompareW,
              LPCWSTR, pszURL1,
              LPCWSTR, pszURL2,
              BOOL,    fIgnoreSlash)
{
  dprintf(("not correctly implemented."));

  return wcscmp((const wchar_t *)pszURL1,
                (const wchar_t *)pszURL2);
}


/*****************************************************************************
 * Name      : UrlCreateFromPath
 * Purpose   : Takes a DOS path and converts it to a canonicalized URL.
 * Parameters: pszPath    Pointer to the string with the DOS path.
 *             pszUrl     Value used to return the URL.
 *             pcchPath   Length of pszUrl.
 *             dwReserved Reserved. Set this parameter to NULL.
 * Variables :
 * Result    : Returns S_FALSE if pszPath is already in URL format. In this
 *             case, pszPath will simply be copied to pszUrl. Otherwise, it
 *             returns S_OK if successful or a standard OLE error value if not.
 * Remark    : SHLWAPI.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION4(HRESULT,UrlCreateFromPathA,
              LPCSTR, pszPath,
              LPSTR,  pszUrl,
              LPDWORD,pcchUrl,
              DWORD,  dwReserved)
{
  dprintf(("not implemented."));

  return S_FALSE;
}


ODINFUNCTION4(HRESULT,UrlCreateFromPathW,
              LPCWSTR,pszPath,
              LPWSTR, pszUrl,
              LPDWORD,pcchUrl,
              DWORD,  dwReserved)
{
  dprintf(("not implemented."));

  return S_FALSE;
}


/*****************************************************************************
 * Name      : UrlEscape
 * Purpose   : Converts unsafe characters, such as spaces, into their
 *             corresponding escape sequences.
 * Parameters: pszURL     [in]  Pointer to a NULL-terminated string with the URL.
 *             pszEscaped [out] Pointer to a NULL-terminated string containing
 *                              the string pointed to by pszURL, with unsafe
 *                              characters converted to their escape sequences.
 *             pcchEscaped [in/out] Number of characters in the buffer pointed
 *                              to by pszEscaped. On entry, the value pcchEscaped
 *                              points to is set to the size of the buffer.
 *                              When the function returns, the value pcchEscaped
 *                              points to is set to the number of characters
 *                              written to that buffer, not counting the terminating
 *                              NULL character. If an E_POINTER error code is
 *                              returned, the buffer was too small, and the
 *                              value pcchEscaped points to is set to the
 *                              required number of characters in the buffer. If
 *                              any other errors are returned, the value that
 *                              pcchEscaped points to is undefined.
 *             dwFlags     [in] Flags that control which characters are escaped.
 *                              It can be a combination of the following flags.
 *               Flag           Description
 *               URL_DONT_ESCAPE_EXTRA_INFO
 *                              Don't convert the # or ? character, or
 *                              any characters following them in the
 *                              string.
 *               URL_ESCAPE_SPACES_ONLY
 *                              Only escape space characters. This
 *                              flag cannot be combined with
 *                              URL_ESCAPE_PERCENT or
 *                              URL_ESCAPE_SEGMENT_ONLY.
 *               URL_ESCAPE_PERCENT
 *                              Escape the % character. By default,
 *                              this character is not escaped.
 *               URL_ESCAPE_SEGMENT_ONLY
 *                              Escape the sections following the
 *                              server component, but not the extra
 *                              information sections following a # or
 *                              ? character.
 * Variables :
 * Result    : Returns an OLE success code if successful. The value pointed to
 *             by pcchEscaped will be set to the number of characters written
 *             to the output buffer, excluding the terminating NULL. If the
 *             buffer was too small, E_POINTER is returned, and the value
 *             pointed to by pcchEscaped will be set to the required buffer
 *             size. Otherwise, an OLE error value is returned.
 * Remark    : SHLWAPI.
 *             Unsafe characters are those characters that may be altered
 *             during transport across the internet. This functions converts
 *             unsafe characters into their equivalent "%xy" escape sequences.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION4(HRESULT, UrlEscapeA,
              LPCSTR,  pszURL,
              LPSTR,   pszEscaped,
              LPDWORD, pcchEscaped,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_FALSE;
}


ODINFUNCTION4(HRESULT, UrlEscapeW,
              LPCWSTR, pszURL,
              LPWSTR,  pszEscaped,
              LPDWORD, pcchEscaped,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_FALSE;
}


/*****************************************************************************
 * Name      : UrlGetLocation
 * Purpose   : Retrieves the location from a URL.
 * Parameters: pszURL [in] Pointer to a NULL-terminated string that contains
 *             the location.
 * Variables :
 * Result    : Returns a pointer to a NULL-terminated string with the
 *             location, or NULL otherwise.
 * Remark    : SHLWAPI.
 *             The location is the segment of the URL starting with a ? or #
 *             character. If a file URL has a query string, the returned
 *             string includes the query string.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION1(LPCSTR, UrlGetLocationA,
              LPCSTR, pszURL)
{
  dprintf(("not implemented."));

  return pszURL;
}


ODINFUNCTION1(LPCWSTR, UrlGetLocationW,
              LPCWSTR, pszURL)
{
  dprintf(("not implemented."));

  return pszURL;
}


/*****************************************************************************
 * Name      : UrlGetPart
 * Purpose   : Takes a URL string and returns a specified part.
 * Parameters: pszIn   [in]     NULL-terminated string that contains the URL.
 *             pszOut  [out]    A buffer that will receive a NULL-terminated
 *                              string with the specified part.
 *             pcchOut [in/out] Address of a value set to the number of
 *                              characters in the pszOut buffer. When the
 *                              function returns, the value depends on whether
 *                              the function is successful or returns E_POINTER.
 *                              For other return values, the value of this
 *                              parameter is meaningless.
 *             dwPart  [in]     Flags that specify which part of the URL to retrieve.
 *                              It can have one of the following values.
 *               Flag                Description
 *               URL_PART_HOSTNAME   The host name.
 *               URL_PART_PASSWORD   The password.
 *               URL_PART_PORT       The port number.
 *               URL_PART_QUERY      The query portion of the URL.
 *               URL_PART_SCHEME     The URL scheme.
 *               URL_PART_USERNAME   The username.
 *
 *             dwFlags [in]     Flag that can be set to keep the URL scheme,
 *                              in addition to the part that is specified by dwPart.
 *               Flag                    Description
 *               URL_PARTFLAG_KEEPSCHEME Keep the URL scheme.
 * Variables :
 * Result    :
 * Remark    : SHLWAPI.
 *             Returns an OLE success code if successful. The value pointed to
 *             by pcchOut will be set to the number of characters written to
 *             the output buffer, excluding the terminating NULL. If the buffer
 *             was too small, E_POINTER is returned, and the value pointed to
 *             by pcchOut will be set to the minimum number of characters that
 *             the buffer must be able to contain, including the terminating
 *             NULL character. Otherwise, an OLE error value is returned.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION5(HRESULT, UrlGetPartA,
              LPCSTR,  pszIn,
              LPSTR,   pszOut,
              LPDWORD, pcchOut,
              DWORD,   dwPart,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}


ODINFUNCTION5(HRESULT, UrlGetPartW,
              LPCWSTR, pszIn,
              LPWSTR,  pszOut,
              LPDWORD, pcchOut,
              DWORD,   dwPart,
              DWORD,   dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}


/*****************************************************************************
 * Name      : UrlHash
 * Purpose   : Hashes a URL string.
 * Parameters: pszURL [in]  Pointer to a NULL-terminated string with the URL.
 *             pbHash [out] Buffer to receive the hashed array.
 *             cbHash [in]  Number of elements in pbHash. It should be no larger
 *                          than 256.
 * Variables :
 * Result    : Returns S_OK if successful, or a standard OLE error value otherwise.
 * Remark    : SHLWAPI.
 *             For example, to hash a URL into a single byte, set
 *             cbHash = sizeof(BYTE) and
 *             pbHash = (LPBYTE)&bHashedValue, where bHashedValue is a
 *             one-byte buffer. To hash a URL into a DWORD, set
 *             cbHash = sizeof(DWORD) and
 *             pbHash = (LPBYTE)&dwHashedValue, where dwHashedValue is a
 *             DWORD buffer.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION3(HRESULT,UrlHashA,
              LPCSTR, pszURL,
              LPBYTE, pbHash,
              DWORD,  cbHash)
{
  dprintf(("not implemented."));

  return S_OK;
}


ODINFUNCTION3(HRESULT,UrlHashW,
              LPCWSTR,pszURL,
              LPBYTE, pbHash,
              DWORD,  cbHash)
{
  dprintf(("not implemented."));

  return S_OK;
}


/*****************************************************************************
 * Name      : UrlIs
 * Purpose   : Tests whether or not a URL is a specified type.
 * Parameters: pszUrl [in] Pointer to a string containing the URL.
 *             UrlIs  [in] Type of URL to be tested for.
 *                         UrlIs can take one of the following values:
 *               URLIS_APPLIABLE Attempt to determine a valid scheme for the URL.
 *               URLIS_DIRECTORY Does the URL string end with a directory?
 *               URLIS_FILEURL   Is the URL a file URL?
 *               URLIS_HASQUERY  Does the URL have an appended query string?
 *               URLIS_NOHISTORY Is the URL a "No History" URL?
 *               URLIS_OPAQUE    Is the URL opaque?
 *               URLIS_URL       Is the URL valid?
 * Variables :
 * Result    : For all but one of the URL types, UrlIs returns TRUE if the URL
 *             is the specified type, or FALSE if not. If UrlIs is set to
 *             URLIS_APPLIABLE, UrlIs will attempt to determine the URL scheme.
 *             If the function is able to determine a scheme, it returns TRUE,
 *             or FALSE otherwise.
 * Remark    : SHLWAPI.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION2(BOOL,   UrlIsA,
              LPCSTR, pszUrl,
              URLIS,  UrlIs)
{
  dprintf(("not implemented."));

  return TRUE;
}


ODINFUNCTION2(BOOL,    UrlIsW,
              LPCWSTR, pszUrl,
              URLIS,   UrlIs)
{
  dprintf(("not implemented."));

  return TRUE;
}

/*****************************************************************************
 * Name      : UrlIsNoHistory
 * Purpose   : Returns whether or not a URL is a No History URL.
 * Parameters: pszURL [in] NULL-terminated string with the URL.
 * Variables :
 * Result    : Returns a non-zero value if the URL is a No History URL, or zero otherwise.
 * Remark    : SHLWAPI.
 *             A No History URL is a URL that browsers typically do not
 *             include in their navigation history.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION1(BOOL,  UrlIsNoHistoryA,
              LPCSTR,pszURL)
{
  return UrlIsA(pszURL, URLIS_NOHISTORY);
}


ODINFUNCTION1(BOOL,   UrlIsNoHistoryW,
              LPCWSTR,pszURL)
{
  return UrlIsW(pszURL, URLIS_NOHISTORY);
}


/*****************************************************************************
 * Name      : UrlIsOpaque
 * Purpose   : Returns whether a URL is opaque.
 * Parameters: pszURL [in] NULL-terminated string with the URL.
 * Variables :
 * Result    : Returns a non-zero value if the URL is opaque, or zero
 *             otherwise.
 * Remark    : SHLWAPI.
 *             A URL that has a scheme that is not followed by two slashes (//)
 *             is opaque. For example, mailto:xyz@somecompany.com is an opaque
 *             URL. Opaque URLs cannot be separated into the standard
 *             URL heirarchy.
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION1(BOOL,   UrlIsOpaqueA,
              LPCSTR, pszURL)
{
  return UrlIsA(pszURL, URLIS_OPAQUE);
}


ODINFUNCTION1(BOOL,   UrlIsOpaqueW,
              LPCWSTR,pszURL)
{
  return UrlIsW(pszURL, URLIS_OPAQUE);
}


/*****************************************************************************
 * Name      : UrlUnEscape
 * Purpose   : Converts escape sequences back into ordinary characters.
 * Parameters: pszURL [in/out]    Pointer to a NULL-terminated string with the
 *                                URL. If dwFlags is set to URL_UNESCAPE_INPLACE,
 *                                the converted string is returned through this
 *                                parameter.
 *             pszUnEscaped [out] Pointer to a buffer that will receive a
 *                                NULL-terminated string containing the
 *                                unescaped version of pszURL. If
 *                                URL_UNESCAPE_INPLACE is set in dwFlags, this
 *                                parameter is ignored.
 *             pcchUnEscaped [in/out] Number of characters in the buffer
 *                                pointed to by pcchUnEscaped. On entry, the
 *                                value pcchUnEscaped points to is set to the
 *                                size of the buffer. If the function returns
 *                                a success code, the value that pcchUnEscaped
 *                                points to is set to the number of characters
 *                                written to that buffer, not counting the
 *                                terminating NULL character. If an E_POINTER
 *                                error code is returned, the buffer was too
 *                                small, and the value pcchUnEscaped points to
 *                                is set to the required number of characters
 *                                that the buffer must be able to contain. If
 *                                any other errors are returned, the value
 *                                that pcchUnEscaped points to is undefined.
 *             dwFlags      [in]  Flags that control which characters are
 *                                unescaped. It can be a combination of the
 *                                following flags.
 *               Flag              Description
 *               URL_DONT_UNESCAPE_EXTRA_INFO
 *                                 Don't convert the # or ? character,
 *                                 or any characters following them
 *                                 in the string.
 *               URL_UNESCAPE_INPLACE Use pszURL to return the converted
 *                                 string instead of pszUnEscaped.
 * Variables :
 * Result    : Returns an OLE success code if successful. If the
 *             URL_UNESCAPE_INPLACE flag is not set, the value pointed to by
 *             pcchUnEscaped will be set to the number of characters in the
 *             output buffer pointed to by pszUnEscaped. Returns E_POINTER if
 *             the URL_UNESCAPE_INPLACE flag is not set and the output buffer
 *             is too small. The pcchUnEscaped parameter will be set to the
 *             required buffer size. Otherwise, returns an OLE error value.
 * Remark    : SHLWAPI.UrlUnEscape
 *             An escape sequence has the form "%xy".
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 2000/04/25 02:02]
 *****************************************************************************/

ODINFUNCTION4(HRESULT,UrlUnescapeA,
              LPSTR,  pszURL,
              LPSTR,  pszUnEscaped,
              LPDWORD,pcchUnEscaped,
              DWORD,  dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}


ODINFUNCTION4(HRESULT,UrlUnescapeW,
              LPWSTR, pszURL,
              LPWSTR, pszUnEscaped,
              LPDWORD,pcchUnEscaped,
              DWORD,  dwFlags)
{
  dprintf(("not implemented."));

  return S_OK;
}
