/* $Id: devsegdf.h,v 1.1 2001-09-11 01:27:27 bird Exp $
 *
 * Segment stuff used in 16 and 32 bits C/C++.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#ifndef _devSegDf_h
#define _devSegDf_h

#ifdef INCL_16

/*
 * More segment stuff
 */
#define DATA16_INIT     _based(_segname("DATA16_INIT"))
#define DATA16_GLOBAL   _based(_segname("DATA16_GLOBAL"))
#define DATA16          _based(_segname("DATA16"))

#else

/*
 * More segment stuff
 */
#define DATA16_INIT
#define DATA16_GLOBAL
#define DATA16

#endif


#endif


