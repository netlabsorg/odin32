<?php

require "Odin32DBhelpers.php3";

/*
 * Configuration:
 */
$sCVSROOT = ".";
$sCVSROOT = "d:/odin32/cvs/cvsroot";


/**
 * Quick and dirty CVS file parser.
 */
class CVSFile
{
    var     $fOk;                       /* Status of contructor. */
    var     $sError;                    /* Last error message. */
    var     $sFullName;                 /* Full path of the  */
    var     $sDir;                      /* CVSROOT relative directory */
    var     $sName;                     /* Workfile filename. */
    var     $sExt;                      /* Workfile extention. */
    var     $aasKeys;                   /* base keys */
    var     $aasDeltas;                 /* the text values only */
    var     $aaasRevs;                  /* all types of revision info (but the text) */


    /**
     * Constructor.
     * Opens a CVS repository file, reads it into memory and closes it.
     */
    function CVSFile($sFilename, $fNoDeltas)
    {
        global $sCVSROOT;


        $this->fOk = 0;
        /*
         * TODO: Security: Check that the path and filename is valid!
         *       We can't allow relative paths (ie. "..")
         */
        if (strlen($sFilename) < 3 || substr($sFilename, strlen($sFilename)-2) != ",v")
        {
            $this->sError = "filename is invalid";
            return 1;
        }

        /*
         * Check filesize. Minimum size is 10 bytes!
         */
        $this->sFullname = $sCVSROOT."/".$sFilename;
        $cbFile = filesize($this->sFullname);
        if ($cbFile <= 10)
        {
            $this->sError = "too small file,  " . $this->sFullname . ", ". $cbFile ."\n";
            return 1;
        }
        if (!$fNoDeltas && $cbFile >= (2*1024*1024)) //currently max size of 2MB.
        {
            $this->sError = "\ntoo large file,  ". $this->sFullname .", ". $cbFile ."\n";
            return 1;
        }


        /*
         * Seems ok. Let's, init object variables
         */
        $this->fOk          = 0;
        $this->sError       = "";
        $i = strrpos($sFilename, "\\");
        $j = strrpos($sFilename, "/");
        $i = ($i > $j) ? $i : $j;
        $this->sName        = substr($sFilename, $i > 0 ? $i + 1 : 0, strlen($sFilename)-2 - ($i > 0 ? $i + 1 : 0));
        $this->sDir         = substr($sFilename, 0, $i);
        if (($i = strrpos($this->sName, '.')) > 0)
            $this->sExt     = substr($this->sName, $i+1);
        else
            $this->sExt     = "";
        $this->aasKeys      = array();
        $this->aasDeltas    = array();
        $this->aaasRevs     = array();


        /*
         * Open the file
         */
        $hFile = fopen($this->sFullname, "rb");
        if (!$hFile)
        {
            $this->sError = "\nfailed to open the file $this->sFullname\n";
            fclose($hFile);
            return 1;
        }

        /*
         * Parse the file.
         */
        $this->fOk = $this->ParseFile2($hFile, 0);// $fNoDeltas);

        fclose($hFile);

        /*
         * Return.
         */

        return 1;
    }


