/* $Id: odin32.e,v 1.8 2001-10-22 04:11:08 bird Exp $
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
 * Ctrl+Shift+T:
 *
 * Ctrl+Shift+B: KLOGENTRYX(..)
 * Ctrl+Shift+E: KLOGEXIT(..)
 * Ctrl+Shift+N: Do kLog stuff for the current file. No questions.
 * Ctrl+Shift+Q: Do kLog stuff for the current file. Ask a lot of questions.
 *
 * Remember to set the correct sOdin32UserName, sOdin32UserEmail and sOdin32UserInitials
 * before compiling and loading the macros into Visual SlickEdit.
 *
 * These macros are compatible with both 3.0(c) and 4.0(b).
 *
 */
defeventtab default_keys
def  'C-S-A' = odin32_signature
def  'C-S-B' = odin32_klogentry
def  'C-S-C' = odin32_classbox
def  'C-S-E' = odin32_klogexit
def  'C-S-F' = odin32_funcbox
def  'C-S-G' = odin32_globalbox
def  'C-S-H' = odin32_headerbox
def  'C-S-I' = odin32_intfuncbox
def  'C-S-K' = odin32_constbox
def  'C-S-M' = odin32_modulebox
def  'C-S-N' = odin32_klog_file_no_ask
def  'C-S-O' = odin32_oneliner
def  'C-S-Q' = odin32_klog_file_ask
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
        _insert_text("; $Id: odin32.e,v 1.8 2001-10-22 04:11:08 bird Exp $\n");
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
        _insert_text("/* $Id: odin32.e,v 1.8 2001-10-22 04:11:08 bird Exp $\n");
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


/**
 * Hot-Key: Inserts a KLOGENTRY statement at start of nearest function.
 */
void odin32_klogentry()
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (!odin32_func_goto_nearest())
    {
        /*
         * Get parameters.
         */
        _str sParams = odin32_getparams();
        if (sParams)
        {
            /*
             * Insert text.
             */
            if (!search("{"))
            {
                p_col++;
                cArgs = odin32_countparams(sParams);
                if (cArgs > 0)
                {
                    sArgs = "";
                    for (i = 0; i < cArgs; i++)
                    {
                        _str sType, sName, sDefault
                        if (!odin32_enumparams(sParams, i, sType, sName, sDefault))
                            sArgs = sArgs', 'sName;
                    }

                    _insert_text("\n    KLOGENTRY"cArgs"(\""sParams"\""sArgs");"); /* todo tab size.. or smart indent */
                }
                else
                    _insert_text("\n    KLOGENTRY0();"); /* todo tab size.. or smart indent */

                /*
                 * Check if the next word is KLOGENTRY.
                 */
                next_word();
                if (def_next_word_style == 'E')
                    prev_word();
                if (substr(cur_word(iIgnorePos), 1, 9) == "KLOGENTRY")
                    delete_line();

            }
            else
                message("didn't find {");
        }
        else
            message("odin32_getparams failed, sParams=" sParams);
        return;
    }

    _restore_pos2(org_pos);
}


/**
 * Hot-Key: Inserts a KLOGEXIT statement at cursor location.
 */
void odin32_klogexit()
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (!prev_proc())
    {
        /*
         * Get parameters.
         */
        _str sType = odin32_getreturntype(true);
        _restore_pos2(org_pos);
        if (sType)
        {
            /*
             * Insert text.
             */
            cur_col = p_col;
            if (sType == 'void' || sType == 'VOID')
            {
                while (p_col <= p_SyntaxIndent)
                    keyin(" ");
                _insert_text("KLOGEXITVOID();");
            }
            else
            {
                if (sType == 'unsigned' || sType == 'UNSIGNED' || sType == 'UINT')
                    _insert_text("KLOGEXITUINT();");
                else if (sType == 'int' || sType == 'INT')
                    _insert_text("KLOGEXITINT();");
                else if (sType == 'void *' || sType == 'VOID *' || sType == 'PVOID' || sType == 'LPVOID')
                    _insert_text("KLOGEXITHEX();");
                else
                    _insert_text("KLOGEXIT(\""sType"\", );");
                _insert_text("\n");
                for (i = 1; i < cur_col; i++)
                    _insert_text(" ");
                search(")","E-");

                /*
                 * Check if it's a simple return statement.
                 */
                _save_pos2(valuepos);
                next_word();
                if (def_next_word_style == 'E')
                    prev_word();
                if (cur_word(iIgnorePos) == 'return')
                {
                    p_col += length('return');
                    _save_pos2(posStart);
                    offStart = _QROffset();
                    if (!search(";", "E+"))
                    {
                        offEnd = _QROffset();
                        _restore_pos2(posStart);
                        _str sValue = strip(get_text(offEnd - offStart));
                        if (substr(sValue, 1, 1) == '(' && substr(sValue, length(sValue), 1) == ')')
                            sValue = strip(substr(sValue, 2, length(sValue) - 2));
                        _restore_pos2(valuepos);
                        _save_pos2(valuepos);
                        _insert_text(sValue);
                    }
                }
                _restore_pos2(valuepos);

                /*
                 * Remove old KLOGEXIT statement on previous line if any.
                 */
                _save_pos2(valuepos);
                newexitline = p_line;
                p_line--; p_col = 1;
                next_word();
                if (def_next_word_style == 'E')
                    prev_word();
                if (p_line == newexitline - 1 && substr(cur_word(iIgnorePos), 1, 8) == 'KLOGEXIT')
                    delete_line();
                _restore_pos2(valuepos);
            }
        }
        else
            message("odin32_getparams failed, sType=" sType);
        return;
    }

    _restore_pos2(org_pos);
}


