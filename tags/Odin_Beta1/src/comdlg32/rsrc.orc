/*
 * Top level resource file for Common Dialogs
 *
 */

#include "windef.h"
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
#include "cdlg_Ca.orc"
#include "cdlg_Cs.orc"
#include "cdlg_Da.orc"
#include "cdlg_De.orc"
#include "cdlg_En.orc"
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
#include "cdlg_Sk.orc"
#include "cdlg_Sv.orc"
#include "cdlg_Wa.orc"
#include "cdlg_Ja.orc"
#include "cdlg_Zh.orc"