    /**
     * Parses the file.
     * (internal)
     */
    function ParseFile($hFile, $fNoDeltas)
    {

        /*
         * Parse file.
         */
        $fAt    = 0;
        $fNewKey= 1;
        $sKey   = "";
        $sRev   = "";
        $fDesc  = 0;

        $iLine  = -1;
        $sLine  = "";
        $fStop  = 0;
        while (($sLine != "" || !feof($hFile)) && !$fStop)
        {
            /*
             * Left trim.
             * If empty line, get next and iterate.
             */
            $sLine = ltrim($sLine);
            if (!$sLine || $sLine == "" || $sLine == "\n" || $sLine == "\r")
            {
                $iLine++;
                $sLine = fgets($hFile, 0x1000);
                continue;
            }

            /*
             * Are we looking for a new key word?
             */
            if ($fNewKey)
            {
                //$sKey = CopyWord($sLine);
                $cch = strlen($sLine);
                for ($i = 0; $i < $cch; $i++)
                {
                    $c = $sLine[$i];
                    if (!(
                          ($c >= 'a'  && $c <= 'z')
                          ||
                          ($c >= 'A'  && $c <= 'Z')
                          ||
                          ($c >= '0'  && $c <= '9')
                          ||
                          $c == '.'
                          ||
                          $c == '_'
                          )
                        )
                        break;
                }
                $sKey = substr($sLine, 0, $i);

                $sLine = ltrim(SkipWord($sLine));
                if ($sKey[0] >= "0" && $sKey[0] <= "9")
                    /* Revision number: delta or revision info */
                    $sRev = $sKey;
                else
                    $fNewKey = 0;
                continue;
            }


            /*
             * Extract value
             */
            $fNoSemicolon = ($sKey == "desc" || $sKey == "log" || $sKey == "desc");
            if ($fAt = ($sLine[0] == "@")) //check if the value is enclosed in '@'s
                $sLine = substr($sLine, 1);
            $asValue = array();
            $fEnd = 0;
            while (!$fEnd)
            {
                /* get new line? */
                if (!$sLine || $sLine == "" || $sLine == "\n" || $sLine == "\r")
                {
                    if (feof($hFile))
                        break;
                    /* Get next line and remove any EOF chars */
                    $iLine++;
                    $sLine = str_replace("\x1a", "", fgets($hFile, 0x1000));
                    continue;
                }

                //echo "debug line $iLine: $sLine";

                /*
                 * Look for end char (either ; or @) and copy.
                 * If end of value then $sLine <- rest of line.
                 */
                $fEnd = 0;
                $cchLine = strlen($sLine);
                if ($fAt)
                {   /* terminated with @ */
                    //$iAt = 0;
                    //for ($iAt; $iAt+1 < $cchLine; $iAt++)
                    //    if ($sLine[$iAt] == '@' && ($fEnd = ($sLine[++$iAt] != '@')))
                    //        break;
                    if ($sLine[0] == '@' && $sLine[1] != '@')
                        $fEnd = $iAt = 1;
                    else
                    {
                        $iAt = 0;
                        while ($iAt = strpos($sLine, "@", $iAt+1))
                           if ($fEnd = ($sLine[++$iAt] != '@'))
                                break;
                    }

                    if ($fEnd)
                    {
                        $asValue[] = str_replace("@@", "@", substr($sLine, 0, $iAt - 1));
                        /* if semicolon end, skip to it. ASSUMES: same line! */
                        if (!$fNoSemicolon && ($iAt = strpos($sLine, ";", $iAt)) >= 0)
                            $iAt++;
                        $sLine = (strlen($sLine) > $iAt && $iAt >= 0) ? substr($sLine, $iAt) : "";
                    }
                    else
                    {
                        $asValue[] = str_replace("@@", "@", $sLine);
                        $sLine = "";
                    }
                }
                else
                {   /* terminated with ';' */
                    $i = strpos($sLine, ';');
                    if ($fEnd = ($i > 0 || $sLine[0] == ';'))
                    {
                        //$asValue[] = str_replace("@@", "@", substr($sLine, 0, $i));
                        $asValue[] = substr($sLine, 0, $i);
                        $sLine = (strlen($sLine) > $i+1) ? substr($sLine, $i+1) : "";
                    }
                    else
                    {
                        //$asValue[] = str_replace("@@", "@", $sLine);
                        $asValue[] = $sLine;
                        $sLine = "";
                    }
                }
            }


            /*
             * Process the key.
             */
            switch ($sKey)
            {
                /*
                 * This is normally the keyword separating
                 * revision info from log+text info.
                 */
                case "desc":
                    $fDesc = 1;
                    $sRev = "";
                    break;

                /*
                 * Stop after the first log entry.
                 */
                case "log":
                    $fStop = $fNoDeltas;
                    break;

                /*
                 * Don'r read deltas for archives with the expand tag set
                 */
                case "expand":
                    $fNoDeltas = 1;//= $asValue[0] != "";
                    break;
            }

            /*
             * Save key and value in the appopriate place.
             */
            if ($sRev == "")
            {   /* Base keys */
                if (sizeof($this->aaKeys) <= 0 //sanity check! head must come first and have a value!
                    && ($sKey != "head" || sizeof($asValue) <= 0 || $asValue[0] == ""))
                {
                    $this->sError = "Invalid file format.";
                    fclose($hFile);
                    return 0;
                }
                $this->aasKeys[$sKey] = $asValue;
            }
            else if ($sKey != "text")
            {   /* Revision information keys  */
                if (!isset($this->aaasRevs[$sRev]))
                    $this->aaasRevs[$sRev] = array($sKey => $asValue);
                else
                    $this->aaasRevs[$sRev][$sKey] = $asValue;
            }
            else
            {   /* Delta (ie. 'text') key */
                $this->aasDeltas[$sRev] = $asValue;
            }

            /*
             * Completed reading of this key, so next one.
             */
            $fNewKey = 1;

            /* debug */
            //echo "debug key: $sKey  value(".sizeof($asValue)."):".$asValue[0]."\n";
        }

        return 1;
    }


