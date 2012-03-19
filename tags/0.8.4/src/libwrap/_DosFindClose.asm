        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosFindClose:PROC
        PUBLIC  _DosFindClose
_DosFindClose PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosFindClose
    add    esp, 4
    pop    fs
    ret
_DosFindClose ENDP

CODE32          ENDS

                END
