/* $Id: odin32.e,v 1.6 2000-10-02 04:08:49 bird Exp $
 *
 * Visual SlickEdit Documentation Macros.
 *
 * Copyright (c) 1999-2000 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 ****
 *
 * This define the following keys:
 *---------------------------------
 * Ctrl+Shift+C: Class description box.
 * Ctrl+Shift+F: Function/method description box.
 * Ctrl+Shift+M: Module(file) description box
 * Ctrl+Shift+O: One-liner (comment)
 *
 * Ctrl+Shift+G: Global box
 * Ctrl+Shift+H: Header box
 * Ctrl+Shift+I: Internal function box
 * Ctrl+Shift+K: Const/macro box
 * Ctrl+Shift+S: Struct/Typedef box
 *
 * Ctrl+Shift+T: Makes tag file
 *
 * Remember to set the correct sOdin32UserName, sOdin32UserEmail and sOdin32UserInitials
 * before compiling and loading the macros into Visual SlickEdit.
 *
 * These macros are compatible with both 3.0(c) and 4.0(b).
 *
 */
defeventtab default_keys
def  'C-S-A' = odin32_signature
def  'C-S-C' = odin32_classbox
def  'C-S-F' = odin32_funcbox
def  'C-S-G' = odin32_globalbox
def  'C-S-H' = odin32_headerbox
def  'C-S-I' = odin32_intfuncbox
def  'C-S-K' = odin32_constbox
def  'C-S-M' = odin32_modulebox
def  'C-S-O' = odin32_oneliner
def  'C-S-S' = odin32_structbox
def  'C-S-T' = odin32_maketagfile


//MARKER.  Editor searches for this line!
#pragma option(redeclvars, on)
#include 'slick.sh'

/* Remeber to change these! */
static _str sOdin32UserInitials = "kso";
static _str sOdin32UserName     = "knut st. osmundsen";
static _str sOdin32UserEmail    = "knut.stange.osmundsen@mynd.no";



/**
 * Insers a date string. The date is in ISO format.
 */
void odin32_date()
{
    int i,j;
    _str date;

    date = _date('U');
    i = pos("/", date);
    j = pos("/", date, i+1);
    month = substr(date, 1, i-1);
    if (length(month) == 1) month = '0'month;
    day   = substr(date, i+1, j-i-1);
    if (length(day)   == 1) day   = '0'day;
    year  = substr(date, j+1);
    _insert_text(nls("%s-%s-%s", year, month, day));
}


/**
 * Get the current year.
 * @returns   Current year string.
 */
_str odin32_year()
{
    date = _date('U');
    return  substr(date, pos("/",date, pos("/",date)+1)+1, 4);
}


/**
 * Inserts the first line in a box.
 * @param     sTag  Not used - box tag.
 */
static void odin32_firstline(sTag)
{
    begin_line();
    if (file_eq(p_extension, 'asm'))
        _insert_text(";\n");
    else
    {
        _insert_text("/");
        for (i = 0; i < 80-1; i++)
            _insert_text("*");
        _insert_text("\n");
    }
}

/**
 * Inserts a line with a '*' in both ends and some text (a) inside.
 * @param     a  text.
 */
void odin32_starlinestr(a)
{
    if (file_eq(p_extension, 'asm'))
    {
        _insert_text("; ");
        _insert_text(a);
        _insert_text("\n");
    }
    else
    {
        _insert_text("* ");
        _insert_text(a);
        for (i = 0; i < 80-3-length(a); i++)
            _insert_text(" ");
        _insert_text("*\n");
    }
}


/**
 * Empty line with a '*' in both ends.
 */
void odin32_starline()
{
    odin32_starlinestr("");
}


/**
 * Inserts the last line in a box.
 */
void odin32_lastline()
{
    if (file_eq(p_extension, 'asm'))
        _insert_text(";\n");
    else
    {
        for (i = 0; i < 80-1; i++)
            _insert_text("*");
        _insert_text("/\n");
    }
}



/**
 * Inserts a signature. Form: "//Initials ISO-date:"
 * @remark    defeventtab
 */
void odin32_signature()
{
    if (file_eq(p_extension, 'asm'))
        _insert_text(";"sOdin32UserInitials" ");
    else
        _insert_text("//"sOdin32UserInitials" ");

    odin32_date()
    _insert_text(":");
}


/**
 * SDS - Classbox(/header).
 * @remark    defeventtab
 */
