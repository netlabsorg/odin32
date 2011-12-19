        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinAlarm:PROC
        PUBLIC  _WinAlarm
_WinAlarm PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinAlarm
    add    esp, 8
    pop    fs
    ret
_WinAlarm ENDP

CODE32          ENDS

                END
