        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   DosUnwindException:PROC
        PUBLIC  _DosUnwindException
_DosUnwindException PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   DosUnwindException
    add    esp, 12
    pop    fs
    ret
_DosUnwindException ENDP

CODE32          ENDS

                END