    /**
     * Parses the file.
     * (internal)
     */
    function ParseFile2($hFile, $fNoDeltas)
    {

        /*
         * Parse file.
         */
        $sKey   = "";
        $sRev   = "";

        $sLine  = "";
        $fStop  = 0;
        while (!$fStop)
        {
            /*
             * Left trim.
             * If empty line, get next and iterate.
             */
            $sLine = ltrim($sLine);
            if ($sLine == "")
            {
                if (feof($hFile))
                    break;
                $sLine = fgets($hFile, 0x1000);
                continue;
            }

            /*
             * Are we looking for a new key word?
             */
            if ($sKey == "")
            {
                $cch = strlen($sLine);
                for ($i = 0; $i < $cch; $i++)
                {
                    $c = $sLine[$i];
                    if (!(   ($c >= 'a' && $c <= 'z')
                          || ($c >= 'A' && $c <= 'Z')
                          || ($c >= '0' && $c <= '9')
                          || $c == '.'  || $c == '_'
                          )
                        )
                        break;
                }
                if ($sLine[0] >= "0" && $sLine[0] <= "9") // Revision number: delta or revision info
                    $sRev = substr($sLine, 0, $i);
                else
                    $sKey = substr($sLine, 0, $i);
                $sLine = ltrim(substr($sLine, $i));
                continue;
            }


            /*
             * Extract value
             */
            $fSemicolon = !($sKey == "desc" || $sKey == "log" || $sKey == "desc");
            $asValue = array();
            $fEnd = 0;
            if ($sLine[0] == "@") //check if the value is enclosed in '@'s
            {
                $sLine = substr($sLine, 1);
                for (;;)
                {
                    /* get new line? */
                    if ($sLine == "")
                    {
                        if (feof($hFile))
                            break;
                        $sLine = fgets($hFile, 0x1000);
                        continue;
                    }

                    /*
                     * Look for end char ( @) and copy.
                     * If end of value then $sLine <- rest of line.
                     */
                    if ($sLine[0] != '@' || $sLine[1] == '@')
                    {
                        $iAt = 0;
                        while ($iAt = strpos($sLine, "@", $iAt+1))
                            if ($fEnd = ($sLine[++$iAt] != '@'))
                            {
                                $asValue[] = str_replace("@@", "@", substr($sLine, 0, $iAt - 1));
                                /* if semicolon end, skip to it. ASSUMES: same line! */
                                if ($fSemicolon && ($i = strpos($sLine, ";", $iAt)) > 0)
                                    $iAt = $i + 1;
                                $sLine = substr($sLine, $iAt);
                                break;
                            }
                        if ($iAt > 0)
                            break;
                    }
                    else
                    {
                        /* if semicolon end, skip to it. ASSUMES: same line! */
                        if ($fSemicolon && ($iAt = strpos($sLine, ";", 1)) > 0)
                            $sLine = substr($sLine, $iAt+1);
                        else
                            $sLine = substr($sLine, 1);
                        break;
                    }

                    $asValue[] = str_replace("@@", "@", $sLine);
                    $sLine = fgets($hFile, 0x1000);
                }
            }
            else
            {
                for (;;)
                {
                    /* get new line? */
                    if ($sLine == "")
                    {
                        if (feof($hFile))
                            break;
                        $sLine = fgets($hFile, 0x1000);
                        continue;
                    }

                    /*
                     * Look for end char (either ; or @) and copy.
                     * If end of value then $sLine <- rest of line.
                     */
                    if (($i = strpos($sLine, ';')) <= 0 && $sLine[0] != ';')
                    {   //terminator not found.
                        $asValue[] = $sLine;
                        $sLine = fgets($hFile, 0x1000);
                    }
                    else
                    {   //terminator found
                        $asValue[] = substr($sLine, 0, $i);
                        $sLine = substr($sLine, $i+1);
                        break; // end
                    }
                }
            }


            /*
             * Process the key.
             */
            switch ($sKey)
            {
                /*
                 * This is normally the keyword separating
                 * revision info from log+text info.
                 */
                case "desc":
                    $sRev = "";
                    break;

                /*
                 * Stop after the first log entry.
                 */
                case "log":
                    $fStop = $fNoDeltas;
                    break;

                /*
                 * Don'r read deltas for archives with the expand tag set
                 */
                case "expand":
                    $fNoDeltas = 1;//= $asValue[0] != "";
                    break;
            }

            /*
             * Save key and value in the appopriate place.
             */
            if ($sRev == "")
            {   /* Base keys */
                if (sizeof($this->aaKeys) <= 0 //sanity check! head must come first and have a value!
                    && ($sKey != "head" || sizeof($asValue) <= 0 || $asValue[0] == ""))
                {
                    $this->sError = "Invalid file format.";
                    fclose($hFile);
                    return 1;
                }
                $this->aasKeys[$sKey] = $asValue;
            }
            else if ($sKey != "text")
            {   /* Revision information keys  */
                if (!isset($this->aaasRevs[$sRev]))
                    $this->aaasRevs[$sRev] = array($sKey => $asValue);
                else
                    $this->aaasRevs[$sRev][$sKey] = $asValue;
            }
            else
            {   /* Delta (ie. 'text') key */
                $this->aasDeltas[$sRev] = $asValue;
            }

            /*
             * Completed reading of this key, so next one.
             */
            $sKey = "";
        }

        return 1;
    }



    /**
     * Debug dump function.
     */
    function DumpInfo()
    {
        echo "\nDump:<br>\n";

        while (list ($sKey, $asValue) = each ($this->aasKeys))
        {
            echo "* key: $sKey *<br>\n";
            if (sizeof((array)$asValue) > 0)
            {
                while (list ($key, $s) = each ($asValue))
                    echo $s;
                echo "<br>\n";
            }
        }

        while (list ($sRev, $aasKeys) = each ($this->aaasRevs))
        {
            echo "* Revision: $sRev *<br>\n";
            if (sizeof((array)$aasKeys) > 0)
            {
                while (list ($sKey, $asValue) = each ($aasKeys))
                {
                    echo "* key: $sKey *<br>\n";
                    if (sizeof((array)$asValue) > 0)
                    {
                        while (list ($key, $s) = each ($asValue))
                            echo $s;
                        echo "<br>\n";
                    }
                }
            }
        }

        if (0)
        {
            while (list ($sKey, $asValue) = each ($this->aasDeltas))
            {
                echo "* delta for revision: $sKey *<br>\n";
                if (sizeof((array)$asValue) > 0)
                {
                    while (list ($key, $s) = each ($asValue))
                        echo $s."<br>";
                    echo "\n";
                }
            }
        }
    }


    /**
     * Prints the contents of the file to stdout.
     *
     * Color coding is enabled. (TODO)
     *
     * Currently only $sRevision == head revision is supported
     * @returns       Success indicator (true / false)
     * @param         $sRevision.       Revision number. defaults to head revision.
     *
     */
    function PrintRevision($sRevision)
    {
        /* defaults to head revision if empty */
        if ($sRevision == "") $sRevision = $this->aasKeys["head"][0];
        if (!isset($this->aasDeltas[$sRevision]))
        {
            $this->sError = "CVSFile::PrintRevision is called with an invalid revision number. ($sRevision)";
            return 0;
        }
        /* to-be-removed - TODO - FIXME */
        if ($sRevision != $this->aasKeys["head"][0])
        {
            $this->sError = "CVSFile::PrintRevision is called with an invalid revision number (not head).";
            return 0;
        }

        /*
         * Initiate the color encoder.
         */
        switch ($this->sExt)
        {
            case 'c':
            case 'cpp':
            case 'cxx':
            case 'h':
            case 'hpp':
                C_ColorInit($aVariables);
                $iColorEncoder = 1;
                break;
            default:
                $iColorEncoder = 0;
        }



        /*
         * Write it!
         */
        echo "<table><tr><td bgcolor=#020286><pre><font size=-0 face=\"System VIO, System Monospaced\" color=#02FEFE>\n";

        for ($cLines = sizeof($this->aasDeltas[$sRevision]), $iLine = 0;
             ($iLine < $cLines);
             $iLine++)
        {
            /*
             * Preprocessing... Color coding
             */
            switch ($iColorEncoder)
            {
                case 1:
                    echo "<a name=$iLine>";
                    echo  C_ColorEncode(htmlspecialchars($this->aasDeltas[$sRevision][$iLine]), $aVariables);
                    //C_ColorEncode2(htmlspecialchars($this->aasDeltas[$sRevision][$iLine]), $aVariables);
                    echo "</a>";
                    break;

                default:
                    echo  "<a name=$iLine>",htmlspecialchars($this->aasDeltas[$sRevision][$iLine]),"</a>";
            }
        }

        echo "</pre></td></tr></table>\n";

        return 1;
    }


