/*:VRX         Main
*/
/*  Main
*/
ok = VRRedirectStdIO('OFF')
Main:
/*  Process the arguments.
    Get the parent window.
*/
    parse source . calledAs .
    parent = ""
    argCount = arg()
    argOff = 0
    if( calledAs \= "COMMAND" )then do
        if argCount >= 1 then do
            parent = arg(1)
            argCount = argCount - 1
            argOff = 1
        end
    end; else do
        call VROptions 'ImplicitNames'
        call VROptions 'NoEchoQuit'
    end
    InitArgs.0 = argCount
    if( argCount > 0 )then do i = 1 to argCount
        InitArgs.i = arg( i + argOff )
    end
    drop calledAs argCount argOff

/*  Load the windows
*/
    call VRInit
    parse source . . spec
    _VREPrimaryWindowPath = ,
        VRParseFileName( spec, "dpn" ) || ".VRW"
    _VREPrimaryWindow = ,
        VRLoad( parent, _VREPrimaryWindowPath )
    drop parent spec
    if( _VREPrimaryWindow == "" )then do
        call VRMessage "", "Cannot load window:" VRError(), ,
            "Error!"
        _VREReturnValue = 32000
        signal _VRELeaveMain
    end

/*  Process events
*/
    call Init
    signal on halt
    do while( \ VRGet( _VREPrimaryWindow, "Shutdown" ) )
        _VREEvent = VREvent()
        interpret _VREEvent
    end
_VREHalt:
    _VREReturnValue = Fini()
    call VRDestroy _VREPrimaryWindow
_VRELeaveMain:
    call VRFini
exit _VREReturnValue

VRLoadSecondary:
    __vrlsWait = abbrev( 'WAIT', translate(arg(2)), 1 )
    if __vrlsWait then do
        call VRFlush
    end
    __vrlsHWnd = VRLoad( VRWindow(), VRWindowPath(), arg(1) )
    if __vrlsHWnd = '' then signal __vrlsDone
    if __vrlsWait \= 1 then signal __vrlsDone
    call VRSet __vrlsHWnd, 'WindowMode', 'Modal' 
    __vrlsTmp = __vrlsWindows.0
    if( DataType(__vrlsTmp) \= 'NUM' ) then do
        __vrlsTmp = 1
    end
    else do
        __vrlsTmp = __vrlsTmp + 1
    end
    __vrlsWindows.__vrlsTmp = VRWindow( __vrlsHWnd )
    __vrlsWindows.0 = __vrlsTmp
    do while( VRIsValidObject( VRWindow() ) = 1 )
        __vrlsEvent = VREvent()
        interpret __vrlsEvent
    end
    __vrlsTmp = __vrlsWindows.0
    __vrlsWindows.0 = __vrlsTmp - 1
    call VRWindow __vrlsWindows.__vrlsTmp 
    __vrlsHWnd = ''
__vrlsDone:
return __vrlsHWnd

/*:VRX         _AnalyseConfigSys
*/
_AnalyseConfigSys: 
    /* From CONFIG.SYS we get the information, if
       XF86SUP.SYS is installed
       WIN32K.SYS is installed (currently)
       which display driver is installed
     */
    FoundWin32KSysNow = 0
    ConfigSys = SysBootDrive()'\CONFIG.SYS'
    do while lines(ConfigSys) > 0 
        ConfigLine = translate(strip(Linein(ConfigSys)))
        if left(ConfigLine,3) <> 'REM' then do
            select
                when Pos('XF86SUP.SYS', ConfigLine) > 0 then do
                    Foundxf86supsys = 1
                    xf86supsysLine = substr(ConfigLine,pos(':',ConfigLine)-1)
                    xf86supsysLine = left(xf86supsysLine, pos('SYS', xf86supsysLine) + 3)
                end
                when Pos('WIN32K.SYS', ConfigLine) > 0 then do
                    Win32Kparm = substr(ConfigLine,pos("WIN32K.SYS", ConfigLine) + 11)
                    FoundWin32KSysNow = 1
                end
                when Pos('SDDGRADD', ConfigLine) > 0 then  do
                    Driver = "Scitech Display Doctor"
                    FoundScitech = 1
                end
                when Pos('SDDHELP.SYS',ConfigLine) > 0 then  SDDHelpSys=strip(substr(ConfigLine,8))

                when Pos('MGAX64', ConfigLine) > 0 then      Driver = "Matrox Driver"
                when Pos('GENGRADD', ConfigLine) > 0 then    Driver = "Generic GRADD"
                when Pos('ATIGRADD', ConfigLine) > 0 then    Driver = "ATI GRADD"
                when Pos('CHPGRADD', ConfigLine) > 0 then    Driver = "CHP GRADD"
                when Pos('DYNGRADD', ConfigLine) > 0 then    Driver = "DYN GRADD"
                when Pos('G_VGRADD', ConfigLine) > 0 then    Driver = "G_V GRADD"
                when Pos('MGAGRADD', ConfigLine) > 0 then    Driver = "Matrox GRADD"
                when Pos('NMGRADD', ConfigLine) > 0 then     Driver = "NeoMagic GRADD"
                when Pos('S3GRADD', ConfigLine) > 0 then     Driver = "S3 GRADD"
                when Pos('TRIGRADD', ConfigLine) > 0 then    Driver = "Trident GRADD"
                when Pos('VIRGEGRADD', ConfigLine) > 0 then  Driver = "S3 ViRGE GRADD"
                when Pos('VGAGRADD', ConfigLine) > 0 then    Driver = "VGA GRADD"
                /* add other display drivers here */
                otherwise nop
            end
        end
    end 
    call lineout(ConfigSys)
return

/*:VRX         _AnalyseKernel32DLL
*/
_AnalyseKernel32DLL: 
    /*  we get build level information
        from Kernel32.DLL by spawning
        BLDLEVEL.EXE, redirecting the 
        output and analysing it */
        
    /* return if we do not have a valid directory */
    if OdinSysDir = '' then return
    
    /* get Kernel32 version */
    BldInfo     = 'BldInfo.TXT'
    
    'BLDLevel 'OdinSysDir'Kernel32.DLL >'BldInfo
    do until lines(BldInfo) = 0
        BldLine = translate(linein(BldInfo))
        if pos('FILE VERSION',BldLine) > 0 then do
            Kernel32Ver = word(BldLine,3)
        end
        if pos('DATE/TIME',BldLine) > 0 then do
            Kernel32Date = word(BldLine,4)||right('0'||_xmonth(word(BldLine,3)),2)||right('0'||word(BldLine,2),2)
        end
    end
    call lineout BldInfo
    ok = SysFileDelete(BldInfo)
return

/*:VRX         _AnalyseOdin32_xLog
*/
_AnalyseOdin32_xLog: 
    /* We get the information about
       WIN32K.SYS installed (when bug occured)
       Odin directory
       Odin\System directory
       Commandline of Win32 program
       location of Odin.INI
       we have a visible progressbar,
       this analysis is usually done after 
       reading 50 lines or so of the logfile
     */
    If LogFileName = '' then return

    /* clear variables */
    OdinWinDir = ''
    OdinSysDir = ''
    ExeFile    = ''
    FoundWinDir = 0
    FoundSysDir = 0
    FoundExe = 0
    FoundOdinIni = 0

    ExceptFilename = filespec('D',LogFileName)||filespec('P',LogFileName)||'Except.$$$'
    LogFileSize = stream(LogfileName,'c','QUERY SIZE')
    ApproxLines = LogFileSize % 41
    call _InitProgress
    LogLine = ''
    do J = 1 to 5 until FoundWin32KSysAtBugTime = 1
        LogLine = linein(LogFileName)
        if pos('WIN32K - INSTALLED',translate(LogLine)) > 0 then do
            FoundWin32KSysAtBugTime = 1
            Win32KLogLine = LogLine
        end            
    end
    do until lines(LogFileName) = 0
        LogLine = linein(LogFileName)
        select
            when pos('Windows  dir:',LogLine) > 0 then do
                OdinWinDir = strip(Substr(LogLine,pos('Windows  dir:',LogLine)+14))'\'
                FoundWinDir = 1
            end
            when pos('System32 dir:',LogLine) > 0 then do
                OdinSysDir = strip(Substr(LogLine,pos('System32 dir:',LogLine)+14))'\'
                FoundSysDir = 1
            end
            when pos('Cmd line:',LogLine) > 0 then do
                EXEFile = strip(Substr(LogLine,pos('Cmd line:',LogLine)+10))
                FoundExe = 1
            end
            when pos('GetCommandLineA:',LogLine) > 0 then do
                ExeFile = strip(Substr(LogLine,pos('GetCommandLineA:',LogLine)+16))
                FoundExe = 1
            end
            when pos('Odin ini loaded:',LogLine) > 0 then do
                OdinINI = strip(Substr(LogLine,pos('Odin ini loaded:',LogLine)+16))
                FoundOdinIni = 1
            end
            otherwise do
                I = I + 1
                if I // 1000 = 0 then do
                    call _CheckForEvent
                    Progress = I * 100 % ApproxLines
                    ok = VRSet("SL_Progress","Percentile",Progress)
                end
            end
            if FoundWinDir & FoundSysDir & FoundExe & FoundOdinIni then leave
        end
    end
    ok = VRSet("Progress","Visible", 0)
    call lineout(LogFileName)
return

/*:VRX         _AnalyseOdinIni
*/
_AnalyseOdinIni: 
    /* we get the INSTALLDATE information from Odin.INI */
    
    /* old method to locate Odin.INI - obsolete,
       now we get Odin.Ini from the Logfile
    Odin_INI = value('ODIN_INI',,'OS2ENVIRONMENT') */

    do until lines(OdinINI) = 0
        OdinIniLine = translate(linein(OdinINI))
        if pos('INSTALLDATE',OdinIniLine) > 0 then do
            OdinInstLine  = strip(substr(OdinIniLine,pos('=',OdinIniLine)+1))
            OdinInstDoW   = word(OdinInstLine,1)
            OdinInstMonth = _xmonth(word(OdinInstLine,2))
            OdinInstDay   = right('0'||word(OdinInstLine,3),2)
            OdinInstTime  = word(OdinInstLine,4)
            OdinInstYear  = word(OdinInstLine,5)
            OdinInstDate  = OdinInstDay'.'OdinInstMonth'.'OdinInstYear
            OdinInstDate2 = OdinInstYear||right('0'||OdinInstMonth,2)||right('0'||odinInstday,2)
            leave
        end 
    end
    call lineout(OdinIni)
    LastOdinInst=OdinInstDate' at 'OdinInstTime', current version 'OdinInstVer
    ok = VRSet("EF_LastOdinInst","Value",LastOdinInst)
    if OdinInstDate2 < builddate & builddate <> '' then do
        Msg.Text = "You did not run OdinInst.EXE after your last installation. Run OdinInst.EXE, and rerun your program before submitting an Odin Bug Report!"
        Msg.Type = "Error"
        call _ShowMsg
        ok = VRSet("PB_CreateReport","Visible", 0)
    end
return

/*:VRX         _AnalyseOdininst
*/
_AnalyseOdininst: 
    /*  we get build level information
        from OdinInst.EXE by spawning
        BLDLEVEL.EXE, redirecting the 
        output and analysing it */

    /* get OdinInst Version */
    BldInfo     = 'BldInfo.TXT'
    
    'BLDLevel 'OdinSysDir'OdinInst.exe >'BldInfo
    do until lines(BldInfo) = 0
        BldLine = translate(linein(BldInfo))
        if pos('FILE VERSION',BldLine) > 0 then do
            OdinInstVer = word(BldLine,3)
            leave
        end
    end
    call lineout BldInfo
    ok = SysFileDelete(BldInfo)
return