/**
 * Processes a file - ask user all the time.
 */
void odin32_klog_file_ask()
{
    odin32_klog_file_int(true)
}


/**
 * Processes a file - no questions.
 */
void odin32_klog_file_no_ask()
{
    odin32_klog_file_int(false)
}



/**
 * Processes a file.
 */
static void odin32_klog_file_int(boolean fAsk)
{
    show_all();
    bottom();
    _refresh_scroll();

    /* ask question so we can get to the right position somehow.. */
    if (fAsk && _message_box("kLog process this file?", "Visual SlickEdit", MB_YESNO | MB_ICONQUESTION) != IDYES)
        return;

    /*
     * Entries.
     */
    while (!prev_proc())
    {
        //say 'entry main loop: ' odin32_getfunction();

        /*
         * Skip prototypes.
         */
        if (odin32_prototype())
            continue;

        /*
         * Ask user.
         */
        center_line();
        _refresh_scroll();
        sFunction = odin32_getfunction();
        rc = fAsk ? _message_box("Process this function ("sFunction")?", "Visual SlickEdit", MB_YESNOCANCEL | MB_ICONQUESTION) : IDYES;
        if (rc == IDYES)
        {
            _save_pos2(procpos);
            odin32_klogentry();
            _restore_pos2(procpos);
        }
        else if (rc == IDNO)
            continue;
        else
            break;
    }

    /*
     * Exits.
     */
    bottom(); _refresh_scroll();
    boolean fUserCancel = false;
    while (!prev_proc() && !fUserCancel)
    {
        _save_pos2(procpos);
        sCurFunction = odin32_getfunction();
        //say 'exit main loop: ' sCurFunction

        /*
         * Skip prototypes.
         */
        if (odin32_prototype())
            continue;

        /*
         * Select procedure.
         */
        while (!search("return", "WE<+") && odin32_getfunction() == sCurFunction)
        {
            //say 'exit sub loop: ' p_line
            /*
             * Ask User.
             */
            center_line();
            _refresh_scroll();
            sFunction = odin32_getfunction();
            rc =  fAsk ? _message_box("Process this exit from "sFunction"?", "Visual SlickEdit", MB_YESNOCANCEL | MB_ICONQUESTION) : IDYES;
            deselect();
            if (rc == IDYES)
            {
                _save_pos2(returnpos);
                odin32_klogexit();
                _restore_pos2(returnpos);
                p_line++;
            }
            else if (rc != IDNO)
            {
                fUserCancel = true;
                break;
            }
            p_line++;                       /* just so we won't hit it again. */
        }

        /*
         * If void function we'll have to check if there is and return; prior to the ending '}'.
         */
        _restore_pos2(procpos);
        _save_pos2(procpos);
        sType = odin32_getreturntype(true);
        if (!fUserCancel && sType && (sType == 'void' || sType == 'VOID'))
        {
            if (    !search("{", "E+")
                &&  !find_matching_paren())
            {
                _save_pos2(funcend);
                prev_word();
                if (cur_word(iIgnorePos) != "return")
                {
                    /*
                     * Ask User.
                     */
                    _restore_pos2(funcend);
                    center_line();
                    _refresh_scroll();
                    sFunction = odin32_getfunction();
                    rc = fAsk ? _message_box("Process this exit from "sFunction"?", "Visual SlickEdit", MB_YESNOCANCEL | MB_ICONQUESTION) : IDYES;
                    deselect();
                    if (rc == IDYES)
                    {
                        _save_pos2(returnpos);
                        odin32_klogexit();
                        _restore_pos2(returnpos);
                    }
                }
            }
        }

        /*
         * Next proc.
         */
        _restore_pos2(procpos);
    }


}



