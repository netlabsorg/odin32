        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplStdDelete:PROC
        PUBLIC  _SplStdDelete
_SplStdDelete PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   SplStdDelete
    add    esp, 4
    pop    fs
    ret
_SplStdDelete ENDP

CODE32          ENDS

                END
