/**/


/*
 * Init stuff.
 */
signal on NoValue Name SignalHanlder_NoValue;
NUMERIC DIGITS 11


/*
 * Globals
 */
sGlobals = 'ulHandleTable aProc. sGlobals';
ulHandleTable = 0;
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
     * pmsems
     */
    when (sCmd = 'pmsemcheck') then
        return pmsemCheck(sArgs);
    when (sCmd = 'pmsemdump') then
        return pmsemDump(sArgs);
    when (sCmd = 'pmsemdumpall') then
        return PmsemDumpAll(sArgs);

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
     * PM stuff
     */
    when (sCMD = 'pmstatus') then
        return PmStatus(sArgs);

    /*
     * Generic dump
     */
    when (sCmd = 'dump' | sCmd = '.d') then
    do
        parse var sArgs sStruct sDumperArgs
        select
            when (sStruct = 'mq') then
                return MqDump(sDumperArgs);
            when (sStruct = 'pmsem') then
                return PmsemDump(sDumperArgs);
            when (sStruct = 'qmsg') then
                return QmsgDump(sDumperArgs);
            when (sStruct = 'sms') then
                return SmsDump(sDumperArgs);
            when (sStruct = 'sqmsg') then
                return SqmsgDump(sDumperArgs);
            when (sStruct = 'wnd') then
                return WndDump(sDumperArgs);

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
 * Dump a number of pm/gre sems.
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
        do i = 0 to cCount - 1
            call pmsemDump1 memCopy(i * pmsemSize(), pmsemSize(), sMem);
        end
    end
    else
        say 'error: failed to read semaphore table.';
return -1;


/**
 * Dumps all PM/GRE sems
 * @returns 0 on success. -1 on error.
 */
PmsemDumpAll: procedure expose(sGlobals)
    /* read memory and do the dump */
    sMem = dfReadMem('pmsemaphores', 35 * pmsemSize())
    if (sMem <> '') then
    do
        do i = 0 to 34
            call pmsemDump1 memCopy(i * pmsemSize(), pmsemSize(), sMem),, i;
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
wndReserved0:       procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('20'), sMem);
wndReserved1:       procedure expose(sGlobals); parse arg sMem;      return memDword(x2d('24'), sMem);
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
    say '     Reserved0:' d2x(wndReserved0(sMem),8);
    say '     Reserved1:' d2x(wndReserved1(sMem),8);
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
    say '         ulWPS:' d2x(wndWPSULong(sMem),8) '('dfNear('%'d2x(wndWPSULong(sMem),8))')'
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
mqSize:             procedure expose(sGlobals);                     return x2d('b0');
mqNext:             procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('00'), sMem);
mqEntrySize:        procedure expose(sGlobals); parse arg sMem;     return memWord(x2d('04'), sMem);
/*mqQueue:            procedure expose(sGlobals);  parse arg sMem;     return memWord(x2d('06'), sMem);*/
mqMessages:         procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('08'), sMem);
/* ?? */
mqMaxMessages:      procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('0c'), sMem);
mqPid:              procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('18'), sMem);
mqTid:              procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('1c'), sMem);
mqFirstMsg:         procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('20'), sMem);
mqLastMsg:          procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('24'), sMem);
mqSGid:             procedure expose(sGlobals); parse arg sMem;     return memWord(x2d('28'), sMem);
mqHev:              procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('2c'), sMem);
mqSent:             procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('3c'), sMem);
mqCurrent:          procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('40'), sMem);

mqBadPwnd:          procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('68'), sMem);
mqBadQueue:         procedure expose(sGlobals); parse arg sMem;     return memByte(x2d('6c'), sMem);
mqCountTimers:      procedure expose(sGlobals); parse arg sMem;     return memByte(x2d('6d'), sMem);
mqHeap:             procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('70'), sMem);
mqHAccel:           procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('74'), sMem);

mqShutdown:         procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('90'), sMem);

