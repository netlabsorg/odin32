<?php

/**
 * Profiling function for mysql queries.
 * @returns     same as mysql_query
 * @param       $sSql   SQL statement.
 * @param       $db     Database connection.
 * @sketch
 *              Get time.
 *              Execute query.
 *              Get time.
 *              Calc time ellapsed.
 *              Start a HTML comment.
 *              Log time and sql statement in the HTML comment.
 *              EXPLAIN sql statement and log it in the HTML comment.
 *              End the HTML comment.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function _mysql_query($sSql, $db)
{
    $sMsStart = microtime();

    $Result = mysql_query($sSql, $db);

    $sMsEnd = microtime();

    /*
     * Format: 0.mmmmmmmm sssssssss
     *         0.00322252 965056018
     *
     *         01234567890123456789
     *         0         1
     */
    $sMsStart = substr($sMsStart,11) . substr($sMsStart,1,9);
    $sMsEnd   = substr($sMsEnd,11)   . substr($sMsEnd,1,9);
    $sMsTime  = (double)$sMsEnd - (double)$sMsStart;

    echo
        "\n<!-- start: $sMsStart  end: $sMsEnd  time: $sMsTime".
        "\nSQL:\n".
        $sSql.
        "\n".
        "rows: ". mysql_num_rows($Result) ."\n";

    $ResultExplain = mysql_query("EXPLAIN ".$sSql, $db);
    if ($ResultExplain)
    {
        echo "Explain:\n";
        printf("%-15s  %-17  %-20s  %-12s  %-10s  %-20s  %-8s  %s\n",
            "Table",
            "Type",
            "Possible keys",
            "Key",
            "Key length",
            "Ref",
            "Rows",
            "Extra");

        while ($aExplain = mysql_fetch_array($ResultExplain))
        {
            printf("%-15s  %-17  %-20s  %-12s  %-10s  %-20s  %-8s  %s\n",
                isset($aExplain["0"]) ? $aExplain["0"] : "NULL",
                isset($aExplain["1"]) ? $aExplain["1"] : "NULL",
                isset($aExplain["2"]) ? $aExplain["2"] : "NULL",
                isset($aExplain["3"]) ? $aExplain["3"] : "NULL",
                isset($aExplain["4"]) ? $aExplain["4"] : "NULL",
                isset($aExplain["5"]) ? $aExplain["5"] : "NULL",
                isset($aExplain["6"]) ? $aExplain["6"] : "NULL",
                isset($aExplain["7"]) ? $aExplain["7"] : "NULL");
        }
        mysql_free_result($ResultExplain);
    }
    else
        echo "\nexplain failed\n";

    echo "-->\n";

    return $Result;
}


/**
 * Starts a timer. Writes a nag in the file.
 * @returns
 * @param       $sText  Log text.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBTimerStart($sText)
{
    $sMsStart = microtime();
    $sMsStart = substr($sMsStart,11) . substr($sMsStart,1,9);
    echo "\n<!-- Timer started: $sMsStart  $sText -->\n";
    return $sMsStart.$sText;
}


/**
 * Stops the timer $sMsStart. And writes the elapsed time.
 * @returns     Nothing.
 * @param       $sTimer   The timer handle returned by Odin32DBTimerStart.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBTimerStop($sTimer)
{
    $sMsEnd  = microtime();
    $sMsEnd  = substr($sMsEnd,11) . substr($sMsEnd,1,9);
    $sMsStart= substr($sTimer,0,18);
    $sMsTime = (double)$sMsEnd - (double)$sMsStart;
    $sText   = substr($sTimer,18);
    echo "\n<!-- Timer stopped: $sMsEnd ($sMsStart)  Elapsed: $sMsTime  $sText -->\n";
}


/**
 * Compute completion percentage for a module.
 * @returns     Completion percentage. Range 0-100.
 *              On error -1 or -2 is returned.
 * @param       $iModule    Module reference code.
 * @param       $db         Database connection variable.
 * @sketch      Get total number of function in the module.
 *              Get number of completed functions in the module.
 *              return complete*100 / total
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBComputeCompletion($iModule, $db)
{
    /*
     * Count the total number of functions in the module.
     */
    $sql = sprintf("SELECT  SUM(s.weight)/COUNT(f.state)
                        FROM
                            function f,
                            state s
                        WHERE
                            f.state = s.refcode
                            AND module = %d",
                        $iModule);
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
 * Draws a completion bar for a module (or all modules.).
 * @param       $iModule    Module reference code.
 *                          If < 0 then for the entire project.
 * @param       $iModName   Module name.
 * @param       $db         Database connection variable.
 * @sketch      Call Odin32DBCompletionBar2 with an appropriate condition.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBarModule($iModule, $sModName, $db)
{
    if ($iModule < 0)
        return Odin32DBcompletionBar2("", $sModName, $db);
    return Odin32DBcompletionBar2("module = ".$iModule." AND f.type IN ('A', 'I')", $sModName, $db);
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
     * Count the total number of functions for the author.
     */
    $sql = "SELECT COUNT(*) FROM fnauthor fa JOIN function f\n".
           "WHERE fa.function = f.refcode AND fa.author = ".$iAuthor." AND f.type IN ('A', 'I')";
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
    echo "\n<table width=100% border=0 cellspacing=0 cellpadding=0>";
    if ($sAuthorName != '')
        echo "  <tr><td width=90%><font size=-1 color=#000099><tt>".$sAuthorName."</tt></font></td><td width=10%></td></tr>\n";
    echo "  <tr><td width=90%>\n".
         "  <table width=100% border=0 cellspacing=0 cellpadding=0>\n".
         "  <tr>\n";

    /*
     * Get states and make bar.
     */
    $sql = "SELECT\n".
           "    COUNT(f.refcode)    AS count,\n".
           "    s.refcode           AS state,\n".
           "    s.color             AS color,\n".
           "    s.weight            AS weight\n".
           "FROM\n".
           "    fnauthor fa,\n".
           "    function f,\n".
           "    state s\n".
           "WHERE\n".
           "    fa.author = ".$iAuthor." AND\n".
           "    fa.function = f.refcode AND\n".
           "    f.state = s.refcode AND\n".
           "    f.type IN ('A', 'I')\n".
           "GROUP BY s.refcode\n".
           "ORDER BY state\n";
    $rdCompletePercent = 0.0;
    if (!($result = mysql_query($sql, $db)))
        Odin32DBSqlError($sql);
    else if (mysql_num_rows($result) < 1)
        echo "  <td colspan=2 bgcolor=#dddddd><font size=-1>&nbsp;</font></td>\n";
    else
    {
        while ($row = mysql_fetch_row($result))
        {
            $iPercent = (int)($row[0] * 90 /* 10% is reserved to % */ / $cFunctions);
            if ($iPercent == 0)
                $iPercent = 1;
            echo "  <td width=".$iPercent." bgcolor=".$row[2]."><font size=-1>&nbsp;</font></td>\n";

            $rdCompletePercent += ((double)$row[3] * (double)$row[0]) / $cFunctions;
        }

    }
    mysql_free_result($result);

    /*
     * Complete bar with a total completion percent.
     */
    echo "  <td width=10% align=right><font size=-1 color=#000099>".(int)$rdCompletePercent."%</font></td>\n".
         "  </tr>\n".
         "  </table>\n".
         "</td></tr>\n".
         "</table>\n";
}

