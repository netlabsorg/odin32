/* $Id: main.h,v 1.1 1999-05-24 20:19:13 ktk Exp $ */

/*
 * Wine initialization definitions
 */

#ifndef __WINE_MAIN_H
#define __WINE_MAIN_H

extern void MAIN_Usage(char*);
extern BOOL MAIN_MainInit(void);
extern BOOL MAIN_WineInit( int *argc, char *argv[] );
extern HINSTANCE MAIN_WinelibInit( int *argc, char *argv[] );
extern int MAIN_GetLanguageID(char*lang, char*country, char*charset, char*dialect);
extern BOOL MAIN_ParseDebugOptions(char *options);

extern void MAIN_ParseLanguageOption( char *arg );
extern void MAIN_ParseModeOption( char *arg );

extern BOOL RELAY_Init(void);
extern int RELAY_ShowDebugmsgRelay(const char *func);
extern void* CALL32_Init(void);

extern BOOL THUNK_Init(void);

#endif  /* __WINE_MAIN_H */
