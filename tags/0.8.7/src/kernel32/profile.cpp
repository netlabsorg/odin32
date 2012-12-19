/* $Id: profile.cpp,v 1.37 2004-04-07 09:13:53 sandervl Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 profile API functions
 *
 * Copyright 1993 Miguel de Icaza
 * Copyright 1996 Alexandre Julliard
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Patrick Haller
 * Copyright 1999 Christoph Bratschi
 */

#include <os2win.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unicode.h>
#include <odinwrap.h>
#include "heap.h"
#include "heapstring.h"

#include <sys/stat.h>

#include "winbase.h"
#include "winuser.h"
#include "winnls.h"

#include "profile.h"

#define DBG_LOCALLOG	DBG_profile
#include "dbglocal.h"

#include "initterm.h"

ODINDEBUGCHANNEL(KERNEL32-PROFILE)


// -------------------------
// WINE compatibility macros
// -------------------------

#ifndef MAX_PATHNAME_LEN
#define MAX_PATHNAME_LEN 260
#endif

#define SystemHeap               GetProcessHeap()
#define DOSFS_GetFullName(a,b,c) strcpy(c,a)


typedef struct tagPROFILEKEY
{
    char                  *name;
    char                  *value;
    struct tagPROFILEKEY  *next;
} PROFILEKEY;

typedef struct tagPROFILESECTION
{
    char                       *name;
    struct tagPROFILEKEY       *key;
    struct tagPROFILESECTION   *next;
} PROFILESECTION;


typedef struct
{
    BOOL             changed;
    PROFILESECTION  *section;
    char            *filename; //first open name
    char            *fullname; //name with path
    time_t           mtime;
} PROFILEROOT;



#define N_CACHED_PROFILES 10

/* Cached profile files */
static PROFILEROOT *MRUProfile[N_CACHED_PROFILES]={NULL};

#define CurProfile (MRUProfile[0])

/* ODIN.INI profile content */
BOOL OdinProfileChanged = FALSE;
static PROFILESECTION *PROFILE_OdinProfile = NULL;

#define PROFILE_MAX_LINE_LEN   1024
#define WINININAME "WIN.INI"

/* Odin profile: the profile file being used */
static char PROFILE_OdinIniUsed[MAX_PATHNAME_LEN] = "";

/* Check for comments in profile */
#define IS_ENTRY_COMMENT(str)  ((str)[0] == ';')

static LPCWSTR wininiW = NULL;

static CRITICAL_SECTION PROFILE_CritSect;


/***********************************************************************
 *           PROFILE_CopyEntry
 *
 * Copy the content of an entry into a buffer, removing quotes, and possibly
 * translating environment variables.
 */
static void PROFILE_CopyEntry( char *buffer,
                               const char *value,
                               int len,
                               int handle_env )
{
    char quote = '\0';
    const char *p;

    if ((*value == '\'') || (*value == '\"'))
    {
        if (value[1] && (value[strlen(value)-1] == *value)) quote = *value++;
    }

    if (!handle_env)
    {
        lstrcpynA( buffer, value, len );
        if (quote && (len >= strlen(value))) buffer[strlen(buffer)-1] = '\0';
        return;
    }

    for (p = value; (*p && (len > 1)); *buffer++ = *p++, len-- )
    {
        if ((*p == '$') && (p[1] == '{'))
        {
            char env_val[1024];
            const char *env_p;
            const char *p2 = strchr( p, '}' );
            if (!p2) continue;  /* ignore it */
            lstrcpynA(env_val, p + 2, MIN( sizeof(env_val), (int)(p2-p)-1 ));
            if ((env_p = getenv( env_val )) != NULL)
            {
                lstrcpynA( buffer, env_p, len );
                buffer += strlen( buffer );
                len -= strlen( buffer );
            }
            p = p2 + 1;
        }
    }
    *buffer = '\0';
}


/***********************************************************************
 *           PROFILE_Save
 *
 * Save a profile tree to a file.
 */
static void PROFILE_Save( FILE *file, PROFILESECTION *section )
{
    PROFILEKEY *key;

    for ( ; section; section = section->next)
    {
        if (section->name) fprintf( file, "\n[%s]\n", section->name );
        for (key = section->key; key; key = key->next)
        {
            fprintf( file, "%s", key->name );
            if (key->value) fprintf( file, "=%s", key->value );
            fprintf( file, "\n" );
        }
    }
}


/***********************************************************************
 *           PROFILE_Free
 *
 * Free a profile tree.
 */
static void PROFILE_Free( PROFILESECTION *section )
{
    PROFILESECTION *next_section;
    PROFILEKEY *key, *next_key;

    for ( ; section; section = next_section)
    {
        if (section->name) HeapFree( SystemHeap, 0, section->name );
        for (key = section->key; key; key = next_key)
        {
            next_key = key->next;
            if (key->name) HeapFree( SystemHeap, 0, key->name );
            if (key->value) HeapFree( SystemHeap, 0, key->value );
            HeapFree( SystemHeap, 0, key );
        }
        next_section = section->next;
        HeapFree( SystemHeap, 0, section );
    }
}

static int
PROFILE_isspace(char c) {
   if (isspace(c)) return 1;
   if (c=='\r' || c==0x1a) return 1;
   /* CR and ^Z (DOS EOF) are spaces too  (found on CD-ROMs) */
   return 0;
}


/***********************************************************************
 *           PROFILE_Load
 *
 * Load a profile tree from a file.
 */
