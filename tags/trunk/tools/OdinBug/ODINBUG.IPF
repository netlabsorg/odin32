:userdoc.:title.OdinBug Help 0.5.8
:docprof toc=123.
.***************************************************
.*** IPF-Generator Version 0.82 for Framework IV ***
.***************************************************

:h1 res=100 x=right y=bottom width=66% height=100%.Introduction

:artwork align=center name='E:\DEV\REXX\VXREXX\PROJECTS\ODINBUG\ODIN.BMP' fit.:font facename='Default' size=0x0.
:p.OdinBug is a helper application for Odin&comma. the Win32-subsystem of
OS&slr.2 (eCS) . Its purpose is to ease the creation of proper bug reports.
:p.If everything goes well&comma. you just have to specify :link reftype=hd res=220.a proper logfile:elink. and
some more (non technical) information&comma. OdinBug does (or should do) the
rest for you.
:p.This document has three purposes&colon.
:ol compact.
:li.It serves as the online help for OdinBug.
:li.It should enable you to setup Odin to create :link reftype=hd res=220.a proper logfile:elink..
:li.It should enable you to create proper bug reports&comma. without having to
look anywhere else.
:eol.
:p.Please read on&comma. and help Team Odin in development by submitting proper
and useful bug reports!
:p.Look :link reftype=hd res=850.here:elink. if you have questions about the installation of OdinBug itself.
:i1.Introduction

:h2 res=120 x=right y=bottom width=66% height=100%.Reporting Odin bugs

:p.:font facename='Default' size=0x0.To make our job easier when dealing with reports of failures to run 
certain applications&comma. there's a :link reftype=hd res=840.standard form:elink. to report bugs. Use
OdinBug to create these bug reports.

:note.Before you can create a bugreport&comma. you need to :link reftype=hd res=220.create a proper logfile:elink.!

:p.Some remarks&colon.
:ul.:font facename='Helv' size=8x4.
:li.Always try the debug build with the application unless only the release build
shows the problem.
:li.Make sure you have installed Odin properly (using WarpIN).
:li.If you installed using a ZIP file&comma. do not forget to run :link reftype=fn refid=1.OdinInst.EXE:elink.
after unpacking the ZIP file! 
:li.PE.EXE is the recommended way to run Windows applications; remove Win32K.SYS
from your CONFIG.SYS
:li.Use a recent daily build. Bug reports with ancient (> 1 week old) builds are
not very useful. The problem you report might no longer be present!
:li.Always send these bug reports to the odinusers mailing list; not directly to
the developers (so others can respond if the problem is old or already
fixed).
:li.Currently the only valid mailing list address is:hp2. odinusers@yahoogroups.com:ehp2..
In order to send a bug report you have to be subscribed to this mailing list.
:li.Go to :hp2.http&colon.&slr.&slr.groups.yahoo.com&slr.groups&slr.odinusers&slr.join
:ehp2.in order to subscribe (or use the URL object that OdinBug has created in your
Odin folder.)
:li.Look here to check how to :link reftype=hd res=220.create proper logfiles:elink.!
:eul.:font facename='Default' size=0x0.
:p.Sometimes the small part of the logfile isn't enough for us to determine 
where the problem lies.
:p.In that case&comma. we'll ask you to mail us the entire file (ZIPped).
So please :hp2.do not:ehp2. send big logfiles to the mailing list!
:fn id=1.:p.When running OdinInst.EXE&comma. be sure to type in :font facename='Courier' size=14x8.OdinInst.EXE:font facename='Default' size=0x0. on the
commandline (do not leave out the .EXE!)&comma. because it is a known issue
that on some systems OdinInst.EXE hangs otherwise.:efn.
:i1.OdinInst.EXE hang workaround
:i1.Reporting bugs

:h2 res=130 x=right y=bottom width=66% height=100%.Useful tips

:p.:font facename='Default' size=0x0.Crashes are always easy to locate in logfiles (search for 'Exception Information')&comma. 
but improper behaviour isn't.
:p.In the system menu (top left button of a window)&comma. there's a menu item called
'Put mark in debug log'. Selecting this item puts the line 'Mark requested 
by user' in the log. 
:p.This can be useful e.g. if a window is resized improperly after you click
on a button. To make it easy for us to locate the right log lines&comma. you
should select the 'Put mark in debug log' menu item and then click on the
button that causes the problem.
:p.Look here to check how to :link reftype=hd res=220.create proper logfiles:elink.!
:i1.Useful tips

:h2 res=140 x=right y=bottom width=66% height=100%.How to create a bug report in a few easy steps

:p.:font facename='Default' size=0x0.Ideally OdinBug should be called immediately after you experienced a crash
in a Win32 program and after you have :link reftype=hd res=220.created a log file:elink..

:ol.:font facename='Helv' size=8x4.
:li.OdinBug prompts you to enter the :link reftype=hd res=220.logfile:elink. (odin32_?.log) that was created 
during the execution of the Win32 program.

