/*
 * GetBinaryTypeA/W (Wine Port)
 *
 * Copyright 1995 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include <windows.h>
#include <module.h>
#include <debugtools.h>
#include <heapstring.h>
#include <misc.h>

#define FILE_strcasecmp  strcmpi

/* Check whether a file is an OS/2 or a very old Windows executable
 * by testing on import of KERNEL.
 *
 * FIXME: is reading the module imports the only way of discerning
 *        old Windows binaries from OS/2 ones ? At least it seems so...
 */
static DWORD MODULE_Decide_OS2_OldWin(HANDLE hfile, IMAGE_DOS_HEADER *mz, IMAGE_OS2_HEADER *ne)
{
    DWORD currpos = SetFilePointer( hfile, 0, NULL, SEEK_CUR);
    DWORD type = SCS_OS216_BINARY;
    LPWORD modtab = NULL;
    LPSTR nametab = NULL;
    DWORD len;
    int i;

    /* read modref table */
    if ( (SetFilePointer( hfile, mz->e_lfanew + ne->ne_modtab, NULL, SEEK_SET ) == -1)
      || (!(modtab = (LPWORD)HeapAlloc( GetProcessHeap(), 0, ne->ne_cmod*sizeof(WORD))))
      || (!(ReadFile(hfile, modtab, ne->ne_cmod*sizeof(WORD), &len, NULL)))
      || (len != ne->ne_cmod*sizeof(WORD)) )
	goto broken;

    /* read imported names table */
    if ( (SetFilePointer( hfile, mz->e_lfanew + ne->ne_imptab, NULL, SEEK_SET ) == -1)
      || (!(nametab = (LPSTR)HeapAlloc( GetProcessHeap(), 0, ne->ne_enttab - ne->ne_imptab)))
      || (!(ReadFile(hfile, nametab, ne->ne_enttab - ne->ne_imptab, &len, NULL)))
      || (len != ne->ne_enttab - ne->ne_imptab) )
	goto broken;

    for (i=0; i < ne->ne_cmod; i++)
    {
	LPSTR module = &nametab[modtab[i]];
	TRACE("modref: %.*s\n", module[0], &module[1]);
	if (!(strncmp(&module[1], "KERNEL", module[0])))
	{ /* very old Windows file */
	    MESSAGE("This seems to be a very old (pre-3.0) Windows executable. Expect crashes, especially if this is a real-mode binary !\n");
	    type = SCS_WOW_BINARY;
	    goto good;
	}
    }

broken:
    ERR("Hmm, an error occurred. Is this binary file broken ?\n");

good:
    HeapFree( GetProcessHeap(), 0, modtab);
    HeapFree( GetProcessHeap(), 0, nametab);
    SetFilePointer( hfile, currpos, NULL, SEEK_SET); /* restore filepos */
    return type;
}

/***********************************************************************
 *           MODULE_GetBinaryType
 *
 * The GetBinaryType function determines whether a file is executable
 * or not and if it is it returns what type of executable it is.
 * The type of executable is a property that determines in which
 * subsystem an executable file runs under.
 *
 * Binary types returned:
 * SCS_32BIT_BINARY: A Win32 based application
 * SCS_DOS_BINARY: An MS-Dos based application
 * SCS_WOW_BINARY: A Win16 based application
 * SCS_PIF_BINARY: A PIF file that executes an MS-Dos based app
 * SCS_POSIX_BINARY: A POSIX based application ( Not implemented )
 * SCS_OS216_BINARY: A 16bit OS/2 based application
 *
 * Returns TRUE if the file is an executable in which case
 * the value pointed by lpBinaryType is set.
 * Returns FALSE if the file is not an executable or if the function fails.
 *
 * To do so it opens the file and reads in the header information
 * if the extended header information is not present it will
 * assume that the file is a DOS executable.
 * If the extended header information is present it will
 * determine if the file is a 16 or 32 bit Windows executable
 * by check the flags in the header.
 *
 * Note that .COM and .PIF files are only recognized by their
 * file name extension; but Windows does it the same way ...
 */