static PROFILESECTION *PROFILE_Load( FILE *file )
{
    char buffer[PROFILE_MAX_LINE_LEN];
    char *p, *p2;
    int line = 0;
    PROFILESECTION *section, *first_section;
    PROFILESECTION **next_section;
    PROFILEKEY *key, *prev_key, **next_key;

    if (file == NULL)
        return NULL;

    first_section = (PROFILESECTION *)HEAP_xalloc( SystemHeap, 0, sizeof(*section) );
    first_section->name = NULL;
    first_section->key  = NULL;
    first_section->next = NULL;
    next_section = &first_section->next;
    next_key     = &first_section->key;
    prev_key     = NULL;

    while (fgets( buffer, PROFILE_MAX_LINE_LEN, file ))
    {
        line++;
        p = buffer;
        while (*p && PROFILE_isspace(*p)) p++;
        if (*p == '[')  /* section start */
        {
            if (!(p2 = strrchr( p, ']' )))
            {
                dprintf(("Kernel32:Profile:Invalid section header at line %d: '%s'\n",
           line, p ));
            }
            else
            {
                *p2 = '\0';
                p++;
                section = (PROFILESECTION*)HEAP_xalloc( SystemHeap, 0, sizeof(*section) );
                section->name = HEAP_strdupA( SystemHeap, 0, p );
                section->key  = NULL;
                section->next = NULL;
                *next_section = section;
                next_section  = &section->next;
                next_key      = &section->key;
                prev_key      = NULL;

                dprintf2(("Kernel32:Profile:New section: '%s'\n",section->name));

                continue;
            }
        }

        p2=p+strlen(p) - 1;
        while ((p2 > p) && ((*p2 == '\n') || PROFILE_isspace(*p2))) *p2--='\0';

        if ((p2 = strchr( p, '=' )) != NULL)
        {
            char *p3 = p2 - 1;
            while ((p3 > p) && PROFILE_isspace(*p3)) *p3-- = '\0';
            *p2++ = '\0';
            while (*p2 && PROFILE_isspace(*p2)) p2++;
        }

        if(*p || !prev_key || *prev_key->name)
          {
           key = (PROFILEKEY*)HEAP_xalloc( SystemHeap, 0, sizeof(*key) );
           key->name  = HEAP_strdupA( SystemHeap, 0, p );
           key->value = p2 ? HEAP_strdupA( SystemHeap, 0, p2 ) : NULL;
           key->next  = NULL;
           *next_key  = key;
           next_key   = &key->next;
           prev_key   = key;

           dprintf2(("Kernel32:Profile:New key: name='%s', value='%s'\n",key->name,key->value?key->value:"(none)"));
          }
    }
    return first_section;
}


/***********************************************************************
 *           PROFILE_DeleteSection
 *
 * Delete a section from a profile tree.
 */
static BOOL PROFILE_DeleteSection( PROFILESECTION **section, LPCSTR name )
{
    while (*section)
    {
        if ((*section)->name && !strcasecmp( (*section)->name, name ))
        {
            PROFILESECTION *to_del = *section;
            *section = to_del->next;
            to_del->next = NULL;
            PROFILE_Free( to_del );
            return TRUE;
        }
        section = &(*section)->next;
    }
    return FALSE;
}

/***********************************************************************
 *           PROFILE_DeleteKey
 *
 * Delete a key from a profile tree.
 */
static BOOL PROFILE_DeleteKey( PROFILESECTION **section,
                LPCSTR section_name, LPCSTR key_name )
{
    while (*section)
    {
        if ((*section)->name && !strcasecmp( (*section)->name, section_name ))
        {
            PROFILEKEY **key = &(*section)->key;
            while (*key)
            {
                if (!strcasecmp( (*key)->name, key_name ))
                {
                    PROFILEKEY *to_del = *key;
                    *key = to_del->next;
                    if (to_del->name) HeapFree( SystemHeap, 0, to_del->name );
                    if (to_del->value) HeapFree( SystemHeap, 0, to_del->value);
                    HeapFree( SystemHeap, 0, to_del );
                    return TRUE;
                }
                key = &(*key)->next;
            }
        }
        section = &(*section)->next;
    }
    return FALSE;
}

/***********************************************************************
 *           PROFILE_DeleteAllKeys
 *
 * Delete all keys from a profile tree.
 */
void PROFILE_DeleteAllKeys( LPCSTR section_name)
{
    PROFILESECTION **section= &CurProfile->section;
    while (*section)
    {
        if ((*section)->name && !strcasecmp( (*section)->name, section_name ))
        {
            PROFILEKEY **key = &(*section)->key;
            while (*key)
            {
                PROFILEKEY *to_del = *key;
		*key = to_del->next;
		if (to_del->name) HeapFree( GetProcessHeap(), 0, to_del->name );
		if (to_del->value) HeapFree( GetProcessHeap(), 0, to_del->value);
		HeapFree( GetProcessHeap(), 0, to_del );
		CurProfile->changed =TRUE;
            }
        }
        section = &(*section)->next;
    }
}

/***********************************************************************
 *           PROFILE_Find
 *
 * Find a key in a profile tree, optionally creating it.
 */
static PROFILEKEY *PROFILE_Find( PROFILESECTION **section,
                                 const char *section_name,
                                 const char *key_name, int create )
{
    while (*section)
    {
        if ((*section)->name && !strcasecmp( (*section)->name, section_name ))
        {
            PROFILEKEY **key = &(*section)->key;
            while (*key)
            {
                if (!strcasecmp( (*key)->name, key_name )) return *key;
                key = &(*key)->next;
            }
            if (!create) return NULL;
            *key = (PROFILEKEY*)HEAP_xalloc( SystemHeap, 0, sizeof(PROFILEKEY) );
            (*key)->name  = HEAP_strdupA( SystemHeap, 0, key_name );
            (*key)->value = NULL;
            (*key)->next  = NULL;
            return *key;
        }
        section = &(*section)->next;
    }
    if (!create) return NULL;
    *section = (PROFILESECTION*)HEAP_xalloc( SystemHeap, 0, sizeof(PROFILESECTION) );
    (*section)->name = HEAP_strdupA( SystemHeap, 0, section_name );
    (*section)->next = NULL;
    (*section)->key  = (tagPROFILEKEY*)HEAP_xalloc( SystemHeap, 0, sizeof(PROFILEKEY) );
    (*section)->key->name  = HEAP_strdupA( SystemHeap, 0, key_name );
    (*section)->key->value = NULL;
    (*section)->key->next  = NULL;
    return (*section)->key;
}


/***********************************************************************
 *           PROFILE_FlushFile
 *
 * Flush the current profile to disk if changed.
 */
static BOOL PROFILE_FlushFile(void)
{
    FILE *file = NULL;
    struct stat buf;

    if(!CurProfile)
    {
        dprintf(("Kernel32:Profile:No current profile!\n"));
        return FALSE;
    }

    // not changed, return immediately
    if (!CurProfile->changed)
      return TRUE;

    // try to open file
    file = fopen(CurProfile->fullname, "w");
    if (!file)
    {
        dprintf(("Kernel32:Profile:could not save profile file %s\n", CurProfile->fullname));
        return FALSE;
    }

    dprintf(("Kernel32:Profile:Saving %s\n", CurProfile->fullname ));
    PROFILE_Save( file, CurProfile->section );
    fclose( file );
    CurProfile->changed = FALSE;
    if(!stat(CurProfile->fullname,&buf))
       CurProfile->mtime=buf.st_mtime;
    return TRUE;
}


