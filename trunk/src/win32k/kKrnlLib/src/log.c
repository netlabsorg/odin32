/* $Id: log.c,v 1.1 2002-03-10 02:45:56 bird Exp $
 *
 * Configures the logging for kKrnlLib.sys.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.com)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: log.c,v 1.1 2002-03-10 02:45:56 bird Exp $";
#define KLIBFUNCTION    KLIBFUNCTIONDEF
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_KKL_LOG
#define INCL_KKL_AVL
#include <kKrnlLib.h>


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/**
 * Config of default types for kKrnlLib.
 * All are currently refering to the above level of logging.
 */
KLOGDEFTYPEINFO     akLogDefTypes_kKrnlLib[] =
{
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_LOGINIT    */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_INITMOD    */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_TERMMOD    */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_START      */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_ENTRY      */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_EXIT       */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_STOP       */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_PRINTF     */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_ASSERT     */
    {KLOG_FLAGS_AS_ABOVE, KLOG_FLAGS_AS_ABOVE}, /* KLOG_TYPE_LOGASSERT  */
};


/**
 * Logging Config Data for kKrnlLib.
 */
KLOGMODDATA         kLogModData_kKrnlLib =
{
    "kKrnlLib",
    "Odin32 Ring-0 Kernel Library - copyright (c) 1998-2001 knut st. osmundsen",

    0,                                  /* cTypedefs;   */
    NULL,                               /* paTypedefs;  */

    0,                                  /* cUserTypes;  */
    NULL,                               /* paUserTypes; */

    KCENTRIES(akLogDefTypes_kKrnlLib),  /* cDefTypes;   */
    &akLogDefTypes_kKrnlLib[0],         /* paDefTypes;  */

    0,                                  /* cFiles;      */
    NULL,                               /* paFiles;     */

#ifndef R3TST
    0,                                  /* fCurrent;    */
    0,                                  /* fDefault;    */
#else
    KLOG_FLAGS_ENABLED,                 /* fCurrent;    */
    KLOG_FLAGS_ENABLED,                 /* fDefault;    */
#endif

    0,0,0,0,                            /* avlnodecore; */
    NULL,                               /* pavlTypedefs;*/
    NULL,                               /* pavlFiles;   */
    0                                   /* ulLastSeq;   */
};


/**
 * The log handle for the kKrnlLib module.
 */
HKLOGMOD    hkLogMod = 0;


/**
 * Inits the logging for kKrnlLib.
 * @returns -1 on error.
 *          0 on success.
 */
int InitkKrnlLibLog(void)
{
    hkLogMod = KLOGINITMOD(&kLogModData_kKrnlLib);
    return hkLogMod != 0 ? 0 : -1;
}

