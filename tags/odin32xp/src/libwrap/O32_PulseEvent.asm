        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   _O32_PulseEvent:PROC
        PUBLIC  O32_PulseEvent
O32_PulseEvent PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    push   dword ptr [esp+8]
    call   _O32_PulseEvent
    add    esp, 4
    pop    fs
    ret
O32_PulseEvent ENDP

CODE32          ENDS

                END
