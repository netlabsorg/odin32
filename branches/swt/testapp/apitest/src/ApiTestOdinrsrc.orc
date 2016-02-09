//! GENINFO :: platform:OS/2, version:20.45, target:ApiTest.generate
/*****************************************************************************\
* ApiTestOdin.rc :: Odin Resources                                            *
* --------------------------------------------------------------------------- *
*                                                                             *
\*****************************************************************************/
#include    "ids.h"
//#include  <windows.h>


IDR_MAINMENU MENU
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "E&xit",           ID_FILE_EXIT
    END

    POPUP "&Testing"
    BEGIN
        MENUITEM "Test &1",         ID_TESTING_TEST1
        MENUITEM "Test &2",         ID_TESTING_TEST2
        MENUITEM "Test &3",         ID_TESTING_TEST3
        MENUITEM "Test &4",         ID_TESTING_TEST4
        MENUITEM "Test &5",         ID_TESTING_TEST5
        MENUITEM "Test &6",         ID_TESTING_TEST6
        MENUITEM "Test &7",         ID_TESTING_TEST7
        MENUITEM "Test &8",         ID_TESTING_TEST8
        MENUITEM "Test &9",         ID_TESTING_TEST9
    END

    POPUP "&Usp10"
    BEGIN
        MENUITEM "Test &1",         ID_USP10_TEST1
        MENUITEM "Test &2",         ID_USP10_TEST2
        MENUITEM "Test &3",         ID_USP10_TEST3
        MENUITEM "Test &4",         ID_USP10_TEST4
        MENUITEM "Test &5",         ID_USP10_TEST5
        MENUITEM "Test &6",         ID_USP10_TEST6
        MENUITEM "Test &7",         ID_USP10_TEST7
        MENUITEM "Test &8",         ID_USP10_TEST8
        MENUITEM "Test &9",         ID_USP10_TEST9
    END

    POPUP "&Gdiplus"
    BEGIN
        MENUITEM "Test &1",         ID_GDIPLUS_TEST1
        MENUITEM "Test &2",         ID_GDIPLUS_TEST2
        MENUITEM "Test &3",         ID_GDIPLUS_TEST3
        MENUITEM "Test &4",         ID_GDIPLUS_TEST4
        MENUITEM "Test &5",         ID_GDIPLUS_TEST5
        MENUITEM "Test &6",         ID_GDIPLUS_TEST6
        MENUITEM "Test &7",         ID_GDIPLUS_TEST7
        MENUITEM "Test &8",         ID_GDIPLUS_TEST8
        MENUITEM "Test &9",         ID_GDIPLUS_TEST9
    END

    POPUP "&Help"
    BEGIN
        MENUITEM "&About",          ID_HELP_ABOUT
    END
END

