        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinTerminate:PROC
        PUBLIC  _WinTerminate
_WinTerminate PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   WinTerminate
    add    esp, 4
    pop    fs
    ret
_WinTerminate ENDP

CODE32          ENDS

                END
