        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   SplQmAbortDoc:PROC
        PUBLIC  _SplQmAbortDoc
_SplQmAbortDoc PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   SplQmAbortDoc
    add    esp, 4
    pop    fs
    ret
_SplQmAbortDoc ENDP

CODE32          ENDS

                END
