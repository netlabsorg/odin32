        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_EnumFonts:PROC
        PUBLIC  O32_EnumFonts
O32_EnumFonts PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   _O32_EnumFonts
    add    esp, 16
    pop    fs
    ret
O32_EnumFonts ENDP

CODE32          ENDS

                END
