/* $Id: odin32env.cmd,v 1.2 2000-05-12 12:21:59 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

    sToolkit = 'c:\appl\os2\toolkt40';
    sVAC     = 'c:\appl\os2\ibmcpp30';
    sDDKBase = 'd:\knut\ddk\base';
    sMSC     = 'd:\knut\ddk\ddktools\toolkits\msc60';
    sCVS     = 'd:\knut\cvs';
    sEMXBack = 'c:\emx';
    sEMXForw = 'c:/emx';
    sMYSQL   = 'c:\emx\mysql'; /* optionally */

    'SET CVSROOT=:pserver:bird@www.netlabs.org:d:/netlabs.src/odin32'
    'SET HOME=d:\knut\home'
    'SET USER=bird'
    'SET DEBUG=1'

    /* oopps! this one is problemous for 4OS/2 due the the '%N's... */
    'SET NLSPATH=C:\MPTN\MSG\NLS\%%N;C:\TCPIP\MSG\ENUS850\%%N;C:\netqos2\%%N;'sToolkit'\OPENDOC\BASE\LOCALE\EN_US\%%N;'sToolkit'\MSG\%%N;'

    /*
     * Non or minore changes are needed here!
     */
    'SET TOOLKIT='sToolkit
    'SET CPPLOCAL='sVAC
    'SET DDKBASE='sDDKBase
    'SET MSC='sMSC

    'SET CPPHELP_INI=C:\OS2\SYSTEM'
    'SET TMP=%TEMP%'
    'SET TMPDIR=%TMP%'

    'SET VACPP_SHARED=FALSE'
    'SET CPPMAIN='sVAC
    'SET CPPWORK='sVAC
    'SET SMINCLUDE='sToolkit'\som\include;'sToolkit'\idl;'sVAC'\INCLUDE\SOM'
    'SET HELPNDX=EPMKWHLP.NDX+CPP.NDX+CPPBRS.NDX'

    'SET INCLUDE='sVAC'\INCLUDE;'sVAC'\INCLUDE\SOM;'sToolkit'\h;%include%;'
    'SET LIB='sVAC'\LIB;%lib%;'
    'SET LIB='sToolkit'\SPEECH\LIB;'sToolkit'\SAMPLES\MM\LIB;'sToolkit'\LIB;'sToolkit'\SOM\LIB;'sToolkit'\OPENDOC\BASE\LIB;%LIB%';
    'SET LOCPATH='sVAC'\LOCALE;%LOCPATH%'
    'SET HELP='sVAC'\HELP;%HELP%;'
    'SET BEGINLIBPATH='sVAC'\DLL;%BeginLIBPATH%;'

    'SET PATH='sVAC'\BIN;'sMSC'\binp;'sCVS'\bin;%Path%'
    'SET PATH='sToolkit'\BETA\BIN;'sToolkit'\SOM\COMMON;'sToolkit'\SOM\BIN;'sToolkit'\OPENDOC\BASE\BIN;'sToolkit'\BIN;%Path%;'sDDKBase'\tools';

    'SET DPATH='sVAC'\HELP;'sVAC';'sVAC'\LOCALE;%DPATH%'
    'SET DPATH='sToolkit'\SOM\COMMON\SYSTEM;'sToolkit'\SOM\MSG;%DPath%';
    'SET DPATH='sToolkit'\MSG;'sToolkit'\OPENDOC\BASE\MSG;'sToolkit'\BOOK;%DPath'

    'SET BOOKSHELF='sToolkit'\archived;'sToolkit'\book;'sToolkit'\books;'sVAC'\help;%BOOKSHELF%';


    /* emx + mysql */
    'set C_INCLUDE_PATH='sEMXForw'/include'
    'set LIBRARY_PATH='sEMXForw'/lib'
    'set CPLUS_INCLUDE_PATH='sEMXForw'/include/cpp;'sEMXForw'/include'
    'set PROTODIR='sEMXForw'/include/cpp/gen'
    'set OBJC_INCLUDE_PATH='sEMXForw'/include'
    'set INFOPATH='sEMXForw'/info'
    'set BOOKSHELF='sEMXBack'\BOOK;%BOOKSHELF%'
    'set EMXBOOK=emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    'set HELPNDX=emxbook.ndx+%HELPNDX%'
    'set DPATH='sEMXBack'\book;%DPATH%'
    'set PATH='sEMXBack'\bin;%PATH%;'sMYSQL'\bin'
    'set TERM=mono'
    'set TERMCAP='sEMXForw'/etc/termcap.dat'

    exit(0);
