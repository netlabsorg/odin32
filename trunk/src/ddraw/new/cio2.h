/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de) */
/* Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#ifndef _CIO2_H_
#define _CIO2_H_

extern "C"
{
 int io_init1();
 int io_init2(short);
 int io_exit1();

 char c_inb1(short);
 short c_inw1(short);
 long c_inl1(short);
 void c_outb1(short,char);
 void c_outw1(short,short);
 void c_outl1(short,long);
 int psw();
}
#endif
