/*
 * Main function
 *
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Martin von Loewis
 * Copyright 1995, 1996, 1997 Alexandre Julliard
 * Copyright 1997 Eric Youngdale
 * Copyright 1999 Ulrich Weigand
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

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif

#include "build.h"

ORDDEF *EntryPoints[MAX_ORDINALS];
ORDDEF *Ordinals[MAX_ORDINALS];
ORDDEF *Names[MAX_ORDINALS];

SPEC_MODE SpecMode = SPEC_MODE_DLL;
SPEC_TYPE SpecType = SPEC_WIN32;

int Base = MAX_ORDINALS;
int Limit = 0;
int DLLHeapSize = 0;
int UsePIC = 0;
int stack_size = 0;
int nb_entry_points = 0;
int nb_names = 0;
int nb_debug_channels = 0;
int nb_lib_paths = 0;
int nb_errors = 0;
int display_warnings = 0;
int kill_at = 0;

/* we only support relay debugging on i386 */
#if defined(__i386__) && !defined(NO_TRACE_MSGS)
int debugging = 1;
#else
int debugging = 0;
#endif

char *owner_name = NULL;
char *dll_name = NULL;
char *dll_file_name = NULL;
char *init_func = NULL;
char **debug_channels = NULL;
char **lib_path = NULL;

char *input_file_name = NULL;
const char *output_file_name = NULL;

static FILE *input_file;
static FILE *output_file;
static const char *current_src_dir;
static int nb_res_files;
static char **res_files;

/* set the dll file name from the input file name */
static void set_dll_file_name( const char *name )
{
    char *p;

    if (dll_file_name) return;

    if ((p = strrchr( name, '\\' ))) name = p + 1;
    if ((p = strrchr( name, '/' ))) name = p + 1;
    dll_file_name = xmalloc( strlen(name) + 5 );
    strcpy( dll_file_name, name );
    if ((p = strrchr( dll_file_name, '.' )) && !strcmp( p, ".def" )) *p = 0;
    if (!strchr( dll_file_name, '.' )) strcat( dll_file_name, ".dll" );

    dll_name = strdup(dll_file_name);
    p = strrchr(dll_name, '.');
    if(p)  *p = 0;
}


/*******************************************************************
 *         command-line option handling
 */
static const char usage_str[] =
"Usage: pebuild dllname dllname.def\n\n";


/*******************************************************************
 *         main
 */
int main(int argc, char **argv)
{
    if(argc != 3) {
        printf(usage_str);
        return -1;
    }

    set_dll_file_name(argv[1]);
    input_file = open_input_file( NULL, argv[2] );

    ParseTopLevel(input_file);
    BuildSpec32File( stdout );

    return 0;
}
