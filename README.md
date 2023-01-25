Proyecto: tp1-ej15
Descripción:

Entrega del tp1.
Para correr el programa se cuenta con dos opciones de build: "make all" y "make debug". Es necesario hacer "make clean" al cambiar de modo
de compilacion para poder ver los cambios correctamente.
make debug: define flags que por compilación condicional agregan breakpoints y más información en el monitor del emulador para así poder
analizar más facilmente la ejecucion de las tareas. Muestra que tarea está en ejecución, el valor del system tick y se detiene por cada tick del
sistema. Además se puede apretar la tecla R(run) para ignorar los breakpoints una vez pasada la inicialización (puede tomar más de 1 continue en tomar acción) y luego apretar la tecla S(stop) cuando esta corriendo libremente para volver a activar los breakpoints. Aclaración: el punto en el que para es cuando se imprime la nueva tarea a ejecutar, el contexto todavía no ha cambiado en esa instancia.
make all: todas las variables e instrucciones relacionadas a debug no aparecen en el código, por lo que corre libremente, tampoco imprime información relacionada con la ejecución del sistema.
make bochs: levanta el emulador con la configuración local del directorio y en modo visual.

Interfaz:
Una vez pasada la inicalización se verán en pantalla todas las instrucciones necesarias para usar el sistema. Al ingresar caracteres hexa de 0 a F se irá formando un número de 64 bits (de forma circular) que se puede ver debajo a la izquierda y al presionar enter dicho numero se agrega a la tabla de digitos donde las tareas de suma realizan la suma de digitos. En el centro de la pantalla figuran los resultados de ambas variantes: suma 1 no saturada en tamaño quadword y suma 2 saturada en tamaño word. Además debajo del resultado 1 está el valor de la memoria RAM en la dirección del valor igual al resultado 1 (suponiendo que dicho valor no exceda la RAM del sistema). Por último abajo a la izquierda se indican la teclas para producir excepciones intencionalemente:
O-#GP Y-#DE U-#UD I-#DF
las cuales generar un error en pantalla, hacen un "cli" y ponen el sistema en hlt (es decir hay que reiniciar el sistema).

Alumno: Martín Pereira Nuñez Machado
Legajo: 152258-9
Email: martinpereira.n@gmail.com
Slack: Martin Pereira (R5054)