/* $Id: StateUpd.cmd,v 1.1 2000-03-14 16:17:04 bird Exp $
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
    if (translate(sDllName) = 'OPENGL) then
    do
        sOldDir = directory('mesa');

        sStateUpd || ' ' || sAllArgs;
        lRc = rc;
        call directory sOldDir;
    end
    else
    do
        sStateUpd || ' ' || sAllArgs;
        lRc = rc;
    end
    exit(lRc);
