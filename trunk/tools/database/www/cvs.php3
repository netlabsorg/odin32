<?php

require "Odin32DBHelpers.php3";

/*
 * Configuration:
 */
$sCVSROOT  = "g:/cvsroot";
$sCVSROOT  = "e:/netlabs.cvs/odin32";
$sIconsUrl = "/icons";
$sIconsUrl = "http://www.netlabs.org/icons";

echo '<!-- cvsroot:'.$sCVSROOT.' -->';

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
        if (strlen($sFilename) < 3 || substr($sFilename, -2) != ",v")
        {
            $this->sError = "filename is invalid";
            return 1;
        }
        $sFilename = str_replace("\\", "/", $sFilename);
        if ((substr($sFilename, 0, 3) == "../")
            ||
            (substr($sFilename, -3) == "/..")
            ||
            (strpos($sFilename, "/../") > 0)
            )
            {
            $this->sError = "Invalid parameter: \$sFilename $sFilename";
            return 87;
            }
        if ($sFilename[0] == '/')
            $sFilename = ".".$sFilename;
        else if (substr($sFilename, 0, 2) != "./")
            $sFilename = "./".$sFilename;

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
        $Timer = Odin32DBTimerStart("CVS Parse");
        $this->fOk = $this->ParseFile($hFile, $fNoDeltas);
        Odin32DBTimerStop($Timer);

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
                 * Reparse the value.
                 */
                case "symbols":
                    $asValue2 = $asValue;
                    $asValue = array();
                    while (list ($sIgnore, $sVal) = each ($asValue2))
                    {
                        if (($sVal = trim($sVal)) != "")
                        {
                            if ($iColon = strpos($sVal, ":"))
                                $asValue[substr($sVal, 0, $iColon)] = substr($sVal, $iColon+1);
                            else
                                echo "\n<!-- error in symbols parsing -->\n";
                        }
                    }
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
                if (sizeof($this->aasKeys) <= 0 //sanity check! head must come first and have a value!
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
     * Color coding is enabled.
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

        /*
         * Make header
         */
        echo "<table><tr><td bgcolor=#f0f0f0>\n<table>";
        //file info
        echo "<tr><td valign=top><font size=-1>File:</font></td>\n",
             "<td><font size=-1>", $this->getDirUrls(), " / <a href=\"cvs.php?sFile=$this->sDir/$this->sName,v\">$this->sName</a></font></td></tr>\n";

        //revision info
        echo "<tr><td valign=top><font size=-1>Revision:</font></td>\n",
             "<td><font size=-1>$sRevision, ",
             CVSFormatDate($this->getDate($sRevision)),
             " (",CVSGetAge($this->getDate($sRevision), 6), ")",
             "<br>by ", $this->getAuthor($sRevision),
             "</font></td></tr>\n";

        //branch info
        echo "<tr><td valign=top><font size=-1>Branch:</font></td>\n",
             "<td><font size=-1>", $this->getBranch($sRevision),
             "</font></td></tr>\n";

        //tag info
        echo "<tr><td valign=top><font size=-1>Tags:</font></td>\n",
             "<td><font size=-1>", $this->getTags($sRevision),
             "</font></td></tr>\n";

        //log info
        $asLog = $this->getLog($sRevision);
        echo "<tr><td valign=top><font size=-1>Log:</font></td>",
             "<td><font size=-1>\n";
        if (isset($asLog))
            while (list($sKey, $s) = each ($asLog))
                echo $s; //this should be <pre> but, that will need some line wrapping...
                //echo str_replace("\n", "<br>", $s), "\n"; //this should be <pre> but, that will need some line wrapping...
        echo "</font></td><tr>\n";

        echo "</table>\n";//<hr noshade>\n";


        /*
         * Initiate the color encoder.
         */
        switch (strtolower($this->sExt))
        {
            case "c":
            case "cpp":
            case "cxx":
            case "h":
            case "hpp":
                C_ColorInit($aVariables);
                $iColorEncoder = 1;
                break;

            case "asm":
            case "inc":
            case "s":
                ASM_ColorInit($aVariables);
                $iColorEncoder = 2;
                break;

            case "mk":
            case "mak":
                Make_ColorInit($aVariables);
                $iColorEncoder = 3;
                break;

            default:
                if (strtolower($this->sName) == "makefile")
                {
                    Make_ColorInit($aVariables);
                    $iColorEncoder = 3;
                    break;
                }
                $iColorEncoder = 0;
        }



        /*
         * Write it!
         */
        $Timer = Odin32DBTimerStart("Write timer");
        echo "<tr><td bgcolor=#020286><pre><font size=-0 face=\"System VIO, System Monospaced\" color=#02FEFE>";
        if ($sRevision == $this->aasKeys["head"][0])
        {
            //head revision
            for ($cLines = sizeof($this->aasDeltas[$sRevision]), $iLine = 0;
                 ($iLine < $cLines);
                 $iLine++)
            {
                /*
                 * Preprocessing... Color coding
                 */
                echo "<a name=$iLine>";
                switch ($iColorEncoder)
                {
                    case 1:
                        echo  str_replace("\t", "    ", C_ColorEncode(htmlspecialchars($this->aasDeltas[$sRevision][$iLine]), $aVariables));
                        break;
                    case 2:
                        echo  str_replace("\t", "    ", ASM_ColorEncode(htmlspecialchars($this->aasDeltas[$sRevision][$iLine]), $aVariables));
                        break;
                    case 3:
                        echo  str_replace("\t", "    ", Make_ColorEncode(htmlspecialchars($this->aasDeltas[$sRevision][$iLine]), $aVariables));
                        break;
                    default:
                        echo  str_replace("\t", "    ", htmlspecialchars($this->aasDeltas[$sRevision][$iLine]));
                }
                echo "</a>";
            }
        }
        else
        {
            //build revision
            $sRev = $this->aasKeys["head"][0];
            $asText = $this->aasDeltas[$sRev];
            do
            {
                /*
                 * determin revision.
                 * (hope this works ok...)
                 */
                $sPrevRev = $sRev;
                $asBranches = $this->aaasRevs[$sRev]["branches"];
                for ($f = 0, $i = 0; $i < sizeof($asBranches); $i++)
                {
                    if (($sB = trim($asBranches[$i])) != "")
                    {
                        $sB2 = substr($sB, 0, strrpos($sB, ".") + 1);
                        if ($f = ($sB2 == substr($sRevision, 0, strlen($sB2))))
                        {
                            $sRev = $sB;
                            break;
                        }
                    }
                }

                if (!$f)    $sRev = $this->aaasRevs[$sRev]["next"][0];
                echo "<!-- \$sRev=$sRev -->";


                /*
                 * Apply the delta.
                 */
                $asOrg  = $asText;
                $asText = array();
                $iOrg   = 0;
                $cOrg   = sizeof($asOrg);
                $iDelta = 0;
                $cDelta = sizeof($this->aasDeltas[$sRev]);
                $iText  = 0;
                while ($cDelta > $iDelta)
                {
                    //get the next diff chunk
                    $iDiff = (int)substr($this->aasDeltas[$sRev][$iDelta], 1, strpos($this->aasDeltas[$sRev], " ") - 1) - 1;

                    //skip to it
                    while ($iDiff > $iOrg && $iOrg < $cOrg)
                        $asText[$iText++] = $asOrg[$iOrg++];

                    //apply it
                    $c = (int)substr($this->aasDeltas[$sRev][$iDelta],
                                     (int)strpos($this->aasDeltas[$sRev][$iDelta], " ") + 1);
                    if ($this->aasDeltas[$sRev][$iDelta][0] == 'a')
                    {
                        $iDelta++;
                        while ($iDelta < $cDelta && $c-- > 0)
                            $asText[$iText++] = $this->aasDeltas[$sRev][$iDelta++];

                        while ($c-- > 0)
                            $asText[$iText++] = "";
                    }
                    else
                    {
                        $iDelta++;
                        $iOrg += $c;
                    }
                }

                //copy remaining
                while ($iOrg < $cOrg)
                    $asText[$iText++] = $asOrg[$iOrg++];

            } while ($sRev != "" && $sRev != $sRevision);

            /*
             * Print it
             */
            for ($cLines = sizeof($asText), $iLine = 0; $iLine < $cLines; $iLine++)
            {
                /*
                 * Preprocessing... Color coding
                 */
                echo "<a name=$iLine>";
                switch ($iColorEncoder)
                {
                    case 1:
                        echo  str_replace("\t", "    ", C_ColorEncode(htmlspecialchars($asText[$iLine]), $aVariables));
                        break;
                    case 2:
                        echo  str_replace("\t", "    ", ASM_ColorEncode(htmlspecialchars($asText[$iLine]), $aVariables));
                        break;
                    case 3:
                        echo  str_replace("\t", "    ", Make_ColorEncode(htmlspecialchars($asText[$iLine]), $aVariables));
                        break;
                    default:
                        echo  str_replace("\t", "    ", htmlspecialchars($asText[$iLine]));
                }
                echo "</a>";
            }
        }
        echo "                                                                               \n", //80-columns line
             "</pre></td></tr></table>\n";
        Odin32DBTimerStop($Timer);

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
        if (($sDate = @$this->aaasRevs[$sRev]["date"][0]) != ""
            && $sDate[2] == "." //check for two digit date
            )
            return "19".$sDate;
        return $sDate;
    }

    /**
     * Get the workfile extention.
     * @returns     The workfile extention (without '.').
     */
    function getWorkfileExt()
    {
        return $this->sExt;
    }

    /**
     * Get the workfile extention.
     * @returns     The workfile name (with extention)
     */
    function getWorkfileName()
    {
        return $this->sName;
    }

    /**
     * Is this a binary file? We'll simply check for the expand keyword.
     * @returns     True (1) if binary, false (0) if not.
     */
    function isBinary()
    {
        return isset($this->aasKeys["expand"]);
    }

    /**
     * Get loginfo for the given revision.
     * @returns     Array of log info for the given revision.
     * @param       $sRev       Revision number to get loginfo for.
     */
    function getLog($sRev)
    {
        return @$this->aaasRevs[$sRev]["log"];
    }

    /**
     * Get the branch name for the given revision.
     * @return      Branch name.
     * @param       $sRev       Revision number to get branchname for.
     */
    function getBranch($sRev)
    {
        $aiDots = array();

        for ($i = strlen($sRev) - 1; $i >= 0; $i--)
            if ($sRev[$i] == ".")
                $aiDots[] = $i;

        if (sizeof($aiDots) == 1)
            return "MAIN";

        $sBranch = substr($sRev, 0, $aiDots[1]).".0";
        $cchBranch = strlen($sBranch);
        reset($this->aasKeys["symbols"]);
        while (list($sTag, $sTagRev) = each($this->aasKeys["symbols"]))
        {
            $j = strrpos($sTagRev, ".");
            //echo "<!-- $j  $cchBranch ($sBranch, ".substr($sTagRev, 0, $j).")-->";
            if ($cchBranch == $j && $sBranch == substr($sTagRev, 0, $j))
                return $sTag;
        }

        return "";
    }

    /**
     * Get the tag names associated with the given revision.
     * @return      comma separated list of tag names.
     * @param       $sRev       Revision number to get tag names for.
     */
    function getTags($sRev)
    {
        //didn't find a search function, so we'll do a linear search.
        //thru the symbols. Correct this when/if a array search function
        //is found.
        $sTags = "";
        if (isset($this->aasKeys["symbols"]))
        {
            $asSymbols = $this->aasKeys["symbols"];
            while (list($sTag, $sTagRev) = each($asSymbols))
                if ($sTagRev == $sRev)
                    $sTags = ", $sTag";
        }

        //check for head revision
        if ($sRev == $this->aasKeys["head"][0])
            $sTags = ", HEAD";

        return substr($sTags, 2); //remove ", "
    }

    /**
     * Get a directory string (for this file) where every level
     * is an URL to the page for it.
     * @return      URL directory string.
     */
    function getDirUrls()
    {
        return CVSGetDirUrls($this->sDir);
    }

    /**
     * Get changes string for this revision.
     * (Won't work for the head revision!)
     * @return      Changes string: "+x -y lines".
     * @param       $sRev   Revision which we're to count changes for.
     */
    function getChanges($sRev)
    {
        if (!isset($this->aasDeltas[$sRev]))
            return "<i>error</i>";
        if ($sRev == $this->aasKeys["head"][0])
            return "+0 -0 lines";
        $cAdd = 0;
        $cDelete = 0;

        for ($i = 0, $c = sizeof($this->aasDeltas[$sRev]); $i < $c; $i++)
        {
            $sLine = $this->aasDeltas[$sRev][$i];
            if ($sLine[0] == "d")
                $cDelete += (int)substr($sLine, strpos($sLine, " ") + 1);
            else if ($sLine[0] == "a")
            {
                $cAdd += (int)substr($sLine, strpos($sLine, " ") + 1);
                $i += $cLines;
            }
            else
                echo "<!-- hmm internal error in getChanges -->\n";
        }

        return "+$cDelete -$cAdd lines"; //note the deltas is for going the other way...
    }

    /**
     *
     * @return
     */
    function PrintAllInfo()
    {

        //file info
        echo "<font size=-1>", $this->getDirUrls(), " / $this->sName</font><p>\n",
             "\n";

        echo "<table>\n";
        //do we have to sort the array first? no...
        $i = 0; //max is 256!!! (ChangeLog,v is _very_ big).
        while ($i++ < 256 && list($sRevision, $aasRev) = each($this->aaasRevs))
        {
            echo "<tr><td bgcolor=#d0dce0><a name=\"$sRevision\">Rev. </a>",
                 "<a href=\"cvs.php?sFile=$this->sDir/$this->sName,v&sRevision=$sRevision\">",
                 "$sRevision</a> by ",
                 $this->getAuthor($sRevision) ,"</td></tr>\n",
                 "<tr><td bgcolor=#f0f0f0>";

            echo "<table>";
            //revision date info
            echo "<tr><td valign=top><font size=-1>Date:</font></td>\n",
                 "<td><font size=-1>",CVSFormatDate($this->getDate($sRevision)),
                 "<br>(",CVSGetAge($this->getDate($sRevision), 6), ")",
                 "</font></td></tr>\n";

            //branch info
            echo "<tr><td valign=top><font size=-1>Branch:</font></td>\n",
                 "<td><font size=-1>", $this->getBranch($sRevision),
                 "</font></td></tr>\n";

            //tag info
            echo "<tr><td valign=top><font size=-1>Tags:</font></td>\n",
                 "<td><font size=-1>", $this->getTags($sRevision),
                 "</font></td></tr>\n";

            //Changes info
            if (isset($aasRev["next"]) && ($sPrevRev = $aasRev["next"][0]) != "")
            {
                echo "<tr><td valign=top><font size=-1>Changes since $sPrevRev:</font></td>\n",
                     "<td><font size=-1>", $this->getChanges($sPrevRev),
                     "</font></td></tr>\n";
            }

            //log info
            $asLog = $this->getLog($sRevision);
            echo "<tr><td valign=top><font size=-1>Log:</font></td><td><font size=-1>\n";
            if (isset($asLog))
                while (list($sKey, $s) = each ($asLog))
                    echo $s; //this should be <pre> but, that will need some line wrapping...
                    //echo str_replace("\n", "<br>", $s), "\n"; //this should be <pre> but, that will need some line wrapping...
            echo "</font></td></tr>\n";

            echo "</table>\n";

            echo "</td></tr>\n";
        }

        echo "</table>\n";
        return 1;
    }
}


