<?php

/**
 * Compute completion percentage for a dll.
 * @returns     Completion percentage. Range 0-100.
 *              On error -1 or -2 is returned.
 * @param       $iDll  Dll reference code.
 * @param       $db     Database connection variable.
 * @sketch      Get total number of function in the dll.
 *              Get number of completed functions in the dll.
 *              return complete*100 / total
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBComputeCompletion($iDll, $db)
{
    /*
     * Count the total number of functions in the DLL.
     */
    $sql = sprintf("SELECT  SUM(s.weight)/COUNT(f.state)
                        FROM
                            function f,
                            state s
                        WHERE
                            f.state = s.refcode
                            AND dll = %d",
                        $iDll);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) < 1)
    {
        echo "<br>Odin32DBComputeCompletion: IPE no. 1 <br>";
        return -1;
    }
    $row = mysql_fetch_row($result);
    $iComplete = $row[0];
    mysql_free_result($result);
}


/**
 * Draws a completion bar for a Function.
 * @param       $iFunction      Function reference code.
 * @param       $iFunctionName  Function name.
 * @param       $db             Database connection variable.
 * @sketch      Call Odin32DBCompletionBar2 with an appropriate condition.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBarFunction($iFunction, $sFunctionName, $db)
{
    return Odin32DBcompletionBar2("refcode = ".$iFunction, $sFunctionName, $db);
}


/**
 * Draws a completion bar for a dll (or all dlls).
 * @param       $iDll       Dll reference code.
 *                          If < 0 then for the entire project.
 * @param       $iDllName   Dll name.
 * @param       $db         Database connection variable.
 * @sketch      Call Odin32DBCompletionBar2 with an appropriate condition.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBarDll($iDll, $sDllName, $db)
{
    if ($iDll < 0)
        return Odin32DBcompletionBar2("", $sDllName, $db);
    return Odin32DBcompletionBar2("dll = ".$iDll, $sDllName, $db);
}

/**
 * Draws a completion bar for a File.
 * @param       $iFile          File reference code.
 * @param       $iFileName      File name.
 * @param       $db             Database connection variable.
 * @sketch      Call Odin32DBCompletionBar2 with an appropriate condition.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBarFile($iFile, $sFileName, $db)
{
    return Odin32DBcompletionBar2("file = ".$iFile, $sFileName, $db);
}

/**
 * Draws a completion bar for an API Group.
 * @param       $iAPIGroup      API Group reference code.
 * @param       $iAPIGroupName  API Group name.
 * @param       $db             Database connection variable.
 * @sketch      Call Odin32DBCompletionBar2 with an appropriate condition.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBarAPIGroup($iAPIGroup, $sAPIGroupName, $db)
{
    return Odin32DBcompletionBar2("apigroup = ".$iAPIGroup, $sAPIGroupName, $db);
}

/**
 * Draws a completion bar for an Author.
 * @param       $iAuthor        Author reference code.
 * @param       $iAuthorName    Author name.
 * @param       $db             Database connection variable.
 * @sketch      Call Odin32DBCompletionBar2 with an appropriate condition.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBarAuthor($iAuthor, $sAuthorName, $db)
{
    /*
     * Count the total number of functions in the DLL.
     */
    $sql = "SELECT COUNT(*) FROM fnauthor fa JOIN function f\n".
           "WHERE fa.function = f.refcode AND fa.author = ".$iAuthor;
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) < 1)
    {
        printf("\n\n<br>Odin32DBCompletionBar2: IPE(1).<br>\n\n");
        return -1;
    }
    $row = mysql_fetch_row($result);
    $cFunctions = $row[0];
    mysql_free_result($result);


    /*
     * Make
     */
    echo "
        <table width=100% border=0 cellspacing=0 cellpadding=0>
        ";
    if ($sAuthorName != '')
        echo "
        <tr>
            <td width=90%>
                <font size=-1 color=000099>
                <tt>".$sAuthorName."</tt>
                </font>
            </td>
            <td width=10%></td>
        </tr>";
    echo "
        <tr>
            <td width=90%>
                <table width=100% border=0 cellspacing=0 cellpadding=0>
                <tr>
        ";


    /*
     * Get states and make bar.
     */
    $sql = "SELECT\n".
           "    COUNT(f.refcode)    AS count,\n".
           "    f.state             AS state,\n".
           "    s.color             AS color,\n".
           "    s.weight            AS weight\n".
           "FROM\n".
           "    fnauthor fa,\n".
           "    function f,\n".
           "    state s\n".
           "WHERE\n".
           "    fa.author = ".$iAuthor." AND\n".
           "    fa.function = f.refcode AND\n".
           "    f.state = s.refcode\n".
           "GROUP BY f.state\n".
           "ORDER BY state\n";
    $rdCompletePercent = 0.0;
    if (!($result = mysql_query($sql, $db)))
        Odin32DBSqlError($sql);
    else if (mysql_num_rows($result) < 1)
    {
        echo "
                    <td colspan=2 bgcolor=dddddd>
                        <font size=-1>
                        &nbsp;
                        </font>
                    </td>
            ";
    }
    else
    {
        while ($row = mysql_fetch_row($result))
        {
            $iPercent = (int)($row[0] * 90 /* 10% is reserved to % */ / $cFunctions);
            if ($iPercent == 0)
                $iPercent = 1;
            echo "
                    <td width=".$iPercent." bgcolor=".$row[2].">
                        <font size=-1>
                        &nbsp;
                        </font>
                    </td>
                ";

            $rdCompletePercent += ((double)$row[3] * (double)$row[0]) / $cFunctions;
        }

    }
    mysql_free_result($result);

    /*
     * Complete bar with a total completion percent.
     */
    echo "
                    <td width=10% align=right>
                        <font size=-1 color=000099>
                        ".(int)$rdCompletePercent."%
                        </font>
                    </td>
                </tr>
                </table>
            </td>
        </tr>
        </table>
        ";

}

/**
 * Draws a completion bar.
 * @param       $iDll   Dll reference code.
 *                      If < 0 then for the entire project.
 * @param       $db     Database connection variable.
 * @sketch      Get total number of function in the dll.
 *              Get the number of function per status. (+state color)
 *              Draw bar.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBar2($sCondition, $sName, $db)
{
    /*
     * Count the total number of functions in the DLL.
     */
    $sql = "SELECT COUNT(*) FROM function f";
    if ($sCondition != "")  $sql = $sql." WHERE f.".$sCondition;
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) < 1)
    {
        printf("\n\n<br>Odin32DBCompletionBar2: IPE(1).<br>\n\n");
        Odin32DBSqlError($sql);
        return -1;
    }
    $row = mysql_fetch_row($result);
    $cFunctions = $row[0];
    mysql_free_result($result);


    /*
     * Make
     */
    echo "
        <table width=100% border=0 cellspacing=0 cellpadding=0>
        ";
    if ($sName != '')
        echo "
        <tr>
            <td width=90%>
                <font size=-1 color=000099>
                <tt>".$sName."</tt>
                </font>
            </td>
            <td width=10%></td>
        </tr>";
    echo "
        <tr>
            <td width=90%>
                <table width=100% border=0 cellspacing=0 cellpadding=0>
                <tr>
        ";


    /*
     * Get states and make bar.
     */
    if ($sCondition != "")  $sCondition = "f.".$sCondition." AND";
    $sql = "SELECT\n".
           "    COUNT(f.refcode)    AS count,\n".
           "    f.state             AS state,\n".
           "    s.color             AS color,\n".
           "    s.weight            AS weight\n".
           "FROM\n".
           "    function f,\n".
           "    state s\n".
           "WHERE\n".
           "    ".$sCondition."\n".
           "    s.refcode = f.state\n".
           "GROUP BY f.state\n".
           "ORDER BY state\n";
    $rdCompletePercent = 0.0;
    if (!($result = mysql_query($sql, $db)))
        Odin32DBSqlError($sql);
    else if (mysql_num_rows($result) < 1)
    {
        echo "
                    <td colspan=2 bgcolor=dddddd>
                        <font size=-1>
                        &nbsp;
                        </font>
                    </td>
            ";

    }
    else
    {
        while ($row = mysql_fetch_row($result))
        {
            $iPercent = (int)($row[0] * 90 /* 10% is reserved to % */ / $cFunctions);
            if ($iPercent == 0)
                $iPercent = 1;
            echo "
                    <td width=".$iPercent." bgcolor=".$row[2].">
                        <font size=-1>
                        &nbsp;
                        </font>
                    </td>
                ";

            $rdCompletePercent += ((double)$row[3] * (double)$row[0]) / $cFunctions;
        }

    }
    mysql_free_result($result);

    /*
     * Complete bar with a total completion percent.
     */
    echo "
                    <td width=10% align=right>
                        <font size=-1 color=000099>
                        ".(int)$rdCompletePercent."%
                        </font>
                    </td>
                </tr>
                </table>
            </td>
        </tr>
        </table>
        ";
}