/**
 * Draws a completion bar.
 * @param       $sCondition Where conditions.
 * @param       $sName      Name...
 * @param       $db         Database connection variable.
 * @sketch      Get total number of function in the selection.
 *              Get the number of function per status. (+state color)
 *              Draw bar.
 * @status      Completely implemented
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBCompletionBar2($sCondition, $sName, $db)
{
    /*
     * Count the total number of functions in selection.
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
     * Double table. Eventually write the name.
     */
    echo "\n<table width=100% border=0 cellspacing=0 cellpadding=0>\n";
    if ($sName != "")
        echo "<tr><td width=90%><font size=-1 color=#000099><tt>".$sName."</tt></font></td><td width=10%></td></tr>\n";
    echo "<tr><td width=90%>\n".
         "  <table width=100% border=0 cellspacing=0 cellpadding=0>\n".
         "  <tr>\n";

    /*
     * Get states and make bar.
     */
    if ($sCondition != "")  $sCondition = "f.".$sCondition." AND";
    $sql = "SELECT\n".
           "    COUNT(f.refcode)    AS count,\n".
           "    s.refcode           AS state,\n".
           "    s.color             AS color,\n".
           "    s.weight            AS weight\n".
           "FROM\n".
           "    function f,\n".
           "    state s\n".
           "WHERE\n".
           "    ".$sCondition."\n".
           "    s.refcode = f.state\n".
           "GROUP BY s.refcode\n".
           "ORDER BY state\n";
    $rdCompletePercent = 0.0;
    if (!($result = mysql_query($sql, $db)))
        Odin32DBSqlError($sql);
    else if (mysql_num_rows($result) < 1)
        echo "  <td colspan=2 bgcolor=#dddddd><font size=-1>&nbsp;</font></td>\n";
    else
    {
        while ($row = mysql_fetch_row($result))
        {
            $iPercent = (int)($row[0] * 90 /* 10% is reserved to % */ / $cFunctions);
            if ($iPercent == 0)
                $iPercent = 1;
            echo "  <td width=".$iPercent." bgcolor=".$row[2]."><font size=-1>&nbsp;</font></td>\n";
            $rdCompletePercent += ((double)$row[3] * (double)$row[0]) / $cFunctions;
        }

    }
    mysql_free_result($result);

    /*
     * Complete bar with a total completion percent.
     */
    echo "  <td width=10% align=right><font size=-1 color=#000099>".(int)$rdCompletePercent."%</font></td>\n".
         "  </tr>\n".
         "  </table>\n".
         "</td></tr>\n".
         "</table>\n";
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
     * Select the states.
     */
    $sql = "SELECT name,
                   color
            FROM   state
            ORDER BY refcode";
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) < 1)
    {
        printf("\n\n<br>Odin32DBStateLegned: IPE(1).<br>\n\n");
        return -1;
    }
    else
    {
        echo "\n".
             "<tr><td></td></tr>\n".
             "<tr><td><center><B><font face=\"WarpSans, Arial\" color=\"#990000\">Status Legend:</font></b></center></td></tr>\n".
             "<tr><td>\n".
             "  <table width=100% border=0 cellspacing=2 cellpadding=0 align=right>\n";

        while ($row = mysql_fetch_row($result))
            echo "  <tr><td width=85% align=right><font size=1 color=#000099>".$row[0]."</font></td>\n".
                 "      <td width=15% bgcolor=".$row[1]."><font size=-1>&nbsp;<br>&nbsp;</font></td>\n".
                 "  </tr>\n";

        echo "  </table>\n".
             "</td></tr>\n";
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
    echo "  <tr>\n".
         "    <td width=35%><tt>".$sName."</tt></td>\n".
         "    <td valign=top>";
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

    echo "</td>\n".
         "  </tr>\n";
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
    echo "  <tr>\n".
         "    <td width=35%><tt>".$sName."</tt></td>\n".
         "    <td valign=top>";
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

    echo "</td>\n".
         "  </tr>\n";
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
    echo "  <tr>\n".
         "    <td width=35%><tt>".$sName."</tt></td>\n".
         "    <td valign=top>";
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

    echo "</td>\n".
         "  </tr>\n";
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
function Odin32DBDocRow($sName, $sLabel, $array, $sValueName)
{
    Odin32DBNaslov($sName, $sLabel);
    if (isset($array[$sValueName]))
    {
        echo $array[$sValueName];
    }
    else
        echo "<i>not available</i>";
}


/**
 * Writes a state section based on a sqlstatment returning the following values (ordered):
 *      0. state
 *      1. functions
 *
 * @returns     nothing.
 * @param       $cFunctions     Number of functions.
 * @param       $sql            Sql statement.
 * @param       $db             Database connection.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteStates($cFunctions, $sql, $db)
{
    Odin32DBNaslov("Status", "status");
    if ($result2 = mysql_query($sql, $db))
    {
        $result = mysql_query("SELECT refcode, name, color FROM state ORDER BY refcode", $db);
        if ($result)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n";

            $aState2 = mysql_fetch_array($result2);
            while ($aState = mysql_fetch_row($result))
            {
                if ($aState2 && $aState[0] == $aState2[0])
                {
                    $cStateFunctions = (int)$aState2[1];
                    $aState2 = mysql_fetch_array($result2);
                }
                else
                    $cStateFunctions = 0;
                printf("<tr>\n".
                       "    <td width=75%%><font size=-1 color=%s>%s</font></td>\n".
                       "    <td align=right><font size=-1 color=%s>%s</font></td>\n".
                       "    <td align=right><font size=-1 color=%s>%d%%</font></td>\n".
                       "</tr>\n",
                       $aState[2], $aState[1],
                       $aState[2], $cStateFunctions,
                       $aState[2], @(int)($cStateFunctions * 100 / $cFunctions));
            }

            echo "</table>\n";
        }
        else
            Odin32DBSqlError($sql);
    }
    else
        Odin32DBSqlError($sql);
}


/**
 * Writes the a function listing base sqlstatement with these columns (ordered):
 *      0. Module refcode
 *      1. Module name
 *      2. number of functions
 *
 * @returns     nothing
 * @param       $sql        SQL statement.
 * @param       $db         Database connection.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteModules($sql, $db)
{
    if ($result2 = mysql_query($sql, $db))
    {
        if (mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n".
                 "<tr>\n".
                 "  <td width=75%><font size=-1><b>Modules</b></font></td>\n".
                 "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                 "</tr>\n";
            while ($aFunction = mysql_fetch_array($result2))
                printf("<tr>".
                       "<td><font size=-1><a href=\"Odin32DB.phtml?modrefcode=%s\">%s</a></font></td>".
                       "<td align=right><font size=-1>%s</font></td>".
                       "</tr>\n",
                       $aFunction[0], $aFunction[1], $aFunction[2]);

            echo "</table>\n";
        }
        else
            echo "<i>No Files.</i><br>\n";
    }
    else
        Odin32DBSqlError($sql);
}


/**
 * Writes the a function listing base sqlstatement with these columns (ordered):
 *      0. function refcode
 *      1. function name
 *      2. state color
 *      3. state name
 *
 * @returns     nothing
 * @param       $sql        SQL statement.
 * @param       $db         Database connection.
 * @param       $sURLArgs   URL arguments.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteFunctions($sql, $db, $sURLArgs)
{
    if ($result2 = mysql_query($sql, $db))
    {
        if (mysql_num_rows($result2) > 0)
        {
            echo
                "<table width=100% border=0 cellpadding=0>\n",
                "<tr>\n",
                "  <td width=75%><font size=-1><b>Function Name</b></font></td>\n",
                "  <td><font size=-1><b>State</b></font></td>\n",
                "</tr>\n";

            while ($aFunction = mysql_fetch_row($result2))
                printf("<tr>".
                       "<td><font size=-1><a href=\"Odin32DB.phtml?functionrefcode=%s\">%s</a></font></td>".
                       "<td><font size=-1 color=%s>%s</font></td></tr>\n",
                       $aFunction[0], $aFunction[1], $aFunction[2], $aFunction[3]);

            echo "</table></font>\n";

            /*
             * Sort text.
             */
            $fSortByState = strstr($sURLArgs, "&fSortByState=1");
            if ($fSortByState)
                $sURLArgs = str_replace("&fSortByState=1", "", $sURLArgs);
            else
                $sURLArgs = $sURLArgs."&fSortByState=1";

            echo "<p>Click <a href=\"Odin32DB.phtml#functions?".$sURLArgs."\">here</a> to view functions sorted ".
                 ($fSortByState  ? "alphabetical" : "by state"). ".<br>";
        }
        else
            echo "<i>No Functions.</i><br>\n";
        mysql_free_result($result2);
    }
    else
        Odin32DBSqlError($sql);
}