/**
 * Get a directory string where every level
 * is an URL to the page for it.
 * @return      URL directory string.
 * @param       Directory string to process.
 */
function CVSGetDirUrls($sDir)
{
    if ($sDir == "")
        $sDir = "./";
    else if (substr($sDir, -1) != "/")
        $sDir .= "/";

    $iPrev = 2;
    $sRet = "<a href=\"cvs.php?sDir=.\">[root]</a>";
    while ($i = @strpos($sDir, "/", $iPrev))
    {
        $sRet .= " / <a href=\"cvs.php?sDir=".substr($sDir, 0, $i)."\">".
                    substr($sDir, $iPrev, $i - $iPrev)."</a>";
        $iPrev = $i + 1;
    }

    return $sRet;
}



/**
 * Get a understandable date string from a CVS date.
 * @returns     Date string (human readable)
 * @param       $sDate      CVS date string. (as returned by getDate())
 */
function CVSFormatDate($sDate)
{
    $Time = mktime( substr($sDate,11, 2), //hour
                    substr($sDate,14, 2), //minute
                    substr($sDate,17, 2), //second
                    substr($sDate, 5, 2), //month
                    substr($sDate, 8, 2), //day
                    substr($sDate, 0, 4));//year
    return date("D M d h:i:s Y", $Time);
}


