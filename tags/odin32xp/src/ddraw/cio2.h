/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de) */
/* Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#ifndef _CIO2_H_
#define _CIO2_H_

extern "C"
{
 int _System io_init1();
 int _System io_init2(short);
 int _System _io_exit1();

 char _System c_inb1(short);
 short _System c_inw1(short);
 long _System c_inl1(short);
 void _System c_outb1(short,char);
 void _System c_outw1(short,short);
 void _System c_outl1(short,long);
 int psw();
}
#endif
