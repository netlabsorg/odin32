#ifndef __INITDLL_H__
#define __INITDLL_H__

#if (defined(__IBMCPP__) || defined(__IBMC__))
#ifdef __cplusplus
extern "C" {
#endif

void _Optlink __ctordtorInit( void );
void _Optlink __ctordtorTerm( void );

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