/**
 * Moves cursor to nearest function start.
 */
int odin32_func_goto_nearest()
{
    cur_line = p_line;
    prev_line = -1;
    next_line = -1;
    _save_pos2(org_pos);

    if (!prev_proc(1))
    {
        prev_line = p_line;
        if (!next_proc(1) && p_line == cur_line)
        {
            _restore_pos2(org_pos);
            return 0;
        }
        _restore_pos2(org_pos);
        _save_pos2(org_pos);
    }

    if (!next_proc(1))
    {
        next_line = p_line;
        if (!prev_proc(1) && p_line == cur_line)
        {
            _restore_pos2(org_pos);
            return 0;
        }
        _restore_pos2(org_pos);
        _save_pos2(org_pos);
    }

    if (prev_line != -1 && (next_line == -1 || cur_line - prev_line <= next_line - cur_line))
    {
        prev_proc(1);
        return 0;
    }

    if (next_line != -1 && (prev_line == -1 || cur_line - prev_line > next_line - cur_line))
    {
        next_proc();
        return 0;
    }

    return -1;
}


/**
 * Check if nearest function is a prototype.
 */
boolean odin32_prototype()
{
    /*
     * Check if this is a real function implementation.
     */
    _save_pos2(procpos);
    if (!odin32_func_goto_nearest())
    {
        proc_line = p_line;

        if (!search("{", "E+"))
        {
            prev_proc();
            if (p_line != proc_line)
            {
                _restore_pos2(procpos);
                return true;
            }
        }
    }
    _restore_pos2(procpos);

    return false;
}


/**
 * Gets the name fo the current function.
 */
static _str odin32_getfunction()
{
    return current_proc();
}


/**
 * Goes to the neares function and gets its parameters.
 * @remark  Should be reimplemented to use tags (if someone can figure out how to query that stuff).
 */
static _str odin32_getparams()
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (    !odin32_func_goto_nearest()
        &&  !search("(")               /* makes some assumptions. */
        )
    {
        /*
         * Get parameters.
         */
        _save_pos2(posStart);
        offStart = _QROffset();
        if (!find_matching_paren())
        {
            offEnd = _QROffset();
            _restore_pos2(posStart);
            p_col++;
            _str sParamsRaw = strip(get_text(offEnd - offStart - 1));


            /*
             * Remove new lines and double spaces within params.
             */
            _str sParams = "";

            int i;
            for (i = 1, chPrev = ' '; i <= length(sParamsRaw); i++)
            {
                ch = substr(sParamsRaw, i, 1);

                /*
                 * Do fixups.
                 */
                if (ch == " " && chPrev == " ")
                        continue;

                if ((ch :== "\n") || (ch :== "\r") || (ch :== "\t"))
                {
                    if (chPrev == ' ')
                        continue;
                    ch = ' ';
                }

                if (ch == ',' && chPrev == ' ')
                {
                    sParams = substr(sParams, 1, length(sParams) - 1);
                }

                if (ch == '*')
                {
                    if (chPrev != ' ')
                        sParams = sParams :+ ' * ';
                    else
                        sParams = sParams :+ '* ';
                    chPrev = ' ';
                }
                else
                {
                    sParams = sParams :+ ch;
                    chPrev = ch;
                }

            } /* for */

            sParams = strip(sParams);
            if (sParams == 'void' || sParams == 'VOID')
                sParams = "";
            _restore_pos2(org_pos);
            return sParams;
        }
        else
            message("find_matchin_paren failed");
    }

    _restore_pos2(org_pos);
    return false;
}



/**
 * Enumerates the parameters to the function.
 * @param   sParams     Parameter string from odin32_getparams.
 * @param   iParam      The index (0-based) of the parameter to get.
 * @param   sType       Type. (output)
 * @param   sName       Name. (output)
 * @param   sDefault    Default value. (output)
 * @remark  Doesn't perhaps handle function pointers very well (I think)?
 * @remark  Should be reimplemented to use tags (if someone can figure out how to query that stuff).
 */
