        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryColorData:PROC
        PUBLIC  _GpiQueryColorData
_GpiQueryColorData PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    push   dword ptr [esp+16]
    call   GpiQueryColorData
    add    esp, 12
    pop    fs
    ret
_GpiQueryColorData ENDP

CODE32          ENDS

                END
