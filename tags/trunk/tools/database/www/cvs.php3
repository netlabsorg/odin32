<?php


$sCVSROOT = "d:/odin32/cvs/cvsroot";
$sCVSROOT = ".";

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
        $this->sName        = substr($sFilename, $i > 0 ? $i + 1 : 0, strlen($sFilename)-2);
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
                $sKey = CopyWord($sLine);
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
                    $iAt = 0;
                    for ($iAt; $iAt+1 < $cchLine; $iAt++)
                        if ($sLine[$iAt] == '@' && ($fEnd = ($sLine[++$iAt] != '@')))
                            break;
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
                    if ($fEnd = ($i >= 0))
                    {
                        $asValue[] = str_replace("@@", "@", substr($sLine, 0, $i));
                        $sLine = (strlen($sLine) > $i+1) ? substr($sLine, $i+1) : "";
                    }
                    else
                    {
                        $asValue[] = str_replace("@@", "@", $sLine);
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
            $fNewKey = 1;

            /* debug */
            //echo "debug key: $sKey  value(".sizeof($asValue)."):".$asValue[0]."\n";
        }

        fclose($hFile);

        /*
         * Return successfully.
         */
        $this->fOk = 1;
        return 1;
    }


    /**
     * Debug dump function.
     */
    function DumpInfo()
    {
        echo "\nDump:\n";
        while (list ($sKey, $asValue) = each ($this->aasKeys))
        {
            echo "* key: $sKey *\n";
            if (sizeof((array)$asValue) > 0)
            {
                while (list ($key, $s) = each ($asValue))
                    echo $s;
                echo "\n";
            }
        }

        while (list ($sRev, $aasKeys) = each ($this->aaasRevs))
        {
            echo "* Revision: $sRev *\n";
            if (sizeof((array)$aasKeys) > 0)
            {
                while (list ($sKey, $asValue) = each ($aasKeys))
                {
                    echo "* key: $sKey *\n";
                    if (sizeof((array)$asValue) > 0)
                    {
                        while (list ($key, $s) = each ($asValue))
                            echo $s;
                        echo "\n";
                    }
                }
            }
        }

        while (list ($sKey, $asValue) = each ($this->aasDeltas))
        {
            echo "* delta for revision: $sKey *\n";
            if (sizeof((array)$asValue) > 0)
            {
                while (list ($key, $s) = each ($asValue))
                    echo $s;
                echo "\n";
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
                break;
        }



        /*
         * Write it!
         */
        echo "<table><tr><td bgcolor=\"#020286\"><pre><font size=-0 face=\"System VIO, System Monospaced\" color=\"#02FEFE\">\n";

        $fComment = 0;
        $iLine = 0;
        $cLines = sizeof($this->aasDeltas[$sRevision]);
        //echo "<!-- debug $this->sExt -->\n";
        while ($iLine < $cLines)
        {
            $sLine = htmlspecialchars($this->aasDeltas[$sRevision][$iLine++]);

            /*
             * Preprocessing... Color coding
             */
            switch ($this->sExt)
            {
                case 'c':
                case 'cpp':
                case 'cxx':
                case 'h':
                case 'hpp':
                    $sLine = C_ColorEncode($sLine, $aVariables);
                    break;
            }

            /*
             * Finished processing of the line. So, write it.
             */
            echo  "<a name=$iLine>$sLine</a>";
        }

        echo "</pre></td></tr></table>\n";

        return 1;
    }


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
    "auto" => 1,
    "break" => 1,
    "case" => 1,
    "char" => 1,
    "const" => 1,
    "continue" => 1,
    "default" => 1,
    "defined" => 1,
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
    "finally" => 1,
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
    "bool" => 1,
    "true" => 1,
    "false" => 1,
    "explicit" => 1,
    "mutable" => 1,
    "typename" => 1,
    "static_cast" => 1,
    "const_cast" => 1,
    "reinterpret_cast" => 1,
    "dynamic_cast" => 1,
    "using" => 1,
    "typeid" => 1,
    "asm" => 1,
    "catch" => 1,
    "signed" => 1,
    "template" => 1,
    "throw" => 1,
    "try" => 1,
    "volatile" => 1,
    "namespace" => 1);

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
    "&" => 1,
    "&amp;" => 1,
    "*" => 1,
    "-" => 1,
    "=" => 1,
    "+" => 1,
    ":" => 1,
    ";" => 1,
    "<" => 1,
    "&lt;" => 1,
    ">" => 1,
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
    $aVariables["fComment"] = 0;
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
        /* comment check */
        if ($i+1 < $cchLine && $sLine[$i] == '/')
        {
            if ($sLine[$i+1] == '/')
            {   /* one-line comment */
                return $sRet . "<font color=\"#02FE02\">" . substr($sLine, $i) . "</font>";
            }

            if ($sLine[$i+1] == '*')
            {   /* Start of multiline comment */
                if ($j = strpos($sLine, "*/", $i + 2))
                {
                    $sRet = $sRet . "<font color=\"#02FE02\">" . substr($sLine, $i, $j+2 - $i) . "</font>";
                    $i = $j + 2;
                }
                else
                {
                    $aVariables["fComment"] = 1;
                    return $sRet . "<font color=\"#02FE02\">" . substr($sLine, $i);
                }
                continue;
            }
        }


        /*
         * Check for string.
         */
        if ((($fDbl = ($sLine[$i] == '"' || substr($sLine, $i, 6) == "&quot;")) || $sLine[$i] == "'")
             && ($i == 0 || $sLine[$i-1] != '\\'))
        {   /* start of a string */
            $j = $i + 1;
            if ($fDbl)
            {
                if ($sLine[$i] == '"')
                    while ($j < $cchLine && $sLine[$j] != '"')
                        $j += ($sLine[$j] == '\\') ? 2 : 1;
                else
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
            $sRet .= "<font color=\"#FEFE02\">".substr($sLine, $i, $j - $i)."</font>";
            $i = $j;
            continue;
        }


        /*
         * Check for preprocessor directive.
         */
        if ($fFirstNonBlank && $sLine[$i] == "#")
        {
            $j = $i + 1;
            while ($j < $cchLine && ($sLine[$j] == ' ' || $sLine[$j] == '\t'))
                $j++;
            $j += C_WordLen($sLine, $cchLine, $j);
            $sRet .= "<font color=\"#CECECE\">" . substr($sLine, $i, $j - $i) . "</font>";
            $i = $j;
            $fFirstNonBlank = 0;
            continue;
        }

        /*
         * If non-blank, lets check if we're at the start of a word...
         */
        $fBlank = ($sLine[$i] == ' ' || $sLine[$i] == '\t'); //TODO more "blanks"?
        if ($fFirstNonBlank)    $fFirstNonBlank = $fBlank;
        $cchWord = !$fBlank ? C_WordLen($sLine, $cchLine, $i) : 0;

        if ($cchWord > 0)
        {
            /*
             * Check for keyword.
             */
            if ($cchWord > 0 && isset($aC_Keywords[substr($sLine, $i, $cchWord)]))
                $sRet .= "<font color=\"#FF0202\">" . substr($sLine, $i, $cchWord) . "</font>";

            /*
             * Check for number
             */
            else if ($sLine[$i] >= '0' && $sLine[$i] <= '9')
                $sRet .= "<font color=\"#FE0202\">" . substr($sLine, $i, $cchWord) . "</font>";

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
        if ($sLine[$i] == '&')
        {
            while ($cchWord < 8 && $sLine[$i+$cchWord] != ';' &&
                    (   ($sLine[$i+$cchWord] >= 'a' && $sLine[$i+$cchWord] <= 'z')
                     || ($sLine[$i+$cchWord] >= 'A' && $sLine[$i+$cchWord] <= 'Z')
                    )
                   )
                   $cchWord++;
            if ($sLine[$i + $cchWord++] != ';')
                $cchWord = 1;
        }

        /*
         * Check for Symbol.
         */
        if (isset($aC_Symbols[substr($sLine, $i, 1)]))
        {
            $sRet .= "<font color=\"#CECECE\">" . substr($sLine, $i, $cchWord) . "</font>";
            $i += $cchWord;
            continue;
        }


        /*
         * Copy char
         */
        $sRet = $sRet.$sLine[$i];
        $i++;
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
function C_WordLen($sLine, $cchLine, $i)
{

    /*
     * Check that previous letter wasen't a possible
     * word part.
     */
    if ($i > 0 &&
        (
            ($sLine[$i-1] >= 'a' && $sLine[$i-1] <= 'z')
         || ($sLine[$i-1] >= 'A' && $sLine[$i-1] <= 'Z')
         || ($sLine[$i-1] >= '0' && $sLine[$i-1] <= '9')
         || ($sLine[$i-1] == '_')
         || ($sLine[$i-1] == '$')
         )
        )
        return 0;

    /*
     * Count letters in the word
     */
    $cch = 0;
    $cchLine = strlen($sLine);
    while ($i < $cchLine &&
           (
               ($sLine[$i] >= 'a' && $sLine[$i] <= 'z')
            || ($sLine[$i] >= 'A' && $sLine[$i] <= 'Z')
            || ($sLine[$i] >= '0' && $sLine[$i] <= '9')
            || ($sLine[$i] == '_')
            || ($sLine[$i] == '$')
            )
           )
    {
        $i++;
        $cch++;
    }
    return $cch;
}

?>