/***********************************************************************
 *           PROFILE_ReleaseFile
 *
 * Flush the current profile to disk and remove it from the cache.
 */
static void PROFILE_ReleaseFile(void)
{
    PROFILE_FlushFile();
    PROFILE_Free( CurProfile->section );
    if (CurProfile->filename) HeapFree( SystemHeap, 0, CurProfile->filename );
    if (CurProfile->fullname) HeapFree(SystemHeap,0,CurProfile->fullname);
    CurProfile->changed   = FALSE;
    CurProfile->section   = NULL;
    CurProfile->filename  = NULL;
    CurProfile->fullname  = NULL;
    CurProfile->mtime     = 0;
}


/***********************************************************************
 *           PROFILE_Open
 *
 * Open a profile file, checking the cached file first.
 */
static BOOL PROFILE_Open( LPCSTR filename )
{
    FILE *file = NULL;
    int i,j;
    struct stat buf;
    PROFILEROOT *tempProfile;

    if (!filename || filename[0] == 0) return FALSE;

    /* First time around */

    if(!CurProfile)
       for(i=0;i<N_CACHED_PROFILES;i++)
         {
          MRUProfile[i]= (PROFILEROOT*)HEAP_xalloc( SystemHeap, 0, sizeof(PROFILEROOT) );
          MRUProfile[i]->changed=FALSE;
          MRUProfile[i]->section=NULL;
          MRUProfile[i]->filename=NULL;
          MRUProfile[i]->fullname=NULL;
          MRUProfile[i]->mtime=0;
         }

    /* Check for a match */

    for(i=0;i<N_CACHED_PROFILES;i++)
      {
       if (MRUProfile[i]->filename && (!stricmp(filename,MRUProfile[i]->filename) || !stricmp(filename,MRUProfile[i]->fullname)))
         {
          if(i)
            {
             PROFILE_FlushFile();
             tempProfile=MRUProfile[i];
             for(j=i;j>0;j--)
                MRUProfile[j]=MRUProfile[j-1];
             CurProfile=tempProfile;
            }
          if(!stat(CurProfile->fullname,&buf) && CurProfile->mtime==buf.st_mtime)
             dprintf(("Kernel32:Profile:(%s): already opened (mru=%d)\n",
                              filename, i ));
          else
              dprintf(("Kernel32:Profile:(%s): already opened, needs refreshing (mru=%d)\n",
                             filename, i ));
     return TRUE;
         }
      }

    /* Rotate the oldest to the top to be replaced */

    if(i==N_CACHED_PROFILES)
      {
       tempProfile=MRUProfile[N_CACHED_PROFILES-1];
       for(i=N_CACHED_PROFILES-1;i>0;i--)
          MRUProfile[i]=MRUProfile[i-1];
       CurProfile=tempProfile;
      }

    /* Flush the profile */

    if(CurProfile->filename) PROFILE_ReleaseFile();

    CurProfile->filename  = HEAP_strdupA(SystemHeap,0,filename);

    /* check for path */

    if ((!strchr( filename,'/') &&
         !strchr( filename,'\\')) ||
        !strchr( filename,':'))
    {
      char fullname[MAX_PATHNAME_LEN];

      GetWindowsDirectoryA(fullname,sizeof(fullname));
      strcat(fullname,"\\");
      strcat(fullname,filename);
      CurProfile->fullname  = HEAP_strdupA(SystemHeap,0,fullname);
    } else CurProfile->fullname  = HEAP_strdupA(SystemHeap,0,filename);

    file = fopen(CurProfile->fullname,"r");
    if (file)
    {
      dprintf(("Kernel32:Profile:(%s): found it in %s\n",
               filename, CurProfile->fullname ));

        CurProfile->section = PROFILE_Load( file );
        fclose( file );
        if(!stat(CurProfile->fullname,&buf))
           CurProfile->mtime=buf.st_mtime;
    }
    else
    {
        /* Does not exist yet, we will create it in PROFILE_FlushFile */
        dprintf(("Kernel32:Profile:profile file %s not found\n", CurProfile->fullname ));
    }
    return TRUE;
}


/***********************************************************************
 *           PROFILE_GetSection
 *
 * Returns all keys of a section.
 * If return_values is TRUE, also include the corresponding values.
 */
static INT PROFILE_GetSection( PROFILESECTION *section, LPCSTR section_name,
                LPSTR buffer, UINT len, BOOL handle_env,
                BOOL return_values )
{
    PROFILEKEY *key;
    while (section)
    {
        if (section->name && !strcasecmp( section->name, section_name ))
        {
            UINT oldlen = len;
            for (key = section->key; key; key = key->next)
            {
                if (len <= 2) break;
                if (!*key->name) continue;  /* Skip empty lines */
                if (IS_ENTRY_COMMENT(key->name)) continue;  /* Skip comments */
                PROFILE_CopyEntry( buffer, key->name, len - 1, handle_env );

#ifdef __WIN32OS2__
                /* CW: This fixes a crash in Windows Media Player setup where GetPrivateProfileSectionA() is called with len=5
                   which is not enough for holding any key name. */
                if(strlen(key->name)>len-2) {
                  buffer += len-1;
                  len=1; /* Causes the funktion to end with a return code of len-2 */
                  break;
                }
                /*****************************************/
#endif

                len -= strlen(buffer) + 1;
                buffer += strlen(buffer) + 1;
      if (return_values && key->value) {
         buffer[-1] = '=';
         PROFILE_CopyEntry ( buffer,
            key->value, len - 1, handle_env );

#ifdef __WIN32OS2__
                  /* CW: This is the same as above for the keyname. I don't have a crashing app to prove this code but
                     expect the same problem as with the key name if the len is to short. */
                  if(strlen(key->value)>len-2) {
                    buffer += len;
                    len=1;
                    break;
                  }
                  /*****************************************/
#endif

         len -= strlen(buffer) + 1;
         buffer += strlen(buffer) + 1;
                }
            }
            *buffer = '\0';
            if (len <= 1)
                /*If either lpszSection or lpszKey is NULL and the supplied
                  destination buffer is too small to hold all the strings,
                  the last string is truncated and followed by two null characters.
                  In this case, the return value is equal to cchReturnBuffer
                  minus two. */
            {
      buffer[-1] = '\0';
                return oldlen - 2;
            }
            return oldlen - len;
        }
        section = section->next;
    }
    buffer[0] = buffer[1] = '\0';
    return 0;
}


