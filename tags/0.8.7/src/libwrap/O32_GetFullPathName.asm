        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_GetFullPathName:PROC
        PUBLIC  O32_GetFullPathName
O32_GetFullPathName PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    push   dword ptr [esp+20]
    call   _O32_GetFullPathName
    add    esp, 16
    pop    fs
    ret
O32_GetFullPathName ENDP

CODE32          ENDS

                END
