/* $Id: kernel32test.h,v 1.1 1999-11-28 23:10:07 bird Exp $
 *
 * Kernel32Test.
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _KERNEL32TEST_H_
#define _KERNEL32TEST_H_

#ifndef WIN32API
    #if defined(__WATCOMC__) || defined(__WATCOMCPP__)
        #define WIN32API __stdcall
    #else
        #define WIN32API __stdcall
    #endif
#endif

int WIN32API Kernel32TestMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

/*
 * Test functions.
 */
void TestResources(void);


#endif /* _KERNEL32TEST_H_ */
