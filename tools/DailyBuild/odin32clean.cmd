/* $Id: odin32clean.cmd,v 1.5 2003-02-06 21:03:43 bird Exp $
 *
 * Removes trees. WARNING!!!!! All tree<date> directories are removed
 * if .nodelete is not found in the root of them.
 *
 * (Delpath is a "deltree" clone I've made, use your own.)
 *
 * Copyright (c) 1999-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    /* get build settings */
    sDate = value('BUILD_DATE',, 'OS2ENVIRONMENT');
    sType = value('BUILD_TYPE',, 'OS2ENVIRONMENT');
    if ((sDate = '') | (sType = '')) then do say 'BUILD_DATE/BUILD_TYPE unset, you didn''t start job.cmd.'; exit(16); end

    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysloadFuncs;

    sTree    = 'tree' || sDate;

    /*
     * We assume currentdirectory is the current tree.
     */
    rc = SysFileTree('..\tree'||substr(sDate,1,4)||'*.', 'asTrees', 'DO');
    if (rc = 0) then
    do
        do i = 1 to asTrees.0
            if (stream(asTrees.i||'\.nodelete', 'c', 'query exists') = '') then
            do
                parse source . . sSrc
                filespec('drive', sSrc)||filespec('path', sSrc)||'\bin\rm -rF "'asTrees.i'"';
            end
            say asTrees.i
        end
    end
exit(0);


failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
exit(rc);

