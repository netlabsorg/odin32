/* $Id: ExecExcl.cmd,v 1.1 2000-11-20 03:49:59 bird Exp $
 *
 * Exclusive execute. Intented to fix ILINK problem.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

parse arg sLockFile sCmd

if (sCmd = '') then
do
    call syntax;
    exit(1);
end

/*
 * Try get the lockfile.
 *      25 retries, with 3 seconds wait inbetween.
 */
fLocked = 0;
do i = 1 to 25
    rc = stream(sLockFile, 'c', 'open write');
    if (substr(rc, 1, 5) = 'READY') then do
        fLocked = 1;
        leave;
    end
    if (i = 1) then
        call RxFuncAdd 'SysSleep', 'RexxUtil', 'SysSleep';
    call SysSleep 3;                    /* Sleep for three seconds before retrying. */
end
if (\fLocked) then
do
    say 'Error: Failed to get hold of the lockfile "' ||sLockFile|| '"."';
    exit(2);                            /* We exit here! */
end


/*
 * Execute the command and save the result.
 */
sCmd
retRc = rc;


/*
 * Unlock the file and return saved result.
 */
rc = stream(sLockFile, 'c', 'close');
exit(retRc);




/**
 * Display syntax.
 */
syntax: procedure
    say 'ExecExcl.cmd <lockfilename> <command with arguments>'
    return;



/*
 * Establishes the lock.
 * Returns  1 on success
 *          0 on error
 */
lock: procedure
    parse arg sTag
    rc = stream('.dirlocked' || sTag, 'c', 'open write');
    return substr(rc, 1, 5) = 'READY';


/*
 * Releases the lock.
 */
unlock: procedure
    parse arg sTag
    rc = stream('.dirlocked' || sTag, 'c', 'close');
    call SysFileDelete '.dirlocked' || sTag;
    return 0;

