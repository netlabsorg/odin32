        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiSetAttrMode:PROC
        PUBLIC  _GpiSetAttrMode
_GpiSetAttrMode PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+12]
    push   dword ptr [esp+12]
    call   GpiSetAttrMode
    add    esp, 8
    pop    fs
    ret
_GpiSetAttrMode ENDP

CODE32          ENDS

                END
