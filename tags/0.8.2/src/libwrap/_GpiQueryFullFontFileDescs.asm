        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryFullFontFileDescs:PROC
        PUBLIC  _GpiQueryFullFontFileDescs
_GpiQueryFullFontFileDescs PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryFullFontFileDescs
    add    esp, 20
    pop    fs
    ret
_GpiQueryFullFontFileDescs ENDP

CODE32          ENDS

                END
