#ifndef __INITDLL_H__
#define __INITDLL_H__

#if (defined(__IBMCPP__) || defined(__IBMC__))
#ifdef __cplusplus
extern "C" {
#endif

#if (__IBMCPP__ == 300) || (__IBMC__ == 300)
void _Optlink __ctordtorInit( void );
#define ctordtorInit()	__ctordtorInit()

void _Optlink __ctordtorTerm( void );
#define ctordtorTerm()	__ctordtorTerm()

#elif (__IBMCPP__ == 360) || (__IBMC__ == 360)
void _Optlink __ctordtorInit( int flag );
#define ctordtorInit()	__ctordtorInit(0)

void _Optlink __ctordtorTerm( int flag );
#define ctordtorTerm()	__ctordtorTerm(0)

#else
#error "Unknown compiler!"
#endif


/*-------------------------------------------------------------------*/
/* _CRT_init is the C run-time environment initialization function.  */
/* It will return 0 to indicate success and -1 to indicate failure.  */
/*-------------------------------------------------------------------*/
int  _Optlink _CRT_init(void);

/*-------------------------------------------------------------------*/
/* _CRT_term is the C run-time environment termination function.     */
/* It only needs to be called when the C run-time functions are      */
/* statically linked.                                                */
/*-------------------------------------------------------------------*/
void _Optlink _CRT_term(void);


#ifdef __cplusplus
}
#endif

#endif


#endif //__INITDLL_H__