    /**
     * Gets the revision number of the head revision.
     * @returns     head revision number
     */
    function getHead()
    {
        return $this->aasKeys["head"][0];
    }


    /**
     * Gets the log string for the given revision.
     * @returns     Array of strings in the log text.
     * @param       $sRev       Revision number to get log text for.
     */
    function getLog($sRev)
    {
        return @$this->aaasRevs[$sRev]["log"];
    }


    /**
     * Gets the author for a revision.
     * @return      Author name.
     * @param       $sRev       Revision number to get author name for.
     */
    function getAuthor($sRev)
    {
        return @$this->aaasRevs[$sRev]["author"][0];
    }

    /**
     * Get date+time stap on a revision.
     * @returns     date string for the given revision.
     * @param       $sRev       Revision number to get date+time for.
     */
    function getDate($sRev)
    {
        return @$this->aaasRevs[$sRev]["date"][0];
    }

    /**
     * Get the age of the given revision.
     * @returns     Age string. (human readable)
     * @param       $sRev       Revision number to get age for.
     */
    function getAge($sRev)
    {
        if (!isset($this->aaasRevs[$sRev]["date"][0]))
            return "<i>error</i>";

        $sDate = $this->aaasRevs[$sRev]["date"][0];
        $sCurDate = date("Y.m.d.H.i.s");
        if ($sDate > $sCurDate)
            return "0 seconds"; //fixme?

        /* seconds */
        $i1 = substr($sCurDate, 17, 2);
        $i2 = substr($sDate, 17, 2);
        if ($fBorrow = ($i1 < $i2))
            $i1 += 60;
        $iSeconds = $i1 - $i2;

        /* minuttes */
        $i1 = substr($sCurDate, 14, 2);
        $i2 = substr($sDate, 14, 2);
        if ($fBorrow)
            $i1--;
        if ($fBorrow = ($i1 < $i2))
            $i1 += 60;
        $iMinuttes = $i1 - $i2;

        /* hours */
        $i1 = substr($sCurDate, 11, 2);
        $i2 = substr($sDate, 11, 2);
        if ($fBorrow)
            $i1--;
        if ($fBorrow = ($i1 < $i2))
            $i1 += 24;
        $iHours = $i1 - $i2;

        /* days */
        $i1 = substr($sCurDate, 8, 2);
        $i2 = substr($sDate, 8, 2);
        if ($fBorrow)
            $i1--;
        if ($fBorrow = ($i1 < $i2))
        {
            $iM = substr($sCurDate, 5, 2);
            $iY = substr($sCurDate, 0, 4);
            if ($iM == 1 || $iM == 3 || $iM == 5 || $iM == 7 || $iM == 8 || $iM == 10 || $iM == 12)
                $i1 += 31;
            else if ($iM == 4 || $iM == 6 || $iM == 9 || $iM == 11)
                $i1 += 30;
            else if (($iY % 4) != 0 || (($iY % 100) == 0 && ($iY % 1000) != 0))
                $i1 += 28;
            else
                $i1 += 29;
        }
        $iDays = $i1 - $i2;

        /* months */
        $i1 = substr($sCurDate, 5, 2);
        $i2 = substr($sDate, 5, 2);
        if ($fBorrow)
            $i1--;
        if ($fBorrow = ($i1 < $i2))
            $i1 += 12;
        $iMonths = $i1 - $i2;

        /* years */
        $i1 = substr($sCurDate, 0, 4);
        $i2 = substr($sDate, 0, 4);
        if ($fBorrow)
            $i1--;
        $iYears = $i1 - $i2;

        //printf("<!-- $sCurDate - $sDate = %04d.%02d.%02d.%02d.%02d.%02d -->\n", $iYears, $iMonths, $iDays, $iHours, $iMinuttes, $iSeconds);

        /* make output */
        if ($iYears > 0)
            return "$iYears year".($iYears > 1 ? "s" : "")." $iMonths month".($iMonths > 1 ? "s" : "");
        if ($iMonths > 0)
            return "$iMonths month".($iMonths > 1 ? "s" : "")." $iDays day".($iDays > 1 ? "s" : "");
        if ($iDays > 0)
            return "$iDays day".($iDays > 1 ? "s" : "")." $iHours hour".($iHours > 1 ? "s" : "");
        if ($iHours > 0)
            return "$iHours hour".($iHours > 1 ? "s" : "")." $iMinuttes min";
        if ($iMinuttes > 0)
            return "$iMinuttes min $iSeconds sec";
       return "$iSeconds seconds";
    }

}


/**
 * This function displayes the contents of an directory.
 */