static int odin32_enumparams(_str sParams, int iParam, _str &sType, _str &sName, _str &sDefault)
{
    int     i;
    int     iParLevel;
    int     iCurParam;
    int     iStartParam;

    sType = sName = sDefault = "";

    /* no use working on empty string! */
    if (length(sParams) == 0)
        return -1;

    /* find the parameter in question */
    for (iStartParam = i = 1, iParLevel = iCurParam = 0; i <= length(sParams); i++)
    {
        _str ch = substr(sParams, i, 1);
        if (ch == ',' && iParLevel == 0)
        {
            /* is it this parameter ? */
            if (iParam == iCurParam)
                break;

            iCurParam++;
            iStartParam = i + 1;
        }
        else if (ch == '(')
            iParLevel++;
        else if (ch == ')')
            iParLevel--;
    }

    /* did we find the parameter? */
    if (iParam == iCurParam)
    {   /* (yeah, we did!) */
        sArg = strip(substr(sParams, iStartParam, i - 1));

        /* lazy approach, which doens't support function types */

        if (pos('=', sParams) > 0)      /* default */
        {
            sDefault = strip(substr(sParams, pos('=', sParams) + 1));
            sArg = strip(substr(sArg, 1, pos('=', sParams) - 1));
        }

        for (i = length(sArg); i > 1; i--)
        {
            _str ch = substr(sArg, i, 1);
            if (    !(ch >= 'a' &&  ch <= 'z')
                &&  !(ch >= 'A' &&  ch <= 'Z')
                &&  !(ch >= '0'  && ch <= '9')
                &&  ch != '_' && ch != '$')
                break;
        }
        sName = strip(substr(sArg, i + 1));
        sType = strip(substr(sArg, 1, i));

        return 0;
    }

    return -1;
}


/**
 * Counts the parameters to the function.
 * @param   sParams     Parameter string from odin32_getparams.
 * @remark  Should be reimplemented to use tags (if someone can figure out how to query that stuff).
 */
static int odin32_countparams(_str sParams)
{
    int     i;
    int     iParLevel;
    int     iCurParam;

    sType = sName = sDefault = "";

    /* check for 0 parameters */
    if (length(sParams) == 0)
        return 0;

    /* find the parameter in question */
    for (i = 1, iParLevel = iCurParam = 0; i <= length(sParams); i++)
    {
        _str ch = substr(sParams, i, 1);
        if (ch == ',' && iParLevel == 0)
        {
            iCurParam++;
        }
        else if (ch == '(')
            iParLevel++;
        else if (ch == ')')
            iParLevel--;
    }

    return iCurParam + 1;
}

/**
 * Gets the return type.
 */
static _str odin32_getreturntype(boolean fPureType = false)
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (!odin32_func_goto_nearest())
    {
        /*
         * Return type is from function start to function name...
         */
        _save_pos2(posStart);
        offStart = _QROffset();

        if (!search("("))               /* makes some assumptions. */
        {
            prev_word();
            offEnd = _QROffset();
            _restore_pos2(posStart);
            _str sTypeRaw = strip(get_text(offEnd - offStart - 1));

            /*
             * Remove static, inline, _Optlink, stdcall, EXPENTRY etc.
             */
            if (fPureType)
            {
                sTypeRaw = stranslate(sTypeRaw, "", "__static__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__static", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "static__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "static", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__inline__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__inline", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "inline__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "inline", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "EXPENTRY", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_Optlink", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__stdcall", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__cdecl", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_cdecl", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "cdecl", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "PASCAL", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__far", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_far", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "far", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__near", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_near", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "near", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "WIN32API", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "WINAPI", "I");
            }

            /*
             * Remove new lines and double spaces within params.
             */
            _str sType = "";

            int i;
            for (i = 1, chPrev = ' '; i <= length(sTypeRaw); i++)
            {
                ch = substr(sTypeRaw, i, 1);

                /*
                 * Do fixups.
                 */
                if (ch == " " && chPrev == " ")
                        continue;

                if ((ch :== "\n") || (ch :== "\r") || (ch :== "\t"))
                {
                    if (chPrev == ' ')
                        continue;
                    ch = ' ';
                }

                if (ch == ',' && chPrev == ' ')
                {
                    sType = substr(sType, 1, length(sType) - 1);
                }

                if (ch == '*')
                {
                    if (chPrev != ' ')
                        sType = sType :+ ' * ';
                    else
                        sType = sType :+ '* ';
                    chPrev = ' ';
                }
                else
                {
                    sType = sType :+ ch;
                    chPrev = ch;
                }

            } /* for */

            sType = strip(sType);

            _restore_pos2(org_pos);
            return sType;
        }
        else
            message('odin32_getreturntype: can''t find ''(''.');
    }

    _restore_pos2(org_pos);
    return false;
}