/*:VRX         _AnalyseScitechDD
*/
_AnalyseScitechDD: 
    /* we know that we have have a Scitech Driver
       now lets determine which one. We analyse
       SDDHELP.SYS and GRAPHICS.LOG */
       
    /* we get SDDHelpSys from Config.Sys now
       SDDHelpSys = SysBootDrive()'\OS2\SDDHELP.SYS' */

    /* our new PREXX30 compliant SNAP detector */
    isSNAP = 0
    do until chars(SDDHelpSYS) = 0
        C = charin(SDDHELPSYS)
        select 
            when C = 'b' then do
                SddBuildNr = C||charin(SDDHELPSYS,,8)
                if translate(word(SDDBuildNr,1)) <> 'BUILD' then iterate
                    else leave
            end
            when C = 'S' then do
                SciTechType = C||charin(SDDHELPSYS,,3)
                if translate(SciTechType) <> 'SNAP' then iterate
                else isSNAP = 1
            end
            when C = 'D' then do
                SciTechType = C||charin(SDDHELPSYS,,5)
                if translate(ScitechType) <> 'DOCTOR' then iterate
                    else isSNAP = 0
            end
            otherwise iterate
        end
    end
    
    ok = stream(SDDHelpsys,'c','close')

    NUCLEUS_PATH = value('NUCLEUS_PATH',,'OS2ENVIRONMENT')
    if Nucleus_Path = '' then Nucleus_Path = SysBootDrive()'\OS2\DRIVERS\NUCLEUS\CONFIG'
    GraphicsLog = Nucleus_Path'\GRAPHICS.LOG'

    License = linein(GraphicsLog)
    select 
        when (word(License,1)= 'Valid') & (word(License,2) = 'IHV') & (\IsSNAP) then Driver = 'Scitech Display Doctor/SE'
        when (word(License,1)= 'Valid') & (word(License,2) = 'IHV') & (IsSNAP)  then Driver = 'Scitech SNAP/OEM'
        when (word(License,1)= 'Registered') & (word(License,2) = 'end') & (\IsSNAP) then Driver = 'Scitech Display Doctor/Pro'
        when (word(License,1)= 'Registered') & (word(License,2) = 'end') & (IsSNAP)  then Driver = 'Scitech SNAP'
        otherwise if IsSNAP then Driver = 'Scitech SNAP/Demo'
            else Driver = 'Scitech Display Doctor/Demo'
    end

    LogLine = ''
    do until pos('Chipset...',LogLine) > 0 | lines(GraphicsLog) = 0
        LogLine = linein(GraphicsLog)
    end
    ChipSet = ''
    if lines(graphicslog) = 0 then ChipSet = "Detection failed!"
    else do I = 2 to words(logline)
        ChipSet = Chipset||word(Logline,I)||' '
    end
    ok = stream(GraphicsLog,'c','CLOSE')
return

/* old SDD 

    LogLine = ''
    IsSNAP  = 0
    LC = 0
    do until lines(SDDHelpSys) = 0
        LogLine = translate(linein(SDDHelpSys),' ', d2c(0))
        LC = LC + 1
        if pos('build',logline) > 0 then do
            SddBuildNr = substr(LogLine,pos('build',logline),10)
            IsSNAP = sign(Pos('SNAP',LogLine))
            leave
        end
    end
    ok = stream(SDDHelpSys,'c','CLOSE')
    do until chars(SDDHELPSYS)=0
 */
/*:VRX         _AnalyseSyslevelFPK
*/
_AnalyseSyslevelFPK: 
    sys_os2 = SysSearchPath('path', 'syslevel.FPK')
    if sys_os2 = '' then do
        fpk_level = '(none)'
        return 
    end
    sys_info = translate(Charin(sys_os2, 1, 150),' ','00'x)
    Parse Var sys_info . 45 fpk_level 141 os_comp 150
    fpk_level = left(fpk_level,7)
    ok = stream(sys_os2,'c','CLOSE')
return

/*:VRX         _AnalyseSysLevelOS2
*/
_AnalyseSysLevelOS2: 
    sys_os2 = SysSearchPath('path', 'syslevel.os2')
    sys_info = Charin(sys_os2, 1, 150)
    Parse Var sys_info . 53 os_type 61 os_name 141 os_comp 150
    os_name = Strip(os_name, 't', '00'x)
    os_type = Strip(os_type, 't', '_')
    parse var os_type . 4 major 6 minor
    os_ver  = SysOS2Ver()
    if os_ver = 2.30 then OS2Type = "OS/2 Warp 3.0x (not supported)"
    select 
        when os_comp = '5639A6100' then do
            if os_ver = 2.40 then OS2Type = "OS/2 Warp 4.00"
            if os_ver = 2.45 then OS2Type = "OS/2 Warp 4.50"
        end
        when os_comp = '5639A6101' & minor = '01' then OS2Type = "OS/2 Warp 4.51 (MCP1)"
        when os_comp = '5639A6101' & minor = '02' then OS2Type = "OS/2 Warp 4.52 (MCP2)"
        when os_comp = '5639A6150' & minor = '00' then OS2Type = "OS/2 WSeB 4.50 uni"
        when os_comp = '5639A6150' & minor = '01' then OS2Type = "OS/2 WSeB 4.51 uni (ACP1)"
        when os_comp = '5639A6150' & minor = '02' then OS2Type = "OS/2 WSeB 4.52 uni (ACP2)"
        when os_comp = '5639A5550' & minor = '00' then OS2Type = "OS/2 WSeB 4.50 smp"
        when os_comp = '5639A5550' & minor = '01' then OS2Type = "OS/2 WSeB 4.51 smp (ACP1)"
        when os_comp = '5639A5550' & minor = '02' then OS2Type = "OS/2 WSeB 4.52 smp (ACP2)"
        otherwise do
        end
    end
    ok = stream(sys_os2,'c','CLOSE')

    /* detect eComStation - a very simplicistic approach */    
    ecs = 1
    ok = SysFileTree(SysBootDrive()'\WiseMachine.fit',ecs.,'FO')
    ecs = ecs * ecs.0
    ok = SysFileTree(SysBootDrive()'\eCS',ecs.,'DO')
    ecs = ecs * ecs.0
    ok = SysFileTree(SysBootDrive()'\TVoice',ecs.,'DO')
    ecs = ecs * ecs.0
    if ecs then OS2Type = 'eComStation (Base: 'substr(OS2Type,10)')'
    
    /* get Kernel Version */
    BldInfo     = 'BldInfo.TXT'
    'BLDLevel 'SysBootDrive()'\os2krnl >'BldInfo
    do until lines(BldInfo) = 0
        BldLine = linein(BldInfo)
        if pos('Signature',BldLine) > 0 then do
            KrnlBuildVer = translate(word(BldLine,2),'   ','#@:')
            KrnlBuildVer = word(KrnlBuildVer,2)||word(KrnlBuildVer,3)
            leave
        end
    end
    call lineout BldInfo
    ok = SysFileDelete(BldInfo)
return

/*:VRX         _AnalyseWarpINdatabase
*/
_AnalyseWarpINdatabase: 
    FoundWarpIN = 1

    WarpINDir = strip(VRGetIni( "WarpIN", "Path", "User" ),,d2c(0))
    if WarpINDir = '' then do
        Msg.Text = "WarpIN directory not found!"
        Msg.Type = "Warning"
        call _ShowMsg
        FoundWarpIN = 0
        return
    end   
    BDL = left(SysBootDrive(),1)
    ok = SysFileTree(WarpInDir||'\DATBAS_'BDL'.INI',WarpINDB., 'FO')
    select
        when WarpINDB.0 = 0 then do
            Msg.Text = "No WarpIN database file found!"
            Msg.Type = "Warning"
            call _ShowMsg
            FoundWarpIN = 0
        end
        when WarpINDB.0 = 1 then do
            WarpINDB = WarpINDB.1
        end
/*      when WarpINDB.0 > 1 then do
            Msg.Text = "You appear to have more than one WarpIN database! Choosing biggest!"
            Msg.Type = "Warning"
            call _ShowMsg
            MaxSize = 0
            do I = 1 to WarpINDB.0
                CurrSize = query(WarpinDB.I,'c','query size')
                if  CurrSize >= MaxSize then do
                    WarpINDB = WarpINDB.I
                    MaxSize = CurrSize
                end
            end
        end   */
        otherwise do
            msg.Text = 'Something weird has happened! You should not see this message!'
            msg.Type = "Error"
            call _ShowMsg
            FoundWarpIN = 0
        end
    end

