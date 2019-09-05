# icc3201-201920-tarea1
### Julio Castro - Richard Le May
#### Repositorio base para tarea 1 de Sistemas Operativos y Redes 201920

- **Dificultades encontradas**:
  - Lo que mas nos complico fue el uso del timer, dado que debiamos interrumpir el intento de conexion de los dos procesos, y no supimos como implementarlo.
  - Entender bien como funcionan los comandos necesarios para los sockets, dado que esto nunca lo habiamos hecho antes.
  - En el insert, el problema principal fue el de las key, para que no se repitan las key aleatorias, ya que habian varios casos en donde podian toparse, como por ejemplo, si un random key caia en uno ya existente se debia crear un nuevo key hasta que este no ubiese sido utilizado.

- **Funciones no implementadas**:
  - En el enunciado de la tarea aparece que hay que invocar a `malloc`, lo cual lo encontramos innecesario, dado que sin esta invocacion igual podiamos hacer funcionar el programa, por lo que no lo implementamos.
  - Lo mismo ocurrio con `fork`, dado que no fueron necesarios, no lo implementamos.
  - Al no aparecer que hacer especificamente con el server para que termine de correr, lo dejamos corriendo indefinidamente, y para hacer que termine de correr, se tiene o que cerrar la consola o parar el proceso.

- **Problemas conocidos**:
  - El principal problema fue el de implementar el timer, pensamos implementar un timer antes de la conexion y detenerlo despues de la conexion, y si este superaba los 10sg desconectabamos el socket, pero esto no era lo buscado, por lo que no lo implementamos.
  - Si esta conectado un cliente al servidor, y este se cierra de otra forma que no sea con el comando `quit`, el servidor se queda esperando algun comando de este cliente indefinidamente, y no recibe ningun otro cliente.