function ListDirectory($sDir, $iSortColumn)
{
    global $sCVSROOT;
    $timer = Odin32DBTimerStart("List Directory");

    /*
     * Validate and fixup $sDir.
     * Note that relative .. is not allowed!
     */
    $sDir = str_replace("\\", "/", $sDir);
    if ($sDir == "")
        $sDir = ".";
    if ($sDir[0] == '/')
        $sDir = substr($sDir, 1);
    if ($sDir[strlen($sDir)-1] == '/')
        $sDir = substr($sDir, 0, strlen($sDir) - 1);
    if ((strlen($sDir) == 2 && $sDir == "..")
        ||
        (substr($sDir, 0, 3) == "../")
        ||
        (substr($sDir, strlen($sDir)-3) == "/..")
        ||
        (strpos($sDir, "/../") > 0)
        )
        {
        echo "<!-- Invalid parameter: \$sDir $sDir -->\n";
        echo "<i>Invalid parameter: \$sDir $sDir </i>\n";
        return 87;
        }

    /*
     * Open the directory, read the contents into two arrays;
     *  one for files and one for directories. All files which
     *  don't end with ',v' are ignored.
     */
    $hDir = opendir($sCVSROOT.'/'.$sDir);
    if (!$hDir)
    {
        echo "<!-- debug error opendir($sDir) failed -->\n";
        echo "<i>debug error opendir($sDir) failed</i>\n";
        return 5;
    }

    $asFiles = array();
    $asSubDirs = array();
    while ($sEntry = readdir($hDir))
    {
        if (is_dir($sCVSROOT.'/'.$sDir.'/'.$sEntry))
        {
            if ($sEntry != '..' && $sEntry != '.')
                $asSubDirs[] = $sEntry;
        }
        else
        {
            $cchEntry = strlen($sEntry);
            if ($cchEntry > 2 && substr($sEntry, $cchEntry - 2,  2) == ',v')
                $asFiles[$sEntry] = $sEntry;
        }
    }
    closedir($hDir);

    /*
     * Get CVS data.
     */
    $cvstimer = Odin32DBTimerStart("Get CVS Data");
    $asRev      = array();
    $asAge      = array();
    $asAuthor   = array();
    $asLog      = array();
    for ($i = 0; list($sKey, $sFile) = each($asFiles); $i++)
    {
        $obj = new CVSFile($sDir.'/'.$sFile, 1);
        if ($obj->fOk)
        {
            $asRev[$sFile]    = $sRev = $obj->getHead();
            $asAge[$sFile]    = $obj->getAge($sRev);
            $asAuthor[$sFile] = $obj->getAuthor($sRev);
            $asTmpLog         = $obj->getLog($sRev);
            for ($sLog = "", $j = sizeof($asTmpLog) - 1; $j >= 0; $j--)
            {
                if ($sLog == "")
                {
                    if (trim($asTmpLog[$j]) != "")
                        $sLog = $asTmpLog[$j];
                    continue;
                }
                $sLog = $asTmpLog[$j]."<br>".$sLog;
            }
            $asLog[$sFile] = $sLog;
            $sLog = "";
        }
        else
            $asLog[$sFile]    = $obj->sError;
    }
    Odin32DBTimerStop($cvstimer);

    /*
     * Sort the stuff.
     */
    sort($asSubDirs);
    switch ($iSortColumn)
    {
        case 0:     $asSorted = $asFiles; break;
        case 1:     $asSorted = $asRev; break;
        case 2:     $asSorted = $asAge; break;
        case 3:     $asSorted = $asAuthor; break;
        case 4:     $asSorted = $asLog; break;
        default:    $asSorted = $asFiles; break;
    }
    asort($asSorted);


    /*
     * Present data
     */
    $aColumnColors = array("#cccccc","#cccccc","#cccccc","#cccccc", "#88ff88","#cccccc","#cccccc","#cccccc","#cccccc");
    echo "<table border=0 width=100% cellspacing=1 cellpadding=2>\n",
         "  <hr NOSHADE>\n",
         "    <th bgcolor=",$aColumnColors[4+0-$iSortColumn],"><font size=-1><b><a href=cvs.phtml?sDir=$sDir&iSortColumn=0>Filename</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+1-$iSortColumn],"><font size=-1><b><a href=cvs.phtml?sDir=$sDir&iSortColumn=1>Rev</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+2-$iSortColumn],"><font size=-1><b><a href=cvs.phtml?sDir=$sDir&iSortColumn=2>Age</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+3-$iSortColumn],"><font size=-1><b><a href=cvs.phtml?sDir=$sDir&iSortColumn=3>Author</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+4-$iSortColumn],"><font size=-1><b><a href=cvs.phtml?sDir=$sDir&iSortColumn=4>Last Log Entry</a></b></font></th>\n",
         "  </hr>\n";
    $i = 0;
    /* directories */
    if ($sDir != "." && $sDir != "")
    {
        if (($j = strrpos($sDir, '/')) > 0)
            $sParentDir = substr($sDir, 0, $j - 1);
        else
            $sParentDir = "";
        $sBgColor = ($i++ % 2) ? "" : " bgcolor=#ccccee";
        echo "<tr>\n",
             " <td", $sBgColor , ">",
               "<font size=-1><a href=\"cvs.phtml?sDir=",$sParentDir,"\"><img src=\"cvsicons/parent.gif\" border=0> Parent Directory</a></font></td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             "</tr>\n";
    }
    while (list($sKey, $sVal) = each($asSubDirs))
    {
        $sBgColor = ($i++ % 2) ? "" : " bgcolor=#ccccee";
        echo "<tr>\n",
             " <td$sBgColor><font size=-1><a href=\"cvs.phtml?sDir=$sDir/$sVal\"><img src=\"cvsicons/dir.gif\" border=0> $sVal</a></font></td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             "</tr>\n";
    }

    /* files */
    while (list($sKey, $sVal) = each($asSorted))
    {
        $sBgColor = ($i++ % 2) ? "" : " bgcolor=#ccccee";
        $sRev   = isset($asRev[$sKey])  ? $asRev[$sKey]     : "<i> error </i>";
        $sAge   = isset($asAge[$sKey])  ? $asAge[$sKey]     : "<i> error </i>";
        $sAuthor= isset($asAuthor[$sKey])?$asAuthor[$sKey]  : "<i> error </i>";
        $sLog   = isset($asLog[$sKey])  ?$asLog[$sKey]      : "<i> error </i>";
        echo "<tr>\n",
             " <td$sBgColor><font size=-1><a href=\"cvs.phtml?sFile=$sDir/$sKey\"><img src=\"cvsicons/file.gif\" border=0>",substr($sKey, 0, -2),"</a></font></td>\n",
             " <td$sBgColor><font size=-2><a href=\"cvs.phtml?sFile=$sDir/$sKey?sRev=$sRev\">$sRev</a></font></td>\n",
             " <td$sBgColor><font size=-2>$sAge</font></td>\n",
             " <td$sBgColor><font size=-2>$sAuthor</font></td>\n",
             " <td$sBgColor><font size=-2>$sLog</font></td>\n",
             "</tr>\n";
    }

    echo "</table>\n";
    Odin32DBTimerStop($timer);


    /*
     * Debug dump.
     *//*
    while (list ($sKey, $sVal) = each ($asSubDirs))
        echo "Dir: $sVal<br>\n";
    while (list ($sKey, $sVal) = each ($asFiles))
        echo "File: $sVal<br>\n";
    */
}


