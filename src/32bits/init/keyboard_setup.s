; rutina de inicializacion del teclado para trabajar con interrupciones

USE32
section .kernel32

GLOBAL __keyboard_Setup__

__keyboard_Setup__:
        call .wait_clear_to_send
        mov al, 0xAD
        out 0x64, al

        ; Disable second PS/2 port
        call .wait_clear_to_send
        mov al, 0xA7
        out 0x64, al
        
        ; Read port to flush it
        in   al, 0x60

        ; Request the status byte
        call .wait_clear_to_send
        mov al, 0x20
        out 0x64, al

        ; Read status byte
        call .wait_clear_to_read
        in  al, 0x60
        and al, 10111100b; Clear bits 0,1 and 6
        push ax

        ; Request to send new status
        call .wait_clear_to_send
        mov al, 0x60
        out 0x64, al

        ; Send new status
        call .wait_clear_to_send
        mov ax, [esp] 
        out 0x60, al

        ; Perform self-test on Controller 1
        call .wait_clear_to_send
        mov al, 0xAA
        out 0x64, al
        call .wait_clear_to_read
        in al, 0x60
        cmp al, 0x55
        jne .halt

        ; Perform self-test on Controller 2
        mov al, 0xAB
        out 0x64, al
        call .wait_clear_to_read
        in al, 0x60
        cmp al, 0x00
        jne .halt

        ; enable port
        mov al, 0xAE
        out 0x64, al

        ; get the status sent before from the stack and enable interrupt
        mov al, 0x60; Request to send new status
        out 0x64, al
        call .wait_clear_to_send
        pop ax
        or  al, 0x01 ; Set interrupt but to 1 (enabled)
        out 0x60, al; Send new status

        call .wait_clear_to_send
        mov al, 0xFF
        out 0x60, al
        call .wait_clear_to_read
        in al, 0x60
        cmp al, 0xFC; if it is 0xFC there was an error
        je .halt
        ret

    .wait_clear_to_send: ; wait until 0x64 or 0x60 is ready to receive information
        in   al, 0x64
        test al, 0x02
        jne .wait_clear_to_send
        ret;

    .wait_clear_to_read: ; wait until 0x64 has information to read from it
        in   al, 0x64
        test al, 0x01
        je .wait_clear_to_read
        ret;

    .halt: 
        hlt
        jmp .halt
