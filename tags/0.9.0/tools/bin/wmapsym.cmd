/* REXX */
/*
 * Creates a .SYM file from the Watcom .MAP file using IBM MAPSYM.
 *
 * Copyright (C) 2010-2011 Dmitriy Kuminov
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

signal on syntax
signal on halt
signal on novalue
trace off
numeric digits 12
'@echo off'


/*------------------------------------------------------------------------------
 globals
------------------------------------------------------------------------------*/

/* all globals to be exposed in procedures */
Globals = 'G. Opt. Static.'


/*------------------------------------------------------------------------------
 startup + main + termination
------------------------------------------------------------------------------*/

/* init system REXX library */
if (RxFuncQuery('SysLoadFuncs')) then do
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs
end

/* detect script file and directory */
parse source . . G.ScriptPath
G.ScriptDir = FixDir(filespec('D', G.ScriptPath) || filespec('P', G.ScriptPath))
G.ScriptFile = FixDir(filespec('N', G.ScriptPath))
G.StartDir = directory()

parse arg aArgs
if (aArgs == '') then do
    say
    say 'Creates a .SYM file from the Watcom .MAP file using IBM MAPSYM'
    say 'Version 1.1 (2011-12-18)'
    say 'Copyright (C) 2011 Dmitriy Kuminov'
    say
    say 'Usage: '
    say
    say '  'G.ScriptFile' <watcom_map_file>'

    call Done 0
end

G.WatMapFile = aArgs

call Main

call Done 0


/*------------------------------------------------------------------------------
 functions
------------------------------------------------------------------------------*/

/**
 * Just do the job.
 */
