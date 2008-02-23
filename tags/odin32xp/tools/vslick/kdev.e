/* $Id: kdev.e,v 1.8 2003-11-26 10:24:09 bird Exp $
 *
 * Visual SlickEdit Documentation Macros.
 *
 * Copyright (c) 1999-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/***
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
 * Ctrl+Shift+E: Exported Symbols
 * Ctrl+Shift+I: Internal function box
 * Ctrl+Shift+K: Const/macro box
 * Ctrl+Shift+S: Struct/Typedef box
 *
 * Ctrl+Shift+A: Signature+Date marker
 * Ctrl+Shift+P: Mark line as change by me
 *
 * Ctrl+Shift+T: Update project tagfile.
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
def  'C-S-A' = k_signature
def  'C-S-C' = k_javadoc_classbox
def  'C-S-E' = k_box_exported
def  'C-S-F' = k_javadoc_funcbox
def  'C-S-G' = k_box_globals
def  'C-S-H' = k_box_headers
def  'C-S-I' = k_box_intfuncs
def  'C-S-K' = k_box_consts
def  'C-S-M' = k_javadoc_moduleheader
def  'C-S-O' = k_oneliner
def  'C-S-P' = k_mark_modified_line
def  'C-S-S' = k_box_structs
def  'C-S-T' = odin32_maketagfile

//optional stuff
//def  'C-S-Q' = odin32_klog_file_ask
//def  'C-S-N' = odin32_klog_file_no_ask
//def  'C-S-1' = odin32_klogentry
//def  'C-S-3' = odin32_klogexit


//MARKER.  Editor searches for this line!
#pragma option(redeclvars, on)
#include 'slick.sh'

/* Remeber to change these! */
static _str skUserInitials  = "kso";
static _str skUserName      = "knut st. osmundsen";
static _str skUserEmail     = "bird-srcspam@anduin.net";


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static _str     skCodeStyle     = 'Opt2Ind4'; /* coding style scheme. */
static _str     skDocStyle      = 'javadoc';/* options: javadoc, */
static _str     skLicense       = 'GPL';    /* options: GPL, LGPL, Odin32, Confidential */
static _str     skCompany       = '';       /* empty or company name for copyright */
static _str     skProgram       = '';       /* Current program name - used by [L]GPL */
static _str     skChange        = '';       /* Current change identifier. */

static int      ikStyleWidth    = 80;       /* The page width of the style. */
static boolean  fkStyleFullHeaders = false; /* false: omit some tags. */
static int      ikStyleOneliner = 41;       /* The online comment column. */
static int      ikStyleModifyMarkColumn = 105;
static boolean  fkStyleBoxTag   = false;    /* true: Include tag in k_box_start. */

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
/**
 * Gets iso date.
 * @returns ISO formatted date.
 */
static _str k_date()
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
    return year"-"month"-"day;
}


/**
 * Get the current year.
 * @returns   Current year string.
 */
static _str k_year()
{
    date = _date('U');
    return  substr(date, pos("/",date, pos("/",date)+1)+1, 4);
}


/**
 * Aligns a value up to a given alignment.
 */
static int k_alignup(int iValue, iAlign)
{
    if (iAlign <= 0)
    {
        message('k_alignup: iValue='iValue ' iAlign='iAlign);
        iAlign = 4;
    }
    return ((iValue intdiv iAlign) + 1) * iAlign;
}


/**
 * Reads the comment setup for this lexer/extension .
 *
 * @returns Success indicator.
 * @param   sLeft       Left comment. (output)
 * @param   sRight      Right comment. (output)
 * @param   iColumn     Comment mark column. (1-based) (output)
 * @param   sExt        The extension to lookup defaults to the current one.
 * @param   sLexer      The lexer to lookup defaults to the current one.
 * @author  knut st. osmundsen (bird@anduin.net)
 * @remark  This should be exported from box.e, but unfortunately it isn't.
 */
static boolean k_commentconfig(_str &sLeft, _str &sRight, int &iColumn, _str sExt = p_extension, _str sLexer = p_lexer_name)
{
    /* init returns */
    sLeft = sRight = '';
    iColumn = 0;

    /*
     * Get comment setup from the lexer.
     */
    if (sLexer)
    {
        sLine = '';
        /* multiline */
        rc = _ini_get_value(slick_path_search("user.vlx"), sLexer, 'mlcomment', sLine);
        if (rc)
            rc = _ini_get_value(slick_path_search("vslick.vlx"), sLexer, 'mlcomment', sLine);
        if (!rc)
        {
            sLeft  = strip(word(sLine, 1));
            sRight = strip(word(sLine, 2));
            if (sLeft != '' && sRight != '')
                return true;
        }

        /* failed, try single line. */
        rc = _ini_get_value(slick_path_search("user.vlx"), sLexer, 'linecomment', sLine);
        if (rc)
            rc = _ini_get_value(slick_path_search("vslick.vlx"), sLexer, 'linecomment', sLine);
        if (!rc)
        {
            sLeft = strip(word(sLine, 1));
            sRight = '';
            iColumn = 0;
            sTmp = word(sLine, 2);
            if (isnumber(sTmp))
                iColumn = (int)sTmp;
            if (sLeft != '')
                return true;
        }
    }

    /*
     * Read the nonboxchars and determin user or default box.ini.
     */
    sFile = slick_path_search("ubox.ini");
    frc = _ini_get_value(sFile, sExt, 'nonboxchars', sLine);
    if (frc)
    {
        sFile = slick_path_search("box.ini")
        frc = _ini_get_value(sFile, sExt, 'nonboxchars', sLine);
    }

    if (!frc)
    {   /*
         * Found extension.
         */
        sLeft = strip(eq_name2value('left',sLine));
        if (sLeft  == '\e') sLeft = '';
        sRight = strip(eq_name2value('right',sLine));
        if (sRight == '\e') sRight = '';

        /* Read comment column too */
        frc = _ini_get_value(sFile, sExt, 'comment_col', sLine);
        if (frc)
        {
            iColumn = eq_name2value('comment_col', sLine);
            if (iColumn == '\e') iColumn = 0;
        }
        else
            iColumn = 0;
        return true;
    }

    /* failure */
    sLeft = sRight = '';
    iColumn = 0;

    return false;
}


/**
 * Checks if current file only support line comments.
 * @returns True / False.
 * @remark  Use builtin extension stuff!
 */
static boolean k_line_comment()
{
    _str    sRight = '';
    boolean fLineComment = false;
    if (k_commentconfig(sLeft, sRight, iColumn))
        fLineComment = (sRight == '' || iColumn > 0);
    return fLineComment;
}



#define KIC_CURSOR_BEFORE 1
#define KIC_CURSOR_AFTER  2
#define KIC_CURSOR_AT_END 3

/**
 * Insert a comment at current or best fitting position in the text.
 * @param   sStr            The comment to insert.
 * @param   iCursor         Where to put the cursor.
 * @param   iPosition       Where to start the comment.
 *                          Doesn't apply to column based source.
 *                          -1 means at cursor position. (default)
 *                          >0 means at end of line, but not before this column (1-based).
 *                             This also implies a min of one space to EOL.
 */
void k_insert_comment(_str sStr, int iCursor, int iPosition = -1)
{
    _str    sLeft;
    _str    sRight;
    int     iColumn;
    if (!k_commentconfig(sLeft, sRight, iColumn))
    {
        sLeft = '/*'; sRight = '*/'; iColumn = 0;
    }

    if (iColumn <= 0)
    {   /*
         * not column based source
         */

        /* position us first */
        if (iPosition > 0)
        {
            end_line();
            do {
                _insert_text(" ");
            } while (p_col < iPosition);
        }

        /* insert comment saving the position for _BEFORE. */
        iCol = p_col;
        _insert_text(sLeft:+' ':+sStr);
        if (iCursor == KIC_CURSOR_AT_END)
            iCol = p_col;
        /* right comment delimiter? */
        if (sRight != '')
            _insert_text(' ':+sRight);
    }
    else
    {
        if (p_col >= iColumn)
            _insert_text("\n");
        do { _insert_text(" "); } while (p_col < iColumn);
        if (iCursor == KIC_CURSOR_BEFORE)
            iCol = p_col;
        _insert_text(sLeft:+' ':+sStr);
        if (iCursor == KIC_CURSOR_AT_END)
            iCol = p_col;
    }

    /* set cursor. */
    if (iCursor != KIC_CURSOR_AFTER)
        p_col = iCol;
}


