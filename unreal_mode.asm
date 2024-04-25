; maniek86 2024 (c)
;
; Used to enable unreal mode for VESA (mode switching)
;
; NASM generates here pure 16 bit code and GCC generates 16 bit code but instructions generated have 32 bit prefix
; because of that we have to use "db" to create "ret" with 32 bit prefix for functions that are called from C
; this is not needed if functions are called from assembly (see enable_A20) 
;
; 

BITS 16

global _gounreal
global _setmem  ; needed functions by us, they allow us for accesing VBE info memory using unreal mode
global _setmemw
global _getmem
global _getmemw


section .data

section .text


; source: https://wiki.osdev.org/A20_Line
; this should be enough to be compatible with most hardware
enable_A20: 
    mov     ax,2403h                ;--- A20-Gate Support ---
    int     15h
    jb      enable_A20_kb           ;INT 15h is not supported
    cmp     ah,0
    jnz     enable_A20_kb           ;INT 15h is not supported
     
    mov     ax,2402h                ;--- A20-Gate Status ---
    int     15h
    jb      a20_failed              ;couldn't get status
    cmp     ah,0
    jnz     a20_failed              ;couldn't get status
     
    cmp     al,1
    jz      a20_activated           ;A20 is already activated
     
    mov     ax,2401h                ;--- A20-Gate Activate ---
    int     15h
    jb      a20_failed              ;couldn't activate the gate
    cmp     ah,0
    jnz     a20_failed              ;couldn't activate the gate
    
    ; A20 gate activated
    ret
    
a20_failed:
    jmp enable_A20_kb ; try the original method

a20_activated: 
    ret

enable_A20_kb:  ; Original method to enable the A20 line 
        cli
 
        call    a20wait
        mov     al,0xAD
        out     0x64,al
 
        call    a20wait
        mov     al,0xD0
        out     0x64,al
 
        call    a20wait2
        in      al,0x60
        push    eax
 
        call    a20wait
        mov     al,0xD1
        out     0x64,al
 
        call    a20wait
        pop     eax
        or      al,2
        out     0x60,al
 
        call    a20wait
        mov     al,0xAE
        out     0x64,al
 
        call    a20wait
        sti
        ret 
 
a20wait:
        in      al,0x64
        test    al,2
        jnz     a20wait
        ret
 
 
a20wait2:
        in      al,0x64
        test    al,1
        jz      a20wait2
        ret
        

_gounreal:   
    call enable_A20
    
    xor ax, ax       ; make it zero
    mov ds, ax             ; DS=0
    ; mov ss, ax             ; stack starts at seg 0
    ; mov sp, 0x9c00         ; 2000h past code start, 

    
 
   cli                    ; no interrupts
   push ds                ; save real mode
   
 
   lgdt [gdtinfo]         ; load gdt register
 
   mov  eax, cr0          ; switch to pmode by
   or al,1                ; set pmode bit
   mov  cr0, eax
   jmp 0x8:pmode

pmode:
   mov  bx, 0x10          ; select descriptor 2, instead of 1
   mov  ds, bx            ; 10h = 10000b
    
   and al,0xFE            ; back to realmode
   mov  cr0, eax          ; by toggling bit again
   
   jmp 0x0:huge_unreal

huge_unreal:
   pop ds                 ; get back old segment
   sti
    
   ;mov bx, 0x0f01         ; attrib/char of smiley
   ;mov eax, 0x0b8000      ; note 32 bit offset
   ;mov word [ds:eax], bx
 
   db 0x66, 0xc3 ; retl

_getmem:
    push ebp

    mov     ebp, esp
    
    mov     eax, [ebp+8]

    mov byte al, [ds:eax]


    pop     ebp
    db 0x66, 0xc3 ; retl

_getmemw:
    push ebp

    mov     ebp, esp
    mov     eax, [ebp+8]

    mov word eax, [ds:eax]

    pop ebp
    db 0x66, 0xc3 ; retl

_setmem:
    push ebp
    mov     ebp, esp
    push    ebx
    
    mov     eax, [ebp+8]
    mov     bx, [ebp+12]

    mov byte [ds:eax], bl

    pop     ebx
    pop     ebp

    db 0x66, 0xc3 ; retl
 

_setmemw:
    push ebp
    mov     ebp, esp
    push    ebx
    
    mov     eax, [ebp+8]
    mov     bx, [ebp+12]

    mov word [ds:eax], bx

    pop     ebx
    pop     ebp

    db 0x66, 0xc3 ; retl
 

gdtinfo:
   dw gdt_end - gdt - 1   ;last byte in table
   dd gdt                 ;start of table
 
gdt         dd 0,0        ; entry 0 is always unused
flatcode    db 0xff, 0xff, 0, 0, 0, 10011010b, 10001111b, 0
flatdata    db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:
gdt_start:

