# icc3201-201920-tarea1
### Julio Castro - Richard Le May
#### Repositorio base para tarea 1 de Sistemas Operativos y Redes 201920

- **Dificultades encontradas**:
  - la dificultad mas grande de esta entrega fue entender bien como funcionan los lock para lectura y escritura, dado que esto nunca lo habiamos hecho antes.
  - tambien se hizo tedioso el traspaso de gran parte del codigo implementado en la parte anterior a una funcion que no ocupe muchos parametros, ya que segun investigamos, para pasar muchos parametros a una funcion que queremos utilizar como thread debemos utilizar una estructura de datos que contenga dichos parametros, lo cual hace que el traspaso de condigo a funcion no sea tan trivial.
  - Otra dificultad encontrada, fue la de poder probar efectivamente que los locks implementados resolvian el problema de la seccion critica, ya que no sabiamos como ejecutar varios clientes simultaneamente mientras estos ejecutaban comandos que generaran condiciones de carrera.

- **Funciones no implementadas**:
  - El server lo dejamos corriendo indefinidamente, pero se nos dijo que no era necesario cerrarlo mediante comandos del server, de igual manera lo consideramos como una funcion no implementada.
  - no se implemento el timer de 10 segundos para el error de timeout del cliente, en parte por que no sabemos como hacerlo y por que no entendemos bien como deberia funcionar.

- **Problemas conocidos**:
  - el unico bug que se nos presento durante la realizacion de la tarea es que, bajo ciertas circunstancias, el server se cierra si es que un cliente se desconecta de manera abrupta(osea si se hace quit o se cierra la consola sin haber hecho el comando *disconnect* antes), desgraciadamente no hemos podido replicar dicho error de manera consistente, a tal punto de que no sabemos si el error todavia existe.
