/**/


/*
 * Init stuff.
 */
signal on NoValue Name SignalHanlder_NoValue;
NUMERIC DIGITS 11


/*
 * Globals
 */
sGlobals = 'ulHandleTable sGlobals';
ulHandleTable = 0;


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
     * pmsems
     */
    when (sCmd = 'pmsemcheck') then
        return pmsemCheck(sArgs);
    when (sCmd = 'pmsemdump') then
        return pmsemDump(sArgs);

    /*
     * Windows Structures.
     */
    when (sCmd = 'wnddump') then
        return wndDump(sArgs);

    /*
     * Window handles.
     */
    when (sCmd = 'hwnd') then
        return hwnd2PWND(sArgs);

    /*
     * Generic dump
     */
    when (sCmd = 'dump' | sCmd = '.d') then
    do
        parse var sArgs sStruct sDumperArgs
        select
            when (sStruct = 'mq') then
                return MqDump(sDumperArgs);
            when (sStruct = 'wnd') then
                return WndDump(sDumperArgs);
            when (sStruct = 'pmsem') then
                return pmsemDump(sDumperArgs);

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
    say 'PMDF PM Rexx Utils v0.0.1';
    say 'syntax: %'sName' <command> [args]';
    say 'command:'
    say '       checksems       Check the PM semaphores';
return -1;

/* Procedure which we signals on user syntax errors. */
synatxerror:
    say 'syntax error!'
    call syntax;
return -1;



/*
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 * PMSEMS/GRESEMS
 */
/* access functions */
pmsemSize:      procedure expose(sGlobals); return 32;
pmsemIdent:     procedure expose(sGlobals); parse arg iSem, sMem; return memString(iSem * 32, 7, 1, sMem);
pmsem386:       procedure expose(sGlobals); parse arg iSem, sMem; return memByte( iSem * 32 +  7, sMem);
pmsemPid:       procedure expose(sGlobals); parse arg iSem, sMem; return memWord( iSem * 32 +  8, sMem);
pmsemTid:       procedure expose(sGlobals); parse arg iSem, sMem; return memWord( iSem * 32 + 10, sMem);
pmsemPTid:      procedure expose(sGlobals); parse arg iSem, sMem; return memDWord(iSem * 32 +  8, sMem);
pmsemNested:    procedure expose(sGlobals); parse arg iSem, sMem; return memDword(iSem * 32 + 12, sMem);
pmsemWaiting:   procedure expose(sGlobals); parse arg iSem, sMem; return memDword(iSem * 32 + 16, sMem);
pmsemUseCount:  procedure expose(sGlobals); parse arg iSem, sMem; return memDword(iSem * 32 + 20, sMem);/*debug*/
pmsemHEV:       procedure expose(sGlobals); parse arg iSem, sMem; return memDword(iSem * 32 + 24, sMem);
pmsemCallAddr:  procedure expose(sGlobals); parse arg iSem, sMem; return memDword(iSem * 32 + 28, sMem);/*debug*/


/**
 * Structure dumper.
 * @param   sSemMem     32 byte memory block (at least) containing the PMSEM to dump.
 * @parma   sMsg        Optional description message. (optional)
 * @param   iSem        The sem we're dumping. (optional)
 */
pmsemDump1: procedure expose(sGlobals)
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
 * Check if any of the PM sems are taken or have bogus state.
 * @returns 0 on success. -1 on error.
 */
PmsemCheck: procedure expose(sGlobals)
    sMem = dfReadMem('pmsemaphores', 35 * pmsemSize())
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
                call pmsemDump1 memCopy(iSem * pmsemSize(), pmsemSize(), sMem), sMsg, iSem;
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


/**
 * Check if any of the PM sems are taken or have bogus state.
 * @returns 0 on success. -1 on error.
 */
