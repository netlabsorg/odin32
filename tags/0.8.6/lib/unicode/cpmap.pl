#!/usr/bin/perl
#
# Generate code page .c files from ftp.unicode.org descriptions
#
# Copyright 2000 Alexandre Julliard
#

# base directory for ftp.unicode.org files
$BASEDIR = "ftp.unicode.org/Public/";
$MAPPREFIX = $BASEDIR . "MAPPINGS/";

# UnicodeData file
$UNICODEDATA = $BASEDIR . "UNIDATA/UnicodeData.txt";

# Defaults mapping
$DEFAULTS = "./defaults";

# Default char for undefined mappings
$DEF_CHAR = ord '?';

@allfiles =
(
    [ 37,    "VENDORS/MICSFT/EBCDIC/CP037.TXT",   "IBM EBCDIC US Canada" ],
    [ 42,    "VENDORS/ADOBE/symbol.txt",          "Symbol" ],
    [ 424,   "VENDORS/MISC/CP424.TXT",            "IBM EBCDIC Hebrew" ],
    [ 437,   "VENDORS/MICSFT/PC/CP437.TXT",       "OEM United States" ],
    [ 500,   "VENDORS/MICSFT/EBCDIC/CP500.TXT",   "IBM EBCDIC International" ],
    [ 737,   "VENDORS/MICSFT/PC/CP737.TXT",       "OEM Greek 437G" ],
    [ 775,   "VENDORS/MICSFT/PC/CP775.TXT",       "OEM Baltic" ],
    [ 850,   "VENDORS/MICSFT/PC/CP850.TXT",       "OEM Multilingual Latin 1" ],
    [ 852,   "VENDORS/MICSFT/PC/CP852.TXT",       "OEM Slovak Latin 2" ],
    [ 855,   "VENDORS/MICSFT/PC/CP855.TXT",       "OEM Cyrillic" ],
    [ 856,   "VENDORS/MISC/CP856.TXT",            "Hebrew PC" ],
    [ 857,   "VENDORS/MICSFT/PC/CP857.TXT",       "OEM Turkish" ],
    [ 860,   "VENDORS/MICSFT/PC/CP860.TXT",       "OEM Portuguese" ],
    [ 861,   "VENDORS/MICSFT/PC/CP861.TXT",       "OEM Icelandic" ],
    [ 862,   "VENDORS/MICSFT/PC/CP862.TXT",       "OEM Hebrew" ],
    [ 863,   "VENDORS/MICSFT/PC/CP863.TXT",       "OEM Canadian French" ],
    [ 864,   "VENDORS/MICSFT/PC/CP864.TXT",       "OEM Arabic" ],
    [ 865,   "VENDORS/MICSFT/PC/CP865.TXT",       "OEM Nordic" ],
    [ 866,   "VENDORS/MICSFT/PC/CP866.TXT",       "OEM Russian" ],
    [ 869,   "VENDORS/MICSFT/PC/CP869.TXT",       "OEM Greek" ],
    [ 874,   "VENDORS/MICSFT/PC/CP874.TXT",       "ANSI/OEM Thai" ],
    [ 875,   "VENDORS/MICSFT/EBCDIC/CP875.TXT",   "IBM EBCDIC Greek" ],
    [ 878,   "VENDORS/MISC/KOI8-R.TXT",           "Russian KOI8" ],
    [ 932,   "VENDORS/MICSFT/WINDOWS/CP932.TXT",  "ANSI/OEM Japanese Shift-JIS" ],
    [ 936,   "VENDORS/MICSFT/WINDOWS/CP936.TXT",  "ANSI/OEM Simplified Chinese GBK" ],
    [ 949,   "VENDORS/MICSFT/WINDOWS/CP949.TXT",  "ANSI/OEM Korean Unified Hangul" ],
    [ 950,   "VENDORS/MICSFT/WINDOWS/CP950.TXT",  "ANSI/OEM Traditional Chinese Big5" ],
    [ 1006,  "VENDORS/MISC/CP1006.TXT",           "IBM Arabic" ],
    [ 1026,  "VENDORS/MICSFT/EBCDIC/CP1026.TXT",  "IBM EBCDIC Latin 5 Turkish" ],
    [ 1250,  "VENDORS/MICSFT/WINDOWS/CP1250.TXT", "ANSI Eastern Europe" ],
    [ 1251,  "VENDORS/MICSFT/WINDOWS/CP1251.TXT", "ANSI Cyrillic" ],
    [ 1252,  "VENDORS/MICSFT/WINDOWS/CP1252.TXT", "ANSI Latin 1" ],
    [ 1253,  "VENDORS/MICSFT/WINDOWS/CP1253.TXT", "ANSI Greek" ],
    [ 1254,  "VENDORS/MICSFT/WINDOWS/CP1254.TXT", "ANSI Turkish" ],
    [ 1255,  "VENDORS/MICSFT/WINDOWS/CP1255.TXT", "ANSI Hebrew" ],
    [ 1256,  "VENDORS/MICSFT/WINDOWS/CP1256.TXT", "ANSI Arabic" ],
    [ 1257,  "VENDORS/MICSFT/WINDOWS/CP1257.TXT", "ANSI Baltic" ],
    [ 1258,  "VENDORS/MICSFT/WINDOWS/CP1258.TXT", "ANSI/OEM Viet Nam" ],
    [ 10000, "VENDORS/MICSFT/MAC/ROMAN.TXT",      "Mac Roman" ],
    [ 10006, "VENDORS/MICSFT/MAC/GREEK.TXT",      "Mac Greek" ],
    [ 10007, "VENDORS/MICSFT/MAC/CYRILLIC.TXT",   "Mac Cyrillic" ],
    [ 10029, "VENDORS/MICSFT/MAC/LATIN2.TXT",     "Mac Latin 2" ],
    [ 10079, "VENDORS/MICSFT/MAC/ICELAND.TXT",    "Mac Icelandic" ],
    [ 10081, "VENDORS/MICSFT/MAC/TURKISH.TXT",    "Mac Turkish" ],
    [ 20866, "VENDORS/MISC/KOI8-R.TXT",           "Russian KOI8" ],
    [ 28591, "ISO8859/8859-1.TXT",                "ISO 8859-1 Latin 1" ],
    [ 28592, "ISO8859/8859-2.TXT",                "ISO 8859-2 Latin 2 (East European)" ],
    [ 28593, "ISO8859/8859-3.TXT",                "ISO 8859-3 Latin 3 (South European)" ],
    [ 28594, "ISO8859/8859-4.TXT",                "ISO 8859-4 Latin 4 (Baltic old)" ],
    [ 28595, "ISO8859/8859-5.TXT",                "ISO 8859-5 Cyrillic" ],
    [ 28596, "ISO8859/8859-6.TXT",                "ISO 8859-6 Arabic" ],
    [ 28597, "ISO8859/8859-7.TXT",                "ISO 8859-7 Greek" ],
    [ 28598, "ISO8859/8859-8.TXT",                "ISO 8859-8 Hebrew" ],
    [ 28599, "ISO8859/8859-9.TXT",                "ISO 8859-9 Latin 5 (Turkish)" ],
    [ 28600, "ISO8859/8859-10.TXT",               "ISO 8859-10 Latin 6 (Nordic)" ],
    [ 28603, "ISO8859/8859-13.TXT",               "ISO 8859-13 Latin 7 (Baltic)" ],
    [ 28604, "ISO8859/8859-14.TXT",               "ISO 8859-14 Latin 8 (Celtic)" ],
    [ 28605, "ISO8859/8859-15.TXT",               "ISO 8859-15 Latin 9 (Euro)" ]
);


