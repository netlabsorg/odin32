/* $Id: crt_memory.cpp,v 1.1 2000-11-21 23:48:54 phaller Exp $ */

/*
 * The C RunTime DLL
 * 
 * Implements C run-time functionality as known from UNIX.
 *
 * Partialy based on Wine
 *
 * Copyright 1996,1998 Marcus Meissner
 * Copyright 1996 Jukka Iivonen
 * Copyright 1997 Uwe Bonnes
 * Copyright 1999-2000 Jens Wiessner
 *
 * CRTDLL - memory functions
 *
 */

#include <odin.h>
#include <os2win.h>
#include <ctype.h>
#include <memory.h>


/*********************************************************************
 *	_memccpy				(CRTDLL.230)
 */
void * CDECL CRTDLL__memccpy (void *s1, const void *s2, int c, size_t n)
{
  dprintf2(("CRTDLL: _memccpy\n"));
  size_t i;
  
  for (i = 0; i < n; ++i)
    {
      if ((((char *)s1)[i] = ((char *)s2)[i]) == (char)c)
        return (char *)s1+i+1;
    }
  return NULL;
}


/*********************************************************************
 *	_memicmp				(CRTDLL.231)
 */
INT CDECL CRTDLL__memicmp(
	LPCSTR s1,	/* [in] first string */
	LPCSTR s2,	/* [in] second string */
	DWORD len	/* [in] length to compare */ )
{
	dprintf2(("CRTDLL: _memicmp(%08xh, %08xh, %08xh)\n",s1,s2,len));
	int	i;

	for (i=0;i<len;i++) {
		if (tolower(s1[i])<tolower(s2[i]))
			return -1;
		if (tolower(s1[i])>tolower(s2[i]))
			return  1;
	}
	return 0;
}



/*********************************************************************
 *	memchr				(CRTDLL.428)
 */
void * CDECL CRTDLL_memchr( const void *s, int c, size_t n )
{
    dprintf2(("CRTDLL: memchr(%08xh, %08xh, %08xh)\n", s, c, n));
    return memchr( s, c, n );
}


/*********************************************************************
 *	memcmp				(CRTDLL.429)
 */
int CDECL CRTDLL_memcmp( const void * c1, const void * c2, size_t n )
{
    dprintf2(("CRTDLL: memcmp(%08xh, %08xh, %08xh)\n", c1, c2, n));
    return memcmp( c1, c2, n );
}


/*********************************************************************
 *	memcpy				(CRTDLL.430)
 */
void * CDECL CRTDLL_memcpy( void *s1, const void *s2, size_t n )
{
    dprintf2(("CRTDLL: memcpy(%08xh, %08xh, %08xh)\n", s1, s2, n));
    return memcpy( s1, s2, n );
}


/*********************************************************************
 *	memmove				(CRTDLL.431)
 */
VOID CDECL CRTDLL_memmove(VOID UNALIGNED *Destination, CONST VOID UNALIGNED *Source, DWORD Length)
{
  dprintf2(("CRTDLL: memmove"));
  memmove(Destination, Source, Length);
}


/*********************************************************************
 *	memset				(CRTDLL.432)
 */
void * CDECL CRTDLL_memset( void *s, int i, size_t n )
{
    dprintf2(("CRTDLL: memset(%08xh, %08xh, %08xh)\n", s, i, n));
    return memset( s, i, n );
}
