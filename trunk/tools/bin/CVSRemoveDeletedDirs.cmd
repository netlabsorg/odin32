/*
 * Cleanup script for directories we removed recently.
 * Run from root of your odin32 tree.
 *
 * Specify 'remove' on the commandline to remove the dirs too.
 * Default action is only to remove them from the CVS\Entries file.
 */

/*
 * Import helper functions
 */
call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
call SysLoadFuncs;


/*
 * Check that current directory is root of Odin32 tree.
 */
if (  \fnExists('.\ChangeLog'),
    | \fnExists('.\LICENSE.TXT'),
    | \fnExists('.\Configure.cmd'),
    | \fnExists('.\rartree.cmd'),
    | \fnExists('.\ziptree.cmd'),
    | \fnExists('.\makeall.cmd'),
    ) then
do
    say 'Error: You''re not located in the root of the Odin32 tree!'
    exit(-1);
end


/*
 * Parse arguments.
 */
parse arg sArg1 sArg2

fRemoveDir = 0;
if (translate(sArg1) = 'REMOVE') then
    fRemoveDir = 1;


/*
 * Directories to remove.
 */
asDirs.0 = 5
asDirs.1 = 'src\ws2help'
asDirs.2 = 'src\win32k\object'
asDirs.3 = 'src\win32k\list'
asDirs.4 = 'bin\debug'
asDirs.5 = 'bin\release'


/*
 * Remove the directories.
 */
do i = 1 to asDirs.0
    sSubDir = substr(asDirs.i, lastpos('\', asDirs.i) + 1);
    sParentCVSEntries = substr(asDirs.i, 1, lastpos('\', asDirs.i)),
                        || 'CVS\Entries';

    if (fnExists(sParentCVSEntries)) then
    do
        /*
         * Remove the entry for the given directory.
         */
        if (fnReadFile(sParentCVSEntries)) then
        do
            sMatch = translate('D/'||sSubDir||'/');
            do j = 1 to asLines.0
                if (sMatch = translate(substr(asLines.j, 1, length(sMatch)))) then
                    leave
            end
            do k = j+1 to asLines.0
                j = k - 1;
                asLines.j = asLines.k;
            end
            if (j < asLines.0) then /* if directory entry was found. */
                asLines.0 = j;
            if (fnWriteFile(sParentCVSEntries)) then
            do
                say 'info: removed '''asDirs.i'''';
            end
            else
                say 'error: failed to write'''||sParentCVSEntries||'''';
        end
        else
            say 'error: failed to read '''||sParentCVSEntries||'''';

        /*
         * If requested try delete the directory tree.
         */
        if (fRemoveDir) then
        do
            call fnRemoveDir asDirs.i;
            say 'info: removed the directory(tree) '''asDirs.i'''';
        end
    end
    else
        say 'warning: '''||sParentCVSEntries||''' was not found.';
end

exit(0);



/**
 * Checks if a file exists.
 * @param   sFile
 * @returns TRUE if file exists.
 *          FALSE if file doesn't exists.
 */
fnExists: procedure
    parse arg sFile
    rc = stream(sFile, 'c', 'query exist');
return rc <> '';


/**
 * Read a given file into asLines..
 * @returns Success indicator.
 * @param   sFile   Filename to read.
 */
fnReadFile: procedure expose asLines.;
    parse arg sFile

    asLines.0 = 0;

    rc = stream(sFile, 'c', 'open read');
    if (pos('READY', rc) <> 1) then
        return 0;

    iLine = 0;
    do while lines(sFile) > 0
        iLine = iLine + 1;
        asLines.iLine = linein(sFile);
    end
    asLines.0 = iLine;

    call stream sFile, 'c', 'close';
return 1;


/**
 * Write the file in asLines. to a real file.
 * @returns Success indicator.
 * @param   sFile   Filename to write it to.
 */
fnWriteFile: procedure expose asLines.;
    parse arg sFile

    rc = SysFileDelete(sFile);
    if (rc <> 0) then
        say 'sysfiledelete('sFile') -> rc='rc;
    rc = stream(sFile, 'c', 'open write');
    if (pos('READY', rc) <> 1) then
        return 0;
    do iLine = 1 to asLines.0
        call lineout sFile, asLines.iLine
    end

    call stream sFile, 'c', 'close';
return 1;


/**
 * Removes a given directory tree.
 * Currently we use 'rm -Rf' for this.
 *
 * @returns Successindicator.
 * @param   sDir    Directory tree to remove.
 */
fnRemoveDir: procedure;
    parse arg sDir;
    'rm -Rf 'sDir
return rc;


