/* $Id: odin32env.cmd,v 1.1 2000-04-27 11:32:25 bird Exp $
 *
 * Sets the build environment.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
    'SET TOOLKIT=c:\appl\os2\toolkt40';
    'SET VACPP_SHARED=FALSE'
    'SET CPPHELP_INI=C:\OS2\SYSTEM'
    'SET CPPLOCAL=c:\appl\os2\IBMCPP30'
    'SET CPPMAIN=%CPPLOCAL%'
    'SET CPPWORK=%CPPLOCAL%'
    'SET SMINCLUDE=%TOOLKIT%\som\include;%TOOLKIT%\idl;%CPPLOCAL%\INCLUDE\SOM'
    'SET TMP=c:\temp'
    'SET HELPNDX=EPMKWHLP.NDX+CPP.NDX+CPPBRS.NDX'

    'SET INCLUDE=%CPPLOCAL%\INCLUDE;%CPPLOCAL%\INCLUDE\SOM;%TOOLKIT%\h;%include%;'
    'SET LIB=%CPPLOCAL%\LIB;%lib%;'
    'SET LIB=%TOOLKIT%\SPEECH\LIB;%TOOLKIT%\SAMPLES\MM\LIB;%TOOLKIT%\LIB;%TOOLKIT%\SOM\LIB;%TOOLKIT%\OPENDOC\BASE\LIB;%LIB%';
    'SET LOCPATH=%CPPLOCAL%\LOCALE;%LOCPATH%'
    'SET HELP=%CPPLOCAL%\HELP;%HELP%;'
    'SET BEGINLIBPATH=%CPPLOCAL%\DLL;%BeginLIBPATH%;'

    'SET PATH=%CPPLOCAL%\BIN;d:\knut\ddk\ddktools\toolkits\msc60\binp;d:\knut\cvs\bin;%Path%'
    'SET PATH=%TOOLKIT%\BETA\BIN;%TOOLKIT%\SOM\COMMON;%TOOLKIT%\SOM\BIN;%TOOLKIT%\OPENDOC\BASE\BIN;%TOOLKIT%\BIN;%Path%;d:\knut\ddk\base\tools';

    'SET DPATH=%CPPLOCAL%\HELP;%CPPLOCAL%;%CPPLOCAL%\LOCALE;%DPATH%'
    'SET DPATH=%TOOLKIT%\SOM\COMMON\SYSTEM;%TOOLKIT%\SOM\MSG;%DPath%';
    'SET DPATH=%TOOLKIT%\MSG;%TOOLKIT%\OPENDOC\BASE\MSG;%TOOLKIT%\BOOK;%DPath'

    'SET BOOKSHELF=c:\appl\os2\toolkt40\archived;c:\appl\os2\toolkt40\book;c:\appl\os2\toolkt40\books;c:\appl\os2\ibmcpp30\help;%BOOKSHELF%';

    'SET NLSPATH=C:\MPTN\MSG\NLS\%%N;C:\TCPIP\MSG\ENUS850\%%N;C:\netqos2\%%N;C:\appl\os2\toolkt40\OPENDOC\BASE\LOCALE\EN_US\%%N;C:\appl\os2\toolkt40\MSG\%%N;'

    'SET CVSROOT=:pserver:bird@www.netlabs.org:d:/netlabs.src/odin32'
    'SET HOME=d:\knut\home'
    'SET USER=bird'
    'SET DEBUG=1'

    /* emx + mysql */
    'set C_INCLUDE_PATH=C:/emx/include'
    'set LIBRARY_PATH=C:/emx/lib'
    'set CPLUS_INCLUDE_PATH=C:/emx/include/cpp;C:/emx/include'
    'set PROTODIR=C:/emx/include/cpp/gen'
    'set OBJC_INCLUDE_PATH=C:/emx/include'
    'set TERM=mono'
    'set TERMCAP=C:/emx/etc/termcap.dat'
    'set INFOPATH=C:/emx/info'
    'set BOOKSHELF=C:\OS2\BOOK;C:\EMX\BOOK;%BOOKSHELF%'
    'set EMXBOOK=emxdev.inf+emxlib.inf+emxgnu.inf+emxbsd.inf'
    'set HELPNDX=emxbook.ndx+%HELPNDX%'
    'set DPATH=C:\emx\book;%DPATH%'
    'set PATH=C:\emx\bin;%PATH%;c:\emx\mysql\bin'
    exit(0);