/**
 * Gets the comment prefix or postfix.
 * @returns Comment prefix or postfix.
 * @param   fRight  If clear left comment string - default.
 *                  If set right comment string.
 * @author  knut st. osmundsen (bird@anduin.net)
 */
static _str k_comment(boolean fRight = false)
{
    sComment = '/*';
    if (k_commentconfig(sLeft, sRight, iColumn))
        sComment = (!fRight || iColumn > 0 ? sLeft : sRight);

    return strip(sComment);
}


/*******************************************************************************
*   BOXES                                                                      *
*******************************************************************************/

/**
 * Inserts the first line in a box.
 * @param     sTag  Not used - box tag.
 */
static void k_box_start(sTag)
{
    if (!k_commentconfig(sLeft, sRight, iColumn))
        return;
    _begin_line();
    if (iColumn >= 0)
        while (p_col < iColumn)
           _insert_text(" ");

    sText = sLeft;
    if (sTag != '' && fkStyleBoxTag)
    {
        if (substr(sText, length(sText)) != '*')
            sText = sText:+'*';
        sText = sText:+sTag;
    }

    for (i = length(sText); i <= ikStyleWidth - p_col; i++)
        sText = sText:+'*';
    sText = sText:+"\n";

    _insert_text(sText);
}


/**
 * Places a string, sStr, into a line started and ended by '*'.
 * @param   sStr    Text to have between the '*'s.
 */
static void k_box_line(_str sStr)
{
    if (!k_commentconfig(sLeft, sRight, iColumn))
        return;
    if (iColumn >= 0)
        while (p_col < iColumn)
           _insert_text(" ");

    sText = '';
    if (k_line_comment())
        sText = sLeft;
    if (sText == '' || substr(sText, length(sText)) != '*')
        sText = sText:+'*';

    sText = sText:+' ';
    for (i = length(sText); i < p_SyntaxIndent; i++)
        sText = sText:+' ';

    sText = sText:+sStr;

    for (i = length(sText) + 1; i <= ikStyleWidth - p_col; i++)
        sText = sText:+' ';
    sText = sText:+"*\n";

    _insert_text(sText);
}


/**
 * Inserts the last line in a box.
 */
static void k_box_end()
{
    if (!k_commentconfig(sLeft, sRight, iColumn))
        return;
    if (iColumn >= 0)
        while (p_col < iColumn)
           _insert_text(" ");

    sText = '';
    if (k_line_comment())
        sText = sLeft;
    for (i = length(sText) + length(sRight); i <= ikStyleWidth - p_col; i++)
        sText = sText:+'*';
    sText = sText:+sRight:+"\n";

    _insert_text(sText);
}



/*******************************************************************************
*   FUNCTION AND CODE PARSERS                                                  *
*******************************************************************************/
/**
 * Moves cursor to nearest function start.
 * @returns 0 if ok.
 *          -1 on failure.
 */
static int k_func_goto_nearest_function()
{
    boolean fFix = false;               /* cursor at function fix. (last function) */
    cur_line = p_line;
    prev_line = -1;
    next_line = -1;
    _save_pos2(org_pos);

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
    else
    {
        p_col++;                        /* fixes problem with single function files. */
        fFix = true;
    }

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


    if (prev_line != -1 && (next_line == -1 || cur_line - prev_line <= next_line - cur_line))
    {
        if (fFix)
            p_col++;
        prev_proc(1);
        return 0;
    }

    if (next_line != -1 && (prev_line == -1 || cur_line - prev_line > next_line - cur_line))
    {
        next_proc();
        return 0;
    }

    _restore_pos2(org_pos);
    return -1;
}


/**
 * Check if nearest function is a prototype.
 * @returns True if function prototype.
 *          False if not function prototype.
 */
