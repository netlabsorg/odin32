/**/


/*
 * Init stuff.
 */
signal on NoValue Name SignalHanlder_NoValue;
NUMERIC DIGITS 11


/*
 * Globals
 */
sGlobals = 'ulShSemTbl aProc. sGlobals';
ulShSemTbl = 0;
aProc.0 = 0;                            /* process table */


/*
 * Args
 */
parse arg sCmd sArgs
sCmd = lowercase(sCmd);
sArg = lowercase(sArgs);
say '';


/*
 * Operation
 */
select
    /*
     *
     */
    when (sCmd = 'hsem') then
        return Hsem2psem(sArgs);
    when (sCmd = 'hsemdump') then
        return HsemDump(sArgs);

    /*
     * Sem32
     */
    when (sCmd = 'sem32check') then
        return Sem32Check(sArgs);


    /*
     * Generic dump
     */
    when (sCmd = 'dump' | sCmd = '.d') then
    do
        parse var sArgs sStruct sDumperArgs
        select

            otherwise
                say 'syntax error: no or invalid structure name.';
        return syntax(sArgs);
        end
    end


    /*
     * Help and syntax error.
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
    say 'PMDF SEM Rexx Utils v0.0.1';
    say 'syntax: %'sName' <command> [args]';
    say 'command:'
return -1;

/* Procedure which we signals on user syntax errors. */
synatxerror:
    say 'syntax error!'
    call syntax;
return -1;

/*
 * HSEM GENERIC SEM HANDLE WORKERS
 * HSEM GENERIC SEM HANDLE WORKERS
 * HSEM GENERIC SEM HANDLE WORKERS
 * HSEM GENERIC SEM HANDLE WORKERS
 * HSEM GENERIC SEM HANDLE WORKERS
 * HSEM GENERIC SEM HANDLE WORKERS
 */

/**
 * Checks if a handle is private or shared.
 * @param   iHandle     Decimal handle value.
 * @returns true/false
 */
hsemIsPrivate: procedure expose(sGlobals)
parse arg iHandle;
return iHandle < x2d('80000000'); /*??*/


/**
 * Returns the index part of the handle.
 * @param   iHandle     Decimal handle value.
 * @returns Index.
 */
hsemGetIndex: procedure expose(sGlobals)
parse arg iHandle;
return iHandle // 65536;


/**
 * Returns the index part of the handle.
 * @param   sHandle     Handle.
 * @returns Index.
 */
hsemIsHexHandle: procedure expose(sGlobals)
parse arg sHandle;
fResult = (left(sHandle, 4) = '8001') | (left(sHandle, 4) = '8003') | (left(sHandle, 4) = '0001');
return fResult;


/**
 * Get shared sem table base address
 * @returns ulShSemTbl.
 *          0/-1 on error.
 */
hsemShSemTbl: procedure expose(sGlobals)
    if (ulShSemTbl > 0) then
        return ulShSemTbl;
    ulShSemTbl = dfReadDword('_pShSemTbl');
return ulShSemTbl


/**
 * Converts a semaphore handle to a sem structure pointer.
 * @returns Linear address of sem structure.
 *          0 on failure.
 * @param   Semaphore handle.
 */
Hsem2psem: procedure expose(sGlobals)
parse arg sHSEM
    sHSEM = strip(sHSEM);
    /* argument stuff. */
    if (hsemIsHexHandle(sHSEM)) then
        iHandle = x2d(sHSEM);
    else
        iHandle = sHSEM;

    /* lookup the index */
    iIdx = hsemGetIndex(iHandle);
    ulSem = 0;
    if (hsemIsPrivate(iHandle)) then
    do  /* private sem lookup */
        ulPTDA = dfProcPTDA('#');
        if (dfReadDword('%'ulPTDA '+ (%ulPrTotUsed - %ptda_start)') > iIdx) then
            ulSem  = dfReadDword('%(dw(%'ulPTDA '+ (%pPrSemTbl - %ptda_start))+'d2x(iIdx*4)')');
        else
            say 'error-Hsem2psem: Invalid semaphore index. ('sHSEM')'
    end
    else
    do  /* global sem lookup */
        ulAddr = hsemShSemTbl() + iIdx*4;
        ulSem  = dfReadDword('%'d2x(ulAddr));
    end

    say 'Hsem2psem:' sHSEM '<=>' d2x(ulSem, 8)
