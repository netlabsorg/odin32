/* 
 */
'@echo off'

/*
 * Parse argument(s).
 */
parse arg inFileSpec

dllName = substr(inFileSpec,1,pos('.', inFileSpec)-1);
outFileDef = dllName".def"

rc = stream(inFileSpec, 'c', 'open read');

if (pos('READY', rc) <> 1) then
do
    say 'Failed to open include file' inFileSpec'.';
    exit(-20);
end

rc = stream(outFileDef, 'c', 'open write');

if (pos('READY', rc) <> 1) then
do
    say 'Failed to open include file' outFileDef'.';
    exit(-20);
end

ordinal = 0

Call LINEOUT outFileDef, "LIBRARY "TRANSLATE(dllName)" INITINSTANCE"
Call LINEOUT outFileDef, "DESCRIPTION 'Odin32 System DLL - "dllName"'"
Call LINEOUT outFileDef, "DATA MULTIPLE NONSHARED"
Call LINEOUT outFileDef, ""
Call LINEOUT outFileDef, "EXPORTS"
Call LINEOUT outFileDef, ""

do while (lines(inFileSpec) > 0)
    varSize = 0;   
    comment = 0;
    sLine = strip(linein(inFileSpec));
    if (sLine = '') then
        iterate;
    if (substr(sLine, 1, 1) <> '@') then
        iterate;
    ordinal = ordinal + 1;
    sLine1 = sLine;
    sLine = substr(sLine, 3);
    if (word(sLine, 1) = 'stdcall') then
        sLine = substr(sLine, 8);
    if (word(sLine, 1) = 'stub') then
    do
        sLine = substr(sLine, 5);
        comment = 1
    end
    sLine = strip(sLine);
    if (word(sLine, 1) = '#') then
        sLine = substr(sLine, 2);
    sLine = strip(sLine);
    if (word(sLine, 1) = '-private') then
        sLine = substr(sLine, 9);
    openSkobkaPos = pos('(',sLine);
    if (openSkobkaPos > 0) then
    do
        funcName = strip(substr(sLine,1,openSkobkaPos-1));
	ordinalFuncName = funcName;
        closeSkobkaPos = pos(')',sLine);
        sLine = substr(sLine,openSkobkaPos+1, closeSkobkaPos-openSkobkaPos-1);
        /* now sLine contains parameters */
	say '"'sLine'"' words(sLine)
        varSize = words(sLine)*4;
    end
    else
    do
        funcName = strip(sLine);
    end
 
    /* check if ordinal name is differ from real func name */
    funcNamePos = pos(funcName, word(sLine1, words(sLine1)));
    if (funcNamePos > 0) then
    do
      /* found different func name. get last word as funcname */
      funcName = word(sLine1, words(sLine1));
      /* strip brackets */
      openSkobkaPos1 = pos('(',funcName);
      if (openSkobkaPos1 > 0) then
      do
        funcName = strip(substr(funcName,1,openSkobkaPos1-1));
      end
    end

    if (comment == 1) then
        outStr = ";"ordinalFuncName" = _"funcName;
    else
        outStr = ordinalFuncName" = _"funcName;
    if (varSize > 0 | openSkobkaPos > 0) then
        outStr = outStr"@"varSize;
    outStr = outStr" @"ordinal;
    Call LINEOUT outFileDef, outStr
    /* say outStr */
end

call stream inFileSpec, 'c', 'close';
call stream outFileDef, 'c', 'close';
