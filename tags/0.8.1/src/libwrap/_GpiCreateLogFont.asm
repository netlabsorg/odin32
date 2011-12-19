        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiCreateLogFont:PROC
        PUBLIC  _GpiCreateLogFont
_GpiCreateLogFont PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiCreateLogFont
    add    esp, 16
    pop    fs
    ret
_GpiCreateLogFont ENDP

CODE32          ENDS

                END
