<?php
/* $Id: Odin32Changelog.php,v 1.2 2003-04-15 00:39:28 bird Exp $
 *
 * Change log parser/conveter.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

echo "<TABLE border=0 CELLSPACING=0 CELLPADDING=0 COLS=3 WIDTH=\"750\">\n".
     "<TR><td>\n";
$aChangeLog = ChangeLog_Read(file("y:/bldodin/scripts/StateD/ChangeLog"));
//ChangeLog_Dump($aChangeLog);
ChangeLog_MakeTable($aChangeLog);

echo "</td></tr></table>\n";


/**
 * Reads, well really it pareses the file array passed into it.
 * @returns Array structure of the changelog (see remarks)
 * @param   $aFile  Array of the lines in the change log file to parse.
 * @sketch
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 *
 * Return structure sketch:
 *  array $aChangeLog[]
 *  {
 *      int "cEntries";
 *      array //entry
 *      {
 *          _str "sDate";
 *          _str "sName";
 *          _str "sEmail";
 *          int  "cModules";
 *          array "aModules"[]
 *          {
 *              array //entry
 *              {
 *                  _str "sModule";
 *                  int  "cChanges";
 *                  array "aChanges"[]
 *                  {
 *                      _str //change description
 *                  };
 *              };
 *          };
 *      };
 *  }
 */
function ChangeLog_Read($aFile)
{
    $aChangeLog = array();              /* Empty change log array. */
    $iChangeLog = -1;                   /* Current Entry in $aChangeLog*/
    $fFirst = 1;                        /* Searching for the first line entry,
                                         * ie. an ISO date. */
    $fPre   = 0;                        /* We're parsing preformatted text.
                                         * Add newlines! */
    $iModule = -1;
    $iChange = -1;

    reset($aFile);
    while (list($sKey, $sLine) = each($aFile))
    {
        $sTrimmed = trim($sLine);

        /* ISO date on this line? */
        if (strlen($sTrimmed) > 10 && $sTrimmed[10] == ':' && isISODate(substr($sTrimmed, 0, 10)) )
        {
            $fFirst = 0;
            /* example line:
             * 2000-10-23: Sander van Leeuwen <sandervl@xs4all.nl>
             * Locates the email address (which is enclosed within '<'-'>'.)
             */
            $iLeft  = strpos($sTrimmed, "<");
            $iRight = strrpos($sTrimmed, ">");
            if ($iLeft >= $iRight)
            {
                //echo "<!-- $iLeft >= $iRight; line $sKey -->";
                $fFirst = 1; // skips this entry.
                continue;
            }
            $sDate  = substr($sTrimmed, 0, 10);
            $sName  = trim(substr($sTrimmed, 11, $iLeft - 11));
            $sEmail = trim(substr($sTrimmed, $iLeft + 1, $iRight - $iLeft - 1));
            //echo "<p>\r\n$sDate: $sName <$sEmail><br>\r\n";

            $aChangeLog[++$iChangeLog] =
                array("sDate"    => $sDate,
                      "sName"    => $sName,
                      "sEmail"   => $sEmail,
                      "cModules" => 0,
                      "aModules" => array()
                        );
            $iModule = -1;
        }
        else
        {
            /* example lines:
             *    - KERNEL32: o Add rename entry for winspool.drv to odin.ini. (if not present)
             *                o Check pointers in Read/WriteProcessMemory
             *                o Add OLE key during installation:<pre>
             *                  [HKEY_LOCAL_MACHINE\Software\Microsoft\OLE]
             *                  "EnableDCOM"="Y"
             *                  "EnableRemoteConnect"="N"</pre>
             *
             *    - WINSPOOL: o Removed changes. (now generates winspool.dll again)
             */

            /* Look for new module */
            if (    strlen($sTrimmed) > 0
                &&  $sTrimmed[0] == '-'
                &&  ($iColon = strpos($sTrimmed, ":")) > 1
                &&  strlen($sModule = trim(substr($sTrimmed, 1, $iColon - 1))) > 0
                &&  strlen($sModule) < 42
                )
            {
                $sTrimmed = ltrim(substr($sTrimmed, $iColon+1));
                $sLine = substr($sLine, strpos($sLine, ':')+1);

                $aChangeLog[$iChangeLog]["cModules"] = ++$iModule + 1;
                $aChangeLog[$iChangeLog]["aModules"][$iModule] =
                    array("sModule"  => $sModule,
                          "cChanges" => 0,
                          "aChanges" => array()
                        );
                $iChange = 0;

                //echo "$sModule:\r\n";
            }

            /* Look for start of new point */
            if (    strlen($sTrimmed) > 0
                && (    $sTrimmed[0] == "ú"
                    || ($sTrimmed[0] == "o" && $sTrimmed[1] == " ")
                    )
                )
            {
                if ($iChange > 0 ||
                    isset($aChangeLog[$iChangeLog]["aModules"][$iModule]["aChanges"][$iChange]))
                    ++$iChange;
                $aChangeLog[$iChangeLog]["aModules"][$iModule]["cChanges"] = $iChange + 1;
                $sTrimmed = ltrim(substr($sTrimmed, 1));

                //echo "    o ";
            }

            /* If not empty line, then add it to current change description */
            if (strlen($sTrimmed) > 0)
            {
                if (isset($aChangeLog[$iChangeLog]["aModules"][$iModule]["aChanges"][$iChange]))
                    $aChangeLog[$iChangeLog]["aModules"][$iModule]["aChanges"][$iChange] .=
                        " ".$sTrimmed;
                else
                    $aChangeLog[$iChangeLog]["aModules"][$iModule]["aChanges"][$iChange] =
                        $sTrimmed;

                /* check for <pre> and </pre> */
                $sTrimmed = strtolower($sTrimmed);
                $iPre = strpos($sTrimmed, "<pre");
                $iPre = ($iPre ? $iPre : -1);
                $iEndPre = strpos($sTrimmed, "</pre");
                $iEndPre = ($iEndPre ? $iEndPre : -1);
                //echo "<!--$iPre,$iEndPre-->\n";

                if ($fPre && $iEndPre > $iPre)
                    $fPre = false;

                if (!$fPre && $iPre > $iEndPre)
                    $fPre = true;

                /* add newline if inside a <pre> */
                if ($fPre)
                    $aChangeLog[$iChangeLog]["aModules"][$iModule]["aChanges"][$iChange] .=
                        "\n";
            }

        }
    }
    $aChangeLog["cEntries"] = $iChangeLog+1;

    /* return array */
    return $aChangeLog;
}