static boolean k_func_prototype()
{
    /*
     * Check if this is a real function implementation.
     */
    _save_pos2(procpos);
    if (!k_func_goto_nearest_functions())
    {
        proc_line = p_line;

        if (!k_func_searchcode("{"))
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
 * @returns The current function name.
 */
static _str k_func_getfunction_name()
{
    sFunctionName = current_proc();
    if (!sFunctionName)
        sFunctionName = "";
    //say 'functionanme='sFunctionName;
    return sFunctionName;
}


/**
 * Goes to the neares function and gets its parameters.
 * @remark  Should be reimplemented to use tags (if someone can figure out how to query that stuff).
 */
static _str k_func_getparams()
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (    !k_func_goto_nearest_function()
        &&  !k_func_searchcode("(")     /* makes some assumptions. */
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
 * @param   sParams     Parameter string from k_func_getparams.
 * @param   iParam      The index (0-based) of the parameter to get.
 * @param   sType       Type. (output)
 * @param   sName       Name. (output)
 * @param   sDefault    Default value. (output)
 * @remark  Doesn't perhaps handle function pointers very well (I think)?
 * @remark  Should be reimplemented to use tags (if someone can figure out how to query that stuff).
 */
static int k_func_enumparams(_str sParams, int iParam, _str &sType, _str &sName, _str &sDefault)
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
        sArg = strip(substr(sParams, iStartParam, i - iStartParam));
        /* remove M$ stuff */
        sArg = stranslate(sArg, "", "IN", "E");
        sArg = stranslate(sArg, "", "OUT", "E");
        sArg = stranslate(sArg, "", "OPTIONAL", "E");
        sArg = strip(sArg);

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
        if (sArg == "...")
            i = 0;
        sName = strip(substr(sArg, i + 1));
        sType = strip(substr(sArg, 1, i));

        return 0;
    }

    return -1;
}


/**
 * Counts the parameters to the function.
 * @param   sParams     Parameter string from k_func_getparams.
 * @remark  Should be reimplemented to use tags (if someone can figure out how to query that stuff).
 */
static int k_func_countparams(_str sParams)
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
static _str k_func_getreturntype(boolean fPureType = false)
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (!k_func_goto_nearest_function())
    {
        /*
         * Return type is from function start to function name...
         */
        _save_pos2(posStart);
        offStart = _QROffset();

        if (!k_func_searchcode("("))               /* makes some assumptions. */
        {
            prev_word();
            offEnd = _QROffset();
            _restore_pos2(posStart);
            _str sTypeRaw = strip(get_text(offEnd - offStart));

            //say 'sTypeRaw='sTypeRaw;
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
                sTypeRaw = stranslate(sTypeRaw, "", "__PASCAL", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_PASCAL", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "PASCAL", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__Far32__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__Far32", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "Far32__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_Far32_", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_Far32", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "Far32_", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "Far32", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__far", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_far", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "far", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__near", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_near", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "near", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__loadds__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__loadds", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "loadds__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_loadds_", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_loadds", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "loadds_", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "loadds", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__loades__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__loades", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "loades__", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_loades_", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "_loades", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "loades_", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "loades", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "WIN32API", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "WINAPI", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "LDRCALL", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "KRNLCALL", "I");
                sTypeRaw = stranslate(sTypeRaw, "", "__operator__", "I"); /* operator fix */
                sTypeRaw = stranslate(sTypeRaw, "", "__operator", "I");   /* operator fix */
                sTypeRaw = stranslate(sTypeRaw, "", "operator__", "I");   /* operator fix */
                sTypeRaw = stranslate(sTypeRaw, "", "operator", "I");     /* operator fix */
                sTypeRaw = stranslate(sTypeRaw, "", "IN", "E");
                sTypeRaw = stranslate(sTypeRaw, "", "OUT", "E");
                sTypeRaw = stranslate(sTypeRaw, "", "OPTIONAL", "E");
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
            message('k_func_getreturntype: can''t find ''(''.');
    }

    _restore_pos2(org_pos);
    return false;
}


/**
 * Search for some piece of code.
 */
static int k_func_searchcode(_str sSearchString, _str sOptions = "E+")
{
    int rc;
    rc = search(sSearchString, sOptions);
    while (!rc && !k_func_in_code())
    {
        p_col++;
        rc = search(sSearchString, sOptions);
    }
    return rc;
}


/**
 * Checks if cursor is in code or in comment.
 * @return  True if cursor in code.
 */
static boolean k_func_in_code()
{
    _save_pos2(searchsave);
    boolean fRc = !_in_comment();
    _restore_pos2(searchsave);
    return fRc;
}


/*
 * Gets the next piece of code.
 */
static _str k_func_get_next_code_text()
{
    _str ch;
    _save_pos2(searchsave);
    ch = k_func_get_next_code_text2();
    _restore_pos2(searchsave);
    return ch;
}


/**
 * Checks if there is more code on the line.
 */
static boolean k_func_more_code_on_line()
{
    boolean fRc;
    int     curline = p_line;

    _save_pos2(searchsave);
    k_func_get_next_code_text2();
    fRc = curline == p_line;
    _restore_pos2(searchsave);

    return fRc;
}


/**
 * Gets the next piece of code.
 * Doesn't preserver cursor position.
 */
static _str k_func_get_next_code_text2()
{
    _str ch;
    do
    {
        curcol = ++p_col;
        end_line()
        if (p_col <= curcol)
        {
            p_line++;
            p_col = 1;
        }
        else
            p_col = curcol;

        ch = get_text();
        //say ch ' ('_asc(ch)')';
        while (ch == "#")                  /* preprocessor stuff */
        {
            p_col = 1;
            p_line++;
            ch = get_text();
            //say ch ' ('_asc(ch)')';
            continue;
        }
    } while (ch :== ' ' || ch :== "\t" || ch :== "\n" || ch :== "\r" || !k_func_in_code());

    return ch;
}




/*******************************************************************************
*   JAVA DOC STYLED WORKERS                                                    *
*******************************************************************************/

/** starts a javadoc documentation box. */
static void k_javadoc_box_start(_str sStr = '', boolean fDouble = true)
{
    if (!k_commentconfig(sLeft, sRight, iColumn))
        return;
    _begin_line();
    if (iColumn >= 0)
        while (p_col < iColumn)
           _insert_text(" ");

    sText = sLeft;
    if (fDouble)
        sText = sLeft:+substr(sLeft, length(sLeft), 1);
    if (sStr != '')
        sText = sText:+' ':+sStr;
    sText = sText:+"\n";

    _insert_text(sText);
}

/** inserts a new line in a javadoc documentation box. */
static void k_javadoc_box_line(_str sStr = '', int iPadd = 0, _str sStr2 = '', int iPadd2 = 0, _str sStr3 = '')
{
    if (!k_commentconfig(sLeft, sRight, iColumn))
        return;
    if (iColumn >= 0)
        while (p_col < iColumn)
           _insert_text(" ");

    if (k_line_comment())
        sText = sLeft;
    else
    {
        sText = sLeft;
        sText = ' ':+substr(sLeft, length(sLeft));
    }

    if (sStr != '')
        sText = sText:+' ':+sStr;
    if (iPadd > 0)
    {
        for (i = length(sText); i < iPadd; i++)
            sText = sText:+' ';

        if (sStr2 != '')
            sText = sText:+sStr2;

        if (iPadd2 > 0)
        {
            for (i = length(sText); i < iPadd2; i++)
                sText = sText:+' ';

            if (sStr3 != '')
                sText = sText:+sStr3;
        }
    }
    sText = sText:+"\n";

    _insert_text(sText);
}

/** ends a javadoc documentation box. */
static void k_javadoc_box_end()
{
    if (!k_commentconfig(sLeft, sRight, iColumn))
        return;
    if (iColumn >= 0)
        while (p_col < iColumn)
           _insert_text(" ");

    if (k_line_comment())
        sText = sLeft;
    else
    {
        sText = sRight;
        /*if (substr(sText, 1, 1) != '*')
            sText = '*':+sText;*/
        sText = ' ':+sText;
    }
    sText = sText:+"\n";

    _insert_text(sText);
}


/**
 * Write a Javadoc styled classbox.
 */
void k_javadoc_classbox()
{
    int     iCursorLine;
    int     iPadd = k_alignup(12, p_SyntaxIndent);

    k_javadoc_box_start();
    iCursorLine = p_RLine;
    k_javadoc_box_line(' ');

    if (fkStyleFullHeaders)
    {
        k_javadoc_box_line('@shortdesc', iPadd);
        k_javadoc_box_line('@dstruct', iPadd);
        k_javadoc_box_line('@version', iPadd);
        k_javadoc_box_line('@verdesc', iPadd);
    }
    k_javadoc_box_line('@author', iPadd, skUserName ' <' skUserEmail '>');
    k_javadoc_box_line('@approval', iPadd);
    k_javadoc_box_end();

    up(p_RLine - iCursorLine);
    end_line();
    keyin(' ');
}


/**
 * Javadoc - functionbox(/header).
 */
void k_javadoc_funcbox()
{
    int     cArgs = 1;
    _str    sArgs = "";
    int     iCursorLine;
    int     iPadd = k_alignup(11, p_SyntaxIndent);
    /* look for parameters */
    boolean fFoundFn = !k_func_goto_nearest_function();
    if (fFoundFn)
    {
        sArgs = k_func_getparams();
        cArgs = k_func_countparams(sArgs);
    }

    k_javadoc_box_start();
    iCursorLine = p_RLine;
    k_javadoc_box_line(' ');
    if (file_eq(p_extension, 'asm'))
        k_javadoc_box_line('@cproto', iPadd);
    k_javadoc_box_line('@returns', iPadd);
    if (fFoundFn)
    {
        /*
         * Determin parameter description indent.
         */
        int     iPadd2 = 0;
        for (i = 0; i < cArgs; i++)
        {
            _str sName, sType, sDefault;
            if (!k_func_enumparams(sArgs, i, sType, sName, sDefault)
                && iPadd2 < length(sName))
                iPadd2 = length(sName);
        }
        iPadd2 = k_alignup((iPadd + iPadd2), p_SyntaxIndent);

        /*
         * Insert parameter.
         */
        for (i = 0; i < cArgs; i++)
        {
            _str sName, sType, sDefault;
            if (!k_func_enumparams(sArgs, i, sType, sName, sDefault))
            {
                sStr3 = '';
                if (sDefault != "")
                    sStr3 = '(default='sDefault')';
                k_javadoc_box_line('@param', iPadd, sName, iPadd2, sStr3);
            }
            else
                k_javadoc_box_line('@param', iPadd);
        }
    }
    else
        k_javadoc_box_line('@param', iPadd);

    if (file_eq(p_extension, 'asm'))
        k_javadoc_box_line('@uses', iPadd);
    if (fkStyleFullHeaders)
    {
        k_javadoc_box_line('@equiv', iPadd);
        k_javadoc_box_line('@time', iPadd);
    k_javadoc_box_line('@sketch', iPadd);
    }
    k_javadoc_box_line('@status', iPadd);
    k_javadoc_box_line('@author', iPadd, skUserName '<' skUserEmail '>');
    if (fkStyleFullHeaders)
    k_javadoc_box_line('@remark', iPadd);
    k_javadoc_box_end();

    up(p_RLine - iCursorLine);
    end_line();
    keyin(' ');
}


/**
 * Javadoc module header.
 */
void k_javadoc_moduleheader()
{
    int iCursorLine;

    _begin_line();
    k_javadoc_box_start('$':+'I':+'d: $', false);
    k_javadoc_box_line();
    iCursorLine = p_RLine;
    k_javadoc_box_line();
    k_javadoc_box_line();

    if (skLicense == 'Confidential')
    {
        k_javadoc_box_line(skCompany ' confidential');
        k_javadoc_box_line();
    }

    if (skCompany != '')
    {
        k_javadoc_box_line('Copyright (c) ' k_year() ' ' skCompany);
        k_javadoc_box_line('Author: ' skUserName' <' skUserEmail '>');
    }
    else
        k_javadoc_box_line('Copyright (c) ' k_year() ' 'skUserName' <' skUserEmail '>');
    k_javadoc_box_line();
    switch (skLicense)
    {
        case 'Odin32':
            k_javadoc_box_line('Project Odin Software License can be found in LICENSE.TXT.');
            break;

        case 'GPL':
            sProg = skProgram;
            k_javadoc_box_line()
            if (sProg == '')
                sProg = 'This program';
            else
            {
                k_javadoc_box_line('This file is part of ' sProg '.');
                k_javadoc_box_line();
            }
            k_javadoc_box_line(sProg ' is free software; you can redistribute it and/or modify');
            k_javadoc_box_line('it under the terms of the GNU General Public License as published by');
            k_javadoc_box_line('the Free Software Foundation; either version 2 of the License, or');
            k_javadoc_box_line('(at your option) any later version.');
            k_javadoc_box_line()
            k_javadoc_box_line(sProg ' is distributed in the hope that it will be useful,');
            k_javadoc_box_line('but WITHOUT ANY WARRANTY; without even the implied warranty of');
            k_javadoc_box_line('MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the');
            k_javadoc_box_line('GNU General Public License for more details.');
            k_javadoc_box_line()
            k_javadoc_box_line('You should have received a copy of the GNU General Public License');
            k_javadoc_box_line('along with ' sProg '; if not, write to the Free Software');
            k_javadoc_box_line('Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA');
            break;

        case 'LGPL':
            sProg = skProgram;
            k_javadoc_box_line()
            if (sProg == '')
                sProg = 'This program';
            else
            {
                k_javadoc_box_line('This file is part of ' sProg '.');
                k_javadoc_box_line();
            }
            k_javadoc_box_line(sProg ' is free software; you can redistribute it and/or modify');
            k_javadoc_box_line('it under the terms of the GNU Lesser General Public License as published');
            k_javadoc_box_line('by the Free Software Foundation; either version 2 of the License, or');
            k_javadoc_box_line('(at your option) any later version.');
            k_javadoc_box_line()
            k_javadoc_box_line(sProg ' is distributed in the hope that it will be useful,');
            k_javadoc_box_line('but WITHOUT ANY WARRANTY; without even the implied warranty of');
            k_javadoc_box_line('MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the');
            k_javadoc_box_line('GNU Lesser General Public License for more details.');
            k_javadoc_box_line()
            k_javadoc_box_line('You should have received a copy of the GNU Lesser General Public License');
            k_javadoc_box_line('along with ' sProg '; if not, write to the Free Software');
            k_javadoc_box_line('Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA');
            break;

        case 'Confidential':
            k_javadoc_box_line('All Rights Reserved');
            break;

        default:

    }
    k_javadoc_box_line();
    k_javadoc_box_end();

    up(p_RLine - iCursorLine);
    end_line();
    keyin(' ');
}







/*******************************************************************************
*   Keyboard Shortcuts                                                         *
*******************************************************************************/
/** Makes global box. */
void k_box_globals()
{
    k_box_start('Global');
    k_box_line('Global Variables');
    k_box_end();
}

/** Makes header box. */
void k_box_headers()
{
    k_box_start("Header");
    k_box_line("Header Files");
    k_box_end();
}

/** Makes internal function box. */
void k_box_intfuncs()
{
    k_box_start("IntFunc");
    k_box_line("Internal Functions");
    k_box_end();
}

/** Makes def/const box. */
void k_box_consts()
{
    k_box_start("Const");
    k_box_line("Defined Constants And Macros");
    k_box_end();
}

/** Structure box */
void k_box_structs()
{
    k_box_start("Struct");
    k_box_line("Structures and Typedefs");
    k_box_end();
}

/** Makes exported symbols box. */
void k_box_exported()
{
    k_box_start('Exported');
    k_box_line('Exported Symbols');
    k_box_end();
}



/** oneliner comment */
void k_oneliner()
{
    if (    k_commentconfig(sLeft, sRight, iColumn)
        &&  iColumn > 0)
    {   /* column based needs some tricy repositioning. */
        _end_line();
        if (p_col > iColumn)
        {
            _begin_line();
            _insert_text("\n\r");
            up();
        }
    }
    k_insert_comment("", KIC_CURSOR_AT_END, ikStyleOneliner);
}

/** mark line as modified. */
void k_mark_modified_line()
{
    /* not supported for column based sources */
    if (    !k_commentconfig(sLeft, sRight, iColumn)
        ||  iColumn > 0)
        return;

    if (skChange != '')
        sStr = skChange ' (' skUserInitials ')';
    else
        sStr = skUserInitials;
    k_insert_comment(sStr, KIC_CURSOR_BEFORE, ikStyleModifyMarkColumn);
    down();
}


/**
 * Inserts a signature. Form: "//Initials ISO-date:"
 * @remark    defeventtab
 */
void k_signature()
{
    /* kso I5-10000 2002-09-10: */
    if (skChange != '')
        sSig = skUserInitials ' ' skChange ' ' k_date() ': ';
    else
        sSig = skUserInitials ' ' k_date() ': ';
    k_insert_comment(sSig, KIC_CURSOR_AT_END);
}


/*******************************************************************************
*   kLIB Logging                                                               *
*******************************************************************************/
/**
 * Hot-Key: Inserts a KLOGENTRY statement at start of nearest function.
 */
void klib_klogentry()
{
    _save_pos2(org_pos);

    /*
     * Go to nearest function.
     */
    if (!k_func_goto_nearest_function())
    {
        /*
         * Get parameters.
         */
        _str sParams = k_func_getparams();
        if (sParams)
        {
            _str sRetType = k_func_getreturntype(true);
            if (!sRetType || sRetType == "")
                sRetType = "void";      /* paranoia! */

            /*
             * Insert text.
             */
            if (!k_func_searchcode("{"))
            {
                p_col++;
                cArgs = k_func_countparams(sParams);
                if (cArgs > 0)
                {
                    sArgs = "";
                    for (i = 0; i < cArgs; i++)
                    {
                        _str sType, sName, sDefault
                        if (!k_func_enumparams(sParams, i, sType, sName, sDefault))
                            sArgs = sArgs', 'sName;
                    }

                    _insert_text("\n    KLOGENTRY"cArgs"(\""sRetType"\",\""sParams"\""sArgs");"); /* todo tab size.. or smart indent */
                }
                else
                    _insert_text("\n    KLOGENTRY0(\""sRetType"\");"); /* todo tab size.. or smart indent */

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
            message("k_func_getparams failed, sParams=" sParams);
        return;
    }

    _restore_pos2(org_pos);
}


/**
 * Hot-Key: Inserts a KLOGEXIT statement at cursor location.
 */
void klib_klogexit()
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
        _str sType = k_func_getreturntype(true);
        _restore_pos2(org_pos);
        if (sType)
        {
            boolean fReturn = true;     /* true if an return statment is following the KLOGEXIT statement. */

            /*
             * Insert text.
             */
            cur_col = p_col;
            if (sType == 'void' || sType == 'VOID')
            {   /* procedure */
                fReturn = cur_word(iIgnorePos) == 'return';
                if (!fReturn)
                {
                    while (p_col <= p_SyntaxIndent)
                        keyin(" ");
                }

                _insert_text("KLOGEXITVOID();\n");

                if (fReturn)
                {
                    for (i = 1; i < cur_col; i++)
                        _insert_text(" ");
                }
                search(")","E-");
            }
            else
            {   /* function */
                _insert_text("KLOGEXIT();\n");
                for (i = 1; i < cur_col; i++)
                    _insert_text(" ");
                search(")","E-");

                /*
                 * Insert value if possible.
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
                    if (!k_func_searchcode(";", "E+"))
                    {
                        offEnd = _QROffset();
                        _restore_pos2(posStart);
                        _str sValue = strip(get_text(offEnd - offStart));
                        //say 'sValue = 'sValue;
                        _restore_pos2(valuepos);
                        _save_pos2(valuepos);
                        _insert_text(sValue);
                    }
                }
                _restore_pos2(valuepos);
            }

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

            /*
             * Check for missing '{...}'.
             */
            if (fReturn)
            {
                boolean fFound = false;
                _save_pos2(valuepos);
                p_col--; find_matching_paren(); p_col += 2;
                k_func_searchcode(';', 'E+'); /* places us at the ';' of the return. (hopefully) */

                _str ch = k_func_get_next_code_text();
                if (ch != '}')
                {
                    _restore_pos2(valuepos);
                    _save_pos2(valuepos);
                    p_col--; find_matching_paren(); p_col += 2;
                    k_func_searchcode(';', 'E+'); /* places us at the ';' of the return. (hopefully) */
                    p_col++;
                    if (k_func_more_code_on_line())
                        _insert_text(' }');
                    else
                    {
                        _save_pos2(returnget);
                        k_func_searchcode("return", "E-");
                        return_col = p_col;
                        _restore_pos2(returnget);

                        end_line();
                        _insert_text("\n");
                        while (p_col < return_col - p_SyntaxIndent)
                            _insert_text(' ');
                        _insert_text('}');
                    }

                    _restore_pos2(valuepos);
                    _save_pos2(valuepos);
                    prev_word();
                    p_col -= p_SyntaxIndent;
                    codecol = p_col;
                    _insert_text("{\n");
                    while (p_col < codecol)
                        _insert_text(' ');
                }

                _restore_pos2(valuepos);
            }
        }
        else
            message("k_func_getreturntype failed, sType=" sType);
        return;
    }

    _restore_pos2(org_pos);
}


/**
 * Processes a file - ask user all the time.
 */
void klib_klog_file_ask()
{
    klib_klog_file_int(true)
}


/**
 * Processes a file - no questions.
 */
void klib_klog_file_no_ask()
{
    klib_klog_file_int(false)
}



/**
 * Processes a file.
 */
static void klib_klog_file_int(boolean fAsk)
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
        //say 'entry main loop: ' k_func_getfunction_name();

        /*
         * Skip prototypes.
         */
        if (k_func_prototype())
            continue;

        /*
         * Ask user.
         */
        center_line();
        _refresh_scroll();
        sFunction = k_func_getfunction_name();
        rc = fAsk ? _message_box("Process this function ("sFunction")?", "Visual SlickEdit", MB_YESNOCANCEL | MB_ICONQUESTION) : IDYES;
        if (rc == IDYES)
        {
            _save_pos2(procpos);
            klib_klogentry();
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
        sCurFunction = k_func_getfunction_name();
        //say 'exit main loop: ' sCurFunction

        /*
         * Skip prototypes.
         */
        if (k_func_prototype())
            continue;

        /*
         * Select procedure.
         */
        while (   !k_func_searchcode("return", "WE<+")
               &&  k_func_getfunction_name() == sCurFunction)
        {
            //say 'exit sub loop: ' p_line
            /*
             * Ask User.
             */
            center_line();
            _refresh_scroll();
            sFunction = k_func_getfunction_name();
            rc =  fAsk ? _message_box("Process this exit from "sFunction"?", "Visual SlickEdit", MB_YESNOCANCEL | MB_ICONQUESTION) : IDYES;
            deselect();
            if (rc == IDYES)
            {
                _save_pos2(returnpos);
                klib_klogexit();
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
        sType = k_func_getreturntype(true);
        if (!fUserCancel && sType && (sType == 'void' || sType == 'VOID'))
        {
            if (    !k_func_searchcode("{", "E+")
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
                    sFunction = k_func_getfunction_name();
                    rc = fAsk ? _message_box("Process this exit from "sFunction"?", "Visual SlickEdit", MB_YESNOCANCEL | MB_ICONQUESTION) : IDYES;
                    deselect();
                    if (rc == IDYES)
                    {
                        _save_pos2(returnpos);
                        klib_klogexit();
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


/*******************************************************************************
*   Odin32 backward compatibility                                              *
*******************************************************************************/
_command void odin32_maketagfile()
{
    /* We'll */
    if (file_match('-p 'maybe_quote_filename(strip_filename(_project_name,'e'):+TAG_FILE_EXT),1) != "")
    {
        _project_update_files_retag(false,false,false,false);
        /*
        RetagFilesInTagFile2(project_tag_file, orig_view_id, temp_view_id, rebuild_all, false,
                             doRemove,false,true,true);*/7
    }
    else
        _project_update_files_retag(true,false,false,true);
}

_command void odin32_setcurrentdir()
{
    //_ini_get_value(_project_name,"COMPILER","WORKINGDIR", workingdir);
    //cd(workingdir);
    /* Go the the directory containing the project filename */
    cd(strip_filename(_project_name, 'NE'));
}




/*******************************************************************************
*   Styles                                                                     *
*******************************************************************************/
static _str StyleLanguages[] =
{
    "c",
    "e",
    "java"
};

struct StyleScheme
{
    _str name;
    _str settings[];
};

static StyleScheme StyleSchemes[] =
{
    {
        "Opt2Ind4",
        {
           "orig_tabsize=4",
           "syntax_indent=4",
           "tabsize=4",
           "align_on_equal=1",
           "pad_condition_state=1",
           "indent_with_tabs=0",
           "nospace_before_paren=0",
           "indent_comments=1",
           "indent_case=1",
           "statement_comment_col=0",
           "disable_bestyle=0",
           "decl_comment_col=0",
           "bestyle_on_functions=0",
           "use_relative_indent=1",
           "nospace_before_brace=0",
           "indent_fl=1",
           "statement_comment_state=2",
           "indent_pp=1",
           "be_style=1",
           "parens_on_return=0",
           "eat_blank_lines=0",
           "brace_indent=0",
           "eat_pp_space=1",
           "align_on_parens=1",
           "continuation_indent=0",
           "cuddle_else=0",
           "nopad_condition=1",
           "pad_condition=0",
           "indent_col1_comments=0"
        }
    }
    ,
    {
        "Opt2Ind3",
        {
           "orig_tabsize=3",
           "syntax_indent=3",
           "tabsize=3",
           "align_on_equal=1",
           "pad_condition_state=1",
           "indent_with_tabs=0",
           "nospace_before_paren=0",
           "indent_comments=1",
           "indent_case=1",
           "statement_comment_col=0",
           "disable_bestyle=0",
           "decl_comment_col=0",
           "bestyle_on_functions=0",
           "use_relative_indent=1",
           "nospace_before_brace=0",
           "indent_fl=1",
           "statement_comment_state=2",
           "indent_pp=1",
           "be_style=1",
           "parens_on_return=0",
           "eat_blank_lines=0",
           "brace_indent=0",
           "eat_pp_space=1",
           "align_on_parens=1",
           "continuation_indent=0",
           "cuddle_else=0",
           "nopad_condition=1",
           "pad_condition=0",
           "indent_col1_comments=0"
        }
    }
    ,
    {
        "Opt2Ind8",
        {
           "orig_tabsize=8",
           "syntax_indent=8",
           "tabsize=8",
           "align_on_equal=1",
           "pad_condition_state=1",
           "indent_with_tabs=0",
           "nospace_before_paren=0",
           "indent_comments=1",
           "indent_case=1",
           "statement_comment_col=0",
           "disable_bestyle=0",
           "decl_comment_col=0",
           "bestyle_on_functions=0",
           "use_relative_indent=1",
           "nospace_before_brace=0",
           "indent_fl=1",
           "statement_comment_state=2",
           "indent_pp=1",
           "be_style=1",
           "parens_on_return=0",
           "eat_blank_lines=0",
           "brace_indent=0",
           "eat_pp_space=1",
           "align_on_parens=1",
           "continuation_indent=0",
           "cuddle_else=0",
           "nopad_condition=1",
           "pad_condition=0",
           "indent_col1_comments=0"
        }
    }
    ,
    {
        "Opt3Ind4",
        {
           "orig_tabsize=4",
           "syntax_indent=4",
           "tabsize=4",
           "align_on_equal=1",
           "pad_condition_state=1",
           "indent_with_tabs=0",
           "nospace_before_paren=0",
           "indent_comments=1",
           "indent_case=1",
           "statement_comment_col=0",
           "disable_bestyle=0",
           "decl_comment_col=0",
           "bestyle_on_functions=0",
           "use_relative_indent=1",
           "nospace_before_brace=0",
           "indent_fl=1",
           "statement_comment_state=2",
           "indent_pp=1",
           "be_style=2",
           "parens_on_return=0",
           "eat_blank_lines=0",
           "brace_indent=0",
           "eat_pp_space=1",
           "align_on_parens=1",
           "continuation_indent=0",
           "cuddle_else=0",
           "nopad_condition=1",
           "pad_condition=0",
           "indent_col1_comments=0"
        }
    }
    ,
    {
        "Opt3Ind3",
        {
            "orig_tabsize=3",
            "syntax_indent=3",
            "tabsize=3",
            "align_on_equal=1",
            "pad_condition_state=1",
            "indent_with_tabs=0",
            "nospace_before_paren=0",
            "indent_comments=1",
            "indent_case=1",
            "statement_comment_col=0",
            "disable_bestyle=0",
            "decl_comment_col=0",
            "bestyle_on_functions=0",
            "use_relative_indent=1",
            "nospace_before_brace=0",
            "indent_fl=1",
            "statement_comment_state=2",
            "indent_pp=1",
            "be_style=2",
            "parens_on_return=0",
            "eat_blank_lines=0",
            "brace_indent=0",
            "eat_pp_space=1",
            "align_on_parens=1",
            "continuation_indent=0",
            "cuddle_else=0",
            "nopad_condition=1",
            "pad_condition=0",
            "indent_col1_comments=0"
        }
    }
};


static void k_styles_create()
{
    /*
     * Find user format ini file.
     */
    userini = maybe_quote_filename(_config_path():+'uformat.ini');
    if (file_match('-p 'userini, 1) == '')
    {
        ini = maybe_quote_filename(slick_path_search('uformat.ini'));
        if (ini != '') userini = ini;
    }


    /*
     * Remove any old schemes.
     */
    for (i = 0; i < StyleSchemes._length(); i++)
        for (j = 0; j < StyleLanguages._length(); j++)
        {
            sectionname = StyleLanguages[j]:+'-scheme-':+StyleSchemes[i].name;
            if (!_ini_get_section(userini, sectionname, tv))
            {
                _ini_delete_section(userini, sectionname);
                _delete_temp_view(tv);
                //message("delete old scheme");
            }
        }

    /*
     * Create the new schemes.
     */
    for (i = 0; i < StyleSchemes._length(); i++)
    {
        for (j = 0; j < StyleLanguages._length(); j++)
        {
            sectionname = StyleLanguages[j]:+'-scheme-':+StyleSchemes[i].name;
            orig_view_id = _create_temp_view(temp_view_id);
            activate_view(temp_view_id);
            for (k = 0; k < StyleSchemes[i].settings._length(); k++)
                insert_line(StyleSchemes[i].settings[k]);

            /* Insert the scheme section. */
            _ini_replace_section(userini, sectionname, temp_view_id);
            //message(userini)
            activate_view(orig_view_id);
        }
    }

    //last_scheme = last scheme name!!!
}


/*
 * Sets the last used beutify scheme.
 */
static k_styles_set(_str scheme)
{

    /*
     * Find user format ini file.
     */
    userini = maybe_quote_filename(_config_path():+'uformat.ini');
    if (file_match('-p 'userini, 1) == '')
    {
        ini = maybe_quote_filename(slick_path_search('uformat.ini'));
        if (ini != '') userini = ini;
    }

    /*
     * Set the scheme for each language.
     */
    for (j = 0; j < StyleLanguages._length(); j++)
    {
        _ini_set_value(userini,
                       StyleLanguages[j]:+'-scheme-Default',
                       'last_scheme',
                       scheme);
    }
}


static _str defoptions[] =
{
    "def-options-sas",
    "def-options-js",
    "def-options-bat",
    "def-options-c",
    "def-options-pas",
    "def-options-e",
    "def-options-java",
    "def-options-bourneshell",
    "def-options-csh",
    "def-options-vlx",
    "def-options-plsql",
    "def-options-sqlserver",
    "def-options-cmd"
};

static _str defsetups[] =
{
    "def-setup-sas",
    "def-setup-js",
    "def-setup-bat",
    "def-setup-fundamental",
    "def-setup-process",
    "def-setup-c",
    "def-setup-pas",
    "def-setup-e",
    "def-setup-asm",
    "def-setup-java",
    "def-setup-html",
    "def-setup-bourneshell",
    "def-setup-csh",
    "def-setup-vlx",
    "def-setup-fileman",
    "def-setup-plsql",
    "def-setup-sqlserver",
    "def-setup-s",
    "def-setup-cmd"
};

static _str defsetupstab8[] =
{
    "def-setup-c"
};


static void k_styles_setindent(int indent, int iBraceStyle, boolean iWithTabs = false)
{
    if (iBraceStyle < 1 || iBraceStyle > 3)
        {
        say 'k_styles_setindent: iBraceStyle is bad (='iBraceStyle')';
        iBraceStyle = 2;
        }
    /*
     * def-options for extentions known to have that info.
     */
    for (i = 0; i < defoptions._length(); i++)
    {
        idx = find_index(defoptions[i], MISC_TYPE);
        if (!idx)
            continue;

        parse name_info(idx) with syntax_indent o2 o3 o4 flags indent_fl o7 indent_case rest;

        /* Begin/end style */
        flags = flags & ~(1|2);
        flags = flags | (iBraceStyle - 1); /* Set style (0-based) */
        flags = flags & ~(16); /* no scape before parent.*/
        indent_fl = 1;         /* Indent first level */
        indent_case = 1;       /* Indent case from switch */

        sNewOptions = indent' 'o2' 'o3' 'o4' 'flags' 'indent_fl' 'o7' 'indent_case' 'rest;
        set_name_info(idx, sNewOptions);
        _config_modify |= CFGMODIFY_DEFDATA;
    }

    /*
     * def-setup for known extentions.
     */
    for (i = 0; i < defsetups._length(); i++)
    {
        idx = find_index(defsetups[i], MISC_TYPE);
        if (!idx)
           continue;
        sExt = substr(defsetups[i], length('def-setup-') + 1);
        sSetup = name_info(idx);

        /*
        parse sSetup with 'MN=' mode_name ','\
          'TABS=' tabs ',' 'MA=' margins ',' 'KEYTAB=' keytab_name ','\
          'WW='word_wrap_style ',' 'IWT='indent_with_tabs ','\
          'ST='show_tabs ',' 'IN='indent_style ','\
          'WC='word_chars',' 'LN='lexer_name',' 'CF='color_flags','\
          'LNL='line_numbers_len','rest;

        indent_with_tabs = 0; /* Indent with tabs */

        /* Make sure all the values are legal */
        _ext_init_values(ext, lexer_name, color_flags);
        if (!isinteger(line_numbers_len))   line_numbers_len = 0;
        if (word_chars == '')               word_chars       = 'A-Za-z0-9_$';
        if (word_wrap_style == '')          word_wrap_style  = 3;
        if (show_tabs == '')                show_tabs        = 0;
        if (indent_style == '')             indent_style     = INDENT_SMART;

        /* Set new indent */
        tabs = '+'indent;
        */

        sNewSetup = sSetup;

        /* Set new indent */
        if (pos('TABS=', sNewSetup) > 0)
        {
            /*
             * If either in defoptions or defsetupstab8 use default tab of 8
             * For those supporting separate syntax indent using the normal tabsize
             * helps us a lot when reading it...
             */
            fTab8 = false;
            for (j = 0; !fTab8 && j < defsetupstab8._length(); j++)
                if (substr(defsetupstab8[j], lastpos('-', defsetupstab8[j]) + 1) == sExt)
                    fTab8 = true;
            for (j = 0; !fTab8 && j < defoptions._length(); j++)
                if (substr(defoptions[j], lastpos('-', defoptions[j]) + 1) == sExt)
                    fTab8 = true;

            parse sNewSetup with sPre 'TABS=' sValue ',' sPost;
            if (fTab8)
                sNewSetup = sPre 'TABS=+8,' sPost
            else
                sNewSetup = sPre 'TABS=+' indent ',' sPost
        }

        /* Set indent with tabs flag. */
        if (pos('IWT=', sNewSetup) > 0)
        {
            parse sNewSetup with sPre 'IWT=' sValue ',' sPost;
            if (iWithTabs)
                sNewSetup = sPre 'IWT=1,' sPost
            else
                sNewSetup = sPre 'IWT=0,' sPost
        }

        /* Do the real changes */
        set_name_info(idx, sNewSetup);
        _config_modify |= CFGMODIFY_DEFDATA;
        _update_buffers(sExt);
    }
}


/*******************************************************************************
*   Menu and Menu commands                                                     *
*******************************************************************************/
static int  iTimer = 0;
static int  mhExtra = 0;
static int  mhCode = 0;
static int  mhDoc = 0;
static int  mhLic = 0;
static int  mhPre = 0;

/*
 * Creates the Extra menu.
 */
static k_menu_create()
{
    if (arg(1) == 'timer')
        _kill_timer(iTimer);
    menu_handle = _mdi.p_menu_handle;
    menu_index  = find_index(_cur_mdi_menu,oi2type(OI_MENU));

    /*
     * Remove any old menu.
     */
    mhDelete = iPos = 0;
    index = _menu_find(menu_handle, "Extra", mhDelete, iPos, 'C');
    //message("index="index " mhDelete="mhDelete " iPos="iPos);
    if (index == 0)
        _menu_delete(mhDelete, iPos);


    /*
     * Insert the "Extra" menu.
     */
    mhExtra = _menu_insert(menu_handle, 9, MF_SUBMENU, "E&xtra", "", "Extra");
    mhCode=_menu_insert(mhExtra, -1, MF_ENABLED | MF_SUBMENU,   "Coding &Style",  "", "coding");
    rc   = _menu_insert(mhCode,  -1, MF_ENABLED | MF_UNCHECKED, "Braces 2, Syntax Indent 4 (knut)",    "k_menu_style Opt2Ind4",    "Opt2Ind4");
    rc   = _menu_insert(mhCode,  -1, MF_ENABLED | MF_UNCHECKED, "Braces 2, Syntax Indent 3",           "k_menu_style Opt2Ind3",    "Opt2Ind3");
    rc   = _menu_insert(mhCode,  -1, MF_ENABLED | MF_UNCHECKED, "Braces 2, Syntax Indent 8",           "k_menu_style Opt2Ind8",    "Opt2Ind8");
    rc   = _menu_insert(mhCode,  -1, MF_ENABLED | MF_UNCHECKED, "Braces 3, Syntax Indent 4 (giws)",    "k_menu_style Opt3Ind4",    "Opt3Ind4");
    rc   = _menu_insert(mhCode,  -1, MF_ENABLED | MF_UNCHECKED, "Braces 3, Syntax Indent 3 (giws)",    "k_menu_style Opt3Ind3",    "Opt3Ind3");

    mhDoc= _menu_insert(mhExtra, -1, MF_ENABLED | MF_SUBMENU,   "&Documentation",       "",                             "doc");
    mhDSJ= _menu_insert(mhDoc,   -1, MF_ENABLED | MF_UNCHECKED, "&Javadoc Style",       "k_menu_doc_style javadoc",     "javadoc");
    mhDSL= _menu_insert(mhDoc,   -1, MF_GRAYED  | MF_UNCHECKED, "&Linux Kernel Style",  "k_menu_doc_style linux",       "linux");

    mhLic= _menu_insert(mhExtra, -1, MF_ENABLED | MF_SUBMENU,   "&License",             "",                             "License");
    rc   = _menu_insert(mhLic,   -1, MF_ENABLED | MF_UNCHECKED, "&Odin32",              "k_menu_license Odin32",        "Odin32");
    rc   = _menu_insert(mhLic,   -1, MF_ENABLED | MF_UNCHECKED, "&GPL",                 "k_menu_license GPL",           "GPL");
    rc   = _menu_insert(mhLic,   -1, MF_ENABLED | MF_UNCHECKED, "&LGPL",                "k_menu_license LGPL",          "LGPL");
    rc   = _menu_insert(mhLic,   -1, MF_ENABLED | MF_UNCHECKED, "&Confidential",        "k_menu_license Confidential",  "Confidential");

    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, "-", "", "dash vars");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, skChange  == '' ? '&Change...'  : '&Change (' skChange ')...',   "k_menu_change", "");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, skProgram == '' ? '&Program...' : '&Program (' skProgram ')...', "k_menu_program", "");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, skCompany == '' ? 'Co&mpany...' : 'Co&mpany (' skCompany ')...', "k_menu_company", "");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, '&User Name (' skUserName ')...',          "k_menu_user_name",     "username");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, 'User &e-mail (' skUserEmail ')...',       "k_menu_user_email",    "useremail");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, 'User &Initials (' skUserInitials ')...',  "k_menu_user_initials", "userinitials");
    rc   = _menu_insert(mhExtra, -1, MF_ENABLED, "-", "", "dash preset");
    mhPre= _menu_insert(mhExtra, -1, MF_SUBMENU, "P&resets", "", "");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "Odin32",      "k_menu_preset javadoc, Odin32, Opt2Ind4,,Odin32",      "odin32");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "Linux Kernel","k_menu_preset linux, GPL, Opt1Ind4,,Linux",            "linux");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "The Bird",    "k_menu_preset javadoc, GPL, Opt2Ind4",                 "bird");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "Win32k",      "k_menu_preset javadoc, GPL, Opt2Ind4,, Win32k",        "Win32k");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "kKrnlLib",    "k_menu_preset javadoc, GPL, Opt2Ind4,, kKrnlLib",      "kKrnlLib");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "kLib",        "k_menu_preset javadoc, GPL, Opt2Ind4,, kLib",          "kLib");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "kBuild",      "k_menu_preset javadoc, GPL, Opt2Ind4,, kBuild",        "kBuild");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "Innotek",     "k_menu_preset javadoc, Confidential, Opt2Ind4, InnoTek Systemberatung GmbH",           "Innotek");
    rc   = _menu_insert(mhPre,   -1, MF_ENABLED, "VPC/2",       "k_menu_preset javadoc, Confidential, Opt2Ind4, InnoTek Systemberatung GmbH, VPC/2",    "VPC2");

    k_menu_doc_style();
    k_menu_license();
    k_menu_style();
}


