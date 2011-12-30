/* $Id: ExecTestcase.cmd,v 1.2 2002-06-20 02:30:31 bird Exp $
 *
 * Executes a testcase writing result to the logfile.
 *
 * Note. The testcase succeeds if rc=0.
 *
 * The makefile is appended with the -f option to the commandline.
 */

/*
 * Parse arguments.
 */
parse arg sLogfile sMakefile '"'sDescription'"' sCmd
sMakefile = strip(sMakefile);
sLogFile = strip(sLogFile);
sCmd = strip(sCmd);
sDescription = strip(sDescription);
if (sCmd = '' | sDescription = '') then
do  /*usage*/
    say 'syntax error!'
    say 'syntax: ExecTestcase.cmd <logfile> <Makefile> "<Description>" <Command>'
    exit(16);
end


/*
 * Color config.
 */
if (  (value('BUILD_NOCOLOR',,'OS2ENVIRONMENT') = ''),
    & (value('SLKRUNS',,'OS2ENVIRONMENT') = '')) then
do
    sClrMak = '[35;1m'
    sClrErr = '[31;1m'
    sClrRst = '[0m'
end
else
do
    sClrMak = ''
    sClrErr = ''
    sClrRst = ''
end


/*
 * Execute testcase and log the result.
 * Allways return 0.
 */
say sClrMak'  ! Executing testcase 'sMakefile'/'sDescription'...'sClrRst
Address CMD sCmd '-f' sMakefile
rcCmd = rc;
if (rcCmd = '0') then
do
    Address CMD '@echo Ok -' sMakefile'/'sDescription' >> 'sLogfile
    say sClrMak'  ! Ok! ('sMakefile'/'sDescription')'sClrRst
end
else
do
    Address CMD '@echo Failed - 'sMakefile'/'sDescription' (rc='rcCmd') >> 'sLogfile
    say sClrErr'  ! Failed! ('sMakefile'/'sDescription')'sClrRst
end
exit(0);