static BOOL MODULE_GetBinaryType( HANDLE hfile, LPCSTR filename, LPDWORD lpBinaryType )
{
    IMAGE_DOS_HEADER mz_header;
    char magic[4], *ptr;
    DWORD len;

    /* Seek to the start of the file and read the DOS header information.
     */
    if (    SetFilePointer( hfile, 0, NULL, SEEK_SET ) != -1  
         && ReadFile( hfile, &mz_header, sizeof(mz_header), &len, NULL )
         && len == sizeof(mz_header) )
    {
        /* Now that we have the header check the e_magic field
         * to see if this is a dos image.
         */
        if ( mz_header.e_magic == IMAGE_DOS_SIGNATURE )
        {
            BOOL lfanewValid = FALSE;
            /* We do have a DOS image so we will now try to seek into
             * the file by the amount indicated by the field
             * "Offset to extended header" and read in the
             * "magic" field information at that location.
             * This will tell us if there is more header information
             * to read or not.
             */
            /* But before we do we will make sure that header
             * structure encompasses the "Offset to extended header"
             * field.
             */
            if ( (mz_header.e_cparhdr<<4) >= sizeof(IMAGE_DOS_HEADER) )
                if ( ( mz_header.e_crlc == 0 ) ||
                     ( mz_header.e_lfarlc >= sizeof(IMAGE_DOS_HEADER) ) )
                    if (    mz_header.e_lfanew >= sizeof(IMAGE_DOS_HEADER)
                         && SetFilePointer( hfile, mz_header.e_lfanew, NULL, SEEK_SET ) != -1  
                         && ReadFile( hfile, magic, sizeof(magic), &len, NULL )
                         && len == sizeof(magic) )
                        lfanewValid = TRUE;

            if ( !lfanewValid )
            {
                /* If we cannot read this "extended header" we will
                 * assume that we have a simple DOS executable.
                 */
                *lpBinaryType = SCS_DOS_BINARY;
                return TRUE;
            }
            else
            {
                /* Reading the magic field succeeded so
                 * we will try to determine what type it is.
                 */
                if ( *(DWORD*)magic      == IMAGE_NT_SIGNATURE )
                {
                    /* This is an NT signature.
                     */
                    *lpBinaryType = SCS_32BIT_BINARY;
                    return TRUE;
                }
                else if ( *(WORD*)magic == IMAGE_OS2_SIGNATURE )
                {
                    /* The IMAGE_OS2_SIGNATURE indicates that the
                     * "extended header is a Windows executable (NE)
                     * header."  This can mean either a 16-bit OS/2
                     * or a 16-bit Windows or even a DOS program 
                     * (running under a DOS extender).  To decide
                     * which, we'll have to read the NE header.
                     */

                     IMAGE_OS2_HEADER ne;
                     if (    SetFilePointer( hfile, mz_header.e_lfanew, NULL, SEEK_SET ) != -1  
                          && ReadFile( hfile, &ne, sizeof(ne), &len, NULL )
                          && len == sizeof(ne) )
                     {
                         switch ( ne.ne_exetyp )
                         {
                         case 2:  *lpBinaryType = SCS_WOW_BINARY;   return TRUE;
                         case 5:  *lpBinaryType = SCS_DOS_BINARY;   return TRUE;
                         default: *lpBinaryType =
				  MODULE_Decide_OS2_OldWin(hfile, &mz_header, &ne);
				  return TRUE;
                         }
                     }
                     /* Couldn't read header, so abort. */
                     return FALSE;
                }
                else
                {
                    /* Unknown extended header, but this file is nonetheless
		       DOS-executable.
                     */
                    *lpBinaryType = SCS_DOS_BINARY;
	            return TRUE;
                }
            }
        }
    }

    /* If we get here, we don't even have a correct MZ header.
     * Try to check the file extension for known types ...
     */
    ptr = strrchr( filename, '.' );
    if ( ptr && !strchr( ptr, '\\' ) && !strchr( ptr, '/' ) )
    {
        if ( !FILE_strcasecmp( ptr, ".COM" ) )
        {
            *lpBinaryType = SCS_DOS_BINARY;
            return TRUE;
        }

        if ( !FILE_strcasecmp( ptr, ".PIF" ) )
        {
            *lpBinaryType = SCS_PIF_BINARY;
            return TRUE;
        }
    }

    return FALSE;
}

/***********************************************************************
 *             GetBinaryTypeA                     [KERNEL32.280]
 */
BOOL WINAPI GetBinaryTypeA( LPCSTR lpApplicationName, LPDWORD lpBinaryType )
{
    BOOL ret = FALSE;
    HANDLE hfile;

    dprintf(("KERNEL32:  GetBinaryTypeA %s %x", lpApplicationName, lpBinaryType));

    /* Sanity check.
     */
    if ( lpApplicationName == NULL || lpBinaryType == NULL ) {
        //TODO: Set last error?????
        dprintf(("WARNING: Invalid parameter!"));
        return FALSE;
    }

    /* Open the file indicated by lpApplicationName for reading.
     */
    hfile = CreateFileA( lpApplicationName, GENERIC_READ, FILE_SHARE_READ,
                         NULL, OPEN_EXISTING, 0, 0 );
    if ( hfile == INVALID_HANDLE_VALUE )
        return FALSE;

    /* Check binary type
     */
    ret = MODULE_GetBinaryType( hfile, lpApplicationName, lpBinaryType );

    /* Close the file.
     */
    CloseHandle( hfile );

    //TODO: Set last error?????
    return ret;
}

/***********************************************************************
 *             GetBinaryTypeW                      [KERNEL32.281]
 */
BOOL WINAPI GetBinaryTypeW( LPCWSTR lpApplicationName, LPDWORD lpBinaryType )
{
    BOOL ret = FALSE;
    LPSTR strNew = NULL;

    dprintf(("KERNEL32: GetBinaryTypeW %x %x", lpApplicationName, lpBinaryType));

    /* Sanity check.
     */
    if ( lpApplicationName == NULL || lpBinaryType == NULL ) {
        //TODO: Set last error?????
        dprintf(("WARNING: Invalid parameter!"));
        return FALSE;
    }

    /* Convert the wide string to a ascii string.
     */
    strNew = HEAP_strdupWtoA( GetProcessHeap(), 0, lpApplicationName );

    if ( strNew != NULL )
    {
        ret = GetBinaryTypeA( strNew, lpBinaryType );

        /* Free the allocated string.
         */
        HeapFree( GetProcessHeap(), 0, strNew );
    }

    return ret;
}
