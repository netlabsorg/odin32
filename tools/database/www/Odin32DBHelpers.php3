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
    $result = mysql_query($sql, $db);
    if (mysql_num_rows($result) < 1)
    {
        echo "<br>Odin32DBComputeCompletion: IPE no. 1 <br>";
        return -1;
    }
    $row = mysql_fetch_row($result);
    $iComplete = $row[0];
    mysql_free_result($result);
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
function Odin32DBCompletionBar($iDll, $sDllName, $db)
{
    /*
     * Count the total number of functions in the DLL.
     */
    if ($iDll < 0)
        $sql = "SELECT COUNT(*) FROM function f";
    else
        $sql = sprintf("SELECT COUNT(*) FROM function f WHERE dll = %d", $iDll);
    $result = mysql_query($sql, $db);
    if (mysql_num_rows($result) < 1)
    {
        printf("\n\n<br>Odin32DBCompletionBar: IPE(1).<br>\n\n");
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
    if ($sDllName != '')
        echo "
        <tr>
            <td width=90%>
                <font size=-1 color=000099>
                <tt>".$sDllName."</tt>
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
    if ($iDll < 0)
        $sDllCond = "";
    else
        $sDllCond = "dll = %d AND ";
    $sql = sprintf("SELECT  COUNT(f.refcode)    AS count,
                            f.state             AS state,
                            s.color             AS color,
                            s.weight            AS weight
                        FROM
                            function f,
                            state s
                        WHERE
                            ".$sDllCond."
                            s.refcode = f.state
                        GROUP BY f.state
                        ORDER BY state",
                   $iDll);
    $result = mysql_query($sql, $db);
    $rdCompletePercent = 0.0;
    if (@mysql_num_rows($result) < 1)
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

    $result = mysql_query($sql, $db);
    if (mysql_num_rows($result) < 1)
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
function Odin32DBFunctionInfoAll(&$aContent, $db, $iRefcode)
{
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
    $result = mysql_query($sql, $db);
    if (@mysql_num_rows($result) > 0 &&
        ($array = mysql_fetch_array($result))
        )
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
        if (isset($array["aliasrefcode"]))
            Odin32DBInfoRow2("Forwards", $array, "aliasdllname", "aliasdllrefcode", "dllrefcode","",".",
                                                 "aliasname", "aliasrefcode", "functionrefcode");
        if (isset($array["apigroup"]))
            Odin32DBInfoRow1("API Group", $array, "apigroupname", "apigrouprefcode", "apigrouprefcode","","");
        Odin32DBInfoRow1("File", $array, "filename", "filerefcode", "filerefcode","not available","");
        echo "\n</table>\n";


        Odin32DBNaslov($aContent, "Declaration", "declaration");
        echo "<pre>\n";
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
        $result2 = mysql_query($sql, $db);
        if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
        if ($result2 &&
            mysql_num_rows($result2) > 0 &&
            mysql_data_seek($result2, 0)
            )
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
        Odin32DBDocRow($aContent, "Remark", "remark", $array, "remark");

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
        $result2 = mysql_query($sql, $db);
        if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
        {
            while ($author = mysql_fetch_array($result2))
                echo "<a href=\"Odin32DB.phtml?authorrefcode=".$author["refcode"]."\">".$author["name"]."</a><br>\n";
        }
        else
            echo "Hmm. Seems noone wrote this function...<br>\n";
    }
    else
    {
        echo "<p> No data! Invalid refcode? </p>";
        Odin32DBSqlError($sql);
    }
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
function Odin32DBDllInfoAll(&$aContent, $db, $iRefcode, $fFunctions, $fFiles, $fAPIGroups, $fAuthors, $fSortByState)
{


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
    $result = mysql_query($sql, $db);
    if (@mysql_num_rows($result) > 0 &&
        ($array = mysql_fetch_array($result))
        )
    {
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
        $result = mysql_query($sql, $db);
        if (@mysql_num_rows($result) > 0 &&
            ($array = mysql_fetch_array($result))
            )
            {
            Odin32DBInfoRow1("# Functions", $array, "functions","","","","");
            $cFunctions = $array["functions"];
            }
        else
            $cFunctions = 0;

        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as files\n".
                       "FROM\n".
                       "    file\n".
                       "WHERE\n".
                       "    dll = %d",
                       $iRefcode);
        $result = mysql_query($sql, $db);
        if (@mysql_num_rows($result) > 0 &&
            ($array = mysql_fetch_array($result))
            )
        {
            Odin32DBInfoRow1("# Source files", $array, "files","","","","");
            $cFiles = $array["files"];
        }
        else
            $cFiles = 0;

        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as apigroups\n".
                       "FROM\n".
                       "    function c\n".
                       "WHERE\n".
                       "    f.dll = %d",
                        $iRefcode);
        $result = mysql_query($sql, $db);
        if (@mysql_num_rows($result) > 0 &&
            ($array = mysql_fetch_array($result))
            )
        {
            Odin32DBInfoRow1("# API Groups", $array, "apigroups","","","","");
            $cAPIGroups = $array["apigroups"];
        }
        else
            $cAPIGroups = 0;

        echo "\n</table>\n";


        /*
         * Completion
         */
        Odin32DBNaslov($aContent, "Completion", "completion");
        Odin32DBCompletionBar($iRefcode, "", $db);

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
        $result2 = mysql_query($sql, $db);
        if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fFunctions=1";
            if ($fFiles)        echo "&fFiles=".$fFiles;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)
                echo "&fSortByState=".!$fSortByState."\">here</a> to view files sorted alphabetical.<br>\n";
            else
                echo "&fSortByState=".!$fSortByState."\">here</a> to view files sorted by state.<br>\n";

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
            $result2 = mysql_query($sql, $db);
            if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
                echo "\n</table>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fFunctions=1";
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
                           "    COUNT(f.refcode)    AS functions\n".
                           "FROM\n".
                           "    file f\n".
                           "    LEFT OUTER JOIN function fn ON fn.file = f.refcode\n".
                           "WHERE\n".
                           "    f.dll = %d\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                            $iRefcode);
            $result2 = mysql_query($sql, $db);
            if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fFiles=1";
            if ($fFunctions)    echo "&fFunctions=".$fFunctions;
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
                               "    LEFT OUTER JOIN function fn ON fn.apigroup = g.refcode\n".
                               "WHERE\n".
                               "    g.dll = %d\n".
                               "GROUP BY g.refcode\n".
                               "ORDER BY g.name\n",
                                $iRefcode);
                $result2 = mysql_query($sql, $db);
                if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Group Name</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?filerefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    Odin32DBSqlError($sql);
            }
            else
            {
                echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fAPIGroups=1";
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
            $result2 = mysql_query($sql, $db);
            if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fAuthors=1";
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
function Odin32DBFileInfoAll(&$aContent, $db, $iRefcode, $fFunctions, $fAPIGroups, $fAuthors, $fSortByState)
{

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
    $result = mysql_query($sql, $db);
    if (@mysql_num_rows($result) > 0 &&
        ($array = mysql_fetch_array($result))
        )
    {
        Odin32DBNaslov($aContent, "General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("Revision", $array, "revision","","","","");
        Odin32DBInfoRow1("Changed", $array, "lastdatetime","","","","");
        Odin32DBInfoRow1("Last Author", $array, "lastauthorname","lastauthorrefcode","authorrefcode","","");
        Odin32DBInfoRow1("Dll", $array, "dllname","dllrefcode","dllrefcode","","");
        Odin32DBInfoRow1("Description", $array, "description","","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    file = %d",
                       $iRefcode);
        $result = mysql_query($sql, $db);
        if (@mysql_num_rows($result) > 0 &&
            ($array = mysql_fetch_array($result))
            )
            {
            Odin32DBInfoRow1("# Functions", $array, "functions","","","","");
            $cFunctions = $array["functions"];
            }
        else
            $cFunctions = 0;

        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as apigroups\n".
                       "FROM\n".
                       "    function c\n".
                       "WHERE\n".
                       "    f.file = %d\n",
                        $iRefcode);
        $result = mysql_query($sql, $db);
        if (@mysql_num_rows($result) > 0 &&
            ($array = mysql_fetch_array($result))
            )
        {
            Odin32DBInfoRow1("# API Groups", $array, "apigroups","","","","");
            $cAPIGroups = $array["apigroups"];
        }
        else
            $cAPIGroups = 0;

        echo "\n</table>\n";


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
        $result2 = mysql_query($sql, $db);
        if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fFunctions=1";
            if ($fFiles)        echo "&fFiles=".$fFiles;
            if ($fAPIGroups)    echo "&fAPIGroups=".$fAPIGroups;
            if ($fAuthors)      echo "&fAuthors=".$fAuthors;
            if ($fSortByState)
                echo "&fSortByState=".!$fSortByState."\">here</a> to view files sorted alphabetical.<br>\n";
            else
                echo "&fSortByState=".!$fSortByState."\">here</a> to view files sorted by state.<br>\n";

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
            $result2 = mysql_query($sql, $db);
            if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
                echo "\n</table>\n";
            }
            else
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fFunctions=1";
            if ($fFiles)        echo "&fFiles=".$fFiles;
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
                               "    LEFT OUTER JOIN function fn ON fn.apigroup = g.refcode AND fn.file = %d\n".
                               "GROUP BY g.refcode\n".
                               "ORDER BY g.name\n",
                                $iRefcode);
                $result2 = mysql_query($sql, $db);
                if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
                {
                    echo "\n<table width=100% border=0 cellpadding=0>\n".
                         "<tr>\n".
                         "  <td width=75%><font size=-1><b>Group Name</b></font></td>\n".
                         "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                         "</tr>\n";
                    while ($aFunction = mysql_fetch_array($result2))
                    {
                        echo "<tr>\n".
                             "  <td><font size=-1><a href=\"Odin32DB.phtml?filerefcode=".$aFunction["refcode"]."\">".$aFunction["name"]."</a></font></td>\n".
                             "  <td align=right><font size=-1>".$aFunction["functions"]."</font></td>\n".
                             "</tr>\n";
                    }
                    echo "\n</table>\n";
                }
                else
                    Odin32DBSqlError($sql);
            }
            else
            {
                echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fAPIGroups=1";
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
                           "    f.file      = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    fa.author   = a.refcode\n".
                           "GROUP BY a.refcode\n".
                           "ORDER BY a.name\n",
                            $iRefcode
                            );
            $result2 = mysql_query($sql, $db);
            if ($result2 && ($cParams = mysql_num_rows($result2)) > 0)
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
                Odin32DBSqlError($sql);
        }
        else
        {
            echo "Click <a href=\"Odin32DB.phtml?dllrefcode=".$iRefcode."&fAuthors=1";
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
}







/* TEXT FORMATTING OVERLOADS */
/* TEXT FORMATTING OVERLOADS */
/* TEXT FORMATTING OVERLOADS */

/**
 * Makes the contents for this page.
 * @sketch      Writes the headers present in the contents array.
 */
function DBWriteContents(&$aContent)
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

