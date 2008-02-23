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
#include "resources\cdlg_xx.orc"

/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */
#include "resources\cdlg_Ca.orc"
#include "resources\cdlg_Cs.orc"
#include "resources\cdlg_Da.orc"
#include "resources\cdlg_De.orc"
#include "resources\cdlg_En.orc"
#include "resources\cdlg_Eo.orc"
#include "resources\cdlg_Es.orc"
#include "resources\cdlg_Fi.orc"
#include "resources\cdlg_Fr.orc"
#include "resources\cdlg_Hu.orc"
#include "resources\cdlg_It.orc"
#include "resources\cdlg_Ko.orc"
#include "resources\cdlg_Nl.orc"
#include "resources\cdlg_No.orc"
#include "resources\cdlg_Pl.orc"
#include "resources\cdlg_Pt.orc"
#include "resources\cdlg_Ru.orc"
#include "resources\cdlg_Sk.orc"
#include "resources\cdlg_Sv.orc"
#include "resources\cdlg_Wa.orc"
#include "resources\cdlg_Ja.orc"
#include "resources\cdlg_Zh.orc"