/**
 * Draws a legend for status colors.
 * @param       $db     Database connection variable.
 * @sketch      Get status codes; fetch name and color.
 *
 *
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBStateLegend($db)
{
    /*
     * Count the total number of functions in the DLL.
     */
    $sql = "SELECT
                    name,
                    color
                FROM
                    state
                ORDER BY refcode";
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) < 1)
    {
        printf("\n\n<br>Odin32DBStateLegned: IPE(1).<br>\n\n");
        return -1;
    }
    else
    {
        echo "
            <tr><td></td></tr>
            <tr>
                <td>
                    <center><B><font face=\"WarpSans, Arial\" color=\"#990000\">
                    Status Legend:
                    </font></b></center>
                </td>
            </tr>
            <tr>
                <td>
                    <table width=100% border=0 cellspacing=2 cellpadding=0 align=right>
            ";
        while ($row = mysql_fetch_row($result))
        {
            if (1)
            {
            echo "
                    <tr>
                        <td width=85% align=right>
                            <font size=1 color=000099>
                            ".$row[0]."
                            </font>
                        </td>
                        <td width=15% bgcolor=".$row[1].">
                            <font size=-1>
                            &nbsp;<br>
                            &nbsp;
                            </font>
                        </td>
                    </tr>
                ";
            }
            else
            {
                echo "
                        <tr>
                            <td align=left bgcolor=".$row[1].">
                                <font size=1 color=000000>
                                ".$row[0]."
                                </font>
                            </td>
                        </tr>
                    ";
                }
        }

        echo "
                    </table>
                    </p>
                </td>
            </tr>
            ";
    }


    mysql_free_result($result);
}


/**
 * Dump an SQL statement in HTML.
 *
 * @returns     nothing.
 * @param       $sql    Sql to display.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBSqlError($sql)
{
    echo "<p><font size=1 face=\"courier\">
        SQL-Error:<br>
        ".mysql_error()."
        <br>
        </font>
        </p>
        ";

    echo "<p><font size=1 face=\"courier\">
        SQL:<br>
        ".str_replace(" ", "&nbsp;", str_replace("\n", "<br>\n", $sql))."
        <br>
        </font>
        </p>
        ";
}


/**
 *
 * @returns
 * @param       $sName
 * @param       $array          Result array.
 * @param       $sValueName     Name in the $array for the value.
 * @param       $sRefName       Name in the $array for the reference.
 * @param       $sOdin32DBArg   Odin32DB.phtml argument.
 * @param       $sNullText      Null text (if the array element is NULL display this).
 * @param       $sPostText      Text to insert after the value.
 *
 */
function Odin32DBInfoRow1($sName, $array, $sValueName, $sRefName, $sOdin32DBArg, $sNullText, $sPostText)
{
    echo "
        <tr>
            <td width=35%><tt>".$sName."</tt></td>
            <td valign=top>";
    if (isset($array[$sValueName]))
    {
        if ($sRefName != "" && isset($array[$sRefName]) && $sOdin32DBArg != "")
        {
            echo "<a href=\"Odin32DB.phtml?".$sOdin32DBArg."=".$array[$sRefName]."\">";
            $sPostText = "</a>".$sPostText;
        }
        echo $array[$sValueName];
        echo $sPostText;
    }
    else if ($sNullText != "")
        echo "<i>".$sNullText."</i>";

    echo "
            </td>
        <tr>\n";
}


/**
 *
 * @returns
 * @param       $sName
 * @param       $sValue         Value.
 * @param       $sRef           Reference.
 * @param       $sOdin32DBArg   Odin32DB.phtml argument.
 * @param       $sNullText      Null text (if the array element is NULL display this).
 * @param       $sPostText      Text to insert after the value.
 *
 */
function Odin32DBInfoRow1NoArray($sName, $sValue, $sRef, $sOdin32DBArg, $sNullText, $sPostText)
{
    echo "
        <tr>
            <td width=35%><tt>".$sName."</tt></td>
            <td valign=top>";
    if (isset($sValue) && $sValue != "")
    {
        if (isset($sRef) && $sRef != "" && $sOdin32DBArg != "")
        {
            echo "<a href=\"Odin32DB.phtml?".$sOdin32DBArg."=".$sRef."\">";
            $sPostText = "</a>".$sPostText;
        }
        echo $sValue.$sPostText;
    }
    else if ($sNullText != "")
        echo "<i>".$sNullText."</i>";

    echo "
            </td>
        <tr>\n";
}



/**
 *
 * @returns
 * @param       $sName
 * @param       $array          Result array.
 * @param       $sValueName1    Name in the $array for the value.
 * @param       $sRefName1      Name in the $array for the reference.
 * @param       $sOdin32DBArg1  Odin32DB.phtml argument.
 * @param       $sNullText      Null text (if the array element is NULL display this).
 * @param       $sPostText      Text to insert after the value.
 * @param       $sValueName2    Name in the $array for the value.
 * @param       $sRefName2      Name in the $array for the reference.
 * @param       $sOdin32DBArg2  Odin32DB.phtml argument.
 *
 */
function Odin32DBInfoRow2($sName, $array, $sValueName1, $sRefName1, $sOdin32DBArg1, $sNullText, $sPostText,
                                          $sValueName2, $sRefName2, $sOdin32DBArg2)
{
    echo "
        <tr>
            <td width=35%><tt>".$sName."</tt></td>
            <td valign=top>";
    if (isset($array[$sValueName1]))
    {
        if ($sRefName1 != "" && isset($array[$sRefName1]) && $sOdin32DBArg1 != "")
        {
            echo "<a href=\"Odin32DB.phtml?".$sOdin32DBArg1."=".$array[$sRefName1]."\">";
            $sPostText = "</a>".$sPostText;
        }
        echo $array[$sValueName1];
        echo $sPostText;

        if (isset($array[$sValueName2]))
        {
            if ($sRefName2 != "" && isset($array[$sRefName2]) && $sOdin32DBArg2 != "")
            {
                echo "<a href=\"Odin32DB.phtml?".$sOdin32DBArg2."=".$array[$sRefName2]."\">";
                echo $array[$sValueName2]."</a>";
            }
            else
                echo $array[$sValueName2];
        }
    }
    else if ($sNullText != "")
        echo "<i>".$sNullText."</i>";

    echo "</td>
        <tr>\n";
}



/**
 * Inserts a documentation row from database..
 * @param       $sName      Name of the information.
 * @param       $array      DB result array.
 * @param       $sValueName Name in the DB result array.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      Displays <i>not available</i> if empty field.
 */
function Odin32DBDocRow1($sName, $array, $sValueName)
{
    PodNaslov($sName);
    if (isset($array[$sValueName]))
    {
        echo $array[$sValueName];
    }
    else
        echo "<i>not available</i>";
}


/**
 * Using (Odin32DB)Naslov.
 * Inserts a documentation row from database..
 * @param       $sName      Name of the information.
 * @param       $sLabel     Section label.
 * @param       $array      DB result array.
 * @param       $sValueName Name in the DB result array.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      Displays <i>not available</i> if empty field.
 */