return ulSem;


/**
 * Dump a sem based by handle value.
 * @param   sHSEM   Semaphore handle to dump.
 */
HsemDump: procedure expose(sGlobals)
parse arg sHSEM

    ulSem = hsem2psem(sHSEM);
    if (ulSem > 0) then
    do
        rc = sem32Dump1Ext(ulSem);
        return 0;
    end
return -1;


/**
 * Dumps a sem32 structure.
 * @param   ulAddr  Decimal address of the sem32 structure.
 */
sem32Dump1Ext: procedure expose(sGlobals)
parse arg ulAddr

    Address df 'CMD' 'asOut' '.d sem32 %'||d2x(ulAddr);

    do i = 1 to asOut.0;
        if (pos('ADDR:', translate(asOut.i)) > 0) then
        do
            sAddr = strip(substr(asOut.i, pos(':', asOut.i) + 1))
            if (x2d(sAddr) > 0) then
                asOut.i = asOut.i '('dfNear(sAddr)')';
        end
        if (pos(':', asOut.i) > 0) then
            say asOut.i;
    end
return 0;


/**
 * Reads a sem32 structure.
 * @param   ulAddr  Decimal address of the sem32 structure.
 */
sem32Read: procedure expose(sGlobals)
parse arg ulAddr
    sSem = '';
    Address df 'CMD' 'asOut' '.d sem32 %'||d2x(ulAddr);
    do i = 1 to asOut.0;
        asOut.i = strip(translate(asOut.i, '=', ': '||'0d0a'x));
        j = pos(' ', asOut.i);
        do while (j > 0)
            asOut.i = substr(asOut.i, 1, j-1) || substr(asOut.i, j+1);
            j = pos(' ', asOut.i);
        end
        if (pos('=', asOut.i) > 0) then
        do
            sSem = sSem||';'asOut.i;
        end
    end
    /*say 'dbg-sem32Read:' sSem;*/
return sSem;

/* access methods */
sem32IsEvent:       procedure expose(sGlobals); parse arg sSem;     return pos('Event;', sSem) > 0;
sem32IsMutex:       procedure expose(sGlobals); parse arg sSem;     return pos('Mutex;', sSem) > 0;
sem32IsPrivate:     procedure expose(sGlobals); parse arg sSem;     return pos('Private', sSem) > 0;
sem32IsShared:      procedure expose(sGlobals); parse arg sSem;     return pos('Shared', sSem) > 0;
sem32Flags:         procedure expose(sGlobals); parse arg sSem;
i = pos('Flags', sSem); if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '';
sem32CreateAddr:    procedure expose(sGlobals); parse arg sSem;
i = pos('CreateAddr', sSem); if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '';
sem32CallerAddr:    procedure expose(sGlobals); parse arg sSem;
i = pos('CallerAddr', sSem); if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '';
sem32MuxQ:          procedure expose(sGlobals); parse arg sSem;
i = pos('pMuxQ', sSem); if (i > 0) then         return substr(sSem, pos('=', sSem, i)+1, 4);
return '';
sem32OpenCt:        procedure expose(sGlobals); parse arg sSem;
i = pos('OpenCount', sSem); if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '00';

