        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetTabbedTextExtent:PROC
        PUBLIC  O32_GetTabbedTextExtent
O32_GetTabbedTextExtent PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    push   dword ptr [esp+24]
    call   _O32_GetTabbedTextExtent
    add    esp, 20
    pop    fs
    ret
O32_GetTabbedTextExtent ENDP

CODE32          ENDS

                END
