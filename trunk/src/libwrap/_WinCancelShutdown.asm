        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinCancelShutdown:PROC
        PUBLIC  _WinCancelShutdown
_WinCancelShutdown PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinCancelShutdown
    add    esp, 8
    pop    fs
    ret
_WinCancelShutdown ENDP

CODE32          ENDS

                END
