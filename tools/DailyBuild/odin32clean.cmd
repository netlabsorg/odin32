/* $Id: odin32clean.cmd,v 1.2 2000-09-05 17:36:22 bird Exp $
 *
 * Removes trees. WARNING!!!!! All tree<date> directories are removed
 * if .nodelete is not found in the root of them.
 *
 * (Delpath is a "deltree" clone I've made, use your own.)
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysloadFuncs;

    sTree    = 'tree' || Date(S);

    /*
     * We assume currentdirectory is the current tree.
     */
    rc = SysFileTree('..\tree'||substr(Date('S'),1,4)||'*.', 'asTrees', 'DO');
    if (rc = 0) then
    do
        do i = 1 to asTrees.0
            if (stream(asTrees.i||'\.nodelete', 'c', 'query exists') = '') then
            do
                'echo y | delpath' asTrees.i;
            end
            say asTrees.i
        end
    end

    exit(0);

failure: procedure
parse arg rc, sText;
    say 'rc='rc sText
    exit(rc);

