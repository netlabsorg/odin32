/**/


/*
 * Init stuff.
 */
signal on NoValue Name SignalHanlder_NoValue;
NUMERIC DIGITS 11


/*
 * Globals
 */
sGlobals = '';

/*
 * Operation
 */
parse arg sCmd sArgs

select
    when (sCmd = 'checksems') then
        return pmsemCheck(sArgs);
/*
    when (sCmd = '') then
            return
*/
    when (sCmd = '?' | sCmd = 'help' | sCmd = '-?' | sCmd = '/?' | sCmd = '-h' | sCmd = '/h' | sCmd = '--help') then
        return syntax(sArgs);
    otherwise
        say 'syntax error: no or invalid command'
        return syntax(sArgs);
    end
exit(0)

/**
 * Display usage syntax:
 */
syntax: procedure;
    parse source . . sSource;
    sName = filespec('name', sSource);
    say 'PMDF PM Rexx Utils v0.0.1';
    say 'syntax: %'sName' <command> [args]';
    say 'command:'
    say '       checksems       Check the PM semaphores';
return -1;



/**
 * Check if any of the PM sems are taken or have bogus state.
 * @returns 0 on success. -1 on error.
 */
pmsemCheck: procedure
    sMem = dfReadMem('pmsemaphores', 35*8*4)
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        cDumps = 0;
        say 'info: checking pm/gre sems'
        do iSem = 0 to 34
            rc = pmsemValidate(iSem, sMem);
            if (rc <> 1) then
            do
                if (cDumps = 0) then say '';
                cDumps = cDumps + 1;
                if rc = 0 then  sMsg = 'Taken';
                else            sMsg = 'Bogus';
                call pmsemDump memCopy(iSem * 32, 32, sMem), sMsg, iSem;
            end
        end
        if (cDumps = 0) then
            say 'info: pm/gre sems are all free and ok.'
        else
            say 'info: 'cDumps 'semaphores was taken or bogus.';
    end
    else
        say 'error: failed to read semaphore table.';
return -1;

/* access functions */
pmsemIdent:   procedure; parse arg iSem, sMem; return memString(iSem * 32,7,1, sMem);
pmsem386:     procedure; parse arg iSem, sMem; return memByte( iSem * 32 +  7, sMem);
pmsemTid:     procedure; parse arg iSem, sMem; return memWord( iSem * 32 +  8, sMem);
pmsemPid:     procedure; parse arg iSem, sMem; return memWord( iSem * 32 + 10, sMem);
pmsemPTid:    procedure; parse arg iSem, sMem; return memDWord(iSem * 32 +  8, sMem);
pmsemNested:  procedure; parse arg iSem, sMem; return memDword(iSem * 32 + 12, sMem);
pmsemWaiting: procedure; parse arg iSem, sMem; return memDword(iSem * 32 + 16, sMem);
pmsemUseCount:procedure; parse arg iSem, sMem; return memDword(iSem * 32 + 20, sMem);/*debug*/
pmsemHEV:     procedure; parse arg iSem, sMem; return memDword(iSem * 32 + 24, sMem);
pmsemCallAddr:procedure; parse arg iSem, sMem; return memDword(iSem * 32 + 28, sMem);/*debug*/


/**
 * Structure dumper.
 * @param   sSemMem     32 byte memory block (at least) containing the PMSEM to dump.
 * @parma   sMsg        Optional description message. (optional)
 * @param   iSem        The sem we're dumping. (optional)
 */
pmsemDump: procedure;
parse arg sSemMem, sMsg, iSem
    if (iSem <> '') then
        say sMsg 'PMSEM/GRESEM -' pmsemGetName(iSem);
    else
        say sMsg 'PMSEM/GRESEM';
    say '         acIdent:' pmsemIdent(0, sSemMem);
    say '           fcSet:' pmsem386(0, sSemMem);
    say '             Tid:' d2x(pmsemTid(0, sSemMem),4);
    say '             Pid:' d2x(pmsemPid(0, sSemMem),4);
    say 'ulNestedUseCount:' d2x(pmsemNested(0, sSemMem),8);
    say '  ulWaitingCount:' d2x(pmsemWaiting(0, sSemMem),8);
    say '      ulUseCount:' d2x(pmsemUseCount(0, sSemMem),8);
    say '   ulEventHandle:' d2x(pmsemHEV(0, sSemMem),8);
    say '    ulCallerAddr:' d2x(pmsemCallAddr(0, sSemMem),8);
return 0;