static INT PROFILE_GetSectionNames( LPSTR buffer, UINT len )
{
    LPSTR buf = buffer;
    WORD l, cursize = 0;
    PROFILESECTION *section;

    for (section = CurProfile->section; section; section = section->next)
   if (section->name) {
       l = strlen(section->name);
       cursize += l+1;
       if (cursize > len+1)
      return len-2;

       strcpy(buf, section->name);
       buf += l+1;
   }

    *buf=0;
    buf++;
    return buf-buffer;
}


/***********************************************************************
 *           PROFILE_GetString
 *
 * Get a profile string.
 */
static INT PROFILE_GetString( LPCSTR section, LPCSTR key_name,
               LPCSTR def_val, LPSTR buffer, UINT len )
{
    PROFILEKEY *key = NULL;

    if (!def_val) def_val = "";
    if (key_name && key_name[0])
    {
        key = PROFILE_Find( &CurProfile->section, section, key_name, FALSE );
        PROFILE_CopyEntry( buffer, (key && key->value) ? key->value : def_val,
                           len, FALSE );
        dprintf(("Kernel32:Profile:('%s','%s','%s'): returning '%s'\n",
                         section, key_name, def_val, buffer ));
        return strlen( buffer );
    }
    if (section && section[0])
        return PROFILE_GetSection(CurProfile->section, section, buffer, len,
            FALSE, FALSE);
    /* undocumented; both section and key_name are NULL */
    return PROFILE_GetSectionNames(buffer, len);
}


/***********************************************************************
 *           PROFILE_SetString
 *
 * Set a profile string.
 */
static BOOL PROFILE_SetString( LPCSTR section_name, LPCSTR key_name,
                LPCSTR value )
{
    if (!key_name)  /* Delete a whole section */
    {
        dprintf(("Kernel32:Profile:('%s')\n", section_name));
        CurProfile->changed |= PROFILE_DeleteSection( &CurProfile->section,
                                                      section_name );
        return TRUE;         /* Even if PROFILE_DeleteSection() has failed,
                                this is not an error on application's level.*/
    }
    else if (!value)  /* Delete a key */
    {
        dprintf(("Kernel32:Profile:('%s','%s')\n",
                         section_name, key_name ));
        CurProfile->changed |= PROFILE_DeleteKey( &CurProfile->section,
                                                  section_name, key_name );
        return TRUE;          /* same error handling as above */
    }
    else  /* Set the key value */
    {
        PROFILEKEY *key = PROFILE_Find( &CurProfile->section, section_name,
                                        key_name, TRUE );
        dprintf(("Kernel32:Profile:('%s','%s','%s'): \n",
                         section_name, key_name, value ));
        if (!key) return FALSE;
        if (key->value)
        {
            if (!strcmp( key->value, value ))
            {
                dprintf(("Kernel32:Profile:  no change needed\n" ));
                return TRUE;  /* No change needed */
            }
            dprintf(("Kernel32:Profile:  replacing '%s'\n", key->value ));
            HeapFree( SystemHeap, 0, key->value );
        }
        else dprintf(("Kernel32:Profile:  creating key\n" ));
        key->value = HEAP_strdupA( SystemHeap, 0, value );
        CurProfile->changed = TRUE;
    }
    return TRUE;
}


/***********************************************************************
 *           PROFILE_GetOdinIniString
 *
 * Get a config string from the odin.ini file.
 */
int WIN32API PROFILE_GetOdinIniString(LPCSTR section, LPCSTR key_name,
                                      LPCSTR def, LPSTR buffer, UINT len)
{
    int  ret;

    EnterCriticalSection( &PROFILE_CritSect );

    if (key_name)
    {
        PROFILEKEY *key = PROFILE_Find(&PROFILE_OdinProfile, section, key_name, FALSE);
        PROFILE_CopyEntry( (char*)buffer, (key && key->value) ? key->value : def,
                           len, TRUE );
        dprintf(("Kernel32:Odin profile:('%s','%s','%s'): returning '%s'\n",
                         section, key_name, def, buffer ));
        ret = strlen( buffer );
    }
    else
    {
        ret = PROFILE_GetSection( PROFILE_OdinProfile, section, (char*)buffer, len, TRUE, FALSE );
    }
    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}

