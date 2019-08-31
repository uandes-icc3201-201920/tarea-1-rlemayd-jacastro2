#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include "util.h"

#include <arpa/inet.h>
#define DIR "/tmp/db.tuples.sock"// direccion predeterminada del socket

using namespace std;

// Almacenamiento KV
KVStore db;

int main(int argc, char** argv) {	
	
	int sflag = 0;
	int opt;

	
	//definimos los parametros necesarios para crear el server
	string sock_dir;//direccion que ingresara el usuario
	int abc = 1;
	int server_fd, new_socket, valread; 
    struct sockaddr_un server_addr; 
    int addrlen = sizeof(server_addr);
    
    char buffer[1024] = {0};
    
    
	
	// Procesar opciones de linea de comando
    while ((opt = getopt (argc, argv, "s:")) != -1) {
        switch (opt)
		{
			/* Procesar el flag s si el usuario lo ingresa */
			case 's':
				sflag = 1;
				cout<<"socket a utilizar: "<<optarg<<endl;
				sock_dir = optarg;
				break;
			default:
				return EXIT_FAILURE;
          }	    	
    }
    
    //des-linkeamos el socket que utilizaremos(por si se esta utilizando antes de hacer bind)
    if(sflag==1)
    {
    	unlink(sock_dir.c_str());
    	strcpy(server_addr.sun_path,sock_dir.c_str());
    }
    else
    {
    	unlink(DIR);
    	strcpy(server_addr.sun_path,DIR);
    }
    
    //creamos el descriptor del socket
    if((server_fd = socket(AF_UNIX, SOCK_STREAM,0))==0)
    {
    	perror("fallo el socket");
    	exit(EXIT_FAILURE);
    }
    
    //conectamos el socket a la direccion
    if(setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &abc, sizeof(abc)))
    {
    	perror("error de setsock");
    	exit(EXIT_FAILURE);
    }
    
    server_addr.sun_family = AF_UNIX;
    
    
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)
    {
    	perror("error de bindeo");
    	exit(EXIT_FAILURE);
    }
    
	cout<<"servidor creado"<<endl;
	if(listen(server_fd, 3)<0)
		{
			perror("error de funcion listen");
			exit(EXIT_FAILURE);
		}
		
	if((new_socket = accept(server_fd, (struct sockaddr *)&server_addr,(socklen_t*)&addrlen))<0)
		{
			perror("error de accept");
			exit(EXIT_FAILURE);
		}
		
	while(true)
	{
		
		valread = read(new_socket, buffer, 1024);
		printf("%s\n",buffer);
		
		
	}
	
	
	
	
	
	// Uso elemental del almacenamiento KV:
	
	// Creamos un arreglo de bytes a mano
	byte data[] = { 0x01, 0x01, 0x01, 0x01, 0x01 };

	// Luego un vector utilizando el arreglo de bytes
	vector<byte> vdata(data, data + sizeof(data));
	
	// Creamos el valor
	Value val = { 5, vdata };
	
	// Insertamos un par clave, valor directamente
	// en el mapa KV
	
	// Nota: Debiera diseñarse una solución más robusta con una interfaz
	// adecuada para acceder a la estructura.
	db.insert(std::pair<unsigned long, Value>(1000, val));
		
	// Imprimir lo que hemos agregado al mapa KV.
	cout << db[1000].size << " " << (int) db[1000].data[0] << endl;
	
	return 0;
}
