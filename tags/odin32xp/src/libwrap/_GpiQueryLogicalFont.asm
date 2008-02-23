        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryLogicalFont:PROC
        PUBLIC  _GpiQueryLogicalFont
_GpiQueryLogicalFont PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryLogicalFont
    add    esp, 20
    pop    fs
    ret
_GpiQueryLogicalFont ENDP

CODE32          ENDS

                END