Main: procedure expose (Globals)

    GroupHeader.0 = 5
    GroupHeader.1 = '+------------+'
    GroupHeader.2 = '|   Groups   |'
    GroupHeader.3 = '+------------+'
    GroupHeader.4 = 'Group                           Address              Size'
    GroupHeader.5 = '=====                           =======              ===='

    SegmentHeader.0 = 5
    SegmentHeader.1 = '+--------------+'
    SegmentHeader.2 = '|   Segments   |'
    SegmentHeader.3 = '+--------------+'
    SegmentHeader.4 = 'Segment                Class          Group          Address         Size'
    SegmentHeader.5 = '=======                =====          =====          =======         ===='

    MemMapHeader.0 = 7
    MemMapHeader.1 = '+----------------+'
    MemMapHeader.2 = '|   Memory Map   |'
    MemMapHeader.3 = '+----------------+'
    MemMapHeader.4 = '* = unreferenced symbol'
    MemMapHeader.5 = '+ = symbol only referenced locally'
    MemMapHeader.6 = 'Address        Symbol'
    MemMapHeader.7 = '=======        ======'

    ImportHeader.0 = 5
    ImportHeader.1 = '+----------------------+'
    ImportHeader.2 = '|   Imported Symbols   |'
    ImportHeader.3 = '+----------------------+'
    ImportHeader.4 = 'Symbol                              Module'
    ImportHeader.5 = '======                              ======'

    state = ''
    state2 = 1
    inMap = 0

    segments.0 = 0
    groups.0 = 0
    image = ''

    G.IbmMapFile = G.WatMapFile
    i = lastpos('.', G.IbmMapFile)
    if (i > 0) then
        G.IbmMapFile = left(G.IbmMapFile, i - 1)
    G.IbmMapFile = SysTempFileName(G.IbmMapFile'.?????-map')

    do while lines(G.WatMapFile)
        line = strip(linein(G.WatMapFile))
        if (line == '') then iterate
        select
            when state == '' then do
                str = 'Executable Image: '
                if (StartsWith(line, str)) then
                    image = filespec('N', substr(line, length(str) + 1))
                    i = pos('.', image)
                    if (i > 0) then do
                        image = left(image, i - 1)
                    end
                else
                if (line == GroupHeader.state2) then do
                    if (state2 == GroupHeader.0) then do
                        state = 'g'
                        state2 = 1
                        iterate
                    end
                    state2 = state2 + 1
                end
                else state2 = 1
            end
            when state == 'g' then do
                if (line == SegmentHeader.state2) then do
                    if (state2 == SegmentHeader.0) then do
                        state = 's'
                        state2 = 1
                        iterate
                    end
                    state2 = state2 + 1
                end
                else do
                    parse value Normalize(line) with name addr size
                    i = groups.0 + 1
                    groups.i.!name = name
                    groups.i.!addr = addr
                    groups.i.!size = size
                    groups.0 = i
                end
            end
            when state == 's' then do
                if (line == MemMapHeader.state2) then do
                    if (state2 == MemMapHeader.0) then do
                        state = 'm'
                        state2 = 1
                        iterate
                    end
                    state2 = state2 + 1
                end
                else do
                    parse value Normalize(line) with name rest
                    i = words(rest)
                    if (i == 2) then parse var rest         addr size
                    else if (i == 3) then parse var rest	group addr size
                    else parse var rest                     class group addr size
                    i = segments.0 + 1
                    segments.i.!name = name
                    segments.i.!class = class
                    segments.i.!group = group
                    segments.i.!addr = addr
                    segments.i.!size = size
                    segments.0 = i
                end
            end
            when state == 'm' then do
                if (line == ImportHeader.state2) then do
                    if (state2 == ImportHeader.0) then do
                        state = 'rest'
                        state2 = 1
                        iterate
                    end
                    state2 = state2 + 1
                end
                else do
                    if (\inMap) then do
                        inMap = 1
                        if (image \== '') then do
                            call lineout G.IbmMapFile, ''
                            call lineout G.IbmMapFile, ' 'image
                        end
                        call lineout G.IbmMapFile, ''
                        call lineout G.IbmMapFile, ' Start         Length     Name                   Class'
                        do i = 1 to segments.0
                            call lineout G.IbmMapFile, ' 'segments.i.!addr' 0'segments.i.!size'H 'segments.i.!name
                        end
                        call lineout G.IbmMapFile, ''
                        call lineout G.IbmMapFile, ' Origin   Group'
                        do i = 1 to groups.0
                            parse var groups.i.!addr seg':'addr
                            addr = strip(addr, 'L', '0')
                            if (addr == '') then addr = '0'
                            call lineout G.IbmMapFile, ' 'seg':'addr'   'groups.i.!name
                        end
                        call lineout G.IbmMapFile, ''
                        call lineout G.IbmMapFile, '  Address         Publics by Value'
                        call lineout G.IbmMapFile, ''
                    end
                    if (StartsWith(line, 'Module: ')) then iterate
                    parse value Normalize(line) with addr name
                    addr = strip(addr, 'T', '*')
                    addr = strip(addr, 'T', '+')
                    if (addr == '0000:00000000') then iterate
                    call lineout G.IbmMapFile, ' 'addr'       'name
                end
            end
            when state == 'rest' then do
                str = 'Entry point address: '
                if (StartsWith(line, str)) then do
                    call lineout G.IbmMapFile, ''
                    call lineout G.IbmMapFile, 'Program entry point at 'substr(line, length(str) + 1)
                end
            end
            otherwise nop
        end

    end
    call lineout G.WatMapFile
    call lineout G.IbmMapFile

    cmdline = 'mapsym.exe' G.IbmMapFile
    cmdline

    if (rc \= 0) then do
        call SayErr 'Executing "'cmdline'" failed with code 'rc'.'
        call SayErr 'Please check the contents of "'G.IbmMapFile'".'
        call Done rc
    end

    call DeleteFile G.IbmMapFile

    return

/*------------------------------------------------------------------------------
 utility functions
------------------------------------------------------------------------------*/

/**
 * Normalizes the given string by removing leading, trailing and extra spaces
 * in the middle so that words in the returned string are separated with exactly
 * one space.
 *
 * @param  aStr         String to normalize.
 * @return              Resulting string.
 */
Normalize: procedure expose (Globals)
    parse arg aStr
    result = ""
    do i = 1 to words(aStr)
        if (result == "") then result = word(aStr, i)
        else result = result" "word(aStr, i)
    end
    return result

/**
 * Returns 1 if the given string @a aStr1 starts with the string @a aStr2 and
 * 0 otherwise. If @a aStr2 is null or empty, 0 is returned.
 *
 * @param  aStr1        String to search in.
 * @param  aStr2        String to search for.
 * @return              1 or 0.
 */
StartsWith: procedure expose (Globals)
    parse arg aStr1, aStr2
    len = length(aStr2)
    if (len == 0) then return 0
    if (length(aStr1) < len) then return 0
    return (left(aStr1, len) == aStr2)

/**
 * Returns 1 if the given string @a aStr1 ends with the string @a aStr2 and
 * 0 otherwise. If @a aStr2 is null or empty, 0 is returned.
 *
 * @param  aStr1        String to search in.
 * @param  aStr2        String to search for.
 * @return              1 or 0.
 */
