;Defino constante del kernel con valor inicial (arrancan en ROM y se copian a RAM)

USE32
section .initialised_data

GLOBAL  WelcomeLine0
GLOBAL  WelcomeLine1
GLOBAL  WelcomeLine2
GLOBAL  WelcomeLine3
GLOBAL  WelcomeLine4
GLOBAL  WelcomeLine5
GLOBAL  WelcomeLine6
GLOBAL  DevideLine
GLOBAL  InstructionsLine
GLOBAL  DebugLine
GLOBAL  ExceptionsInstructions

ALIGN 32

WelcomeLine0:       db      '                 ______  ____       __       _____   ____                       '
WelcomeLine1:       db      "                /\__  _\/\  _`\   /'__`\    /\  __`\/\  _`\                     "
WelcomeLine2:       db      '                \/_/\ \/\ \ \/\ \/\_\L\ \   \ \ \/\ \ \,\L\_\                   '
WelcomeLine3:       db      '                   \ \ \ \ \ \ \ \/_/_\_<_   \ \ \ \ \/_\__ \                   '
WelcomeLine4:       db      '                    \ \ \ \ \ \_\ \/\ \L\ \   \ \ \_\ \/\ \L\ \                 '
WelcomeLine5:       db      '                     \ \_\ \ \____/\ \____/    \ \_____\ `\____\                '
WelcomeLine6:       db      '                      \/_/  \/___/  \/___/      \/_____/\/_____/                '
DevideLine:         db      '################################################################################'
InstructionsLine:   db      '#  Ingrese caracteres hexadecimales (0 a F) y presione ENTER para agregarlos   #'
DebugLine:          db      '#    MODO DEBUG: Presione R para ignorar breakpoints y S para reactivarlos     #'
ExceptionsInstructions: db  ' Para generar excepciones presione: O-#GP Y-#DE U-#UD I-#DF                     '