sem32mtxIsOwned:    procedure expose(sGlobals); parse arg sSem; return sem32mtxOwner(sSem) <> '0000';
sem32mtxOwner:      procedure expose(sGlobals); parse arg sSem;
i = pos('Owner', sSem);     if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '00';
sem32mtxRequestCt:  procedure expose(sGlobals); parse arg sSem;
i = pos('RequestCt', sSem); if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '00';
sem32mtxRequesterCt:procedure expose(sGlobals); parse arg sSem;
i = pos('RequesterCt', sSem);if (i > 0) then    return substr(sSem, pos('=', sSem, i)+1, 4);
return '00';

sem32evtIsPosted:   procedure expose(sGlobals); parse arg sSem; return sem32Flags(sSem) = 'Posted';
sem32evtIsReset:    procedure expose(sGlobals); parse arg sSem; return sem32Flags(sSem) = 'Reset';
sem32evtPostCount:  procedure expose(sGlobals); parse arg sSem;
i = pos('PostCount', sSem); if (i > 0) then     return substr(sSem, pos('=', sSem, i)+1, 4);
return '';


/**
 * Display all the sems which this process is holding.
 * Only private sems are implemented currently.
 */
Sem32Check: procedure  expose(sGlobals)
    ulPTDA = dfProcPTDA('#');

    cPr = dfReadDWord('%'ulPTDA '+ (ulPrTotUsed-ptda_start)');
    if (cPr > 0) then
    do
        sPrTabMem = dfReadMem('%(dw(%'ulPTDA '+(pPrSemTbl-ptda_start)))', cPr*4);
        if (sPrTabMem <> '') then
        do
            do i = 0 to cPr - 1
                if (i // 20 = 0) then
                    say 'info:' right(i-0,length(cPr)) 'of' cPr 'private sems processed...'
                ulSem = memDword(i*4, sPrTabMem);
                if (ulSem >= '10000'x) then
                do
                    sSem = sem32Read(ulSem);
                    if (sSem <> '') then
                    do
                        fOk = 1;
                        if (sem32IsMutex(sSem)) then
                        do /* mutex*/
                            if (sem32mtxIsOwned(sSem)) then
                                fOk = 0;
                        end
                        else if (sem32IsEvent(sSem)) then
                        do  /* event */
                            if (sem32evtIsReset(sSem)) then
                                fOk = 0;
                        end
                        else fOk = 0;
                        if (\fOk) then
                            call sem32Dump1Ext ulSem;
                    end
                end
            end
        end
        else
            say 'error-Sem32Check: failed to read private sem table.'
    end

return 0;



/*
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 * PMDF WORKERS - COMMON COMMON COMMON
 */



/**
 * Read memory.
 * @param   sStartExpr  Expression giving the address where to read from.
 * @param   cbLength    Number of _bytes_ to read.
 * @returns The memory we have read. (internal format!)
 */
dfReadMem: procedure expose(sGlobals)
parse arg sStartExpr, cbLength

    /* dump memory */
    if ((cbLength // 4) = 0) then
    do  /* optimized read */
        /*say 'dbg-df: dd' sStartExpr 'L'cbLength/4'T'*/
        Address df 'CMD' 'asOut' 'dd' sStartExpr 'L'cbLength/4'T'
        /*say 'dbg-df: rc='rc' asOut.0='asOut.0;*/
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
            if (j <> 0) then
                return d2x(j,8)||sMem;
        end

    end
    else
    do  /* slower (more output) byte by byte read */
        /*say 'dbg-df: db' sStartExpr 'L'cbLength'T'*/
        Address df 'CMD' 'asOut' 'db' sStartExpr 'L'cbLength'T'
        /*say 'dbg-df: rc='rc' asOut.0='asOut.0;*/
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
            if (j <> 0) then
                return d2x(j,8)||sMem;
        end
    end
return '';


/**
 * Reads a DWord at a given address.
 * @param   sAddr   Address expression.
 * @return  The value of the dword at given address.
 *          -1 on error.
 */
dfReadByte: procedure expose(sGlobals)
parse arg sAddr
    sMem = dfReadMem(sAddr, 4);
    if (sMem <> '') then
        return memByte(0, sMem);
return -1;


/**
 * Reads a Word at a given address.
 * @param   sAddr   Address expression.
 * @return  The value of the dword at given address.
 *          -1 on error.
 */
dfReadWord: procedure expose(sGlobals)
parse arg sAddr
    sMem = dfReadMem(sAddr, W);
    if (sMem <> '') then
        return memWord(0, sMem);
return -1;


/**
 * Reads a DWord at a given address.
 * @param   sAddr   Address expression.
 * @return  The value of the dword at given address.
 *          -1 on error.
 */
dfReadDword: procedure expose(sGlobals)
parse arg sAddr
    sMem = dfReadMem(sAddr, 4);
    if (sMem <> '') then
        return memDword(0, sMem);
return -1;


/**
 * Get near symbol.
 * @param   sAddr   Address expression.
 * @return  Near output.
 *          '' on error.
 */
dfNear: procedure expose(sGlobals)
parse arg sAddr
    Address df 'CMD' 'asOut' 'ln' sAddr
    if (rc = 0 & asOut.0 > 0) then
    do
        if (pos('symbols found', asOut.1) <= 0) then
        do
            parse var asOut.1 .' 'sRet;
            return strip(sRet);
        end
    end
return '';


/**
 * Read all processes into global variable.
 */
dfProcessReadAll: procedure expose(sGlobals)
parse arg fBlockInfo
    if (\fBlockInfo) then
    do
        say '[reading processes]'
        Address df 'CMD' 'asOut' '.p';
        say '[done]'
        if (rc = 0 & asOut.0 > 0) then
        do
            j = 0;
            do i = 1 to asOut.0
                if (word(asOut.i,1) = 'Slot' | strip(asOut.i) = '') then
                    iterate;
                /*  0074  0033 0000 0033 0002 blk 0500 f88e6000 fe62d220 f9a0b7e8 1e9c 12 muglrqst
                 *  000a  0001 0000 0000 000a blk 081e f8812000 ffdba880 f99f7840 1e94 00 *jitdaem
                 * *000b# 001d 0001 001d 0001 blk 0500 f8814000 fe6270a0 f99f7b44 1e9c 01 pmshell
                 */
                j = j + 1;
                aProc.j.sType = '0';
                aProc.j.hxBlockId = '0';
                asOut.i = translate(left(asOut.i, 10), '  ', '#*') || substr(asOut.i, 11);
                parse var asOut.i aProc.j.hxSlot,
                                  aProc.j.hxPid,
                                  aProc.j.hxPPid,
                                  aProc.j.hxCsid,
                                  aProc.j.hxOrd,
                                  aProc.j.sState,
                                  aProc.j.hxPri,
                                  aProc.j.hxpTSD,
                                  aProc.j.hxpPTDA,
                                  aProc.j.hxpPCB,
                                  aProc.j.hxDisp,
                                  aProc.j.hxSG,
                                  aProc.j.sName;
                if (strip(aProc.j.hxSlot) = '') then
                    j = j - 1;
            end
            aProc.0 = j;
        end
    end
    else
    do
        say '[reading processes]'
        Address df 'CMD' 'asOut' '.pb';
        say '[done]'
        if (rc = 0 & asOut.0 > 0) then
        do
            j = 0;
            do i = 1 to asOut.0
                if (word(asOut.i,1) = 'Slot' | strip(asOut.i) = '') then
                    iterate;
                /*  0044  blk fd436cf8 4os2     Sem32     8001 005d hevResultCodeSet
                 * *000b# blk fd436190 pmshell
                 *  0073  blk 0b008cbe msrv     SysSem
                 */
                asOut.i = translate(left(asOut.i, 10), '  ', '#*') || substr(asOut.i, 11);
                j = j + 1;
                aProc.j.hxPid = '0';
                aProc.j.hxPPid = '0';
                aProc.j.hxCsid = '0';
                aProc.j.hxOrd = '0';
                aProc.j.hxPri = '0';
                aProc.j.hxpTSD = '0';
                aProc.j.hxpPTDA = '0';
                aProc.j.hxpPCB = '0';
                aProc.j.hxDisp = '0';
                aProc.j.hxSG = '0';
                parse var asOut.i aProc.j.hxSlot,
                                  aProc.j.sState,
                                  aProc.j.hxBlockId,
                                  aProc.j.sName,
                                  aProc.j.sType .;
                if (strip(aProc.j.hxSlot) = '') then
                    j = j - 1;
            end
            aProc.0 = j;
        end
    end
return -1;


/**
 * Gets the blockId of a process from the dumpformatter.
 * @param   iSlot   The slot to query.
 * @returns Block id (hex string).
 *          '0' if failure.
 */
dfProcessBlockId: procedure expose(sGlobals)
parse arg iSlot
    Address df 'CMD' 'asOut' '.pb' iSlot;
    if (rc = 0 & asOut.0 > 0) then
    do
        /* *000b# blk fd436190 pmshell */
        asOut.2 = strip(asOut.2);
        parse var asOut.2 .' 'sState' 'sBlockId' 'sProcName
        sBlockId = strip(sBlockId)      /* needed??? */
        if (sBlockId <> '') then
            return sBlockId;
    end
return '0';


/**
 * Gets the PTDA of a process.
 * @param   sSlot   Slot or special chars '*' and '#'.
 * @return  Hex pointer to the PTDA.
 */
dfProcPTDA: procedure expose(sGlobals)
parse arg iSlot
    Address df 'CMD' 'asOut' '.p' iSlot;
    if (rc = 0 & asOut.0 > 0) then
    do
        /*  0074  0033 0000 0033 0002 blk 0500 f88e6000 fe62d220 f9a0b7e8 1e9c 12 muglrqst
         *  000a  0001 0000 0000 000a blk 081e f8812000 ffdba880 f99f7840 1e94 00 *jitdaem
         * *000b# 001d 0001 001d 0001 blk 0500 f8814000 fe6270a0 f99f7b44 1e9c 01 pmshell
         */
        i = 2;
        asOut.i = translate(left(asOut.i, 10), '  ', '#*') || substr(asOut.i, 11);
        parse var asOut.i . . . . . . . hxTSD hxPTDA hxPCB . . .;
        hxPTDA = strip(hxPTDA)      /* needed??? */
        if (hxPTDA <> '') then
            return hxPTDA;
    end
return '0';


/**
 * Gets a byte from the memory array aMem.
 * @param   iIndex      Byte offset into the array.
 */
memByte: procedure expose(sGlobals)
parse arg iIndex, sMem
    cb = memSize(sMem);
    if (iIndex < cb) then
    do
        return x2d(substr(sMem, (iIndex * 2) + 9 + 0, 2));
    end
    say 'error-memByte: access out of range. cb='cb ' iIndex='iIndex;
return -1;


/**
 * Gets a word from the memory array aMem.
 * @param   iIndex      Byte offset into the array.
 */
memWord: procedure expose(sGlobals)
parse arg iIndex, sMem
    cb = memSize(sMem);
    if (iIndex + 1 < cb) then
    do
        return x2d(substr(sMem, (iIndex * 2) + 9 + 2, 2)||,
                   substr(sMem, (iIndex * 2) + 9 + 0, 2));
    end
    say 'error-memWord: access out of range. cb='cb ' iIndex='iIndex;
return -1;


/**
 * Gets a dword from the passed in memory block.
 * @param   iIndex      Byte offset into the array.
 * @param   sMem        Memory block.
 * @remark  note problems with signed!
 */
memDword: procedure expose(sGlobals)
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
    say 'error-memDword: access out of range. cb='cb ' iIndex='iIndex;
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
memString: procedure expose(sGlobals)
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
    say 'error-memWord: access out of range. cb='cb ' cbLength='cbLength;
return '';


/**
 * Dumps a byte range of the given memory to screen.
 * @return  0 on success. -1 on failure.
 * @paran   iIndex      Index into the memory block.
 * @paran   cbLength    Length to dump.
 * @paran   sMem        Memory block.
 */
memDumpByte: procedure expose(sGlobals)
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
    say 'error-memDumpByte: access out of range. cb='cb 'iIndex='iIndex 'cbLength='cbLength;
return -1;


/**
 * Dumps a word range of the given memory to screen.
 * @return  0 on success. -1 on failure.
 * @paran   iIndex      Index into the memory block.
 * @paran   cbLength    Length to dump.
 * @paran   sMem        Memory block.
 */
memDumpWord: procedure expose(sGlobals)
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
    say 'error-memDumpWord: access out of range. cb='cb ' cbLength='cbLength;
return -1;


/**
 * Dumps a dword range of the given memory to screen.
 * @return  0 on success. -1 on failure.
 * @paran   iIndex      Index into the memory block.
 * @paran   cbLength    Length to dump.
 * @paran   sMem        Memory block.
 */
memDumpDword: procedure expose(sGlobals)
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
    say 'error-memDumpDword: access out of range. cb='cb ' cbLength='cbLength;
return -1;


/**
 * Copies a portion of a memory block.
 * @param   iIndex      Index into the memory block.
 * @param   cbLength    Bytes to copy.
 * @param   sMem        Source block.
 */
memCopy: procedure expose(sGlobals)
parse arg iIndex, cbLength, sMem
    cb = memSize(sMem);
    if (iIndex < cb & iIndex + cbLength <= cb) then
    do
        sCopy = d2x(cbLength,8)||substr(sMem, 9 + iIndex * 2, cbLength * 2);
        return sCopy
    end
    say 'error-memCopy: access out of range. cb='cb ' cbLength='cbLength;
return -1;


/**
 * Gets the size of a memory block.
 * @param   sMem    The memory block in question.
 */
memSize: procedure expose(sGlobals)
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
 * Dump all processes. (debug more or less)
 */
procDumpAll: procedure expose(sGlobals)
    say 'Processes:'
    do i = 1 to aProc.0
        say 'slot='aProc.i.hxSlot 'pid='aProc.i.hxPid 'blkid='aProc.i.hxBlockId 'name='aProc.i.sName
    end
return 0;


/**
 * Searches thru the process list looking for a process
 * by it's pid and tid.
 * @returns Index of the process.
 * @param   pid     Process Id. (Decimal value)
 * @param   tid     Thread Id. (Decimal value)
 */
procFindByPidTid: procedure expose(sGlobals)
parse arg pid, tid
    do i = 1 to aProc.0
        if (x2d(aProc.i.hxPid) = pid & x2d(aProc.i.hxTid) = tid) then
            return i;
    end
return 0;


/**
 * Searches thru the process list looking for a process
 * by it's slot number.
 * @returns Index of the process.
 * @param   iSlot   Thread slot number. (Decimal value)
 */
procFindByPidTid: procedure expose(sGlobals)
parse arg iSlot
    do i = 1 to aProc.0
        if (x2d(aProc.i.hxSlot) = iSlot) then
            return i;
    end
return 0;


/**
 * Novaluehandler.
 */
SignalHanlder_NoValue:
    say 'fatal error: novalue signal SIGL='SIGL;
exit(16);


/**
 * Lowercases a string.
 * @param   sString     String to fold down.
 * @returns Lowercase version of sString.
 */
lowercase: procedure expose(sGlobals)
parse arg sString
return translate(sString,,
                 'abcdefghijklmnopqrstuvwxyz',,
                 'ABCDEFGHIJKLMNOPQRSTUVWXYZ');

