/* $Id: CreatePath.cmd,v 1.4 2002-05-16 11:50:22 bird Exp $
 *
 * Createpath.cmd <pathname>
 *
 * Creates a path.
 *
 */

if RxFuncQuery('SysMkDir')=1 THEN
    call RxFuncAdd 'SysMkDir', 'RexxUtil', 'SysMkDir'

    parse arg sArgs
    return createpath(strip(sArgs));

createpath: procedure
    parse arg sDir

    /*
     * Any directories above this? If so we'll have to make sure they exists!
     */
    sPath = filespec('path', sDir);
    if (length(sPath) > 0 & sPath <> '\') then
        rc = createpath(filespec('drive', sDir) || substr(sPath, 1, length(sPath)-1));

    /*
     * Create this directory.
     */
    rc = SysMkDir(sDir);
    /*say 'dir:' sDir '- rc='rc;*/

    return 0;
