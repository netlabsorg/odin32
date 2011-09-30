        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_SetActiveWindow:PROC
        PUBLIC  O32_SetActiveWindow
O32_SetActiveWindow PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_SetActiveWindow
    add    esp, 4
    pop    fs
    ret
O32_SetActiveWindow ENDP

CODE32          ENDS

                END
