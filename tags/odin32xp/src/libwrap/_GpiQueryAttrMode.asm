        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryAttrMode:PROC
        PUBLIC  _GpiQueryAttrMode
_GpiQueryAttrMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   GpiQueryAttrMode
    add    esp, 4
    pop    fs
    ret
_GpiQueryAttrMode ENDP

CODE32          ENDS

                END
