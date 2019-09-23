# icc3201-201920-tarea1
### Julio Castro - Richard Le May
#### Repositorio base para tarea 1 de Sistemas Operativos y Redes 201920

- **Dificultades encontradas**:
  - Para esta entrega, no pudimos implementar el timer ya que para la pasada tampoco pudimos, y no sabemos bien como funciona.
  - Entender bien como funcionan los lock para lectura y escritura, dado que esto nunca lo habiamos hecho antes.

- **Funciones no implementadas**:
  - En el enunciado de la tarea aparece que hay que invocar a `malloc`, lo cual lo encontramos innecesario, dado que sin esta invocacion igual podiamos hacer funcionar el programa, por lo que no lo implementamos.
  - Lo mismo ocurrio con `fork`, dado que no fueron necesarios, no lo implementamos.
  - El server lo dejamos corriendo indefinidamente, y para hacer que termine de correr, se tiene o que cerrar la consola o parar el proceso.

- **Problemas conocidos**:
  - El principal problema fue el de implementar el timer, pensamos implementar un timer antes de la conexion y detenerlo despues de la conexion, y si este superaba los 10sg desconectabamos el socket, pero esto no era lo buscado, por lo que no lo implementamos.
