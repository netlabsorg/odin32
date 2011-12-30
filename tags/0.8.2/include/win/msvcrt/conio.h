/*
 * Console I/O definitions
 *
 * Derived from the mingw header written by Colin Peters.
 * Modified for Wine use by Jon Griffiths and Francois Gouget.
 * This file is in the public domain.
 */
#ifndef __WINE_CONIO_H
#define __WINE_CONIO_H
#define __WINE_USE_MSVCRT


#ifdef __cplusplus
extern "C" {
#endif

char*       MSVCRT__cgets(char*);
int         MSVCRT__cprintf(const char*,...);
int         MSVCRT__cputs(const char*);
int         MSVCRT__cscanf(const char*,...);
int         MSVCRT__getch(void);
int         MSVCRT__getche(void);
int         _kbhit(void);
int         _putch(int);
int         MSVCRT__ungetch(int);

#ifdef _M_IX86
int         _inp(unsigned short);
unsigned long _inpd(unsigned short);
unsigned short _inpw(unsigned short);
int         _outp(unsigned short, int);
unsigned long _outpd(unsigned short, unsigned long);
unsigned short _outpw(unsigned short, unsigned short);
#endif

#ifdef __cplusplus
}
#endif


#ifndef USE_MSVCRT_PREFIX
#define cgets _cgets
#define cprintf _cprintf
#define cputs _cputs
#define cscanf _cscanf
#define getch _getch
#define getche _getche
#define kbhit _kbhit
#define putch _putch
#define ungetch _ungetch
#ifdef _M_IX86
#define inp _inp
#define inpw _inpw
#define outp _outp
#define outpw _outpw
#endif
#endif /* USE_MSVCRT_PREFIX */

#endif /* __WINE_CONIO_H */
