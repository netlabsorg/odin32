/* $Id: kKLInitHlp.h,v 1.2 2001-09-11 01:27:28 bird Exp $
 *
 * Definition of the R0 initiation helper IOCtl interface.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _kKInitHlp_h_
#define _kKInitHlp_h_



/*
 * Ring 0 init helper IOCtl
 */
#define KKL_DEVICE_NAME             "kKrnlHlp"
#define KKL_IOCTL_CAT               0xC1
#define KKL_IOCTL_RING0INIT         0x42


typedef struct _KKLR0INITPARAM
{
    ULONG       ulParam;                /* 32-bit parameter word. */
    #ifdef INCL_16
    ULONG       pfn;                    /* 16-bit C fix */
    #else
    int (* _System pfn)();              /* pointer to function to call. */
    #endif
} KKLR0INITPARAM;
typedef KKLR0INITPARAM FAR *PKKLR0INITPARAM;


typedef struct _KKLR0INITDATA
{
    ULONG       ulRc;                   /* Return code from the call. */
} KKLR0INITDATA;
typedef KKLR0INITDATA FAR *PKKLR0INITDATA;


#endif