extern "C" {

int WIN32API PROFILE_SetOdinIniString(LPCSTR section_name, LPCSTR key_name,
                                      LPCSTR value)
{
    int  ret;

    EnterCriticalSection( &PROFILE_CritSect );

    if (!key_name)  /* Delete a whole section */
    {
        dprintf(("Kernel32:Odin profile:('%s')\n", section_name));
        OdinProfileChanged |= PROFILE_DeleteSection( &PROFILE_OdinProfile,
                                                      section_name );
        ret = TRUE;         /* Even if PROFILE_DeleteSection() has failed,
                               this is not an error on application's level.*/
    }
    else if (!value)  /* Delete a key */
    {
        dprintf(("Kernel32:Odin profile:('%s','%s')\n", section_name, key_name ));
        OdinProfileChanged |= PROFILE_DeleteKey( &PROFILE_OdinProfile,
                                                  section_name, key_name );
        ret = TRUE;          /* same error handling as above */
    }
    else  /* Set the key value */
    {
        PROFILEKEY *key = PROFILE_Find( &PROFILE_OdinProfile, section_name,
                                        key_name, TRUE );
        dprintf(("Kernel32:Odin profile:('%s','%s','%s'): \n",
                         section_name, key_name, value ));
        if (!key) ret = FALSE; else
        {
          if (key->value && !strcmp(key->value,value))
          {
            dprintf(("Kernel32:Odin profile:  no change needed\n" ));
            /* No change needed */
          } else
          {
            if (key->value)
            {
              dprintf(("Kernel32:Odin profile:  replacing '%s'\n", key->value ));
              HeapFree( SystemHeap, 0, key->value );
            }
            else dprintf(("Kernel32:Odin profile:  creating key\n" ));
            key->value = HEAP_strdupA( SystemHeap, 0, value );
            OdinProfileChanged = TRUE;
          }
          ret = TRUE;
        }
    }

    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}


/***********************************************************************
 *           PROFILE_GetOdinIniInt
 *
 * Get a config integer from the odin.ini file.
 */
int WIN32API PROFILE_GetOdinIniInt(LPCSTR section, LPCSTR key_name,
                                   int def )
{
    char buffer[20];
    char *p;
    long result;
    PROFILEKEY *key;
    int  ret;

    EnterCriticalSection( &PROFILE_CritSect );

    key = PROFILE_Find( &PROFILE_OdinProfile, section, key_name, FALSE );
    if (!key || !key->value) {
       ret = def;
    } else {
       PROFILE_CopyEntry( buffer, key->value, sizeof(buffer), TRUE );
       result = strtol( buffer, &p, 0 );
       ret = (p == buffer) ? 0  /* No digits at all */ : (int)result;
    }

    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}


int WIN32API PROFILE_SetOdinIniInt(LPCSTR section, LPCSTR key_name, int value )
{
    char buffer[20];

    sprintf(buffer,"%d",value);
    return PROFILE_SetOdinIniString(section,key_name,buffer);
}

/******************************************************************************
 *
 *   int  PROFILE_EnumerateOdinIniSection(
 *     char const  *section,  #Name of the section to enumerate
 *     void  (*cbfn)(char const *key, char const *value, void *user),
 *                            # Address of the callback function
 *     void  *user )          # User-specified pointer.
 *
 *   For each entry in a section in the wine.conf file, this function will
 *   call the specified callback function, informing it of each key and
 *   value.  An optional user pointer may be passed to it (if this is not
 *   needed, pass NULL through it and ignore the value in the callback
 *   function).
 *
 *   The callback function must accept three parameters:
 *      The name of the key (char const *)
 *      The value of the key (char const *)
 *      A user-specified parameter (void *)
 *   Note that the first two are char CONST *'s, not char *'s!  The callback
 *   MUST not modify these strings!
 *
 *   The return value indicates the number of times the callback function
 *   was called.
 */
int PROFILE_EnumerateOdinIniSection(LPCSTR section,PVOID (*cbfn)(LPCSTR, LPCSTR, PVOID),PVOID userptr)
{
    PROFILESECTION  *scansect;
    PROFILEKEY  *scankey;
    int  calls = 0;

    EnterCriticalSection( &PROFILE_CritSect );

    /* Search for the correct section */
    for(scansect = PROFILE_OdinProfile; scansect; scansect = scansect->next) {
   if(scansect->name && !strcasecmp(scansect->name, section)) {

       /* Enumerate each key with the callback */
       for(scankey = scansect->key; scankey; scankey = scankey->next) {

      /* Ignore blank entries -- these shouldn't exist, but let's
         be extra careful */
      if(scankey->name[0]) {
          cbfn(scankey->name, scankey->value, userptr);
          ++calls;
      }
       }

       break;
   }
    }
    LeaveCriticalSection( &PROFILE_CritSect );

    return calls;
}


/******************************************************************************
 *
 *   int  PROFILE_GetOdinIniBool(
 *      char const  *section,
 *      char const  *key_name,
 *      int  def )
 *
 *   Reads a boolean value from the odin.ini file.  This function attempts to
 *   be user-friendly by accepting 'n', 'N' (no), 'f', 'F' (false), or '0'
 *   (zero) for false, 'y', 'Y' (yes), 't', 'T' (true), or '1' (one) for
 *   true.  Anything else results in the return of the default value.
 *
 *   This function uses 1 to indicate true, and 0 for false.  You can check
 *   for existence by setting def to something other than 0 or 1 and
 *   examining the return value.
 */
int WIN32API PROFILE_GetOdinIniBool(LPCSTR section,
                                    LPCSTR key_name,
                                    int def)
{
    char  key_value[2];
    int  retval;

    PROFILE_GetOdinIniString(section, key_name, "~", key_value, 2);

    switch(key_value[0]) {
    case 'n':
    case 'N':
    case 'f':
    case 'F':
    case '0':
       retval = 0;
       break;

    case 'y':
    case 'Y':
    case 't':
    case 'T':
    case '1':
       retval = 1;
       break;

    default:
       retval = def;
    }

    dprintf(("Kernel32:Odin profile:(\"%s\", \"%s\", %s), "
          "[%c], ret %s.\n", section, key_name,
          def ? "TRUE" : "FALSE", key_value[0],
          retval ? "TRUE" : "FALSE"));

    return retval;
}


int WIN32API PROFILE_SetOdinIniBool(LPCSTR section, LPCSTR key_name, int value)
{
  return PROFILE_SetOdinIniString(section,key_name,value ? "1":"0");
}

int WINAPI PROFILE_Initialize (void)
{
    InitializeCriticalSection( &PROFILE_CritSect );
    MakeCriticalSectionGlobal( &PROFILE_CritSect );
    return 1;
}

/***********************************************************************
 *           LoadOdinIni
 *
 * Load the odin.ini file.
 */
int WINAPI PROFILE_LoadOdinIni()
{
    char buffer[MAX_PATHNAME_LEN];
    const char *p;
    FILE *f;

    if ( (p = getenv( "ODIN_INI" )) && (f = fopen( p, "r" )) )
    {
        PROFILE_OdinProfile = PROFILE_Load( f );
        fclose( f );
        strncpy(PROFILE_OdinIniUsed,p,MAX_PATHNAME_LEN);
        PROFILE_OdinIniUsed[MAX_PATHNAME_LEN-1] = 0;
    }
    else
    {
        strcpy(PROFILE_OdinIniUsed, kernel32Path);
        strcat(PROFILE_OdinIniUsed, ODINININAME);
        if ((f = fopen(PROFILE_OdinIniUsed, "r")))
        {
            PROFILE_OdinProfile = PROFILE_Load(f);
            fclose(f);
        }
    }

    dprintf(("Kernel32: Odin ini loaded: %s",PROFILE_OdinIniUsed));

    return 1;
}

static BOOL fDisableOdinIni = FALSE;

void WIN32API DisableOdinIni()
{
    fDisableOdinIni = TRUE;
}

int WIN32API PROFILE_SaveOdinIni()
{
  int ret;
  FILE *file;

  if(fDisableOdinIni) return TRUE;

  EnterCriticalSection( &PROFILE_CritSect );

  if (OdinProfileChanged)
  {
    file = fopen(PROFILE_OdinIniUsed,"w");
    if (!file)
    {
        dprintf(("Kernel32:Odin profile:could not save profile file %s\n", PROFILE_OdinIniUsed));
        ret = FALSE;
    } else
    {
      dprintf(("Kernel32:Odin profile:Saving %s\n", PROFILE_OdinIniUsed ));
      PROFILE_Save( file, PROFILE_OdinProfile );
      fclose( file );
      OdinProfileChanged = FALSE;
      ret = TRUE;
    }
  }

  LeaveCriticalSection( &PROFILE_CritSect );

  return ret;
}

#if 0
/***********************************************************************
 *           PROFILE_UsageOdinIni
 *
 * Explain the wine.ini file to those who don't read documentation.
 * Keep below one screenful in length so that error messages above are
 * noticed.
 */
void PROFILE_UsageOdinIni(void)
{
    MESSAGE("Perhaps you have not properly edited or created "
   "your Odin configuration file.\n");
    MESSAGE("This is either %s or $HOME%s\n",WINE_INI_GLOBAL,PROFILE_WineIniName);
    MESSAGE("  or it is determined by the -config option or from\n"
        "  the WINE_INI environment variable.\n");
    if (*PROFILE_WineIniUsed)
   MESSAGE("Wine has used %s as configuration file.\n", PROFILE_WineIniUsed);
    /* RTFM, so to say */
}
#endif

/***********************************************************************
 *           PROFILE_GetStringItem
 *
 *  Convenience function that turns a string 'xxx, yyy, zzz' into
 *  the 'xxx\0 yyy, zzz' and returns a pointer to the 'yyy, zzz'.
 */
char* PROFILE_GetStringItem( char* start )
{
    char* lpchX, *lpch;

    for (lpchX = start, lpch = NULL; *lpchX != '\0'; lpchX++ )
    {
        if( *lpchX == ',' )
        {
            if( lpch ) *lpch = '\0'; else *lpchX = '\0';
            while( *(++lpchX) )
                if( !PROFILE_isspace(*lpchX) ) return lpchX;
        }
   else if( PROFILE_isspace( *lpchX ) && !lpch ) lpch = lpchX;
        else lpch = NULL;
    }
    if( lpch ) *lpch = '\0';
    return NULL;
}

/********************* API functions **********************************/

/***********************************************************************
 *           GetProfileInt32A   (KERNEL32.264)
 */
UINT WIN32API GetProfileIntA(LPCSTR section, LPCSTR entry, INT def_val)
{
    return GetPrivateProfileIntA( section, entry, def_val, WINININAME );
}

/***********************************************************************
 *           GetProfileInt32W   (KERNEL32.264)
 */
UINT WIN32API GetProfileIntW(LPCWSTR section, LPCWSTR entry, INT def_val)
{
    if (!wininiW) wininiW = HEAP_strdupAtoW( SystemHeap, 0, WINININAME );
    return GetPrivateProfileIntW( section, entry, def_val, wininiW );
}

/***********************************************************************
 *           GetProfileString32A   (KERNEL32.268)
 */
int WIN32API  GetProfileStringA(LPCSTR  section, LPCSTR  entry,
                                LPCSTR  def_val,
                                LPSTR  buffer, UINT  len)
{
    return GetPrivateProfileStringA( section, entry, def_val,
                                     buffer, len, WINININAME );
}

/***********************************************************************
 *           GetProfileString32W   (KERNEL32.269)
 */
int WIN32API  GetProfileStringW(LPCWSTR section, LPCWSTR entry,
                                LPCWSTR def_val, LPWSTR buffer,
                                UINT len)
{
    if (!wininiW) wininiW = HEAP_strdupAtoW( SystemHeap, 0, WINININAME );
    return GetPrivateProfileStringW( section, entry, def_val,
                 buffer, len, wininiW );
}

/***********************************************************************
 *           WriteProfileString32A   (KERNEL32.587)
 */
BOOL WIN32API WriteProfileStringA(LPCSTR  section, LPCSTR  entry,
                                  LPCSTR  string)
{
    return WritePrivateProfileStringA( section, entry, string, WINININAME );
}

/***********************************************************************
 *           WriteProfileString32W   (KERNEL32.588)
 */
BOOL WIN32API WriteProfileStringW(LPCWSTR section, LPCWSTR entry,
                                  LPCWSTR string)
{
    if (!wininiW) wininiW = HEAP_strdupAtoW( SystemHeap, 0, WINININAME );
    return WritePrivateProfileStringW( section, entry, string, wininiW );
}

/***********************************************************************
 *           GetPrivateProfileInt32A   (KERNEL32.251)
 */
UINT WIN32API GetPrivateProfileIntA(LPCSTR section, LPCSTR entry,
                                    INT def_val, LPCSTR filename)
{
    char buffer[20];
    char *p;
    long result;

    GetPrivateProfileStringA( section, entry, "",
                              buffer, sizeof(buffer), filename );
    if (!buffer[0]) return (UINT)def_val;
    result = strtol( buffer, &p, 0 );
    if (p == buffer) return 0;  /* No digits at all */
    return (UINT)result;
}

/***********************************************************************
 *           GetPrivateProfileInt32W   (KERNEL32.252)
 */
UINT WIN32API GetPrivateProfileIntW(LPCWSTR section,
                                    LPCWSTR entry,
                                    INT def_val,
                                    LPCWSTR filename)
{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR entryA    = HEAP_strdupWtoA( GetProcessHeap(), 0, entry );
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    UINT res = GetPrivateProfileIntA(sectionA, entryA, def_val, filenameA);
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, filenameA );
    HeapFree( GetProcessHeap(), 0, entryA );
    return res;
}

