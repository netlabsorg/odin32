/* $Id: TestKernels.cmd,v 1.3 2001-01-19 02:28:33 bird Exp $
 *
 * This script loops thru a set of different kernels running testcase 1.
 * Note: The kernels and symbols files are in a single directory
 *      Name format
 *      nnnn[n]bk[.SYM]
 *
 *  nnnn[n]     Build number. (4 or 5 digits). (ex. 9036)
 *  b           Build type: A - allstrict; H - halfstrict; R - retail (GA)
 *  k           Kernel type: U - uniprocess; S - smp; 4 - warp 4 fixpack 13+
 *  [.SYM]      Kernel files has no extention while the symbol files has .SYM.
 */

    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs';
    call SysLoadFuncs;

    parse arg sDir sdummy
    if (sDir = '' | sDir = '-?' | sDir = '/?' | sDir = '-h' | sDir = '-H' | sDir  = '/h' | sDir = '/H' | sDir = '--help') then
    do
        call syntax;
        exit -1;
    end

    /*
     * Read directory
     */
    rc = SysFileTree(sDir'\*', 'asFiles', 'FO');
    if (rc <> 0) then
    do
        say 'SysFileTree failed with rc='rc'.';
        exit -2;
    end
    if (asFiles.0 <= 0) then
    do
        say 'No files found';
        exit -3;
    end

    iRetCode = 0;
    do i = 1 to asFiles.0
        /*
         * Interpret name (get build no., kernel type and build type).
         */
        sName = translate(filespec('name', asFiles.i));
        if (lastpos('.', sName) > 0) then
        do
            sExt = substr(sName, lastpos('.', sName) + 1);
            sName = substr(sName, 1, lastpos('.', sName) - 1);
        end
        else
            sExt = '';

        if (sExt <> '') then
            iterate

        if (substr(sName, 5, 1) <= '9') then    cchBuild = 5;
        else                                    cchBuild = 4;
        iBuild = substr(sName, 1, cchBuild);
        chBuildType = substr(sName, cchBuild + 1, 1);
        chKernelType = substr(sName, cchBuild + 2, 1);
        chRev = substr(sName, cchBuild + 3, 1);

        /*
         * Validate name.
         */
        do j = 1 to length(iBuild)
            if (substr(iBuild, j, 1) < '0' | substr(iBuild, j, 1) > '9') then
            do
                j = -1;
                leave;
            end
        end
        if (j = -1) then
            iterate;

        if (chBuildType <> 'A' & chBuildType <> 'H' & chBuildType <> 'R') then
        do
            say 'invalid build type char:' chBuildType '('asFiles.i')';
            exit -4;
        end

        if (chKernelType <> 'U' & chKernelType <> 'S' & chKernelType <> '4') then
        do
            say 'invalid kernel type char:' chKernelType '('asFiles.i')';
            exit -4;
        end

        /*
         * Determin version number (based on build number).
         */
        iVerMajor = 2;
        if (iBuild >= 14000) then
            iVerMinor = 45;
        else if (iBuild >= 9000) then
            iVerMinor = 40;
        else if (iBuild >= 8000) then
            iVerMinor = 30;
        else if (iBuild >= 6200) then
            iVerMinor = 21;
        else
        do
            say 'unsupported build number:' iBuild '('asFiles.i')';
            exit(-5);
        end

        /*
         * Process it
         */
        say;
        say;
        say 'Processing' asFiles.i'....';
        if (chBuildType = 'R') then
            sCmd = 'win32ktst.exe 1' asFiles.i iVerMajor iVerMinor iBuild chKernelType chBuildType chRev ;
        else
            sCmd = 'win32ktst.exe 1' asFiles.i iVerMajor iVerMinor iBuild chKernelType chBuildType chRev asFiles.i||'.SYM';
        say sCmd;
        sCmd;
        if (rc <> 0) then
        do
            say 'failed... rc='rc;
            exit rc;
        end

    end

    exit(0);




/*
 * Write syntax:
 */
syntax: procedure;
    say 'TestKernels.cmd <kernel-directory>';
    return;