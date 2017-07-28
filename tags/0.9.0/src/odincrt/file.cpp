/* $Id: file.cpp,v 1.5 2003-07-28 11:30:17 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 * strncpy replacement (one in RTL lib is buggy; doesn't stop at 0 terminator)
 *
 * Copyright 1999 Sander van Leeuwen
 * Copyright 1999 Patrick Haller
 *
 */
#define ORIGINAL_VAC_FUNCTIONS
#include <stdio.h>
#include <os2sel.h>
#include <wchar.h>

int  _LNK_CONV CRTWRAP(fclose)( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fclose(fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(feof)( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = feof(fp);
	SetFS(sel);
	return rc;
}
int _LNK_CONV CRTWRAP(ferror)( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = ferror(fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fflush)( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fflush(fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fgetc)( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fgetc(fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fgetpos)( FILE *fp, fpos_t *pos)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fgetpos(fp,pos);
	SetFS(sel);
	return rc;
}

char *_LNK_CONV CRTWRAP(fgets)( char *buf, int size, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    char * rc;

	rc = fgets(buf, size, fp);
	SetFS(sel);
	return rc;
}

FILE * _LNK_CONV CRTWRAP(fopen)( const char *name, const char *type)
{
    unsigned short sel = RestoreOS2FS();
    FILE * rc;

	rc = fopen(name, type);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fputc)( int c, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fputc(c, fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fputs)( const char *string, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fputs(string, fp);
	SetFS(sel);
	return rc;
}

size_t _LNK_CONV CRTWRAP(fread)( void *buf, size_t size, size_t elsize, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    size_t rc;

	rc = fread(buf, size, elsize, fp);
	SetFS(sel);
	return rc;
}

FILE *  _LNK_CONV CRTWRAP(freopen)( const char *name, const char *type, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    FILE * rc;

	rc = freopen(name, type, fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fseek)( FILE *fp, long int pos, int type)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fseek(fp, pos, type);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fsetpos)( FILE *fp, const fpos_t *pos)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fsetpos(fp, pos);
	SetFS(sel);
	return rc;
}

long int _LNK_CONV CRTWRAP(ftell)( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    long rc;

	rc = ftell(fp);
	SetFS(sel);
	return rc;
}

size_t  _LNK_CONV CRTWRAP(fwrite)( const void *buf, size_t size, size_t elsize, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    size_t rc;

	rc = fwrite(buf, size, elsize, fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(vfprintf)( FILE *fp, const char *string, __va_list list )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = vfprintf(fp, string, list);
	SetFS(sel);
	return rc;
}

wint_t  _LNK_CONV CRTWRAP(fgetwc)(FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    wint_t rc;

	rc = fgetwc(fp);
	SetFS(sel);
	return rc;
}

wchar_t * _LNK_CONV CRTWRAP(fgetws)(wchar_t *buf, int size, FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    wchar_t * rc;

	rc = fgetws(buf, size, fp);
	SetFS(sel);
	return rc;
}

wint_t _LNK_CONV CRTWRAP(fputwc)(wchar_t character, FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    wint_t rc;

	rc = fputwc(character, fp);
	SetFS(sel);
	return rc;
}

int _LNK_CONV CRTWRAP(fputws)(const wchar_t *string, FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fputws(string, fp);
	SetFS(sel);
	return rc;
}

FILE * _LNK_CONV CRTWRAP(fdopen)( int a, const char *bla)
{
    unsigned short sel = RestoreOS2FS();
    FILE *rc;

	rc = _fdopen(a, bla);
	SetFS(sel);
	return rc;
}


char * _LNK_CONV CRTWRAP(tmpnam)( char * a)
{
    unsigned short sel = RestoreOS2FS();
    char *rc;

	rc = tmpnam(a);
	SetFS(sel);
	return rc;
}
