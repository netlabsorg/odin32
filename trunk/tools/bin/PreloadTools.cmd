/* $Id: PreloadTools.cmd,v 1.2 2001-12-19 01:50:06 bird Exp $
 *
 * Preloads all the tools which we might turn into using.
 * Specify '-u' to unload the tools.
 */
'@echo off'

/*
 * Parse argument(s).
 */
parse arg '-'chUnload

sEmxloadArgs = '-e';
if (translate(substr(chUnload,1,1)) = 'U') then
    sEmxloadArgs = '-u';


/*
 * Find the tools directory.
 */
parse source sOS sCMD sSrc;
sToolsDir = filespec('drive', sSrc) || filespec('path', sSrc); /* with slash */


/*
 * Do the preloading or unloading.
 */
'emxload' sEmxloadArgs 'rm.exe';
'emxload' sEmxloadArgs,
    sToolsDir'impdef.exe',
    sToolsDir'lxlite.exe',
    sToolsDir'fastdep.exe',
    sToolsDir'wrc.exe';

/*
 * Compiler specific stuff.
 */
sCCEnv = translate(value('CCENV',,'OS2ENVIRONMENT'));
if (sCCEnv = '') then sCCenv = 'VAC3';

/* Visual Age */
if ((sCCEnv = 'VAC3') | (sCCEnv = 'VAC36')) then
    'emxload' sEmxloadArgs 'implib.exe ilib.exe nmake.exe alp.exe rc.exe';