%ctype =
(
    "upper"  => 0x0001,
    "lower"  => 0x0002,
    "digit"  => 0x0004,
    "space"  => 0x0008,
    "punct"  => 0x0010,
    "cntrl"  => 0x0020,
    "blank"  => 0x0040,
    "xdigit" => 0x0080,
    "alpha"  => 0x0100
);

%categories =
(
    "Lu" => $ctype{"alpha"}|$ctype{"upper"}, # Letter, Uppercase
    "Ll" => $ctype{"alpha"}|$ctype{"lower"}, # Letter, Lowercase
    "Lt" => $ctype{"alpha"},    # Letter, Titlecase
    "Mn" => $ctype{"punct"},    # Mark, Non-Spacing
    "Mc" => $ctype{"punct"},    # Mark, Spacing Combining
    "Me" => $ctype{"punct"},    # Mark, Enclosing
    "Nd" => $ctype{"digit"},    # Number, Decimal Digit
    "Nl" => $ctype{"punct"},    # Number, Letter
    "No" => $ctype{"punct"},    # Number, Other
    "Zs" => $ctype{"space"},    # Separator, Space
    "Zl" => 0,                  # Separator, Line
    "Zp" => 0,                  # Separator, Paragraph
    "Cc" => $ctype{"cntrl"},    # Other, Control
    "Cf" => 0,                  # Other, Format
    "Cs" => 0,                  # Other, Surrogate
    "Co" => 0,                  # Other, Private Use
    "Cn" => 0,                  # Other, Not Assigned
    "Lm" => $ctype{"punct"},    # Letter, Modifier
    "Lo" => $ctype{"alpha"},    # Letter, Other
    "Pc" => $ctype{"punct"},    # Punctuation, Connector
    "Pd" => $ctype{"punct"},    # Punctuation, Dash
    "Ps" => $ctype{"punct"},    # Punctuation, Open
    "Pe" => $ctype{"punct"},    # Punctuation, Close
    "Pi" => $ctype{"punct"},    # Punctuation, Initial quote
    "Pf" => $ctype{"punct"},    # Punctuation, Final quote
    "Po" => $ctype{"punct"},    # Punctuation, Other
    "Sm" => $ctype{"punct"},    # Symbol, Math
    "Sc" => $ctype{"punct"},    # Symbol, Currency
    "Sk" => $ctype{"punct"},    # Symbol, Modifier
    "So" => $ctype{"punct"}     # Symbol, Other 
);