EndsWith: procedure expose (Globals)
    parse arg aStr1, aStr2
    len = length(aStr2)
    if (len == 0) then return 0
    if (length(aStr1) < len) then return 0
    return (right(aStr1, len) == aStr2)

DeleteFile: procedure expose (Globals)
    parse arg file
    rc = SysFileDelete(file)
    if (rc \= 0 & rc \= 2) then do
        call SayErr 'FATAL: Could not delete file '''file'''!'
        call SayErr 'SysFileDelete returned 'rc
        call Done 1
    end
    return

/**
 * Prefixes all lines of text with the given prefix.
 *
 * @param aLines    Text to prefix.
 * @param aPrefix   Prefix.
 *
 * @return Prefixed text.
 */
PrefixLines: procedure expose (Globals)
    parse arg aLines, aPrefix
    return aPrefix||Replace(aLines, G.EOL, G.EOL||aPrefix)

SaySayEx: procedure expose (Globals)
    parse arg str, prefix, noeol
    noeol = (noeol == 1)
    if (noeol) then call charout, str
    else say str
    return

SaySay: procedure expose (Globals)
    parse arg str, noeol
    noeol = (noeol == 1)
    if (noeol) then call charout, str
    else say str
    return

SayWrn: procedure expose (Globals)
    parse arg str, noeol
    str = 'WARNING: 'str
    call SaySay str, noeol
    return

SayErr: procedure expose (Globals)
    parse arg str, noeol
    str = 'ERROR: 'str
    call SaySay str, noeol
    return

/**
 *  Fixes the directory path by a) converting all slashes to back
 *  slashes and b) ensuring that the trailing slash is present if
 *  the directory is the root directory, and absent otherwise.
 *
 *  @param dir      the directory path
 *  @param noslash
 *      optional argument. If 1, the path returned will not have a
 *      trailing slash anyway. Useful for concatenating it with a
 *      file name.
 */
FixDir: procedure expose (Globals)
    parse arg dir, noslash
    noslash = (noslash = 1)
    dir = translate(dir, '\', '/')
    if (right(dir, 1) == '\' &,
        (noslash | \(length(dir) == 3 & (substr(dir, 2, 1) == ':')))) then
        dir = substr(dir, 1, length(dir) - 1)
    return dir

/**
 *  Shortcut to FixDir(dir, 1)
 */
FixDirNoSlash: procedure expose (Globals)
    parse arg dir
    return FixDir(dir, 1)

/**
 *  NoValue signal handler.
 */
NoValue:
    errl    = sigl
    say
    say
    say 'EXPRESSION HAS NO VALUE at line #'errl'!'
    say
    say 'This is usually a result of a misnamed variable.'
    say 'Please contact the author.'
    call Done 252

/**
 *  Nonsense handler.
 */
Nonsense:
    errl    = sigl
    say
    say
    say 'NONSENSE at line #'errl'!'
    say
    say 'The author decided that this point of code should'
    say 'have never been reached, but it accidentally had.'
    say 'Please contact the author.'
    call Done 253

/**
 *  Syntax signal handler.
 */
Syntax:
    errn    = rc
    errl    = sigl
    say
    say
    say 'UNEXPECTED PROGRAM TERMINATION!'
    say
    say 'REX'right(errn , 4, '0')': 'ErrorText(rc)' at line #'errl
    say
    say 'Possible reasons:'
    say
    say '  1. Some of REXX libraries are not found but required.'
    say '  2. You have changed this script and made a syntax error.'
    say '  3. Author made a mistake.'
    say '  4. Something else...'
    call Done 254

/**
 *  Halt signal handler.
 */
Halt:
    say
    say 'CTRL-BREAK pressed, exiting.'
    call Done 255

/**
 *  Always called at the end. Should not be called directly.
 *  @param the exit code
 */
Done: procedure expose (Globals)
    parse arg code
    /* protect against recursive calls */
    if (value('G.Done_done') == 1) then exit code
    call value 'G.Done_done', 1
    /* cleanup stuff goes there */
    if (symbol('G.AtExit.0') == 'VAR') then do
        /* run all AtExit slots */
        do i = 1 to G.AtExit.0
            if (symbol('G.AtExit.'i) == 'VAR') then
                call RunAtExitSlot i
        end
    end
    drop G.AtExit.
    /* finally, exit */
    if (code \= 0) then do
        call SaySay G.ScriptFile': FAILED with exit code 'code
    end
    exit code

