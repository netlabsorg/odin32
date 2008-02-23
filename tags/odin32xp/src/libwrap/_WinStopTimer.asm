        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinStopTimer:PROC
        PUBLIC  _WinStopTimer
_WinStopTimer PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   WinStopTimer
    add    esp, 12
    pop    fs
    ret
_WinStopTimer ENDP

CODE32          ENDS

                END