function Odin32DBDocRow(&$aContent, $sName, $sLabel, $array, $sValueName)
{
    Odin32DBNaslov($aContent, $sName, $sLabel);
    if (isset($array[$sValueName]))
    {
        echo $array[$sValueName];
    }
    else
        echo "<i>not available</i>";
}


/* NAVIGATION */
/* NAVIGATION */
/* NAVIGATION */

/**
 * Make top of page navigation stuff for the Odin32 database pages.
 * @param       $sExpand    Expand arguments.
 * @param       $sCollapse  Collapse arguments.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBNavigationTop($sExpand, $sCollapse)
{
    echo "\n<center><font size=1>\n";

    echo "<a href=\"Odin32DB.phtml\">Root</a>\n".
         " - <a href=\"Odin32DB.phtml?dlls=1\">Dlls</a>\n".
         " - <a href=\"Odin32DB.phtml?authors=1\">Authors</a>\n".
         " - <a href=\"Odin32DB.phtml?apigroups=1\">API Groups</a>\n";

    if ($sExpand != "" && $sCollapse != "")
    {
        echo "<br><a href=\"Odin32DB.phtml?".$sExpand."\">Expand</a> - \n".
             "<a href=\"Odin32DB.phtml?".$sCollapse."\">Collapse</a>\n";
    }

    echo "</font></center>\n";
}


/**
 * Make bottom of page navigation stuff for the Odin32 database pages.
 * @param       $sExpand    Expand arguments.
 * @param       $sCollapse  Collapse arguments.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBNavigationBottom($sExpand, $sCollapse)
{
    echo "\n<p><br><center>\n".
         "<font size=1>\n";

    if ($sExpand != "" && $sCollapse != "")
    {
        echo "<a href=\"Odin32DB.phtml?".$sExpand."\">Expand</a> - \n".
             "<a href=\"Odin32DB.phtml?".$sCollapse."\">Collapse</a><br>\n";
    }

    echo
         "<a href=\"Odin32DB.phtml\">Root</a>\n".
         " - <a href=\"Odin32DB.phtml?dlls=1\">Dlls</a>\n".
         " - <a href=\"Odin32DB.phtml?authors=1\">Authors</a>\n".
         " - <a href=\"Odin32DB.phtml?apigroups=1\">API Groups</a>\n";
    echo "</font></<center>\n";
}



/* INFO OUTPUT */
/* INFO OUTPUT */
/* INFO OUTPUT */


/**
 * Writes standard function info.
 *
 * @returns     void
 * @param       $aContent       Contents array. (input/output)
 * @param       $db             Database handle.
 * @param       $iRefcode       Function reference code.
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBFunctionInfo(&$aContent, $db, $iRefcode)
{
    Odin32DBNavigationTop("","");

    $sql = sprintf("SELECT\n".
                   "    f.name          AS name,\n".
                   "    f.intname       AS intname,\n".
                   "    f.ordinal       AS ordinal,\n".
                   "    f.return        AS return,\n".
                   "    f.description   AS description,\n".
                   "    f.remark        AS remark,\n".
                   "    f.returndesc    AS returndesc,\n".
                   "    f.sketch        AS sketch,\n".
                   "    f.equiv         AS equiv,\n".
                   "    f.time          AS time,\n".
                   "    g.name          AS apigroupname,\n".
                   "    g.refcode       AS apigrouprefcode,\n".
                   "    a.name          AS aliasname,\n".
                   "    a.refcode       AS aliasrefcode,\n".
                   "    ad.name         AS aliasdllname,\n".
                   "    ad.refcode      AS aliasdllrefcode,\n".
                   "    d.name          AS dllname,\n".
                   "    d.refcode       AS dllrefcode,\n".
                   "    i.name          AS filename,\n".
                   "    i.refcode       AS filerefcode,\n".
                   "    s.name          AS state,\n".
                   "    c.description   AS type\n".
                   "FROM\n".
                   "    function f\n".
                   "    LEFT OUTER JOIN function a ON f.aliasfn  = a.refcode\n".
                   "    LEFT OUTER JOIN dll ad     ON a.dll      = ad.refcode\n".
                   "    LEFT OUTER JOIN apigroup g ON f.apigroup = g.refcode\n".
                   "    LEFT       JOIN dll d      ON f.dll      = d.refcode\n".
                   "    LEFT       JOIN state s    ON f.state    = s.refcode\n".
                   "    LEFT OUTER JOIN file i     ON f.file     = i.refcode\n".
                   "    LEFT       JOIN code c     ON f.type     = c.code\n".
                   "WHERE\n".
                   "    c.codegroup = 'FTYP' AND\n".
                   "    f.refcode = %d",
                    $iRefcode);
    if (($result = mysql_query($sql, $db)) && @mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov($aContent, "General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        if (isset($array["intname"]))
            Odin32DBInfoRow1("Internal Name", $array, "intname","","","","");
        else
            Odin32DBInfoRow1("Internal Name", $array, "name","","","","");
        Odin32DBInfoRow1("Type", $array, "type", "", "","invalid","");
        Odin32DBInfoRow1("State", $array, "state", "", "","invalid","");
        Odin32DBInfoRow1("Dll", $array, "dllname", "dllrefcode", "dllrefcode","","");
        Odin32DBInfoRow1("Ordinal", $array, "ordinal","","","not available","");
        if (isset($array["apigroupname"]))
            Odin32DBInfoRow1("API Group", $array, "apigroupname", "apigrouprefcode", "apigrouprefcode","","");
        Odin32DBInfoRow1("File", $array, "filename", "filerefcode", "filerefcode","not available","");
        if (isset($array["aliasrefcode"]))
            Odin32DBInfoRow2("Forwards", $array, "aliasdllname", "aliasdllrefcode", "dllrefcode","",".",
                                                 "aliasname", "aliasrefcode", "functionrefcode");
        else
        {
            $sql = sprintf("SELECT\n".
                           "    d.name      AS dllname,\n".
                           "    d.refcode   AS dllrefcode,\n".
                           "    f.name      AS fnname,\n".
                           "    f.refcode   AS fnrefcode\n".
                           "FROM\n".
                           "    function f,\n".
                           "    dll d\n".
                           "WHERE\n".
                           "    f.aliasfn = %d AND\n".
                           "    f.dll = d.refcode\n".
                           "ORDER BY d.name, f.name\n",
                           $iRefcode);
            if (($result2 = mysql_query($sql, $db)))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    $sValue = "";
                    $f = 0;
                    while ($aAlias = mysql_fetch_array($result2))
                    {
                        if ($f) $sValue = $sValue."<br>";
                        else    $f = 1;
                        $sValue = $sValue."<a href=\"Odin32DB.phtml?dllrefcode=".
                                          $aAlias["dllrefcode"]."\">".$aAlias["dllname"]."</a>.".
                                          "<a href=\"Odin32DB.phtml?functionrefcode=".
                                          $aAlias["fnrefcode"]."\">".$aAlias["fnname"]."</a>";
                    }
                    Odin32DBInfoRow1NoArray("Forwarded as", $sValue, "","","","");
                }
            }
            else
                Odin32DBSqlError($sql);
        }
        echo "\n</table>\n";

        /*
         * Completion
         */
        Odin32DBNaslov($aContent, "Completion", "completion");
        Odin32DBCompletionBarFunction($iRefcode, "", $db);


        /*
         * Declaration
         */
        Odin32DBNaslov($aContent, "Declaration", "declaration");
        echo "\n<pre>";
        if (isset($array["return"]))
            echo $array["return"]." ";
        echo $array["name"]."(";
        $sql = sprintf("SELECT\n".
                       "    name                AS name,\n".
                       "    type                AS type,\n".
                       "    description         AS description\n".
                       "FROM\n".
                       "    parameter\n".
                       "WHERE\n".
                       "    function = %d\n".
                       "ORDER BY sequencenbr",
                        $iRefcode);
        if (($result2 = mysql_query($sql, $db)) && ($cParams = mysql_num_rows($result2)) > 0)
        {
            while ($param = mysql_fetch_array($result2))
            {
                if (--$cParams == 0)
                    printf("\n        %-20s %s", $param["type"], $param["name"]);
                else
                    printf("\n        %-20s %s,", $param["type"], $param["name"]);
            }
        }
        else
            echo "void";

        echo ");\n";
        echo "</pre>\n";

        /*
         * Description
         */
        Odin32DBDocRow($aContent, "Description", "desc", $array, "description");


        /*
         * Parameters
         */
        Odin32DBNaslov($aContent, "Parameters", "params");
        if ($result2 && mysql_num_rows($result2) > 0 && mysql_data_seek($result2, 0))
        {
            while ($param = mysql_fetch_array($result2))
            {
                echo "\n<dt><b>".$param["name"].":</b></dt>\n";
                if (isset($param["description"]))
                {
                    if (1)
                    {
                        echo "\n  <table width=100% border=0 cellpadding=0>\n";
                        echo "  <tr><td width=10%>&nbsp;</td>\n";
                        $sDescription = str_replace("<BR>", "", str_replace("<BR><BR>\n","<br>",$param["description"]));
                        echo "  <td width=90%><font size=-1>".$sDescription."</font></td></tr>\n";
                        echo "\n</table>\n";
                    }
                    else
                    {
                        $sDescription = str_replace("<BR>", "", str_replace("<BR><BR>\n","<br>",$param["description"]));
                        echo "<dd><font size=-1>".$sDescription."</font></dd>\n\n";
                    }
                }
                echo "<p>\n";
            }
        }
        else
            echo "void";


        /*
         * Returns
         */
        Odin32DBDocRow($aContent, "Returns", "return", $array, "returndesc");

        /*
         * Sketch/Algorithm
         */
        Odin32DBDocRow($aContent, "Sketch/Algorithm", "sketch",  $array, "sketch");

        /*
         * Remark
         */
        Odin32DBDocRow($aContent, "Remarks", "remark", $array, "remark");

        /*
         * Authors
         */
        Odin32DBNaslov($aContent, "Authors", "Authors");
        $sql = sprintf("SELECT\n".
                       "    a.name                AS name,\n".
                       "    a.refcode             AS refcode\n".
                       "FROM\n".
                       "    fnauthor fa\n".
                       "    JOIN function f\n".
                       "    JOIN author a\n".
                       "WHERE\n".
                       "    f.refcode  = %d AND\n".
                       "    fa.function = f.refcode AND\n".
                       "    fa.author   = a.refcode\n".
                       "ORDER BY a.name",
                        $iRefcode);
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0)
        {
            while ($author = mysql_fetch_array($result2))
                echo "<a href=\"Odin32DB.phtml?authorrefcode=".$author["refcode"]."\">".$author["name"]."</a><br>\n";
        }
        else
            echo "<i>Hmm. Seems noone wrote this function...</i><br>\n";
    }
    else
    {
        echo "<p> No data! Invalid refcode? </p>";
        Odin32DBSqlError($sql);
    }
    Odin32DBNavigationBottom("","");
}



