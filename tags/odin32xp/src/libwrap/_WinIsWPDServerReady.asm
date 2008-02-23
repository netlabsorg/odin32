        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   WinIsWPDServerReady:PROC
        PUBLIC  _WinIsWPDServerReady
_WinIsWPDServerReady PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   WinIsWPDServerReady
    pop    fs
    ret
_WinIsWPDServerReady ENDP

CODE32          ENDS

                END