/***********************************************************************
 *           GetPrivateProfileString32A   (KERNEL32.255)
 */
INT WIN32API GetPrivateProfileStringA(LPCSTR section,
                                      LPCSTR entry,
                                      LPCSTR def_val,
                                      LPSTR  buffer,
                                      UINT   len,
                                      LPCSTR filename)
{
    int     ret;

    if (!filename)
    filename = WINININAME;

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename )) {
        ret = PROFILE_GetString( section, entry, def_val, buffer, len );
    } else {
       lstrcpynA( buffer, def_val, len );
       ret = strlen( buffer );
    }

    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}

/***********************************************************************
 *           GetPrivateProfileString32W   (KERNEL32.256)
 */
INT WIN32API GetPrivateProfileStringW(LPCWSTR section,
                                      LPCWSTR entry,
                                      LPCWSTR def_val,
                                      LPWSTR  buffer,
                                      UINT    len,
                                      LPCWSTR filename)
{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR entryA    = HEAP_strdupWtoA( GetProcessHeap(), 0, entry );
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    LPSTR def_valA  = HEAP_strdupWtoA( GetProcessHeap(), 0, def_val );
    LPSTR bufferA   = (LPSTR)HeapAlloc( GetProcessHeap(), 0, len );
    INT ret = GetPrivateProfileStringA( sectionA, entryA, def_valA,
                                        bufferA, len, filenameA );
    lstrcpynAtoW( buffer, bufferA, len );
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, entryA );
    HeapFree( GetProcessHeap(), 0, filenameA );
    HeapFree( GetProcessHeap(), 0, def_valA );
    HeapFree( GetProcessHeap(), 0, bufferA);
    return ret;
}

