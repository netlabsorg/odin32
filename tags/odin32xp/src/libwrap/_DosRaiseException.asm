        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosRaiseException:PROC
        PUBLIC  _DosRaiseException
_DosRaiseException PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   DosRaiseException
    add    esp, 4
    pop    fs
    ret
_DosRaiseException ENDP

CODE32          ENDS

                END
