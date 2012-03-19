/* $Id: options.h,v 1.8 2001-06-02 08:30:58 sandervl Exp $ */

/*
 * Command-line options.
 *
 * Copyright 1994 Alexandre Julliard
 * Copyright 1999 Christoph Bratschi
 */

#ifndef __WINE_OPTIONS_H
#define __WINE_OPTIONS_H

//#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif

  /* Supported languages */
  /* When adding a new language look at ole/ole2nls.c
   * for the LANG_Xx name to choose, and uncomment there
   * the proper case line
   */
typedef enum
{   LANG_Xx,  /* Just to ensure value 0 is not used */
    LANG_En,  /* English */
    LANG_Es,  /* Spanish */
    LANG_De,  /* German */
    LANG_No,  /* Norwegian */
    LANG_Fr,  /* French */
    LANG_Fi,  /* Finnish */
    LANG_Da,  /* Danish */
    LANG_Cs,  /* Czech */
    LANG_Eo,  /* Esperanto */
    LANG_It,  /* Italian */
    LANG_Ko,  /* Korean */
    LANG_Hu,  /* Hungarian */
    LANG_Pl,  /* Polish */
    LANG_Pt,  /* Portuguese */
    LANG_Sv,  /* Swedish */
    LANG_Ca,  /* Catalan */
    LANG_Nl,  /* Dutch */
    LANG_Ru,  /* Russian */
    LANG_Wa   /* Walon */
} WINE_LANGUAGE;

typedef struct
{
    const char *name;
    WORD        langid;
} WINE_LANGUAGE_DEF;

extern const WINE_LANGUAGE_DEF Languages[];

/* Supported modes */
typedef enum
{
    MODE_STANDARD,
    MODE_ENHANCED
} WINE_MODE;

struct options
{
    int  *argc;
    char **argv;
    char * desktopGeometry; /* NULL when no desktop */
    char * programName;     /* To use when loading resources */
    char * argv0;           /* argv[0] of Wine process */
    char  *dllFlags;        /* -dll flags (hack for Winelib support) */
    int    usePrivateMap;
    int    useFixedMap;
    int    synchronous;     /* X synchronous mode */
    int    backingstore;    /* Use backing store */
    short  cmdShow;
    int    debug;
    int    failReadOnly;    /* Opening a read only file will fail
                               if write access is requested */
    WINE_MODE mode;         /* Start Wine in selected mode
                               (standard/enhanced) */
    WINE_LANGUAGE language; /* Current language */
    int    managed;         /* Managed windows */
    int    perfectGraphics; /* Favor correctness over speed for graphics */
    int    noDGA;           /* Disable XFree86 DGA extensions */
    char * configFileName;  /* Command line config file */
    int screenDepth;
};

extern struct options Options;

/* Odin profile functions */
/* Odin profile name in KERNEL32.DLL directory */
#define ODINININAME        "ODIN.INI"
#define ODINCOLORS         "COLORS"
#define ODINDIRECTORIES    "DEVDIRECTORIES"
#define ODINCUSTOMIZATION  "CUSTOMIZATION"
#define ODINSYSTEM_SECTION "ODINSYSTEM"
#define HIGHMEM_KEY        "HIGHMEM"
#define WINMM_SECTION      "WINMM"
#define DEFVOL_KEY         "DefVolume"

int WINAPI PROFILE_GetOdinIniString(LPCSTR section,LPCSTR key_name,LPCSTR def,LPSTR buffer,UINT len);
int WINAPI PROFILE_SetOdinIniString(LPCSTR section,LPCSTR key_name,LPCSTR value);
int WINAPI PROFILE_GetOdinIniInt(LPCSTR section,LPCSTR key_name,int def);
int WINAPI PROFILE_SetOdinIniInt(LPCSTR section,LPCSTR key_name,int value);
int WINAPI PROFILE_GetOdinIniBool(LPCSTR section,LPCSTR key_name,int def);
int WINAPI PROFILE_SetOdinIniBool(LPCSTR section,LPCSTR key_name,int value);
int WINAPI PROFILE_SaveOdinIni(void);
int WINAPI PROFILE_LoadOdinIni(void);
void WINAPI WriteOutProfiles(void);

//INT WINAPI GetPrivateProfileStringA(LPCSTR section, LPCSTR entry,
//                                          LPCSTR def_val, LPSTR buffer,
//                                          UINT len, LPCSTR filename);

/* Version functions */
extern void VERSION_ParseWinVersion( const char *arg );
extern void VERSION_ParseDosVersion( const char *arg );

#ifdef __cplusplus
}
#endif

#endif  /* __WINE_OPTIONS_H */
