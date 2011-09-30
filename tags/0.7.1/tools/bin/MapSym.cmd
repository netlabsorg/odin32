/* $Id: MapSym.cmd,v 1.8 2002-08-29 10:04:10 bird Exp $
 *
 * Helper script for calling MAPSYM.EXE.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 * Configuration.
 */
sWatcom = ';wat11c;wat11;watcom;wat11c-16;wat11-16;wlink;wlink.exe;'
sIBMOld = ';vac3xx;vac365;vac308;emx;emxpgcc;mscv6;mscv6-16;ilink;ilink.exe;link386;link386.exe;link;link.exe;ibmold;'
sVAC40  = ';vac40;'
sLinkers = strip(sVAC40, 'T', ';')||strip(sIBMOld, 'T', ';')||strip(sWatcom, 'T', ';')||';'
/* look for 4os2 */
f4OS2   = 0;
Address CMD 'set 4os2test_env=%@eval[2 + 2]';
if (value('4os2test_env',, 'OS2ENVIRONMENT') = '4') then
    f4OS2 = 1;
sCopy = '@copy'
if (f4OS2) then
    sCopy = '@copy /Q'

/*
 * Parse arguments.
 */
parse arg sLinker sMapfile sSymFile sDummy

if (  (sDummy <> '') ,
    | (sMapFile = '') ,
    | (pos(';'translate(sLinker)';', translate(sLinkers)) <= 0) ,
    )
then
do
    say 'syntax error';
    call syntax;
    exit(16);