/**
 * Writes standard dll info.
 *
 * @returns     void
 * @param       $aContent       Contents array. (input/output)
 * @param       $db             Database handle.
 * @param       $iRefcode       Dll reference code.
 * @param       $fFunctions     Flags which tells wether to list all functions or not.
 * @param       $fFiles         Flags which tells wether to list all files or not.
 * @param       $fAPIGroups     Flags which tells wether to list all apigroups or not.
 * @param       $fAuthors       Flags which tells wether to list all authors or not.
 * @param       $fSortByState   Flags which tells wether to sort functions by
 *                              state and function name or just by function name.
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBDllInfo(&$aContent, $db, $iRefcode, $fFunctions, $fFiles, $fAPIGroups, $fAuthors, $fSortByState)
{
    /*
     * Navigation - TOP
     */
    $sExpand    = "dllrefcode=".$iRefcode."&fFiles=1&fFunctions=1&fAPIGroups=1&fAuthors=1";
    if ($fSortByState)  $sExpand = $sExpand."&fSortByState=".$fSortByState;
    $sCollapse  = "dllrefcode=".$iRefcode;
    Odin32DBNavigationTop($sExpand, $sCollapse);

    /*
     * Fetch (vital) data.
     */
    $sql = sprintf("SELECT\n".
                   "    d.name          AS name,\n".
                   "    d.description   AS description,\n".
                   "    c.description   AS type\n".
                   "FROM\n".
                   "    dll d,\n".
                   "    code c\n".
                   "WHERE\n".
                   "    c.codegroup = 'DTYP' AND\n".
                   "    d.type = c.code AND\n".
                   "    d.refcode = %d",
                    $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov($aContent, "General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("Description", $array, "description","","","","");
        Odin32DBInfoRow1("Type", $array, "type","","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    dll = %d",
                       $iRefcode);
        $cFunctions = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($array2 = mysql_fetch_array($result2)))
            {
            Odin32DBInfoRow1("# Functions", $array2, "functions","","","","");
            $cFunctions = $array2["functions"];
            }

        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as files\n".
                       "FROM\n".
                       "    file\n".
                       "WHERE\n".
                       "    dll = %d",
                       $iRefcode);
        $cFiles = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($array2 = mysql_fetch_array($result2)))
        {
            Odin32DBInfoRow1("# Source files", $array2, "files","","","","");
            $cFiles = $array2["files"];
        }

        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as apigroups\n".
                       "FROM\n".
                       "    apigroup\n".
                       "WHERE\n".
                       "    dll = %d",
                       $iRefcode);
        $cAPIGroups = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($array2 = mysql_fetch_array($result2)))
        {
            Odin32DBInfoRow1("# API Groups", $array2, "apigroups","","","","");
            $cAPIGroups = $array2["apigroups"];
        }

        echo "\n</table>\n";


        /*
         * Completion
         */
        Odin32DBNaslov($aContent, "Completion", "completion");
        Odin32DBCompletionBarDll($iRefcode, "", $db);

        /*
         * States
         */
        Odin32DBNaslov($aContent, "Status", "status");
        $sql = sprintf("SELECT\n".
                       "    s.name              AS state,\n".
                       "    s.color             AS color,\n".
                       "    COUNT(f.state)      AS functions\n".
                       "FROM\n".
                       "    state s\n".
                       "    LEFT OUTER JOIN function f ON s.refcode = f.state AND f.dll = %d\n".
                       "GROUP BY s.refcode\n".
                       "ORDER BY s.refcode",
                        $iRefcode);
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n";
            while ($aState = mysql_fetch_array($result2))
            {
                echo "<tr>\n".
                     "    <td width=75%><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["state"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["functions"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".@(int)((int)$aState["functions"] * 100 / $cFunctions)."%</b></font></td>\n".
                     "</tr>\n";
            }

            echo "\n</table>\n";
        }
        else
            Odin32DBSqlError($sql);


        /*
         * Functions
         */
        Odin32DBNaslov($aContent, "Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    s.name              AS state,\n".
                           "    s.color             AS color\n".
                           "FROM\n".
                           "    function f\n".
                           "    LEFT JOIN state s ON f.state = s.refcode\n".
                           "WHERE\n".
                           "    f.dll = %d\n",
                            $iRefcode);
            if ($fSortByState)
                $sql = $sql."ORDER BY s.refcode, f.name";
            else
                $sql = $sql."ORDER BY f.name";
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Function Name</b></font></td>\n".
                         "  <td><font size=-1><b>State</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?functionrefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td><font size=-1 color=\"#".$aFunction["color"]."\">".$aFunction["state"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n".
                         "<p>Click <a href=\"Odin32DB.phtml#functions?dllrefcode=".$iRefcode."&fFunctions=1";
                    if ($fFiles)        echo "&fFiles=".$fFiles;
                    if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
                    if ($fAuthors)      echo "&fAuthors=".$fAuthors;
                    if ($fSortByState)  echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted alphabetical.<br>\n";
                    else                echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted by state.<br>\n";
                }
                else
                    echo "<i>No Functions.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#functions?dllrefcode=".$iRefcode."&fFunctions=1";
            if ($fFiles)        echo "&fFiles=".$fFiles;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all functions.\n";
        }


        /*
         * Files
         */
        Odin32DBNaslov($aContent, "Files", "files");
        if ($fFiles)
        {
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    COUNT(fn.refcode)   AS functions\n".
                           "FROM\n".
                           "    file f\n".
                           "    LEFT OUTER JOIN function fn ON fn.file = f.refcode\n".
                           "WHERE\n".
                           "    f.dll = %d\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                            $iRefcode);

            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td><font size=-1><b>Filename</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td width=75%><font size=-1><a href=\"Odin32DB.phtml?filerefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>No Files.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#files?dllrefcode=".$iRefcode."&fFiles=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all files.\n";
        }


        /*
         * API Groups
         */
        if ($cAPIGroups > 0)
        {
            Odin32DBNaslov($aContent, "API Groups", "apigroups");
            if ($fAPIGroups)
            {
                $sql = sprintf("SELECT\n".
                               "    g.name              AS name,\n".
                               "    g.refcode           AS refcode,\n".
                               "    COUNT(f.refcode)    AS functions\n".
                               "FROM\n".
                               "    apigroup g\n".
                               "    JOIN function f\n".
                               "WHERE\n".
                               "    g.dll = %d AND\n".
                               "    f.dll = %d AND\n".
                               "    f.apigroup = g.refcode\n".
                               "GROUP BY f.apigroup\n".
                               "ORDER BY g.name\n",
                               $iRefcode,
                               $iRefcode);
                if ($result2 = mysql_query($sql, $db))
                {
                    if (mysql_num_rows($result2) > 0)
                    {
                        echo "\n<table width=100% border=0 cellpadding=0>\n".
                             "<tr>\n".
                             "  <td width=75%><font size=-1><b>Group Name</b></font></td>\n".
                             "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                             "</tr>\n";
                        while ($aFunction = mysql_fetch_array($result2))
                        {
                            echo "<tr>\n".
                                 "  <td><font size=-1><a href=\"Odin32DB.phtml?apigrouprefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                                 "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                                 "</tr>\n";
                        }
                        echo "\n</table>\n";
                    }
                    else
                        echo "<i>No API Groups.</i><br>\n";
                }
                else
                    Odin32DBSqlError($sql);
            }
            else
            {
                echo "Click <a href=\"Odin32DB.phtml#apigroups?dllrefcode=".$iRefcode."&fAPIGroups=1";
                if ($fFunctions)    echo "&fFunctions=".$fFunctions;
                if ($fFiles)        echo "&fFiles=".$fFiles;
                if ($fAuthors)      echo "&fAuthors=".$fAuthors;
                if ($fSortByState)  echo "&fSortByState=".$fSortByState;
                echo "\">here</a> to see all the API Groups.\n";
            }
        }


        /*
         * Authors
         */
        Odin32DBNaslov($aContent, "Authors", "authors");
        if ($fAuthors)
        {
            $sql = sprintf("SELECT\n".
                           "    a.name              AS name,\n".
                           "    a.refcode           AS refcode,\n".
                           "    COUNT(f.refcode)    AS functions\n".
                           "FROM\n".
                           "    fnauthor fa\n".
                           "    JOIN function f\n".
                           "    JOIN author a\n".
                           "WHERE\n".
                           "    f.dll       = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    fa.author   = a.refcode\n".
                           "GROUP BY a.refcode\n".
                           "ORDER BY a.name\n",
                            $iRefcode
                            );
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Author</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?authorrefcode=".$aFunction["refcode"]."&dll=".$iRefcode."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>No Authors</i>.<br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#authors?dllrefcode=".$iRefcode."&fAuthors=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fFiles)        echo "&fFiles=".$fFiles;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all authors.\n";
        }
    }
    else
    {
        echo "<p> No data! Invalid refcode? </p>";
        Odin32DBSqlError($sql);
    }

    /*
     * Navigation - Bottom
     */
    Odin32DBNavigationBottom($sExpand, $sCollapse);
}