/**
 * Writes the a function listing base sqlstatement with these columns (ordered):
 *      0. module refcode
 *      1. module name
 *      2. function refcode
 *      3. function name
 *      4. state color
 *      5. state name
 *
 * @returns     nothing
 * @param       $sql        SQL statement.
 * @param       $db         Database connection.
 * @param       $sURLArgs   URL arguments.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteFunctionsWithModules($sql, $db, $sURLArgs)
{
    if ($result2 = mysql_query($sql, $db))
    {
        if (mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n".
                 "<tr>\n".
                 "  <td width=30%><font size=-1><b>Module Name</b></font></td>\n".
                 "  <td width=45%><font size=-1><b>Function Name</b></font></td>\n".
                 "  <td><font size=-1><b>State</b></font></td>\n".
                 "</tr>\n";
            while ($aFunction = mysql_fetch_row($result2))
                printf("<tr>".
                       "<td><font size=-1><a href=\"Odin32DB.phtml?modrefcode=%s\">%s</a></font></td>".
                       "<td><font size=-1><a href=\"Odin32DB.phtml?functionrefcode=%s\">%s</a></font></td>".
                       "<td><font size=-1 color=%s>%s</font></td>".
                       "</tr>\n",
                       $aFunction[2], $aFunction[3],
                       $aFunction[0], $aFunction[1],
                       $aFunction[4], $aFunction[5]);

            echo "</table>\n";

            /*
             * Sort text.
             */
            $fSortByState = strstr($sURLArgs, "&fSortByState=1");
            if ($fSortByState)
                $sURLArgs = str_replace("&fSortByState=1", "", $sURLArgs);
            else
                $sURLArgs = $sURLArgs."&fSortByState=1";

            echo "<p>Click <a href=\"Odin32DB.phtml#functions?".$sURLArgs."\">here</a> to view functions sorted ".
                 ($fSortByState  ? "alphabetical by module" : "by state"). ".<br>";
        }
        else
            echo "<i>No functions found</i><br>\n";
    }
    else
        Odin32DBSqlError($sql);
}