/**
 * Change change Id.
 */
_command k_menu_change()
{
    sRc = show("-modal k_form_simple_input", "Change ID", skChange);
    if (sRc != "\r")
    {
        skChange = sRc;
        k_menu_create();
    }
}


/**
 * Change program name.
 */
_command k_menu_program()
{
    sRc = show("-modal k_form_simple_input", "Program", skProgram);
    if (sRc != "\r")
    {
        skProgram = sRc;
        k_menu_create();
    }
}


/**
 * Change company.
 */
_command k_menu_company()
{
    if (skCompany == '')
        sRc = show("-modal k_form_simple_input", "Company", 'InnoTek Systemberatung GmbH');
    else
        sRc = show("-modal k_form_simple_input", "Company", skCompany);
    if (sRc != "\r")
    {
        skCompany = sRc;
        k_menu_create();
    }
}


/**
 * Change user name.
 */
_command k_menu_user_name()
{
    sRc = show("-modal k_form_simple_input", "User Name", skUserName);
    if (sRc != "\r" && sRc != '')
    {
        skUserName = sRc;
        k_menu_create();
    }
}


/**
 * Change user email.
 */
_command k_menu_user_email()
{
    sRc = show("-modal k_form_simple_input", "User e-mail", skUserEmail);
    if (sRc != "\r" && sRc != '')
    {
        skUserEmail = sRc;
        k_menu_create();
    }
}


