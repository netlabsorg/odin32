# $Id: doxygen.mak,v 1.2 2002-06-19 23:16:36 bird Exp $

#
# This script contains the testcases for Doxygen version 1.2.16
#
# Note! We doesn't include any MAKE_INCLUDE_PROCESS file.
#

# Include the setup.mak and testsuite.tools.mk.
PATH_ROOT = ..\..
!include $(PATH_ROOT)\$(BUILD_SETUP_MAK)
!include $(PATH_MAKE)\testsuite.tools.mk


# Testsuite config for the process include.
TARGET_MODE = TESTCASE
TARGET_NAME = doxygen
MAKEFILE    = $(TARGET_NAME).mak

# Include the process rulez.
!include $(MAKE_INCLUDE_PROCESS)



#
# These are the files which should be installed in order to run doxygen.
#
DOXYGEN = $(PATH_ROOT)\testsuite\bin\doxygen.exe
BIN_FILES =\
    $(DOXYGEN)


#
# Main rule, this runs all the other rulez.
#
testcase:   $(BIN_FILES)
    @$(ECHO) + Running testcases for DoxyGen v1.2.16! $(CLRRST)
    $(TS_EXEC_1) "help" $(TS_EXEC_2) testcase-help
    $(TS_EXEC_1) "kFileFormatBase" $(TS_EXEC_2) testcase-kFileFormatBase
    @$(ECHO) + Completed DoxyGen v1.2.16! $(CLRRST)


#
# This testcase checks the --help output.
# BUGBUG: why doesn't DOXYGEN.EXE output an absolute path?
#
testcase-help: $(PATH_TARGET)\doxygen.help.stdout
    -1 $(TS_PEC) $(DOXYGEN) --help > $(PATH_TARGET)\stdout 2> $(PATH_TARGET)\stderr
    $(TS_TEST_FILE_Z)   $(PATH_TARGET)\stderr
    $(TS_TEST_FILE_NZ)  $(PATH_TARGET)\stdout
    $(TS_SED) "s/ [.][.].*\\doxygen.exe/ DOXYGEN.EXE/I" $(PATH_TARGET)\stdout | $(TS_CMP) - $(PATH_TARGET)\doxygen.help.stdout

# This is the output of testcase-help
$(PATH_TARGET)\doxygen.help.stdout: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
Doxygen version 1.2.16
Copyright Dimitri van Heesch 1997-2002

You can use doxygen in a number of ways:

1) Use doxygen to generate a template configuration file:
    DOXYGEN.EXE [-s] -g [configName]

    If - is used for configName doxygen will write to standard output.

2) Use doxygen to update an old configuration file:
    DOXYGEN.EXE [-s] -u [configName]

3) Use doxygen to generate documentation using an existing configuration file:
    DOXYGEN.EXE [configName]

    If - is used for configName doxygen will read from standard input.

4) Use doxygen to generate a template style sheet file for RTF, HTML or Latex.
    RTF:   DOXYGEN.EXE -w rtf styleSheetFile
    HTML:  DOXYGEN.EXE -w html headerFile footerFile styleSheetFile [configFile]
    LaTeX: DOXYGEN.EXE -w latex headerFile styleSheetFile [configFile]

5) Use doxygen to generate an rtf extensions file
    RTF:   DOXYGEN.EXE -e rtf extensionsFile