PmsemDump: procedure expose(sGlobals)
parse arg sAddr  cCount
    /* defaults and param validation */
    if (cCount = '' | datatype(cCount) <> 'NUM') then
        cCount = 1;
    if (sAddr = '') then
        signal SyntaxError

    /* read memory and do the dump */
    sMem = dfReadMem(sAddr, cCount * pmsemSize())
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        do i = 0 to cCount - 1
            call pmsemDump1 memCopy(i * pmsemSize(), pmsemSize(), sMem);
        end
    end
    else
        say 'error: failed to read semaphore table.';
return -1;


/**
 * Checks a give PM sem is free and not bogus.
 * @returns 1 if free and not bogus.
 *          0 if taken.
 *          -1 if bogus.
 * @param   iSem    Semaphore index.
 * @param   sMem    Memory containging the semaphore array.
 *                  (If no array use iSem=0)
 */
pmsemValidate: procedure expose(sGlobals)
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
pmsemGetName: procedure expose(sGlobals)
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
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 * WINDOW STRUCTURE (WND)
 */
/* size and access functions */
wndSize:            procedure expose(sGlobals); return 144; /* guesswork! */

wndNext:            procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('00'), sMem);
wndParent:          procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('04'), sMem);
wndChild:           procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('08'), sMem);
wndOwner:           procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('0c'), sMem);
wndRecs:            procedure expose(sGlobals); parse arg iWord,sMem;return memWord( x2d('10') + iWord*2, sMem);
wndStyle:           procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('18'), sMem);
wndId:              procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('1c'), sMem);
wndMsgQueue:        procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('28'), sMem);
wndHWND:            procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('2c'), sMem);
wndModel:           procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('30'), sMem);
wndProc:            procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('34'), sMem);
wndThunkProc:       procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('38'), sMem);
wndPresParams:      procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('40'), sMem);
wndFocus:           procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('44'), sMem);
wndWPSULong:        procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('48'), sMem);
wndInstData:        procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('50'), sMem);
wndOpen32:          procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('58'), sMem);
/*
wndWord:            procedure expose(sGlobals); parse arg iWord,sMem;return memDword(96 + iWord*4, sMem);
*/
/** dump one wnd structure */
wndDump1: procedure expose(sGlobals)
parse arg sMem, sMsg
    if (sMsg <> '') then
        say sMsg
    say '      pwndNext:' d2x(wndNext(sMem),8);
    say '    pwndParent:' d2x(wndParent(sMem),8);
    say '     pwndChild:' d2x(wndChild(sMem),8);
    say '     pwndOwner:' d2x(wndOwner(sMem),8);
    say '     rcsWindow: xl='wndRecs(0, sMem)',yl='wndRecs(1, sMem),
                        'xr='wndRecs(2, sMem)',yr='wndRecs(3, sMem) '(decimal)'
    say '       ulStyle:' d2x(wndStyle(sMem),8);
    say '            id:' d2x(wndId(sMem),8);
    say '   pmqMsgQueue:' d2x(wndMsgQueue(sMem),8);
    say '          hwnd:' d2x(wndHWND(sMem),8);
    say '   fModel16bit:' d2x(wndModel(sMem),8);
    say '    pfnWinProc:' d2x(wndProc(sMem),8)  '('dfNear('%'d2x(wndProc(sMem),8))')'
    if (wndThunkProc(sMem) = 0) then
        say '  pfnThunkProc:' d2x(wndThunkProc(sMem),8)
    else
        say '  pfnThunkProc:' d2x(wndThunkProc(sMem),8) ' ('dfNear('%'d2x(wndThunkProc(sMem),8))')'
    say '   ppresParams:' d2x(wndPresParams(sMem),8);
    say '     pwndFocus:' d2x(wndFocus(sMem),8);
    say '         ulWPS:' d2x(wndWPSULong(sMem),8);
    say '     pInstData:' d2x(wndInstData(sMem),8);
    say '            ??:' d2x(memDword(x2d('54'), sMem),8);
    say '       pOpen32:' d2x(wndOpen32(sMem),8);
