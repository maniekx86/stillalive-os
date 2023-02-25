bits 16
    db 0x55, 0xAA ; signature
    db 32768/512; initialization size in 512 byte blocks

load_addr equ 0x800

    jmp _init
_init:
    cld
    mov si, 512
    xor di,di ;kuda
    mov cx,load_addr
    mov es,cx
    mov ax,cs
    mov ds,ax
    mov cx, 32768-512

    rep
    movsw

; inicjalizacja steka (0000:fffe) i segmentu dannyh

;    mov ax,0xfffe
;    mov sp,ax
;    xor ax,ax
;    mov ss,ax

;    mov ax,load_addr
;    mov ds,ax

    jmp load_addr:0000