/***********************************************************************
 *           GetPrivateProfileSection32A   (KERNEL32.255)
 */
INT WIN32API GetPrivateProfileSectionA(LPCSTR  section,
                                       LPSTR   buffer,
                                       DWORD   len,
                                       LPCSTR  filename)
{
    int     ret = 0;

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename ))
        ret = PROFILE_GetSection(CurProfile->section, section, buffer, len,
             FALSE, TRUE);

    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}

/***********************************************************************
 *           GetPrivateProfileSection32W   (KERNEL32.256)
 */

INT WIN32API GetPrivateProfileSectionW(LPCWSTR section,
                                       LPWSTR  buffer,
                                       DWORD   len,
                                       LPCWSTR filename)

{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    LPSTR bufferA   = (LPSTR)HeapAlloc( GetProcessHeap(), 0, len );
    INT ret = GetPrivateProfileSectionA( sectionA, bufferA, len,
                                         filenameA );
    MultiByteToWideChar(CP_ACP,0,bufferA,ret,buffer,len);
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, filenameA );
    HeapFree( GetProcessHeap(), 0, bufferA);
    return ret;
}

/***********************************************************************
 *           GetProfileSection32A   (KERNEL32.268)
 */
INT WIN32API GetProfileSectionA(LPCSTR section, LPSTR buffer,
                                DWORD len)
{
    return GetPrivateProfileSectionA( section, buffer, len, WINININAME );
}

/***********************************************************************
 *           GetProfileSection32W   (KERNEL32)
 */
INT WIN32API GetProfileSectionW(LPCWSTR section, LPWSTR buffer,
                                DWORD len)
{
    if (!wininiW) wininiW = HEAP_strdupAtoW( SystemHeap, 0, WINININAME );
    return GetPrivateProfileSectionW( section, buffer, len, wininiW );
}

/***********************************************************************
 *           WritePrivateProfileString32A   (KERNEL32.582)
 */
BOOL WIN32API WritePrivateProfileStringA(LPCSTR section,
                                         LPCSTR entry,
                                         LPCSTR string,
                                         LPCSTR filename)
{
    BOOL ret = FALSE;

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename ))
    {
        if (!section && !entry && !string)
        {
            PROFILE_FlushFile();
            PROFILE_ReleaseFile();  /* always return FALSE in this case */
        }
        else
        {
            ret = PROFILE_SetString( section, entry, string );
            PROFILE_FlushFile();
        }
    }

    LeaveCriticalSection( &PROFILE_CritSect );
    return ret;
}

/***********************************************************************
 *           WritePrivateProfileString32W   (KERNEL32.583)
 */
BOOL WIN32API WritePrivateProfileStringW(LPCWSTR section,
                                         LPCWSTR entry,
                                         LPCWSTR string,
                                         LPCWSTR filename)
{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR entryA    = HEAP_strdupWtoA( GetProcessHeap(), 0, entry );
    LPSTR stringA   = HEAP_strdupWtoA( GetProcessHeap(), 0, string );
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    BOOL res = WritePrivateProfileStringA( sectionA, entryA,
                                           stringA, filenameA );
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, entryA );
    HeapFree( GetProcessHeap(), 0, stringA );
    HeapFree( GetProcessHeap(), 0, filenameA );
    return res;
}

/***********************************************************************
 *           WritePrivateProfileSection32A   (KERNEL32)
 */
BOOL WIN32API WritePrivateProfileSectionA(LPCSTR section,
                                          LPCSTR string,
                                          LPCSTR filename)
{
    BOOL ret = FALSE;
    LPSTR p ;

    dprintf(("WritePrivateProfileSectionA %s %s %s", section, string, filename));

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename )) {
        if (!section && !string)
            PROFILE_ReleaseFile();  /* always return FALSE in this case */
        else if (!string) /* delete the named section*/
        {
	    ret = PROFILE_SetString(section,NULL,NULL);
            PROFILE_FlushFile();
        }
        else {
	    PROFILE_DeleteAllKeys(section);
	    ret = TRUE;
	    while(*string) {
	        LPSTR buf=HEAP_strdupA( GetProcessHeap(), 0, string );
                if((p=strchr( buf, '=')) != NULL){
                    *p='\0';
                    ret = PROFILE_SetString( section, buf, p+1 );

                }
                HeapFree( GetProcessHeap(), 0, buf );
                string += strlen(string)+1;
            }
            PROFILE_FlushFile();
        }
    }

    LeaveCriticalSection( &PROFILE_CritSect );
    return ret;
}

/***********************************************************************
 *           WritePrivateProfileSection32W   (KERNEL32)
 */
BOOL WIN32API WritePrivateProfileSectionW(LPCWSTR section,
                                          LPCWSTR string,
                                          LPCWSTR filename)
{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR stringA   = HEAP_strdupWtoA( GetProcessHeap(), 0, string );
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    BOOL res = WritePrivateProfileSectionA( sectionA, stringA, filenameA );
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, stringA );
    HeapFree( GetProcessHeap(), 0, filenameA );
    return res;
}

/***********************************************************************
 *           WriteProfileSection32A   (KERNEL32.747)
 */
BOOL WIN32API WriteProfileSectionA(LPCSTR section, LPCSTR keys_n_values)
{
    return WritePrivateProfileSectionA( section, keys_n_values, WINININAME);
}

