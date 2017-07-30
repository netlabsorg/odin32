/* REXX */
/*
 * Works around stupid mapsym dumbness regarding the output .sym path.
 *
 * (c) too simple to be copyrighted
 */

'@echo off'

if (RxFuncQuery('SysLoadFuncs')) then do
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs
end

parse arg aArgs
call TokenizeString aArgs, 'G.!args'

if (G.!args.0 < 3) then do
    say "Usage: runmapsym <mapsym_cmd> <map_file> <sym_file>"
    exit 255
end

aMapSymEXE = translate(G.!args.1, '\' , '/')
aMapFile = translate(G.!args.2, '\' , '/')
aSymFile = translate(G.!args.3, '\' , '/')

curDir = directory()

mapSymExeDir = filespec('D', aMapSymEXE)||filespec('P', aMapSymEXE)
if (mapSymExeDir \== '') then do
    if (right(mapSymExeDir, 2) \== ':\') then
        mapSymExeDir = strip(mapSymExeDir, 'T', '\')
    mapSymExeDir = directory(mapSymExeDir)
    if (mapSymExeDir == '') then do
        say 'ERROR: Directory of "'aMapSymEXE'" does not exist.'
        exit 255
    end
    if (right(mapSymExeDir, 2) \== ':\') then
        mapSymExeDir = mapSymExeDir'\'
    mapSymExe = mapSymExeDir||filespec('N', aMapSymEXE)
end
else do
    mapSymExe = aMapSymEXE
end

call directory curDir

mapFile = stream(aMapFile, 'C', 'QUERY EXISTS')
if (mapFile == '') then do
    say 'ERROR: Cannot find file "'aMapFile'".'
    exit 255
end

symDir = filespec('D', aSymFile)||filespec('P', aSymFile)
if (symDir == '') then symDir = '.'
else if (right(symDir, 2) \== ':\') then symDir = strip(symDir, 'T', '\')
symDir = directory(symDir)
if (symDir == '') then do
    say 'ERROR: Directory of "'aSymFile'" does not exist.'
    exit 255
end

symFile = fileSpec('N', aSymFile)

'call' mapSymExe mapFile

if (rc \== 0) then do
    say 'ERROR: Executing "'aMapSymExe'" failed with exit code 'rc'.'
end
else do
    dumbSymFile = filespec('N', mapFile)
    lastDot = lastpos('.', dumbSymFile)
    if (lastDot > 0) then dumbSymFile = left(dumbSymFile, lastDot - 1)
    dumbSymFile = dumbSymFile'.sym'
    if (translate(symFile) \== translate(dumbSymFile)) then do
        'del' symFile '1>nul 2>nul'
        'rename' dumbSymFile symFile
        if (rc \== 0) then do
            say 'ERROR: Renaming "'dumbSymFile'" to "'symFile'"',
                'failed with exit code 'rc'.'
        end
    end
end

call directory curDir

exit rc

/**
 *  Returns a list of all words from the string as a stem.
 *  Delimiters are spaces, tabs and new line characters.
 *  Words containg spaces must be enclosed with double
 *  quotes. Double quote symbols that need to be a part
 *  of the word, must be doubled.
 *
 *  @param string   the string to tokenize
 *  @param stem
 *      the name of the stem. The stem must be global
 *      (i.e. its name must start with 'G.!'), for example,
 *      'G.!wordlist'.
 *  @param leave_ws
 *      1 means whitespace chars are considered as a part of words they follow.
 *      Leading whitespace (if any) is always a part of the first word (if any).
 *
 *  @version 1.1
 */
TokenizeString: procedure expose G.

    parse arg string, stem, leave_ws
    leave_ws = (leave_ws == 1)

    delims  = '20090D0A'x
    quote   = '22'x /* " */

    num = 0
    token = ''

    len = length(string)
    last_state = '' /* D - in delim, Q - in quotes, W - in word */
    seen_QW = 0

    do i = 1 to len
        c = substr(string, i, 1)
        /* determine a new state */
        if (c == quote) then do
            if (last_state == 'Q') then do
                /* detect two double quotes in a row */
                if (substr(string, i + 1, 1) == quote) then i = i + 1
                else state = 'W'
            end
            else state = 'Q'
        end
        else if (verify(c, delims) == 0 & last_state \== 'Q') then do
            state = 'D'
        end
        else do
            if (last_state == 'Q') then state = 'Q'
            else state = 'W'
        end
        /* process state transitions */
        if ((last_state == 'Q' | state == 'Q') & state \== last_state) then c = ''
        else if (state == 'D' & \leave_ws) then c = ''
        if (last_state == 'D' & state \== 'D' & seen_QW) then do
            /* flush the token */
            num = num + 1
            call value stem'.'num, token
            token = ''
        end
        token = token||c
        last_state = state
        seen_QW = (seen_QW | state \== 'D')
    end

    /* flush the last token if any */
    if (token \== '' | seen_QW) then do
        num = num + 1
        call value stem'.'num, token
    end

    call value stem'.0', num

    return