void odin32_classbox()
{
    _begin_line();
    _insert_text("/**\n");
    _insert_text(" * \n");
    _insert_text(" * @shortdesc   \n");
    _insert_text(" * @dstruct     \n");
    _insert_text(" * @version     \n");
    _insert_text(" * @verdesc     \n");
    _insert_text(" * @author      " sOdin32UserName " (" sOdin32UserEmail ")\n");
    _insert_text(" * @approval    \n");
    _insert_text(" */\n");

    up(8);
    p_col += 3;
}


/**
 * SDS - functionbox(/header).
 * @remark    defeventtab
 */
void odin32_funcbox()
{
    _begin_line();
    if (file_eq(p_extension, 'asm'))
    {
        _insert_text(";;\n");
        _insert_text("; \n");
        _insert_text("; @cproto   \n");
        _insert_text("; @returns  \n");
        _insert_text("; @param    \n");
        _insert_text("; @uses     \n");
        _insert_text("; @equiv    \n");
        _insert_text("; @time     \n");
        _insert_text("; @sketch   \n");
        _insert_text("; @status   \n");
        _insert_text("; @author   "sOdin32UserName" (" sOdin32UserEmail ")\n");
        _insert_text("; @remark   \n");
        up(11);
        p_col = 3;
    }
    else
    {
        _insert_text("/**\n");
        _insert_text(" * \n");
        _insert_text(" * @returns \n");
        _insert_text(" * @param   \n");
        _insert_text(" * @equiv   \n");
        _insert_text(" * @time    \n");
        _insert_text(" * @sketch  \n");
        _insert_text(" * @status  \n");
        _insert_text(" * @author  "sOdin32UserName" (" sOdin32UserEmail ")\n");
        _insert_text(" * @remark  \n");
        _insert_text(" */\n");
        up(10);
        p_col = 4;
    }
}


/**
 *
 * @remark    defeventtab
 */
void odin32_globalbox()
{
    odin32_firstline("Global");
    odin32_starlinestr("  Global Variables");
    odin32_lastline();
}


void odin32_headerbox()
{
    odin32_firstline("Header");
    odin32_starlinestr("  Header Files");
    odin32_lastline();
}


void odin32_intfuncbox()
{
    odin32_firstline("IntFunc");
    odin32_starlinestr("  Internal Functions");
    odin32_lastline();
}


void odin32_constbox()
{
    odin32_firstline("Const");
    odin32_starlinestr("  Defined Constants And Macros");
    odin32_lastline();
}


void odin32_oneliner()
{
    end_line();
    do
    {
        _insert_text(" ");
    } while (p_col < 41);

    if (file_eq(p_extension, 'asm'))
    {
        _insert_text("; ");
    }
    else
    {
        _insert_text("/*  */");
        p_col = p_col - 3;
    }
}


void odin32_structbox()
{
    odin32_firstline("Struct");
    odin32_starlinestr("  Structures and Typedefs");
    odin32_lastline();
}


void odin32_modulebox()
{
    _begin_line();
    if (file_eq(p_extension, 'asm'))
    {
        _insert_text("; $Id: odin32.e,v 1.6 2000-10-02 04:08:49 bird Exp $\n");
        _insert_text("; \n");
        _insert_text("; \n");
        _insert_text("; \n");
        _insert_text("; Copyright (c) " odin32_year() " "sOdin32UserName" (" sOdin32UserEmail ")\n");
        _insert_text("; \n");
        _insert_text("; Project Odin Software License can be found in LICENSE.TXT\n");
        _insert_text("; \n");
        up(6);
        p_col = 3;
    }
    else
    {
        _insert_text("/* $Id: odin32.e,v 1.6 2000-10-02 04:08:49 bird Exp $\n");
        _insert_text(" * \n");
        _insert_text(" * \n");
        _insert_text(" * \n");
        _insert_text(" * Copyright (c) " odin32_year() " "sOdin32UserName" (" sOdin32UserEmail ")\n");
        _insert_text(" *\n");
        _insert_text(" * Project Odin Software License can be found in LICENSE.TXT\n");
        _insert_text(" *\n");
        _insert_text(" */\n");
        up(7);
        p_col = 4;
    }
}


_command void odin32_maketagfile()
{
    /* We'll */
    if (file_match('-p 'maybe_quote_filename(strip_filename(_project_name,'e'):+TAG_FILE_EXT),1)=="")
        _project_update_files_retag(true,false,false,true);
    else
        _project_update_files_retag(false,false,false,false);
}

_command void odin32_setcurrentdir()
{
    //_ini_get_value(_project_name,"COMPILER","WORKINGDIR", workingdir);
    //cd(workingdir);
    /* Go the the directory containing the project filename */
    cd(strip_filename(_project_name, 'NE'));
}

