<?php

/**
 * Compute completion percentage for a dll.
 * @returns     Completion percentage. Range 0-100.
 *              On error -1 or -2 is returned.
 * @param       $ulDll  Dll reference code.
 * @param       $db     Database connection variable.
 * @sketch      Get total number of function in the dll.
 *              Get number of completed functions in the dll.
 *              return complete*100 / total
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBComputeCompletion($ulDll, $db)
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
                        $ulDll);
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
 * @param       $ulDll  Dll reference code.
 * @param       $db     Database connection variable.
 * @sketch      Get total number of function in the dll.
 *              Get the number of function per status. (+state color)
 *              Draw bar.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBar($ulDll, $sDllName, $db)
{
    /*
     * Count the total number of functions in the DLL.
     */
    $sql = sprintf("SELECT COUNT(*) FROM function f WHERE dll = %d",
                        $ulDll);
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
    $sql = sprintf("SELECT  COUNT(f.refcode)    AS count,
                            f.state             AS state,
                            s.color             AS color,
                            s.weight            AS weight
                        FROM
                            function f,
                            state s
                        WHERE
                            dll = %d
                            AND s.refcode = f.state
                        GROUP BY f.state
                        ORDER BY state",
                   $ulDll);
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
        printf("\n\n<br>Odin32DBCompletionBar: IPE(1).<br>\n\n");
        return -1;
    }
    else
    {
        echo "
            <tr><td></td></tr>
            <tr>
                    <td valign=center >
                        <center><B><font face=\"WarpSans, Arial\" color=\"#990000\">
                        Legend:
                        </font></b></center>
                    </td>
            </tr>
            <tr>
                <table width=100% border=0 cellspacing=10 cellpadding=0 align=right>
            ";
        while ($row = mysql_fetch_row($result))
        {
            echo "
                <tr>
                    <td width=90% align=right>
                        <font size=1 color=000099>
                        ".$row[0]."
                        </font>
                    </td>
                    <td width=1%>
                        <font size=1 color=000099>
                        &nbsp;
                        </font>
                    </td>
                    <td width=9% bgcolor=".$row[1].">
                        <font size=1 color=000099>
                        &nbsp;
                        </font>
                    </td>
                </tr>
                ";
        }

        echo "
                </table>
                </p>
            </tr>
            ";
    }


    mysql_free_result($result);
}



?>
