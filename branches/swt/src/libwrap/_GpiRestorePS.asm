        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiRestorePS:PROC
        PUBLIC  _GpiRestorePS
_GpiRestorePS PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiRestorePS
    add    esp, 8
    pop    fs
    ret
_GpiRestorePS ENDP

CODE32          ENDS

                END
