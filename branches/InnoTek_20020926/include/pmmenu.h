/* $Id: pmmenu.h,v 1.2 1999-05-27 15:17:58 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __PMMENU_H__
#define __PMMENU_H__

#pragma pack(1) /*PLF Fri  98-02-27 16:29:08*/
      typedef struct _mtios2     /* mti */
      {
         USHORT afStyle;      /* Style Flags      */
         USHORT pad;          /* pad for template */
         USHORT idItem;       /* Item ID          */
         CHAR   c[2];         /*                  */
      } MTI_OS2;

      typedef struct _mtos2      /* mt */
      {
         ULONG  len;          /* Length of template in bytes */
         USHORT codepage;     /* Codepage                    */
         USHORT reserved;     /* Reserved.                   */
         USHORT cMti;         /* Count of template items.    */
         MTI_OS2 rgMti[1];     /* Array of template items.    */
      } MT_OS2;
      typedef MT_OS2 *LPMT_OS2;
#pragma pack()  /*PLF Fri  98-02-27 16:29:19*/

#endif
