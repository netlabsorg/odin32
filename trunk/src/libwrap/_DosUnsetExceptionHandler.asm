        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosUnsetExceptionHandler:PROC
        PUBLIC  _DosUnsetExceptionHandler
_DosUnsetExceptionHandler PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosUnsetExceptionHandler
    add    esp, 4
    pop    fs
    ret
_DosUnsetExceptionHandler ENDP

CODE32          ENDS

                END