/**
 * Writes standard file info.
 *
 * @returns     void
 * @param       $aContent       Contents array. (input/output)
 * @param       $db             Database handle.
 * @param       $iRefcode       File reference code.
 * @param       $fFunctions     Flags which tells wether to list all functions or not.
 * @param       $fAPIGroups     Flags which tells wether to list all apigroups or not.
 * @param       $fAuthors       Flags which tells wether to list all authors or not.
 * @param       $fSortByState   Flags which tells wether to sort functions by
 *                              state and function name or just by function name.
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBFileInfo(&$aContent, $db, $iRefcode, $fFunctions, $fAPIGroups, $fAuthors, $fSortByState)
{
    /*
     * Navigation - TOP
     */
    $sExpand    = "filerefcode=".$iRefcode."&fFiles=1&fFunctions=1&fAPIGroups=1&fAuthors=1";
    if ($fSortByState)  $sExpand = $sExpand."&fSortByState=".$fSortByState;
    $sCollapse  = "filerefcode=".$iRefcode;
    Odin32DBNavigationTop($sExpand, $sCollapse);

    /*
     * Fetch (vital) data.
     */
    $sql = sprintf("SELECT\n".
                   "    f.name          AS name,\n".
                   "    f.refcode       AS refcode,\n".
                   "    f.lastdatetime  AS lastdatetime,\n".
                   "    a.name          AS lastauthorname,\n".
                   "    f.lastauthor    AS lastauthorrefcode,\n".
                   "    f.revision      AS revision,\n".
                   "    f.description   AS description,\n".
                   "    f.dll           AS dllrefcode,\n".
                   "    d.name          AS dllname\n".
                   "FROM\n".
                   "    file f,\n".
                   "    dll d,\n".
                   "    author a\n".
                   "WHERE\n".
                   "    f.refcode   = %d AND\n".
                   "    f.dll       = d.refcode AND\n".
                   "    f.lastauthor= a.refcode",
                    $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov($aContent, "General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("Revision", $array, "revision","","","","");
        Odin32DBInfoRow1("Changed", $array, "lastdatetime","","","","");
        Odin32DBInfoRow1("Last Author", $array, "lastauthorname","lastauthorrefcode","authorrefcode","","");
        Odin32DBInfoRow1("Dll", $array, "dllname","dllrefcode","dllrefcode","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    file = %d",
                       $iRefcode);
        $cFunctions = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($aFunctions = mysql_fetch_array($result2)))
            {
            Odin32DBInfoRow1("# Functions", $aFunctions, "functions","","","","");
            $cFunctions = $aFunctions["functions"];
            }

        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    file = %d AND\n".
                       "    apigroup IS NOT NULL\n".
                       "GROUP BY apigroup\n",
                        $iRefcode);
        $cAPIGroups = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cAPIGroups = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# API Groups", $cAPIGroups, "","","","");
        if (!$result2)
            Odin32DBSqlError($sql);

        echo "\n</table>\n";

        /*
         * Description
         */
        Odin32DBDocRow($aContent, "Description", "description", $array, "description");

        /*
         * Completion
         */
        Odin32DBNaslov($aContent, "Completion", "completion");
        Odin32DBCompletionBarFile($iRefcode, "", $db);

        /*
         * States
         */
        Odin32DBNaslov($aContent, "Status", "status");
        $sql = sprintf("SELECT\n".
                       "    s.name              AS state,\n".
                       "    s.color             AS color,\n".
                       "    COUNT(f.state)      AS functions\n".
                       "FROM\n".
                       "    state s\n".
                       "    LEFT OUTER JOIN function f ON s.refcode = f.state AND f.file = %d\n".
                       "GROUP BY s.refcode\n".
                       "ORDER BY s.refcode",
                        $iRefcode);
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n";
            while ($aState = mysql_fetch_array($result2))
            {
                echo "<tr>\n".
                     "    <td width=75%><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["state"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["functions"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".@(int)((int)$aState["functions"] * 100 / $cFunctions)."%</b></font></td>\n".
                     "</tr>\n";
            }

            echo "\n</table>\n";
        }
        else
            Odin32DBSqlError($sql);


        /*
         * Functions
         */
        Odin32DBNaslov($aContent, "Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    s.name              AS state,\n".
                           "    s.color             AS color\n".
                           "FROM\n".
                           "    function f\n".
                           "    LEFT JOIN state s ON f.state = s.refcode\n".
                           "WHERE\n".
                           "    f.file = %d\n",
                           $iRefcode);
            if ($fSortByState)
                $sql = $sql."ORDER BY s.refcode, f.name";
            else
                $sql = $sql."ORDER BY f.name";
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Function Name</b></font></td>\n".
                         "  <td><font size=-1><b>State</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?functionrefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td><font size=-1 color=\"#".$aFunction["color"]."\">".$aFunction["state"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n".
                         "<p>Click <a href=\"Odin32DB.phtml#functions?filerefcode=".$iRefcode."&fFunctions=1";
                    if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
                    if ($fAuthors)      echo "&fAuthors=".$fAuthors;
                    if ($fSortByState)  echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted alphabetical.<br>\n";
                    else                echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted by state.<br>\n";
                }
                else
                    echo "<i>No functions found</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#functions?filerefcode=".$iRefcode."&fFunctions=1";
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all functions.\n";
        }


        /*
         * API Groups
         */
        if ($cAPIGroups > 0)
        {
            Odin32DBNaslov($aContent, "API Groups", "apigroups");
            if ($fAPIGroups)
            {
                $sql = sprintf("SELECT\n".
                               "    g.name              AS name,\n".
                               "    g.refcode           AS refcode,\n".
                               "    COUNT(f.refcode)    AS functions\n".
                               "FROM\n".
                               "    apigroup g\n".
                               "    JOIN function f\n".
                               "WHERE\n".
                               "    f.file = %d AND\n".
                               "    f.apigroup = g.refcode\n".
                               "GROUP BY f.apigroup\n".
                               "ORDER BY g.name\n",
                               $iRefcode);
                if ($result2 = mysql_query($sql, $db))
                {
                    if (mysql_num_rows($result2) > 0)
                    {
                        echo "\n<table width=100% border=0 cellpadding=0>\n".
                             "<tr>\n".
                             "  <td width=75%><font size=-1><b>Group Name</b></font></td>\n".
                             "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                             "</tr>\n";
                        while ($aFunction = mysql_fetch_array($result2))
                        {
                            echo "<tr>\n".
                                 "  <td><font size=-1><a href=\"Odin32DB.phtml?apigrouprefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                                 "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                                 "</tr>\n";
                        }
                        echo "\n</table>\n";
                    }
                    else
                        echo "<i>Not API Groups found.</i><br>\n";
                }
                else
                    Odin32DBSqlError($sql);
            }
            else
            {
                echo "Click <a href=\"Odin32DB.phtml#apigroups?filerefcode=".$iRefcode."&fAPIGroups=1";
                if ($fFunctions)    echo "&fFunctions=".$fFunctions;
                if ($fAuthors)      echo "&fAuthors=".$fAuthors;
                if ($fSortByState)  echo "&fSortByState=".$fSortByState;
                echo "\">here</a> to see all the API Groups.\n";
            }
        }


        /*
         * Authors
         */
        Odin32DBNaslov($aContent, "Authors", "authors");
        if ($fAuthors)
        {
            $sql = sprintf("SELECT\n".
                           "    a.name              AS name,\n".
                           "    a.refcode           AS refcode,\n".
                           "    COUNT(f.refcode)    AS functions\n".
                           "FROM\n".
                           "    fnauthor fa\n".
                           "    JOIN function f\n".
                           "    JOIN author a\n".
                           "WHERE\n".
                           "    f.file      = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    fa.author   = a.refcode\n".
                           "GROUP BY a.refcode\n".
                           "ORDER BY a.name\n",
                            $iRefcode
                            );
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Author</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?authorrefcode=".$aFunction["refcode"]."&file=".$iRefcode."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>Not authors found.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#authors?filerefcode=".$iRefcode."&fAuthors=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all authors.\n";
        }
    }
    else
    {
        echo "<p> No data! Invalid refcode? </p>";
        Odin32DBSqlError($sql);
    }

    /*
     * Navigation - Bottom
     */
    Odin32DBNavigationBottom($sExpand, $sCollapse);
}



