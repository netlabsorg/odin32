/* $Id: makedesc.cmd,v 1.1 2000-11-23 15:12:05 bird Exp $
 *
 * Syntax: MakeDesc.cmd <.def> "<Vendor>" "<Version>" "<FileVer>" "<description>"
 * Adds a Description string to the given .deffile.
 * Fills in build machine and time/date.
 */
parse arg  sDefFile '"'sVendor'"' '"'sVersion'"' '"'sDescription'"'


sHostname = VALUE('HOSTNAME',,'OS2ENVIRONMENT');
if (sHostname = 'SD66290O') then
    sHostname = 'UnivacJob';

sOutput = "@"sVendor":"sVersion"#@##1## "Date()" "Time();
sOutput = left(sOutput, 42) || sHostname": :::"||,
          sVersion"::@@"||sDescription;
/* say sOutput; */
"@echo DESCRIPTION '"||sOutput||"' >>" sDefFile
exit(0);
