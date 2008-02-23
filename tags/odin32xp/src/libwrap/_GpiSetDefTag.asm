        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetDefTag:PROC
        PUBLIC  _GpiSetDefTag
_GpiSetDefTag PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetDefTag
    add    esp, 8
    pop    fs
    ret
_GpiSetDefTag ENDP

CODE32          ENDS

                END
