/* $Id: DLGCONVERT.H,v 1.3 1999-06-10 16:50:35 phaller Exp $ */

/*
 * Project Odin Software License can be found in LICENSE.TXT
 */
#ifndef __DLGCONVERT_H__
#define __DLGCONVERT

#ifdef __cplusplus
      extern "C" {
#endif

DLGTEMPLATE *ConvertWin32DlgTemplate(DLGTEMPLATE *windlg);
void         DeleteWin32DlgTemplate(DLGTEMPLATE *os2dlg);

#ifdef __cplusplus
        }
#endif

#endif