/**
 * Copies the first word.
 * A words is: [a-zA-Z0-9_.]
 *
 * tested ok
 * @returns     Returns the word at the start of $s.
 */
function CopyWord($s)
{
    $cch = strlen($s);
    for ($i = 0; $i < $cch; $i++)
    {
        $c = $s[$i];
        if (!($c >= 'a'  && $c <= 'z')
            &&
            !($c >= 'A'  && $c <= 'Z')
            &&
            !($c >= '0'  && $c <= '9')
            &&
            !($c == '.'  || $c == '_')
            )
            break;
    }
    return substr($s, 0, $i);
}


/**
 * Skips the first word.
 * A words is: [a-zA-Z0-9_.]
 *
 * tested ok
 * @returns     $s - first word.
 */
function SkipWord($s)
{
    $cch = strlen($s);
    for ($i = 0; $i < $cch; $i++)
    {
        $c = $s[$i];
        if (!($c >= 'a'  && $c <= 'z')
            &&
            !($c >= 'A'  && $c <= 'Z')
            &&
            !($c >= '0'  && $c <= '9')
            &&
            !($c == '.'  || $c == '_')
            )
            break;
    }
    return substr($s, $i);
}




/*
 * C color encoding.
 */
$aC_Keywords = array(
//    "auto" => 1,
    "break" => 1,
    "case" => 1,
    "char" => 1,
    "const" => 1,
    "continue" => 1,
    "default" => 1,
//    "defined" => 1,
    "do" => 1,
    "double" => 1,
    "else" => 1,
    "enum" => 1,
    "extern" => 1,
    "float" => 1,
    "for" => 1,
    "goto" => 1,
    "if" => 1,
    "int" => 1,
    "long" => 1,
    "register" => 1,
    "return" => 1,
    "short" => 1,
    "sizeof" => 1,
    "static" => 1,
    "struct" => 1,
    "switch" => 1,
    "typedef" => 1,
    "union" => 1,
    "unsigned" => 1,
    "void" => 1,
    "while" => 1,
    "class" => 1,
    "delete" => 1,
//    "finally" => 1,
    "friend" => 1,
    "inline" => 1,
    "new" => 1,
    "operator" => 1,
    "overload" => 1,
    "private" => 1,
    "protected" => 1,
    "public" => 1,
    "this" => 1,
    "virtual" => 1,
//    "bool" => 1,
//    "true" => 1,
//    "false" => 1,
    "explicit" => 1,
    "mutable" => 1,
    "typename" => 1,
//    "static_cast" => 1,
//    "const_cast" => 1,
//    "reinterpret_cast" => 1,
//    "dynamic_cast" => 1,
//    "using" => 1,
    "typeid" => 1,
//    "asm" => 1,
    "catch" => 1,
    "signed" => 1,
    "template" => 1,
    "throw" => 1,
    "try" => 1,
//    "namespace" => 1,
    "volatile" => 1

    );

$aC_Symbols = array(
    "{" => 1,
    "}" => 1,
//    "[" => 1,
//    "]" => 1,
//    "(" => 1,
//    ")" => 1,
//    "." => 1,
//    "," => 1,
    "!" => 1,
    "%" => 1,
//    "&" => 1,
    "&amp;" => 1,
    "*" => 1,
    "-" => 1,
    "=" => 1,
    "+" => 1,
    ":" => 1,
    ";" => 1,
//    "<" => 1,
    "&lt;" => 1,
//    ">" => 1,
    "&gt;" => 1,
    "?" => 1,
    "/" => 1,
    "|" => 1,
    "~" => 1,
    "^" => 1,
    "*" => 1);

/**
 * Initiate the variable array used by the C Color encoder.
 * @param       $aVaraibles     Variable array. (output)
 */
function C_ColorInit(&$aVariables)
{
    global $aC_Keywords;
    global $aC_Symbols;

    $aVariables["fComment"] = 0;

    ksort($aC_Keywords);
    ksort($aC_Symbols);
}


/**
 * Encode a line of C code.
 * @param       $sLine          Line string to encode.
 * @param       $aVariables     Variable array.
 * @returns     Color encoded line string.
 */