/**
 * Change user initials.
 */
_command k_menu_user_initials()
{
    sRc = show("-modal k_form_simple_input", "User e-mail", skUserInitials);
    if (sRc != "\r" && sRc != '')
    {
        skUserInitials = sRc;
        k_menu_create();
    }
}



/**
 * Checks the correct menu item.
 */
_command void k_menu_doc_style(_str sNewDocStyle = '')
{
    //say 'sNewDocStyle='sNewDocStyle;
    if (sNewDocStyle != '')
        skDocStyle = sNewDocStyle
    _menu_set_state(mhDoc, "javadoc",   MF_UNCHECKED);
    _menu_set_state(mhDoc, "linux",     MF_UNCHECKED | MF_GRAYED);
    _menu_set_state(mhDoc, skDocStyle,  MF_CHECKED);
}


/**
 * Checks the correct menu item.
 */
_command void k_menu_license(_str sNewLicense = '')
{
    //say 'sNewLicense='sNewLicense;
    if (sNewLicense != '')
        skLicense = sNewLicense
    _menu_set_state(mhLic, "Odin32",        MF_UNCHECKED);
    _menu_set_state(mhLic, "GPL",           MF_UNCHECKED);
    _menu_set_state(mhLic, "LGPL",          MF_UNCHECKED);
    _menu_set_state(mhLic, "Confidential",  MF_UNCHECKED);
    _menu_set_state(mhLic, skLicense,       MF_CHECKED);
}