end
if (stream(sMapFile, 'c', 'query exist') = '') then
do
    say 'error: the mapfile '''sMapFile''' doesn''t exist.';
    call syntax;
    exit(16);
end

sMapBaseName = filespec('name', sMapFile);
if (pos('.', sMapBaseName) > 0) then
    sMapBaseName = left(sMapBaseName, pos('.', sMapBaseName) - 1);

if (sSymFile = '') then
do
    if (lastpos('.', filespec('name', sMapFile)) > 0) then
        sSymFile = left(sMapFile, lastpos('.', sMapFile)) || 'sym';
    else
        sSymFile = sMapFile || '.sym';
end


/*
 * Convert linker input.
 */
sLinker = translate(sLinker);           /* easier to compare */
if (pos(';'||sLinker||';', translate(sWatcom)) > 0) then
    sLinker = 'WATCOM';
if (pos(';'||sLinker||';', translate(sIBMOld)) > 0) then
    sLinker = 'IBMOLD';
if (pos(';'||sLinker||';', translate(sVAC40)) > 0) then
    sLinker = 'VAC40';

/*
 * Load rexxutil functions.
 */
call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
call SysLoadFuncs


/*
 * Get the temp directory.
 */
sTmp = value('TMP',, 'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = value('TEMP',, 'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = value('TMPDIR',, 'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = value('TEMPDIR',, 'OS2ENVIRONMENT');
if (sTmp = '') then
do
    say 'Fatal error: Hey mister! don''t you have a tmp directory?'
    exit(16);
end
sTmp = strip(strip(sTmp, 'T', '\'), 'T','/');


/*
 * Make temporary filename.
 * (We'll make a workcopy, modified for some linkers, of the mapfile
 *  to this file. And call mapsym on it.)
 */
sTmpMapFile = SysTempFileName(sTmp'\'||sMapBaseName||'.???');
if (sTmpMapFile = '') then
do
    say 'error: failed to make temporary file!';
    exit(16);
end


/*
 * Do pre mapsym.exe processing of the sym file.
 */
select
    when (sLinker = 'IBMOLD') then
    do
        sCopy sMapFile sTmpMapFile
        if (rc <> 0) then
        do
            say 'error: failed to copy '''sMapFile''' to '''sTmpMapFile'''. (rc='rc')';
            exit(16);
        end
    end

    when (sLinker = 'WATCOM') then
    do
        /*sTmpMapFile = 'watos2.map'*/
        rc = wat2map(sMapFile, sTmpMapFile);
        if (rc <> 0) then
        do
            say 'error: wat2map failed. (rc='rc')';
            exit(16);
        end
    end

    when (sLinker = 'VAC40') then
    do
        rc = vac40conv(sMapFile, sTmpMapFile);
        if (rc <> 0) then
        do
            say 'error: vac40conv failed. (rc='rc')';
            exit(16);
        end
    end

    otherwise
    say 'bad linker, sLinker='sLinker;
    exit(16);
end


/*
 * Call mapsym.exe
 */
sOldDir = directory();
call directory(sTmp);
'@mapsym.exe' sTmpMapFile
if (rc <> 0) then
do
    say 'error: mapsym.exe '''sTmpMapFile''' failed with rc='rc'.';
    call directory(sOldDir);
    exit(rc);
end
call directory(sOldDir);



/*
 * Copy the symfile to the target path.
 */
sCopy left(sTmpMapFile, length(sTmpMapFile) - 4)||'.sym' sSymFile;
if (rc <> 0) then
do
    say 'error: failed to copy '''left(sTmpMapFile, length(sTmpMapFile) - 4)||'.sym'' to '''sSymFile'''. (rc='rc')';
    exit(16);
end

/*
 * Delete temporary files.
 */
call SysFileDelete sTmpMapFile;
call SysFileDelete left(sTmpMapFile, length(sTmpMapFile) - 4)||'.sym';

exit(0);



/*******************************************************************************
*   Function Area                                                              *
*******************************************************************************/


/**
 * Display usage info.
 */
syntax: procedure;
    say 'syntax: MapSym.cmd <linker> <mapfile> [symfile]';
    say '   linker      watcom, vac3xx, link386 or vac40. All $(BUILD_ENV).';
    say '   mapfile     Name of the input map file.';
    say '   symfile     Name of the output sym file. If not specified';
    say '               mapfile is used as a base name.';
return 0;


/**
 * Converts a vac40 mapfile to a file readable for mapsym.exe.
 * @returns 0 on success.
 *          !0 on error.
 * @param   sInFile     Input vac40 mapfile.
 * @param   sOutFile    Output filename.
 * @status  completely implemented.
 * @author  knut st. osmundsen (bird@anduin.net)
 * @remark  Make some assume about the module name. Not sure if it matters.
 */
vac40conv: procedure;
    parse arg sInFile, sOutFile;

    /*
     * variables.
     */
    rc = 0;
    sState = 'Init';                    /* shows the current content status. */

    /*
     * Read file line by line.
     */
    do while ((rc = 0) & (lines(sInFile) > 0))
        /*
         * Read the line and look for state change.
         */
        sLine = linein(sInFile);
        sNewState = vac40ProbeState(sState, sLine);

        /*
         * State switch.
         */
        select
            /*
             * First line.
             */
            when (sState = 'Init') then
            do
                if (pos('Link Map...', sLine) <= 0) then
                do
                    say 'error: Not VAC40 map file.'
                    rc = 1;
                end

                call lineout sOutFile, '';
                sModName = translate(filespec('name',sInFile));
                if (lastpos('.', sModName) > 0) then sModName = left(sModName, lastpos('.', sModName) - 1);
                call lineout sOutFile, ' '||sModName;
                call lineout sOutFile, '';
                sNewState = 'AnyState';
            end

            /*
             * Segments - let's try copy then raw.
             */
            when ((sNewState = 'Segments') | (sState = 'Segments')) then
            do
                if (sState <> 'Segments') then
                do
                    sSegment = '';
                    cbSegment = 0;
                    call lineout sOutFile, ' Start         Length     Name                   Class'
                end
                else
                do
                    if (pos('at offset', sLine) > 0) then
                    do  /*
                         * At offset line:
	                     *          at offset 000006A8 bytes 00006H for "@197"
                         */
                         parse var sLine . 'offset' sOffset 'bytes' sLength'H' . .
                         /*say 'off='sOffset 'len='sLength 'line:' left(sLine, 40);*/
                         cbSegment = HexToDec(sOffset) + HexToDec(sLength);
                    end
                    else if ( (sNewState = 'Segments') & (sSegment = ''),
                             & (strip(sLine) <> '') & (pos(':', sLine) > 0) ) then
                    do  /*
                         * New segment.
                         */
                        /*say 'NewSeg:' sLine*/
                        sSegment = sLine;
                    end
                    else if ((strip(sLine) = '') & (sSegment <> '')) then
                    do  /*
                         * Time to write segment line:
                         *    0001:00000000		DATA32		DATA
                         */
                        sSegment = translate(sSegment, ' ', '09'x);
                        parse var sSegment iSeg':'iSegOffset sSegName sSegClass
                        /*say 'SegLn: seg='iSeg 'off='iSegOffset 'name='sSegName 'class='sSegClass; say sSegment*/
                        if (length(sSegName) < 22) then sSegName = left(sSegName, 22, ' ');
                        call lineout sOutFile, ' '||strip(iSeg)':'iSegOffset right(DecToHex(cbSegment), 9, '0'),
                                     ||'H     '||sSegName||' '||strip(sSegClass);
                        sSegment = '';
                        cbSegment = 0;
                    end
                end
            end

            /*
             * Publics by value - filter out some of the stupid stuff.
             */
            when ((sNewState = 'PublicValue') | (sState = 'PublicValue')) then
            do
                if (sState = sNewState) then
                do
                    sLine = translate(sLine, '', '"'||d2c(9));
                    if ((strip(sLine) <> '') & (pos('|', sLine) <= 0) & (pos('@', sLine) <= 0)) then
                        call lineout sOutFile, ' '||word(sLine, 1)||'       '||word(sLine, 2);
                end
                else if (sNewState = 'PublicValue') then
                do  /* first call */
                    call lineout sOutFile, '';
                    call lineout sOutFile, '  Address         Publics by Value';
                end
            end

            /*
             * Entry pointe - raw copy.
             */
            when (sNewState = 'EntryPoint') then
            do
                call lineout sOutFile, '';
                call lineout sOutFile, sLine;
                sNewState = 'Stop';
            end

            /*
             * Got to next state.
             */
            when (sState = 'AnyState') then
            do
                if (strip(sLine) = '') then
                    iterate;
                if (sNewState = sState) then
                do
                    say 'state error, bad input?';
                    rc = 2;
                end
            end

            /*
             * Skips till we hit a new state.
             */
            when (sState = 'Skip') then
                nop;
            /* debug */
            when ((sState = 'ImportsName') | (sState = 'PublicNames') | (sState = 'ImportLibs') | (sState = 'ConstDest')) then
                nop;

            /*
             * Final.
             */
            when (sState = 'Stop') then
                leave;

            /*
             * Should not happen.
             */
            otherwise
            do
                say 'bad state, sState='sState;
                rc = 3;
            end
        end /* select */

        /*
         * next state
         */
        /*say 'state debug:' sState '->' sNewState 'ln:' left(sLine, 40)*/
        sState = sNewState;
    end /* do while */

    /*
     * cleanup.
     */
    call stream sOutFile, 'c', 'close'
    call stream sMapfile, 'c', 'close'

return rc;


/**
 * Checks if this line is recognizes as a state change line.
 * @returns New state.
 * @param   sState      The current state.
 * @param   sLine       The input line we're processing.
 */
vac40ProbeState: procedure
    parse arg sState, sLine
    select
        when (pos('...Options...', sLine) > 0)              then sState = 'Skip';
        when (pos('...Segments...', sLine) > 0)             then sState = 'Segments';
        when (pos('...Constructor/Destructor pairs to run...', sLine) > 0) then sState = 'ConstDest';
        when (pos('Imports by name', sLine) > 0)            then sState = 'ImportsName';
        when (pos('Publics by name', sLine) > 0)            then sState = 'PublicNames';
        when (pos('Publics by value', sLine) > 0)           then sState = 'PublicValue';
        when (pos('...Import library list...', sLine) > 0)  then sState = 'ImportLibs';
        when (pos('Line numbers for', sLine) > 0)           then sState = 'Skip';
        when (pos('Program entry point', sLine) > 0)        then sState = 'EntryPoint';
        otherwise
            nop;
    end
return sState;



/**
 * Converts a hex string (no prefix/postfix) to a decimal value.
 * @returns decimal value.
 * @param   sStringHex  Hexstring to convert. (no 0x prefix or H postfix!)
 * @status  completely implemented.
 * @author  knut st. osmundsen (bird@anduin.net)
 */
HexToDec: procedure
    parse arg sStringHex
/*    say 'HexToDec('sStringHex'):' strip(strip(sStringHex), 'L', '0')*/
    sStringHex = strip(strip(sStringHex), 'L', '0');
    if (sStringHex = '') then
        sStringHex = 0;
return x2d(sStringHex);


/**
 * Converts a dec string to a hex string.
 * @returns hex string value.
 * @param   sStringDec    Decstring to convert.
 * @status  completely implemented.
 * @author  knut st. osmundsen (bird@anduin.net)
 */
DecToHex: procedure
    parse arg sStringDec
    sStringDec = strip(strip(sStringDec), 'L', '0');
    if (sStringDec = '') then
        sStringDec = 0;
return d2x(sStringDec);


/* SCCSID = %W% %E% */
/****************************************************************************
 *                                                                          *
 * Copyright (c) IBM Corporation 1994 - 1997.                               *
 *                                                                          *
 * The following IBM OS/2 source code is provided to you solely for the     *
 * the purpose of assisting you in your development of OS/2 device drivers. *
 * You may use this code in accordance with the IBM License Agreement       *
 * provided in the IBM Device Driver Source Kit for OS/2.                   *
 *                                                                          *
 ****************************************************************************/
/**@internal %W%
 * WAT2MAP - translate symbol map from Watcom format to MS format.
 * @version %I%
 * @context
 *  Unless otherwise noted, all interfaces are Ring-0, 16-bit, kernel stack.
 * @notes
 *  Usage:  WAT2MAP <watcom_mapfile >ms_mapfile
 *               - or -
 *          type watcom_mapfile | WAT2MAP >ms_mapfile
 *
 *          Reads from stdin, writes to stdout.  Will accept the Watcom map filename
 *          as an argument (in place of reading from stdin).  Eg.,
 *
 *          WAT2MAP watcom_mapfile >ms_mapfile
 *
 *  Notes:
 *    1.)  The symbol handling in the debug kernel won't work for some of the
 *         characters used in the C++ name space.  WAT2MAP handles these symbols
 *         as follows.
 *             Scoping operator symbol '::' is translated to '__'.
 *             Destructor symbol '~' is translated to 'd'.
 *             Symbols for operators '::operator' are not provided.
 *
 *         Eg., for user defined class 'A', the symbol for constructor A::A is
 *         translated to A__A, and the destructor symbol A::~A becomes A__dA, and
 *         the assignment operator, 'A::operator =', is not translated.
 *
 *    2.)  Bug - C++ provides for defining multiple functions with same fn name
 *         but different function signatures (different parameter lists).  This
 *         utility just translates all the address / symbol combinations it finds,
 *         so you can end up with several addresses for a given fn name.
 * @history
*/
/* <End of helpText> - don't modify this string - used to flag end of help. */
/****************************************************************************/

wat2map: procedure
Parse Arg arg1, arg2
If (Length( arg1 ) = 0) | (Verify( arg1, '/-?' ) = 0) Then Do;
   Do i = 1 to 1000
      helpText = Sourceline(i)
      If Pos( '<End of helpText>', helpText ) <> 0 Then Leave;       /* quit loop */
      Say helpText
   End;
   Return 1
End;
If Length( arg2 ) = 0 Then Do;
   Say " Way to go Beaver... How about an out-put file name ?"
   Return 2
End;
mapFile = arg1          /* Can be Null, in which case we pull from stdin. */
outFile = arg2
fFlatMode = 0;

/* erase outfile */  /* kill the old map file */
rc=SysFileDelete(outfile)


/*--- 1.  Find & translate module name.  ---*/
Do While Lines( mapFile ) <> 0
   watcomText = LineIn( mapFile )
   /*Parse Value watcomText With "Executable Image: " fileName "." fileExt*/
   Parse Value watcomText With "Executable Image: " sFilename
   If (sFilename <> '') Then Do;
      sFilename = filespec('name', sFilename);
      Parse Var sFilename fileName "." fileExt
      If fileName <> "" Then Do;   /* Found match */
         call lineout outfile ,' '
         call lineout outfile ,' ' || fileName
         call lineout outfile ,' '
         Leave;                    /* Break from loop. */
      End;
   End;
End
If Lines( mapFile ) = 0 Then Do;        /* If end of file ... */
   Say "Error:  Expected to find line with text 'Executable Image:' "
   Return 3
End

/*--- 2.  Skip the group definitions - Rob's notes say we don't need them. -*/

/*--- 3.  Skip to the start of the segment table.  ---*/
Do While Lines( mapFile ) <> 0
   watcomText = LineIn( mapFile )
   Parse Value watcomText With "Segment" header_2_3 "Address" header_5
   If Strip( header_5 ) = "Size" Then Leave;      /* Found header line for Segment table. */
End
If Lines( mapFile ) = 0 Then Do;        /* If end of file ... */
   Say "Error:  Expected to find line with text 'Segments ... Size' "
   Return 4
End


junk = LineIn( mapFile )      /* Discard a couple lines of formatting. */
junk = LineIn( mapFile )      /* Discard a couple lines of formatting. */
/*--- 4.  Translate segment table.  ---*/
/*"Segment                Class          Group          Address         Size"*/
iClass = pos('Class', watcomText);
iGroup = pos('Group', watcomText);
iAddress = pos('Address', watcomText);
iSize = pos('Size', watcomText);

call lineout outfile , " Start         Length     Name                   Class"  /* bird bird bird fixed!!! */
Do While Lines( mapFile ) <> 0
   watcomText = LineIn( mapFile )
   /* do it the hard way to make sure we support spaces segment names. */
   segName = strip(substr(watcomText, 1, iClass-1));
   If segName = "" Then Leave;          /* Empty line, break from loop. */
   className = strip(substr(watcomText, iClass, iGroup-iClass));
   groupName = strip(substr(watcomText, iGroup, iAddress-iGroup));
   address = strip(substr(watcomText, iAddress, iSize-iAddress));
   size = strip(substr(watcomText, iSize));
   if (pos(':', address) <= 0) then     /* NT binaries doesn't have a segment number. */
   do
      fFlatMode = 1;
      address = '0001:'||address;
   end
   length = right(strip(strip(size), 'L', '0'), 9, '0') || 'H     '
   segName = Left( segName, 23 )
   call lineout outfile ,' ' || address || ' ' || length || segName || className
End
call lineout outfile ,' '     /* Extra line feed. */


/*--- 5.  For all remaining lines in the input file:  if the line starts
   with a 16:16 address, assume it's a symbol declaration and translate
   it into MS format.  ---*/

call lineout outfile ,'  Address         Publics by Value'
/* call lineout outfile ,' '*/

Do While Lines( mapFile ) <> 0
   watcomText = LineIn( mapFile )
   Parse Value watcomText With seg ':' ofs 14 . 16 declaration
   if (fFlatMode) then
   do
      seg = '0001';
      Parse Value watcomText With ofs 9 . 16 declaration
   end
   else
   do  /* kso: more workarounds */
       if (is_Hex(seg) & length(ofs) > 4 & \is_Hex(substr(ofs,5,1))) then
           ofs = '0000'||left(ofs,4);
   end
   /*say ofs  '-'declaration*/
   is_Adress = (is_Hex(seg) = 1) & (is_Hex(ofs) = 1)
   If ((is_Adress = 1) & (seg <> '0000')) Then Do;          /* bird: skip symbols with segment 0. (__DOSseg__) */
      /*--- Haven't done the work to xlate operator symbols - skip the line. */
      If Pos( '::operator', declaration ) <> 0 Then Iterate;

      /*--- Strip any arguement list if this is a function prototype.  */
      declaration = StripMatchedParen( declaration )

      /*--- Strip array brackets if this is an array. */
      sqBracket = Pos( '[', declaration )
      If sqBracket <> 0
         Then declaration = Substr(declaration, 1, sqBracket-1);

      /*--- Strip leading tokens from the function name.
         Eg., remove function return type, near/far, etc.  */
      declaration = Word( declaration, Words(declaration) )

      /*--- Strip any remaining parens around function name. ---*/
      declaration = ReplaceSubstr( '(', ' ', declaration )
      declaration = ReplaceSubstr( ')', ' ', declaration )

      /*--- Debug kernel doesn't like symbol for scoping operator "::"
         in symbol names.  Replace :: with double underscore "__". ---*/
      declaration = ReplaceSubstr( '::', '__', declaration )

      /*--- Debug kernel doesn't like symbol for destructor "~"
         in symbol names.  Replace ~ with character "d" for "destructor.
         Note destructor for a class will translate "A::~A" -> "A__dA". ---*/
      declaration = ReplaceSubstr( '~', 'd', declaration )

      call lineout outfile ,' ' || seg || ':' || ofs || '       ' || declaration
   End;

   /* check for entry point, if found we add it and quit. */
   if (pos('Entry point address', watcomText) > 0) then
   do
       parse var watcomText 'Entry point address:' sEntryPoint
       if (pos(':', sEntryPoint) <= 0) then
           sEntryPoint = '0001:'||strip(sEntryPoint);
       call lineout outfile, ''
       call lineout outfile, 'Program entry point at' strip(sEntryPoint)
       leave;
   end
End; /* End While through symbol section, end of input file. */

call stream outfile, 'c', 'close';
call stream mapfile, 'c', 'close';

Return 0;  /* End of program.  */

/*--- Helper subroutines. ---*/

StripMatchedParen:
/* Strips matched "( )" from end of string.  Returns
   a substring with the trailing, matched parens deleted. */

   Parse Arg string

   ixOpenParen = LastPos( "(", string );
   ixCloseParen = LastPos( ")", Substr( string, 1, Length(string)-1 ));

   If (ixOpenParen = 0)                     /* No match. */
      Then Return string
   Else If ixCloseParen < ixOpenParen       /* Found match, no imbedded "()". */
      Then Return Substr( string, 1, ixOpenParen-1 )
   Else Do;                                 /* Imbedded (), must skip over them. */
      /* Parse Value string With first ixCloseParen+1 rest */
      first = Substr( string, 1, ixCloseParen)
      rest = Substr( string, ixCloseParen+1 )
      string = StripMatchedParen( first ) || rest
      Return StripMatchedParen( string )
   End;
End;

ReplaceSubstr:
/* Replaces oldPat (old pattern) with newPat (new pattern) in string. */

   Parse Arg oldPat , newPat , string

   ix = Pos( oldPat, string )
   if ix <> 0 Then Do;
      first = Substr( string, 1, ix-1 )
      rest  = Substr( string, ix + Length( oldPat ) )
      string = first || newPat || rest
   End;
   Return string
End;

is_Hex:
/* Returns 1 if String is valid hex number, 0 otherwise. */
   Parse Arg string
   Return (Length(string) > 0) &  (Verify( string, '0123456789abcdefABCDEF' ) = 0)
End;