/**
 * Calculate the period between $sDate and the current date.
 * @returns     Age string. (human readable)
 * @param       $sDate      CVS date string. (as returned by getDate())
 * @param       $cLevels    Number of levels to be specified.
 */
function CVSGetAge($sDate, $cLevels)
{
    $sCurDate = date("Y.m.d.H.i.s");
    if ($sDate > $sCurDate)
        return "0 seconds"; //fixme?

    /* seconds */
    $i1 = substr($sCurDate, 17, 2);
    $i2 = substr($sDate, 17, 2);
    if ($fBorrow = ($i1 < $i2))
        $i1 += 60;
    $iSeconds = $i1 - $i2;

    /* minutes */
    $i1 = substr($sCurDate, 14, 2);
    $i2 = substr($sDate, 14, 2);
    if ($fBorrow)
        $i1--;
    if ($fBorrow = ($i1 < $i2))
        $i1 += 60;
    $iMinutes = $i1 - $i2;

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

    //printf("<!-- $sCurDate - $sDate = %04d.%02d.%02d.%02d.%02d.%02d -->\n", $iYears, $iMonths, $iDays, $iHours, $iMinutes, $iSeconds);


    /* make output */
    $sRet = "";
    if ($cLevels > 0 && $iYears > 0)
    {
        $cLevels--;
        $sRet .= "$iYears year".($iYears > 1 ? "s" : "");
    }
    if ($cLevels > 0 && $iMonths > 0)
    {
        $cLevels--;
        $sRet .= " $iMonths month".($iMonths > 1 ? "s" : "");
    }
    if ($cLevels > 0 && $iDays > 0)
    {
        $cLevels--;
        $sRet .= " $iDays day".($iDays > 1 ? "s" : "");
    }
    if ($cLevels > 0 && $iHours > 0)
    {
        $cLevels--;
        $sRet .= " $iHours hour".($iHours > 1 ? "s" : "");
    }
    if ($cLevels > 0 && $iMinutes > 0)
    {
        $cLevels--;
        $sRet .= " $iMinutes minute".($iHours > 1 ? "s" : "");
    }
    if ($cLevels > 0)
        $sRet .= " $iSeconds second".($iHours > 1 ? "s" : "");
    return ltrim($sRet);
}


