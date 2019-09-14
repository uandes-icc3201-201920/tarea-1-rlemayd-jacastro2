#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include <time.h>
#include "util.h"
#include <pthread.h>
#include <arpa/inet.h>


#define DIR "/tmp/db.tuples.sock"// direccion predeterminada del socket

using namespace std;

// Almacenamiento KV
KVStore db;
int DBcount;//contador de la BD
string buffer;//string donde se guardaran los mensajes recibidos del cliente

void* server_cliente(void* sock)
{
	int new_socket = *((int*)sock);
	int readval;	
	while(strcmp(buffer.c_str(),"desconectado") != 0)
	{	
		buffer.clear();
		char tempbuff[1024] = {0};
		if((readval = read(new_socket, tempbuff, 1024)) == 0)//si el read es igual a 0 la conexion termino por una desconexion abrupta
		{
			cout<<"Error de conexion con el cliente."<<endl;
			break;
		}
		buffer = tempbuff;
		if(strcmp(buffer.c_str(),"conectado") == 0)
		{
			cout<<"Se ha conectado el cliente numero "<<new_socket-3<<endl;
		}
		
		else if(buffer[0]== '1')
		{
			string msg;
			cout<<"Cliente a solicitado funcion \"insert and generate\""<<endl;
			//ejecutar funcion insert con solo el valor
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			
			Value val = { temp.str().length(),temp.str() };
			map<unsigned long, Value>::iterator it = db.find(DBcount);
			while(it != db.end())
			{
				DBcount++;
				it = db.find(DBcount);
			}
			db.insert(std::pair<unsigned long, Value>(DBcount++, val));//se inserta el valor en la key solicitada
			msg =  "Tupla guardada en key: " + to_string(DBcount-1);
			
			send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
		}
		else if(buffer[0]=='2')
		{
			cout<<"Cliente a solicitado funcion \"insert\""<<endl;
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
			string msg;
			if ( it == db.end() )
			{
				db.insert(std::pair<unsigned long, Value>(key, val));//se inserta el valor en la key solicitada
				msg = "Tupla guardada";
			}
			else
			{
				msg = "No se va a insertar el valor porque el key ya esta utilizado con otro valor.";
			}
			
			send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
		}
		else if(buffer[0]=='3')
		{
			cout<<"Cliente a solicitado funcion \"get\""<<endl;	
			//ejecutar funcion get
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			if ( db.find(stoul(temp.str())) == db.end() )
			{
				string msg = "Llave solicitada no existe";
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
			cout<<"Cliente a solicitado funcion \"peek\""<<endl;
			//ejecutar funcion peek
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			
			if ( db.find(stoul(temp.str())) == db.end() ) 
			{
				string msg = "False";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else 
			{
				string msg = "True";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
		}
		else if(buffer[0]=='5')
		{
			cout<<"Cliente a solicitado funcion \"update\"\n";
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
				string msg = "La tupla no existe";
				cout<<"\tTupla solicitada por el cliente es inexistente"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else  
			{
				it->second = {value.length(),value}; 
				string msg = "Tupla actualizada";
				cout<<"\tExito en la operacion"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
		}
		else if(buffer[0]=='6')
		{
			cout<<"Cliente a solicitado funcion \"delete\""<<endl;
			//ejecutar funcion delete
			stringstream temp;
			for(int i = 2; i <= (int)buffer.length(); i++)
			{
				temp<<buffer[i];
			}
			
			if (db.find(stoul(temp.str())) == db.end() ) 
			{
				string msg = "La key no existe";
				cout<<"\tKey solicitada por el cliente es inexistente"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			else  
			{
				db.erase(stoul(temp.str()));
				string msg = "Tupla borrada";
				cout<< "\tExito en la operacion"<<endl;
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
		}
		else if(strcmp(buffer.c_str(),"list")== 0)
		{
			//ejecutar funcion list
			cout<<"Cliente a solicitado funcion \"list\""<<endl;
			// Imprimir lo que hemos agregado al mapa KV.
			for(map<unsigned long,Value>::iterator it = db.begin(); it != db.end(); ++it) 
			{
				string msg = to_string(it->first)+"\n";
				send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
			}
			string msg = "\tFin de la lista\n";
			send(new_socket, msg.c_str(), strlen(msg.c_str()),0);
		}
	}
	cout<<"se ha desconectado un cliente"<<endl;
	return NULL;
}




int main(int argc, char** argv) {	
	
	int sflag = 0;
	int opt;

	
	//definimos los parametros necesarios para crear el server
	string sock_dir;//direccion que ingresara el usuario
	int abc = 1;
	int server_fd, new_socket; 
    struct sockaddr_un server_addr; 
    int addrlen = sizeof(server_addr);   
    srand (time(NULL));
	DBcount = rand()% 9001 + 1000;
	
	
	// Procesar opciones de linea de comando
    while ((opt = getopt (argc, argv, "s:")) != -1) {
        switch (opt)
		{
			/* Procesar el flag s si el usuario lo ingresa */
			case 's':
				sflag = 1;
				cout<<"Socket a utilizar: "<<optarg<<endl;
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
    	perror("Fallo el socket");
    	exit(EXIT_FAILURE);
    }
    
    //conectamos el socket a la direccion
    if(setsockopt(server_fd,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &abc, sizeof(abc)))
    {
    	perror("Error de setsock");
    	exit(EXIT_FAILURE);
    }
    
    server_addr.sun_family = AF_UNIX;
    
    
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0)
    {
    	perror("Error de bindeo");
    	exit(EXIT_FAILURE);
    }
    
	cout<<"Servidor creado"<<endl;
	
	///aqui empieza la comunicaicon inter proceso///
	
	cout<<"Esperando conexion..."<<endl;
	while(true)
	{
		buffer.clear();
		//hacemos listen ppara escuchar a algun cliente que se quiera conectar//
		if(listen(server_fd, 10)<0)
		{
			perror("Error de funcion listen");
			exit(EXIT_FAILURE);
		}
		//luego de escuchar a algun cliente aceptamos y guardamos el socket del cliente//
		if((new_socket = accept(server_fd, (struct sockaddr *)&server_addr,(socklen_t*)&addrlen))<0)
		{
			perror("Error de accept");
			exit(EXIT_FAILURE);
		}
		
		cout<<new_socket<<endl;
		
		//aqui ejecutamos la funcion server_cliente como un thread
		pthread_t s_c;
		pthread_create(&s_c, NULL, &server_cliente,(void*)&new_socket);
		
		
	}
	
	return 0;
}
