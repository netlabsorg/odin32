/* $Id: StateUpd.cmd,v 1.4 2000-08-02 20:25:50 bird Exp $
 *
 * Helper script which invokes StateUpd.cmd.
 * This was created to hold special rules for dirs like OpenGl.
 *
 * Copyright (c) 2000 knut st. osmundsen
 *
 */
    /* find (possible) dll name from directory name */
    sDllName = filespec('name', directory());

    /* find StateUpd exe path assuming it's in the same dir as this script. */
    parse source sD1 sD2 sSrc;
    sSrc = filespec('drive', sSrc) || filespec('path', sSrc);
    sStateUpd = sSrc||'StateUpd.exe';

    /* parse arguments */
    parse arg sAllArgs

    /* apecial cases and general case */
    if (translate(sDllName) = 'OPENGL') then
    do
        /* save dir and change dir into mesa */
        /*sOldDir = directory();
        call directory('mesa'); */

        /* execute update  */
        sStateUpd || ' -dll:opengl32 -s ' || sAllArgs;
        lRc = rc;

        /* restore dir */
        /*call directory sOldDir;*/
    end
    else if (translate(sDllName) = 'WNETAP32') then
    do
        /* execute update  */
        sStateUpd || ' -dll:netapi32 ' || sAllArgs;
        lRc = rc;
    end
    else
    do
        /* execute update  */
        sStateUpd || ' ' || sAllArgs;
        lRc = rc;
    end
    exit(lRc);
