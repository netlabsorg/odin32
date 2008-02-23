/* $Id: d16crt.h,v 1.1 2002-03-31 19:30:40 bird Exp $
 *
 * Moved all the crt replacements used by the 16-bit part into one file.
 *
 * Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _dev16crt_h_
#define _dev16crt_h_


/* File an output replacements */
HFILE    fopen(const char * pszFilename, const char * pszIgnored);
int      fread(void * pvBuffer, unsigned short cbBlock, unsigned short cBlock,  HFILE hFile);
int      fseek(HFILE hfile, signed long off, int iOrg);
unsigned long fsize(HFILE hFile);
int      feof(HFILE hFile);
char *   fgets(char * psz, int num, HFILE hFile);

/* C-library replacements and additions. */
void *   kmemcpy(char *psz1, const char *psz2, int cch);
char *   kstrstr(const char *psz1, const char *psz2);
int      kstrcmp(const char *psz1, const char *psz2);
int      kstrncmp(const char *psz1, const char *psz2, int cch);
int      kstrnicmp(const char *psz1, const char *psz2, int cch);
int      kstrlen(const char *psz);
char *   kstrcpy(char * pszTarget, const char * pszSource);
char *   kstrncpy(char * pszTarget, const char * pszSource, int cch);
char *   kstrcat(char * pszTarget, const char * pszSource);
char *   kstrtok(char * pszTarget, const char * pszToken);
int      kargncpy(char *pszTarget, const char *pszArg, unsigned cchMaxlen);

#endif