/**
 * Check the correct style menu item.
 */
_command void k_menu_style(_str sNewStyle = '')
{
    //say 'sNewStyle='sNewStyle;
    _menu_set_state(mhCode, "Opt1Ind4", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt1Ind3", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt1Ind8", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt2Ind4", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt2Ind3", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt2Ind8", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt3Ind4", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt3Ind3", MF_UNCHECKED);
    _menu_set_state(mhCode, "Opt3Ind8", MF_UNCHECKED);

    if (sNewStyle != '')
    {
        int iIndent = (int)substr(sNewStyle, 8, 1);
        int iBraceStyle = (int)substr(sNewStyle, 4, 1);
        skCodeStyle = sNewStyle;
        k_styles_setindent(iIndent, iBraceStyle);
        k_styles_set(sNewStyle);
    }

    _menu_set_state(mhCode, skCodeStyle, MF_CHECKED);
}


/**
 * Load a 'preset'.
 */
_command void k_menu_preset(_str sArgs = '')
{
    parse sArgs with sNewDocStyle ',' sNewLicense ',' sNewStyle ',' sNewCompany ',' sNewProgram ',' sNewChange
    sNewDocStyle= strip(sNewDocStyle);
    sNewLicense = strip(sNewLicense);
    sNewStyle   = strip(sNewStyle);
    sNewCompany = strip(sNewCompany);
    sNewProgram = strip(sNewProgram);
    sNewChange  = strip(sNewChange);

    //say 'k_menu_preset('sNewDocStyle',' sNewLicense',' sNewStyle',' sNewCompany',' sNewProgram')';
    k_menu_doc_style(sNewDocStyle);
    k_menu_license(sNewLicense);
    k_menu_style(sNewStyle);
    skCompany = sNewCompany;
    skProgram = sNewProgram;
    skChange = sNewChange;
    k_menu_create();
}



/* future ones..
_command k_menu_setcolor()
{
    createMyColorSchemeAndUseIt();
}


_command k_menu_setkeys()
{
    rc = load("d:/knut/VSlickMacros/BoxerDef.e");
}

_command k_menu_settings()
{
    mySettings();
}
*/


/*******************************************************************************
*   Dialogs                                                                    *
*******************************************************************************/
_form k_form_simple_input {
   p_backcolor=0x80000005
   p_border_style=BDS_DIALOG_BOX
   p_caption='Simple Input'
   p_clip_controls=FALSE
   p_forecolor=0x80000008
   p_height=1120
   p_width=5020
   p_x=6660
   p_y=6680
   _text_box entText {
      p_auto_size=TRUE
      p_backcolor=0x80000005
      p_border_style=BDS_FIXED_SINGLE
      p_completion=NONE_ARG
      p_font_bold=FALSE
      p_font_italic=FALSE
      p_font_name='MS Sans Serif'
      p_font_size=8
      p_font_underline=FALSE
      p_forecolor=0x80000008
      p_height=270
      p_tab_index=1
      p_tab_stop=TRUE
      p_text='text'
      p_width=3180
      p_x=1680
      p_y=240
      p_eventtab2=_ul2_textbox
   }
   _label lblLabel {
      p_alignment=AL_VCENTERRIGHT
      p_auto_size=FALSE
      p_backcolor=0x80000005
      p_border_style=BDS_NONE
      p_caption='Label'
      p_font_bold=FALSE
      p_font_italic=FALSE
      p_font_name='MS Sans Serif'
      p_font_size=8
      p_font_underline=FALSE
      p_forecolor=0x80000008
      p_height=240
      p_tab_index=2
      p_width=1380
      p_word_wrap=FALSE
      p_x=180
      p_y=240
   }
   _command_button btnOK {
      p_cancel=FALSE
      p_caption='&OK'
      p_default=TRUE
      p_font_bold=FALSE
      p_font_italic=FALSE
      p_font_name='MS Sans Serif'
      p_font_size=8
      p_font_underline=FALSE
      p_height=360
      p_tab_index=3
      p_tab_stop=TRUE
      p_width=1020
      p_x=180
      p_y=660
   }
   _command_button btnCancel {
      p_cancel=TRUE
      p_caption='Cancel'
      p_default=FALSE
      p_font_bold=FALSE
      p_font_italic=FALSE
      p_font_name='MS Sans Serif'
      p_font_size=8
      p_font_underline=FALSE
      p_height=360
      p_tab_index=4
      p_tab_stop=TRUE
      p_width=840
      p_x=1380
      p_y=660
   }
}

defeventtab k_form_simple_input
btnOK.on_create(_str sLabel = '', _str sText = '')
{
    p_active_form.p_caption = sLabel;
    lblLabel.p_caption = sLabel;
    entText.p_text = sText;
}

btnOK.lbutton_up()
{
    sText = entText.p_text;
    p_active_form._delete_window(sText);
}
btnCancel.lbutton_up()
{
    sText = entText.p_text;
    p_active_form._delete_window("\r");
}



/**
 * Module initiation.
 */
definit()
{
   k_styles_create();
   k_menu_create();
   iTimer = _set_timer(1000, k_menu_create, "timer");
   /* createMyColorSchemeAndUseIt();*/
}


