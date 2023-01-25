USE32
section .start32

GLOBAL __reprogramPICs_

; ReprogamarPICs
; brief Reprograma la base de interrupciones de los PICs
; details Corre la base de los tipos de interrupcion de ambos PICs 8259A de la PC a los 8 tipos consecutivos a partir de los valores base que recibe en BH para el PIC#1 y BL para el PIC#2.A su retorno las Interrupciones de ambos PICs est�n deshabilitadas.
__reprogramPICs_:
;// Inicializacion PIC Nr1
  ;//ICW1:
  mov     al,0x11;//Establece IRQs activas x flanco, Modo cascada, e ICW4
  out     0x20,al
  ;//ICW2:
  mov     al,0x20   ;//Establece para el PIC#1 el valor base del Tipo de INT que recibio en el registro BH = 0x20
  out     0x21,al
  ;//ICW3:
  mov     al,0x04;//Establece PIC#1 como Master, e indica que un PIC Slave cuya Interrupcion ingresa por IRQ2
  out     0x21,al
  ;//ICW4
  mov     al,0x01;// Establece al PIC en Modo 8086
  out     0x21,al
;//Antes de inicializar el PIC Nr2, deshabilitamos las Interrupciones del PIC1
  mov     al,0xFF   ;0x11111101
  out     0x21,al
;//Ahora inicializamos el PIC Nr2
  ;//ICW1
  mov     al,0x11;//Establece IRQs activas x flanco, Modo cascada, e ICW4
  out     0xA0,al
  ;//ICW2
  mov     al,0x28    ;//Establece para el PIC#2 el valor base del Tipo de INT que recibi� en el registro BL = 0x28
  out     0xA1,al
  ;//ICW3
  mov     al,0x02;//Establece al PIC#2 como Slave, y le indca que ingresa su Interrupci�n al Master por IRQ2
  out     0xA1,al
  ;//ICW4
  mov     al,0x01;// Establece al PIC en Modo 8086
  out     0xA1,al
;Enmascaramos el resto de las Interrupciones (las del PIC#2)
  mov     al,0xFF
  out     0xA1,al
  ret