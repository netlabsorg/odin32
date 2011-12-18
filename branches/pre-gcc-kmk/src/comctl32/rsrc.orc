/* $Id: rsrc.orc,v 1.7 2002-05-08 11:25:02 sandervl Exp $ */
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
#include "resources\cctl_xx.orc"

/*
 * Everything specific to any language goes
 * in one of the specific files.
 * Note that you can and may override resources
 * which also have a neutral version. This is to
 * get localized bitmaps for example.
 */

#include "resources\cctl_Ca.orc"
#include "resources\cctl_Cs.orc"
#include "resources\cctl_Da.orc"
#include "resources\cctl_De.orc"
#include "resources\cctl_En.orc"
#include "resources\cctl_Eo.orc"
#include "resources\cctl_Es.orc"
#include "resources\cctl_Fi.orc"
#include "resources\cctl_Fr.orc"
#include "resources\cctl_Hu.orc"
#include "resources\cctl_It.orc"
#include "resources\cctl_Ko.orc"
#include "resources\cctl_Nl.orc"
#include "resources\cctl_No.orc"
#include "resources\cctl_Pl.orc"
#include "resources\cctl_Pt.orc"
#include "resources\cctl_Ru.orc"
#include "resources\cctl_Sv.orc"
#include "resources\cctl_Wa.orc"
