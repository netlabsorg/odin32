        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinSetCapture:PROC
        PUBLIC  _WinSetCapture
_WinSetCapture PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   WinSetCapture
    add    esp, 8
    pop    fs
    ret
_WinSetCapture ENDP

CODE32          ENDS

                END
