        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinWakeThread:PROC
        PUBLIC  _WinWakeThread
_WinWakeThread PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinWakeThread
    add    esp, 4
    pop    fs
    ret
_WinWakeThread ENDP

CODE32          ENDS

                END