/**
 * Writes standard file info.
 *
 * @returns     void
 * @param       $aContent       Contents array. (input/output)
 * @param       $db             Database handle.
 * @param       $iRefcode       Author reference code.
 * @param       $fDlls          Flags which tells wether to list all dlls or not.
 * @param       $fFunctions     Flags which tells wether to list all functions or not.
 * @param       $fFiles         Flags which tells wether to list all files or not.
 * @param       $fAPIGroups     Flags which tells wether to list all apigroups or not.
 * @param       $fSortByState   Flags which tells wether to sort functions by
 *                              state and function name or just by function name.
 * @param       $iDllRefcode    Dll refcode. All Dll if < 0.
 *                              (not implemented yet)
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBAuthorInfo(&$aContent, $db, $iRefcode, $fDlls, $fFunctions, $fFiles, $fAPIGroups, $fSortByState, $iDllRefcode)
{
    /*
     * Navigation - TOP
     */
    $sExpand    = "authorrefcode=".$iRefcode."&fDlls=1&fFiles=1&fFunctions=1&fAPIGroups=1&fAuthors=1&dll=".$iDllRefcode;
    if ($fSortByState)  $sExpand = $sExpand."&fSortByState=".$fSortByState;
    $sCollapse  = "authorrefcode=".$iRefcode;
    Odin32DBNavigationTop($sExpand, $sCollapse);

    /*
     * Fetch (vital) data.
     */
    $sql = sprintf("SELECT\n".
                   "    a.name          AS name,\n".
                   "    a.refcode       AS refcode,\n".
                   "    a.initials      AS initials,\n".
                   "    a.alias         AS alias,\n".
                   "    a.email         AS email,\n".
                   "    a.country       AS country,\n".
                   "    a.location      AS location,\n".
                   "    a.description   AS description\n".
                   "FROM\n".
                   "    author a\n".
                   "WHERE\n".
                   "    a.refcode = %d",
                   $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov($aContent, "General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("e-mail", $array, "email","","","",""); //???? should all authors have email address displayed?
        Odin32DBInfoRow1("CVS User", $array, "alias","","","","");
        Odin32DBInfoRow1("Country", $array, "country","","","","");
        Odin32DBInfoRow1("Location", $array, "location","","","","");
        if (isset($array["description"]))
            Odin32DBInfoRow1("Description", $array, "description","","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    fnauthor\n".
                       "WHERE\n".
                       "    author = %d",
                       $iRefcode);
        $cFunctions = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($aFunctions = mysql_fetch_array($result2)))
            {
            Odin32DBInfoRow1("# Functions", $aFunctions, "functions","","","","");
            $cFunctions = $aFunctions["functions"];
            }
        $sql = sprintf("SELECT\n".
                       "    COUNT(f.dll) as functions
                       \n".
                       "FROM\n".
                       "    fnauthor fa,\n".
                       "    function f\n".
                       "WHERE\n".
                       "    fa.author = %d AND".
                       "    f.refcode = fa.function\n".
                       "GROUP BY f.dll",
                       $iRefcode);
        $cDlls = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cDlls = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# Dlls", $cDlls, "","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(f.dll) as functions
                       \n".
                       "FROM\n".
                       "    fnauthor fa,\n".
                       "    function f\n".
                       "WHERE\n".
                       "    fa.author = %d AND".
                       "    f.file >= 0 AND".
                       "    f.refcode = fa.function \n".
                       "GROUP BY f.file",
                       $iRefcode);
        $cFiles = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cFiles = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# Files", $cFiles, "","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(f.dll) as functions
                       \n".
                       "FROM\n".
                       "    fnauthor fa,\n".
                       "    function f\n".
                       "WHERE\n".
                       "    fa.author = %d AND".
                       "    f.apigroup IS NOT NULL AND".
                       "    f.refcode = fa.function\n".
                       "GROUP BY f.apigroup",
                       $iRefcode);
        $cAPIGroups = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cAPIGroups = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# API Groups", $cAPIGroups, "","","","");

        echo "\n</table>\n";

        /*
         * Completion
         */
        Odin32DBNaslov($aContent, "Completion", "completion");
        Odin32DBCompletionBarAuthor($iRefcode, "", $db);

        /*
         * States
         */
        Odin32DBNaslov($aContent, "Status", "status");
        $sql = sprintf("SELECT\n".
                       "    s.name              AS state,\n".
                       "    s.color             AS color,\n".
                       "    COUNT(f.refcode)    AS functions\n".
                       "FROM\n".
                       "    state s\n".
                       "    LEFT OUTER JOIN fnauthor fa ON fa.author = %d\n".
                       "    LEFT OUTER JOIN function f  ON s.refcode = f.state AND fa.function = f.refcode\n".
                       "GROUP BY s.refcode\n".
                       "ORDER BY s.refcode",
                        $iRefcode);
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n";
            while ($aState = mysql_fetch_array($result2))
            {
                echo "<tr>\n".
                     "    <td width=75%><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["state"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["functions"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".@(int)((int)$aState["functions"] * 100 / $cFunctions)."%</b></font></td>\n".
                     "</tr>\n";
            }

            echo "\n</table>\n";
        }
        else
            Odin32DBSqlError($sql);

        /*
         * Dlls
         */
        Odin32DBNaslov($aContent, "Dlls", "dlls");
        if ($fDlls)
        {
            $sql = sprintf("SELECT\n".
                           "    d.name              AS name,\n".
                           "    d.refcode           AS refcode,\n".
                           "    COUNT(f.refcode)    AS functions\n".
                           "FROM\n".
                           "    fnauthor fa,\n".
                           "    dll d,\n".
                           "    function f\n".
                           "WHERE\n".
                           "    fa.author = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    f.dll = d.refcode\n".
                           "GROUP BY d.refcode\n".
                           "ORDER BY d.name\n",
                            $iRefcode);
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td><font size=-1><b>Dlls</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td width=75%><font size=-1><a href=\"Odin32DB.phtml?dllrefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>No Files.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#dlls?authorrefcode=".$iRefcode."&fDlls=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fFiles)        echo "&fFiles=".$fFiles;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all files.\n";
        }


        /*
         * Functions
         */
        Odin32DBNaslov($aContent, "Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    d.name              AS dllname,\n".
                           "    d.refcode           AS dllrefcode,\n".
                           "    s.name              AS state,\n".
                           "    s.color             AS color\n".
                           "FROM\n".
                           "    fnauthor fa\n".
                           "    JOIN function f\n".
                           "    JOIN dll d\n".
                           "    LEFT JOIN state s ON f.state = s.refcode\n".
                           "WHERE\n".
                           "    fa.author = %d AND\n".
                           "    fa.function = f.refcode AND \n".
                           "    f.dll = d.refcode\n",
                           $iRefcode);
            if ($fSortByState)
                $sql = $sql."ORDER BY s.refcode, f.name, d.name";
            else
                $sql = $sql."ORDER BY d.name, f.name";
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=30%><font size=-1><b>Dll Name</b></font></td>\n".
                         "  <td width=45%><font size=-1><b>Function Name</b></font></td>\n".
                         "  <td><font size=-1><b>State</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?dllrefcode=".$aFunction["dllrefcode"]."\">".$aFunction["dllname"]."</a></font></td>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?functionrefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td><font size=-1 color=\"#".$aFunction["color"]."\">".$aFunction["state"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n".
                         "<p>Click <a href=\"Odin32DB.phtml#function?authorrefcode=".$iRefcode."&fFunctions=1";
                    if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
                    if ($fFiles)        echo "&fAuthors=".$fFiles;
                    if ($fSortByState)  echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted alphabetical by dll.<br>\n";
                    else                echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted by state.<br>\n";
                }
                else
                    echo "<i>No functions found</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#functions?authorrefcode=".$iRefcode."&fFunctions=1";
            if ($fDlls)         echo "&fDlls=".$fDlls;
            if ($fFiles)        echo "&fAuthors=".$fFiles;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all functions.\n";
        }


        /*
         * Files
         */
        Odin32DBNaslov($aContent, "Files", "files");
        if ($fFiles)
        {
            //TODO: OPTMIZE THIS SQL!!!
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    COUNT(fn.refcode)   AS functions\n".
                           "FROM\n".
                           "    fnauthor fa,\n".
                           "    file f\n".
                           "    LEFT OUTER JOIN function fn\n".
                           "        ON fn.file = f.refcode\n".
                           "WHERE\n".
                           "    fa.author = %d AND fa.function = fn.refcode\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                            $iRefcode);
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td><font size=-1><b>Filename</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td width=75%><font size=-1><a href=\"Odin32DB.phtml?filerefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>No Files.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#files?authorrefcode=".$iRefcode."&fFiles=1";
            if ($fDlls)         echo "&fDlls=".$fDlls;
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all files.\n";
        }


        /*
         * API Groups
         */
        if ($cAPIGroups > 0)
        {
            Odin32DBNaslov($aContent, "API Groups", "apigroups");
            if ($fAPIGroups)
            {
                $sql = sprintf("SELECT\n".
                               "    g.name              AS name,\n".
                               "    g.refcode           AS refcode,\n".
                               "    COUNT(f.refcode)    AS functions\n".
                               "FROM\n".
                               "    fnauthor fa\n".
                               "    JOIN apigroup g\n".
                               "    JOIN function f\n".
                               "WHERE\n".
                               "    fa.author = %d AND\n".
                               "    fa.function = f.refcode AND\n".
                               "    f.apigroup = g.refcode\n".
                               "GROUP BY f.apigroup\n".
                               "ORDER BY g.name\n",
                               $iRefcode);
                if ($result2 = mysql_query($sql, $db))
                {
                    if (mysql_num_rows($result2) > 0)
                    {
                        echo "\n<table width=100% border=0 cellpadding=0>\n".
                             "<tr>\n".
                             "  <td width=75%><font size=-1><b>Group Name</b></font></td>\n".
                             "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                             "</tr>\n";
                        while ($aFunction = mysql_fetch_array($result2))
                        {
                            echo "<tr>\n".
                                 "  <td><font size=-1><a href=\"Odin32DB.phtml?apigrouprefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                                 "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                                 "</tr>\n";
                        }
                        echo "\n</table>\n";
                    }
                    else
                        echo "<i>Not API Groups found.</i><br>\n";
                }
                else
                    Odin32DBSqlError($sql);
            }
            else
            {
                echo "Click <a href=\"Odin32DB.phtml#apigroups?authorrefcode=".$iRefcode."&fAPIGroups=1";
                if ($fDlls)         echo "&fDlls=".$fDlls;
                if ($fFunctions)    echo "&fFunctions=".$fFunctions;
                if ($fFiles)        echo "&fFiles=".$fFiles;
                if ($fSortByState)  echo "&fSortByState=".$fSortByState;
                echo "\">here</a> to see all the API Groups.\n";
            }
        }
    }
    else
    {
        echo "<p> No data! Invalid refcode? </p>";
        Odin32DBSqlError($sql);
    }

    /*
     * Navigation - Bottom
     */
    Odin32DBNavigationBottom($sExpand, $sCollapse);
}



