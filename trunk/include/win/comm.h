/* $Id: comm.h,v 1.1 1999-05-24 20:19:09 ktk Exp $ */

#ifndef __WINE_COMM_H
#define __WINE_COMM_H

#define MAX_PORTS   9

struct DosDeviceStruct {
    char *devicename;   /* /dev/cua1 */
    int fd;
    int suspended;
    int unget,xmit;
    int baudrate;
    /* events */
    int commerror, eventmask;
    /* buffers */
    char *inbuf,*outbuf;
    unsigned ibuf_size,ibuf_head,ibuf_tail;
    unsigned obuf_size,obuf_head,obuf_tail;
    /* notifications */
    int wnd, n_read, n_write;
};

extern void COMM_Init(void);

#endif  /* __WINE_COMM_H */
