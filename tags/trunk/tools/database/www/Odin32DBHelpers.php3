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
        <tr>
            <td width=90%>
                <font size=-1 color=000099>
                <tt>".$sDllName."</tt>
                </font>
            </td>
            <td width=10%></td>
        </tr>
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
function Odin32DBDumpSql($sql)
{
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

?>

