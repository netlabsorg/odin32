/* $Id: minihll2.c,v 1.1.2.2 2001-08-20 18:47:54 bird Exp $
 *
 * The data file.
 *
 * This is done so in order to get the data first and have lxlite
 * cut off the blanks in the jmp instruction.
 *
 * Hence we put this first a and the minihll.obj file last.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/*
 * Must be in the same segment as the code.
 * This is linked in before the code and hence the code will come last and will
 * save a few bytes because of 4 zero bytes in the last instruction.
 */
#ifdef __WATCOMC__
#pragma data_seg("CODEANDDATA", "CODE")
#endif
#ifdef __IBMC__
#pragma data_seg(CODEANDDATACODE32)
#endif
char szMsg[19] = {"I'm really small!\n"};


/*
 * Define a stack segment.
 */
#ifdef __WATCOMC__
#pragma data_seg("MYSTACK", "STACK")
#endif



