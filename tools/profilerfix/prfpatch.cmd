/* $Id: prfpatch.cmd,v 1.1 2002-04-11 21:04:07 bird Exp $
 *
 * Applies fix the VAC308 profiler .obj.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*
 * Arguments.
 */
parse arg sFilename sDummy
if ((sFilename = '') | (sDummy <> '')) then
do
    say 'syntax error';
    exit(1);
end

/*
 * Apply the patch.
 */
say 'Changing DosLoadModule to prfLoadModule...'
sDosLoadModule = charin(sFilename, 229, 13);
if (sDosLoadModule = 'DosLoadModule') then
do
    irc = charout(sFilename, 'prfLoadModule', 229);
    if (irc <> 0) then
        say 'Error: failed to apply fix. rc='irc;
    else
        say 'Fix applied.'
    exit(irc);
end
else if (sDosLoadModule = 'prfLoadModule') then
do
    say 'Fix is allready applied.'
    exit(0);
end

/*
 * Error
 */
say 'invalid file?'
say 'sDosLoadModule='c2x(sDosLoadModule);
exit(1);

