        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   GpiTranslate:PROC
        PUBLIC  _GpiTranslate
_GpiTranslate PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   GpiTranslate
    add    esp, 16
    pop    fs
    ret
_GpiTranslate ENDP

CODE32          ENDS

                END
