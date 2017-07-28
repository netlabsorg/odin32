/* $Id: readme.txt,v 1.2 2000-09-05 17:36:22 bird Exp $ */

This is a short readme for the daily builder scripts. These are all the
scripts doing the daily odin builds.


Directory structure
-------------------
The build scripts assumes that the scripts are found in ..\DailyBuild and
that the Odin32 CVS tree checkout is to be placed in ..\tree<date> (relative
to the start directory). The job.cmd is normally started from the
DailyBuild directory, and starts out changing the to ..\tree<date>.
Logs are placed in ..\logs.

    <basedir>\DailyBuild        - All these scripts.
    <basedir>\tree<date>        - The CVS tree (don't work on this, tree it's delete every night...).
    <basedir>\DailyBuild\logs   - Logs.
    <basedir>\DailyBuild\dbbackup   - Database backups.


Modfications / Configuration
----------------------------
The scripts are and should be configured for running at the (main)
daily build machine. Paths and such are spread out in several files but
mainly job.cmd, odin32env.cmd and odin32ftp*.cmd.
When modifications are done please notify the one in response of the build
machine so he (or she) could update the scripts which are used.

Currently this is: knut.stange.osmundsen@mynd.no


Y2K...
------
The scripts should be Y2K compilant. :-) They worked well on the 29th of
February. There are a some date computation on the scripts.


Requirements
------------
    - All the compilers (VAC 3.08, MSC 6.0a and GCC-EMX), Toolkit
      and DDK.
    - 4OS/2 - tee and del (as far as I remember)
    - new ftp (odin32ftp2.cmd) requires:
       1) Working RexxFtp
       2) ncftpput (from ncftp v3.0 beta)
          AND/OR
          ssh and scp (sshos203.zip at hobbes) to use secure ftp account.
    - old ftp (odin32ftp.cmd) requires:
       1) Working RexxFtp
    - scheduler program which will invoke the script at a specific time.
      (I have a very simple one if needed.)

Tue 05.09.2000
  knut
