/* $Id: kKLFunc.h,v 1.2 2001-09-17 00:11:09 bird Exp $
 *
 * Function overloading functions.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _kKLFuncs_h_
#define _kKLFuncs_h_


/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
BOOL kKLOverload32(ULONG ulFuncAddr, ULONG ulOverloaderAddr);
BOOL kKLRestore32(ULONG ulFuncAddr, ULONG ulOverloaderAddr);
BOOL kKLOverload16(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr);
BOOL kKLRestore16(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr);
BOOL kKLOverload16H(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr);
BOOL kKLRestore16H(ULONG ulFuncFarAddr, ULONG ulOverloaderFarAddr);


#endif

