/*
** THIS SOFTWARE IS SUBJECT TO COPYRIGHT PROTECTION AND IS OFFERED ONLY
** PURSUANT TO THE 3DFX GLIDE GENERAL PUBLIC LICENSE. THERE IS NO RIGHT
** TO USE THE GLIDE TRADEMARK WITHOUT PRIOR WRITTEN PERMISSION OF 3DFX
** INTERACTIVE, INC. A COPY OF THIS LICENSE MAY BE OBTAINED FROM THE 
** DISTRIBUTOR OR BY CONTACTING 3DFX INTERACTIVE INC(info@3dfx.com). 
** THIS PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
** EXPRESSED OR IMPLIED. SEE THE 3DFX GLIDE GENERAL PUBLIC LICENSE FOR A
** FULL TEXT OF THE NON-WARRANTY PROVISIONS.  
** 
** USE, DUPLICATION OR DISCLOSURE BY THE GOVERNMENT IS SUBJECT TO
** RESTRICTIONS AS SET FORTH IN SUBDIVISION (C)(1)(II) OF THE RIGHTS IN
** TECHNICAL DATA AND COMPUTER SOFTWARE CLAUSE AT DFARS 252.227-7013,
** AND/OR IN SIMILAR OR SUCCESSOR CLAUSES IN THE FAR, DOD OR NASA FAR
** SUPPLEMENT. UNPUBLISHED RIGHTS RESERVED UNDER THE COPYRIGHT LAWS OF
** THE UNITED STATES.  
** 
** COPYRIGHT 3DFX INTERACTIVE, INC. 1999, ALL RIGHTS RESERVED
**
** $Revision: 1.1 $
** $Date: 2000-02-18 10:46:43 $
*/
#ifndef _FX_MEMMAP_H_
#define _FX_MEMMAP_H_

/*
 * DeviceIoControl service table control codes for communication with
 * fxmemmap vxd. The codes here must match those in fxmemmap.asm.
 * ESPECIALLY fxmajor_ver and fx_minor_ver!!!
 */

#define   FX_MAJOR_VER          1
#define   FX_MINOR_VER          4

#define   DIOC_CLOSEHANDLE     -1     
#define   DIOC_GETVERSIONDWORD  0   
#define   GETAPPVERSIONDWORD    1   
#define   GETLINEARADDR         2   
#define   GETLDTSELECTOR        3
#define   FREELDTSELECTOR       4
#define   GETLINEARADDR_AUTO    5 /* Watches for & handles PASSTHRU,
                                     Otherwise identical to 2 */
#define   GETMSR                6 /* Get the contents of an MSR */
#define   SETMSR                7
#define   DECREMENTMUTEX        8
#define   SETPASSTHROUGHBASE    9
#define   SETADDRPERM           10

/* FxMemmap Int 2F API Codes */
#define   PROCMAPPHYS              1UL
#define   PROCGETMSR               2UL
#define   PROCSETMSR               3UL
#define   PROCGETVERSION           4UL
#define   PROCUNMAPPHYS            5UL
#define   PROCMAPPHYS16            6UL
#define   PROCGETMSR16             7UL
#define   PROCSETMSR16             8UL   
#define   PROCGETVERSION16         9UL
#define   PROCUNMAPPHYS16          10UL  /* NOT YET IMPLEMENTED */
#define   PROCINCREMENTMUTEX16     11UL
#define   PROCDECREMENTMUTEX16     12UL
#define   PROCSETPASSTHROUGHBASE16 13UL
#define   PROCOUTPUTDEBUGSTRING16  14UL
#define   PROCSETADDRPERM16        15UL

#endif /* _FX_MEMMAP_H_ */