# a few characters need additional categories that cannot be determined automatically
%special_categories =
(
    "xdigit" => [ ord('0')..ord('9'),ord('A')..ord('F'),ord('a')..ord('f'),
                  0xff10..0xff19, 0xff21..0xff26, 0xff41..0xff46 ],
    "space"  => [ 0x09..0x0d, 0xfeff ],
    "blank"  => [ 0x09, 0x20, 0xa0, 0xfeff ]
);

%directions =
(
    "L"   => 1,    # Left-to-Right
    "LRE" => 11,   # Left-to-Right Embedding
    "LRO" => 11,   # Left-to-Right Override
    "R"   => 2,    # Right-to-Left
    "AL"  => 2,    # Right-to-Left Arabic
    "RLE" => 11,   # Right-to-Left Embedding
    "RLO" => 11,   # Right-to-Left Override
    "PDF" => 11,   # Pop Directional Format
    "EN"  => 3,    # European Number
    "ES"  => 4,    # European Number Separator
    "ET"  => 5,    # European Number Terminator
    "AN"  => 6,    # Arabic Number
    "CS"  => 7,    # Common Number Separator
    "NSM" => 0,    # Non-Spacing Mark
    "BN"  => 0,    # Boundary Neutral
    "B"   => 8,    # Paragraph Separator
    "S"   => 9,    # Segment Separator
    "WS"  => 10,   # Whitespace
    "ON"  => 11    # Other Neutrals
);


################################################################
# main routine

READ_DEFAULTS();
DUMP_CASE_MAPPINGS();
DUMP_COMPOSE_TABLES();
DUMP_CTYPE_TABLES();

foreach $file (@allfiles) { HANDLE_FILE( @$file ); }

OUTPUT_CPTABLE();

exit(0);


