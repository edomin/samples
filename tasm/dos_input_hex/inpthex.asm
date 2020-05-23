.model small
.stack 256

.data
msg    db "Input 2 hex numbers: $"
decode db 48 dup(0)
       db 0h, 1h, 2h, 3h, 4h, 5h, 6h, 7h, 8h, 9h, 7 dup(0)
       db 0Ah, 0Bh, 0Ch, 0Dh, 0Eh, 0Fh, 26 dup(0)
       db 0Ah, 0Bh, 0Ch, 0Dh, 0Eh, 0Fh, 133 dup(0)

.code
main:
    ; Init ds
    mov ax, @data
    mov ds, ax

    ; Output Message
    mov ah, 9
    mov dx, offset msg
    int 21h

    ; Clean ax
    xor ax, ax

    ; Input character
    mov ah, 1
    int 21h

    ; Move decode table address to bx
    lea bx, decode
    ; Decode
    xlat

    ; Move first HEX-character to dl
    mov dl, al

    ; Move first HEX-character to higher nimble
    shl dl, 4

    ; Input character
    int 21h

    ; Decode
    xlat

    ; Move second HEX-character to lower nimble of dl
    or dl, al

    ; Exit
    mov ax, 4c00h
    int 21h
end main
