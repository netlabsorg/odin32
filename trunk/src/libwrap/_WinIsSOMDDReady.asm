        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsSOMDDReady:PROC
        PUBLIC  _WinIsSOMDDReady
_WinIsSOMDDReady PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   WinIsSOMDDReady
    pop    fs
    ret
_WinIsSOMDDReady ENDP

CODE32          ENDS

                END