################################################################
# read in the defaults file
sub READ_DEFAULTS
{
    @unicode_defaults = ();
    @unicode_aliases = ();
    @tolower_table = ();
    @toupper_table = ();
    @category_table = ();
    @direction_table = ();
    @decomp_table = ();
    @compose_table = ();

    # first setup a few default mappings

    open DEFAULTS or die "Cannot open $DEFAULTS";
    print "Loading $DEFAULTS\n";
    while (<DEFAULTS>)
    {
        next if /^\#/;  # skip comments
        next if /^$/;  # skip empty lines
        if (/^(([0-9a-fA-F]+)(,[0-9a-fA-F]+)*)\s+([0-9a-fA-F]+|'.'|none)\s+(\#.*)?/)
        {
            my @src = map hex, split /,/,$1;
            my $dst = $4;
            my $comment = $5;
            if ($#src > 0) { push @unicode_aliases, \@src; }
            next if ($dst eq "none");
            $dst = ($dst =~ /\'.\'/) ? ord substr($dst,1,1) : hex $dst;
            foreach $src (@src)
            {
                die "Duplicate value" if defined($unicode_defaults[$src]);
                $unicode_defaults[$src] = $dst;
            }
            next;
        }
        die "Unrecognized line $_\n";
    }

    # now build mappings from the decomposition field of the Unicode database

    open UNICODEDATA or die "Cannot open $UNICODEDATA";
    print "Loading $UNICODEDATA\n";
    while (<UNICODEDATA>)
    {
	# Decode the fields ...
	($code, $name, $cat, $comb, $bidi, 
	 $decomp, $dec, $dig, $num, $mirror, 
	 $oldname, $comment, $upper, $lower, $title) = split /;/;

        my $src = hex $code;

        die "unknown category $cat" unless defined $categories{$cat};
        die "unknown directionality $bidi" unless defined $directions{$bidi};

        $uniname[$src] = $name;
        $category_table[$src] = $categories{$cat};
        $direction_table[$src] = $directions{$bidi};

        if ($lower ne "")
        {
            $tolower_table[$src] = hex $lower;
            $category_table[$src] |= $ctype{"upper"}|$ctype{"alpha"};
        }
        if ($upper ne "")
        {
            $toupper_table[$src] = hex $upper;
            $category_table[$src] |= $ctype{"lower"}|$ctype{"alpha"};
        }
        if ($dec ne "")
        {
            $category_table[$src] |= $ctype{"digit"};
        }

        # copy the category and direction for everything between First/Last pairs
        if ($name =~ /, First>/) { $start = $src; }
        if ($name =~ /, Last>/)
        {
            while ($start < $src)
            {
                $category_table[$start] = $category_table[$src];
                $direction_table[$start] = $direction_table[$src];
                $start++;
            }
        }
        
        next if $decomp eq "";  # no decomposition, skip it

        if ($decomp =~ /^<([a-zA-Z]+)>\s+([0-9a-fA-F]+)$/)
        {
            # decomposition of the form "<foo> 1234" -> use char if type is known
            next unless ($1 eq "font" ||
                         $1 eq "noBreak" ||
                         $1 eq "circle" ||
                         $1 eq "super" ||
                         $1 eq "sub" ||
                         $1 eq "wide" ||
                         $1 eq "narrow" ||
                         $1 eq "compat" ||
                         $1 eq "small");
            $dst = hex $2;
        }
        elsif ($decomp =~ /^<compat>\s+0020\s+([0-9a-fA-F]+)/)
        {
            # decomposition "<compat> 0020 1234" -> combining accent
            $dst = hex $1;
        }
        elsif ($decomp =~ /^([0-9a-fA-F]+)/)
        {
            # decomposition contains only char values without prefix -> use first char
            $dst = hex $1;
            $category_table[$src] |= $category_table[$dst];
            # store decomposition if it contains two chars
            if ($decomp =~ /^([0-9a-fA-F]+)\s+([0-9a-fA-F]+)$/)
            {
                $decomp_table[$src] = [ hex $1, hex $2 ];
                push @compose_table, [ hex $1, hex $2, $src ];
            }
        }
        else
        {
            next;
        }

        next if defined($unicode_defaults[$src]);  # may have been set in the defaults file

        # check for loops
        for ($i = $dst; ; $i = $unicode_defaults[$i])
        {
            die sprintf("loop detected for %04x -> %04x",$src,$dst) if $i == $src;
            last unless defined($unicode_defaults[$i]);
        }
        $unicode_defaults[$src] = $dst;
    }

    # patch the category of some special characters

    foreach $cat (keys %special_categories)
    {
        my $flag = $ctype{$cat};
        foreach $i (@{$special_categories{$cat}}) { $category_table[$i] |= $flag; }
    }
}


################################################################
# parse the input file
sub READ_FILE
{
    my $name = shift;
    open INPUT,$name or die "Cannot open $name";
    @cp2uni = ();
    @lead_bytes = ();
    @uni2cp = ();

    while (<INPUT>)
    {
        next if /^\#/;  # skip comments
        next if /^$/;  # skip empty lines
        next if /\x1a/;  # skip ^Z
        next if (/^0x([0-9a-fA-F]+)\s+\#UNDEFINED/);  # undefined char

        if (/^0x([0-9a-fA-F]+)\s+\#DBCS LEAD BYTE/)
        {
            $cp = hex $1;
            push @lead_bytes,$cp;
            $cp2uni[$cp] = 0;
            next;
        }
        if (/^0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)\s+(\#.*)?/)
        {
            $cp = hex $1;
            $uni = hex $2;
            $cp2uni[$cp] = $uni unless defined($cp2uni[$cp]);
            $uni2cp[$uni] = $cp unless defined($uni2cp[$uni]);
            next;
        }
        die "$name: Unrecognized line $_\n";
    }
}


################################################################
# parse the symbol.txt file, since its syntax is different from the other ones
sub READ_SYMBOL_FILE
{
    my $name = shift;
    open INPUT,$name or die "Cannot open $name";
    @cp2uni = ();
    @lead_bytes = ();
    @uni2cp = ();

    while (<INPUT>)
    {
        next if /^\#/;  # skip comments
        next if /^$/;  # skip empty lines
        next if /\x1a/;  # skip ^Z
        if (/^([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(\#.*)?/)
        {
            $uni = hex $1;
            $cp = hex $2;
            $cp2uni[$cp] = $uni unless defined($cp2uni[$cp]);
            $uni2cp[$uni] = $cp unless defined($uni2cp[$uni]);
            next;
        }
        die "$name: Unrecognized line $_\n";
    }
}


################################################################
# add default mappings once the file had been read
sub ADD_DEFAULT_MAPPINGS
{
    # Apply aliases

    foreach $alias (@unicode_aliases)
    {
        my $target = undef;
        foreach $src (@$alias)
        {
            if (defined($uni2cp[$src]))
            {
                $target = $uni2cp[$src];
                last;
            }
        }
        next unless defined($target);

        # At least one char of the alias set is defined, set the others to the same value
        foreach $src (@$alias)
        {
            $uni2cp[$src] = $target unless defined($uni2cp[$src]);
        }
    }

    # For every src -> target mapping in the defaults table,
    # make uni2cp[src] = uni2cp[target] if uni2cp[target] is defined

    for ($src = 0; $src < 65536; $src++)
    {
        next if defined($uni2cp[$src]);  # source has a definition already
        next unless defined($unicode_defaults[$src]);  # no default for this char
        my $target = $unicode_defaults[$src];

        # do a recursive mapping until we find a target char that is defined
        while (!defined($uni2cp[$target]) &&
               defined($unicode_defaults[$target])) { $target = $unicode_defaults[$target]; }

        if (defined($uni2cp[$target])) { $uni2cp[$src] = $uni2cp[$target]; }
    }

    # Add an identity mapping for all undefined chars

    for ($i = 0; $i < 256; $i++)
    {
        next if defined($cp2uni[$i]);
        next if defined($uni2cp[$i]);
        $cp2uni[$i] = $uni2cp[$i] = $i;
    }
}

################################################################
# dump an array of integers
sub DUMP_ARRAY
{
    my ($format,$default,@array) = @_;
    my $i, $ret = "    ";
    for ($i = 0; $i < $#array; $i++)
    {
        $ret .= sprintf($format, defined $array[$i] ? $array[$i] : $default);
        $ret .= (($i % 8) != 7) ? ", " : ",\n    ";
    }
    $ret .= sprintf($format, defined $array[$i] ? $array[$i] : $default);
    return $ret;
}

################################################################
# dump an SBCS mapping table
sub DUMP_SBCS_TABLE
{
    my ($codepage, $name) = @_;
    my $i;

    # output the ascii->unicode table

    printf OUTPUT "static const WCHAR cp2uni[256] =\n";
    printf OUTPUT "{\n%s\n};\n\n", DUMP_ARRAY( "0x%04x", $DEF_CHAR, @cp2uni[0 .. 255] );

    # count the number of unicode->ascii subtables that contain something

    my @filled = ();
    my $subtables = 1;
    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $uni2cp[$i];
        $filled[$i >> 8] = 1;
        $subtables++;
        $i |= 255;
    }

    # output all the subtables into a single array

    printf OUTPUT "static const unsigned char uni2cp_low[%d] =\n{\n", $subtables*256;
    for ($i = 0; $i < 256; $i++)
    {
        next unless $filled[$i];
        printf OUTPUT "    /* 0x%02x00 .. 0x%02xff */\n", $i, $i;
        printf OUTPUT "%s,\n", DUMP_ARRAY( "0x%02x", $DEF_CHAR, @uni2cp[($i<<8) .. ($i<<8)+255] );
    }
    printf OUTPUT "    /* defaults */\n";
    printf OUTPUT "%s\n};\n\n", DUMP_ARRAY( "0x%02x", 0, ($DEF_CHAR) x 256 );

    # output a table of the offsets of the subtables in the previous array

    my $pos = 0;
    my @offsets = ();
    for ($i = 0; $i < 256; $i++)
    {
        if ($filled[$i]) { push @offsets, $pos; $pos += 256; }
        else { push @offsets, ($subtables-1) * 256; }
    }
    printf OUTPUT "static const unsigned short uni2cp_high[256] =\n";
    printf OUTPUT "{\n%s\n};\n\n", DUMP_ARRAY( "0x%04x", 0, @offsets );

    # output the code page descriptor

    printf OUTPUT "const struct sbcs_table cptable_%03d =\n{\n", $codepage;
    printf OUTPUT "    { %d, 1, 0x%04x, 0x%04x, \"%s\" },\n",
                  $codepage, $DEF_CHAR, $DEF_CHAR, $name;
    printf OUTPUT "    cp2uni,\n";
    printf OUTPUT "    uni2cp_low,\n";
    printf OUTPUT "    uni2cp_high\n};\n";
}


################################################################
# dump a DBCS mapping table
sub DUMP_DBCS_TABLE
{
    my ($codepage, $name) = @_;
    my $i, $x, $y;

    # build a list of lead bytes that are actually used

    my @lblist = ();
    LBLOOP: for ($y = 0; $y <= $#lead_bytes; $y++)
    {
        my $base = $lead_bytes[$y] << 8;
        for ($x = 0; $x < 256; $x++)
        {
            if (defined $cp2uni[$base+$x])
            {
                push @lblist,$lead_bytes[$y];
                next LBLOOP;
            }
        }
    }
    my $unused = ($#lead_bytes > $#lblist);

    # output the ascii->unicode table for the single byte chars

    printf OUTPUT "static const WCHAR cp2uni[%d] =\n", 256 * ($#lblist + 2 + $unused);
    printf OUTPUT "{\n%s,\n", DUMP_ARRAY( "0x%04x", $DEF_CHAR, @cp2uni[0 .. 255] );

    # output the default table for unused lead bytes

    if ($unused)
    {
        printf OUTPUT "    /* unused lead bytes */\n";
        printf OUTPUT "%s,\n", DUMP_ARRAY( "0x%04x", 0, ($DEF_CHAR) x 256 );
    }

    # output the ascii->unicode table for each DBCS lead byte

    for ($y = 0; $y <= $#lblist; $y++)
    {
        my $base = $lblist[$y] << 8;
        printf OUTPUT "    /* lead byte %02x */\n", $lblist[$y];
        printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", $DEF_CHAR, @cp2uni[$base .. $base+255] );
        printf OUTPUT ($y < $#lblist) ? ",\n" : "\n};\n\n";
    }

    # output the lead byte subtables offsets

    my @offsets = ();
    for ($x = 0; $x < 256; $x++) { $offsets[$x] = 0; }
    for ($x = 0; $x <= $#lblist; $x++) { $offsets[$lblist[$x]] = $x + 1; }
    if ($unused)
    {
        # increment all lead bytes offset to take into account the unused table
        for ($x = 0; $x <= $#lead_bytes; $x++) { $offsets[$lead_bytes[$x]]++; }
    }
    printf OUTPUT "static const unsigned char cp2uni_leadbytes[256] =\n";
    printf OUTPUT "{\n%s\n};\n\n", DUMP_ARRAY( "0x%02x", 0, @offsets );

    # count the number of unicode->ascii subtables that contain something

    my @filled = ();
    my $subtables = 1;
    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $uni2cp[$i];
        $filled[$i >> 8] = 1;
        $subtables++;
        $i |= 255;
    }

    # output all the subtables into a single array

    printf OUTPUT "static const unsigned short uni2cp_low[%d] =\n{\n", $subtables*256;
    for ($y = 0; $y < 256; $y++)
    {
        next unless $filled[$y];
        printf OUTPUT "    /* 0x%02x00 .. 0x%02xff */\n", $y, $y;
        printf OUTPUT "%s,\n", DUMP_ARRAY( "0x%04x", $DEF_CHAR, @uni2cp[($y<<8) .. ($y<<8)+255] );
    }
    printf OUTPUT "    /* defaults */\n";
    printf OUTPUT "%s\n};\n\n", DUMP_ARRAY( "0x%04x", 0, ($DEF_CHAR) x 256 );

    # output a table of the offsets of the subtables in the previous array

    my $pos = 0;
    my @offsets = ();
    for ($y = 0; $y < 256; $y++)
    {
        if ($filled[$y]) { push @offsets, $pos; $pos += 256; }
        else { push @offsets, ($subtables-1) * 256; }
    }
    printf OUTPUT "static const unsigned short uni2cp_high[256] =\n";
    printf OUTPUT "{\n%s\n};\n\n", DUMP_ARRAY( "0x%04x", 0, @offsets );

    # output the code page descriptor

    printf OUTPUT "const struct dbcs_table cptable_%03d =\n{\n", $codepage;
    printf OUTPUT "    { %d, 2, 0x%04x, 0x%04x, \"%s\" },\n",
                  $codepage, $DEF_CHAR, $DEF_CHAR, $name;
    printf OUTPUT "    cp2uni,\n";
    printf OUTPUT "    cp2uni_leadbytes,\n";
    printf OUTPUT "    uni2cp_low,\n";
    printf OUTPUT "    uni2cp_high,\n";
    DUMP_LB_RANGES();
    printf OUTPUT "};\n";
}


################################################################
# dump the list of defined lead byte ranges
sub DUMP_LB_RANGES
{
    my @list = ();
    my $i = 0;
    foreach $i (@lead_bytes) { $list[$i] = 1; }
    my $on = 0;
    printf OUTPUT "    { ";
    for ($i = 0; $i < 256; $i++)
    {
        if ($on)
        {
            if (!defined $list[$i]) { printf OUTPUT "0x%02x, ", $i-1; $on = 0; }
        }
        else
        {
            if ($list[$i]) { printf OUTPUT "0x%02x, ", $i; $on = 1; }
        }
    }
    if ($on) { printf OUTPUT "0xff, "; }
    printf OUTPUT "0x00, 0x00 }\n";
}


################################################################
# dump the case mapping tables
sub DUMP_CASE_MAPPINGS
{
    open OUTPUT,">casemap.c" or die "Cannot create casemap.c";
    printf "Building casemap.c\n";
    printf OUTPUT "/* Unicode case mappings */\n";
    printf OUTPUT "/* Automatically generated; DO NOT EDIT!! */\n\n";
    printf OUTPUT "#include \"wine/unicode.h\"\n\n";

    DUMP_CASE_TABLE( "casemap_lower", @tolower_table );
    DUMP_CASE_TABLE( "casemap_upper", @toupper_table );
    close OUTPUT;
}


################################################################
# dump a case mapping table
sub DUMP_CASE_TABLE
{
    my ($name,@table) = @_;

    # count the number of sub tables that contain something

    my @filled = ();
    my $pos = 512;
    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $table[$i];
        $filled[$i >> 8] = $pos;
        $pos += 256;
        $i |= 255;
    }
    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $table[$i];
        $table[$i] = ($table[$i] - $i) & 0xffff;
    }

    # dump the table

    printf OUTPUT "const WCHAR %s[%d] =\n", $name, $pos;
    printf OUTPUT "{\n    /* index */\n";
    printf OUTPUT "%s,\n", DUMP_ARRAY( "0x%04x", 256, @filled );
    printf OUTPUT "    /* defaults */\n";
    printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", 0, (0) x 256 );
    for ($i = 0; $i < 256; $i++)
    {
        next unless $filled[$i];
        printf OUTPUT ",\n    /* 0x%02x00 .. 0x%02xff */\n", $i, $i;
        printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", 0, @table[($i<<8) .. ($i<<8)+255] );
    }
    printf OUTPUT "\n};\n";
}


################################################################
# dump the ctype tables
sub DUMP_CTYPE_TABLES
{
    open OUTPUT,">wctype.c" or die "Cannot create casemap.c";
    printf "Building wctype.c\n";
    printf OUTPUT "/* Unicode ctype tables */\n";
    printf OUTPUT "/* Automatically generated; DO NOT EDIT!! */\n\n";
    printf OUTPUT "#include \"wine/unicode.h\"\n\n";

    my $i;
    my @array = (0) x 256;

    # add the direction in the high 4 bits of the category
    for ($i = 0; $i < 65536; $i++)
    {
        $category_table[$i] |= $direction_table[$i] << 12;
    }

    # try to merge table rows
    for ($row = 0; $row < 256; $row++)
    {
        my $rowtxt = sprintf "%04x" x 256, @category_table[($row<<8)..($row<<8)+255];
        if (defined($sequences{$rowtxt}))
        {
            # reuse an existing row
            $array[$row] = $sequences{$rowtxt};
        }
        else
        {
            # create a new row
            $sequences{$rowtxt} = $array[$row] = $#array + 1;
            push @array, @category_table[($row<<8)..($row<<8)+255];
        }
    }

    printf OUTPUT "const unsigned short wctype_table[%d] =\n{\n", $#array+1;
    printf OUTPUT "    /* offsets */\n%s,\n", DUMP_ARRAY( "0x%04x", 0, @array[0..255] );
    printf OUTPUT "    /* values */\n%s\n};\n", DUMP_ARRAY( "0x%04x", 0, @array[256..$#array] );

    close OUTPUT;
}


################################################################
# dump the char composition tables
sub DUMP_COMPOSE_TABLES
{
    open OUTPUT,">compose.c" or die "Cannot create compose.c";
    printf "Building compose.c\n";
    printf OUTPUT "/* Unicode char composition */\n";
    printf OUTPUT "/* Automatically generated; DO NOT EDIT!! */\n\n";
    printf OUTPUT "#include \"wine/unicode.h\"\n\n";

    ######### composition table

    my @filled = ();
    foreach $i (@compose_table)
    {
        my @comp = @$i;
        push @{$filled[$comp[1]]}, [ $comp[0], $comp[2] ];
    }

    # count how many different second chars we have

    for ($i = $count = 0; $i < 65536; $i++)
    {
        next unless defined $filled[$i];
        $count++;
    }

    # build the table of second chars and offsets

    my $pos = $count + 1;
    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $filled[$i];
        push @table, $i, $pos;
        $pos += @{$filled[$i]};
    }
    # terminator with last position
    push @table, 0, $pos;
    printf OUTPUT "const WCHAR unicode_compose_table[0x%x] =\n{\n", 2*$pos;
    printf OUTPUT "    /* second chars + offsets */\n%s", DUMP_ARRAY( "0x%04x", 0, @table );

    # build the table of first chars and mappings

    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $filled[$i];
        my @table = ();
        my @list = sort { $a->[0] <=> $b->[0] } @{$filled[$i]};
        for ($j = 0; $j <= $#list; $j++)
        {
            push @table, $list[$j][0], $list[$j][1];
        }
        printf OUTPUT ",\n    /* 0x%04x */\n%s", $i, DUMP_ARRAY( "0x%04x", 0, @table );
    }
    printf OUTPUT "\n};\n\nconst unsigned int unicode_compose_table_size = %d;\n\n", $count;

    ######### decomposition table

    # first determine all the 16-char subsets that contain something

    my @filled = (0) x 4096;
    my $pos = 16*2;  # for the null subset
    for ($i = 0; $i < 65536; $i++)
    {
        next unless defined $decomp_table[$i];
        $filled[$i >> 4] = $pos;
        $pos += 16*2;
        $i |= 15;
    }
    my $total = $pos;

    # now count the 256-char subsets that contain something

    my @filled_idx = (256) x 256;
    $pos = 256 + 16;
    for ($i = 0; $i < 4096; $i++)
    {
        next unless $filled[$i];
        $filled_idx[$i >> 4] = $pos;
        $pos += 16;
        $i |= 15;
    }
    my $null_offset = $pos;  # null mapping
    $total += $pos;

    # add the index offsets to the subsets positions

    for ($i = 0; $i < 4096; $i++)
    {
        next unless $filled[$i];
        $filled[$i] += $null_offset;
    }

    # dump the main index

    printf OUTPUT "const WCHAR unicode_decompose_table[%d] =\n", $total;
    printf OUTPUT "{\n    /* index */\n";
    printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", 0, @filled_idx );
    printf OUTPUT ",\n    /* null sub-index */\n%s", DUMP_ARRAY( "0x%04x", 0, ($null_offset) x 16 );

    # dump the second-level indexes

    for ($i = 0; $i < 256; $i++)
    {
        next unless ($filled_idx[$i] > 256);
        my @table = @filled[($i<<4)..($i<<4)+15];
        for ($j = 0; $j < 16; $j++) { $table[$j] ||= $null_offset; }
        printf OUTPUT ",\n    /* sub-index %02x */\n", $i;
        printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", 0, @table );
    }

    # dump the 16-char subsets

    printf OUTPUT ",\n    /* null mapping */\n";
    printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", 0, (0) x 32 );

    for ($i = 0; $i < 4096; $i++)
    {
        next unless $filled[$i];
        my @table = (0) x 32;
        for ($j = 0; $j < 16; $j++)
        {
            if (defined $decomp_table[($i<<4) + $j])
            {
                $table[2 * $j] = ${$decomp_table[($i << 4) + $j]}[0];
                $table[2 * $j + 1] = ${$decomp_table[($i << 4) + $j]}[1];
            }
        }
        printf OUTPUT ",\n    /* 0x%03x0 .. 0x%03xf */\n", $i, $i;
        printf OUTPUT "%s", DUMP_ARRAY( "0x%04x", 0, @table );
    }

    printf OUTPUT "\n};\n";
    close OUTPUT;
}


################################################################
# read an input file and generate the corresponding .c file
sub HANDLE_FILE
{
    my ($codepage,$filename,$comment) = @_;

    # symbol codepage file is special
    if ($codepage == 42) { READ_SYMBOL_FILE($MAPPREFIX . $filename); }
    else { READ_FILE($MAPPREFIX . $filename); }

    ADD_DEFAULT_MAPPINGS();

    my $output = sprintf "c_%03d.c", $codepage;
    open OUTPUT,">$output" or die "Cannot create $output";

    printf "Building %s from %s (%s)\n", $output, $filename, $comment;

    # dump all tables

    printf OUTPUT "/* code page %03d (%s) */\n", $codepage, $comment;
    printf OUTPUT "/* generated from %s */\n", $MAPPREFIX . $filename;
    printf OUTPUT "/* DO NOT EDIT!! */\n\n";
    printf OUTPUT "#include \"wine/unicode.h\"\n\n";

    if ($#lead_bytes == -1) { DUMP_SBCS_TABLE( $codepage, $comment ); }
    else { DUMP_DBCS_TABLE( $codepage, $comment ); }
    close OUTPUT;
}


################################################################
# output the list of codepage tables into the cptable.c file
sub OUTPUT_CPTABLE
{
    @tables_decl = ();

    foreach $file (@allfiles)
    {
        my ($codepage,$filename,$comment) = @$file;
        push @tables_decl, sprintf("extern union cptable cptable_%03d;\n",$codepage);
    }

    push @tables_decl, sprintf("\nstatic const union cptable * const cptables[%d] =\n{\n",$#allfiles+1);
    foreach $file (@allfiles)
    {
        my ($codepage,$filename,$comment) = @$file;
        push @tables_decl, sprintf("    &cptable_%03d,\n", $codepage);
    }
    push @tables_decl, "};";
    REPLACE_IN_FILE( "cptable.c", @tables_decl );
}

################################################################
# replace the contents of a file between ### cpmap ### marks

sub REPLACE_IN_FILE
{
    my $name = shift;
    my @data = @_;
    my @lines = ();
    open(FILE,$name) or die "Can't open $name";
    while (<FILE>)
    {
	push @lines, $_;
	last if /\#\#\# cpmap begin \#\#\#/;
    }
    push @lines, @data;
    while (<FILE>)
    {
	if (/\#\#\# cpmap end \#\#\#/) { push @lines, "\n", $_; last; }
    }
    push @lines, <FILE>;
    open(FILE,">$name") or die "Can't modify $name";
    print FILE @lines;
    close(FILE);
}
