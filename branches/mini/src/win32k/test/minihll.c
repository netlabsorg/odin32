#ifdef __IBMC__
#pragma strings(writeable)
#endif

/*
 * Config
 */
//#define LIBC



/*******************************************************************************
*   External Functions                                                         *
*******************************************************************************/
#ifndef LIBC
unsigned long _System DosPutMessage(unsigned long hFile,
                                    unsigned long cchMsg,
                                    char *  pszMsg);
#else
extern int    _Optlink printf( char * pszMsg, ...);
#endif

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef __WATCOMC__
#pragma data_seg("STACK32", "CODE")
#endif
char szMsg[19] = {"I'm really small!\n"};

#ifdef __WATCOMC__
#pragma data_seg("MYSTACK", "STACK")
int dummy;
#endif


#ifdef __IBMC__
#pragma entry(minihll)
#endif


/*
 * Main entry potin etc.
 */
void _Optlink minihll(void)
{
    #ifndef LIBC
    DosPutMessage(0, 18, szMsg);
    #else
    printf(szMsg);
    #endif
}


