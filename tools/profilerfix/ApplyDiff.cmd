/* $Id: ApplyDiff.cmd,v 1.1 2002-04-19 07:05:53 bird Exp $
 *
 * Applies any .diff file to a binary file.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*
 * Arguments.
 */
parse arg sDiffFile sFilename sDummy
if ((sFilename = '') | (sDummy <> '')) then
do
    say 'syntax error';
    exit(1);
end



/*
 * Read the diff file.
 */
aDiff.0 = 0;
do while (lines(sDiffFile) > 0)
    sLine = translate(linein(sDiffFile),,
                      '                                      ',,
                      x2c('000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f'))
    if (word(sLine, 1) <> 'mismatch') then
        leave;
    parse var sLine 'mismatch at' sOffset '-' sTo'('.') !=' sFrom'('.
    if ((sTo = '') | (sFrom = '') | (sOffset = '')) then
    do
        /*say 'dbg1:' sTo
        say 'dbg1:' sFrom
        say 'dbg1:' sOffset
        say     sLine
        */
        leave;
    end

    /* add it */
    i = aDiff.0 + 1;
    aDiff.0 = i;
    aDiff.i.iOffset = HexToDec(sOffset);
    aDiff.i.chTo = x2c(strip(sTo))
    aDiff.i.chFrom = x2c(strip(sFrom))
end

say 'Read 'aDiff.0' differences from' sDiffFile'.'
if (aDiff.0 <= 0) then
do
    say ' No differences in the difffile, ' sDiffFile'!';
    exit(3);
end


/*
 * Apply the diffeneces.
 */
iRet = 0;
do i = 1 to aDiff.0
    /* say 'diff 'i':' aDiff.i.iOffset '-' c2x(aDiff.i.chFrom)' -> 'c2x(aDiff.i.chTo); */
    ch = charin(sFilename, aDiff.i.iOffset + 1, 1);
    if (ch <> aDiff.i.chFrom) then
    do
        if (ch <> aDiff.i.chTo) then
        do
            say 'patch mismatch at offset' aDiff.i.iOffset;
            iRet = iRet + 1;
            leave;
        end
    end
    else
    do
        irc = charout(sFilename, aDiff.i.chTo, aDiff.i.iOffset + 1);
        if (irc <> 0) then
        do
            say 'Error: failed to apply fix. rc='irc;
            iRet = iRet + 1;
        end
    end
end


/*
 * Print indicator message.
 */
if (iRet > 0) then
    say iRet 'errors occured during applying of the patch.'
else
    say 'Successfully patched '''sFilename'''.';

exit(iRet);




HexToDec: procedure
    parse arg sStringHex
return x2d(strip(strip(sStringHex), 'L', '0'));