/**
 * Dumps the array - debugging.
 * @returns nothing.
 * @param   $aChangeLog     Array returned by ChangeLog_Read.
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function ChangeLog_Dump($aChangeLog)
{
    echo "<p><t1>Dump</t1>\r\n";

    for ($iChangeLog = 0; $iChangeLog < $aChangeLog["cEntries"]; $iChangeLog++)
    {
        $aLogEntry = $aChangeLog[$iChangeLog];

        echo $aLogEntry["sDate"].": ".$aLogEntry["sName"]." <".$aLogEntry["sEmail"]."> ".
             $aLogEntry["cModules"]." modules\r\n";

        $aModules = $aLogEntry["aModules"];
        for ($iModule = 0; $iModule < $aLogEntry["cModules"]; $iModule++)
        {
            //$aModule = $aModules[$iModule];
            $aModule = $aChangeLog[$iChangeLog]["aModules"][$iModule];
            echo "    - ".$aModule["sModule"]." ".$aModule["cChanges"]." changes\r\n";
            if ($aModule["cChanges"] > 0)
            {
                for ($iChange = 0; $iChange < $aModule["cChanges"]; $iChange++)
                    echo "     o ".$aModule["aChanges"][$iChange]."\r\n";
            }
            else
                echo "      ".$aModule["aChanges"][0]."\r\n";

        }
    } /* while */
}


/**
 * Write the entrie change log to a table.
 * @returns nothing.
 * @param   $aChangeLog     Changelog array - output from ChangeLog_Read.
 * @sketch
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function ChangeLog_MakeTable($aChangeLog)
{
    echo "\n".
         "<table>\n";

    for ($iChangeLog = 0; $iChangeLog < $aChangeLog["cEntries"]; $iChangeLog++)
        ChangeLog_MakeTableEntry($aChangeLog[$iChangeLog]);

    echo "</table>\n";
}


/**
 * Presents a single log entry ($aLogEntry).
 * (It inserts a row into an existing table in the HTML output.)
 * @returns nothing.
 * @param   $aLogEntry  Log entry to present.
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function ChangeLog_MakeTableEntry($aLogEntry)
{
    echo "<tr><td bgcolor=#d0dce0><font size=-1>".
         $aLogEntry["sDate"].": ".
         $aLogEntry["sName"].
         /*" &lt;".$aLogEntry["sEmail"]."&gt;".*/"</td></tr>\r\n";

    echo "  <tr><td bgcolor=#f0f0f0>\n".
         "    <table>\n";

    $aModules = $aLogEntry["aModules"];
    for ($iModule = 0; $iModule < $aLogEntry["cModules"]; $iModule++)
    {
        $aModule = $aModules[$iModule];
        $sModule = ChangeLog_CaseModuleName($aModule["sModule"]);

        echo "      <tr><td valign=top><font size=-1><b>".$sModule."</b>\n";

        echo "        <ul compact>\n";
        if ($aModule["cChanges"] > 0)
        {
            for ($iChange = 0; $iChange < $aModule["cChanges"]; $iChange++)
                echo "        <li>".$aModule["aChanges"][$iChange]."\n";
        }
        else
            echo "        <li>".$aModule["aChanges"][0]."\r\n";
        echo "        </ul>\n".
             "      </td></tr>\n";
    }

    echo "    </table></td>\n".
         "  </tr>\n";
}


/**
 * This function cases the module name if it is a dll name or a directory name.
 * If not a dll, nothing is done.
 * @returns Modulename string to present.
 * @param   $sModule    Modulename string.
 * @sketch
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function ChangeLog_CaseModuleName($sModule)
{
    $sModule = trim($sModule);
    if (    strpos($sModule, ".") > 0
        ||  strtoupper($sModule) != $sModule)
        return $sModule;

    $sResult = "";
    $sModule = strtolower($sModule);
    while (($i = strpos($sModule, "/")) > 0 || ($i = strpos($sModule, "\\")) > 0)
    {
        $sResult .= ucfirst(substr($sModule, 0, $i+1));
        $sModule = substr($sModule, $i+1);
    }
    $sResult .= ucfirst($sModule);

    return $sResult;
}


/**
 * Checks if a date is a valid ISO date.
 * @returns true/false according to the query.
 *
 * @param   $sDate  Possible ISO date.
 * @status
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
function isISODate($sDate)
{
    /*YYYY-MM-DD*/
    /*0123456789*/

    if (strlen($sDate) != 10
        ||
        ($sDate[4] != '-' || $sDate[7] != '-')
        )
        return false;
    $iYear  = @(int)substr($sDate, 0, 4);
    $iMonth = @(int)substr($sDate, 5, 2);
    $iDay   = @(int)substr($sDate, 8, 2);
    if (    $iYear  <= 0
        ||  $iYear  > 9999
        ||  $iMonth <= 0
        ||  $iMonth > 12
        ||  $iDay   <= 0
        ||  $iDay   > 31
        )
        return false;

    return true;
}




?>
