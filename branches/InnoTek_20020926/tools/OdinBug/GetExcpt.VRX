/*:VRX         Main
*/
Main:
    signal on halt
    ok = VRRedirectSTdIO('OFF')
    parse arg LogFilename

    ExceptFilename = filespec('D',LogFileName)||filespec('P',LogFileName)||'Except.$$$'

    ok = SysFileDelete(Exceptfilename)
    I = 0
    do while pos('Exception Information',LogLine) = 0 & lines(LogFileName) <> 0
        LogLine = linein(LogFileName)
        idx = I // 10 + 1
        Buffer.idx = LogLine
        I = I + 1
    end
    if lines(LogFileName) = 0 then do
        call lineout ExceptFileName, 'No exception found in logfile!'
    end
    else do /* we had an exception lets create the dump */
        do J = idx+1 to 10
            call lineout ExceptFileName, Buffer.J
        end
        do J = 1 to idx
            call lineout ExceptFileName, Buffer.J
        end
        LogLine = linein(LogFileName)
        do until pos('End Of Exception Information',LogLine) > 0 | lines(LogFileName) = 0
            LogLine = linein(LogFileName)
            call lineout ExceptFileName, LogLine
        end

        do while pos('** BEGIN STACK DUMP **',LogLine) = 0 & lines(LogFileName) <> 0
            LogLine = linein(LogFileName)
            I = I + 1
        end 
        call lineout ExceptFileName, LogLine
        do until pos('** END OF STACK DUMP **',LogLine) > 0 | lines(LogFileName) = 0
            LogLine = linein(LogFileName)
            call lineout ExceptFileName,LogLine
        end
    end
    call lineout(ExceptFileName)
    call lineout(LogFileName)
    call halt
exit

/*:VRX         Halt
*/
Halt:

exit