/**
 * Checks a give PM sem is free and not bogus.
 * @returns 1 if free and not bogus.
 *          0 if taken.
 *          -1 if bogus.
 * @param   iSem    Semaphore index.
 * @param   sMem    Memory containging the semaphore array.
 *                  (If no array use iSem=0)
 */
pmsemValidate: procedure
parse arg iSem, sMem
    if (pmsemPTid(iSem, sMem) <> 0) then
        return 0;
    if (pos(pmsemIdent(iSem, sMem), "PMSEM;;;;;GRESEM") < 0) then
        return -1;
    if (pmsemWaiting(iSem, sMem) > 0) then
        return -1;
    if (pmsemHEV(iSem, sMem) = 0) then
        return -1;
return 1;



/**
 * Gives us the name of the pmsem at a given index.
 * @returns Namestring.
 * @param   i   Index
 */
pmsemGetName: procedure;
parse arg i
    select
        when i = 0  then return 'PMSEM_ATOM';
        when i = 1  then return 'PMSEM_USER';
        when i = 2  then return 'PMSEM_VISLOCK';
        when i = 3  then return 'PMSEM_DEBUG';
        when i = 4  then return 'PMSEM_HOOK';
        when i = 5  then return 'PMSEM_HEAP';
        when i = 6  then return 'PMSEM_DLL';
        when i = 7  then return 'PMSEM_THUNK';
        when i = 8  then return 'PMSEM_XLCE';
        when i = 9  then return 'PMSEM_UPDATE';
        when i = 10 then return 'PMSEM_CLIP';
        when i = 11 then return 'PMSEM_INPUT';
        when i = 12 then return 'PMSEM_DESKTOP';
        when i = 13 then return 'PMSEM_HANDLE';
        when i = 14 then return 'PMSEM_ALARM';
        when i = 15 then return 'PMSEM_STRRES';
        when i = 16 then return 'PMSEM_TIMER';
        when i = 17 then return 'PMSEM_CONTROLS';
        when i = 18 then return 'GRESEM_GREINIT';
        when i = 19 then return 'GRESEM_AUTOHEAP';
        when i = 20 then return 'GRESEM_PDEV';
        when i = 21 then return 'GRESEM_LDEV';
        when i = 22 then return 'GRESEM_CODEPAGE';
        when i = 23 then return 'GRESEM_HFONT';
        when i = 24 then return 'GRESEM_FONTCNTXT';
        when i = 25 then return 'GRESEM_FNTDRVR';
        when i = 26 then return 'GRESEM_SHMALLOC';
        when i = 27 then return 'GRESEM_GLOBALDATA';
        when i = 28 then return 'GRESEM_DBCSENV';
        when i = 29 then return 'GRESEM_SRVLOCK';
        when i = 30 then return 'GRESEM_SELLOCK';
        when i = 31 then return 'GRESEM_PROCLOCK';
        when i = 32 then return 'GRESEM_DRIVERSEM';
        when i = 33 then return 'GRESEM_SEMLFICACHE';
        when i = 34 then return 'GRESEM_SEMFONTTABLE';
    otherwise
    end
return 'Unknown-'i;

/*
 * COMMON
 * COMMON
 * COMMON
 * COMMON
 * COMMON
 * COMMON
 */



/**
 * Read memory.
 * @param   sStartExpr  Expression giving the address where to read from.
 * @param   cbLength    Number of _bytes_ to read.
 * @returns The memory we have read. (internal format!)
 */