/**
 * Writes the a file listing base sqlstatement with these columns (ordered):
 *      0. file refcode
 *      1. file name
 *      2. number of functions
 *
 * @returns     nothing
 * @param       $sql        SQL statement.
 * @param       $db         Database connection.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteFiles($sql, $db)
{
    if ($result2 = mysql_query($sql, $db))
    {
        if (mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n".
                 "<tr>\n".
                 "  <td><font size=-1><b>Filename</b></font></td>\n".
                 "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                 "</tr>\n";
            while ($aFile = mysql_fetch_row($result2))
                printf("<tr>".
                       "<td width=75%%><font size=-1><a href=\"Odin32DB.phtml?filerefcode=%s\">%s</a></font></td>".
                       "<td align=right><font size=-1>%s</font></td>".
                       "</tr>\n",
                       $aFile[0], $aFile[1], $aFile[2]);

            echo "</table>\n";
        }
        else
            echo "<i>No Files.</i><br>\n";
    }
    else
        Odin32DBSqlError($sql);
}


/**
 * Writes the an API Group listing base sqlstatement with these columns (ordered):
 *      0. apigroup refcode
 *      1. apigroup name
 *      2. number of functions
 *
 * @returns     nothing
 * @param       $sql        SQL statement.
 * @param       $db         Database connection.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteAPIGroups($sql, $db)
{
    if ($result2 = mysql_query($sql, $db))
    {
        if (mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n".
                 "<tr>\n".
                 "  <td width=75%><font size=-1><b>Group Name</b></font></td>\n".
                 "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                 "</tr>\n";
            while ($aAPIGroup = mysql_fetch_row($result2))
                printf("<tr>".
                       "<td><font size=-1><a href=\"Odin32DB.phtml?apigrouprefcode=%s\">%s</a></font></td>".
                       "<td align=right><font size=-1>%s</font></td>".
                       "</tr>\n",
                       $aAPIGroup[0], $aAPIGroup[1], $aAPIGroup[2]);
            echo "</table>\n";
        }
        else
            echo "<i>No API Groups.</i><br>\n";
        mysql_free_result($result2);
    }
    else
        Odin32DBSqlError($sql);
}



/**
 * Writes the a author listing base sqlstatement with these columns (ordered):
 *      0. author refcode
 *      1. author name
 *      2. number of functions
 *
 * @returns     nothing
 * @param       $sql        SQL statement.
 * @param       $db         Database connection.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
function Odin32DBWriteAuthors($sql, $db)
{
    if ($result2 = mysql_query($sql, $db))
    {
        if (mysql_num_rows($result2) > 0)
        {
            echo "\n<table width=100% border=0 cellpadding=0>\n".
                 "<tr>\n".
                 "  <td width=75%><font size=-1><b>Author</b></font></td>\n".
                 "  <td align=right><font size=-1><b>Functions</b></font></td>\n".
                 "</tr>\n";
            while ($aAuthor = mysql_fetch_row($result2))
                printf("<tr>".
                       "<td><font size=-1><a href=\"Odin32DB.phtml?authorrefcode=%s\">%s</a></font></td>".
                       "<td align=right><font size=-1>%s</font></td>".
                       "</tr>\n",
                       $aAuthor[0], $aAuthor[1], $aAuthor[2]);

            echo "</table>\n";
        }
        else
            echo "<i>No Authors</i>.<br>\n";
        mysql_free_result($result2);
    }
    else
        Odin32DBSqlError($sql);
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
         " - <a href=\"Odin32DB.phtml?modules=1\">Moduless</a>\n".
         " - <a href=\"Odin32DB.phtml?authors=1\">Authors</a>\n".
         " - <a href=\"Odin32DB.phtml?apigroups=1\">API Groups</a>\n";

    if ($sExpand != "" && $sCollapse != "")
    {
        echo "<br><a href=\"Odin32DB.phtml?".$sExpand."\">Expand All</a> - \n".
             "<a href=\"Odin32DB.phtml?".$sCollapse."\">Collapse All</a>\n";
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
        echo "<a href=\"Odin32DB.phtml?".$sExpand."\">Expand All</a> - \n".
             "<a href=\"Odin32DB.phtml?".$sCollapse."\">Collapse All</a><br>\n";
    }

    echo
         "<a href=\"Odin32DB.phtml\">Root</a>\n".
         " - <a href=\"Odin32DB.phtml?moduless=1\">Modules</a>\n".
         " - <a href=\"Odin32DB.phtml?authors=1\">Authors</a>\n".
         " - <a href=\"Odin32DB.phtml?apigroups=1\">API Groups</a>\n";
    echo "</font></center>\n";
}



/* INFO OUTPUT */
/* INFO OUTPUT */
/* INFO OUTPUT */