/*  This aint right!
    i = 0;
    do while (i < 12)
        say ' aulWin['d2x(i,2)'-'d2x(i+3,2)']: '||,
                             d2x(wndWord(i+0, sMem), 8) d2x(wndWord(i+1, sMem), 8),
                             d2x(wndWord(i+2, sMem), 8) d2x(wndWord(i+3, sMem), 8)
        i = i + 4;
    end
*/
return 0;


/**
 * Dump window structures.
 */
WndDump: procedure expose(sGlobals)
parse arg sAddr cCount
    /*defaults and param validation */
    if (cCount = '' | datatype(cCount) <> 'NUM') then
        cCount = 1;
    if (sAddr = '') then
        signal SyntaxError
    if (hwndIsHandle(sAddr)) then
    do
        ulPWND = hwnd2PWND(sAddr);
        if (ulPWND > 0) then
            sAddr = '%'d2x(ulPWND);
    end

    /* read memory */
    sMem = dfReadMem(sAddr, cCount * wndSize())
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        do i = 0 to cCount - 1
            call wndDump1 memCopy(i * wndSize(), wndSize(), sMem);
        end
    end
    else
        say 'error: failed to read window structure at '''sAddr'''.';
return 0;




/*
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 * WINDOW HANDLE (HWND)
 */
hwnd2PWND: procedure expose(sGlobals)
parse arg sHwnd sDummy
    ulIndex = x2d(right(sHwnd, 4));
    ulBase = hwndpHandleTable();
    if (ulBase = 0) then
        return 0;

    ulHandleEntry = ulBase + ulIndex * 8 + 32;
return dfReadDword('%'d2x(ulHandleEntry, 8), 4);


/**
 * Checks if a value is a hwnd.
 * @returns true/false.
 * @param   sValue  Value in question.
 */
hwndIsHandle: procedure expose(sGlobals)
parse arg sValue sDummy

    /* Paranoid check if this is a number or hex value or whatever*/
    sValue = strip(sValue);
    if (sValue = '') then
        return 0;
    if (datatype(sValue) <> 'NUM') then
    do  /* assumes kind of hexx */
        sValue = translate(sValue);
        if (left(sValue, 2) = '0X') then
            sValue = substr(sValue, 3);
        if (right(sValue,1) = 'H') then
            sValue = substr(sValue, 1, length(sValue) - 1);
        if (sValue = '') then
            return 0;
        if (strip(translate(sValue, '', '123456767ABCDEF')) <> '') then
            return 0;
        ulValue = x2d(sValue);
    end
    else
    do  /* check if decimal value, if not try hex */
        if (sValue >= x2d('80000001') & sValue < x2d('8000ffff')) then
            return 1;
         ulValue = x2d(sValue);
    end

    /* Check for valid handle range. */
return ulValue >= x2d('80000001') & ulValue < x2d('8000ffff');


/**
 * Gets the pointer to the handle table.
 */
hwndpHandleTable: procedure expose(sGlobals)
    if (ulHandleTable > 0) then
        return ulHandleTable;

    ulHandleTable = dfReadDword('pHandleTable', 4);
    if (ulHandleTable > 0) then
        return ulHandleTable
    say 'error-hwndpHandleTable: failed to read pHandleTable';
return 0;



/*
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 * MESSAGE QUEUE STRUCTURE (MQ)
 */
mqSize:             procedure expose(sGlobals);  return x2d('b0');
mqNext:             procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('00'), sMem);
mqEntrySize:        procedure expose(sGlobals);  parse arg sMem;     return memWord(x2d('04'), sMem);
/*mqQueue:            procedure expose(sGlobals);  parse arg sMem;     return memWord(x2d('06'), sMem);*/
mqMessages:         procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('08'), sMem);
/* ?? */
mqMaxMessages:      procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('0c'), sMem);
mqTid:              procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('18'), sMem);
mqPid:              procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('1c'), sMem);
mqFirstMsg:         procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('20'), sMem);
mqLastMsg:          procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('24'), sMem);
mqSGid:             procedure expose(sGlobals);  parse arg sMem;     return memWord(x2d('28'), sMem);
mqHev:              procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('2c'), sMem);
mqSent:             procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('3c'), sMem);
mqCurrent:          procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('40'), sMem);

mqBadPwnd:          procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('68'), sMem);
mqBadQueue:         procedure expose(sGlobals);  parse arg sMem;     return memByte(x2d('6c'), sMem);
mqCountTimers:      procedure expose(sGlobals);  parse arg sMem;     return memByte(x2d('6d'), sMem);
mqHeap:             procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('70'), sMem);
mqHAccel:           procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('74'), sMem);

mqShutdown:         procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('90'), sMem);

mqRcvdSMSList:      procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('98'), sMem);
mqSlot:             procedure expose(sGlobals);  parse arg sMem;     return memDword(x2d('a0'), sMem);

/** dump one mq structure */
mqDump1: procedure expose(sGlobals)
parse arg sMem;
    say '     pmqNext:' d2x(mqNext(sMem), 8);
    say '     cbEntry:' d2x(mqEntrySize(sMem), 8);
    say '   cMessages:' d2x(mqMessages(sMem), 8);
    say 'cMaxMessages:' d2x(mqMaxMessages(sMem), 8);
    say '         Tid:' d2x(mqTid(sMem), 8);
    say '         Pid:' d2x(mqPid(sMem), 8);
    say 'psmsFirstMsg:' d2x(mqFirstMsg(sMem), 8);
    say ' psmsLastMsg:' d2x(mqLastMsg(sMem), 8);
    say '        SGId:' d2x(mqSGid(sMem), 8);
    say '         hev:' d2x(mqHev(sMem), 8);
    say '    psmsSent:' d2x(mqSent(sMem), 8);
    say ' psmsCurrent:' d2x(mqCurrent(sMem), 8);
    say '     pwndBad:' d2x(mqBadPWND(sMem), 8);
    say '   fBadQueue:' d2x(mqBadQueue(sMem), 2);
    say '     cTimers:' d2x(mqCountTimers(sMem), 2);
    say '       pHeap:' d2x(mqHeap(sMem), 8);
    say '      HACCEL:' d2x(mqHAccel(sMem), 8);
    say ' fchShutdown:' d2x(mqShutdown(sMem), 2);
    say ' RcvdSMSList:' d2x(mqRcvdSMSList(sMem), 8);
    say '        Slot:' d2x(mqSlot(sMem), 4);
return 0;


/**
 * Message queue dumper.
 * @param   sAddr   Address expression of a MQ struct, or a window
 *                  which message queue you wanna dump.
 * @returns 0
 */
mqDump: procedure expose(sGlobals)
parse arg sAddr cCount
    /*defaults and param validation */
    if (cCount = '' | datatype(cCount) <> 'NUM') then
        cCount = 1;
    if (sAddr = '') then
        signal SyntaxError

    /*
     * The user might have passed in an window handle.
     * If so we'll dump it's queue.
     */
    if (hwndIsHandle(sAddr)) then
    do  /* input is a hwnd, we will try dump it's queue */
        ulPWND = hwnd2PWND(sAddr);
        if (ulPWND > 0) then
        do
            sMem = dfReadMem('%'d2x(ulPWND), wndSize());
            if (sMem <> '') then
            do
                ulMQ = wndMsgQueue(sMem);
                if (ulMq > 0) then
                    sAddr = '%'d2x(ulPWND);
            end
            drop sMem;
        end
    end

    /* read memory */
    sMem = dfReadMem(sAddr, cCount * mqSize())
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        do i = 0 to cCount - 1
            call mqDump1 memCopy(i * mqSize(), mqSize(), sMem);
        end
    end
    else
        say 'error: failed to read window structure at '''sAddr'''.';
return 0;




/*
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
 * PMDF WORKERS
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
dfReadDWord: procedure expose(sGlobals)
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
        parse var asOut.1 .' 'sRet;
        return strip(sRet);
    end
return '';


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
