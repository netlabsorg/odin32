/* $Id: rsrc.orc,v 1.6 2000-08-09 15:57:55 cbratschi Exp $ */
/*
 * Top level resource file for Common Controls
 *
 */

#include "winuser.h"
#include "winnls.h"
#include "comctl32.h"
#include "odinrsrc.h"

/*
 * Everything that does not depend on language,
 * like textless bitmaps etc, go into the
 * neutral language. This will prevent them from
 * being duplicated for each language.
 */
#include "cctl_xx.orc"

/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */

#include "cctl_Ca.orc"
#include "cctl_Cs.orc"
#include "cctl_Da.orc"
#include "cctl_De.orc"
#include "cctl_En.orc"
#include "cctl_Eo.orc"
#include "cctl_Es.orc"
#include "cctl_Fi.orc"
#include "cctl_Fr.orc"
#include "cctl_Hu.orc"
#include "cctl_It.orc"
#include "cctl_Ko.orc"
#include "cctl_Nl.orc"
#include "cctl_No.orc"
#include "cctl_Pl.orc"
#include "cctl_Pt.orc"
#include "cctl_Ru.orc"
#include "cctl_Sv.orc"
#include "cctl_Wa.orc"
