/* $Id: mapsymWat.cmd,v 1.1 2000-11-21 04:34:58 bird Exp $
 *
 * Script which makes .sym file from Watcom Map file.
 *  Requires: wat2map - which didn't work.
 *            Currently it only make a dummy .sym file.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*
 * Parse input and make sSym filename.
 */
parse arg sMap sDummy

iExtPos = lastpos('.', sMap);
if (sMap = '' | substr(sMap, 1, 1) = '-' | iExtPos <= 0 | sDummy <> '') then
do
    say 'syntax error';
    say 'mapsymWat.cmd <mapfile>';
    exit(1);
end
sSym = substr(sMap, 1, iExtPos)||'sym';

/*
 * Determin temporary directory.
 */
sTmp = VALUE('TMP',,'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = VALUE('TEMP',,'OS2ENVIRONMENT');
if (sTmp = '') then
    sTmp = '.';

/*
 * Make temporary filename.
 */
call RxFuncAdd 'SysTempFileName', 'RexxUtil', 'SysTempFileName'
call RxFuncAdd 'SysFileDelete', 'RexxUtil', 'SysFileDelete'
/* start: remove this code when the rest of the scrip is working again. */
call SysFileDelete sSym;
call stream sSym, 'c', 'open write';
call stream sSym, 'c', 'close';
exit(0);
/* end */
sTempFile = SysTempFileName(sTmp'\mapsymw.???');
if (sTempFile = '') then
do
    say 'error: failed to make temporary file!';
    exit(2);
end
sTempSymFile = substr(sTempFile, 1, length(sTempFile)-3)||'sym';

/*
 * Convert the Watcom mapfile to a mapfile mapsym likes.
 */
'wat2map' sMap sTempFile
if (rc <> 0) then do
    say 'error: Wat2Map failed with rc='rc'.';
    exit(rc);
end

/*
 * Run mapsym.
 */
sOldDir = directory();
call directory(sTmp);
'mapsym' sTempFile
retrc = rc;
call directory(sOldDir);
if (retrc <> 0) then
do
    say 'error: mapsym failed with rc='rc'.';
    call SysFileDelete sTempFile
    call SysFileDelete sTempSymFile
    exit(retrc);
end

/*
 * Copy result sym file to target sym file.
 */
'copy' sTempSymFile sSym
retrc = rc;
if (rc <> 0) then
    say 'error: copy failed with rc='rc'.';

/* Cleanup and exit */
call SysFileDelete sTempFile
call SysFileDelete sTempSymFile
exit(retrc);

