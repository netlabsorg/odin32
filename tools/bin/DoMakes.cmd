/* $Id: DoMakes.cmd,v 1.2 2002-08-20 04:07:11 bird Exp $
 *
 * Rexx script which executes a given command with each of
 * the spesified makefiles using the option -f.
 *
 * Syntax: DoMakes.cmd "<sMakefiles>" <sCommand>
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

parse arg '"'sMakefiles'" 'sCommand

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
 * Loop thru each directory.
 * The directories are space separated.
 */
iRc = 0;                                /* Returncode */
iStart = 1;                             /* Subdirectory Index */
do while (iStart <= length(sMakefiles))
    iEnd = pos(' ', sMakefiles, iStart);
    if (iEnd <= 0 & iStart <= length(sMakefiles)) then
        iEnd = length(sMakefiles) + 1;

    if (iEnd > 0) then
    do
        /*
         * Copy out the makefile from sMakefiles. Iterate if empty name.
         * Move iStart forward to the next directory i sMakefiles.
         * Execute command with makefile as -f argument.
         * Check return code. Complain and fail if this failes.
         */
        sMakefile = substr(sMakefiles, iStart, iEnd - iStart);
        iStart = iEnd + 1;
        if (sMakefile = ' ' | sMakefile = '') then/* If empty directory name iterate. */
            iterate;
        say sClrMak||'['||sPass||'Processing Makefile:' sMakefile||']'||sClrRst;
        sCommand '-f' sMakefile
        if (rc <> 0) then
        do
            say sClrErr||'['||sPass||'Failed rc='rc' makefile:' sMakefile||']'||sClrRst;
            exit(rc);
        end
        say sClrMak||'['||sPass||'Completed  Makefile:' sMakefile||']'||sClrRst;
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