:li.OdinBug tries to determine which :link reftype=hd res=320.build:elink. you have installed. If it cannot
find an appropriate :link reftype=hd res=320.buildfile:elink.&comma. it prompts you to enter the currently installed 
Odin-build file (i.e odin32bin-200?????.ZIP or odin32bin-200?????.WPI) in 
a standard OS&slr.2 (eCS) file open dialogue.

:note.If OdinBug was installed properly&comma. you may doubleclick on a odin32_?.log
file and OdinBug will be fired up. If you saved OdinBug's settings to disk and
your buildfiles all reside in the same :link reftype=hd res=530.directory:elink.&comma. you will not even have to
specify the buildfile.

:li.Complete :link reftype=hd res=240.name:elink.&comma. :link reftype=hd res=250.version:elink. and eventually a :link reftype=hd res=270.location:elink.&comma. where the program can be 
downloaded on the :link reftype=hd res=200.Application page:elink.. Then enter your own :link reftype=hd res=280.verbal description:elink. 
of the problem.

:p.All other fields on the other pages of the notebook have probably been
filled automatically. Complete any missing information.

:li.When everything is complete press ":link reftype=hd res=720.Create:elink." button. The program 
continuosly checks for empty fields&comma. and will only let you create a 
report if everything is filled out.

:li.Press ":link reftype=hd res=730.EMail:elink.". For :link reftype=hd res=630.Netscape Messenger:elink.&comma. :link reftype=hd res=640.PMMail:elink. and :link reftype=hd res=650.PolarBar:elink. users the whole email is
created automatically&comma. users of :link reftype=hd res=650.other email clients:elink. have to paste the report
into the body of the email.

:note.A bugreport contains important information about your system. The emails are 
created automatically for you&comma. but you always will have to send them yourself.
Sending important system information without user interaction would be Micros0ft 
style and you will not see something like that from me!
:eol.:font facename='Default' size=0x0.

:h1 res=200 x=right y=bottom width=66% height=100%.Application

:p.:font facename='Default' size=0x0.The "Application" page of OdinBug presents a form to fill in all
kind of information about the program that you want to create a
bug report for.
:p.The following items have to be entered manually&comma. because
they cannot be gathered from various other sources on the
system&colon.
:ul.
:li.:link reftype=hd res=240.Application name:elink.
:li.:link reftype=hd res=250.Application version:elink.
:li.:link reftype=hd res=270.Application URL:elink.
:li.:link reftype=hd res=280.Verbal problem description:elink.
:eul.
:p.Without entering these 4 items&comma. OdinBug will not allow you
to create a bug report!
:i1.Application page

:h2 res=220.Odin logfiles and how to create them

:p.:font facename='Default' size=0x0.The logfile is the main source of bug information for OdinBug. It
is impossible to create a bugreport without a logfile!

:p.You only get logfiles if you use a debug build and have the
environment variable
:p.:font facename='Courier' size=14x8.         SET WIN32LOG_ENABLED=1:font facename='Default' size=0x0.
:p.properly set. This turns on logging for all debug dlls.

:p.On bottom of the "Odin" notebook page of OdinBug there is a
checkmark that shows you if logging is turned on for the
session&comma.
in which OdinBug runs. :hp2.Unfortunately this cannot be used to turn
logging on or off!

:note.:ehp2.Only those builds&comma. that have "debug" in their name can
generate logfiles&comma. no matter if daily or weekly
builds&comma. in order to show what a win32 application is
doing&comma. while it is running. The logfile is inevitable to
determine why certain applications don't run correctly.

:p.The name of the logfile follows a naming scheme&colon.
:p.:font facename='Courier' size=14x8.         odin32_?.log:font facename='Default' size=0x0.&comma. where ? is 0&comma. 1&comma. 2 etc.

:p.The major disadvantage of logging is the overhead. Therefore it
has been disabled by default in all the "debug" builds. :hp2.Only if
the environment variable is set&comma. a logfile will be created!

:p.:ehp2.However&comma. this can result in huge logfiles. To reduce the
size&comma. use release builds of dlls that are not relevant to
the problem you're seeing. The general rule is that usually only
the logging information generated by :font facename='Courier' size=14x8.GDI32:font facename='Default' size=0x0.&comma. :font facename='Courier' size=14x8.KERNEL32:font facename='Default' size=0x0. and
:font facename='Courier' size=14x8.USER32:font facename='Default' size=0x0. is important.
:p.Use the release build of :font facename='Courier' size=14x8.GDI32:font facename='Default' size=0x0. if the problem you're seeing isn't
related to painting. (:font facename='Courier' size=14x8.GDI32:font facename='Default' size=0x0. generates a lot of logging
information)

:p.To disable logging again&comma. you must :hp2.clear:ehp2.  this variable (do
not set it to 0)&colon.
:p.:font facename='Courier' size=14x8.         SET WIN32LOG_ENABLED=:font facename='Default' size=0x0.

:p.There is also :link reftype=hd res=820.an extended logging feature:elink.&comma. which is not so
important for everyday user.

