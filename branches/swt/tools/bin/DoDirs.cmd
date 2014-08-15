/* $Id: DoDirs.cmd,v 1.7 2002-08-29 11:49:36 bird Exp $
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
signal on novalue name NoValueHandler
Address CMD '@echo off'

parse arg '"'sDirs'" 'sCommand

/*
 * Color config.
 */
if ((getenv('BUILD_NOCOLORS') = '') & (getenv('SLKRUNS') = '')) then
do
    sClrMak = '[35;1m'
    sClrErr = '[31;1m'
    sClrRst = '[0m'
end
else
do
    sClrMak = ''
    sClrErr = ''
    sClrRst = ''
end


/*
 * Build Pass
 */
sPass = getenv('_BUILD_PASS');
if (sPass <> '') then
    sPass = 'Pass '||sPass||' - '


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
            say sClrMak||'['||sPass||'Entering  Directory:' directory()||']'||sClrRst;
            sCommand
            if (rc <> 0) then
            do
                say sClrErr||'['||sPass||'Failed rc='rc' directory:' directory()||']'||sClrRst;
                exit(rc);
            end
            say sClrMak||'['||sPass||'Leaving   Directory:' directory()||']'||sClrRst;
        end
        else
        do
            say sClrErr||'['||sPass||'Failed to change directory to' sDir||']'||sClrRst;
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


/**
 * Get environment variable value.
 * @returns Environment variable value if set.
 *          '' if not set.
 * @param   sVar    Variable name.
 */
getenv: procedure
parse arg sVar
return value(sVar,,'OS2ENVIRONMENT');

/**
 * No value handler
 */
NoValueHandler:
    say 'NoValueHandler: line 'SIGL;
return 0;

