/* $Id: file.cpp,v 1.2 2000-03-18 15:14:02 sandervl Exp $ */
/*
 * Project Odin Software License can be found in LICENSE.TXT
 * strncpy replacement (one in RTL lib is buggy; doesn't stop at 0 terminator)
 *
 * Copyright 1999 Sander van Leeuwen
 * Copyright 1999 Patrick Haller
 *
 */
#include <stdio.h>
#include <os2sel.h>
#include <wchar.h>

int  _LNK_CONV os2_fclose( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fclose(fp);
	return rc;
}

int _LNK_CONV os2_feof( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = feof(fp);
	return rc;
}
int _LNK_CONV os2_ferror( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = ferror(fp);
	return rc;
}
int _LNK_CONV os2_fflush( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fflush(fp);
	return rc;
}
int _LNK_CONV os2_fgetc( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fgetc(fp);
	return rc;
}
int _LNK_CONV os2_fgetpos( FILE *fp, fpos_t *pos)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fgetpos(fp,pos);
	return rc;
}
char *_LNK_CONV os2_fgets( char *buf, int size, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    char * rc;

	rc = fgets(buf, size, fp);
	return rc;
}
FILE * _LNK_CONV os2_fopen( const char *name, const char *type)
{
    unsigned short sel = RestoreOS2FS();
    FILE * rc;

	rc = fopen(name, type);
	return rc;
}
int _LNK_CONV os2_fputc( int c, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fputc(c, fp);
	return rc;
}
int _LNK_CONV os2_fputs( const char *string, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fputs(string, fp);
	return rc;
}
size_t _LNK_CONV os2_fread( void *buf, size_t size, size_t elsize, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    size_t rc;

	rc = fread(buf, size, elsize, fp);
	return rc;
}
FILE *  _LNK_CONV os2_freopen( const char *name, const char *type, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    FILE * rc;

	rc = freopen(name, type, fp);
	return rc;
}
int _LNK_CONV os2_fseek( FILE *fp, long int pos, int type)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fseek(fp, pos, type);
	return rc;
}
int _LNK_CONV os2_fsetpos( FILE *fp, const fpos_t *pos)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fsetpos(fp, pos);
	return rc;
}
long int _LNK_CONV os2_ftell( FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    long rc;

	rc = ftell(fp);
	return rc;
}
size_t  _LNK_CONV os2_fwrite( const void *buf, size_t size, size_t elsize, FILE *fp )
{
    unsigned short sel = RestoreOS2FS();
    size_t rc;

	rc = fwrite(buf, size, elsize, fp);
	return rc;
}
int _LNK_CONV os2_vfprintf( FILE *fp, const char *string, __va_list list )
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = vfprintf(fp, string, list);
	return rc;
}
wint_t  _LNK_CONV os2_fgetwc(FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    wint_t rc;

	rc = fgetwc(fp);
	return rc;
}
wchar_t * _LNK_CONV os2_fgetws(wchar_t *buf, int size, FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    wchar_t * rc;

	rc = fgetws(buf, size, fp);
	return rc;
}
wint_t _LNK_CONV os2_fputwc(wchar_t character, FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    wint_t rc;

	rc = fputwc(character, fp);
	return rc;
}
int _LNK_CONV os2_fputws(const wchar_t *string, FILE *fp)
{
    unsigned short sel = RestoreOS2FS();
    int rc;

	rc = fputws(string, fp);
	return rc;
}

FILE * _LNK_CONV os2_fdopen( int a, const char *bla)
{
    unsigned short sel = RestoreOS2FS();
    FILE *rc;

	rc = _fdopen(a, bla);
	return rc;
}