:hp7.:p.Source of information&colon.:ehp7.

:p.The logfile should be provided as a parameter to OdinBug.
Alternatively&comma. the Search button can be pressed to locate
the file in an standard filesearch dialogue.
:i1.Odin logfile

:h2 res=230.Windows executable

:p.:font facename='Default' size=0x0.The field "Windows Executable" should hold the name of the binary
program file&comma. that was used to run the Win32-application.
:p.If the name is not what you think it should be&comma. you might
have accidentally picked the wrong logfile!
:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the odin32_?.log file.
:i1.Windows executable

:h2 res=240.Application name

:p.:font facename='Default' size=0x0.The application name is not meant to be the name of the
executable&comma. but the name of the program as it is
advertised&comma. for example&colon.

:xmp.:ul compact.
:li.Adobe Acrobat Reader
:li.Mesonic Corporate WINLine
:li.SUN StarOffice
:li.RealPlayer Basic
:eul.:exmp.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information has to be provided by the user.
:i1.Application name

:h2 res=250.Version

:p.:font facename='Default' size=0x0.The version field should contain the exact revision number of the
program to make it easy for Team Odin to exactly identify the
program you are using. For example&colon.

:xmp.:ul compact.
:li.5.0.1
:li.7.0.8 (453)
:li.8.0lite
:eul.
:exmp.
:hp7.:p.Source of information&colon.:ehp7.

:p.This information has to be provided by the user.
:i1.Application version
:i1.Version

:h2 res=260.Ohhh, no

:p.:font facename='Default' size=0x0.Set the "Ohh&comma. no!" checkmark&comma. if a popup messagebox
saying "Ohhh&comma. no! Unhandled exception..." was shown during
the program was run.
:hp7.:p.Source of information&colon.:ehp7.

:p.This information has to be provided by the user.
:i1.Unhandled exception
:i1.Ohhh&comma. no!

:h2 res=270.Available from

:p.:font facename='Default' size=0x0.The purpose of the "Available from" field is to tell Team Odin
from where they eventually could achieve the application for
testing it with Odin. This could be a URL or Webpage.
:p.Do not send them programs automatically without being asked for!
:hp7.:p.Source of information&colon.:ehp7.

:p.This information has to be provided by the user.
:i1.URL
:i1.Available from

:h2 res=280.Verbal description

:p.:font facename='Default' size=0x0.The "Verbal description" field is the plcae where you (the user)
can express in your own words&comma. what you have done&comma.
seen or heard.
:p.Your description should be short and as precise as possible. For
example&colon.
:xmp.It started up completely&comma. but crashed 
after choosing menu option xyz.
:exmp.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information has to be provided by the user.
:i1.Verbal description

:h2 res=290.Exception page

:p.:font facename='Default' size=0x0.The "Exception"page shows the most important techniscal
information&comma. that Team Odin needs in order to track down a
bug.
:p.There is no need for the user to change anything on this
page&comma. it is just presented for convenience purposes in
order to be able to go through a number of logfiles and quickly
check whether they all fail at the same (or similar) postion.

:h1 res=300 x=right y=bottom width=66% height=100%.Odin

:p.:font facename='Default' size=0x0.The "Odin" page shows all the necessary information about your
installed Odin-subsystem.
:p.All information is gathered from various source around your
system automatically.
:p.Only change the entries&comma. if you are very sure the automatic
detection was not correct.
:p.The only exception the is the Odin build file&comma. OdinBug has
a sort of guessing algorithm built in but like every guess&comma.
it might be wrong.

:h2 res=320.Current Odin build

:p.:font facename='Default' size=0x0.The "Current Odin build" field shows the filename of the
currently installed Odin build. As Odin buildfiles follow a
strict name scheme&comma. this can usually be determined
automatically.
:p.In case the expected buildfile is not found in the buildfile
directory&comma. the user is prompted to point to the appropriate
file using a convenient file search dialogue.
:p.It can be specified manually by pressing the Search button.
:hp7.:p.Source of information&colon.:ehp7.

:p.This information has either to be specified by the user or it is
guessed from a combination of kernel32.dll revision and buildfile
availability.
:i1.Odin buildfile

:h2 res=330.Installation method

:p.:font facename='Default' size=0x0.The field "Installation method" shows whether the last install
was done from a daily build ZIP file or from a weekly build WPI
file.

:note.Only debug build (daily or weekly) an be used to generate bug
reports. You might however want to use some .DLLs from the
release build to generate a :link reftype=hd res=220.smaller logfile:elink..

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the build file.
:i1.Installation method

:h2 res=340.Launching method

:p.:font facename='Default' size=0x0.The "Launching method" field shows whether the Win32 program was
launched using PE.EXE&comma. PEC.EXE or the transparent loader
Win32K.SYS.

:note.Team Odin does not recommend to use PE.EXE&comma. if Win32K.SYS
is installed.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from CONFIG.SYS and verified from the
odin32_?.log file.
:i1.Launching method

:h2 res=350.Win32K.SYS parameters