If -s is specified the comments in the config file will be omitted.
If configName is omitted `Doxyfile' will be used as a default.

<<KEEP



#
# Testcase where we try to run doxygen on tools/common/kFileFormatBase.[cpp|h].
#
testcase-kFileFormatBase: \
            $(PATH_TARGET)\kFileFormatBase.conf \
            $(PATH_TARGET)\kFileFormatBase.stdout \
            $(PATH_TARGET)\kFileFormatBase.html.dir \
            $(PATH_TARGET)\kFileFormatBase.latex.dir
    $(TS_PEC) $(DOXYGEN) $(PATH_TARGET)\kFileFormatBase.conf 2> $(PATH_TARGET)\stderr2 | $(TS_TEE) $(PATH_TARGET)\stdout2
    $(TS_SED) "s/Read .* bytes/Read x bytes/" $(PATH_TARGET)\stdout2 \
        | $(TS_SED) "s/Preprocessing .*[\/\\]kFile/Preprocessing kFile/" \
        | $(TS_SED) "s/Parsing file .*[\/\\]kFile/Parsing file kFile/" \
        | $(TS_CMP) - $(PATH_TARGET)\kFileFormatBase.stdout
    $(TS_TEST_FILE_Z) $(PATH_TARGET)\kFileFormatBase.stderr2
    $(TS_DIR_B) $(PATH_TARGET)\html  | $(TS_SED) "s/.*\\//g" | $(TS_CMP) - $(PATH_TARGET)\kFileFormatBase.html.dir
    $(TS_DIR_B) $(PATH_TARGET)\latex | $(TS_SED) "s/.*\\//g" | $(TS_CMP) - $(PATH_TARGET)\kFileFormatBase.latex.dir
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\rtf\refman.rtf
    $(TS_TEST_DIR) $(PATH_TARGET)\man\man3
    $(TS_TEST_FILE_NZ) $(PATH_TARGET)\man\man3\kFileFormatBase.3


$(PATH_TARGET)\kFileFormatBase.conf: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
# Doxyfile 1.2.16
PROJECT_NAME           = Odin32Testsuite-kFileFormatBase
INPUT                  = $(PATH_ROOT)\tools\common\kFileFormatBase.h $(PATH_ROOT)\tools\common\kFileFormatBase.cpp
GENERATE_HTML          = YES
HTML_OUTPUT            = $(PATH_TARGET)\html
GENERATE_HTMLHELP      = YES
GENERATE_CHI           = YES
GENERATE_TREEVIEW      = YES
# INCLUDE_PATH           = $(PATH_ROOT)\tools\common - doesn't work.
GENERATE_LATEX         = YES
LATEX_OUTPUT           = $(PATH_TARGET)\latex
GENERATE_XML           = NO
# XML_OUTPUT             = $(PATH_TARGET)\xml - doesn't work
GENERATE_MAN           = YES
MAN_OUTPUT             = $(PATH_TARGET)\man
GENERATE_RTF           = YES
RTF_OUTPUT             = $(PATH_TARGET)\rtf
SEARCHENGINE           = NO
<<KEEP

# stdout
$(PATH_TARGET)\kFileFormatBase.stdout: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
Searching for include files...
Searching for example files...
Searching for images...
Searching for dot files...
Searching for files to exclude
Reading input files...
Reading tag files
Preprocessing kFileFormatBase.h...
Preprocessing kFileFormatBase.cpp...
Read x bytes
Parsing input...
Parsing file kFileFormatBase.h...
Parsing file kFileFormatBase.cpp...
Freeing input...
Building group list...
Building namespace list...
Building file list...
Building class list...
Building example list...
Searching for documented variables...
Building member list...
Searching for friends...
Searching for documented defines...
Computing template instances...
Creating members for template instances...
Computing class relations...
Searching for enumerations...
Searching for member function documentation...
Building page list...
Search for main page...
Sorting lists...
Freeing entry tree
Determining which enums are documented
Computing member relations...
Building full member lists recursively...
Adding members to member groups.
Computing member references...
Inheriting documentation...
Adding source references...
Adding todo/test/bug list items...
Counting data structures...
Generating disk names...
Resolving user defined references...
Generating index page...
Generating file index...
Generating example documentation...
Generating file sources...
Generating code for file kFileFormatBase.h...
Generating file documentation...
Generating class documentation...
Generating annotated compound index...
Generating hierarchical class index...
Generating member index...
Generating docs for compound kFileFormatBase...
Generating page documentation...
Generating group documentation...
Generating namespace index...
Generating group index...
Generating example index...
Generating file member index...
Generating namespace member index...
Generating page index...
Generating graph info page...
Generating search index...
Generating style sheet...
Combining RTF output...
<<KEEP

# html files created
$(PATH_TARGET)\kFileFormatBase.html.dir: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
annotated.html
classkFileFormatBase-members.html
classkFileFormatBase.html
doxygen.css
doxygen.png
files.html
ftv2blank.png
ftv2doc.png
ftv2folderclosed.png
ftv2folderopen.png
ftv2lastnode.png
ftv2link.png
ftv2mlastnode.png
ftv2mnode.png
ftv2node.png
ftv2plastnode.png
ftv2pnode.png
ftv2vertline.png
functions.html
index.hhc
index.hhk
index.hhp
index.html
kFileFormatBase_8h-source.html
main.html
tree.html
tree.js
treeview.js
<<KEEP

# latex files created
$(PATH_TARGET)\kFileFormatBase.latex.dir: $(MAKEFILE)
    @$(TOOL_ECHO) Creating$(CLRRST) <<$@
annotated.tex
classkFileFormatBase.tex
doxygen.sty
Makefile
refman.tex
<<KEEP



#
# Clean rule
#
clean:
    $(TOOL_RM) *.log $(PATH_TARGET)\html\* $(PATH_TARGET)\latex\* $(PATH_TARGET)\man\man3\*
    $(TOOL_RM) $(PATH_TARGET)\rtf\* $(PATH_TARGET)\xml\*
    -$(TOOL_RMDIR) $(PATH_TARGET)\html     2> nul
    -$(TOOL_RMDIR) $(PATH_TARGET)\latex    2> nul
    -$(TOOL_RMDIR) $(PATH_TARGET)\man\man3 2> nul
    -$(TOOL_RMDIR) $(PATH_TARGET)\man      2> nul
    -$(TOOL_RMDIR) $(PATH_TARGET)\rtf      2> nul
    -$(TOOL_RMDIR) $(PATH_TARGET)\xml      2> nul
    -$(TOOL_RM) $(PATH_TARGET)\*