/**
 * Writes standard file info.
 *
 * @returns     void
 * @param       $aContent       Contents array. (input/output)
 * @param       $db             Database handle.
 * @param       $iRefcode       Author reference code.
 * @param       $fFunctions     Flags which tells wether to list all functions or not.
 * @param       $fFiles         Flags which tells wether to list all files or not.
 * @param       $fAuthors       Flags which tells wether to list all authors or not.
 * @param       $fSortByState   Flags which tells wether to sort functions by
 *                              state and function name or just by function name.
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBAPIGroupInfo(&$aContent, $db, $iRefcode, $fFunctions, $fFiles, $fAuthors, $fSortByState)
{
    /*
     * Navigation - TOP
     */
    $sExpand    = "apigrouprefcode=".$iRefcode."&fFiles=1&fFunctions=1&fAuthors=1";
    if ($fSortByState)  $sExpand = $sExpand."&fSortByState=".$fSortByState;
    $sCollapse  = "apigrouprefcode=".$iRefcode;
    Odin32DBNavigationTop($sExpand, $sCollapse);

    /*
     * Fetch (vital) data.
     */
    $sql = sprintf("SELECT\n".
                   "    g.name          AS name,\n".
                   "    g.refcode       AS refcode,\n".
                   "    g.description   AS description,\n".
                   "    d.name          AS dllname,\n".
                   "    d.refcode       AS dllrefcode\n".
                   "FROM\n".
                   "    apigroup g\n".
                   "    JOIN dll d\n".
                   "WHERE\n".
                   "    g.refcode = %d AND".
                   "    g.dll = d.refcode\n",
                   $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov($aContent, "General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("Dll", $array, "dllname","dllrefcode","dllrefcode","bad configuration","");
        if (isset($array["description"]))
            Odin32DBInfoRow1("Description", $array, "description","","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    apigroup = %d",
                       $iRefcode);
        $cFunctions = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($aFunctions = mysql_fetch_array($result2)))
            {
            Odin32DBInfoRow1("# Functions", $aFunctions, "functions","","","","");
            $cFunctions = $aFunctions["functions"];
            }
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    apigroup = %d AND".
                       "    file >= 0\n".
                       "GROUP BY file",
                       $iRefcode);
        $cFiles = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cFiles = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# Files", $cFiles, "","","","");

        $sql = sprintf("SELECT\n".
                       "    COUNT(f.dll) as functions\n".
                       "FROM\n".
                       "    fnauthor fa,\n".
                       "    function f\n".
                       "WHERE\n".
                       "    f.apigroup = %d AND".
                       "    f.refcode = fa.function\n".
                       "GROUP BY fa.author",
                       $iRefcode);
        $cAuthors = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cAuthors = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# Authors", $cAuthors, "","","","");

        echo "\n</table>\n";

        /*
         * Completion
         */
        Odin32DBNaslov($aContent, "Completion", "completion");
        Odin32DBCompletionBarAPIGroup($iRefcode, "", $db);

        /*
         * States
         */
        Odin32DBNaslov($aContent, "Status", "status");
        $sql = sprintf("SELECT\n".
                       "    s.name              AS state,\n".
                       "    s.color             AS color,\n".
                       "    COUNT(f.state)      AS functions\n".
                       "FROM\n".
                       "    state s\n".
                       "    LEFT OUTER JOIN function f ON s.refcode = f.state AND f.apigroup = %d\n".
                       "GROUP BY s.refcode\n".
                       "ORDER BY s.refcode",
                        $iRefcode);
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n";
            while ($aState = mysql_fetch_array($result2))
            {
                echo "<tr>\n".
                     "    <td width=75%><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["state"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".$aState["functions"]."</b></font></td>\n".
                     "    <td align=right><font size=-1 color=\"#".$aState["color"]."\"><b>".@(int)((int)$aState["functions"] * 100 / $cFunctions)."%</b></font></td>\n".
                     "</tr>\n";
            }

            echo "\n</table>\n";
        }
        else
            Odin32DBSqlError($sql);

        /*
         * Functions
         */
        Odin32DBNaslov($aContent, "Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    s.name              AS state,\n".
                           "    s.color             AS color\n".
                           "FROM\n".
                           "    function f\n".
                           "    LEFT JOIN state s ON f.state = s.refcode\n".
                           "WHERE\n".
                           "    f.apigroup = %d\n",
                           $iRefcode);
            if ($fSortByState)
                $sql = $sql."ORDER BY s.refcode, f.name";
            else
                $sql = $sql."ORDER BY f.name";
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Function Name</b></font></td>\n".
                         "  <td><font size=-1><b>State</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?functionrefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td><font size=-1 color=\"#".$aFunction["color"]."\">".$aFunction["state"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n".
                         "<p>Click <a href=\"Odin32DB.phtml#function?apigrouprefcode=".$iRefcode."&fFunctions=1";
                    if ($fAuthors)      echo "&fAuthors=".$fAuthors;
                    if ($fFiles)        echo "&fAuthors=".$fFiles;
                    if ($fSortByState)  echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted alphabetical.<br>\n";
                    else                echo "&fSortByState=".!$fSortByState."\">here</a> to view functions sorted by state.<br>\n";
                }
                else
                    echo "<i>No functions found</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#functions?apigrouprefcode=".$iRefcode."&fFunctions=1";
            if ($fFiles)        echo "&fAuthors=".$fFiles;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all functions.\n";
        }


        /*
         * Files
         */
        Odin32DBNaslov($aContent, "Files", "files");
        if ($fFiles)
        {
            $sql = sprintf("SELECT\n".
                           "    f.name              AS name,\n".
                           "    f.refcode           AS refcode,\n".
                           "    COUNT(fn.refcode)   AS functions\n".
                           "FROM\n".
                           "    file f\n".
                           "    LEFT OUTER JOIN function fn ON fn.file = f.refcode\n".
                           "WHERE\n".
                           "    fn.apigroup = %d\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                            $iRefcode);

            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td><font size=-1><b>Filename</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td width=75%><font size=-1><a href=\"Odin32DB.phtml?filerefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>No Files.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#files?apigrouprefcode=".$iRefcode."&fFiles=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all files.\n";
        }


        /*
         * Authors
         */
        Odin32DBNaslov($aContent, "Authors", "authors");
        if ($fAuthors)
        {
            $sql = sprintf("SELECT\n".
                           "    a.name              AS name,\n".
                           "    a.refcode           AS refcode,\n".
                           "    COUNT(f.refcode)    AS functions\n".
                           "FROM\n".
                           "    fnauthor fa\n".
                           "    JOIN function f\n".
                           "    JOIN author a\n".
                           "WHERE\n".
                           "    f.apigroup = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    fa.author = a.refcode\n".
                           "GROUP BY a.refcode\n".
                           "ORDER BY a.name\n",
                            $iRefcode
                            );
            if ($result2 = mysql_query($sql, $db))
            {
                if (mysql_num_rows($result2) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Author</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?authorrefcode=".$aFunction["refcode"]."&apigroup=".$iRefcode."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    echo "<i>Not authors found.</i><br>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml#authors?apigrouprefcode=".$iRefcode."&fAuthors=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
            if ($fFiles)        echo "&fFiles=".$fFiles;
            if ($fSortByState)  echo "&fSortByState=".$fSortByState;
            echo "\">here</a> to see all authors.\n";
        }
    }
    else
    {
        echo "<p> No data! Invalid refcode? </p>";
        Odin32DBSqlError($sql);
    }

    /*
     * Navigation - Bottom
     */
    Odin32DBNavigationBottom($sExpand, $sCollapse);
}





/* TEXT FORMATTING OVERLOADS */
/* TEXT FORMATTING OVERLOADS */
/* TEXT FORMATTING OVERLOADS */

/**
 * Makes the contents for this page.
 * @sketch      Writes the headers present in the contents array.
 */
function Odin32DBWriteContents(&$aContent)
{
    TocBeg();
    for ($i = 0; $i < sizeof($aContent); $i += 2)
        AnchNaslov($aContent[$i], $aContent[$i + 1], "");
    TocEnd();
}

/**
 * Forwarder which also maintains the contents array.
 */
function Odin32DBNaslov(&$aContent, $sFull, $sShort)
{
    $aContent[] = $sFull;
    $aContent[] = $sShort;
    return Naslov($sFull, $sShort);
}



?>