function C_ColorEncode($sLine, &$aVariables)
{
    global $aC_Keywords;
    global $aC_Symbols;

    $sRet = "";
    $cchLine = strlen($sLine);

    /*
     * If mulitline comment we'll only check if it ends at this line.
     * if it doesn't we'll do nothing.
     * if it does we'll skip to then end of it.
     */
    if ($aVariables["fComment"])
    {
        if (!(($i = strpos($sLine, "*/")) || ($cchLine >= 2 && $sLine[0] == '*' && $sLine[1] == '/')))
            return $sLine;
        $i += 2;
        $sRet = substr($sLine, 0, $i)."</font>";
        $aVariables["fComment"] = 0;
    }
    else
        $i = 0;

    /*
     * Loop thru the (remainings) of the line.
     */
    $fFirstNonBlank = 1;
    while ($i < $cchLine)
    {
        $ch = $sLine[$i];
        /* comment check */
        if ($i+1 < $cchLine && $ch == '/')
        {
            if ($sLine[$i+1] == '/')
            {   /* one-line comment */
                return $sRet . "<font color=#02FE02>" . substr($sLine, $i) . "</font>";
            }

            if ($sLine[$i+1] == '*')
            {   /* Start of multiline comment */
                if ($j = strpos($sLine, "*/", $i + 2))
                {
                    $sRet .= "<font color=#02FE02>" . substr($sLine, $i, $j+2 - $i) . "</font>";
                    $i = $j + 2;
                }
                else
                {
                    $aVariables["fComment"] = 1;
                    return $sRet . "<font color=#02FE02>" . substr($sLine, $i);
                }
                continue;
            }
        }

        /*
         * Check for string.
         */
        if ((($fDbl = (/*$sLine[$i] == '"' ||*/ substr($sLine, $i, 6) == "&quot;")) || $sLine[$i] == "'")
             && ($i == 0 || $sLine[$i-1] != '\\'))
        {   /* start of a string */
            $j = $i + 1;
            if ($fDbl)
            {
               /* if ($sLine[$i] == '"')
                    while ($j < $cchLine && $sLine[$j] != '"')
                        $j += ($sLine[$j] == '\\') ? 2 : 1;
                else */
                {
                    while ($j < $cchLine && ($sLine[$j] != '&' || substr($sLine, $j, 6) != "&quot;"))
                        $j += ($sLine[$j] == '\\') ? 2 : 1;
                    if ($j < $cchLine)
                        $j += 5;
                }
            }
            else
                while ($j < $cchLine && $sLine[$j] != "'")
                    $j += ($sLine[$j] == '\\') ? 2 : 1;
            $j++;
            $sRet .= "<font color=#FEFE02>".substr($sLine, $i, $j - $i)."</font>";
            $i = $j;
            continue;
        }

        /*
         * Check for preprocessor directive.
         */
        if ($fFirstNonBlank && $ch == "#")
        {
            $j = $i + 1;
            while ($j < $cchLine && ($sLine[$j] == ' ' || $sLine[$j] == '\t'))
                $j++;
            $j += C_WordLen($sLine, $cchLine, $j);
            $sRet .= "<font color=#CECECE>" . substr($sLine, $i, $j - $i) . "</font>";
            $i = $j;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * If non-blank, lets check if we're at the start of a word...
         */
        $fBlank = ($ch == ' ' || $ch == '\t'); //TODO more "blanks"?
        if ($fFirstNonBlank)    $fFirstNonBlank = $fBlank;
        $cchWord = !$fBlank ? C_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            /*
             * Check for keyword or number.
             */
            if ($cchWord > 0 && (isset($aC_Keywords[substr($sLine, $i, $cchWord)]) || ($ch >= '0' && $ch <= '9')))
                $sRet .= "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";

            /*
             * Skip word.
             */
            else
                $sRet .= substr($sLine, $i, $cchWord);
            $i += $cchWord;
            continue;
        }


        /*
         * Prepare for symbol check. (we'll have to check for HTML stuff like &amp;).
         */
        $cchWord = 1;
        if ($ch == '&')
        {
            /*
            while ($cchWord < 8 && $sLine[$i+$cchWord] != ';' &&
                    (   ($sLine[$i+$cchWord] >= 'a' && $sLine[$i+$cchWord] <= 'z')
                     || ($sLine[$i+$cchWord] >= 'A' && $sLine[$i+$cchWord] <= 'Z')
                    )
                   )
                   $cchWord++;

            if ($sLine[$i + $cchWord++] != ';')
                $cchWord = 1;
            */
            if (substr($sLine, $i, 5) == "&amp;")
                $cchWord = 5;
            else if (substr($sLine, $i, 4) == "&gt;" || substr($sLine, $i, 4) == "&lt;")
                $cchWord = 4;
        }

        /*
         * Check for Symbol.
         */
        if (isset($aC_Symbols[substr($sLine, $i, $cchWord)]))
        {
            $sRet .= "<font color=#CECECE>" . substr($sLine, $i, $cchWord) . "</font>";
            $i += $cchWord;
            continue;
        }


        /*
         * Copy char
         */
        $sRet .= $sLine[$i];
        $i++;
    }

    return $sRet;
}


/**
 * Encode a line of C code.
 * @param       $sLine          Line string to encode.
 * @param       $aVariables     Variable array.
 * @returns     Color encoded line string.
 */
