/* $Id: rsrc.orc,v 1.3 2000-03-26 16:31:45 cbratschi Exp $ */
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
#include "cdlg_xx.orc"

/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */

/* TODO TODO FIXME
 * The localized resources have be pulled out due to
 * the time required to move some strings from here to
 * shell32.dll.  This is something to deal with
 * after release
 */
#if 0
#include "cdlg_Ca.orc"
#include "cdlg_Cs.orc"
#include "cdlg_Da.orc"
#include "cdlg_De.orc"
#endif
#include "cdlg_En.orc"
#if 0
#include "cdlg_Eo.orc"
#include "cdlg_Es.orc"
#include "cdlg_Fi.orc"
#include "cdlg_Fr.orc"
#include "cdlg_Hu.orc"
#include "cdlg_It.orc"
#include "cdlg_Ko.orc"
#include "cdlg_Nl.orc"
#include "cdlg_No.orc"
#include "cdlg_Pl.orc"
#include "cdlg_Pt.orc"
#include "cdlg_Ru.orc"
#include "cdlg_Sv.orc"
#include "cdlg_Wa.orc"
#endif
