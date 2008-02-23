        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosQueryVerify:PROC
        PUBLIC  _DosQueryVerify
_DosQueryVerify PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosQueryVerify
    add    esp, 4
    pop    fs
    ret
_DosQueryVerify ENDP

CODE32          ENDS

                END