/**
 * Writes standard function info.
 *
 * @returns     void
 * @param       $db             Database handle.
 * @param       $iRefcode       Function reference code.
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBFunctionInfo($db, $iRefcode)
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
                   "    f.line          AS line,\n".
                   "    g.name          AS apigroupname,\n".
                   "    g.refcode       AS apigrouprefcode,\n".
                   "    a.name          AS aliasname,\n".
                   "    a.refcode       AS aliasrefcode,\n".
                   "    ad.name         AS aliasnmodname,\n".
                   "    ad.refcode      AS aliasmodrefcode,\n".
                   "    d.name          AS modname,\n".
                   "    d.refcode       AS modrefcode,\n".
                   "    i.name          AS filename,\n".
                   "    i.refcode       AS filerefcode,\n".
                   "    s.name          AS state,\n".
                   "    c.description   AS type\n".
                   "FROM\n".
                   "    function f\n".
                   "    LEFT OUTER JOIN function a ON f.aliasfn  = a.refcode\n".
                   "    LEFT OUTER JOIN module ad  ON a.module   = ad.refcode\n".
                   "    LEFT OUTER JOIN apigroup g ON f.apigroup = g.refcode\n".
                   "    LEFT       JOIN module d   ON f.module   = d.refcode\n".
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
        Odin32DBNaslov("General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        if (isset($array["intname"]))
            Odin32DBInfoRow1("Internal Name", $array, "intname","","","","");
        else
            Odin32DBInfoRow1("Internal Name", $array, "name","","","","");
        Odin32DBInfoRow1("Type", $array, "type", "", "","invalid","");
        Odin32DBInfoRow1("State", $array, "state", "", "","invalid","");
        Odin32DBInfoRow1("Module", $array, "modname", "modrefcode", "modrefcode","","");
        Odin32DBInfoRow1("Ordinal", $array, "ordinal","","","not available","");
        if (isset($array["apigroupname"]))
            Odin32DBInfoRow1("API Group", $array, "apigroupname", "apigrouprefcode", "apigrouprefcode","","");
        Odin32DBInfoRow1("File", $array, "filename", "filerefcode", "filerefcode", "not available",
                         " (<a href=\"cvs.phtml#".$array["line"]."?sFile=./src/".$array["modname"]."/".$array["filename"].",v&sRevision=\">cvs</a>)");
        if ($array["line"] > 0)
            Odin32DBInfoRow1("Line", $array, "line", "", "","","");
        else
            Odin32DBInfoRow1("Line", $array, "unavailable", "", "","not available","");
        if (isset($array["aliasrefcode"]))
            Odin32DBInfoRow2("Forwards", $array, "aliasmodname", "aliasmodrefcode", "modrefcode","",".",
                                                 "aliasname", "aliasrefcode", "functionrefcode");
        else
        {
            $sql = sprintf("SELECT\n".
                           "    m.name      AS modname,\n".
                           "    m.refcode   AS modrefcode,\n".
                           "    f.name      AS fnname,\n".
                           "    f.refcode   AS fnrefcode\n".
                           "FROM\n".
                           "    function f,\n".
                           "    module m\n".
                           "WHERE\n".
                           "    f.aliasfn = %d AND\n".
                           "    f.module = m.refcode\n".
                           "ORDER BY m.name, f.name\n",
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
                        $sValue = $sValue."<a href=\"Odin32DB.phtml?modrefcode=".
                                          $aAlias["modrefcode"]."\">".$aAlias["modname"]."</a>.".
                                          "<a href=\"Odin32DB.phtml?functionrefcode=".
                                          $aAlias["fnrefcode"]."\">".$aAlias["fnname"]."</a>";
                    }
                    Odin32DBInfoRow1NoArray("Forwarded as", $sValue, "","","","");
                }
            }
            else
                Odin32DBSqlError($sql);
        }
        echo "</table>\n";

        /*
         * Completion
         */
        Odin32DBNaslov("Completion", "completion");
        Odin32DBCompletionBarFunction($iRefcode, "", $db);


        /*
         * Declaration
         */
        Odin32DBNaslov("Declaration", "declaration");
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
        Odin32DBDocRow("Description", "desc", $array, "description");


        /*
         * Parameters
         */
        Odin32DBNaslov("Parameters", "params");
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
                        echo "</table>\n";
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
        Odin32DBDocRow("Returns", "return", $array, "returndesc");

        /*
         * Sketch/Algorithm
         */
        Odin32DBDocRow("Sketch/Algorithm", "sketch",  $array, "sketch");

        /*
         * Remark
         */
        Odin32DBDocRow("Remarks", "remark", $array, "remark");

        /*
         * Authors
         */
        Odin32DBNaslov("Authors", "Authors");
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
 * Writes standard module info.
 *
 * @returns     void
 * @param       $db             Database handle.
 * @param       $iRefcode       Module reference code.
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
function Odin32DBModuleInfo($db, $iRefcode, $fFunctions, $fFiles, $fAPIGroups, $fAuthors, $fSortByState)
{
    $sURLArgs = "modrefcode=".$iRefcode.
                ($fFunctions   ? "&fFunctions=1"   : "").
                ($fFiles       ? "&fFiles=1"       : "").
                ($fAPIGroups   ? "&fAPIGroups=1"   : "").
                ($fAuthors     ? "&fAuthors=1"     : "").
                ($fSortByState ? "&fSortByState=1" : "");

    /*
     * Navigation - TOP
     */
    $sExpand    = "modrefcode=".$iRefcode."&fFiles=1&fFunctions=1&fAPIGroups=1&fAuthors=1";
    if ($fSortByState)  $sExpand = $sExpand."&fSortByState=".$fSortByState;
    $sCollapse  = "modrefcode=".$iRefcode;
    Odin32DBNavigationTop($sExpand, $sCollapse);

    /*
     * Fetch (vital) data.
     */
    $sql = sprintf("SELECT\n".
                   "    m.name          AS name,\n".
                   "    m.description   AS description,\n".
                   "    c.description   AS type\n".
                   "FROM\n".
                   "    module d,\n".
                   "    code c\n".
                   "WHERE\n".
                   "    c.codegroup = 'DTYP' AND\n".
                   "    m.type = c.code AND\n".
                   "    m.refcode = %d",
                    $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov("General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("Description", $array, "description","","","","");
        Odin32DBInfoRow1("Type", $array, "type","","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(*) as functions\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    module = %d",
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
                       "    module = %d",
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
                       "    module = %d",
                       $iRefcode);
        $cAPIGroups = 0;
        if (($result2 = mysql_query($sql, $db)) && mysql_num_rows($result2) > 0 && ($array2 = mysql_fetch_array($result2)))
        {
            Odin32DBInfoRow1("# API Groups", $array2, "apigroups","","","","");
            $cAPIGroups = $array2["apigroups"];
        }

        echo "</table>\n";


        /*
         * Completion
         */
        Odin32DBNaslov("Completion", "completion");
        Odin32DBCompletionBarModule($iRefcode, "", $db);

        /*
         * States
         */
        $sql = sprintf("SELECT\n".
                       "    state,\n".
                       "    COUNT(state)\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    module = %d\n".
                       "GROUP BY state\n".
                       "ORDER BY state",
                        $iRefcode);
        Odin32DBWriteStates($cFunctions, $sql, $db);

        /*
         * Functions
         */
        Odin32DBNaslov("Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    s.color,\n".
                           "    s.name\n".
                           "FROM\n".
                           "    function f\n".
                           "    LEFT JOIN state s ON f.state = s.refcode\n".
                           "WHERE\n".
                           "    f.module = %d\n",
                           $iRefcode);
            if ($fSortByState)
                $sql = $sql."ORDER BY s.refcode, f.name";
            else
                $sql = $sql."ORDER BY f.name";
            Odin32DBWriteFunctions($sql, $db, $sURLArgs);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#functions?".$sURLArgs."&fFunctions=1".
                 "\">here</a> to see all functions.\n";


        /*
         * Files
         */
        Odin32DBNaslov("Files", "files");
        if ($fFiles)
        {
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    COUNT(fn.refcode)\n".
                           "FROM\n".
                           "    file f\n".
                           "    LEFT OUTER JOIN function fn ON fn.file = f.refcode\n".
                           "WHERE\n".
                           "    f.module = %d\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                           $iRefcode);
            Odin32DBWriteFiles($sql, $db);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#files?".$sURLArgs."&fFiles=1".
                 "\">here</a> to see all files.\n";


        /*
         * API Groups
         */
        if ($cAPIGroups > 0)
        {
            Odin32DBNaslov("API Groups", "apigroups");
            if ($fAPIGroups)
            {
                $sql = sprintf("SELECT\n".
                               "    g.refcode,\n".
                               "    g.name,\n".
                               "    COUNT(f.refcode)\n".
                               "FROM\n".
                               "    apigroup g\n".
                               "    JOIN function f\n".
                               "WHERE\n".
                               "    g.module = %d AND\n".
                               "    f.module = %d AND\n".
                               "    f.apigroup = g.refcode\n".
                               "GROUP BY f.apigroup\n".
                               "ORDER BY g.name\n",
                               $iRefcode,
                               $iRefcode);
                Odin32DBWriteAPIGroups($sql, $db);
            }
            else
                echo "Click <a href=\"Odin32DB.phtml#apigroups?".$sURLArgs."&fAPIGroups=1".
                     "\">here</a> to see all the API Groups.\n";
        }


        /*
         * Authors
         */
        Odin32DBNaslov("Authors", "authors");
        if ($fAuthors)
        {
            $sql = sprintf("SELECT\n".
                           "    a.refcode,\n".
                           "    a.name,\n".
                           "    COUNT(f.refcode)\n".
                           "FROM\n".
                           "    fnauthor fa\n".
                           "    JOIN function f\n".
                           "    JOIN author a\n".
                           "WHERE\n".
                           "    f.module    = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    fa.author   = a.refcode\n".
                           "GROUP BY a.refcode\n".
                           "ORDER BY a.name\n",
                            $iRefcode);
            Odin32DBWriteAuthors($sql, $db);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#authors?".$sURLArgs."&fAuthors=1".
                 "\">here</a> to see all authors.\n";
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
function Odin32DBFileInfo($db, $iRefcode, $fFunctions, $fAPIGroups, $fAuthors, $fSortByState)
{
    $sURLArgs = "filerefcode=".$iRefcode.
                ($fFunctions   ? "&fFunctions=1"   : "").
                ($fAuthors     ? "&fAuthors=1"     : "").
                ($fAPIGroups   ? "&fAPIGroups=1"   : "").
                ($fSortByState ? "&fSortByState=1" : "");

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
                   "    f.module        AS modrefcode,\n".
                   "    m.name          AS modname\n".
                   "FROM\n".
                   "    file f,\n".
                   "    module m,\n".
                   "    author a\n".
                   "WHERE\n".
                   "    f.refcode   = %d AND\n".
                   "    f.module    = m.refcode AND\n".
                   "    f.lastauthor= a.refcode",
                    $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov("General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","",
                         " (<a href=\"cvs.phtml?sFile=./src/".$array["modname"]."/".$array["name"].",v&sRevision=\">cvs</a>)");
        Odin32DBInfoRow1("Revision", $array, "revision","","","","");
        Odin32DBInfoRow1("Changed", $array, "lastdatetime","","","","");
        Odin32DBInfoRow1("Last Author", $array, "lastauthorname","lastauthorrefcode","authorrefcode","","");
        Odin32DBInfoRow1("Module", $array, "modname","modrefcode","modrefcode","","");
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

        echo "</table>\n";

        /*
         * Description
         */
        Odin32DBDocRow("Description", "description", $array, "description");

        /*
         * Completion
         */
        Odin32DBNaslov("Completion", "completion");
        Odin32DBCompletionBarFile($iRefcode, "", $db);

        /*
         * States
         */
        $sql = sprintf("SELECT\n".
                       "    state,\n".
                       "    COUNT(state)\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    file = %d\n".
                       "GROUP BY state\n".
                       "ORDER BY state",
                        $iRefcode);
        Odin32DBWriteStates($cFunctions, $sql, $db);


        /*
         * Functions
         */
        Odin32DBNaslov("Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    s.color,\n".
                           "    s.name\n".
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
            Odin32DBWriteFunctions($sql, $db, $sURLArgs);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#functions?".$sURLArgs."&fFunctions=1".
                 "\">here</a> to see all functions.\n";


        /*
         * API Groups
         */
        if ($cAPIGroups > 0)
        {
            Odin32DBNaslov("API Groups", "apigroups");
            if ($fAPIGroups)
            {
                $sql = sprintf("SELECT\n".
                               "    g.refcode,\n".
                               "    g.name,\n".
                               "    COUNT(f.refcode)\n".
                               "FROM\n".
                               "    apigroup g\n".
                               "    JOIN function f\n".
                               "WHERE\n".
                               "    f.file = %d AND\n".
                               "    f.apigroup = g.refcode\n".
                               "GROUP BY f.apigroup\n".
                               "ORDER BY g.name\n",
                               $iRefcode);
                Odin32DBWriteAPIGroups($sql, $db);
            }
            else
                echo "Click <a href=\"Odin32DB.phtml#apigroups?".$sURLArgs."&fAPIGroups=1".
                     "\">here</a> to see all the API Groups.\n";
        }


        /*
         * Authors
         */
        Odin32DBNaslov("Authors", "authors");
        if ($fAuthors)
        {
            $sql = sprintf("SELECT\n".
                           "    a.refcode,\n".
                           "    a.name,\n".
                           "    COUNT(f.refcode)\n".
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
            Odin32DBWriteAuthors($sql, $db);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#authors?".$sURLArgs."&fAuthors=1".
                 "\">here</a> to see all authors.\n";
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
 * @param       $db             Database handle.
 * @param       $iRefcode       Author reference code.
 * @param       $fModules       Flags which tells wether to list all modules or not.
 * @param       $fFunctions     Flags which tells wether to list all functions or not.
 * @param       $fFiles         Flags which tells wether to list all files or not.
 * @param       $fAPIGroups     Flags which tells wether to list all apigroups or not.
 * @param       $fSortByState   Flags which tells wether to sort functions by
 *                              state and function name or just by function name.
 * @param       $iModRefcode    Module refcode. All modules if < 0.
 *                              (not implemented yet)
 * @sketch
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function Odin32DBAuthorInfo($db, $iRefcode, $fModules, $fFunctions, $fFiles, $fAPIGroups, $fSortByState, $iModRefcode)
{
    $sURLArgs = "authorrefcode=".$iRefcode.
                ($fModules     ? "&fModules=1"        : "").
                ($fFunctions   ? "&fFunctions=1"   : "").
                ($fFiles       ? "&fFiles=1"       : "").
                ($fAPIGroups   ? "&fAPIGroups=1"   : "").
                ($fSortByState ? "&fSortByState=1" : "");

    /*
     * Navigation - TOP
     */
    $sExpand    = "authorrefcode=".$iRefcode."&fModules=1&fFiles=1&fFunctions=1&fAPIGroups=1&fAuthors=1&mod=".$iModRefcode;
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
        Odin32DBNaslov("General", "general");
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
                       "    COUNT(f.module) as functions
                       \n".
                       "FROM\n".
                       "    fnauthor fa,\n".
                       "    function f\n".
                       "WHERE\n".
                       "    fa.author = %d AND".
                       "    f.refcode = fa.function\n".
                       "GROUP BY f.module",
                       $iRefcode);
        $cModules = 0;
        if (($result2 = mysql_query($sql, $db)) && ($cModules = mysql_num_rows($result2)) > 0)
            Odin32DBInfoRow1NoArray("# Moduless", $cModules, "","","","");
        $sql = sprintf("SELECT\n".
                       "    COUNT(f.module) as functions
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
                       "    COUNT(f.module) as functions
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

        echo "</table>\n";

        /*
         * Completion
         */
        Odin32DBNaslov("Completion", "completion");
        Odin32DBCompletionBarAuthor($iRefcode, "", $db);

        /*
         * States
         */
        //TODO: optimize this further.
        $sql = sprintf("SELECT\n".
                       "    f.state,\n".
                       "    COUNT(f.refcode)\n".
                       "FROM\n".
                       "    function f,\n".
                       "    fnauthor fa\n".
                       "WHERE\n".
                       "    fa.author = %d AND\n".
                       "    fa.function = f.refcode\n".
                       "GROUP BY f.state\n".
                       "ORDER BY f.state",
                        $iRefcode);
        Odin32DBWriteStates($cFunctions, $sql, $db);

        /*
         * Modules
         */
        Odin32DBNaslov("Modules", "Modules");
        if ($fModules)
        {
            $sql = sprintf("SELECT\n".
                           "    m.refcode,\n".
                           "    m.name,\n".
                           "    COUNT(f.refcode)\n".
                           "FROM\n".
                           "    fnauthor fa,\n".
                           "    module m,\n".
                           "    function f\n".
                           "WHERE\n".
                           "    fa.author = %d AND\n".
                           "    fa.function = f.refcode AND\n".
                           "    f.module = m.refcode\n".
                           "GROUP BY m.refcode\n".
                           "ORDER BY m.name\n",
                            $iRefcode);
            Odin32DBWriteModules($sql, $db, $sURLArgs);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#modules?".$sURLArgs."&fModules=1".
                 "\">here</a> to see all files.\n";


        /*
         * Functions
         */
        Odin32DBNaslov("Functions", "functions");
        if ($fFunctions)
        {
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    m.refcode,\n".
                           "    m.name,\n".
                           "    s.color,\n".
                           "    s.name\n".
                           "FROM\n".
                           "    fnauthor fa\n".
                           "    JOIN function f\n".
                           "    JOIN module m\n".
                           "    LEFT JOIN state s ON f.state = s.refcode\n".
                           "WHERE\n".
                           "    fa.author = %d AND\n".
                           "    fa.function = f.refcode AND \n".
                           "    f.module = m.refcode\n",
                           $iRefcode);
            if ($fSortByState)
                $sql = $sql."ORDER BY s.refcode, f.name, d.name";
            else
                $sql = $sql."ORDER BY d.name, f.name";
            Odin32DBWriteFunctionsWithModules($sql, $db, $sURLArgs);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#functions?".$sURLArgs."&fFunctions=1".
                 "\">here</a> to see all functions.\n";


        /*
         * Files
         */
        Odin32DBNaslov("Files", "files");
        if ($fFiles)
        {
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    COUNT(fn.refcode)\n".
                           "FROM\n".
                           "    fnauthor fa,\n".
                           "    file f,\n".
                           "    function fn\n".
                           "WHERE\n".
                           "    fa.author = %d AND\n".
                           "    fa.function = fn.refcode AND\n".
                           "    fn.file = f.refcode\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                           $iRefcode);
            Odin32DBWriteFiles($sql, $db);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#files?".$sURLArgs."&fFiles=1".
                 "\">here</a> to see all files.\n";


        /*
         * API Groups
         */
        if ($cAPIGroups > 0)
        {
            Odin32DBNaslov("API Groups", "apigroups");
            if ($fAPIGroups)
            {
                $sql = sprintf("SELECT\n".
                               "    g.refcode,\n".
                               "    g.name,\n".
                               "    COUNT(f.refcode)\n".
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
                Odin32DBWriteAPIGroups($sql, $db);
            }
            else
                echo "Click <a href=\"Odin32DB.phtml#apigroups?".$sURLArgs."&fAPIGroups=1".
                     "\">here</a> to see all the API Groups.\n";
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
function Odin32DBAPIGroupInfo($db, $iRefcode, $fFunctions, $fFiles, $fAuthors, $fSortByState)
{
    $sURLArgs = "apigrouprefcode=".$iRefcode.
                ($fFunctions   ? "&fFunctions=1"   : "").
                ($fFiles       ? "&fFiles=1"       : "").
                ($fAuthors     ? "&fAuthors=1"     : "").
                ($fSortByState ? "&fSortByState=1" : "");

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
                   "    m.name          AS modname,\n".
                   "    m.refcode       AS modrefcode\n".
                   "FROM\n".
                   "    apigroup g\n".
                   "    JOIN module m\n".
                   "WHERE\n".
                   "    g.refcode = %d AND".
                   "    g.module = m.refcode\n",
                   $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        Odin32DBNaslov("General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Name", $array, "name","","","","");
        Odin32DBInfoRow1("Module", $array, "modname","modrefcode","modrefcode","bad configuration","");
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
                       "    COUNT(f.module) as functions\n".
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

        echo "</table>\n";

        /*
         * Completion
         */
        Odin32DBNaslov("Completion", "completion");
        Odin32DBCompletionBarAPIGroup($iRefcode, "", $db);

        /*
         * States
         */
        $sql = sprintf("SELECT\n".
                       "    state,\n".
                       "    COUNT(state)\n".
                       "FROM\n".
                       "    function\n".
                       "WHERE\n".
                       "    apigroup = %d\n".
                       "GROUP BY state\n".
                       "ORDER BY state",
                        $iRefcode);
        Odin32DBWriteStates($cFunctions, $sql, $db);

        /*
         * Functions
         */
        Odin32DBNaslov("Functions", "functions");
        if ($fFunctions)
        {
            //TODO: optimize this...
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    s.color,\n".
                           "    s.name\n".
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
            Odin32DBWriteFunctions($sql, $db, $sURLArgs);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#functions?".$sURLArgs."&fFunctions=1".
                 "\">here</a> to see all functions.\n";


        /*
         * Files
         */
        Odin32DBNaslov("Files", "files");
        if ($fFiles)
        {
            //TODO: optimize this...
            $sql = sprintf("SELECT\n".
                           "    f.refcode,\n".
                           "    f.name,\n".
                           "    COUNT(fn.refcode)\n".
                           "FROM\n".
                           "    file f\n".
                           "    LEFT OUTER JOIN function fn ON fn.file = f.refcode\n".
                           "WHERE\n".
                           "    fn.apigroup = %d\n".
                           "GROUP BY f.refcode\n".
                           "ORDER BY f.name\n",
                           $iRefcode);
            Odin32DBWriteFiles($sql, $db);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#files?".$sURLArgs."&fFiles=1".
                 "\">here</a> to see all files.\n";


        /*
         * Authors
         */
        Odin32DBNaslov("Authors", "authors");
        if ($fAuthors)
        {
            //TODO: optimize this...
            $sql = sprintf("SELECT\n".
                           "    a.refcode,\n".
                           "    a.name,\n".
                           "    COUNT(f.refcode)\n".
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
            Odin32DBWriteAuthors($sql, $db);
        }
        else
            echo "Click <a href=\"Odin32DB.phtml#authors?".$sURLArgs."&fAuthors=1".
                 "\">here</a> to see all authors.\n";
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
 * Displays a design not.
 * @returns     void
 * @param       $db             Database handle.
 * @param       $iRefcode       Author reference code.
 * @author  knut st. osmundsen (kosmunds@csc.com)
 * @remark
 */
function Odin32DBDesignNote($db, $iRefcode)
{
    /*
     * Navigation - TOP
     */
    $sExpand    = "authorrefcode=".$iRefcode."&fModules=1&fFiles=1&fFunctions=1&fAPIGroups=1&fAuthors=1&module=".$iModRefcode;
    if ($fSortByState)  $sExpand = $sExpand."&fSortByState=".$fSortByState;
    $sCollapse  = "authorrefcode=".$iRefcode;
    Odin32DBNavigationTop($sExpand, $sCollapse);

    /*
     * Fetch (vital) data.
     */
    $sql = sprintf("SELECT\n".
                   "    dn.name          AS name,\n".
                   "    dn.note          AS note,\n".
                   "    dn.refcode       AS refcode,\n".
                   "    dn.module        AS modrefcode,\n".
                   "    dn.file          AS filerefcode,\n".
                   "    dn.seqnbrnote    AS seqnbnote,\n".
                   "    dn.level         AS level,\n".
                   "    dn.seqnbr        AS seqnbr,\n".
                   "    dn.line          AS line,\n".
                   "    m.name           AS modname,\n".
                   "    f.name           AS file\n".
                   "FROM\n".
                   "    designnote dn,\n".
                   "    module m,\n".
                   "    file f\n".
                   "WHERE   dn.refcode = %d\n".
                   "    AND dn.module = m.refcode\n".
                   "    AND dn.file = f.refcode\n".
                   "ORDER BY dn.seqnbrnote\n",
                   $iRefcode);
    if (($result = mysql_query($sql, $db)) && mysql_num_rows($result) > 0 && ($array = mysql_fetch_array($result)))
    {
        /*
         * General
         */
        /*Odin32DBNaslov("General", "general");
        echo "\n<table width=100% border=3 cellpadding=0>\n";
        Odin32DBInfoRow1("Title", $array, "name","","","","");
        Odin32DBInfoRow1("Module", $array, "modname","","","","");
        Odin32DBInfoRow1("File", $array, "file","","","","");
        Odin32DBInfoRow1("Line", $array, "line","","","","");
        Odin32DBInfoRow1("Seq#", $array, "seqnbr","","","","");
        echo "</table>\n";
        */

        /*
         * Sections.
         */
        $iLevel = 0;
        $sSectionNbr = "";
        do
        {
            /* make section number */
            if ($array["level"] <= 0)
                $sSection = $array["seqnbr"].".0";
            else
            {
                while ($iLevel > 0 && $iLevel < $array["level"])
                {
                    $sSection .= '.0';
                    $iLevel = $iLevel + 1;
                }

                if ($iLevel == 0) $iLevel++;
                while ($iLevel >= $array["level"])
                {
                    $sSection = substr($sSection, 0, strlen($sSection) - strlen(strrchr($sSection, '.')));
                    $iLevel = $iLevel - 1;
                }
                $sSection = $sSection.".".$array["seqnbr"];
            }
            $iLevel = $array["level"];

            /* print it */
            $sName = $sSection." ".$array["name"];
            Odin32DBNaslov($sName, $sName);
            echo $array["note"]."\n";

        } while ($array = mysql_fetch_array($result));

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
$aContent = array();
$aaSubContent = array();
$fNumberSections = 0;

/**
 * Call this to autogenerate section and subsection numbers.
 */
function ODin32DBNumberSections()
{
    global $fNumberSections;
    $fNumberSections = 1;
}


/**
 * Makes the contents for this page.
 * @sketch      Writes the headers present in the contents array.
 */
function Odin32DBWriteContents()
{
    global $aContent;
    global $aaSubContent;

    echo "\n",
         "<tr><td>\n";

    for ($i = 0; $i < sizeof($aContent); $i += 2)
    {
        echo "<font size=-2 face=\"WarpSans, Helv, Helvetica, Arial\">".
             "<a href=\"#".$aContent[$i+1]."\"><font color=000099>".
             $aContent[$i]."</font></a><br>\n";

        $aSubContent = $aaSubContent[$i/2];
        echo "\n";
        if (sizeof($aSubContent) > 0)
        {
            echo "<table>\n";
            for ($j = 0; $j < sizeof($aSubContent); $j += 2)
            {
                echo "<tr><td width=10%>&nbsp;</td>\n",
                     "    <td><font size=-2 face=\"WarpSans, Helv, Helvetica, Arial\">".
                     "<font size=-2 face=\"WarpSans, Helv, Helvetica, Arial\">".
                     "<a href=\"#".$aSubContent[$j+1]."\"><font color=000099>".
                     $aSubContent[$j]."</font></a><br>\n";
                echo "</td></tr>\n";
            }
            echo "</table>\n";
        }
        else
            echo "<p>\n";
    }

    echo "</td></tr>\n";
}

/**
 * Forwarder which also maintains the contents array.
 */
function Odin32DBNaslov($sFull, $sShort)
{
    global $aContent;
    global $aaSubContent;
    global $fNumberSections;

    if ($fNumberSections)
        $sFull = (sizeof($aContent)/2 + 1).". ".$sFull;

    $aContent[] = $sFull;
    $aContent[] = $sShort;
    $aaSubContent[] = array();
    return Naslov($sFull, $sShort);
}


/**
 * Forwarder which also maintains the contents array.
 * Equal to Odin32DBNaslov, but have allows a different contents text.
 */
function Odin32DBNaslov2($sFull, $sFullContents, $sShort)
{
    global $aContent;
    global $aaSubContent;
    global $fNumberSections;

    if ($fNumberSections)
    {
        $sFull = (sizeof($aContent)/2 + 1).". ".$sFull;
        $sFullContents = (sizeof($aContent)/2 + 1).". ".$sFullContents;
    }

    $aContent[] = $sFullContents;
    $aContent[] = $sShort;
    $aaSubContent[] = array();
    return Naslov($sFull, $sShort);
}


/**
 * Sub title with contents entry.
 */
function Odin32DBPodNaslov($sFull, $sShort)
{
    global $aContent;
    global $aaSubContent;
    global $fNumberSections;

    $j = (sizeof($aContent) / 2) - 1;
    $aSubContent = $aaSubContent[$j];

    if ($fNumberSections)
        $sFull = (sizeof($aContent)/2).".".(sizeof($aSubContent)/2 + 1)." ".$sFull;

    $aSubContent[] = $sFull;
    $aSubContent[] = $sShort;
    $aaSubContent[$j] = $aSubContent;
    echo "<p><b><a name=\"".$sShort."\"><font size=+0 face=\"Helv, Arial\"><br>".$sFull."</font></b></a><p>";
}

?>

