        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiQueryRealColors:PROC
        PUBLIC  _GpiQueryRealColors
_GpiQueryRealColors PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   GpiQueryRealColors
    add    esp, 20
    pop    fs
    ret
_GpiQueryRealColors ENDP

CODE32          ENDS

                END