mqRcvdSMSList:      procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('98'), sMem);
mqSlot:             procedure expose(sGlobals); parse arg sMem;     return memDword(x2d('a0'), sMem);

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
 * SENDMSG STRUCTRURE (SMS)
 * SENDMSG STRUCTRURE (SMS)
 * SENDMSG STRUCTRURE (SMS)
 * SENDMSG STRUCTRURE (SMS)
 * SENDMSG STRUCTRURE (SMS)
 * SENDMSG STRUCTRURE (SMS)
 * SENDMSG STRUCTRURE (SMS)
 */
smsSize:            procedure expose(sGlobals);                     return 64;
smsNextMaster:      procedure expose(sGlobals); parse arg sMem;     return memDword(0, sMem);
smsSendHead:        procedure expose(sGlobals); parse arg sMem;     return memDword(4, sMem);
smsReserved0:       procedure expose(sGlobals); parse arg sMem;     return memDword(8, sMem);
smsReceiveNext:     procedure expose(sGlobals); parse arg sMem;     return memDword(12, sMem);
smsTime:            procedure expose(sGlobals); parse arg sMem;     return memDword(16, sMem);
smsSenderPMQ:       procedure expose(sGlobals); parse arg sMem;     return memDword(20, sMem);
smsReceiverPMQ:     procedure expose(sGlobals); parse arg sMem;     return memDword(24, sMem);
smsResult:          procedure expose(sGlobals); parse arg sMem;     return memDword(28, sMem);
smsReserved1:       procedure expose(sGlobals); parse arg sMem;     return memDword(32, sMem);
smsPWND:            procedure expose(sGlobals); parse arg sMem;     return memDword(36, sMem);
smsMsgId:           procedure expose(sGlobals); parse arg sMem;     return memDword(40, sMem);
smsMP1:             procedure expose(sGlobals); parse arg sMem;     return memDword(44, sMem);
smsMP2:             procedure expose(sGlobals); parse arg sMem;     return memDword(48, sMem);
smsReserved2:       procedure expose(sGlobals); parse arg sMem;     return memDword(52, sMem);
smsReserved3:       procedure expose(sGlobals); parse arg sMem;     return memDword(56, sMem);
smsReserved4:       procedure expose(sGlobals); parse arg sMem;     return memDword(60, sMem);

/** Dumps one sms structure */
smsDump1: procedure expose(sGlobals)
parse arg sMem
    say ' psmsMasterNext:' d2x(smsNextMaster(sMem),8)
    say '   psmsSendHead:' d2x(smsSendHead(sMem),8)
    say '      Reserved0:' d2x(smsReserved0(sMem),8)
    say 'psmsReceiveNext:' d2x(smsReceiveNext(sMem),8)
    say '           Time:' d2x(smsTime(sMem),8)
    say '      pmqSender:' d2x(smsSenderPMQ(sMem),8)
    say '    pmqReceiver:' d2x(smsReceiverPMQ(sMem),8)
    say '       ulResult:' d2x(smsResult(sMem),8)
    say '      Reserved1:' d2x(smsReserved1(sMem),8)
    say '           pWnd:' d2x(smsPWND(sMem),8)
    say '        ulMsgId:' d2x(smsMsgId(sMem),8) '('msgMsgIdToText(smsMsgId(sMem))')'
    say '            MP1:' d2x(smsMP1(sMem),8)
    say '            MP2:' d2x(smsMP2(sMem),8)
    say '      Reserved2:' d2x(smsReserved2(sMem),8)
    say '      Reserved3:' d2x(smsReserved3(sMem),8)
    say '      Reserved4:' d2x(smsReserved4(sMem),8)
return 0;


/**
 * Send message struct (SMS) dumper.
 * @param   sAddr   Address expression of a sms struct.
 * @returns 0
 */
