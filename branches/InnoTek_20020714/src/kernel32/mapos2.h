/* $Id: mapos2.h,v 1.3 1999-06-10 19:11:30 phaller Exp $ */

/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de) */
/* Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#ifndef _MAPOS2_H_
#define _MAPOS2_H_

/* This driver provides a (non-1-to-1!) replacement for SMVDD.SYS
 * function 0x81/0x42, which allows to map physical memory into user
 * address space.
 *
 * THE DRIVER WAS SPECIFICALLY CREATE FOR SUPPORT OF XFREE86/OS2.
 *
 * Usage:
 *
 * 1. Open the device "PMAP$".
 *  rc = DosOpen("PMAP$", &hf, &action, 0, FILE_NORMAL,
 *      OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE, (PEAOP2)NULL);
 *
 * 2. To map a memory area:
 *  ParamPkt p;
 *  ULONG len;
 *  DataPkt d;
 *  void* paddr;
 *  p.u.physaddr = 0xb8000; // e.g. video memory
 *  p.size = 2048;      // e.g. 2KB
 *  rc = DosDevIOCtl(hf, XFREE86_PMAP, PMAP_MAP,
 *      (PULONG)&p, sizeof(p), &len,
 *      (PULONG)&d, sizeof(d), &len);
 *  paddr = (void*)d.addr;
 *
 * 3. To unmap the *same* area, return the virtual address you got from
 *    the data packet of the PMAP_MAP ioctl. Don't return the physical
 *    address!
 *
 *  ParamPkt p;
 *  ULONG len;
 *  void* vmaddr;
 *  p.u.vmaddr = (ULONG)paddr;
 *  p.size = 0;
 *  rc = DosDevIOCtl(hf, XFREE86_PMAP, PMAP_UNMAP,
 *      (PULONG)&p, sizeof(p), &len,
 *      NULL,0,NULL);
 *  paddr = (void*)d.addr;
 *
 * 4. Close the driver
 *  rc = DosClose(hf);
 *
 * Notes:
 * 1. Unlike SMVDD.SYS, PMAP$ will automatically unmap all allocated
 *    memory of a process, if the driver is closed. Thus the pointers
 *    obtained from the driver become immediately invalid (Segmentation
 *    violation when used).
 * 2. Unlike SMVDD.SYS, PMAP$ can allocate more than 64K at a time, thus
 *    the interface changed.
 * 3. You can only unmap the whole memory allocated by a single call, not
 *    parts, e.g. allocate 1000-1FFF will give you a pointer to 1000. If
 *    you now return a pointer to 1800 (made by pointer arithmetics) to
 *    the unmap ioctl, in the hope of retaining 1000-17FF, this will fail.
 *    you could however map the 1000-1FFF in two parts of 2K each.
 *    You must submit *the same* pointer to the unmap ioctl, and this pointer
 *    must have been obtained by the same open file (opening PMAP$ twice and
 *    mapping the same memory will return different and incompatible pointers).
 * 4. This driver operates only with 32 bit code, and the pointer returned is
 *    a flat 32 bit pointer.
 * 5. The driver maintains a systemwide table of 64 mapped adresses at a time.
 *    Attempting to map more than 64 regions will fail.
 * 6. Finally a word of warning: Using this function to access memory you
 *    don't know exactly whom it belongs to - by default OS/2 is the owner of
 *    all memory - WILL LIKELY CRASH THE SYSTEM! The author of the driver
 *    is not responsible for any damage or loss of data caused by using this
 *    driver once it is written in CONFIG.SYS.
 */

/* The parameter packet */
struct xf86_pmap_param {
    union {
        ULONG   physaddr;   /* used for MAP, UNMAP2 */
        ULONG   vmaddr;     /* used for UNMAP */
    } u;
    ULONG   size;
};

/* The data packet */
struct xf86_pmap_data {
    ULONG   addr;
    USHORT  sel;
};

/* The device driver category */
#define XFREE86_PMAP    0x76    /* 'v' */

/* The device driver functions */
#define PMAP_RESERVED0  0x40    /* don't use */
#define PMAP_RESERVED1  0x41    /* don't use */
#define PMAP_RESERVED2  0x42    /* don't use */
#define PMAP_RESERVED3  0x43    /* don't use */

#define PMAP_MAP    0x44    /* map by physical addr */
#define PMAP_UNMAP  0x45    /* unmap by virtual addr */
#define PMAP_UNMAP2 0x46    /* unmap by physical addr */

#define PMAP_NAME   0x60    /* return my name (pass 13 byte buffer) */
#define PMAP_DRVID  0x61    /* return (struct xf86_pmap_drvid) */
#define PMAP_RESERVED4  0x62    /* don't use */
#define PMAP_RESERVED5  0x63    /* don't use */

#define PMAP_READROM    0x64    /* testcfg.sys replacement get adapter memory */

struct xf86_pmap_readrom {
    ULONG command;  /* must be 0 */
    ULONG physaddr; /* 0xc0000 to 0xfffff */
    USHORT numbytes; /* 0-64K */
};
/* data packet must be an array of `numbytesï chars */

#define PMAPDRV_MAGIC   0x36384f58  /* 'XF86' */
#define PMAPDRV_ID  4

struct xf86_pmap_drvid {
    ULONG   magic;
    ULONG   drvtype;
    ULONG   version;
};

#endif