function C_ColorEncode2($sLine, &$aVariables)
{
    global $aC_Keywords;
    global $aC_Symbols;

    $cchLine = strlen($sLine);

    /*
     * If mulitline comment we'll only check if it ends at this line.
     * if it doesn't we'll do nothing.
     * if it does we'll skip to then end of it.
     */
    if ($aVariables["fComment"])
    {
        if (!(($i = strpos($sLine, "*/")) || ($cchLine >= 2 && $sLine[0] == '*' && $sLine[1] == '/')))
        {
            echo $sLine;
            return;
        }
        $i += 2;
        echo substr($sLine, 0, $i)."</font>";
        $aVariables["fComment"] = 0;
    }
    else
        $i = 0;

    /*
     * Loop thru the (remainings) of the line.
     */
    $fFirstNonBlank = 1;
    while ($i < $cchLine)
    {
        $ch = $sLine[$i];
        /* comment check */
        if ($i+1 < $cchLine && $ch == '/')
        {
            if ($sLine[$i+1] == '/')
            {   /* one-line comment */
                echo "<font color=#02FE02>" . substr($sLine, $i) . "</font>";
                return;
            }

            if ($sLine[$i+1] == '*')
            {   /* Start of multiline comment */
                if ($j = strpos($sLine, "*/", $i + 2))
                {
                    echo "<font color=#02FE02>" . substr($sLine, $i, $j+2 - $i) . "</font>";
                    $i = $j + 2;
                }
                else
                {
                    $aVariables["fComment"] = 1;
                    echo "<font color=#02FE02>" . substr($sLine, $i);
                    return;
                }
                continue;
            }
        }

        /*
         * Check for string.
         */
        if ((($fDbl = (/*$sLine[$i] == '"' ||*/ substr($sLine, $i, 6) == "&quot;")) || $sLine[$i] == "'")
             && ($i == 0 || $sLine[$i-1] != '\\'))
        {   /* start of a string */
            $j = $i + 1;
            if ($fDbl)
            {
               /* if ($sLine[$i] == '"')
                    while ($j < $cchLine && $sLine[$j] != '"')
                        $j += ($sLine[$j] == '\\') ? 2 : 1;
                else */
                {
                    while ($j < $cchLine && ($sLine[$j] != '&' || substr($sLine, $j, 6) != "&quot;"))
                        $j += ($sLine[$j] == '\\') ? 2 : 1;
                    if ($j < $cchLine)
                        $j += 5;
                }
            }
            else
                while ($j < $cchLine && $sLine[$j] != "'")
                    $j += ($sLine[$j] == '\\') ? 2 : 1;
            $j++;
            echo "<font color=#FEFE02>".substr($sLine, $i, $j - $i)."</font>";
            $i = $j;
            continue;
        }

        /*
         * Check for preprocessor directive.
         */
        if ($fFirstNonBlank && $ch == "#")
        {
            $j = $i + 1;
            while ($j < $cchLine && ($sLine[$j] == ' ' || $sLine[$j] == '\t'))
                $j++;
            $j += C_WordLen($sLine, $cchLine, $j);
            echo "<font color=#CECECE>" . substr($sLine, $i, $j - $i) . "</font>";
            $i = $j;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * If non-blank, lets check if we're at the start of a word...
         */
        $fBlank = ($ch == ' ' || $ch == '\t'); //TODO more "blanks"?
        if ($fFirstNonBlank)    $fFirstNonBlank = $fBlank;
        $cchWord = !$fBlank ? C_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            /*
             * Check for keyword or number.
             */
            if ($cchWord > 0 && (isset($aC_Keywords[substr($sLine, $i, $cchWord)]) || ($ch >= '0' && $ch <= '9')))
                echo "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";

            /*
             * Skip word.
             */
            else
                echo substr($sLine, $i, $cchWord);
            $i += $cchWord;
            continue;
        }


        /*
         * Prepare for symbol check. (we'll have to check for HTML stuff like &amp;).
         */
        $cchWord = 1;
        if ($ch == '&')
        {
            /*
            while ($cchWord < 8 && $sLine[$i+$cchWord] != ';' &&
                    (   ($sLine[$i+$cchWord] >= 'a' && $sLine[$i+$cchWord] <= 'z')
                     || ($sLine[$i+$cchWord] >= 'A' && $sLine[$i+$cchWord] <= 'Z')
                    )
                   )
                   $cchWord++;

            if ($sLine[$i + $cchWord++] != ';')
                $cchWord = 1;
            */
            if (substr($sLine, $i, 5) == "&amp;")
                $cchWord = 5;
            else if (substr($sLine, $i, 4) == "&gt;" || substr($sLine, $i, 4) == "&lt;")
                $cchWord = 4;
        }

        /*
         * Check for Symbol.
         */
        if (isset($aC_Symbols[substr($sLine, $i, $cchWord)]))
        {
            echo "<font color=#CECECE>" . substr($sLine, $i, $cchWord) . "</font>";
            $i += $cchWord;
            continue;
        }


        /*
         * Copy char
         */
        echo $ch;
        $i++;
    }

    return;
}


/**
 * Calculates the lenght of the word which eventually starts at [$i].
 * @param       $sLine      Line.
 * @param       $cchLine    Line length.
 * @param       $i          Line index.
 * @returns     Word length.
 */
function C_WordLen($sLine, $cchLine, $i)
{

    /*
     * Check that previous letter wasen't a possible
     * word part.
     */
    if ($i > 0)
    {
        $ch = $sLine[$i - 1];
        if (    ($ch >= 'a' && $ch <= 'z')
             || ($ch >= 'A' && $ch <= 'Z')
             || ($ch >= '0' && $ch <= '9')
             || ($ch == '_')
             || ($ch == '$')
            )
            return 0;
    }

    /*
     * Count letters in the word
     */
    $j = $i;
    $ch = $sLine[$i];
    while ($i < $cchLine &&
           (   ($ch >= 'a' && $ch <= 'z')
            || ($ch >= 'A' && $ch <= 'Z')
            || ($ch >= '0' && $ch <= '9')
            || ($ch == '_')
            || ($ch == '$')
            )
           )
        $ch = @$sLine[++$i];
    return $i - $j;
}

?>