SmsDump: procedure expose(sGlobals)
parse arg sAddr cCount
    /*defaults and param validation */
    if (cCount = '' | datatype(cCount) <> 'NUM') then
        cCount = 1;
    if (sAddr = '') then
        signal SyntaxError

    /* read memory */
    sMem = dfReadMem(sAddr, cCount * smsSize())
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        do i = 0 to cCount - 1
            call smsDump1 memCopy(i * smsSize(), smsSize(), sMem);
        end
    end
    else
        say 'error: failed to read SMS structure at '''sAddr'''.';
return 0;


/*
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 * PM QUEUE MESSAGE STRUCTURE (QMSG)
 */
qmsgSize:           procedure expose(sGlobals);                     return 32;
qmsgHwnd:           procedure expose(sGlobals); parse arg sMem;     return memDword(0, sMem);
qmsgMsgId:          procedure expose(sGlobals); parse arg sMem;     return memDword(4, sMem);
qmsgMP1:            procedure expose(sGlobals); parse arg sMem;     return memDword(8, sMem);
qmsgMP2:            procedure expose(sGlobals); parse arg sMem;     return memDword(12, sMem);
qmsgTime:           procedure expose(sGlobals); parse arg sMem;     return memDword(16, sMem);
qmsgPtlX:           procedure expose(sGlobals); parse arg sMem;     return memDword(20, sMem);
qmsgPtlY:           procedure expose(sGlobals); parse arg sMem;     return memDword(24, sMem);
qmsgReserved:       procedure expose(sGlobals); parse arg sMem;     return memDword(28, sMem);

qmsgDump1: procedure expose(sGlobals);
parse arg sMem
    say '     Hwnd:' d2x(qmsgHwnd(sMem),8)
    say '    MsgId:' d2x(qmsgMsgId(sMem),8) '('msgMsgIdToText(qmsgMsgId(sMem))')'
    say '      MP1:' d2x(qmsgMP1(sMem),8)
    say '      MP2:' d2x(qmsgMP2(sMem),8)
    say '     Time:' d2x(qmsgTime(sMem),8)
    say '    Ptl.x:' d2x(qmsgPtlX(sMem),8)
    say '    Ptl.y:' d2x(qmsgPtlY(sMem),8)
    say ' Reserved:' d2x(qmsgReserved(sMem),8)
return 0;


/**
 * Queue message struct (QMSG) dumper.
 * @param   sAddr   Address expression of a sms struct.
 * @returns 0
 */
QmsgDump: procedure expose(sGlobals)
parse arg sAddr cCount
    /*defaults and param validation */
    if (cCount = '' | datatype(cCount) <> 'NUM') then
        cCount = 1;
    if (sAddr = '') then
        signal SyntaxError

    /* read memory */
    sMem = dfReadMem(sAddr, cCount * qmsgSize())
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        do i = 0 to cCount - 1
            call qmsgDump1 memCopy(i * qmsgSize(), qmsgSize(), sMem);
        end
    end
    else
        say 'error: failed to read QMSG structure at '''sAddr'''.';
return 0;


/*
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 * PM SYSSTEM QUEUE MESSAGE STRUCTURE (SQMSG)
 */
sqmsgSize:           procedure expose(sGlobals);                     return 32;
sqmsgMsgId:          procedure expose(sGlobals); parse arg sMem;     return memDword(0, sMem);
sqmsgMP1:            procedure expose(sGlobals); parse arg sMem;     return memDword(4, sMem);
sqmsgMP2:            procedure expose(sGlobals); parse arg sMem;     return memDword(8, sMem);
sqmsgTime:           procedure expose(sGlobals); parse arg sMem;     return memDword(12, sMem);
sqmsgReserved0:      procedure expose(sGlobals); parse arg sMem;     return memDword(16, sMem);
sqmsgReserved1:      procedure expose(sGlobals); parse arg sMem;     return memDword(20, sMem);
sqmsgReserved2:      procedure expose(sGlobals); parse arg sMem;     return memDword(24, sMem);
sqmsgReserved3:      procedure expose(sGlobals); parse arg sMem;     return memDword(28, sMem);

sqmsgDump1: procedure expose(sGlobals);
parse arg sMem
    say '    MsgId:' d2x(sqmsgMsgId(sMem),8) '('msgMsgIdToText(sqmsgMsgId(sMem))')'
    say '      MP1:' d2x(sqmsgMP1(sMem),8)
    say '      MP2:' d2x(sqmsgMP2(sMem),8)
    say '     Time:' d2x(sqmsgTime(sMem),8)
    say 'Reserved0:' d2x(sqmsgReserved0(sMem),8)
    say 'Reserved1:' d2x(sqmsgReserved1(sMem),8)
    say 'Reserved2:' d2x(sqmsgReserved2(sMem),8)
    say 'Reserved3:' d2x(sqmsgReserved3(sMem),8)
return 0;


/**
 * System Queue message struct (SQMSG) dumper.
 * @param   sAddr   Address expression of a sqmsg struct.
 * @returns 0
 */
SqmsgDump: procedure expose(sGlobals)
parse arg sAddr cCount
    /*defaults and param validation */
    if (cCount = '' | datatype(cCount) <> 'NUM') then
        cCount = 1;
    if (sAddr = '') then
        signal SyntaxError

    /* read memory */
    sMem = dfReadMem(sAddr, cCount * sqmsgSize())
    if (sMem <> '') then
    do
        /* loop thru them all listing the taken/bogus ones */
        do i = 0 to cCount - 1
            call sqmsgDump1 memCopy(i * sqmsgSize(), sqmsgSize(), sMem);
        end
    end
    else
        say 'error: failed to read SQMSG structure at '''sAddr'''.';
return 0;


/*
 * MSG HELPERS
 * MSG HELPERS
 * MSG HELPERS
 * MSG HELPERS
 * MSG HELPERS
 * MSG HELPERS
 * MSG HELPERS
 * MSG HELPERS
 */

/**
 * translates a message ID into a message define string
 * @param   iMsgId  The message id in question.
 * @returns Symbol name.
 *          '' if unknown.
 */
msgMsgIdToText: procedure
parse arg iMsgId
    select
        when (iMsgId = x2d('0000')) then return 'WM_NULL';
        when (iMsgId = x2d('0001')) then return 'WM_CREATE';
        when (iMsgId = x2d('0002')) then return 'WM_DESTROY';
        /*when (iMsgId == x2d('0003')) then return '';*/
        when (iMsgId = x2d('0004')) then return 'WM_ENABLE';
        when (iMsgId = x2d('0005')) then return 'WM_SHOW';
        when (iMsgId = x2d('0006')) then return 'WM_MOVE';
        when (iMsgId = x2d('0007')) then return 'WM_SIZE';
        when (iMsgId = x2d('0008')) then return 'WM_ADJUSTWINDOWPOS';
        when (iMsgId = x2d('0009')) then return 'WM_CALCVALIDRECTS';
        when (iMsgId = x2d('000a')) then return 'WM_SETWINDOWPARAMS';
        when (iMsgId = x2d('000b')) then return 'WM_QUERYWINDOWPARAMS';
        when (iMsgId = x2d('000c')) then return 'WM_HITTEST';
        when (iMsgId = x2d('000d')) then return 'WM_ACTIVATE';
        when (iMsgId = x2d('000f')) then return 'WM_SETFOCUS';
        when (iMsgId = x2d('0010')) then return 'WM_SETSELECTION';
        when (iMsgId = x2d('0011')) then return 'WM_PPAINT';
        when (iMsgId = x2d('0012')) then return 'WM_PSETFOCUS';
        when (iMsgId = x2d('0013')) then return 'WM_PSYSCOLORCHANGE';
        when (iMsgId = x2d('0014')) then return 'WM_PSIZE';
        when (iMsgId = x2d('0015')) then return 'WM_PACTIVATE';
        when (iMsgId = x2d('0016')) then return 'WM_PCONTROL';
        when (iMsgId = x2d('0020')) then return 'WM_COMMAND';
        when (iMsgId = x2d('0021')) then return 'WM_SYSCOMMAND';
        when (iMsgId = x2d('0022')) then return 'WM_HELP';
        when (iMsgId = x2d('0023')) then return 'WM_PAINT';
        when (iMsgId = x2d('0024')) then return 'WM_TIMER';
        when (iMsgId = x2d('0025')) then return 'WM_SEM1';
        when (iMsgId = x2d('0026')) then return 'WM_SEM2';
        when (iMsgId = x2d('0027')) then return 'WM_SEM3';
        when (iMsgId = x2d('0028')) then return 'WM_SEM4';
        when (iMsgId = x2d('0029')) then return 'WM_CLOSE';
        when (iMsgId = x2d('002a')) then return 'WM_QUIT';
        when (iMsgId = x2d('002b')) then return 'WM_SYSCOLORCHANGE';
        when (iMsgId = x2d('002d')) then return 'WM_SYSVALUECHANGED';
        when (iMsgId = x2d('002e')) then return 'WM_APPTERMINATENOTIFY';
        when (iMsgId = x2d('002f')) then return 'WM_PRESPARAMCHANGED';
        when (iMsgId = x2d('0030')) then return 'WM_CONTROL';
        when (iMsgId = x2d('0031')) then return 'WM_VSCROLL';
        when (iMsgId = x2d('0032')) then return 'WM_HSCROLL';
        when (iMsgId = x2d('0033')) then return 'WM_INITMENU';
        when (iMsgId = x2d('0034')) then return 'WM_MENUSELECT';
        when (iMsgId = x2d('0035')) then return 'WM_MENUEND';
        when (iMsgId = x2d('0036')) then return 'WM_DRAWITEM';
        when (iMsgId = x2d('0037')) then return 'WM_MEASUREITEM';
        when (iMsgId = x2d('0038')) then return 'WM_CONTROLPOINTER';
        when (iMsgId = x2d('003a')) then return 'WM_QUERYDLGCODE';
        when (iMsgId = x2d('003b')) then return 'WM_INITDLG';
        when (iMsgId = x2d('003c')) then return 'WM_SUBSTITUTESTRING';
        when (iMsgId = x2d('003d')) then return 'WM_MATCHMNEMONIC';
        when (iMsgId = x2d('003e')) then return 'WM_SAVEAPPLICATION';
        when (iMsgId = x2d('0490')) then return 'WM_SEMANTICEVENT';
        when (iMsgId = x2d('1000')) then return 'WM_USER';
        when (iMsgId = x2d('007e')) then return 'WM_VRNDISABLED';
        when (iMsgId = x2d('007f')) then return 'WM_VRNENABLED';
        when (iMsgId = x2d('007a')) then return 'WM_CHAR';
        when (iMsgId = x2d('007b')) then return 'WM_VIOCHAR';
        when (iMsgId = x2d('0070')) then return 'WM_MOUSEMOVE';
        when (iMsgId = x2d('0071')) then return 'WM_BUTTON1DOWN';
        when (iMsgId = x2d('0072')) then return 'WM_BUTTON1UP';
        when (iMsgId = x2d('0073')) then return 'WM_BUTTON1DBLCLK';
        when (iMsgId = x2d('0074')) then return 'WM_BUTTON2DOWN';
        when (iMsgId = x2d('0075')) then return 'WM_BUTTON2UP';
        when (iMsgId = x2d('0076')) then return 'WM_BUTTON2DBLCLK';
        when (iMsgId = x2d('0077')) then return 'WM_BUTTON3DOWN';
        when (iMsgId = x2d('0078')) then return 'WM_BUTTON3UP';
        when (iMsgId = x2d('0079')) then return 'WM_BUTTON3DBLCLK';
        when (iMsgId = x2d('007D')) then return 'WM_MOUSEMAP';
        when (iMsgId = x2d('0410')) then return 'WM_CHORD';
        when (iMsgId = x2d('0411')) then return 'WM_BUTTON1MOTIONSTART';
        when (iMsgId = x2d('0412')) then return 'WM_BUTTON1MOTIONEND';
        when (iMsgId = x2d('0413')) then return 'WM_BUTTON1CLICK';
        when (iMsgId = x2d('0414')) then return 'WM_BUTTON2MOTIONSTART';
        when (iMsgId = x2d('0415')) then return 'WM_BUTTON2MOTIONEND';
        when (iMsgId = x2d('0416')) then return 'WM_BUTTON2CLICK';
        when (iMsgId = x2d('0417')) then return 'WM_BUTTON3MOTIONSTART';
        when (iMsgId = x2d('0418')) then return 'WM_BUTTON3MOTIONEND';
        when (iMsgId = x2d('0419')) then return 'WM_BUTTON3CLICK';
        when (iMsgId = x2d('0420')) then return 'WM_BEGINDRAG';
        when (iMsgId = x2d('0421')) then return 'WM_ENDDRAG';
        when (iMsgId = x2d('0422')) then return 'WM_SINGLESELECT';
        when (iMsgId = x2d('0423')) then return 'WM_OPEN';
        when (iMsgId = x2d('0424')) then return 'WM_CONTEXTMENU';
        when (iMsgId = x2d('0425')) then return 'WM_CONTEXTHELP';
        when (iMsgId = x2d('0426')) then return 'WM_TEXTEDIT';
        when (iMsgId = x2d('0427')) then return 'WM_BEGINSELECT';
        when (iMsgId = x2d('0428')) then return 'WM_ENDSELECT';
        when (iMsgId = x2d('0429')) then return 'WM_PICKUP';
        /*when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '
        when (iMsgId = x2d('')) then return '*/
        when (iMsgId >= x2d('04c0') & iMsgId <= x2d('04ff')) then return 'WM_PENxxx';
        when (iMsgId >= x2d('0500') & iMsgId <= x2d('05ff')) then return 'WM_MMPMxxx';
        when (iMsgId >= x2d('0600') & iMsgId <= x2d('065f')) then return 'WM_STDDLGxxx';
        when (iMsgId >= x2d('0bd0') & iMsgId <= x2d('0bff')) then return 'WM_BIDIxxx';
        when (iMsgId >= x2d('0f00') & iMsgId <= x2d('0fff')) then return 'WM_HELPMGRxxx';
        otherwise
    end
return '';


/*
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 * PM
 */
PmStatus: procedure expose(sGlobals)
parse arg sArgs

    say 'PM Status:'
    say '     fBadAppDialog:' d2x(dfReadDword('fBadAppDialog'), 8)
    sMem = dfReadMem('qhpsBadApp', 8);
    say '        qhpsBadApp: tid='d2x(memWord(0, sMem), 4)','||,
                            'pid='d2x(memWord(2, sMem), 4)','||,
                            'flags='d2x(memWord(4, sMem), 4)','||,
                            'sgid='d2x(memWord(6, sMem), 4);
    say '- Focus & Locks -'
    pwndFocus   = dfReadDword('pwndfocus');
    say '         pwndFocus:' d2x(pwndFocus, 8);
    sMem = dfReadMem('%'||d2x(pwndFocus), wndSize());
    if (sMem <> '') then
    do
    say ' pwndFocus.hwnd   :' d2x(wndHwnd(sMem), 8);
        say ' pwndFocus.mq     :'    d2x(wndMsgQueue(sMem), 8);
        sMem = dfReadMem('%'||d2x(wndMsgQueue(sMem)), mqSize());
        if (sMem <> '') then
        do
    say ' pwndFocus.mq.slot:' d2x(mqSlot(sMem), 4);
    say ' pwndFocus.mq.tid :' d2x(mqTid(sMem), 8);
    say ' pwndFocus.mq.pid :' d2x(mqPid(sMem), 8);
        end
    end
    say '        pmqsyslock:' d2x(dfReadDword('pmqsyslock'), 8);
    say '        pmqVisLock:' d2x(dfReadDword('pmqVisLock'), 8)
    say '      pwndSysModal:' d2x(dfReadDword('pwndSysModal'), 8)
    say '          pmqTrack:' d2x(dfReadDword('pmqTrack'), 8)
    say '     pmqLockUpdate:' d2x(dfReadDword('pmqLockUpdate'), 8)
    say '- Event Receivers -'
    say '      pmqMouseWake:' d2x(dfReadDword('pmqMouseWake'), 8);
    say '        pmqKeyWake:' d2x(dfReadDword('pmqKeyWake'), 8)
    say '      pmqEventWake:' d2x(dfReadDword('pmqEventWake'), 8)
    say '- Lists -'
    say '         pSysqueue:' d2x(dfReadDword('pSysqueue'), 8)
    say '           pmqList:' d2x(dfReadDword('pmqList'), 8)
    say '- Misc Variables -'
    say '       pwndDesktop:' d2x(dfReadDword('pwndDesktop'), 8)
    say '        pwndObject:' d2x(dfReadDword('pwndObject'), 8)
    say '          pmqShell:' d2x(dfReadDword('pmqShell'), 8)
    say '         pmqShell2:' d2x(dfReadDword('pmqShell2'), 8)
    say '       pmqShutdown:' d2x(dfReadDword('pmqShutdown'), 8)
    say '         paAABRegs:' d2x(dfReadDword('paAABRegs'), 8)


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
