        .386p
CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'

        EXTRN   Win32QueryClipbrdViewerChain:PROC
        PUBLIC  _Win32QueryClipbrdViewerChain
_Win32QueryClipbrdViewerChain PROC NEAR
    push   fs
    mov    eax, 0150bh
    mov    fs, eax
    call   Win32QueryClipbrdViewerChain
    pop    fs
    ret
_Win32QueryClipbrdViewerChain ENDP

CODE32          ENDS

                END
