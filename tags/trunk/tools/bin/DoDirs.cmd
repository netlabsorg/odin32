/* $Id: DoDirs.cmd,v 1.3 2001-04-17 04:00:41 bird Exp $
 *
 * Rexx script which executes a given command in each of the given
 * directories. It will fail when a command failes in one of the
 * directories or if it failes to change to one of the directories.
 *
 * Syntax: ProcessDirs.cmd "<sDirs>" <sCommand>
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

parse arg '"'sDirs'" 'sCommand

/*
 * Save current directory.
 */
sCurrentDir = directory();


/*
 * Loop thru each directory.
 * The directories are space separated.
 */
iRc = 0;                                /* Returncode */
iStart = 1;                             /* Subdirectory Index */
do while (iStart <= length(sDirs))
    iEnd = pos(' ', sDirs, iStart);
    if (iEnd <= 0 & iStart <= length(sDirs)) then
        iEnd = length(sDirs) + 1;

    if (iEnd > 0) then
    do
        /*
         * Copy out the directory from sDirs. Iterate if empty name.
         * Move iStart forward to the next directory i sDirs.
         * Try change directory. Complain and fail if this failes.
         * Execute command.
         * Check return code. Complain and fail if this failes.
         */
        sDir = substr(sDirs, iStart, iEnd - iStart);
        iStart = iEnd + 1;
        if (sDir = ' ' | sDir = '') then/* If empty directory name iterate. */
            iterate;
        if (directory(sDir) <> '') then
        do
            say '[Entering directory:' directory()']'
            sCommand
            if (rc <> 0) then
            do
                say '[Failed rc='rc' directory:' directory()']';
                exit(rc);
            end
            say '[Leaving  directory:' directory()']'
        end
        else
        do
            say '[Failed to change directory to' sDir']';
            exit(267);                  /* ERROR_DIRECTORY */
        end
        call directory sCurrentDir;     /* Restore start directory. */
    end
    else
        leave;                          /* No more directories left. */
end


/*
 * Return successfully.
 */
exit(0);
