.model small
.stack 256

.data
msg db "Input 2 hex numbers: $"

.code
main:
    ; Init ds
    mov ax, @data
    mov ds, ax

    ; Output Message
    mov ah, 9
    mov dx, offset msg
    int 21h

    ; Clean ax and dl
    xor ax, ax
    xor dl, dl

    ; Input character
    mov ah, 1
    int 21h

    ; If below ASCII-code of '0'
    cmp al, 30h
    jl inputSecond

    ; If lower or equal ASCII-code of '9'
    cmp al, 39h
    jle number_1

    ; Transform ASCII-code of capital letter to HEX
    sub al, 37h
    jmp inputSecond
number_1:
    ; Transform ASCII-code of number to HEX
    sub al, 30h

inputSecond:
    ; Move HEX to dl
    mov dl, al

    ; Input character
    int 21h

    ; If below ASCII-code of '0'
    cmp al, 30h
    jl exit

    ; Move first HEX-character to higher nimble
    shl dl, 4

    ; If lower or equal ASCII-code of '9'
    cmp al, 39h
    jle number_2

    ; Transform ASCII-code of capital letter to HEX
    sub al, 37h
    jmp exit
number_2:
    ; Transform ASCII-code of number to HEX
    sub al, 30h

exit:
    ; Move second HEX-character to lower nimble of dl
    or dl, al

    ; Exit
    mov ax, 4c00h
    int 21h
end main
