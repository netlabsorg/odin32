/*
 * Small utility functions for winebuild
 *
 * Copyright 2000 Alexandre Julliard
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
 */

#include "config.h"
#include "port.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "build.h"

void *xmalloc (size_t size)
{
    void *res;

    res = malloc (size ? size : 1);
    if (res == NULL)
    {
        fprintf (stderr, "Virtual memory exhausted.\n");
        exit (1);
    }
    return res;
}

void *xrealloc (void *ptr, size_t size)
{
    void *res = realloc (ptr, size);
    if (res == NULL)
    {
        fprintf (stderr, "Virtual memory exhausted.\n");
        exit (1);
    }
    return res;
}

char *xstrdup( const char *str )
{
    char *res = strdup( str );
    if (!res)
    {
        fprintf (stderr, "Virtual memory exhausted.\n");
        exit (1);
    }
    return res;
}

char *strupper(char *s)
{
    char *p;
    for (p = s; *p; p++) *p = toupper(*p);
    return s;
}

void fatal_error( const char *msg, ... )
{
    va_list valist;
    va_start( valist, msg );
    if (input_file_name)
    {
        fprintf( stderr, "%s:", input_file_name );
        if (current_line)
            fprintf( stderr, "%d:", current_line );
        fputc( ' ', stderr );
    }
    vfprintf( stderr, msg, valist );
    va_end( valist );
    exit(1);
}

void fatal_perror( const char *msg, ... )
{
    va_list valist;
    va_start( valist, msg );
    if (input_file_name)
    {
        fprintf( stderr, "%s:", input_file_name );
        if (current_line)
            fprintf( stderr, "%d:", current_line );
        fputc( ' ', stderr );
    }
    vfprintf( stderr, msg, valist );
    perror( " " );
    va_end( valist );
    exit(1);
}

void error( const char *msg, ... )
{
    va_list valist;
    va_start( valist, msg );
    if (input_file_name)
    {
        fprintf( stderr, "%s:", input_file_name );
        if (current_line)
            fprintf( stderr, "%d:", current_line );
        fputc( ' ', stderr );
    }
    vfprintf( stderr, msg, valist );
    va_end( valist );
    nb_errors++;
}

void warning( const char *msg, ... )
{
    va_list valist;

    if (!display_warnings) return;
    va_start( valist, msg );
    if (input_file_name)
    {
        fprintf( stderr, "%s:", input_file_name );
        if (current_line)
            fprintf( stderr, "%d:", current_line );
        fputc( ' ', stderr );
    }
    fprintf( stderr, "warning: " );
    vfprintf( stderr, msg, valist );
    va_end( valist );
}

/* output a standard header for generated files */
void output_standard_file_header( FILE *outfile )
{
    if (input_file_name)
        fprintf( outfile, ";;/* File generated automatically from %s; do not edit! */\n",
                 input_file_name );
    else
        fprintf( outfile, ";;/* File generated automatically; do not edit! */\n" );
    fprintf( outfile,
             ";;/* This file can be copied, modified and distributed without restriction. */\n\n" );
}

/* dump a byte stream into the assembly code */
void dump_bytes( FILE *outfile, const unsigned char *data, int len,
                 const char *label, int constant )
{
    int i;

    fprintf( outfile, "\nstatic %sunsigned char %s[%d] = {",
             constant ? "const " : "", label, len );
    for (i = 0; i < len; i++)
    {
        if (!(i & 7)) fprintf( outfile, "\n  " );
        fprintf( outfile, "0x%02x", *data++ );
        if (i < len - 1) fprintf( outfile, "," );
    }
    fprintf( outfile, "\n};\n" );
}


/*******************************************************************
 *         open_input_file
 *
 * Open a file in the given srcdir and set the input_file_name global variable.
 */
FILE *open_input_file( const char *srcdir, const char *name )
{
    char *fullname;
    FILE *file;

    if (srcdir)
    {
        fullname = xmalloc( strlen(srcdir) + strlen(name) + 2 );
        strcpy( fullname, srcdir );
        strcat( fullname, "/" );
        strcat( fullname, name );
    }
    else fullname = xstrdup( name );

    if (!(file = fopen( fullname, "r" ))) fatal_error( "Cannot open file '%s'\n", fullname );
    input_file_name = fullname;
    current_line = 1;
    return file;
}


/*******************************************************************
 *         close_input_file
 *
 * Close the current input file (must have been opened with open_input_file).
 */
void close_input_file( FILE *file )
{
    fclose( file );
    free( input_file_name );
    input_file_name = NULL;
    current_line = 0;
}


/*******************************************************************
 *         make_c_identifier
 *
 * Map a string to a valid C identifier.
 */
const char *make_c_identifier( const char *str )
{
    static char buffer[256];
    char *p;

    for (p = buffer; *str && p < buffer+sizeof(buffer)-1; p++, str++)
    {
        if (isalnum(*str)) *p = *str;
        else *p = '_';
    }
    *p = 0;
    return buffer;
}


/*****************************************************************
 *  Function:    get_alignment
 *
 *  Description:
 *    According to the info page for gas, the .align directive behaves
 * differently on different systems.  On some architectures, the
 * argument of a .align directive is the number of bytes to pad to, so
 * to align on an 8-byte boundary you'd say
 *     .align 8
 * On other systems, the argument is "the number of low-order zero bits
 * that the location counter must have after advancement."  So to
 * align on an 8-byte boundary you'd say
 *     .align 3
 *
 * The reason gas is written this way is that it's trying to mimick
 * native assemblers for the various architectures it runs on.  gas
 * provides other directives that work consistantly across
 * architectures, but of course we want to work on all arches with or
 * without gas.  Hence this function.
 *
 *
 *  Parameters:
 *                alignBoundary  --  the number of bytes to align to.
 *                                   If we're on an architecture where
 *                                   the assembler requires a 'number
 *                                   of low-order zero bits' as a
 *                                   .align argument, then this number
 *                                   must be a power of 2.
 *
 */
int get_alignment(int alignBoundary)
{
#ifdef __powerpc__

    int n = 0;

    switch(alignBoundary)
    {
    case 2:
        n = 1;
        break;
    case 4:
        n = 2;
        break;
    case 8:
        n = 3;
        break;
    case 16:
        n = 4;
        break;
    case 32:
        n = 5;
        break;
    case 64:
        n = 6;
        break;
    case 128:
        n = 7;
        break;
    case 256:
        n = 8;
        break;
    case 512:
        n = 9;
        break;
    case 1024:
        n = 10;
        break;
    case 2048:
        n = 11;
        break;
    case 4096:
        n = 12;
        break;
    case 8192:
        n = 13;
        break;
    case 16384:
        n = 14;
        break;
    case 32768:
        n = 15;
        break;
    case 65536:
        n = 16;
        break;
    default:
        fatal_error("Alignment to %d-byte boundary not supported on this architecture.\n",
                    alignBoundary);
    }
    return n;

#elif defined(__i386__) || defined(__sparc__)

    return alignBoundary;

#else
#error "How does the '.align' assembler directive work on your architecture?"
#endif
}