/***********************************************************************
 *           WriteProfileSection32W   (KERNEL32.748)
 */
BOOL WIN32API WriteProfileSectionW(LPCWSTR section, LPCWSTR keys_n_values)
{
   if (!wininiW) wininiW = HEAP_strdupAtoW( SystemHeap, 0, WINININAME);

   return WritePrivateProfileSectionW(section,keys_n_values, wininiW);
}

/***********************************************************************
 *           GetPrivateProfileSectionNames16   (KERNEL.143)
 */
WORD WIN32API GetPrivateProfileSectionNames16(LPSTR  buffer,
                                              WORD   size,
                                              LPCSTR filename)
{
    WORD ret = 0;

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename ))
      ret = PROFILE_GetSectionNames(buffer, size);

    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}

/***********************************************************************
 *           GetPrivateProfileSectionNames32A  (KERNEL32.365)
 */
DWORD WIN32API GetPrivateProfileSectionNamesA(LPSTR  buffer,
                                              DWORD  size,
                                              LPCSTR filename)
{
 return GetPrivateProfileSectionNames16(buffer,size,filename);
}

/***********************************************************************
 *           GetPrivateProfileSectionNames32W  (KERNEL32.366)
 */
DWORD WIN32API GetPrivateProfileSectionNamesW(LPWSTR  buffer,
                                              DWORD   size,
                                              LPCWSTR filename)
{
   LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
   LPSTR bufferA   = (LPSTR)HeapAlloc( GetProcessHeap(), 0, size);

   INT ret = GetPrivateProfileSectionNames16 (bufferA, size, filenameA);
   lstrcpynAtoW( buffer, bufferA, size);
   HeapFree( GetProcessHeap(), 0, bufferA);
   HeapFree( GetProcessHeap(), 0, filenameA );

   return ret;
}

/***********************************************************************
 *           GetPrivateProfileStruct32A (KERNEL32.370)
 */
BOOL WIN32API GetPrivateProfileStructA(LPCSTR section, LPCSTR key,
                                       LPVOID buf,
                                       UINT  len,
                                       LPCSTR  filename)
{
    BOOL ret = FALSE;

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename )) {
        PROFILEKEY *k = PROFILE_Find ( &CurProfile->section, section, key, FALSE);
   if (k) {
      lstrcpynA( (LPSTR)buf, k->value, strlen(k->value));
      ret = TRUE;
   }
    }
    LeaveCriticalSection( &PROFILE_CritSect );

    return FALSE;
}

/***********************************************************************
 *           GetPrivateProfileStruct32W (KERNEL32.543)
 */
BOOL WIN32API GetPrivateProfileStructW(LPCWSTR section,
                                       LPCWSTR key,
                                       LPVOID  buffer,
                                       UINT    len,
                                       LPCWSTR filename)
{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR keyA      = HEAP_strdupWtoA( GetProcessHeap(), 0, key);
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    LPSTR bufferA   = (LPSTR)HeapAlloc( GetProcessHeap(), 0, len );

    INT ret = GetPrivateProfileStructA( sectionA, keyA, bufferA,
                                        len, filenameA );
    lstrcpynAtoW( (LPWSTR)buffer, bufferA, len );
    HeapFree( GetProcessHeap(), 0, bufferA);
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, keyA );
    HeapFree( GetProcessHeap(), 0, filenameA );

    return ret;
}


/***********************************************************************
 *           WritePrivateProfileStruct32A (KERNEL32.744)
 */
BOOL WIN32API WritePrivateProfileStructA(LPCSTR section,
                                         LPCSTR key,
                                         LPVOID buf,
                                         UINT   bufsize,
                                         LPCSTR filename)
{
    BOOL ret = FALSE;

    if (!section && !key && !buf)  /* flush the cache */
        return WritePrivateProfileStringA( NULL, NULL, NULL, filename );

    EnterCriticalSection( &PROFILE_CritSect );

    if (PROFILE_Open( filename ))
    {
        ret = PROFILE_SetString( section, key, (LPCSTR)buf );
        PROFILE_FlushFile();
    }
    LeaveCriticalSection( &PROFILE_CritSect );

    return ret;
}

/***********************************************************************
 *           WritePrivateProfileStruct32W (KERNEL32.544)
 */
BOOL WIN32API WritePrivateProfileStructW(LPCWSTR section,
                                         LPCWSTR key,
                                         LPVOID  buf,
                                         UINT    bufsize,
                                         LPCWSTR filename)
{
    LPSTR sectionA  = HEAP_strdupWtoA( GetProcessHeap(), 0, section );
    LPSTR keyA      = HEAP_strdupWtoA( GetProcessHeap(), 0, key);
    LPSTR filenameA = HEAP_strdupWtoA( GetProcessHeap(), 0, filename );
    INT ret = WritePrivateProfileStructA( sectionA, keyA, buf, bufsize,
                                          filenameA );
    HeapFree( GetProcessHeap(), 0, sectionA );
    HeapFree( GetProcessHeap(), 0, keyA );
    HeapFree( GetProcessHeap(), 0, filenameA );

    return ret;
}

/***********************************************************************
 *           WriteOutProfiles   (KERNEL.315)
 * CB: original: 16 bit function
 *     here: necessary to save open ini files
 */
void WIN32API WriteOutProfiles()
{
    PROFILEROOT *lastCurProfile;
    INT x;

    PROFILE_SaveOdinIni();

    EnterCriticalSection(&PROFILE_CritSect);

    if (CurProfile && CurProfile->filename)
      PROFILE_FlushFile(); //flash current

    if (MRUProfile)
    {
      lastCurProfile = CurProfile;
      for(x = 1;x < N_CACHED_PROFILES;x++)
      {
        if (MRUProfile[x] && MRUProfile[x]->filename)
        {
          CurProfile = MRUProfile[x];
          PROFILE_FlushFile();
        }
      }
      CurProfile = lastCurProfile;
    }
    LeaveCriticalSection(&PROFILE_CritSect);
}


/***********************************************************************
 *           CloseProfileUserMapping   (KERNEL.138)
 */
BOOL WINAPI CloseProfileUserMapping(void)
{
    dprintf(("CloseProfileUserMapping: STUB"));
    return TRUE;
}

BOOL WINAPI OpenProfileUserMapping(void)
{
    dprintf(("OpenProfileUserMapping: STUB"));
    return TRUE;
}

} // extern "C"

