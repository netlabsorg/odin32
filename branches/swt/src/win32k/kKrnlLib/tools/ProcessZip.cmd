/* $Id: ProcessZip.cmd,v 1.1 2002-04-07 01:44:47 bird Exp $
 *
 * Testcase Zipped Kernel Processer.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * GPL
 *
 */

/*
 * Config.
 */
sSyms = 'h:\kernels\syms';
sSDFs = 'h:\kernels\sdfs';
sPMDF = 'f:\pmdf';

/* Temp dir. */
sTmp = value('TMP',,'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = value('TEMP',,'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = value('TMPDIR',,'OS2ENVIRONMENT');
if (sTmp = '') then
do
    say 'No TMP/TEMP/TMPDIR env variable.'
    exit(2);
end


/*
 * Input
 */
parse arg sZip sDummy

if ((sZip = '') | (sDummy <> '') | (stream(sZip, 'c', 'query exist') = '')) then
do
    say 'Syntax error or invalid filename!'
    say 'Syntax: ProcessZip.cmd <zipfile name>'
    exit(1);
end


/*
 * zip type?
 */
if (translate(substr(filespec('name', sZip), 1, 2)) = 'DF' ) then
    rc = ProcessDumpFormatterZip(sZip);
else
    rc = ProcessKernelZip(sZip);
exit rc;



/**
 * Extracts os2krnl and os2krnl.sym to the syms directory.
 */
ProcessKernelZip: procedure expose sSyms;
    parse arg sZip
    /*
     * Get the build number.
     */
    queTmp = RxQueue('Create');
    queOld = RxQueue('Set', queTmp);
    Address CMD 'unzip -p' sZip 'os2krnl | grep "Internal revision" | sed "s/.*Internal revision[ \t]*//" | cut -b1-10 | RxQueue' queTmp
    sBuild = '';
    if (queued() > 0) then
        parse pull sBuild
    call RxQueue 'Delete', RxQueue('Set', queOld);
    if (sBuild = '') then
    do
        say 'couldn''t determin build number.' sZip
        return(3);
    end
    rc = InterpretInternalRevision(sBuild);


    /* get kernel build type */
    queTmp = RxQueue('Create');
    queOld = RxQueue('Set', queTmp);
    Address CMD 'unzip -p' sZip 'os2krnl | grep "SAB KNL" | sed "s/.*SAB KNL//" | cut -b1 | RxQueue' queTmp
    chBuildType = '';
    if (queued() > 0) then
        pull chBuildType
    call RxQueue 'Delete', RxQueue('Set', queOld);
    if (sBuild = '') then
    do
        say 'couldn''t determin build type.' sZip
        return(3);
    end


    /*
     *  Construct base filename
     */
    sBase = sSyms||'\'||uBuild||chBuildType||chType||chRev
    say sBase


    /*
     * Skip if allready exists.
     */
    if ( (stream(sBase||'.sym', 'c', 'query exist') <> '') & (stream(sBase, 'c', 'query exist') <> '') ) then
    do
        say 'nothing to do' sZip;
        return(0);
    end

    /*
     * Extract the files.
     */
    rcExit = 0;
    Address CMD 'unzip -p' sZip 'os2krnl >' sBase
    if (rc <> 0) then
    do
        say 'Error: failed to extract kernel file. rc='rc;
        rcExit = rc;
    end
    else
        Address CMD 'attrib +r' sBase

    Address CMD 'unzip -p' sZip 'os2krnl.sym >' sBase||'.sym'
    if (rc <> 0) then
    do
        say 'Error: failed to extract symbol file. rc='rc;
        rcExit = rc;
    end
    else
        Address CMD 'attrib +r' sBase||'.sym'

return(rcExit);




/**
 * Extracts the .sdf files to the sdfs directory.
 * Extracts all the stuff to the pmdf directory and updates pmdfvers.lst.
 */
ProcessDumpFormatterZip: procedure expose sSDFs sPMDF sTmp;
    parse arg sZip

    /*
     * Get content list and extract the first .exe file.
     */
    rc = ListZip(sZip);

    iExe = 0;
    do i = 1 to asFiles.0
        if (translate(right(asFiles.i, 4)) = '.EXE') then
        do
            iExe = i;
            leave;
        end
    end
    if (iExe = 0) then
    do
        say 'Invalid zip, didn''t find any .exe files.' sZip;
        return 10;
    end

    Address CMD 'unzip -p' sZip asFiles.iExe '>' sTmp'\Tmpdf.exe';

    /*
     * Get the build number.
     */
    queTmp = RxQueue('Create');
    queOld = RxQueue('Set', queTmp);
    Address CMD sTmp'\Tmpdf.exe | grep "Internal revision" | sed "s/.*Internal revision[ \t]*//" | RxQueue' queTmp
    sBuild = '';
    if (queued() > 0) then
        parse pull sBuild
    call RxQueue 'Delete', RxQueue('Set', queOld);
    if (sBuild = '') then
    do
        say 'couldn''t determin build number.' sZip
        return(3);
    end
    rc = InterpretInternalRevision(sBuild);


    /*
     * Extract the .sdf.
     */
    do i =  1 to asFiles.0
        if (translate(right(asFiles.i, 4)) = '.SDF') then
        do
            /*
             * Type.
             */
            chType = translate(substr(asFiles.i, 1, 1));
            if (chType = 'W') then
                chType = 'W4';
            else if ((chType <> 'S') & (chType <> 'U')) then
            do
                say 'invalid sub directory:' asFiles.i;
                return 11;
            end

            /*
             * Construct filename
             */
            sFilename = sSdfs||'\'||uBuild||'_'||chType'_';
            if (chRev <> '') then
                sFilename = sFilename||chRev||'_';
            sFilename = sFilename || filespec('name', asFiles.i);
            say sFilename;
            if (stream(sFilename, 'c', 'query exist') = '') then
            do
                Address CMD 'unzip -p' sZip asFiles.i '>' sFilename;
                if (rc <> 0) then
                do
                    say 'extract of' asFiles.i '->' sFilename' failed. rc='rc;
                    rcExit = rc;
                end
                Address CMD 'attrib +r' sFilename;
            end
        end
    end



    /*
     * PMDF.
     */
    sPMDFListDirs = '';
    fSkipDir = 1;
    do i = 1 to asFiles.0
        /*
         * Is the first file in that directory?
         */
        sDir = strip(strip(filespec('path', asFiles.i), 'B', '\'), 'B', '/');
        if (pos(sDir, sPMDFListDirs) <= 0) then
        do
            sPMDFListDirs = sPMDFListDirs ||' ; '||sDir;

            /*
             * Check if df_ret.exe exists - if so, nothing to do.
             */
            if (stream(sPMDF||'\'||sDir||'df_ret.exe', 'c', 'query exist') <> '') then
                fSkipDir = 1;
            else
                fSkipDir = 0;

            /*
             * Create directory and add it to pmdfvers.lst.
             */
            if (\fSkipDir) then
            do
                Address CMD 'mkdir' sPMDF||'\'||sDir;
                Address CMD 'echo' sDir||';'||sBuild||';'||sBuild '>>' sPMDF'\pmdfvers.lst';
            end
        end

        /*
         * Extract the file.
         */
        if (\fSkipDir) then
        do
            Address CMD 'unzip -p' sZip asFiles.i '>' sPMDF||'\'||sDir||'\'||filespec('name', asFiles.i);
        end
    end

return(rcExit);



/**
 * Gets the zip content listing.
 */
ListZip: procedure expose asFiles.;
    parse arg sZip
    queTmp = RxQueue('Create');
    queOld = RxQueue('Set', queTmp);
    Address CMD 'unzip -v' sZip '| RxQueue' queTmp
    asFiles.0 = 0;
    fFiles = 0;
    do while (queued() > 0)
        parse pull sLine
        sLine = strip(sLine);
        /* check for start/end of file listing */
        if (substr(sLine, 1, 1) = '-') then
        do
            fFiles = fFiles + 1;
            iterate;
        end

        /* in file listing? */
        if (fFiles = 1) then
        do
            asFiles.0 = asFiles.0 + 1;
            i = asFiles.0;
            parse var sLine . . . . . . . asFiles.i
            asFiles.i = strip(asFiles.i, 'L');
        end
    end
    call RxQueue 'Delete', RxQueue('Set', queOld);
return 0;


/**
 * Get build number, revision and type.
 */
InterpretInternalRevision: procedure expose uBuild chRev chType;
    parse arg sBuild

    /* got a 10 byte string like 14.086c_UN */
    parse upper var sBuild iHigh'.'sLow'_'sType with
    uBuild = substr(sLow, 1, 3) + iHigh*1000;
    chRev = ''
    if (length(sLow) > 3) then
        chRev = substr(sLow, 4, 1);

    select
        when (substr(sType, 1, 1) = 'U') then
            chType = 'U'
        when (substr(sType, 1, 1) = 'S') then
            chType = 'S'
        when (substr(sType, 1, 1) = 'W') then
            chType = '4'
        otherwise
        do
            say 'fatal error: unknown kernel type: '''sType''''
            say 'sBulid =' sBuild
            exit(4);
        end
    end
return 0;