:p.:font facename='Default' size=0x0.As Win32K.SYS can operate in various modes&comma. the "WIN32K.SYS
parameters" field shows which parameters were used.

:p.Look at the :link reftype=launch object='E.EXE' data='README.TXT'.readme.txt:elink. :link reftype=fn refid=2.(*):elink. that comes with Odin in order to find
out more about Win32K.SYS parameters.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from CONFIG.SYS.
:fn id=2.:p.The link to this external textfile does not work&comma. if
OdinBug was called from the commandline&comma. sorry!:efn.
:i1.Win32K.SYS parameters

:h2 res=360.Last WarpIN installation

:p.:font facename='Default' size=0x0.It is essential that Odin is installed properly. UnZIPping the
files only&comma. is not enough to have a proper Odin
environment.
:p.Therefore the installtype is checked before creating a bug report
in order to make sure Odin is installed properly&comma. before
you supply a bug report.
:note.If you install Odin using the ZIP files&comma. you have at least
run :link reftype=fn refid=1.OdinInst.EXE:elink. once in order to update the registry properly!

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the WarpIN database.
:i1.WarpIN installation

:h2 res=370.Last OdinInst installation

:p.:link reftype=fn refid=1.:font facename='Default' size=0x0.OdinInst.EXE:elink. updates the registry and creates a bunch of
directories.
:p.Both are vital to have a proper Odin environment.
:p.OdinBug therefore checks&comma. when :link reftype=fn refid=1.OdinInst.EXE:elink. was run for the
last time.
:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from Odin.INI.
:i1.OdinInst.EXE installation

:h2 res=380.Odin "Windows" directory

:p.:font facename='Default' size=0x0.Odin creates a directory structure&comma. which resembles the one
usually found  in a real Windows.
:p.The "Windows" directory of Odin is the directory that is
equivalent to C&colon.&bsl.WINDOWS or C&colon.&bsl.WinNT in a
real Windows.
:note.Never&comma. never install Odin into a real "Windows"
directory&comma. the Windows installation will be irreversibly
damaged!
:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the odin32_?.log file.
:i1.Windows directory

:h2 res=390.Odin "System" directory

:p.:font facename='Default' size=0x0.Odin creates a directory structure&comma. which resembles the one
usually found  in a real Windows.
:p.The "System" directory of Odin is the directory that is
equivalent to C&colon.&bsl.WINDOWS&bsl.SYSTEM or
C&colon.&bsl.WinNT&bsl.SYSTEM32 in a real Windows.
:p.This the directory&comma. where most Odin files reside.
:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the odin32_?.log file.
:i1.System directory

:h2 res=3100.Evaluated Odin.INI

:p.:font facename='Default' size=0x0.Odin.INI stores specific settings about the Odin environment.
:p.It is recreated every time :link reftype=fn refid=1.OdinInst.EXE:elink. is called.
:p.For currently implemented Odin.INI settings read this file
(:link reftype=launch object='E.EXE' data='ODIN.INI.TXT'.Odin.Ini.txt:elink. :link reftype=fn refid=2.(*):elink.).
:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the odin32_?.log file.
:i1.Odin.INI

:h1 res=400 x=right y=bottom width=66% height=100%.System

:p.:font facename='Default' size=0x0.The "System" page provides necessary information about your
system&comma. especially about your Operating System and your
display driver.
:p.A proper detection for the display driver&comma. chipset and
revision is only implemented for Scitech drivers.

:h2 res=420.OS/2 system type and version

:p.:font facename='Default' size=0x0.The OS&slr.2 (eCS) system type and version is of general
importance for Team Odin. It simply tells whether you run Warp
3&comma. 4&comma. 4.5 aka MCP&comma. WarpServer for eBusiness or
eComStation.

:p.Note&comma. that Warp 3 is not officially supported by Odin!

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the SYSLEVEL.OS2 file.
:i1.OS&slr.2 (eCS) type

:h2 res=430.OS/2 reports and kernel revision

:p.:font facename='Default' size=0x0.This information is of general importance for Team Odin.

:hp7.:p.Source of information&colon.:ehp7.

:p.OS&slr.2 (eCS) is asked for its version and the kernel revision
is directly read from OS2KRNL.
:p.kernel revision
:p.eCS version
:p.OS&slr.2 version

:h2 res=440.FixPack level

:p.:font facename='Default' size=0x0.The "FixPack level" field shows on which fixpack level your
system is.
:p.Please provide this information manually&comma. if you are on a
fixpack level prior to Warp 4 Fixpack 10&comma. because the
SYSLEVEL.FPK file&comma. from which this information is
gathered&comma. did not exist prior to Fixpack 10.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from the SYSLEVEL.FPK file.


:i1.fixpack level

:h2 res=450.Display driver

:p.:font facename='Default' size=0x0.The "Display driver" field indicates&comma. which video driver
you are currently using.
:p.This information is very important for Team Odin&comma. because
some bugs show up only on some video drivers and not on others.


:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from CONFIG.SYS.
:i1.Display driver

:h2 res=460.Display driver version

:p.:font facename='Default' size=0x0.As the name indicates&comma. the "display driver version" field
shows the driver revision of your video adapter driver.
:p.Please provide this information manually&comma. if you do not run
the Scitech driver suite.
:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from SDDHELP.SYS. It is only determined
for Scitech drivers.
:i1.Display driver version

:h2 res=470.Display chipset

:p.:font facename='Default' size=0x0.The "Display chipset" field indicates the video chipset your
video adapter is using.
:p.Please provide this information manually&comma. if you do not run
the Scitech driver suite.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from GRAPHICS.LOG. It is only
determined for Scitech drivers.
:i1.Display chipset
:i1.Video chipset

:h2 res=480.XF86Sup.SYS loaded

:p.:font facename='Default' size=0x0.This checkbox shows whether Holger Veit's XFree86 support driver
named XF86SUP.SYS is installed on your system.

:note.OdinBug version 0.5.3 had a bug that made it crash&comma. if
XF86Sup.SYS was not installed. This has been fixed in version
0.5.3a or better.

:hp7.:p.Source of information&colon.:ehp7.

:p.This information is taken from CONFIG.SYS.
:i1.XFree86&slr.OS2
:i1.XF86Sup.SYS

:h1 res=500 x=right y=bottom width=66% height=100%.Settings

:p.:font facename='Default' size=0x0.The "Settings" page displays all configurable options for
OdinBug.
:p.By setting these you can change the behaviour of OdinBug to a
limited extent.

:h2 res=520.Add exception info and stack dump and line buffer

:p.:font facename='Default' size=0x0.For Team Odin the exception information and the stack dump is the
most important information&comma. when trying to fix a bug.
:p.The exception information is found on the page "Exception". This
is exactly what gets added to the bug report of this option is
set.
:p.You should generally not uncheck this&comma. because in most
cases this will make your bug report useless!

:h2 res=530.Default build directory

:p.:font facename='Default' size=0x0.This is the directory where you store your complete buildfiles
(i.e the odin32bin-20011010-debug.ZIP or .wpi files).

:h2 res=540.Application directory

:p.:font facename='Default' size=0x0.The application directory is the directory&comma. where OdinBug
should look for logfiles. This is only useful if you have one
application that you want to make bug reports for.
:p.The primary purpose of this setting was to make my life easier
while testing OdinBug.
:p.So normally&comma. the check box "do not save application
directory" should be checked.

:h2 res=550.Logfile browser

:p.:font facename='Default' size=0x0.In the logfile browser field you can specify a program&comma.
that you want to use to manually investigate the odin32_?.log
file.
:p.The VIO version of Michael Shacter's HyperView is a very good
program for this purpose.
:p.On the other hand&comma. you even might specify E.EXE or any
other editor&comma. if that suits your needs.

:h2 res=560.Ask, whether to delete the logfile

:p.:font facename='Default' size=0x0.If this option is set&comma. OdinBug will ask you&comma. if you
would like to delete the currently used odin32_?.log before
OdinBug itself is closed.
:p.Please note&comma. that the corresponding pe_?.log is not
deleted.

:h1 res=600 x=right y=bottom width=66% height=100%.EMail

:p.:font facename='Default' size=0x0.The email settings let you specify&comma. which email client
OdinBug should use for sending the email that holds the bug
report.
:p.The email support is designed to provide a generic support for
every email client that understand the "mailto&colon." statement.
However&comma. using this generic support&comma. you will have to
paste the bug report from the clipboard into the body of the
message. depending on which email client you use&comma. you also
might have to complete the subject line.
:p.The following email clients have an enhanced support&comma. which
does everything automatically&colon.
:ul.
:li.:link reftype=hd res=630.Netscape Messenger 4.61:elink.
:li.:link reftype=hd res=640.PMMail:elink.
:li.:link reftype=hd res=650.PolarBar:elink.
:eul.
:p.Otherwise you have to use the :link reftype=hd res=650.generic email support:elink..

:note.If you updated OdinBug from a release prior to 0.5.5 and you
experience problems using the email functionality&comma.
please&comma. change your email settings to generic&comma. save
the settings and then change back to your favorite email client
and reenter all fields and save again. Now your problem should be
gone!

:i1.EMail configuration
:i1.EMail does not work

:h2 res=620.EMail address

:p.:font facename='Default' size=0x0.This is the address&comma. where the bug report should be sent
to.
:p.Currently the only valid address is
:hp2.odinusers@yahoogroups.com:ehp2.&comma. the address of the odinusers
mailing list at yahoogroups.
:note.You have to be a member of this mailing list in order to be
allowed to send bug reports to this address!
:p.Go to
:hp2.http&colon.&slr.&slr.groups.yahoo.com&slr.groups&slr.odinusers&slr.join
:ehp2.in order to subscribe (or use the URL object that OdinBug has
created in your Odin folder.)

:h2 res=630.Netscape Messenger 4.61

