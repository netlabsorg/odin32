        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiAssociate:PROC
        PUBLIC  _GpiAssociate
_GpiAssociate PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiAssociate
    add    esp, 8
    pop    fs
    ret
_GpiAssociate ENDP

CODE32          ENDS

                END