dfReadMem: procedure
parse arg sStartExpr, cbLength

    /* dump memory */
    if ((cbLength // 4) = 0) then
    do  /* optimized read */
        say 'df: dd' sStartExpr 'L'cbLength/4'T'
        Address df 'CMD' 'asOut' 'dd' sStartExpr 'L'cbLength/4'T'
        say 'df: rc='rc' asOut.0='asOut.0;
        if (rc = 0) then
        do
            /* interpret output */
            j = 0;
            sMem = '';
            do i = 1 to asOut.0
                /* format:
                 * 0000:00000000  45534D50 0000004D 00000000 00000000
                 */
                parse var asOut.i .'  'ch.0' 'ch.1' 'ch.2' 'ch.3
                /*say 'dbg:' asOut.i
                say 'dbg:' ch.0','ch.1','ch.2','ch.3*/
                k = 0;
                ch.4 = '';
                do while(k <= 3 & strip(ch.k) <> '')
                    sMem = sMem || substr(ch.k,7,2)||substr(ch.k,5,2)||substr(ch.k,3,2)||substr(ch.k,1,2);
                    j = j + 4;
                    k = k + 1;
                end
            end
            return d2x(j,8)||sMem;
        end

    end
    else
    do  /* slower (more output) byte by byte read */
        say 'df: db' sStartExpr 'L'cbLength'T'
        Address df 'CMD' 'asOut' 'db' sStartExpr 'L'cbLength'T'
        say 'df: rc='rc' asOut.0='asOut.0;
        if (rc = 0) then
        do
            /* interpret output */
            j = 0;
            sMem = '';
            do i = 1 to asOut.0
                /* format:
                 * 9f47:0000af00 50 4d 53 45 4d 00 00 00-00 00 00 00 00 00 00 00 PMSEM...........
                 */
                ch.16 = '';
                parse var asOut.i .' 'ch.0' 'ch.1' 'ch.2' 'ch.3' 'ch.4' 'ch.5' 'ch.6' 'ch.7'-'ch.8' 'ch.9' 'ch.10' 'ch.11' 'ch.12' 'ch.13' 'ch.14' 'ch.15' '.
                k = 0;
                /*say 'dbg:' asOut.i
                say 'dbg:' ch.0','ch.1','ch.2','ch.3','ch.4','ch.5','ch.6','ch.7','ch.8','ch.9','ch.10','ch.11','ch.12','ch.13','ch.14','ch.15*/
                do while(k <= 15 & strip(ch.k) <> '')
                    sMem = sMem || ch.k;
                    j = j + 1;
                    k = k + 1;
                end
            end
            return right(d2x(j), 8, '0')||sMem;
        end
    end
return '';


/**
 * Gets a byte from the memory array aMem.
 * @param   iIndex      Byte offset into the array.
 */
memByte: procedure
parse arg iIndex, sMem
    cb = memSize(sMem);
    if (iIndex < cb) then
    do
        return x2d(substr(sMem, (iIndex * 2) + 9 + 0, 2));
    end
    say 'error-memByte: access out of range.'
return -1;


/**
 * Gets a word from the memory array aMem.
 * @param   iIndex      Byte offset into the array.
 */
memWord: procedure
parse arg iIndex, sMem
    cb = memSize(sMem);
    if (iIndex + 1 < cb) then
    do
        return x2d(substr(sMem, (iIndex * 2) + 9 + 2, 2)||,
                   substr(sMem, (iIndex * 2) + 9 + 0, 2));
    end
    say 'error-memWord: access out of range.'
return -1;


/**
 * Gets a dword from the passed in memory block.
 * @param   iIndex      Byte offset into the array.
 * @param   sMem        Memory block.
 * @remark  note problems with signed!
 */
memDword: procedure
parse arg iIndex, sMem
    cb = memSize(sMem);
    if (iIndex + 3 < cb) then
    do
        iIndex = iIndex*2 + 9;
        return   x2d(substr(sMem, iIndex + 6, 2)||,
                     substr(sMem, iIndex + 4, 2)||,
                     substr(sMem, iIndex + 2, 2)||,
                     substr(sMem, iIndex + 0, 2));
    end
    say 'error-memDword: access out of range.'
return -1;


/**
 * Gets a string from the memory array aMem.
 * @return  String.
 * @param   iIndex          Byte offset into the array aMem.
 * @param   cchLength       Length of the string. (optional)
 *                          If not specified we'll stop at '\0' or end of aMem.
 * @param   fStoppAtNull    Flag that we'll stop at '\0' even when lenght is specifed. (optional)
 *                          Default is to fetch cchLength if cchLength is specifed.
 */
memString: procedure
parse arg iIndex, cchLength, fStoppAtNull, sMem
    cb = memSize(sMem);
    if (iIndex < cb) then
    do
        /* handle optional parameters */
        if (fStoppAtNull = '') then
            fStoppAtNull = (cchLength = '');
        if (cchLength = '') then
            cchLength = cb - iIndex;
        else if (cchLength + iIndex > cb) then
            cchLength = cb - iIndex;

        /* fetch string */
        sStr = '';
        i = iIndex;
        do  i = iIndex to iIndex + cchLength
            ch = substr(sMem, i*2 + 9, 2);
            if (fStoppAtNull) then
                if (ch = '00') then
                    leave;
            sStr = sStr||x2c(ch);
        end
        return sStr;
    end
    say 'error-memWord: access out of range.'
return '';


/**
 * Dumps a byte range of the given memory to screen.
 * @return  0 on success. -1 on failure.
 * @paran   iIndex      Index into the memory block.
 * @paran   cbLength    Length to dump.
 * @paran   sMem        Memory block.
 */
memDumpByte: procedure
parse arg iIndex, cbLength, sMem
    cb = memSize(sMem);
    if (iIndex < cb & iIndex + cbLength <= cb) then
    do
        iOff = 0;
        do while (cbLength > 0)
            i = 0;
            sLine = '0000:'||d2x(iOff,8);
            do i = 0 to 15
                if (cbLength - i > 0) then
                do
                    if (i = 8) then
                        sLine = sLine || '-' || d2x(memByte(i + iOff, sMem),2);
                    else
                        sLine = sLine || ' ' || d2x(memByte(i + iOff, sMem),2);
                end
                else
                    sLine = sLine || '   ';
            end
            sLine = sLine || ' ';
            do i = 0 to 15
                if (cbLength - i <= 0) then
                    leave;
                iCh = memByte(i + iOff, sMem);
                if (iCh >= 32) then
                    sLine = sLine || d2c(iCh);
                else
                    sLine = sLine || '.';
            end
            say sLine
            iOff = iOff + 16;
            cbLength = cbLength - 16;
        end

        return 0;
    end
    say 'error-memDumpByte: access out of range.'
return -1;


/**
 * Dumps a word range of the given memory to screen.
 * @return  0 on success. -1 on failure.
 * @paran   iIndex      Index into the memory block.
 * @paran   cbLength    Length to dump.
 * @paran   sMem        Memory block.
 */
memDumpWord: procedure
parse arg iIndex, cbLength, sMem
    cb = memSize(sMem);
    if (iIndex < cb & iIndex + cbLength <= cb) then
    do
        iOff = 0;
        do while (cbLength > 0)
            i = 0;
            sLine = '0000:'||d2x(iOff,8)||' ';
            do i = 0 to 7
                if (cbLength - i > 0) then
                    sLine = sLine || ' ' || d2x(memWord(i*2 + iOff, sMem),4);
                else
                    sLine = sLine || '   ';
            end

            say sLine
            iOff = iOff + 16;
            cbLength = cbLength - 16;
        end

        return 0;
    end
    say 'error-memDumpWord: access out of range.'
return -1;


/**
 * Dumps a dword range of the given memory to screen.
 * @return  0 on success. -1 on failure.
 * @paran   iIndex      Index into the memory block.
 * @paran   cbLength    Length to dump.
 * @paran   sMem        Memory block.
 */
memDumpDword: procedure
parse arg iIndex, cbLength, sMem
    cb = memSize(sMem);
    if (iIndex < cb & iIndex + cbLength <= cb) then
    do
        iOff = 0;
        do while (cbLength > 0)
            i = 0;
            sLine = '0000:'||d2x(iOff, 8)||' ';
            do i = 0 to 3
                if (cbLength - i > 0) then
                    sLine = sLine || ' ' || d2x(memDWord(i*4 + iOff, sMem),8);
                else
                    sLine = sLine || '   ';
            end

            say sLine
            iOff = iOff + 16;
            cbLength = cbLength - 16;
        end

        return 0;
    end
    say 'error-memDumpDword: access out of range.'
return -1;


/**
 * Copies a portion of a memory block.
 * @param   iIndex      Index into the memory block.
 * @param   cbLength    Bytes to copy.
 * @param   sMem        Source block.
 */
memCopy: procedure;
parse arg iIndex, cbLength, sMem
    cb = memSize(sMem);
    if (iIndex < cb & iIndex + cbLength <= cb) then
    do
        sCopy = d2x(cbLength,8)||substr(sMem, 9 + iIndex * 2, cbLength * 2);
        say 'dbg-memCopy: 'left(sCopy,8);
        return sCopy
    end
    say 'error-memCopy: access out of range.';
return -1;


/**
 * Gets the size of a memory block.
 * @param   sMem    The memory block in question.
 */
memSize: procedure;
parse arg sMem
/* debug assertions - start - comment out when stable! */
if (length(sMem) - 8 <> x2d(left(sMem, 8)) * 2) then
do
    say 'fatal assert: memSize got a bad memoryblock'
    say '              length(sMem) =' length(sMem);
    say '              cb = ' x2d(left(sMem,8));
    exit(16);
end
/* debug assertions -  end  - comment out when stable! */
return x2d(left(sMem,8));


/**
 * Novaluehandler.
 */
SignalHanlder_NoValue:
    say 'fatal error: novalue signal SIGL='SIGL;
exit(16);