:p.:font facename='Default' size=0x0.If you click on this radio button&comma. you do not have to enter
anything more&comma. OdinBug will do all the rest for you.
:p.Please note that current Mozilla and IBM-Webbrowser releases do
not work with the Netscape 4.61 routine. The generic support has
to be used at this time for Mozilla.

:h2 res=640.PMMail (via PMMSend)

:p.:font facename='Default' size=0x0.If you use PMMail as your favorite email client&comma. you have
to check this radio button. OdinBug will take care of all the
rest.
:p.PMMSend.EXE will be used to process the email.
:note.You have to specify the email directory field and enter the name
of the directory for your account into the account field
(normally one of the directories
"<PMMail-Dir>&bsl.pmmail&bsl.*.ACT)&comma. otherwise it will not
work.
:note.If you updated OdinBug from a release prior to 0.5.5 and you
experience problems using PMMail&comma. please&comma. change your
email settings to Netscape Messenger 4.61&comma. save the
settings and then change back to PMMail and reenter all fields
and save again. Now your problem should be gone!
:i1.PMMail does not work

:h2 res=650.MR/2 ICE

:p.:font facename='Default' size=0x0.If you use MR&slr.2 ICE as your favorite email client&comma. you
have to check this radio button. OdinBug will take care of all
the rest.
:i1.MR&slr.2 ICE

:h2 res=660.PolarBar

:p.:font facename='Default' size=0x0.If you use PolarBar as your favorite email client&comma. you have
to check this radio button.

:note.You have to specify the PolarBar directory (i.e. the directory
where PolarBar.ZIP resides.

:note.In addition you have to specify your Outbox directory&comma. to
enable OdinBug to put the email into the correct directory.

:p.In order to make a running instance of PolarBar recognize the
newly created email&comma. you have to reindex the outbox.
:i1.PolarBar mailer

:h2 res=670.Other email client

:p.:font facename='Default' size=0x0.If you are using the generic email support&comma. you will have
to paste the bug report from the clipboard to the body of the
message. Eventually you will also have to specify the subject.
:p.You would like to see your favorite email client to be supported
by OdinBug? :link reftype=hd res=830.Click here!:elink.

:h2 res=680.EMail directory

:p.:font facename='Default' size=0x0.OdinBug changes into this directory before sending the report if
this field is not empty.
:p.This field is needed for PMMail support and might be useful for
generic support.

:h2 res=690.EMail account/Outbox directory

:p.:font facename='Default' size=0x0.P:hp2.MMail users&colon.
:p.:ehp2.Specify your email account directory name here (normally one of
the directories 
"<PMMail-Dir>&bsl.pmmail&bsl.*.ACT)".
:p.:hp2.PolarBar users&colon.
:p.:ehp2.Specify your outbox directory here (normally
"<PolarBar-Dir>&bsl.Mailer&bsl.Maildata&bsl.<account>&bsl.Outbox").

:h1 res=700 x=right y=bottom width=66% height=100%.Buttons

:p.:font facename='Default' size=0x0.The following pages describe the functions of the various buttons
found in OdinBug.

:h2 res=720.Create

:p.:font facename='Default' size=0x0.By pressing this button&comma. the actual report is created in
the :link reftype=hd res=840.standard form:elink.. All the information has to be gathered and
entered before doing this.
:p.The result is a file named OdinBug.RPT&comma. which has all
information in the :link reftype=hd res=840.appropriate format:elink..
:p.This file is ready to be sent to the :link reftype=hd res=620.appropriate email address:elink..

:h2 res=730.EMail

:p.:font facename='Default' size=0x0.Pressing the EMail button will copy the reportfile to the
clipboard&comma. open an instance of your favorite email program.

:p.The appropriate address will be entered automatically.

:p.You just have to specify the subject and paste the report into
the body of your email.

:p.You are ready to send!

:h2 res=740.Open

:p.:font facename='Default' size=0x0.Pressing this button opens the newly created OdinBug.RPT bug
report file in your favorite editor&comma. in order to modify it
manually before sending it to the appropriate email address.

:h2 res=750.Browse

:p.:font facename='Default' size=0x0.Pressing this button opens the program specified as logfile
browser on the settings page with the current odin32_?.log as
parameter in order to ease the manual investigation of the
logfile.

:h2 res=760.Help

:p.:font facename='Default' size=0x0.This button opens the file you are just reading.
:p.Amazing&comma. ain't it?

:h2 res=770.Exit

:p.:font facename='Default' size=0x0.Pressing this button will close OdinBug without any further
action.

:h2 res=780.Save settings

:p.:font facename='Default' size=0x0.Pressing this button saves all the options to OdinBug.INI&comma.
a standard OS&slr.2 (eCS) INI file which resides in the same
directory as OdinBug.EXE.

:h2 res=790.Recreate objects

:p.:font facename='Default' size=0x0.When OdinBug.EXE is called for the first time (or with the
&slr.InstallOnly commandline switch)&comma. it will
create&slr.update its objects and associations in the Odin
folder.
:p.Pressing this button will do exactly the same.

:h2 res=7100.Destroy objects

:p.:font facename='Default' size=0x0.Pressing this button will destroy all objects that are created by
OdinBug.
:note.OdinBug also creates the Odin folder&comma. if it is not found on
your system. However it does not destroy the Odin folder&comma.
as it might hold other objects&comma. that were not created by
OdinBug.

:h2 res=7110.About

:p.:font facename='Default' size=0x0.Like in every program the "About" button is the most important
function of all.
:p.It displays&comma. what you never would have expected&colon. All
those people&comma. that contributed their time and work to make
all this possible.
:p.It is not allowed to think nasty about one of these people!!!

:h1 res=800 x=right y=bottom width=66% height=100%.Miscanellous

:p.:font facename='Default' size=0x0.The following sections of the manual contain various topics
around Odin&comma. logging and reporting bugs.

:h2 res=820.Extended logging feature

:p.:font facename='Default' size=0x0.Disabling or enabling logging for all the Odin dlls isn't always
useful. To make logging more flexible&comma. you can now disable
or enable separate source files for each dll.

:note.Only implemented in kernel32&comma. user32 and gdi32 for now!

:p.Each dll that supports this feature has a file called
dbglocal.cpp in it's source directory. It contains a listing of
all the sources files for that dll (DbgFileNames) and an array
with boolean values for each of those files.

:p.To add this feature to a dll&comma. you must do the
following&colon.
:ul.
:li. Write a custom dbglocal.cpp and dbglocal.h.
:li. Every source file must include dbglocal.h with the correct debug
constant&colon.
:sl compact.
:li.:p.:font facename='Courier' size=14x8.#define DBG_LOCALLOG DBG_directory
:li.#include "dbglocal.h":font facename='Default' size=0x0.
:esl.
:li.Initterm.cpp must call ParseLogStatus when the dll is loaded
:eul.
:p.Each dprintf now first checks if logging is enabled for this
sourcefile before calling WriteLog.

:note.dbglocal must be included *after* misc.h

:p.When building a debug version of a dll&comma. logging is enabled
for all source files by default.

:hp7.:p.Examples of custom logging&colon.:ehp7.

:p.Disable logging for kernel32&comma. but enable it for profile.cpp
and wprocess.cpp
:p.:font facename='Courier' size=14x8.         set dbg_kernel32=-dll &plus.profile &plus.wprocess:font facename='Default' size=0x0.

:p.Enable logging for kernel32&comma. but disable it for profile.cpp
and wprocess.cpp
:p.:font facename='Courier' size=14x8.         set dbg_kernel32=&plus.dll -profile -wprocess:font facename='Default' size=0x0.
:i1.Extended logging feature

:h2 res=830.Email client support and further development

:p.:font facename='Default' size=0x0.In order to provide email support for a specific email
program&comma. I need to know how to send emails by command line
with this program.
:p.The rest is easy and only little work!
:p.-
:p.Not as easy as I had thought as PMMail support showed me&comma.
but I do my best...
:p.So feel free to contact :link reftype=fn refid=999.me:elink.!
:p.The same applies for bugs or missing features in OdinBug itself.
Basically OdinBug is now at a point&comma. where it does (almost)
everything I feel it should do.
:p.Anyway&comma. if somebody comes up with good ideas&comma. I will
(probably) go for them!
:p.So once again&comma. feel free to contact :link reftype=fn refid=999.me:elink.!
:i1.contact
:fn id=999.:p.mailto&colon. herwig.bauernfeind@aon.at:efn.

:h2 res=840.Standard form of a bug report

:lines align=left.:font facename='Courier' size=12x6.:font facename='Default' size=0x0.--------------------- Begin of Odin Bug Report ----------------------
  
Odin build information&colon.
  
  Current Odin build&colon.     (odin32bin-2001????-debug.wpi or .zip)
  Installation method&colon.    (WarpIN or manually)
  Launching method&colon.       (Win32K.SYS or PE.EXE)
  Last WarpIN install&colon.    (date&slr.time)
  Last OdinInst install&colon.  (date&slr.time)
  Odin "Windows" dir&colon.     (search for "Windows  dir" in logfile!)
  Odin "System32" dir&colon.    (search for "System32 dir" in logfile!)
  Evaluated Odin.INI&colon.     (located in System32 directory)
  
General system information&colon.
  
  OS&slr.2 (eCS) version&colon.     (Warp 4&comma. 4.5&comma. eCS&comma. Warp 3 not officially supported)
  OS&slr.2 reports version&colon.   (2.4&comma. 2.45) kernel revision&colon. (14.0xx)
  FixPack Level&colon.          (XR_M015 or simply 15)
  Is xf86sup.sys loaded?  (Yes&slr.No)
  Display driver&colon.         (Scitech&slr.Matrox&slr.gradd)
  Video Chipset&colon.          (Matrox&slr.Nvidia TNT&slr.Geforec&slr.ATI Rage Pro)
  
Win32 program information&colon.
  
  Odin logfile&colon.           (odin32_?.log found in the application's dir)
  Windows executable&colon.     (commandline of the program)
  Application name&colon.       (name)
  Application version&colon.    (version)
  Available from&colon.         (URL if available)
  
Verbal problem description&colon.

(what happened&comma. in your own words&comma. be as accurate as possible)

Exception information and stack dump&colon.

(search for "[Exception" in the logfile!)

ODIN.INI information&colon.

(located in x&colon.&bsl.Odin&bsl.System32 directory)
---------------------- End of Odin Bug Report -----------------------:elines.:font facename='Default' size=0x0.

:h2 res=850.Installation of OdinBug

:p.:font facename='Default' size=0x0.As OdinBug.EXE is written in VX-REXX it needs the VROBJ.DLL runtime.

:p.VROBJ.DLL can be found in the directory x&colon.&bsl.IMAGES (x=CD-ROM drive)
of  your
original Warp 4 CD-ROM and has to be copied into into d&colon.&bsl.OS2&bsl.DLL
(d=bootdrive)&comma. or somewhere else in your LIBPATH.

:p.There is also a VROBJ.WPI package at the following location&colon.
:sl compact.
:li.:link reftype=fn refid=723.:font facename='Helv' size=8x4.:hp2.http&colon.&slr.&slr.groups.yahoo.com&slr.group&slr.odinusers&slr.files&slr.vrobj.wpi:elink.
:li.:link reftype=fn refid=724.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.weekly&slr.vrobj.wpi:elink.:font facename='Default' size=0x0.
:esl.
:p.:ehp2.This package can be installed the usual way using WarpIN.

:p.OdinBug comes in 3 different packages (only one has to be used)&colon.
:ul.
:li.As part of the weekly build files. OdinBug is installed automatically with
Odin.
:li.As a standalone WPI package. This package can be installed the usual way using
WarpIN. 
:li.As a standalone ZIP package. This should be unZIPped into the Odin-"Windows"
directory. Putting it elsewhere is not recommended and cause unpredictable
results.
:eul.
:p.Whenever a new version of OdinBug is executed for the first time&comma. it 
creates
an object for itself (and a few others) in the Odin folder and  associates
odin32_?.log files with it. Now you can doubleclick on any  odin32_?.log file
and OdinBug is fired up. 


:note.XWorkplace users should create "Odin Log Files" as a subtype of  "Plain
Text" and associate OdinBug with that filetype in order to have  OdinBug as a
default for Odin logfiles and your favorites text editor  as additional
editor&slr.viewer.

:p.Before pressing "Create" for the first time you have to configure your  :link reftype=hd res=600.email
client:elink.on the ":link reftype=hd res=600.EMail:elink." page.

:note.You should not use OdinBug versions prior to 0.5.5!

:fn id=723.:p.http&colon.&slr.&slr.groups.yahoo.com&slr.group&slr.odinusers&slr.files&slr.vrobj.wpi:efn.
:fn id=724.:p.ftp&colon.&slr.&slr.ftp.os2.org&slr.odin&slr.weekly&slr.vrobj.wpi:efn.
:i1.OdinBug installation
:i1.Installation of OdinBug


:h2 res=860.Commandline options for OdinBug

:p.:font facename='Default' size=0x0.OdinBug accepts the following items on the commandline&colon.
:ul.
:li.:font facename='Courier' size=12x6.&slr.InstallOnly:font facename='Default' size=0x0. (not case sensitive)&colon.
:p.This will invoke OdinBug invisibly&comma. create&slr.update the WPS objects
and exit again without any further notification. If the create&slr.update
process fails an error message will be displayed. This option is designed to
be used during an installation process only.
:li.:font facename='Courier' size=12x6.odin32_?.log:font facename='Default' size=0x0.
:p.This is the normal usage. The passed logfile will be used to create a
bugreport.
:li.All other things passed to OdinBug will result in an "Unrecognized parameter"
error message.
:eul.



:h2 res=870.Contributors

:p.:font facename='Default' size=0x0.This compilation was done in November&slr.December 2001 by&colon.

:p.Herwig Bauernfeind (:link reftype=fn refid=999.herwig.bauernfeind@aon.at:elink.)

:p.It is partly based on the file ReportingBugs.TXT done by&colon.

:p.Sander van Leeuwen (sandervl@xs4all.nl)

:p.It is also partly based on the file Logging.TXT done by&colon.

:p.Patrick Haller (patrick.haller@innotek.de)

:p.The following persons contributed ideas&comma. concepts and&slr.or testing to OdinBug&colon.
:ul compact.
:li.Sander van Leeuwen
:li.Peter Weilbacher
:li.Sven Stroh aka Yellow
:li.Kenn Yuill
:eul.

:h2 res=880.Dedication

:p.:font facename='Default' size=0x0.This release is dedicated to the loving memory of my elder
brother Walter Bauernfeind&comma. who died from cancer on
06.01.2002.







:euserdoc.
