        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_BeginPath:PROC
        PUBLIC  O32_BeginPath
O32_BeginPath PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_BeginPath
    add    esp, 4
    pop    fs
    ret
O32_BeginPath ENDP

CODE32          ENDS

                END
