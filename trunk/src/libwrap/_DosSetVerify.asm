        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosSetVerify:PROC
        PUBLIC  _DosSetVerify
_DosSetVerify PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosSetVerify
    add    esp, 4
    pop    fs
    ret
_DosSetVerify ENDP

CODE32          ENDS

                END