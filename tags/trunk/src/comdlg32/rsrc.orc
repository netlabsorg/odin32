/* $Id: rsrc.orc,v 1.1 2000-03-06 23:38:30 bird Exp $ */
/*
 * Top level resource file for Common Dialogs
 *
 */

#include "winuser.h"
#include "winnls.h"
#include "cdlg.h"
#include "winspool.h"
#include "filedlgbrowser.h"

/*
 * Everything that does not depend on language,
 * like textless bitmaps etc, go into the
 * neutral language. This will prevent them from
 * being duplicated for each language.
 */
#include "cdlg_xx.rc"

/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */
//#include "cdlg_Ca.rc"
//#include "cdlg_Cs.rc"
//#include "cdlg_Da.rc"
//#include "cdlg_De.rc"
#include "cdlg_En.rc"
//#include "cdlg_Eo.rc"
//#include "cdlg_Es.rc"
//#include "cdlg_Fi.rc"
//#include "cdlg_Fr.rc"
//#include "cdlg_Hu.rc"
//#include "cdlg_It.rc"
//#include "cdlg_Ko.rc"
//#include "cdlg_Nl.rc"
//#include "cdlg_No.rc"
//#include "cdlg_Pl.rc"
//#include "cdlg_Pt.rc"
//#include "cdlg_Ru.rc"
//#include "cdlg_Sv.rc"
//#include "cdlg_Wa.rc"
