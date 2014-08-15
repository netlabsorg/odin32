/* $Id: lxlite.cmd,v 1.1 2002-08-24 22:33:46 bird Exp $
 *
 * LXLite wrapper to fix EA problem on LAN Server volumes with JFS.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 */

/* get input&output filename */
parse arg sFilename

/* get lxlite directory */
parse source . . sSource
sDir = filespec('drive', sSource) || filespec('path', sSource);

/* stripp of EAs to avoid errors */
'eautil' sFilename 'nul /s'

/* call lxlite */
sDir||'lxlite.exe' sFilename
return rc;