/*  WarpINDB = WarpInDir||'\DATBAS_E.INI' /* '\DATBAS_D.INI' */ */

    if FoundWarpIN then do
        ok = SysIni(WarpInDB,'All:',"Apps")
        if apps.0 = 0 then do
            Msg.Text = "WarpIN database "WarpINDB" reported no applications!"
            Msg.Type = "Warning"
            call _ShowMsg
            FoundWarpIN = 0
            return
        end

        ok = VRset("EF_LastWarpIN","HintText","Last time Odin was installed using WarpIN due to "WarpINDB)
        OdinCoreDateTime = ''
        OdinSYsDateTime = ''
        do I = 1 to apps.0
          select
            when pos('Odin System',apps.I) > 0  then do
                OdinSysDateTime = c2x(strip(VRGetIni( apps.I, "InstallDateTime", WarpINDB ),,d2c(0)))
                OdinSysDir = strip(VRGetIni( apps.I, "TargetPath", WarpINDB ),,d2c(0))||'\'
                OdinSysVer = apps.I
                OdinINI = OdinSysDir||'Odin.INI'
            end
            when pos('Odin Core',apps.I) > 0  then do
                OdinCoreDateTime = c2x(strip(VRGetIni( apps.I, "InstallDateTime", WarpINDB ),,d2c(0)))
                OdinWinDir = strip(VRGetIni( apps.I, "TargetPath", WarpINDB ),,d2c(0))||'\'
                OdinCoreVer = apps.I
            end
            otherwise nop
          end
        end
        if OdinCoreDateTime = '' | OdinSysDateTime = ''  then do
            Msg.Text = 'Could not find last WarpIN install info!'
            Msg.Type = "Warning"
            call _ShowMsg
            LastWarpIN = 'Maybe never!'
        end
        else do
            OdinCoretime = _xtime(substr(OdinCoreDateTime,1,6))
            OdinSysTime  = _xtime(substr(OdinSYsDateTime,1,6))
            OdinCoreDate = _xdate(substr(OdinCoreDateTime,9,8))
            OdinSysDate  = _xdate(substr(OdinSYsDateTime,9,8))
            OdinCoreVer  = translate(substr(OdinCoreVer,pos('Core Files',OdinCoreVer)+11),'.','\')
            OdinSysVer   = translate(substr(OdinSysVer,pos('System Files',OdinSysVer)+13),'.','\')
            LastWarpIN = OdinSysDate' at 'OdinSysTime', it was  version 'OdinSysVer
        end
    end
return

/*:VRX         _AnalyseXF86SupSys
*/
_AnalyseXF86SupSys: 
    /*  we get build level information
        from XF86Sup.SYS by spawning
        BLDLEVEL.EXE, redirecting the 
        output and analysing it */

    /* get XF86Sup.Sys Version */
    BldInfo     = 'BldInfo.TXT'
    
    'BLDLevel 'xf86supsysLine' >'BldInfo
    do until lines(BldInfo) = 0
        BldLine = translate(linein(BldInfo))
        if pos('FILE VERSION',BldLine) > 0 then do
            XF86supVersion = word(BldLine,3)
            leave
        end
    end
    call lineout BldInfo
    ok = SysFileDelete(BldInfo)
return

/*:VRX         _CheckForEvent
*/
_CheckForEvent: 
    /* check and process event queue */
    EventString = ''
    do until EventString = 'nop'
        EventString = VREvent('N')
        if EventString <> 'nop' then do
            interpret EventString
        end
    end
return

/*:VRX         _CheckForExpiredBuild
*/
_CheckForExpiredBuild: 
    if UsingPrivateBuilds then return 1
    if BuildFile = '' then return 0
    AllowReport = 0
    buildYear  = substr(BuildFile,11,4)
    buildmonth = substr(BuildFile,15,2)
    buildday   = substr(BuildFile,17,2)
    builddate  = substr(BuildFile,11,8)

    todayYear  = left(date('S'),4)
    todayMonth = substr(date('S'),5,2)
    todayDay   = right(date('S'),2)

    if datatype(buildyear) <> 'NUM' |datatype(buildmonth) <> 'NUM' |datatype(buildday) <> 'NUM' then do
        Msg.Text = "Invalid buildfilename! - Please enter a valid buildfilename!"
        Msg.Type = "Warning"
        call _ShowMsg
        return 0
    end
    datenumber  = BuildYear*365+BuildMonth*30+BuildDay
    todaynumber = TodayYear*365+TodayMonth*30+TodayDay
    select 
        when todaynumber - datenumber >  TooOld then do
            Msg.Text = "You have to try a newer build before submitting this bug report! Your bug might have been fixed already !"
            Msg.Type = "Error"
            call _ShowMsg
            if OverRideTooOld then do
                AllowReport = 1
                AgeOfBuild  = todaynumber - datenumber
                TooOldMsg   = 1
            end
        end
        when todaynumber - datenumber > WarnOld then do
            Msg.Text = "Before submitting this bug report, please try a newer build! Your bug might have been fixed already!"
            Msg.Type = "Warning"
            call _ShowMsg
            AllowReport = 1
        end
        otherwise AllowReport = 1
    end
return AllowReport

/*:VRX         _CheckInvalidReport
*/
_CheckInvalidReport: 
    Invalidreport = 0
    InvalidReport = InvalidReport + (VRGet("EF_Build","value") = "")
    InvalidReport = InvalidReport + (VRGet("DDCB_InstallType","value") = "")
    InvalidReport = InvalidReport + (VRGet("DDCB_LaunchType","value") = "")
    InvalidReport = InvalidReport + (VRGet("DDCB_OS2Version","value") = "")
    InvalidReport = InvalidReport + (VRGet("DDCB_FPK","value") = "")
    InvalidReport = InvalidReport + (VRGet("DDCB_DD","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_LogFile","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_EXEFile","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_Appname","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_OdinWinDir","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_OdinWinSysDir","value") = "")
    InvalidReport = InvalidReport + (VRGet("MLE_OwnWords","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_LastWarpIN","value") = "")
    InvalidReport = InvalidReport + (VRGet("EF_LastOdinInst","value") = "")
    if \ExceptionSearchDone then do
        ok = SysFileTree(ExceptFileName, ExceptDone., 'FO')
        if ExceptDone.0 = 0 then InvalidReport = 1
        else do
            ExceptionSearchDone = 1
            call _FillPageException
            ok = VRSet("EF_LogFile","Enabled", 1)
            ok = VRSet("PB_SearchLogFile","Enabled", 1)
        end
    end
    if AllowBugReport = 0 & OverRideTooOld = 0 then InvalidReport = 1
return sign(InvalidReport)

/*:VRX         _CommandLineParse
*/
_CommandLineParse: 
    if TestAppDir <> CurDirectory then do
        ok = SysFileTree(TestAppDir'Odin32_?.LOG',TestAppDirLogFile.,'FO')
        ok = SysFileTree(CurDirectory'Odin32_?.LOG',CurDirectoryLogFile.,'FO')
        if CurDirectoryLogFile.0 > 0 & TestAppDirLogFile.0 > 0 then do
            Msg.Type = "W"
            Msg.Text = "Logfiles found in "CurDirectory" and "TestAppDir". Be sure to choose the right one!"
            call _ShowMsg
        end
    end
    if translate(VRGet("Application","Commandline")) = '/INSTALLONLY' then do
        call _InstallWPSObjects
        call Quit
    end
    else do
        if VRGet("Application","Commandline") <> '' then do
            ok = SysFileTree(strip(VRGet("Application","Commandline"),'B','"'),Odin32Log., 'FO')
            if Odin32Log.0 = 1 then do
                LogFilename = Odin32Log.1
                TestAppDir  = strip(VRParseFileName( Odin32Log.1, "DP" ),'T','\') || '\'
            end
            else do
                Msg.Text = 'Unrecognized parameter ("'||VRGet("Application","Commandline")||'") - ignored!'
                Msg.Type = "Warning"
                call _ShowMsg
            end
        end
    end
return

/*:VRX         _CreateExceptionAndStackDump
*/
_CreateExceptionAndStackDump: 
    call lineout RptFileName, 'Exception information and stack dump:'
    call lineout RptFileName, '  '
    do until lines(ExceptFileName) = 0
        call lineout RptFileName, linein(ExceptFileName)
    end
    ok = lineout(ExceptFileName)
return

/*:VRX         _CreateObject
*/
_CreateObject: procedure
    Parse Arg Class, Title, Location, Setup, Collision
    /* Say 'Creating ['Title']' */
    rc = SysCreateObject( Class, Title, Location, Setup, Collision )
    If rc <> 1 Then do
        Msg.Text = ' > failed to create ['Title' | 'Class'] at location ['Location']'
        Msg.Type = 'Error'
        call _ShowMsg
    end
return rc

/*:VRX         _CreateReport
*/
_CreateReport: 
    RPTFileName = VRFileDialog( VRWindow(), "Save Odin Bug report to...", "Save", TestAppDir || "ODINBUG.RPT", , ,  )
    if RPTFilename = '' then return 0 /* not created */
    ok = SysFileTree(RPTFileName,Exists.,'FO')
    if Exists.0 = 1 then do
        Buttons.1 = "~Overwrite"
        Buttons.2 = "~Append"
        Buttons.0 = 2
        id = VRMessage( VRWindow(), RPTFileName||" exists!", "Odin Bug Report Generator", "Query", "Buttons.", buttons.1, buttons.2 )
        if id = 1 then do
            rc = SysFileDelete(RptFileName)
            if rc > 3 then do
                Msg.Type = "E"
                Msg.Text = "Could not open "RPTFileName" for writing! Aborting..."
                call _showMsg
                return 0 /* not created */
            end
        end
    end

    if VRGet("RB_OtherEmailClient","set") then call lineout RptFileName, '<sendto: 'EMail'>'
    call lineout RptFileName, '--------------------- Begin of Odin Bug Report ----------------------'
    if TooOldMsg then call lineout RptFileName, '                  for an expired build ('AgeOfBuild'days old)'
    call lineout RptFileName, '  '
    call lineout RptFileName, 'Odin build information:'
    call lineout RptFileName, '  '
    call lineout RptFileName, '  Current Odin build:     'VRGet("EF_Build","value")',  version 'Kernel32Ver
    call lineout RptFileName, '  Installation method:    'VRGet("DDCB_InstallType","value")
    call charout RptFileName, '  Launching method:       'VRGet("DDCB_LaunchType","value")
    if FoundWin32KSysNow & FoundWin32KSysAtBugTime then do
        call lineout RptFileName, ', Win32K-parameters: 'VRGet("EF_Win32Kparm","Value")
    end
    else call lineout RptFileName, '  '
    call lineout RptFileName, '  Last WarpIN install:    'VRGet("EF_LastWarpIN","Value")
    call lineout RptFileName, '  Last OdinInst install:  'VRGet("EF_LastOdinInst","Value")
    call lineout RptFileName, '  Odin "Windows" dir:     'VRget("EF_OdinWinDir","Value")
    call lineout RptFileName, '  Odin "System32" dir:    'VRget("EF_OdinWinSysDir","Value")
    call lineout RptFileName, '  Evaluated Odin.INI:     'VRGet("EF_OdinINI","Value")
    call lineout RptFileName, '  '
    call lineout RptFileName, 'General system information:'
    call lineout RptFileName, '  '
    call lineout RptFileName, '  OS/2 version:           'VRGet("DDCB_OS2Version","value")
    call lineout RptFileName, '  OS/2 reports version:   'VRGet("EF_Version","Value")', kernel revision: 'VRget("EF_KrnlBuildVer","Value")
    call lineout RptFileName, '  FixPack Level:          'VRGet("DDCB_FPK","value")
    call charout RptFileName, '  Is xf86sup.sys loaded?  '
    if Foundxf86supsys then call lineout RptFileName, 'Yes, version 'XF86SupVersion
                       else call lineout RptFileName, 'No'
    call lineout RptFileName, '  Display driver:         'VRGet("DDCB_DD","value")' 'VRGet("DDCB_DDVer","Value")' ('LC')'
    call lineout RptFileName, '  Video Chipset:          'VRGet("DDCB_chipset","Value")
    call lineout RptFileName, '  '
    call lineout RptFileName, 'Win32 program information:'
    call lineout RptFileName, '  '
    call lineout RptFileName, '  Odin logfile:           'VRget("EF_LogFile","Value")
    call lineout RptFileName, '  Windows executable:     'VRget("EF_EXEFile","Value")
    call lineout RptFileName, '  Application name:       'VRget("EF_AppName","Value")
    call lineout RptFileName, '  Application version:    'VRget("EF_AppVersion","Value")
    call lineout RptFileName, '  Available from:         'VRget("DDCB_URL","Value")
    call lineout RptFileName, '  '
    call lineout RptFileName, 'Verbal problem description:'
    call lineout RptFileName, '  '
    call lineout RptFileName, VRGet("MLE_OwnWords","Value")
    if OhNooo then do
        call lineout RptFileName, 'An "Oh, nooo!"-message box was displayed. '
    end
    call lineout RptFileName, '  '
    if AddException then call _CreateExceptionAndStackDump
                    else call lineout RptFileName, 'No exception information and stack dump added by user request!'
    call lineout RptFileName, '  '
    call lineout RptFileName, 'ODIN.INI information:'
    call lineout RptFileName, '  '
    do until lines(OdinIni) = 0
        call lineout RptFileName, linein(OdinIni)
    end
    call lineout OdinInI
    call lineout RptFileName, '---------------------- End of Odin Bug Report -----------------------'
    call lineout RptFileName, 'Created by OdinBug version 'word(VRGet("OdinBugReport","HintText"),2)' on 'date()' at 'time()
    call lineout RptFileName
return 1 /* success */

/*:VRX         _EMailGeneric
*/
_EMailGeneric: 
    if MessengerDir <> '' then do
        olddir = directory()
        call directory strip(MessengerDir,'T','\')
    end
    'start 'messenger' "mailto:'EMail'?subject=Odin Bug Report: 'VRget("EF_AppName","Value")' 'VRget("EF_AppVersion","Value")'"'  /* &body= */
    if MessengerDir <> '' then ok = directory(oldDir)
    CompleteReport = ''
    do until lines(RPTFileName) = 0
        CompleteReport = CompleteReport||CRLF||linein(RPTFileName)
    end
    call lineout RPTFileName
    ok = VRMethod("Application","PutClipBoard",CompleteReport)
    Msg.Text = "Report was copied to the clipboard! You may paste it into the body of your email now!"
    Msg.Type = "Information"
    call _ShowMsg
return

/*:VRX         _EMailMR2ICE
*/
_EMailMR2ICE: 
    TempFile = strip(value('TEMP',,'OS2ENVIRONMENT'),'T','\')||'\OdinBug.$$$'
    ok = SysFileDelete(TempFile)
    ok = lineout(TempFile, 'To: 'EMail)
    ok = lineout(TempFile, 'Subject: Odin Bug Report: 'VRget("EF_AppName","Value")' 'VRget("EF_AppVersion","Value"))
    ok = lineout(TempFile, 'Content-Type: text/plain; charset=us-ascii')
    ok = lineout(TempFile, 'Content-Transfer-Encoding: 7bit')
    ok = charout(TempFile, CRLF)
    do until lines(RPTFileName) = 0
        ok = lineout(TempFile, linein(RPTFileName))
    end
    ok = Lineout(TempFile)
    ok = lineout(RPTFileName)
    if MessengerDir <> '' then do
        olddir = directory()
        call directory strip(MessengerDir,'T','\')
    end
/* ok = VRRedirectStdio('ON')
say 'start 'MESSENGER' /Q 'TempFile */
    'start 'MESSENGER' /O'TempFile
    if MessengerDir <> '' then ok = directory(oldDir)
return

/*:VRX         _EMailNetscape
*/
_EMailNetscape: 
    TempFile = strip(value('TEMP',,'OS2ENVIRONMENT'),'T','\')||'\OdinBug.$$$'
    ok = SysFileDelete(TempFile)
    ok = lineout(TempFile, 'To: 'EMail)
    ok = lineout(TempFile, 'Subject: Odin Bug Report: 'VRget("EF_AppName","Value")' 'VRget("EF_AppVersion","Value"))
    ok = lineout(TempFile, 'Content-Type: text/plain; charset=us-ascii')
    ok = lineout(TempFile, 'Content-Transfer-Encoding: 7bit')
    ok = charout(TempFile, CRLF)
    do until lines(RPTFileName) = 0
        ok = lineout(TempFile, linein(RPTFileName))
    end
    ok = Lineout(TempFile)
    ok = lineout(RPTFileName)
    if MessengerDir <> '' then do
        olddir = directory()
        call directory strip(MessengerDir,'T','\')
    end
    'start 'messenger' -compose 'TempFile
    if MessengerDir <> '' then ok = directory(oldDir)
return

/*:VRX         _EMailPMMSend
*/
_EMailPMMSend: 
    if MessengerDir <> '' then do
        olddir = directory()
        call directory strip(MessengerDir,'T','\')
    end
    ok = SysfileTree(MessengerDir'TEMP',TempDir., 'DO')
    if TempDir.0 = 0 then call _LaunchPMMail
    ok = SysfileTree(MessengerDir'TEMP',TempDir., 'DO')
    if TempDir.0 = 0 then do
        Msg.Type = 'Warning'
        Msg.Text = 'Failed to start PMMail! Please, check your email settings!'
        call _ShowMsg
    end
    'start /MIN /C 'messenger' /M "'RPTFileName'" "'left(EMail,pos('@',EMail)-1)' <'EMail'>" "Odin Bug Report: 'VRget("EF_AppName","Value")' 'VRget("EF_AppVersion","Value")'" "'VRGet("EF_Account","Value")'"'
    if MessengerDir <> '' then ok = directory(oldDir)
return

/*

*/

/* PMMSend Syntax:

USAGE: PMMSend <OPTION> <File To Send> <To E-Mail Address> <Subject>
       <Account Directory To Send From> <body file (/a option only)>

    Where <OPTION> is one of the following:

        /a Attachment option.  In this case the file to send will be an
                               attachment.  Note that the attachment type
                               will be read from the account config.
                               When using this option, you may specify
                               a text file to be used as the body of the
                               message.
        /f FAX option.  In this case the file to send will be a tiff
                        or other data file.
        /m Message option.  In this case the file to send will be a
                            message body.

EXAMPLE: PMMSEND /F c:\mydir\fax.tif "User <user@domain.com>"
                "A Fax For You" USER_D1.ACT

Options Must Begin With A Slash '/'  */

/*:VRX         _EMailPolarbar
*/
_EMailPolarbar: 
    EMailFile = SysTempFileName(Account||'\OdinB???.POP')
    ok = lineout(EMailFile, 'To: 'EMail)
    ok = lineout(EMailFile, 'Subject: Odin Bug Report: 'VRget("EF_AppName","Value")' 'VRget("EF_AppVersion","Value"))
    ok = lineout(EMailFile, 'Content-Type: text/plain; charset=us-ascii')
    ok = lineout(EMailFile, 'Content-Transfer-Encoding: 7bit')
    ok = charout(EMailFile, CRLF)
    do until lines(RPTFileName) = 0
        ok = lineout(EMailFile, linein(RPTFileName))
    end
    ok = Lineout(EMailFile)
    ok = lineout(RPTFileName)
    ok = VRMethod( "Screen", "ListWindows", "window." )
    PolarBarRunning = 0
    if ok = 1 then do
        do i = 1 to window.0
            if pos("Polarbar",VRGet( word( window.i, 1 ),"Caption" )) > 0 then do
                PolarBarRunning = 1
                leave
            end
        end
    end
    if PolarBarRunning then do
        Msg.Type = 'I'
        Msg.Text = 'PolarBar is running. Please, reindex your outbox now to retrieve the bugreport!'
        call _ShowMsg
    end
    else do /* Start PolarBar */
        if MessengerDir <> '' then do
            olddir = directory()
            call directory strip(MessengerDir,'T','\')
        end
        'start /min /c 'messenger
        if MessengerDir <> '' then ok = directory(oldDir)
    end
return

/*:VRX         _FillPageException
*/
_FillPageException: 
    ok = VRMethod( "LB_Exception", "Reset" ) 
    I = 1
    do until lines(ExceptFilename) = 0
         ExcLine.I = linein(ExceptFileName)
         I = I + 1
    end    
    ExcLine.0 = I
    ok = stream(ExceptFilename,"c", "close")
    ok = VRMethod("LB_Exception", "AddStringList", "ExcLine.") 
return

/*:VRX         _GetException
*/
_GetException: 
    threadID = VRMethod( "Application", "StartThread", "GetExcpt", LogFileName)
return

/*:VRX         _GotNewLogFileName
*/
_GotNewLogFileName: 
    ok = VRSet("PB_CreateReport","Visible", 1)
    ok = VRset("EF_LogFile","Value",LogFilename)
    TestAppDir  = strip(VRParseFileName( LogFileName, "DP" ),'T','\') || '\'
    ok = VRset("EF_TestAppDir","Value",TestAppDir)
    ok = SysFileDelete(ExceptFileName)
    ExceptionSearchDone = 0
    ok = VRSet("EF_LogFile","Enabled", 0)
    ok = VRSet("PB_SearchLogFile","Enabled", 0)
    ok = VRMethod("LB_Exception","Reset")

    call _AnalyseOdin32_xLog
    call _AnalyseKernel32DLL
    call _GUIRedraw2
    call _GetException
    ok = VRSet("PB_BrowseLogFile","Enabled",1)
    ok = VRSet("TM_CheckValid","Enabled",1)
return

/*:VRX         _GUIEMailInit
*/
_GUIEMailInit: 
    select
        when translate(Messenger) = 'MR2I.EXE' then do
            ok = VRSEt("RB_MR2ICE","set",1)
            EMailSearchDirMask = Messenger
            if MessengerDir = "" then do
                ok = VRset("PB_SearchMessengerDir","Visible", 0)
                ok = VRset("DT_MessengerDir","Visible", 0)
                ok = VRset("EF_MessengerDir","Visible", 0)
            end
            else do
                ok = VRset("PB_SearchMessengerDir","Visible", 1)
                ok = VRset("DT_MessengerDir","Caption", "MR/2 ICE directory")
                ok = VRset("DT_MessengerDir","Visible", 1)
                ok = VRset("EF_MessengerDir","Visible", 1)
            end
            ok = VRset("DDCB_Messenger","Visible", 0)
            ok = VRset("DT_Messenger","Visible", 0)
            ok = VRset("EF_Account","Visible", 0)
            ok = VRset("DT_Account","Visible", 0)
        end
        when translate(Messenger) = 'NETSCAPE.EXE' then do
            ok = VRSEt("RB_Messenger461","set",1)
            EMailSearchDirMask = Messenger
            if MessengerDir = "" then do
                ok = VRset("PB_SearchMessengerDir","Visible", 0)
                ok = VRset("DT_MessengerDir","Visible", 0)
                ok = VRset("EF_MessengerDir","Visible", 0)
            end
            else do
                ok = VRset("PB_SearchMessengerDir","Visible", 1)
                ok = VRset("DT_MessengerDir","Caption", "Netscape directory")
                ok = VRset("DT_MessengerDir","Visible", 1)
                ok = VRset("EF_MessengerDir","Visible", 1)
            end
            ok = VRset("DDCB_Messenger","Visible", 0)
            ok = VRset("DT_Messenger","Visible", 0)
            ok = VRset("EF_Account","Visible", 0)
            ok = VRset("DT_Account","Visible", 0)
        end
        when translate(Messenger) = 'PMMSEND.EXE' then do
            EMailSearchDirMask = Messenger
            ok = VRSEt("RB_PMMail","set",1)
            ok = VRset("DT_MessengerDir","Caption", "PMMail directory")
            ok = VRset("DDCB_Messenger","Visible", 0)
            ok = VRset("EF_MessengerDir","Visible", 1)
            ok = VRset("EF_Account","Visible", 1)
            ok = VRset("DT_Messenger","Visible", 0)
            ok = VRset("DT_MessengerDir","Visible", 1)
            ok = VRset("PB_SearchMessengerDir","Visible", 1)
            ok = VRset("DT_Account","Caption", "Account name")
            ok = VRset("DT_Account","Visible", 1)
        end
        when _xlower(Messenger) = 'jre.exe -cp polarbar.zip org.polarbar.mailer' then do
            Messenger = _xlower(Messenger)
            EMailSearchDirMask = 'polarbar.zip'
            ok = VRset("DDCB_Messenger","Value", Messenger)
            ok = VRSEt("RB_Polarbar","set",1)
            ok = VRset("DDCB_Messenger","Visible", 0)
            ok = VRset("PB_SearchMessengerDir","Visible", 1)
            ok = VRset("EF_MessengerDir","Visible", 1)
            ok = VRset("EF_Account","Visible", 1)
            ok = VRset("DT_Messenger","Visible", 0)
            ok = VRset("DT_MessengerDir","Caption", "Polarbar directory")
            ok = VRset("DT_MessengerDir","Visible", 1)
            ok = VRset("DT_Account","Caption", "Outbox directory")
            ok = VRset("DT_Account","Visible", 1)
        end
        otherwise do
            ok = VRSEt("RB_OtherEmailClient","set",1)
            ok = VRset("DDCB_Messenger","Visible", 1)
            ok = VRset("PB_SearchMessengerDir","Visible", 1)
            ok = VRset("EF_MessengerDir","Visible", 1)
            ok = VRset("EF_Account","Visible", 0)
            ok = VRset("DT_Messenger","Visible", 1)
            ok = VRset("DT_MessengerDir","Visible", 1)
            ok = VRset("DT_Account","Visible", 0)
        end 
    end
return

/*:VRX         _GUIInit
*/
_GUIInit: 
    /* Odin Page */
    if WIN32LOG_ENABLED \= '' then ok = VRSet("CB_WIN32LOG_ENABLED","set",1)
                              else ok = VRSet("CB_WIN32LOG_ENABLED","set",0)

    ok = VRSet( "Page_6",            "HintText", "Currently using "our_ini) /* Settings page */
    ok = VRSet( "EF_BuildDir",       "Value", BuildDir       )
    ok = VRSet( "EF_TestAppDir",     "Value", TestAppDir     )
    ok = VRSet( "EF_LogFileBrowser", "Value", LogFileBrowser )
    ok = VRSet( "SPIN_BufferSize",   "Value", BufferSize     )
    ok = VRSet( "CB_AddException",   "set",   AddException   )
    ok = VRSet( "CB_UsingPrivateBuilds",   "set", UsingPrivateBuilds)
    ok = VRSet( "CB_DontSaveAppDir", "set",   DontSaveAppDir )
    ok = VRSet( "CB_AskDeleteLog",   "set",   AskDeleteLog   )
    ok = VRset( "CB_xf86supsys",     "set",   0)

    /* EMail Page */

    ok = VRSet( "DDCB_EMail",        "Value", EMail          )
    ok = VRSet( "DDCB_Messenger",    "Value", Messenger      )
    ok = VRSet( "EF_MessengerDir",   "Value", MessengerDir   )
    ok = VRSet( "EF_Account",        "Value", Account        )

    call _GUIEMailInit

    /*  enter help file */
    ok = VRSet("OdinBugReport", "HelpFile", base_dir"OdinBug.HLP") /* Main Window */
/*  ok = VRSet("Page_1",        "HelpFile", base_dir"OdinBug.HLP") /* Application */
    ok = VRSet("Page_3",        "HelpFile", base_dir"OdinBug.HLP") /* Odin */
    ok = VRSet("Page_4",        "HelpFile", base_dir"OdinBug.HLP") /* System */
    ok = VRSet("Page_5",        "HelpFile", base_dir"OdinBug.HLP") /* Settings */
    ok = VRSet("Page_6",        "HelpFile", base_dir"OdinBug.HLP") /* EMail */  */
return

/*:VRX         _GUIRedraw
*/
_GUIRedraw: 
    ok = VRset("CB_xf86supsys","set", Foundxf86supsys)
    if FoundWin32KSysNow then ok = VRset("DDCB_LaunchType","Value", "Win32K.SYS")
        else ok = VRSet("EF_Win32KParm","Value","Win32K.SYS is currently not installed!")
    ok = VRset("DDCB_DD","Value", Driver)
/*  ok = VRSet("DDCB_DDVer","Value","unknown") */
    ok = VRSet("DDCB_OS2Version","value", OS2Type)
    ok = VRSet("EF_KrnlBuildVer","value", KrnlBuildVer)
    ok = VRSet("EF_Version","Value",os_ver)
    ok = VRSet("DDCB_FPK","Value",fpk_level)
    if FoundScitech then do
        Ok = VRSet("DDCB_DDVer","Value",SddBuildNr)
        ok = VRSet("DDCB_ChipSet","value",Chipset)
    end
    if FoundXf86SupSys 
    then ok = VRSet( "EF_XF86supVersion", "Value", XF86supVersion )
    else ok = VRSet( "EF_XF86supVersion", "Visible", 0 )
    ok = VRSet("EF_LastWarpIN","Value",LastWarpIN)
return

/*:VRX         _GUIRedraw2
*/
_GUIRedraw2: 
    if \FoundWin32KSysNow & FoundWin32KSysAtBugTime then do
        ok = VRset("DDCB_LaunchType","Value", "Win32K.SYS was installed, when the bug occured, but it is not now!")
    end
    if FoundWin32KSysNow & \FoundWin32KSysAtBugTime then do
        ok = VRset("DDCB_LaunchType","Value", "Win32K.SYS is installed now, but was not, when the bug occured!")
    end
    if \FoundWin32KSysNow & \FoundWin32KSysAtBugTime then do
        ok = VRset("DDCB_LaunchType","Value", "PE.EXE")
    end
    if FoundWin32KSysNow & FoundWin32KSysAtBugTime then do
        ok = VRSet("EF_Win32KParm","value",Win32Kparm)
    end
    ok = VRset("EF_OdinWindir","value",OdinWinDir)
    ok = VRset("EF_OdinWinSysdir","value",OdinSysDir)
    ok = VRset("EF_EXEFile","value",EXEFile)
    ok = VRset("EF_OdinINI","value",OdinINI)
    ok = VRMethod("EF_Appname","SetFocus")
return

/*:VRX         _INIRead
*/
_INIRead: 
    /* Show our window */
    if translate(VRGet("Application","Commandline")) <> '/INSTALLONLY' then do
         ok = VRSet("OdinBugReport","Visible", 1)
    end

    /* delete old and obsolete files */
    ok = SysFileDelete('OdinBug.err')
    ok = SysFileDelete('OdinBug.CF')
    ok = SysFileDelete('OdinBug.CFG')

    /* Msg.Title = default title for message boxes */
    Msg.Title               = 'Odin Bug Report Generator'

    /*  Initialize variables */
    WarnOld                 = 10 /* days */
    TooOld                  = 20 /* days */
    OverrideTooOld          = 0
    TooOldMsg               = 0
    AgeOfBuild              = 0
    AllowBugReport          = 0
    TempFile                = ''
    ExceptionSearchDone     = 0
    DestroyWPS              = 0
    ReplaceObjects          = 0

    /* for abbreviation purposes */    
    CRLF                    = d2c(13)||d2c(10)

    /* ConfigSys vars */
    FoundWin32KSysNow       = 0
    Win32KConfigSys         = ''
    FoundScitech            = 0
    Foundxf86supsys         = 0
    XF86supVersion          = '(init)'
    Driver                  = ''

    /* odin32_?.log vars */
    LogFileName             = ''
    FoundWinDir             = 0
    FoundSysDir             = 0
    FoundExe                = 0
    FoundOdinIni            = 0
    FoundWin32KSysAtBugTime = 0
    EXEFile                 = ''
    OdinSysDir              = ''
    OdinWinDir              = ''
    OhNooo                  = 0

    /* Odin.INI */
    OdinInstLine            = '(not found)'
    OdinInstDate            = '(?)'
    OdinInstTime            = '(?)'
    LastOdinInst            = ''
    InstallType             = ''
    OdinINI                 = ''
    buildDate               = ''

    /* OdinInst.EXE */
    OdinInstVer             = '(unknown)'

    /* Kernel32.DLL */
    Kernel32Ver             = '(unknown)'
    Kernel32Date            = ''

    /* Scitech driver */
    SddBuildNr              = '(unknown)'

    /* OS/2 type */
    KrnlBuildVer            = '(unknown)'

    /* WarpIN stuff */
    LastWarpIN              = ''
    WarpINDB                = ''

    /* save current directory */
    CurDirectory            = strip(directory(),'T','\') || '\'

    /* Get logging status for current session - read only */ 
    win32log_enabled        = Value('WIN32LOG_ENABLED',,'OS2ENVIRONMENT')

    /* locate the base directory for INIfile */
    ok = SysFileTree('OdinBug.EXE',exe_file.,'FO')
    if exe_file.0 = 1 then exe_file = exe_file.1
                      else exe_file = SysSearchPath('PATH','OdinBug.EXE')

    /* This does not seem to be reliable...
    exe_file                = VRGet("Application","Program") 
     */
    base_dir                = strip(VRParseFileName( exe_file, "DP" ),'T','\') || '\'
    our_ini                 = base_dir || 'OdinBug.INI'

    /* read settings from our_ini */
    BuildDir                = VRGetIni( "OdinBug", "BuildDir", our_ini, 'NoClose' )
    TestAppDir              = VRGetIni( "OdinBug", "TestAppDir", our_ini, 'NoClose' )
    LogFileBrowser          = VRGetIni( "OdinBug", "LogFileBrowser", our_ini, 'NoClose' )
    DontSaveAppDir          = VRGetIni( "OdinBug", "DontSaveAppDir", our_ini, 'NoClose' )
    AskDeleteLog            = VRGetIni( "OdinBug", "AskDeleteLog", our_ini, 'NoClose' )
    EMail                   = VRGetIni( "OdinBug", "EMail", our_ini, 'NoClose' )
    Messenger     = translate(VRGetIni( "OdinBug", "Messenger", our_ini, 'NoClose' ))
    MessengerDir            = VRGetIni( "OdinBug", "MessengerDir", our_ini, 'NoClose' )
    Account                 = VRGetIni( "OdinBug", "Account", our_ini, 'NoClose' )
    BufferSize              = VRGetIni( "OdinBug", "BufferSize", our_ini, 'NoClose' )
    AddException            = VRGetIni( "OdinBug", "AddException", our_ini, 'NoClose' )
    UsingPrivateBuilds      = VRGetIni( "OdinBug", "UsingPrivateBuilds", our_ini )
    OdinBugVersion          = VRGetIni( "OdinBug", "Version", our_ini )

    /* we need this in order to determine the email client directory */
    EMailSearchDirMask      = ''

    /*  new version ?  Update WPS objects! */
    if (OdinBugVersion = '' | OdinBugVersion < word(VRGet("OdinBugReport","HintText"),2)) & translate(VRGet("Application","Commandline")) <> '/INSTALLONLY' then do
        call _InstallWPSObjects
        call PB_Help_Click
    end

    if OdinBugVersion > word(VRGet("OdinBugReport","HintText"),2) then do
        Msg.Type = "W"
        Msg.Text = "You have used a newer version of OdinBug recently ("OdinBugVersion")! This version ("word(VRGet("OdinBugReport","HintText"),2)") might not have all features that you expect!"
        call _ShowMsg
    end

    /* set default values */
    if BuildDir       = '' then Builddir = strip(directory(),'T','\')||'\'
    if DontSaveAppDir = '' then DontSaveAppDir = 1
    if AskDeleteLog   = '' then AskDeleteLog   = 0
    if TestAppDir     = '' then TestAppDir = strip(directory(),'T','\')||'\'
    if EMail          = '' then EMail = 'odinusers@yahoogroups.com'
    if Messenger      = '' then Messenger = 'NETSCAPE.EXE'
    /* MessengerDir   stays empty if empty */ 
    /* Account        stays empty if empty */
    /* LogFileBrowser stays empty if empty */
    if BufferSize     = '' then BufferSize = 10
    if AddException   = '' then AddException = 1
    if UsingPrivateBuilds = '' then UsingPrivateBuilds = 0
    /* Now let us process the commandline */    
    call _CommandLineParse
return

/*:VRX         _InitProgress
*/
_InitProgress: /* Initialize the progress bar */
    ok = VRSet("Progress","Visible", 1)
    ok = VRSet("DT_Progress","Visible", 0)
    ok = VRSet("SL_Progress","Percentile",0)
return

/*:VRX         _INIWrite
*/
_INIWrite: 
    ok = VRSetIni( "OdinBug", "BuildDir", BuildDir, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "DontSaveAppDir", DontSaveAppDir, our_ini, 'NoClose' )
    if \DontSaveAppDir then ok = VRSetIni( "OdinBug", "TestAppDir", TestAppDir, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "AskDeleteLog", AskDeleteLog, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "EMail", EMail, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "Messenger", Messenger, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "MessengerDir", MessengerDir, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "LogFileBrowser", LogFileBrowser, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "Account", Account, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "BufferSize", BufferSize, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "AddException", AddException, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "UsingPrivateBuilds", UsingPrivateBuilds, our_ini, 'NoClose' )
    ok = VRSetIni( "OdinBug", "Version", OdinBugVersion , our_ini )
return

/*:VRX         _InstallWPSObjects
*/
_InstallWPSObjects: 
    /* write new version number to disk */
    OdinBugVersion = word(VRGet("OdinBugReport","HintText"),2)
    ok = VRSetIni( "OdinBug", "Version", OdinBugVersion , our_ini )

    /* destroy old obsolete OdinBug object */
    ok = SysDestroyObject("<OdinBug>")

    /* Set Creation Disposition - we are polite and update per default */
    if ReplaceObjects then CreateCollision = 'Replace'
                      else CreateCollision = 'Update'

    rc1 = _CreateObject( 'WPFolder','Odin','<WP_PROGRAMSFOLDER>',,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=DEFAULT;'||,
            'OBJECTID=<ODINFOLDER>',,
        CreateCollision )

    rc2 = _CreateObject( 'WPProgram','OdinBug','<ODINFOLDER>',,
            'EXENAME='||exe_file||';'||,
            'PROGTYPE=PM;'||,
            'TITLE=Report Odin Bugs;'||,
            'ASSOCTYPE=Odin-Logfiles,,;'||,
            'ASSOCFILTER=odin32_?.log,,;'||,
            'NOPRINT=YES;'||,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=DEFAULT;'||,
            'OBJECTID=<Report_Odin_Bugs>',,
        CreateCollision )

    rc3 = _CreateObject( 'WPProgram','Report Odin Bugs Manual','<ODINFOLDER>',,
            'EXENAME=VIEW.EXE;'||,
            'PROGTYPE=PM;'||,
            'PARAMETERS='||base_dir||'ODINBUG.HLP Introduction;'||,
            'TITLE=Report Odin Bugs^Manual;'||,
            'NOPRINT=YES;'||,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=DEFAULT;'||,
            'OBJECTID=<Report_Odin_Bugs_Help>',,
        CreateCollision )
    rc4 = _CreateObject( 'WPProgram','Report Odin Bugs Readme','<ODINFOLDER>',,
            'EXENAME=E.EXE;'||,
            'PROGTYPE=PM;'||,
            'PARAMETERS='||base_dir||'Readme.ODINBUG;'||,
            'TITLE=Report Odin Bugs^ReadMe;'||,
            'NOPRINT=YES;'||,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=DEFAULT;'||,
            'OBJECTID=<Report_Odin_Bugs_ReadMe>',,
        CreateCollision )
    rc5 = _CreateObject( 'WPUrl','Subscribe to odinusers mailing list','<ODINFOLDER>',,
            'TITLE=Subscribe to^odinusers mailing list;'||,
            'URL=http://groups.yahoo.com/group/odinusers/join;'||,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=ICON;'||,
            'OBJECTID=<Subscribe_odinusers@yahoogroups>',,
        CreateCollision )
    rc6 = _CreateObject( 'WPProgram','Odin User''s Manual','<ODINFOLDER>',,
            'EXENAME=VIEW.EXE;'||,
            'PROGTYPE=PM;'||,
            'PARAMETERS='||base_dir||'ODINUSER.INF Introduction;'||,
            'TITLE=Odin User''s Manual;'||,
            'NOPRINT=YES;'||,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=DEFAULT;'||,
            'OBJECTID=<Odin_Users_Manual>',,
        CreateCollision )

    rc7 = _CreateObject( 'WPUrl','Subscribe to win32os2-wai mailing list^(for developers)','<ODINFOLDER>',,
            'TITLE=Subscribe to^win32os2-wai mailing list^(for developers);'||,
            'URL=http://groups.yahoo.com/group/win32os2-wai/join;'||,
            'HIDEBUTTON=DEFAULT;'||,
            'MINWIN=DEFAULT;'||,
            'CCVIEW=DEFAULT;'||,
            'DEFAULTVIEW=ICON;'||,
            'OBJECTID=<Subscribe_win32os2wai@yahoogroups>',,
        CreateCollision )

    if rc1 + rc2 + rc3 + rc4 + rc5 + rc6 + rc7 = 7 & translate(VRGet("Application","Commandline")) <> '/INSTALLONLY' then do
        msg.Text = 'All OdinBug object(s) created/updated sucessfully!'
        msg.Type = 'Info'
        call _ShowMsg
    end
    if rc1 + rc2 + rc3 + rc4 + rc5 + rc6 + rc7 <> 7 then do
        msg.Text = 'Error creating OdinBug object(s)!'
        msg.Type = 'Error'
        call _ShowMsg
    end
return

/*:VRX         _LaunchPMMail
*/
_LaunchPMMail: 
    ok = VRSEt("SL_Progress","Visible", 0)
    ok = VRSEt("DT_Progress","Visible", 1)
    ok = VRSet("DT_Progress","Caption", "Trying to start PMMail. Please wait...")
    ok = VRSet("Progress","Visible", 1)
    'start PMMAIL.EXE'
    ok = SysfileTree(MessengerDir'TEMP',TempDir., 'DO')
    Ctd = 10
    do while (TempDir.0 = 0) & (Ctd > 0)
        ok = SysSleep(1)
        Ctd = Ctd - 1
        ok = SysfileTree(MessengerDir'TEMP',TempDir., 'DO')
    end
    ok = VRSet("Progress","Visible", 0)
    ok = VRSEt("SL_Progress","Visible", 1)
    ok = VRSEt("DT_Progress","Visible", 0)
return

/*:VRX         _LoadOtherFuncs
*/
_LoadOtherFuncs: 
    /* Register RexxUtil-API */
    call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
    call SysLoadFuncs
return

/*:VRX         _ProcessHotKeys
*/
_ProcessHotKeys: 
    KeyObj = VRInfo( "Object" )
    KeyStr = VRGet( KeyObj, "KeyString" )
    select
        when KeyStr = "{Alt}c" then call PB_CreateReport_Click 
        when KeyStr = "{Alt}m" then call PB_Email_Click
        when KeyStr = "{Alt}o" then call PB_OpenReport_Click
        when KeyStr = "{Alt}b" then call PB_BrowseLogFile_Click 
        when KeyStr = "{Alt}h" then call PB_Help_Click 
        when KeyStr = "{Alt}x" then call PB_Exit_Click
        when KeyStr = "{Alt}{PgUp}" then call Activate max(CurrentPageNr-1,1)
        when KeyStr = "{Alt}{PgDown}" then call Activate min(CurrentPageNr+1,Pages)
        otherwise ok = VRSet(KeyObj, "KeyString", Keystr)
    end
return
/*:VRX         _ShowMsg
*/
_ShowMsg: 
    Buttons.1 = "OK"
    Buttons.0 = 1
    id = VRMessage( VRWindow(), Msg.Text, Msg.Title, Msg.Type, "Buttons.", buttons.1, buttons.1 )
return

/*:VRX         _StartUp
*/
_StartUp: 
    call _GUIInit
    call _AnalyseConfigSys
    call _AnalyseSysLevelOs2
    if Foundxf86supsys then call _AnalyseXF86SupSys
    call _AnalyseSysLevelFPK
    if FoundScitech then call _AnalyseScitechDD
    call _AnalyseWarpINdatabase
    call _GUIRedraw
    if LogFileName = '' | LogfileName = 'LOGFILENAME'
        then call PB_SearchLogFile_Click
        else call _GotNewLogfileName

    isStartup = 1
    call PB_SearchBuild_Click
return

/*:VRX         _UnInstallWPSObjects
*/
_UnInstallWPSObjects: 
    msg.Text = 'Really remove all OdinBug objects?'
    msg.Type = 'Q'
    Buttons.1 = "Yes"
    Buttons.2 = "No"
    Buttons.0 = 2
    id = VRMessage( VRWindow(), "Really remove all OdinBug objects?", Msg.Title, "Query", "Buttons.", buttons.2, buttons.2 )
    if id = 1 then do
        ok = SysDestroyObject("<Report_Odin_Bugs_Help>")
        ok = SysDestroyObject("<Report_Odin_Bugs_ReadMe>")
        ok = SysDestroyObject("<Subscribe_odinusers@yahoogroups>")
        DestroyWPS = 1
        msg.Text = 'All OdinBug object(s) destroyed!'
        msg.Type = 'Info'
        call _ShowMsg
    end
return

/*:VRX         _xdate
*/
_xdate: procedure
    parse arg datestring
    hdate = ''
    do I = 1 to 2
        J = I * 2 - 1
        hdate = hdate||right('0'||x2d(substr(datestring,J,2)),2)'.'
    end
    Jahr = x2d(substr(datestring,5,2)) + x2d(substr(datestring,7,2)) *256
    hdate= hdate||Jahr
return hdate

/*:VRX         _xlower
*/
_xlower: procedure
    Tstring = arg(1)
    RString = translate(TString, xrange('a','z'), xrange('A','Z'))
return RString

/*:VRX         _xmonth
*/
_xmonth: procedure
   parse upper arg TextMonth
   select
       when left(TextMonth,3) = 'JAN' |,
            left(TextMonth,3) = 'JŽN' then NumMonth = 1
       when left(TextMonth,3) = 'FEB' then NumMonth = 2
       when left(TextMonth,3) = 'MAR' |,
            left(TextMonth,3) = 'MŽR' then NumMonth = 3
       when left(TextMonth,3) = 'APR' then NumMonth = 4
       when left(TextMonth,3) = 'MAY' |,
            left(TextMonth,3) = 'MAI' then NumMonth = 5
       when left(TextMonth,3) = 'JUN' then NumMonth = 6
       when left(TextMonth,3) = 'JUL' then NumMonth = 7
       when left(TextMonth,3) = 'AUG' then NumMonth = 8
       when left(TextMonth,3) = 'SEP' then NumMonth = 9
       when left(TextMonth,3) = 'OCT' |,
            left(TextMonth,3) = 'OKT' then NumMonth = 10
       when left(TextMonth,3) = 'NOV' then NumMonth = 11
       when left(TextMonth,2) = 'DE'  then NumMonth = 12
       otherwise NumMonth = 0 /* this indicates something went wrong... */
   end
   NumMonth = right('0'||NumMonth,2)
return NumMonth

/*:VRX         _xtime
*/
_xtime: procedure
    parse arg timestring
    htime = ''
    do I = 1 to length(timestring)/2
        J = I * 2 - 1
        htime = htime||right('0'||x2d(substr(timestring,J,2)),2)':'
    end
    htime = strip(htime,'T',':')
return htime

/*:VRX         AboutPage_Close
*/
AboutPage_Close: 
    call AboutPage_Fini
return

/*:VRX         AboutPage_Create
*/
AboutPage_Create: 
    call AboutPage_Init
    ok = VRSet("DT_AboutText","Caption","OdinBug Version "word(VRget("OdinBugReport","HintText"),2)||CRLF||"(c) 2001-2002 by Herwig Bauernfeind")
return

/*:VRX         AboutPage_Fini
*/
AboutPage_Fini: 
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         AboutPage_Init
*/
AboutPage_Init: 
    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 1
        call VRMethod window, "Activate"
    end
    drop window
return

/*:VRX         Activate
*/
Activate: 
    parse arg Nr
    call PushbackAll
    CurrentPageNr = Nr
    /* ok = VRSet("PB_"Nr,"Font","9.WarpSans Bold") */
    ok = VRSet("TAB_"Nr,"Top", TabTop-8)
    ok = VRSet("TAB_"Nr,"Left",TabWidth*(Nr-1)+PageLeft)
    ok = VRSet("TAB_"Nr,"PicturePath", "#1"||Nr)
    ok = VRSet("TAB_"Nr,"AutoSize",1)
    ok = VRset("DT_PageName", "Caption", VRGet("Page_"Nr, "Caption"))
    ok = VRSet("Page_"Nr,"Visible", 1)
    ok = VRSet("Page_"Nr,"BorderType", 'None')
    ok = VRSet("Page_"Nr,"Height", VRget("GB_Main2","Height")-600)
    ok = VRSet("Page_"Nr,"Width", VRget("GB_Main2","Width")-450)
    ok = VRSet("Menu_"Nr,"Checked", 1)
    select 
        when Nr = 1 then ok = VRSet("IPB_paper","PicturePath","#4")
        when Nr = Pages then ok = VRSet("IPB_paper","PicturePath","#6")
        otherwise ok = VRSet("IPB_paper","PicturePath","#5")
    end
return

/*:VRX         CB_AddException_Click
*/
CB_AddException_Click: 
    AddException = VRget("CB_AddException","Set")
return

/*:VRX         CB_AskDeleteLog_Click
*/
CB_AskDeleteLog_Click: 
    AskDeleteLog = VRget("CB_AskDeleteLog","set")
return

/*:VRX         CB_DontSaveAppDir_Click
*/
CB_DontSaveAppDir_Click: 
    DontSaveAppDir = VRget("CB_DontSaveAppDir","set")
return

/*:VRX         CB_OhNooo_Click
*/
CB_OhNooo_Click: 
    OhNooo = VRGet("CB_OhNooo","set")
return

/*:VRX         CB_OverRideTooOld_Click
*/
CB_OverRideTooOld_Click: 
    OverRideTooOld = VRGet("CB_OverRideTooOld","set")
return

/*:VRX         CB_ReplaceObjects_Click
*/
CB_ReplaceObjects_Click: 
    ReplaceObjects = VRget("CB_replaceObjects","set")
return

/*:VRX         CB_UsingPrivateBuilds_Click
*/
CB_UsingPrivateBuilds_Click: 
    UsingPrivateBuilds = VRGet("CB_UsingPrivateBuilds","set")
return

/*:VRX         CB_WIN32LOG_ENABLED_Click
*/
CB_WIN32LOG_ENABLED_Click: 
    if WIN32LOG_ENABLED \= '' then ok = VRSet("CB_WIN32LOG_ENABLED","set",1)
                              else ok = VRSet("CB_WIN32LOG_ENABLED","set",0)
return

/*:VRX         CB_xf86supsys_Click
*/
CB_xf86supsys_Click: 
    ok = VRSet("CB_xf86supsys","Set",Foundxf86supsys)
return

/*:VRX         DDCB_EMail_Change
*/
DDCB_EMail_Change: 
    if pos("@",VRGet("DDCB_EMail","Value")) = 0  then return
    EMail = VRGet("DDCB_EMail","Value")
return

/*:VRX         DDCB_LaunchType_Change
*/
DDCB_LaunchType_Change: 
    if FoundWin32KSysNow & FoundWin32KSysAtBugTime & VRGet("DDCB_LaunchType","value") = 'PE.EXE' then do
        Msg.Type = "W"
        Msg.Text = "Usage of PE.EXE is not recommended, if Win32K.SYS is installed!"
        call _ShowMsg
    end
return

/*:VRX         DDCB_Messenger_Change
*/
DDCB_Messenger_Change: 
    if pos(".EXE",translate(VRGet("DDCB_Messenger","Value"))) = 0  then return
    Messenger = VRGet("DDCB_Messenger","Value")
return

/*:VRX         EF_Account_Change
*/
EF_Account_Change: 
    Account = VRget("EF_Account","Value")
return

/*:VRX         EF_AppName_Change
*/
EF_AppName_Change: 
    if left(VRGet("EF_AppName","Value"),1) = '#' then do
        ok = VRset("EF_Appname","Value","Odin Bug Test")
        ok = VRset("EF_AppVersion","Value","9.9.9")
        ok = VRset("DDCB_URL","Value","www.odinbugtest.net")
        ok = VRSet("MLE_OwnWords","Value","This is test data. It is not intended to be sent to Team Odin support sites. The above website does not exist!")
    end
return

/*:VRX         EF_BuildDir_Change
*/
EF_BuildDir_Change: 
    BuildDir = VRGet("EF_BuildDir","Value")
    if Right(BuildDir,1) <> '\' then do
        BuildDir = BuildDir || '\'
        /* ok = VRSet("EF_BuildDir","Value", BuildDir) */
    end
return

/*:VRX         EF_LogFileBrowser_Change
*/
EF_LogFileBrowser_Change: 
    LogFileBrowser = VRget("EF_LogFileBrowser","Value")
return

/*:VRX         EF_MessengerDir_Change
*/
EF_MessengerDir_Change: 
    MessengerDir = VRget("EF_MessengerDir","Value")
    if Right(MessengerDir,1) <> '\' & MessengerDir <> '' then do
        MessengerDir = MessengerDir || '\'
        /* ok = VRSet("EF_MessengerDir","Value", MessengerDir) */
    end
return

/*:VRX         EF_OdinINI_Change
*/
EF_OdinINI_Change: 
    if translate(right(VRGet("EF_OdinINI","Value"),8)) <> 'ODIN.INI' then return
    ok = SysFileTree(VRGet("EF_OdinINI","Value"),OdinIniFile.,"FO")
    if OdinINiFile.0 = 0 then return
    OdinINI = VRGet("EF_OdinINI","Value")
    call _AnalyseOdininst
    call _AnalyseOdinINI
return

/*:VRX         EF_OdinWinSysDir_Change
*/
EF_OdinWinSysDir_Change: 
    if right(VRGet("EF_OdinWinSysDir","Value"),1) <> '\' then return
    ok = SysFileTree(strip(VRGet("EF_OdinWinSysDir","Value"),'T','\'),OdinWinSysDir.,"DO")
    if OdinWinSysDir.0 = 0 then return
    OdinSysDir = VRGet("EF_OdinWinSysDir","Value")
    call _AnalyseOdininst
return

/*:VRX         EF_TestAppDir_Change
*/
EF_TestAppDir_Change: 
    TestAppDir = VRget("EF_TestAppDir","Value")
    if Right(TestAppDir,1) <> '\' then do
        TestAppDir = TestAppDir || '\'
        /* ok = VRSet("EF_TestAppDir","Value", TestAppDir) */
    end
return

/*:VRX         Fini
*/
Fini:
    window = VRWindow()
    call VRSet window, "Visible", 0
    drop window
return 0

/*:VRX         GB_Main_ContextMenu
*/
GB_Main_ContextMenu: 
    nop
return

/*:VRX         GetBootDrive
*/
GetBootDrive: 
/* Use SysBootDrive if possible */
If Rxfuncadd('SysBootDrive', 'RexxUtil', 'SysBootDrive') Then
   Return Left(Value('RUNWORKPLACE',,'OS2ENVIRONMENT'),2)
Else
   Return SysBootDrive()


/*:VRX         Halt
*/
Halt:
    signal _VREHalt
return

/*:VRX         Init
*/
Init:
window = VRWindow()
call VRMethod window, "CenterWindow"
call VRSet window, "Visible", 0
call VRMethod window, "Activate"
drop window
return

/*:VRX         IPB_Paper_Click
*/
IPB_Paper_Click: 
    if VRInfo("Left") >  VRInfo("Top") then call Activate min(CurrentPageNr+1,Pages)
    if VRInfo("Left") <= VRInfo("Top") then call Activate max(CurrentPageNr-1,1)
return

/*:VRX         Menu_1_Click
*/
Menu_1_Click: 
    call Activate 1
return

/*:VRX         Menu_2_Click
*/
Menu_2_Click: 
    call Activate 2
return

/*:VRX         Menu_3_Click
*/
Menu_3_Click: 
    call Activate 3
return

/*:VRX         Menu_4_Click
*/
Menu_4_Click: 
    call Activate 4
return

/*:VRX         Menu_5_Click
*/
Menu_5_Click: 
    call Activate 5
return

/*:VRX         Menu_6_Click
*/
Menu_6_Click: 
    call Activate 6
return

/*:VRX         OdinBugReport_Close
*/
OdinBugReport_Close:
    if AskDeleteLog & LogFileName <> '' then do
        Buttons.1 = "Yes"
        Buttons.2 = "No"
        Buttons.0 = 2
        id = VRMessage( VRWindow(), 'Delete "'LogFileName'"?', "Odin Bug Report Generator", "Query", "Buttons.", 2, 2 )
        if id = 1 then ok = SysFileDelete(LogFileName)
    end
    ok = SysFileDelete(TempFile)
    call Quit
return

/*:VRX         OdinBugReport_ContextMenu
*/
OdinBugReport_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         OdinBugReport_Create
*/
OdinBugReport_Create: 
    call _LoadOtherFuncs
    window = VRLoadSecondary( "Progress" )
    ok = VRMethod("PB_Exit","SetFocus")
    call PushbackAll
    call TAB_1_Click
    call _INIRead
    call Page1_Create
    call Page2_Create
    call Page3_Create
    call Page4_Create
    call Page5_Create
    call Page6_Create
    /* call OdinBugReport_Resize */

return

/*:VRX         OdinBugReport_KeyPress
*/
OdinBugReport_KeyPress: 
    call _ProcessHotKeys
return

/*:VRX         OdinBugReport_Resize
*/
OdinBugReport_Resize: 
    MainWin = "OdinBugReport"
    ok = VRset(MainWin,"Visible", 0)
    ok = VRSet(MainWin,"Width",max(VRget(MainWin,"Width"),Pages*TabWidth+300))
    ok = VRset("GB_Main","width",VRget(MainWin,"Width")-150)
    ok = VRset("GB_Main","Height",VRget(MainWin,"Height")-1175-330)
    ok = VRset("GB_Main2","width",VRget("GB_Main","Width")-260)
    ok = VRset("GB_Main2","Height",VRget("GB_Main","Height")-275)
    ok = VRset("GB_LineVertical","Left",VRget("GB_Main2","Width")-60)
    ok = VRset("GB_LineVertical","Height",VRget("GB_Main2","Height")-410)
    ok = VRset("IPB_Paper","Left",VRget("GB_Main2","width")-510)
    ok = VRset("GB_LineHorizTop","Width",VRget("GB_Main2","width")-480)
    ok = VRset("GB_LineHorizTop","Width",VRget("GB_Main2","width")-480)
    ok = VRset("GB_LineHorizSubtitle","Width",VRget("GB_Main2","width")-480)
    do I = 1 to Pages
        ok = VRSet("Page_"I,"Height", VRget("GB_Main2","Height")-600)
        ok = VRSet("Page_"I,"Width", VRget("GB_Main2","Width")-450)
    end
    ok = VRSEt("PB_CreateReport","top",VRGet("GB_Main","Height")+365)
    ok = VRSEt("PB_EMail","top",VRGet("GB_Main","Height")+365)
    ok = VRSEt("PB_OpenReport","top",VRGet("GB_Main","Height")+365)
    ok = VRSEt("PB_BrowseLogFile","top",VRGet("GB_Main","Height")+365)
    ok = VRSEt("PB_Help","top",VRGet("GB_Main","Height")+365)
    ok = VRSEt("PB_Exit","top",VRGet("GB_Main","Height")+365)
    ButtonDist= 1080
    ButtonNr  = 6
    ButtonPos = max((VRget(MainWin,"Width")-ButtonDist*ButtonNr)/2,0)
    ok = VRSEt("PB_CreateReport","Left",ButtonPos+ButtonDist*(ButtonNr-6))
    ok = VRSEt("PB_EMail","Left",ButtonPos+ButtonDist*(ButtonNr-5))
    ok = VRSEt("PB_OpenReport","Left",ButtonPos+ButtonDist*(ButtonNr-4))
    ok = VRSEt("PB_BrowseLogFile","Left",ButtonPos+ButtonDist*(ButtonNr-3))
    ok = VRSEt("PB_Help","left",ButtonPos+ButtonDist*(ButtonNr-2))
    ok = VRSEt("PB_Exit","left",ButtonPos+ButtonDist*(ButtonNr-1))
    ok = VRset(MainWin,"Visible", 1)
return

/*:VRX         Page1_Create
*/
Page1_Create: 
    call Page1_Init /* Application */
    call _StartUp
Return

/*:VRX         Page1_Init
*/
Page1_Init: 

return

/*:VRX         Page2_Create
*/
Page2_create:

Return
/*:VRX         Page2_init
*/
Page2_init: 

return

/*:VRX         Page3_Create
*/
Page3_create:

Return
/*:VRX         Page3_Init
*/
Page3_Init: 

return

/*:VRX         Page4_Create
*/
Page4_Create: 

Return
/*:VRX         Page4_init
*/
Page4_init: 

return

/*:VRX         Page5_Create
*/
Page5_Create: 

return

/*:VRX         Page6_Create
*/
Page6_Create: 

return

/*:VRX         PB_About_Click
*/
PB_About_Click: 
    window = VRLoadSecondary( "AboutPage", "W" )
return

/*:VRX         PB_AboutOK_Click
*/
PB_AboutOK_Click: 
    call AboutPage_Close
return

/*:VRX         PB_BrowseLogFile_Click
*/
PB_BrowseLogFile_Click: 
    'start "Browsing 'LogFileName'" /F 'LogFileBrowser' "'LogFileName'"'
return

/*:VRX         PB_CreateReport_Click
*/
PB_CreateReport_Click: 
    ok = time('R')
    ok = VRSet("TM_CheckValid","Enabled", 0)
    ok = VRSet("PB_CreateReport","Enabled", 0)
    if _CreateReport() then do
        ok = VRSet("SL_Progress","Percentile",100)
        Msg.Text = "Created "RPTFileName" successfully ("strip(time('E'),'T','0')" sec.)!"
        Msg.Type = "Info"
        call _ShowMsg    
        ok = VRSet("PB_Email","Enabled",1)
        ok = VRSet("PB_OpenReport","Enabled",1)
    end
    else ok = time('E')
    ok = VRSet("PB_CreateReport","Enabled", 1)
    ok = VRSet("TM_CheckValid","Enabled", 1)
return

/*:VRX         PB_Email_Click
*/
PB_Email_Click: 
    CapMessenger = translate(filespec('Name',Messenger))
    select
        when pos('NETSCAPE',CapMessenger) > 0 then call _EMailNetscape
        when pos('PMMSEND',CapMessenger) > 0  then call _EMailPMMSend
        when pos('POLARBAR',CapMessenger) > 0  then call _EMailPolarbar
        when pos('MR2I',CapMessenger) > 0  then call _EMailMR2ICE
        otherwise call _EMailGeneric
    end
return

/*:VRX         PB_ExeFile_Click
*/
PB_ExeFile_Click: 
    ExeFile = VRFileDialog( VRWindow(), "Search Windows Executable", "Open", TestAppDir"*.EXE", , ,  )
    call _GUIRedraw2
return

/*:VRX         PB_Exit_Click
*/
PB_Exit_Click: 
    call OdinBugReport_Close
return

/*:VRX         PB_Help_Click
*/
PB_Help_Click: 
    'view 'base_dir"OdinBug.HLP Introduction"
    /* ok = SysSetObjectData(base_dir"OdinBug.HLP","OPEN=DEFAULT;PARAMETERS=Introduction") */
return

/*:VRX         PB_Install_Click
*/
PB_Install_Click: 
    call _InstallWPSObjects
return
/*:VRX         PB_OpenReport_Click
*/
PB_OpenReport_Click: 
    ok = SysSetObjectData(RPTFileName,"OPEN=DEFAULT")
return

/*:VRX         PB_SaveSettings1_Click
*/
PB_SaveSettings1_Click: 
    call _INIWrite
    msg.Text = 'Settings saved sucessfully!'
    msg.Type = 'Info'
    call _ShowMsg
return

/*:VRX         PB_SaveSettings_Click
*/
PB_SaveSettings_Click: 
    call _INIWrite
    msg.Text = 'Settings saved sucessfully!'
    msg.Type = 'Info'
    call _ShowMsg
return

/*:VRX         PB_SearchBuild_Click
*/
PB_SearchBuild_Click: 
    ok = SysFileTree(BuildDir"odin32bin-"Kernel32Date"*",Builds.,"FO")
    if Builds.0 = 1 & isStartup
    then Filename = Builds.1
    else do
        if \UsingPrivateBuilds then do
            FileName = VRFileDialog( VRWindow(), "Step 2: Search Odin Build file", "Open", BuildDir"odin32bin-"Kernel32Date"*", , ,  )
        end
        else do
            FileName = VRFileDialog( VRWindow(), "Step 2: Search Odin Build file", "Open", BuildDir"*", , ,  )
        end
    end

    if Filename <> '' then do
        BuildFile = filespec('N',Filename)
        BuildDir = filespec('D',Filename)||filespec('P',Filename)
        if translate(right(Filename,3)) = 'WPI' then Installtype = "WarpIN"
        if translate(right(Filename,3)) = 'ZIP' then Installtype = "Manually"

        AllowBugReport = _CheckForExpiredBuild()
        ok = VRset("EF_Build","Value",BuildFile)
        ok = VRset("EF_BuildDir","Value",BuildDir)
        ok = VRset("DDCB_Installtype","Value",InstallType)
        if LogFileName = '' then do
            ok = VRset("EF_OdinWindir","value",OdinWinDir)
            ok = VRset("EF_OdinWinSysdir","value",OdinSysDir)
            ok = VRset("EF_OdinINI","value",OdinINI)
        end
    end
    isStartup = 0
return

/*:VRX         PB_SearchLogFile_Click
*/
PB_SearchLogFile_Click: 
    LogFileName = VRFileDialog( VRWindow(), "Step 1: Search Odin Logfile", "Open", TestAppDir"odin32_?.log", , ,  )
    if LogFileName <> '' 
        then call _GotNewLogfileName
        else ok = VRSet("PB_BrowseLogFile","Enabled",0)
return

/*:VRX         PB_SearchLogFileBrowser_Click
*/
PB_SearchLogFileBrowser_Click: 
    LogFileBrowser = VRFileDialog( VRWindow(), "Search Logfile browser", "Open", "*.EXE", , ,  )
    if LogFileBrowser <> '' then ok = VRset("EF_LogFileBrowser","Value",LogFileBrowser)
return

/*:VRX         PB_SearchMessengerDir_Click
*/
PB_SearchMessengerDir_Click: 
    MessengerDir = strip(VRParseFileName( VRFileDialog( VRWindow(), "Specify the email directory", "Open", EMailSearchDirMask, , ,  ), "DP" ),'T','\') ||'\'
    If MessengerDir = '\' then MessengerDir = VRGet("EF_MessengerDir","Value")
                          else ok = VRSet("EF_MessengerDir","Value",MessengerDir)
return

/*:VRX         PB_ThankYou_Click
*/
PB_ThankYou_Click: 
    Lines.1 = "Many thanks go to Team Odin and their efforts, especially to Sander van Leeuwen for his (almost) endless patience and to my testers Sven Stroh and Peter Weilbacher."
    Lines.2 = " "
    Lines.3 = "This program is free, but copyrighted software. The license for Odin is also valid for this program."
    Lines.0 = 3
    Buttons.1 = "OK"
    Buttons.0 = 1
    id = VRMessageStem( VRWindow(), "Lines.", "Thank you", "Information", "Buttons.", buttons.1, buttons.1 )
return

/*:VRX         PB_UnInstall_Click
*/
PB_UnInstall_Click: 
    call _UnInstallWPSObjects
return
/*:VRX         Progress_Close
*/
Progress_Close: 
    call Progress_Fini
return

/*:VRX         Progress_Create
*/
Progress_Create: 
    call Progress_Init
    ok = VRSet("Progress","Top",5000)
return

/*:VRX         Progress_Fini
*/
Progress_Fini: 
    window = VRInfo( "Window" )
    call VRDestroy window
    drop window
return
/*:VRX         Progress_Init
*/
Progress_Init: 
    window = VRInfo( "Object" )
    if( \VRIsChildOf( window, "Notebook" ) ) then do
        call VRMethod window, "CenterWindow"
        call VRSet window, "Visible", 0
        call VRMethod window, "Activate"
    end
    drop window
return

/*:VRX         PushbackAll
*/
PushbackAll: 
    Pages = 6
    Delta = 400 /* This is the base distance between menubar an Page */

    /* Tab Position */
    TabWidth = VRget("TAB_1","Width")
    TabHeight= VRget("TAB_1","Height")
    TabTop = Delta - TabHeight

    /* Page Position */
    PageLeft = 120
    PageTop = PageLeft + Delta

    /* Page size */
    PageHeight = 4500
    PageWidth  = 7000
    ok = VRset("DT_PageName", "Caption", "")
    ok = VRSet("GB_Main","Left", 0)
    ok = VRSet("GB_Main","Top", Delta - 60)

    do I = 1 to Pages
        ok = VRSet("TAB_"I,"Top", TabTop)
        ok = VRSet("TAB_"I,"PicturePath","#2"||I)
        ok = VRSet("TAB_"I,"Left",TabWidth*(I-1)+PageLeft)
     /* ok = VRSet("TAB_"I,"Width",TabWidth)
        ok = VRSet("TAB_"I,"Height",TabHeight) */
        ok = VRSet("Page_"I,"Height", PageHeight)
        ok = VRSet("Page_"I,"Width", PageWidth)
        ok = VRSet("Page_"I,"Left", PageLeft)
        ok = VRSet("Page_"I,"Top", PageTop)
        ok = VRSet("Page_"I,"Visible", 0)
        ok = VRSet("Page_"I,"BackColor", "<default>")
        ok = VRSet("Menu_"I,"Checked", 0)
    end
return

/*:VRX         Quit
*/
Quit:
    ok = SysFileDelete(ExceptFileName)
    window = VRWindow()
    call VRSet window, "Shutdown", 1
    drop window
    if DestroyWPS then ok = SysDestroyObject("<Report_Odin_Bugs>")
return

/*:VRX         RB_Messenger461_Click
*/
RB_Messenger461_Click: 
    if VRGet("RB_Messenger461","Set") then do
        ok = VRset("DDCB_Messenger","Value",  'NETSCAPE.EXE')
        ok = VRset("EF_MessengerDir","Value", '')
        Messenger = 'NETSCAPE.EXE'
        EMailSearchDirMask = Messenger
        CompleteMessenger = SysSearchPath("PATH",Messenger)
        if CompleteMessenger = '' then do
            Msg.Type = "W"
            Msg.Text = Messenger' could not be found in your PATH. Please, specify the EMail client directory!'
            call _ShowMsg
            ok = VRset("DT_MessengerDir","Caption", "Netscape directory")
            ok = VRset("EF_MessengerDir","Visible", 1)
            ok = VRset("DT_MessengerDir","Visible", 1)
            call PB_SearchMessengerDir_Click
        end
        else do
            ok = VRset("PB_SearchMessengerDir","Visible", 0)
            ok = VRset("EF_MessengerDir","Visible", 0)
            ok = VRset("DT_MessengerDir","Visible", 0)
        end
        ok = VRset("DDCB_Messenger","Visible", 0)
        ok = VRset("DT_Messenger","Visible", 0)
        ok = VRset("EF_Account",  "Visible", 0)
        ok = VRset("DT_Account",  "Visible", 0)
    end
return

/*:VRX         RB_MR2ICE_Click
*/
RB_MR2ICE_Click: 
    if VRGet("RB_MR2ICE","Set") then do
        ok = VRset("DDCB_Messenger","Value",  'MR2I.EXE')
        ok = VRset("EF_MessengerDir","Value", '')
        Messenger = 'MR2I.EXE'
        EMailSearchDirMask = Messenger
        CompleteMessenger = SysSearchPath("PATH",Messenger)
        if CompleteMessenger = '' then do
            Msg.Type = "W"
            Msg.Text = Messenger' could not be found in your PATH. Please, specify the EMail client directory!'
            call _ShowMsg
            ok = VRset("DT_MessengerDir","Caption", "MR/2 ICE directory")
            ok = VRset("EF_MessengerDir","Visible", 1)
            ok = VRset("DT_MessengerDir","Visible", 1)
            call PB_SearchMessengerDir_Click
        end
        else do
            ok = VRset("PB_SearchMessengerDir","Visible", 0)
            ok = VRset("EF_MessengerDir","Visible", 0)
            ok = VRset("DT_MessengerDir","Visible", 0)
        end
        ok = VRset("DDCB_Messenger","Visible", 0)
        ok = VRset("DT_Messenger","Visible", 0)
        ok = VRset("EF_Account",  "Visible", 0)
        ok = VRset("DT_Account",  "Visible", 0)
    end
return
/*:VRX         RB_OtherEmailClient_Click
*/
RB_OtherEmailClient_Click: 
    if VRget("RB_OtherEmailClient","set") then do
        EMailSearchDirMask = '*.EXE'
        ok = VRset("DDCB_Messenger","Visible", 1)
        ok = VRset("PB_SearchMessengerDir","Visible", 1)
        ok = VRset("EF_MessengerDir","Visible", 1)
        ok = VRset("EF_Account","Visible", 0)
        ok = VRset("DT_Messenger","Visible", 1)
        ok = VRset("DT_MessengerDir","Caption", "EMail client directory")
        ok = VRset("DT_MessengerDir","Visible", 1)
        ok = VRset("DT_Account","Visible", 0)
    end
return
/*:VRX         RB_PMMail_Click
*/
RB_PMMail_Click: 
    if VRGet("RB_PMMail","set") then do
        ok = VRset("DDCB_Messenger","Value", 'PMMSEND.EXE')
        Messenger = 'PMMSEND.EXE'
        EMailSearchDirMask = Messenger
        CompleteMessenger = SysSearchPath("PATH",Messenger)
        if CompleteMessenger = '' then do
            Msg.Type = "W"
            Msg.Text = Messenger' could not be found in your PATH. Please, specify the EMail client directory manually!'
            call _ShowMsg
            call PB_SearchMessengerDir_Click
        end
        else do
            ok = VRset("EF_MessengerDir","Value", filespec("D",CompleteMessenger)||filespec("P",CompleteMessenger))
        end
        ok = VRset("DDCB_Messenger","Visible", 0)
        ok = VRset("DT_Messenger","Visible", 0)
        ok = VRset("DT_MessengerDir","Caption", "PMMail directory")
        ok = VRset("EF_MessengerDir","Visible", 1)
        ok = VRset("DT_MessengerDir","Visible", 1)
        ok = VRset("EF_Account","Visible", 1)
        ok = VRSet("DT_Account","Caption","Account name")
        ok = VRset("DT_Account","Visible", 1)
    end
return

/*:VRX         RB_Polarbar_Click
*/
RB_Polarbar_Click: 
    if VRGet("RB_Polarbar","set") then do
        ok = VRset("DDCB_Messenger","Value", 'jre.exe -cp polarbar.zip org.polarbar.mailer')
        Messenger = 'jre.exe -cp polarbar.zip org.polarbar.mailer'
        EMailSearchDirMask = 'polarbar.zip'
        Msg.Type = "W"
        Msg.Text = 'Please, specify the Polarbar directory manually!'
        call _ShowMsg
        ok = VRset("DDCB_Messenger","Visible", 0)
        ok = VRset("DT_Messenger","Visible", 0)
        ok = VRset("PB_SearchMessengerDir","Visible", 1)
        ok = VRset("EF_MessengerDir","Visible", 1)
        ok = VRset("DT_MessengerDir","Caption", "Polarbar directory")
        ok = VRset("DT_MessengerDir","Visible", 1)
        ok = VRset("EF_Account","Visible", 1)
        ok = VRset("DT_Account","Caption", "Outbox directory")
        ok = VRset("DT_Account","Visible", 1)
        call PB_SearchMessengerDir_Click
    end
return
/*:VRX         SPIN_BufferSize_Change
*/
SPIN_BufferSize_Change: 
    BufferSize = VRget("SPIN_BufferSize","value")
return

/*:VRX         TAB_1_Click
*/
TAB_1_Click: 
    call Activate 1
return

/*:VRX         TAB_1_ContextMenu
*/
TAB_1_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         TAB_2_Click
*/
TAB_2_Click: 
    call Activate 2
return

/*:VRX         TAB_2_ContextMenu
*/
TAB_2_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         TAB_3_Click
*/
TAB_3_Click: 
    call Activate 3
return

/*:VRX         TAB_3_ContextMenu
*/
TAB_3_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         TAB_4_Click
*/
TAB_4_Click: 
    call Activate 4
return

/*:VRX         TAB_4_ContextMenu
*/
TAB_4_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         TAB_5_Click
*/
TAB_5_Click: 
    call Activate 5
return

/*:VRX         TAB_5_ContextMenu
*/
TAB_5_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         TAB_6_Click
*/
TAB_6_Click: 
    call Activate 6
return

/*:VRX         TAB_6_ContextMenu
*/
TAB_6_ContextMenu: 
    ok = VRMethod( "Menu_TabMenu", "Popup", , , "", "" )
return

/*:VRX         TM_CheckValid_Trigger
*/
TM_CheckValid_Trigger: 
    ok = VRSet("PB_CreateReport","Enabled",\_CheckInvalidReport())
return