/**
 * This function displayes the contents of an directory.
 */
function ListDirectory($sDir, $iSortColumn)
{
    global $sCVSROOT;
    global $sIconsUrl;
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
    if (substr($sDir, -1) == '/')
        $sDir = substr($sDir, 0,  - 1);
    if ((strlen($sDir) == 2 && $sDir == "..")
        ||
        (substr($sDir, 0, 3) == "../")
        ||
        (substr($sDir, -3) == "/..")
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
    printf('<!-- opendir('.$sCVSROOT.'/'.$sDir.'); -->');
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
    $aExtIcons = array(
            "c"     => "c.gif",
            "cpp"   => "c.gif",
            "cxx"   => "c.gif",
            "h"     => "c.gif",
            "hpp"   => "c.gif",
            "c"     => "c.gif",
            /* these are caught by the isBinary test.
            "exe"   => "binary.gif",
            "dll"   => "binary.gif",
            "lib"   => "binary.gif",
            "obj"   => "binary.gif",
            "a"     => "binary.gif",
            */
            "bmp"   => "image1.gif",
            "gif"   => "image1.gif",
            "ico"   => "image1.gif",
            "jpg"   => "image1.gif",
            "pal"   => "image1.gif",
            "png"   => "image1.gif",
            "asm"   => "text.gif",
            "def"   => "text.gif",
            "doc"   => "text.gif",
            "inc"   => "text.gif",
            "lib"   => "text.gif",
            "mak"   => "text.gif",
            "mk"    => "text.gif",
            "txt"   => "text.gif",
            ""      => "text.gif",
            "bat"   => "script.gif",
            "cmd"   => "script.gif",
            "perl"  => "script.gif",
            "sh"    => "script.gif"
            );
    $cvstimer = Odin32DBTimerStart("Get CVS Data");
    $asRev      = array();
    $asAge      = array();
    $asAuthor   = array();
    $asLog      = array();
    $asIcon     = array();
    for ($i = 0; list($sKey, $sFile) = each($asFiles); $i++)
    {
        $obj = new CVSFile($sDir.'/'.$sFile, 1);
        if ($obj->fOk)
        {
            $asRev[$sFile]    = $sRev = $obj->getHead();
            $asDate[$sFile]   = $obj->getDate($sRev);
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
            $asIcon[$sFile] = isset($aExtIcons[strtolower($obj->getWorkfileExt())])
                              ? $aExtIcons[strtolower($obj->getWorkfileExt())] :
                              ($obj->isBinary() ? "binary.gif" : "unknown.gif");
        }
        else
            $asLog[$sFile]    = $obj->sError;
    }
    Odin32DBTimerStop($cvstimer);

    /*
     * Write header
     */
    echo "<font size=-1>", CVSGetDirUrls(dirname($sDir)),
         ($sDir != "." ? " / ".substr($sDir, strrpos($sDir, "/") + 1) : ""),
         " /</font><p>\n";

    /*
     * Sort the stuff.
     */
    sort($asSubDirs);
    switch ($iSortColumn)
    {
        case 0:     $asSorted = $asFiles; break;
        case 1:     $asSorted = $asRev; break;
        case 2:     $asSorted = $asDate; break;
        case 3:     $asSorted = $asAuthor; break;
        case 4:     $asSorted = $asLog; break;
        default:    $asSorted = $asFiles; break;
    }
    asort($asSorted);


    /*
     * Present data
     */
    $aColumnColors = array("#d0dce0","#d0dce0","#d0dce0","#d0dce0", "#d0dcff","#d0dce0","#d0dce0","#d0dce0","#d0dce0");
    echo "<table border=0 width=100% cellspacing=1 cellpadding=2>\n",
         "  <tr>\n",
         "    <th bgcolor=",$aColumnColors[4+0-$iSortColumn],"><font size=-1><b><a href=cvs.php?sDir=$sDir&iSortColumn=0>Filename</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+1-$iSortColumn],"><font size=-1><b><a href=cvs.php?sDir=$sDir&iSortColumn=1>Rev</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+2-$iSortColumn],"><font size=-1><b><a href=cvs.php?sDir=$sDir&iSortColumn=2>Age</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+3-$iSortColumn],"><font size=-1><b><a href=cvs.php?sDir=$sDir&iSortColumn=3>Author</a></b></font></th>\n",
         "    <th bgcolor=",$aColumnColors[4+4-$iSortColumn],"><font size=-1><b><a href=cvs.php?sDir=$sDir&iSortColumn=4>Last Log Entry</a></b></font></th>\n",
         "  </tr>\n";
    $i = 0;
    /* directories */
    if ($sDir != "." && $sDir != "")
    {
        if (($j = strrpos($sDir, "/")) > 0)
            $sParentDir = substr($sDir, 0, $j);
        else
            $sParentDir = ".";
        $sBgColor = ($i++ % 2) ? "" : " bgcolor=#f0f0f0";
        echo "<tr>\n",
             " <td", $sBgColor , ">",
               "<font size=-1><a href=\"cvs.php?sDir=",$sParentDir,"\"><img src=\"$sIconsUrl/back.gif\" border=0> Parent Directory</a></font></td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             "</tr>\n";
    }
    while (list($sKey, $sVal) = each($asSubDirs))
    {
        $sBgColor = ($i++ % 2) ? "" : " bgcolor=#f0f0f0";
        echo "<tr>\n",
             " <td$sBgColor><font size=-1><a href=\"cvs.php?sDir=$sDir/$sVal\"><img src=\"$sIconsUrl/dir.gif\" border=0> $sVal</a></font></td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             " <td$sBgColor>&nbsp;</td>\n",
             "</tr>\n";
    }

    /* files */
    while (list($sKey, $sVal) = each($asSorted))
    {
        $sBgColor = ($i++ % 2) ? "" : " bgcolor=#f0f0f0";
        $sRev   = isset($asRev[$sKey])  ? $asRev[$sKey]             : "<i> error </i>";
        $sAge   = isset($asDate[$sKey]) ? CVSGetAge($asDate[$sKey], 1) : "<i> error </i>";
        $sAuthor= isset($asAuthor[$sKey])?$asAuthor[$sKey]          : "<i> error </i>";
        $sLog   = isset($asLog[$sKey])  ? $asLog[$sKey]             : "<i> error </i>";
        $sIcon  = isset($asIcon[$sKey]) ? $asIcon[$sKey]            : "<i> error </i>";
        echo "<tr>\n",
             " <td$sBgColor><font size=-1><a href=\"cvs.php?sFile=$sDir/$sKey\"><img src=\"$sIconsUrl/$sIcon\" border=0>",substr($sKey, 0, -2),"</a></font></td>\n",
             " <td$sBgColor><font size=-1><a href=\"cvs.php?sFile=$sDir/$sKey&sRevision=$sRev\">$sRev</a></font></td>\n",
             " <td$sBgColor><font size=-1>$sAge</font></td>\n",
             " <td$sBgColor><font size=-1>$sAuthor</font></td>\n",
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
            while ($j < $cchLine && ($sLine[$j] == " " || $sLine[$j] == "\t"))
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
        $fBlank = ($ch == " " || $ch == "\t" || $ch == "\n");
        if ($fFirstNonBlank)    $fFirstNonBlank = $fBlank;
        $cchWord = !$fBlank ? C_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            /*
             * Check for keyword or number.
             */
            if (isset($aC_Keywords[substr($sLine, $i, $cchWord)]) || ($ch >= '0' && $ch <= '9'))
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
            while ($j < $cchLine && ($sLine[$j] == ' ' || $sLine[$j] == "\t"))
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
        $fBlank = ($ch == ' ' || $ch == "\t"); //TODO more "blanks"?
        if ($fFirstNonBlank)    $fFirstNonBlank = $fBlank;
        $cchWord = !$fBlank ? C_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            /*
             * Check for keyword or number.
             */
            if (isset($aC_Keywords[substr($sLine, $i, $cchWord)]) || ($ch >= '0' && $ch <= '9'))
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


/*
 *
 */

/*
 * ASM color encoding.
 */
$aASM_Keywords = array(
    "aaa" => 1,
    "aad" => 1,
    "aam" => 1,
    "aas" => 1,
    "adc" => 1,
    "add" => 1,
    "and" => 1,
    "arpl" => 1,
    "bound" => 1,
    "bsf" => 1,
    "bsr" => 1,
    "bswap" => 1,
    "bt" => 1,
    "btc" => 1,
    "btr" => 1,
    "bts" => 1,
    "call" => 1,
    "cbw" => 1,
    "cdq" => 1,
    "clc" => 1,
    "cld" => 1,
    "cli" => 1,
    "clts" => 1,
    "cmc" => 1,
    "cmp" => 1,
    "cmps" => 1,
    "cmpxchg" => 1,
    "cwd" => 1,
    "cwde" => 1,
    "daa" => 1,
    "das" => 1,
    "dec" => 1,
    "div" => 1,
    "enter" => 1,
    "hlt" => 1,
    "idiv" => 1,
    "imul" => 1,
    "in" => 1,
    "inc" => 1,
    "ins" => 1,
    "int" => 1,
    "into" => 1,
    "invd" => 1,
    "invlpg" => 1,
    "iret" => 1,
    "ja" => 1,
    "jae" => 1,
    "jb" => 1,
    "jbe" => 1,
    "jc" => 1,
    "jcxz" => 1,
    "jecxz" => 1,
    "je" => 1,
    "jg" => 1,
    "jge" => 1,
    "jl" => 1,
    "jle" => 1,
    "jna" => 1,
    "jnae" => 1,
    "jnb" => 1,
    "jnbe" => 1,
    "jnc" => 1,
    "jne" => 1,
    "jng" => 1,
    "jnge" => 1,
    "jnl" => 1,
    "jnle" => 1,
    "jno" => 1,
    "jnp" => 1,
    "jns" => 1,
    "jnz" => 1,
    "jo" => 1,
    "jp" => 1,
    "jpe" => 1,
    "jpo" => 1,
    "js" => 1,
    "jz" => 1,
    "jmp" => 1,
    "lahf" => 1,
    "lar" => 1,
    "lea" => 1,
    "leave" => 1,
    "lgdt" => 1,
    "lidt" => 1,
    "lldt" => 1,
    "lmsw" => 1,
    "lock" => 1,
    "lods" => 1,
    "loop" => 1,
    "loopz" => 1,
    "loopnz" => 1,
    "loope" => 1,
    "loopne" => 1,
    "lds" => 1,
    "les" => 1,
    "lfs" => 1,
    "lgs" => 1,
    "lss" => 1,
    "lsl" => 1,
    "ltr" => 1,
    "mov" => 1,
    "movs" => 1,
    "movsx" => 1,
    "movzx" => 1,
    "mul" => 1,
    "neg" => 1,
    "nop" => 1,
    "not" => 1,
    "or" => 1,
    "out" => 1,
    "outs" => 1,
    "pop" => 1,
    "popa" => 1,
    "popad" => 1,
    "popf" => 1,
    "popfd" => 1,
    "push" => 1,
    "pusha" => 1,
    "pushad" => 1,
    "pushf" => 1,
    "pushfd" => 1,
    "rcl" => 1,
    "rcr" => 1,
    "rep" => 1,
    "ret" => 1,
    "retf" => 1,
    "rol" => 1,
    "ror" => 1,
    "sahf" => 1,
    "sal" => 1,
    "sar" => 1,
    "sbb" => 1,
    "scas" => 1,
    "seta" => 1,
    "setae" => 1,
    "setb" => 1,
    "setbe" => 1,
    "setc" => 1,
    "sete" => 1,
    "setg" => 1,
    "setge" => 1,
    "setl" => 1,
    "setle" => 1,
    "setna" => 1,
    "setnae" => 1,
    "setnb" => 1,
    "setnbe" => 1,
    "setnc" => 1,
    "setne" => 1,
    "setng" => 1,
    "setnge" => 1,
    "setnl" => 1,
    "setnle" => 1,
    "setno" => 1,
    "setnp" => 1,
    "setns" => 1,
    "setnz" => 1,
    "seto" => 1,
    "setp" => 1,
    "setpe" => 1,
    "setpo" => 1,
    "sets" => 1,
    "setz" => 1,
    "sgdt" => 1,
    "shl" => 1,
    "shld" => 1,
    "shr" => 1,
    "shrd" => 1,
    "sidt" => 1,
    "sldt" => 1,
    "smsw" => 1,
    "stc" => 1,
    "std" => 1,
    "sti" => 1,
    "stos" => 1,
    "str" => 1,
    "sub" => 1,
    "test" => 1,
    "verr" => 1,
    "verw" => 1,
    "wait" => 1,
    "wbinvd" => 1,
    "xadd" => 1,
    "xchg" => 1,
    "xlatb" => 1,
    "xor" => 1,
    "fabs" => 1,
    "fadd" => 1,
    "fbld" => 1,
    "fbstp" => 1,
    "fchs" => 1,
    "fclex" => 1,
    "fcom" => 1,
    "fcos" => 1,
    "fdecstp" => 1,
    "fdiv" => 1,
    "fdivr" => 1,
    "ffree" => 1,
    "fiadd" => 1,
    "ficom" => 1,
    "fidiv" => 1,
    "fidivr" => 1,
    "fild" => 1,
    "fimul" => 1,
    "fincstp" => 1,
    "finit" => 1,
    "fist" => 1,
    "fisub" => 1,
    "fisubr" => 1,
    "fld" => 1,
    "fld1" => 1,
    "fldl2e" => 1,
    "fldl2t" => 1,
    "fldlg2" => 1,
    "fldln2" => 1,
    "fldpi" => 1,
    "fldz" => 1,
    "fldcw" => 1,
    "fldenv" => 1,
    "fmul" => 1,
    "fnop" => 1,
    "fpatan" => 1,
    "fprem" => 1,
    "fprem1" => 1,
    "fptan" => 1,
    "frndint" => 1,
    "frstor" => 1,
    "fsave" => 1,
    "fscale" => 1,
    "fsetpm" => 1,
    "fsin" => 1,
    "fsincos" => 1,
    "fsqrt" => 1,
    "fst" => 1,
    "fstcw" => 1,
    "fstenv" => 1,
    "fstsw" => 1,
    "fsub" => 1,
    "fsubr" => 1,
    "ftst" => 1,
    "fucom" => 1,
    "fwait" => 1,
    "fxam" => 1,
    "fxch" => 1,
    "fxtract" => 1,
    "fyl2x" => 1,
    "fyl2xp1" => 1,
    "f2xm1" => 1
    );

$aASM_PPKeywords = array(
    ".align" => 1,
    ".alpha" => 1,
    "and" => 1,
    "assume" => 1,
    "byte" => 1,
    "code" => 1,
    ".code" => 1,
    "comm" => 1,
    "comment" => 1,
    ".const" => 1,
    ".cref" => 1,
    ".data" => 1,
    ".data?" => 1,
    "db" => 1,
    "dd" => 1,
    "df" => 1,
    "dosseg" => 1,
    "dq" => 1,
    "dt" => 1,
    "dw" => 1,
    "dword" => 1,
    "else" => 1,
    "end" => 1,
    "endif" => 1,
    "endm" => 1,
    "endp" => 1,
    "ends" => 1,
    "eq" => 1,
    "equ" => 1,
    ".err" => 1,
    ".err1" => 1,
    ".err2" => 1,
    ".errb" => 1,
    ".errdef" => 1,
    ".errdif" => 1,
    ".erre" => 1,
    ".erridn" => 1,
    ".errnb" => 1,
    ".errndef" => 1,
    ".errnz" => 1,
    "event" => 1,
    "exitm" => 1,
    "extrn" => 1,
    "far" => 1,
    ".fardata" => 1,
    ".fardata?" => 1,
    "fword" => 1,
    "ge" => 1,
    "group" => 1,
    "gt" => 1,
    "high" => 1,
    "if" => 1,
    "if1" => 1,
    "if2" => 1,
    "ifb" => 1,
    "ifdef" => 1,
    "ifdif" => 1,
    "ife" => 1,
    "ifidn" => 1,
    "ifnb" => 1,
    "ifndef" => 1,
    "include" => 1,
    "includelib" => 1,
    "irp" => 1,
    "irpc" => 1,
    "label" => 1,
    ".lall" => 1,
    "le" => 1,
    "length" => 1,
    ".lfcond" => 1,
    ".list" => 1,
    "local" => 1,
    "low" => 1,
    "lt" => 1,
    "macro" => 1,
    "mask" => 1,
    "mod" => 1,
    ".model" => 1,
    "name" => 1,
    "ne" => 1,
    "near" => 1,
    "not" => 1,
    "offset" => 1,
    "or" => 1,
    "org" => 1,
    "%out" => 1,
    "page" => 1,
    "proc" => 1,
    "ptr" => 1,
    "public" => 1,
    "purge" => 1,
    "qword" => 1,
    ".radix" => 1,
    "record" => 1,
    "rept" => 1,
    ".sall" => 1,
    "seg" => 1,
    "segment" => 1,
    ".seq" => 1,
    ".sfcond" => 1,
    "short" => 1,
    "size" => 1,
    ".stack" => 1,
    "struc" => 1,
    "subttl" => 1,
    "tbyte" => 1,
    ".tfcond" => 1,
    "this" => 1,
    "title" => 1,
    "type" => 1,
    ".type" => 1,
    "width" => 1,
    "word" => 1,
    ".xall" => 1,
    ".xcref" => 1,
    ".xlist" => 1,
    "xor" => 1
    );

$aASM_Symbols = array(
    "{" => 1,
    "}" => 1,
    "!" => 1,
    "%" => 1,
    "&amp;" => 1,
    "*" => 1,
    "-" => 1,
    "=" => 1,
    "+" => 1,
    "&lt;" => 1,
    "&gt;" => 1,
    "/" => 1,
    "|" => 1,
    "~" => 1,
    "*" => 1);

/**
 * Initiate the variable array used by the C Color encoder.
 * @param       $aVaraibles     Variable array. (output)
 */
function ASM_ColorInit(&$aVariables)
{
    global $aASM_Keywords;
    global $aASM_PPKeywords;
    global $aASM_Symbols;

    ksort($aASM_Keywords);
    ksort($aASM_PPKeywords);
    ksort($aASM_Symbols);
}


/**
 * Encode a line of C code.
 * @param       $sLine          Line string to encode.
 * @param       $aVariables     Variable array.
 * @returns     Color encoded line string.
 */
function ASM_ColorEncode($sLine, &$aVariables)
{
    global $aASM_Keywords;
    global $aASM_PPKeywords;
    global $aASM_Symbols;

    $sRet = "";
    $cchLine = strlen($sLine);
    $i = 0;
    $fFirstNonBlank = 1;

    /*
     * Loop thru the (remainings) of the line.
     */
    while ($i < $cchLine)
    {
        $ch = $sLine[$i];

        /* comment check */
        if ($ch == ';')
        {
            return $sRet . "<font color=#02FE02>" . substr($sLine, $i) . "</font>";
        }

        /*
         * Check for string.
         */
        if ((($fDbl = (substr($sLine, $i, 6) == "&quot;")) || $sLine[$i] == "'")
             && ($i == 0 || $sLine[$i-1] != '\\'))
        {   /* start of a string */

            $j = $i + 1;
            if ($fDbl)
            {
                while ($j < $cchLine && ($sLine[$j] != '&' || substr($sLine, $j, 6) != "&quot;"))
                    $j += ($sLine[$j] == '\\') ? 2 : 1;
                if ($j < $cchLine)
                    $j += 5;
            }
            else
                while ($j < $cchLine && $sLine[$j] != "'")
                    $j += ($sLine[$j] == '\\') ? 2 : 1;
            $j++;
            $sRet .= "<font color=#FEFE02>".substr($sLine, $i, $j - $i)."</font>";
            $i = $j;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * If non-blank, lets check if we're at the start of a word...
         */
        $fBlank = ($ch == " " || $ch == "\t" || $ch == "\n");
        $cchWord = !$fBlank ? ASM_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            $sWord = strtolower(substr($sLine, $i, $cchWord));

            /*
             * Check for number.
             */
            if (($ch >= '0' && $ch <= '9'))
                $sRet .= "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";
            else
            {
                if ($fFirstNonBlank)
                {
                    /*
                     * Check for asm keyword.
                     */
                    if (isset($aASM_Keywords[$sWord]))
                        $sRet .= "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";
                    /*
                     * Check for preprocessor directive.
                     */
                    else if (($f = isset($aASM_PPKeywords[$sWord]))
                             ||
                             ($i > 0 && $sLine[$i-1] == '.' && isset($aASM_PPKeywords[".".$sWord]))
                             )
                    {
                        if ($f)
                            $sRet .= "<font color=#CECECE>" . substr($sLine, $i, $cchWord) . "</font>";
                        else
                            $sRet =  substr($sRet, 0, -1) . "<font color=#CECECE>." . substr($sLine, $i, $cchWord) . "</font>";
                    }
                    /*
                     * Skip word.
                     */
                    else
                        $sRet .= substr($sLine, $i, $cchWord);
                }
                else
                {
                    /*
                     * Check for preprocessor directive.
                     */
                    if (($f = isset($aASM_PPKeywords[$sWord]))
                             ||
                             ($i > 0 && $sLine[$i-1] == '.' && isset($aASM_PPKeywords[".".$sWord]))
                             )
                    {
                        if ($f)
                            $sRet .= "<font color=#CECECE>" . substr($sLine, $i, $cchWord) . "</font>";
                        else
                            $sRet =  substr($sRet, 0, -1) . "<font color=#CECECE>." . substr($sLine, $i, $cchWord) . "</font>";
                    }
                    /*
                     * Check for asm keyword.
                     */
                    else if (isset($aASM_Keywords[$sWord]))
                        $sRet .= "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";
                    /*
                     * Skip word.
                     */
                    else
                        $sRet .= substr($sLine, $i, $cchWord);
                }
            }

            $i += $cchWord;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * Prepare for symbol check. (we'll have to check for HTML stuff like &amp;).
         */
        $cchWord = 1;
        if ($ch == '&')
        {
            if (substr($sLine, $i, 5) == "&amp;")
                $cchWord = 5;
            else if (substr($sLine, $i, 4) == "&gt;" || substr($sLine, $i, 4) == "&lt;")
                $cchWord = 4;
        }

        /*
         * Check for Symbol.
         */
        if (isset($aASM_Symbols[substr($sLine, $i, $cchWord)]))
        {
            $sRet .= "<font color=#CECECE>" . substr($sLine, $i, $cchWord) . "</font>";
            $i += $cchWord;
            $fFirstNonBlank = 0;
            continue;
        }


        /*
         * Copy char
         */
        $sRet .= $sLine[$i];
        $i++;
        if ($fFirstNonBlank && !$fBlank)
            $fFirstNonBlank = 0;
    }

    return $sRet;
}

/**
 * Calculates the lenght of the word which eventually starts at [$i].
 * @param       $sLine      Line.
 * @param       $cchLine    Line length.
 * @param       $i          Line index.
 * @returns     Word length.
 */
function ASM_WordLen($sLine, $cchLine, $i)
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
             || ($ch == '@')
             || ($ch == '?')
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
            || ($ch == '@')
            || ($ch == '?')
            || ($ch == '$')
            )
           )
        $ch = @$sLine[++$i];
    return $i - $j;
}



/*
 *
 */
/* hardcoded
$aMake_Keywords = array(
    "\$&amp;" => 1,
    "\$**" => 1,
    "\$*" => 1,
    "\$." => 1,
    "\$:" => 1,
    "\$&lt;" => 1,
    "\$?" => 1,
    "\$@" => 1,
    "\$d" => 1);
*/
$aMake_Symbols = array(
    "@" => 1,
    "(" => 1,
    ")" => 1,
    "." => 1,
    "=" => 1,
    "*" => 1,
    "+" => 1,
    "-" => 1,
    "/" => 1,
    "" => 1,
    "[" => 1,
    "]" => 1,
    "," => 1,
    "&lt;" => 1,
    "&gt;" => 1,
    ":" => 1,
    ";" => 1);
/**
 * Initiate the variable array used by the C Color encoder.
 * @param       $aVaraibles     Variable array. (output)
 */
function Make_ColorInit(&$aVariables)
{
    //global $aMake_Keywords;
    global $aMake_Symbols;
    //$aVariables = array("fInline" => 0)
    //ksort($aMake_Keywords);
    ksort($aMake_Symbols);
}


/**
 * Encode a line of C code.
 * @param       $sLine          Line string to encode.
 * @param       $aVariables     Variable array.
 * @returns     Color encoded line string.
 */
function Make_ColorEncode($sLine, &$aVariables)
{
    global $aMake_Keywords;
    global $aMake_Symbols;

    $sRet = "";
    $cchLine = strlen($sLine);
    $i = 0;
    $fFirstNonBlank = 1;

    /*
     * Loop thru the (remainings) of the line.
     */
    while ($i < $cchLine)
    {
        $ch = $sLine[$i];

        /* comment check */
        if ($ch == '#')
        {
            return $sRet . "<font color=#02FE02>" . substr($sLine, $i) . "</font>";
        }


        /*
         * Check for string.
         */
        if ((($fDbl = (substr($sLine, $i, 6) == "&quot;")) || $sLine[$i] == "'")
             && ($i == 0 || $sLine[$i-1] != '\\'))
        {   /* start of a string */

            $j = $i + 1;
            if ($fDbl)
            {
                while ($j < $cchLine && ($sLine[$j] != '&' || substr($sLine, $j, 6) != "&quot;"))
                    $j += ($sLine[$j] == '\\') ? 2 : 1;
                if ($j < $cchLine)
                    $j += 5;
            }
            else
                while ($j < $cchLine && $sLine[$j] != "'")
                    $j += ($sLine[$j] == '\\') ? 2 : 1;
            $j++;
            $sRet .= "<font color=#FEFE02>".substr($sLine, $i, $j - $i)."</font>";
            $i = $j;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * Check for ! or % words
         */
        if (($fFirstNonBlank && ($ch == "#" || $ch == "!")))
        {
            $j = $i + 1;
            while ($j < $cchLine && ($sLine[$j] == ' ' || $sLine[$j] == "\t"))
                $j++;
            $j += Make_WordLen($sLine, $cchLine, $j);
            echo "<font color=#CECECE>" . substr($sLine, $i, $j - $i) . "</font>";
            $i = $j;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * Check for keyword
         */
        /* don't work
        if ($ch == "$" && $i + 1 < $cchLine)
        {
            $cch = 0;
            $sWord = substr($sLine, $i+1, 1);
            if (   $sWord == "*"
                || $sWord == "."
                || $sWord == ":"
                || $sWord == "?"
                || $sWord == "@"
                || $sWord == "d")
                $cch = 2;
            else if ($i + 2 < $cchLine && ($sWord = substr($sLine, $i+1, 2)) == "**")
                $cch = 3;
            else if ($i + 4 < $cchLine && ($sWord = substr($sLine, $i+1, 5)) == "&amp;")
                $cch = 6;
            else if ($i + 5 < $cchLine && ($sWord = substr($sLine, $i+1, 4)) == "&lt;")
                $cch = 5;
            if ($cch > 0)
            {
                echo "<font color=#CECECE>$" . $sWord . "</font>";
                $i += $cch;
                $fFirstNonBlank = 0;
                continue;
            }
        } */

        /*
         * If non-blank, lets check if we're at the start of a word...
         */
        $fBlank = ($ch == " " || $ch == "\t" || $ch == "\n");
        $cchWord = !$fBlank ? Make_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            $sWord = strtolower(substr($sLine, $i, $cchWord));

            /*
             * Check for keywords.
             */
            if ($f = isset($aMake_Keywords[$sWord]))
                $sRet .= "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";

            /*
             * Check for number.
             */
            else if (($ch >= '0' && $ch <= '9'))
                $sRet .= "<font color=#FF0202>" . substr($sLine, $i, $cchWord) . "</font>";

            /*
             * Skip word.
             */
            else
                $sRet .= substr($sLine, $i, $cchWord);

            $i += $cchWord;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * Prepare for symbol check. (we'll have to check for HTML stuff like &amp;).
         */
        $cchWord = 1;
        if ($ch == '&')
        {
            if (substr($sLine, $i, 5) == "&amp;")
                $cchWord = 5;
            else if (substr($sLine, $i, 4) == "&gt;" || substr($sLine, $i, 4) == "&lt;")
                $cchWord = 4;
        }

        /*
         * Check for Symbol.
         */
        if (isset($aMake_Symbols[substr($sLine, $i, $cchWord)]))
        {
            $sRet .= "<font color=#CECECE>" . substr($sLine, $i, $cchWord) . "</font>";
            $i += $cchWord;
            $fFirstNonBlank = 0;
            continue;
        }


        /*
         * Copy char
         */
        $sRet .= $sLine[$i];
        $i++;
        if ($fFirstNonBlank && !$fBlank)
            $fFirstNonBlank = 0;
    }

    return $sRet;
}

/**
 * Calculates the lenght of the word which eventually starts at [$i].
 * @param       $sLine      Line.
 * @param       $cchLine    Line length.
 * @param       $i          Line index.
 * @returns     Word length.
 */
function Make_WordLen($sLine, $cchLine, $i)
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
            )
           )
        $ch = @$sLine[++$i];
    return $i - $j;
}


?>

