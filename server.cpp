#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include <time.h>
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
	int server_fd, new_socket; 
    struct sockaddr_un server_addr; 
    int addrlen = sizeof(server_addr);
    
    string buffer;//string donde se guardaran los mensajes recibidos del cliente
    
    
    int DBcount;//contador de la BD
    srand (time(NULL));
    DBcount = rand()% 9001 + 1000;
	
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
	
	///aqui empieza la comunicaicon inter proceso///
	
	
	while(true)
	{
	buffer.clear();
	cout<<"esperando conexion..."<<endl;
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
		
	int readval;	
	while(strcmp(buffer.c_str(),"desconectado") != 0)
	{
		buffer.clear();
		char tempbuff[1024] = {0};
		if((readval = read(new_socket, tempbuff, 1024)) == 0)//si el read es igua a 0 la conexion termino por una desconexion abrupta
		{
			cout<<"Error de conexion con el cliente."<<endl;
			break;
		}
		buffer = tempbuff;
		if(strcmp(buffer.c_str(),"conectado") == 0)
		{
			cout<<"se ha conectado un cliente"<<endl;
		}
		
		else if(buffer[0]== '1')
		{
			cout<<"insert and generate"<<endl;
			//ejecutar funcion insert con solo el valor
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			
			Value val = { temp.str().length(),temp.str() };
			db.insert(std::pair<unsigned long, Value>(DBcount++, val));//se inserta el value en una key generada
			string msg = "tupla guardada en key: " + to_string(DBcount-1);
			send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
		}
		else if(buffer[0]=='2')
		{
			cout<<"insert"<<endl;
			stringstream temp;
			unsigned long key;
			string value;
			//ejecutar funcion insert con key y value
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				if(buffer[i] != ';' && buffer[i] != ')') temp<<buffer[i];
				else if(buffer[i]==';')
				{
					key = stoul(temp.str(),nullptr,0);
					temp.str(string());
				}
				else if(buffer[i]==')') value = temp.str();
			}
			Value val = { value.length(),value };
			map<unsigned long, Value>::iterator it = db.find(key);
			if ( it == db.end() )
			{
				db.insert(std::pair<unsigned long, Value>(key, val));//se inserta el valor en la key solicitada
			}
			else
			{
				db.erase(it);
				db.insert(std::pair<unsigned long, Value>(key, val));//se inserta el valor en la key solicitada
			}
			string msg = "tupla guardada";
			send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
		}
		else if(buffer[0]=='3')
		{
			cout<<"get"<<endl;	
			//ejecutar funcion get
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			if ( db.find(stoul(temp.str())) == db.end() )
			{
				string msg = "llave solicitada no existe";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else
			{
				string msg = db[stoul(temp.str())].data;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			
		}
		else if(buffer[0]=='4')
		{
			cout<<"peek"<<endl;
			//ejecutar funcion peek
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			
			if ( db.find(stoul(temp.str())) == db.end() ) 
			{
				string msg = "false";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else 
			{
				string msg = "true";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
		}
		else if(buffer[0]=='5')
		{
			cout<<"update: ";
			//ejecutar funcion update
			stringstream temp;
			unsigned long key;
			string value;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				if(buffer[i] != ';' && buffer[i] != ')') temp<<buffer[i];
				else if(buffer[i]==';')
				{
					key = stoul(temp.str(),nullptr,0);
					temp.str(string());
				}
				else if(buffer[i]==')')  value = temp.str();
			}
			map<unsigned long, Value>::iterator it = db.find(key);
			if ( it == db.end() ) 
			{
				string msg = "la tupla no existe";
				cout<<"tupla solicitada inexistente"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else  
			{
				it->second = {value.length(),value}; 
				string msg = "tupla actualizada";
				cout<<"exito en la operacion"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
		}
		else if(buffer[0]=='6')
		{
			cout<<"delete: "<<endl;
			//ejecutar funcion delete
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			
			if (db.find(stoul(temp.str())) == db.end() ) 
			{
				string msg = "la key no existe";
				cout<<"key solicitada inexistente"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else  
			{
				db.erase(stoul(temp.str()));
				string msg = "tupla borrada";
				cout<< "exito en la operacion"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
		}
		else if(strcmp(buffer.c_str(),"list")== 0)
		{
			//ejecutar funcion list
			cout<<" cliente ejecuto funcion: list()"<<endl;
			// Imprimir lo que hemos agregado al mapa KV.
			for(map<unsigned long,Value>::iterator it = db.begin(); it != db.end(); ++it) 
			{
				string msg = to_string(it->first)+"\n";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			string msg = "fin de la lista\n";
			send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
		}
	}
	cout<<"conexion terminada"<<endl;
	}
	
	return 0;
}
