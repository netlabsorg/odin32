/* $Id: kKLFunc.h,v 1.3 2002-12-16 02:25:06 bird Exp $
 *
 * Function overloading functions.
 *
 * Copyright (c) 2001-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _kKLFuncs_h_
#define _kKLFuncs_h_


/*******************************************************************************
*   Functions                                                                  *
*******************************************************************************/
KBOOL kKLOverload32( unsigned long ulFuncAddr,    unsigned long ulOverloaderAddr);
KBOOL kKLRestore32(  unsigned long ulFuncAddr,    unsigned long ulOverloaderAddr);
KBOOL kKLOverload16( unsigned long ulFuncFarAddr, unsigned long ulOverloaderFarAddr);
KBOOL kKLRestore16(  unsigned long ulFuncFarAddr, unsigned long ulOverloaderFarAddr);
KBOOL kKLOverload16H(unsigned long ulFuncFarAddr, unsigned long ulOverloaderFarAddr);
KBOOL kKLRestore16H( unsigned long ulFuncFarAddr, unsigned long ulOverloaderFarAddr);


#endif

