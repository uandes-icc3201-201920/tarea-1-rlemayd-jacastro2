#include <iostream>
#include <memory>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/un.h>
#include "util.h"

#include <arpa/inet.h>
#define DIR "/tmp/db.tuples.sock"// direccion predeterminada del socket

using namespace std;



int main(int argc, char** argv) {
	
	string cmd = ""; //string de la linea de comandos
	int sflag = 0;//flag que nos dice si el usuario define un socket distinto al predeterminado
	int opt;
	int sock = 0;//numero en el cual guardaremos el valor que nos retorne la funcion socket()
	string sock_dir;//direccion que ingresara el usuario
	bool conectado = false;
	string buffer;//buffer donde se almacenaran los mensajes que lleguen del servidor
	
	struct sockaddr_un cliente_addr; //Contiene la direccion del servidor que nos queremos conectar

	while ((opt = getopt (argc, argv, "s:")) != -1) {
        switch (opt)
		{
			/* Procesar el flag s si el usuario lo ingresa */
			case 's':
				cout<<"socket a utilizar: "<<optarg<<endl;
				sock_dir = optarg;
				sflag = 1;
				break;
			default:
				return EXIT_FAILURE;
		  }	    	
	}
	

	while (cmd != "quit") {
		cout << ">";
		cin >> cmd;

		if(cmd.compare("connect") == 0)//veo si el comando del usuairo es connect
		{
			//Creamos el socket
			if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) //Tambien se puede usar AF_UNIX nose cual es el correcto 
			{ 
				printf("\n Error en la creacion del socket. \n"); 
				return -1; 
			}

			//Lo conectamos
			cliente_addr.sun_family = AF_UNIX; //Contiene un codigo a la ubicacion de la familia
			
			
			if(sflag == 0)
			{
				
				strcpy(cliente_addr.sun_path, DIR);
			}
			else
			{
				strcpy(cliente_addr.sun_path, sock_dir.c_str());//Se conecta al puerto dado
			}
			
			if (connect(sock, (struct sockaddr *)&cliente_addr, sizeof(cliente_addr)) < 0) 
			{ 
				printf("\nConexion fallida \n"); 
				return -1; 
			}
			cout << "Conexion exitosa con el servidor!\n";
			string msg = "conectado";
			conectado = true;
			send(sock, msg.c_str(), strlen(msg.c_str()),0);
		}
		
		
		else if(cmd.compare("disconnect") == 0 && conectado)//veo si el comando del usuairo es disconnect
		{
			string msg = "desconectado";
			send(sock, msg.c_str(), strlen(msg.c_str()),0);
			close(sock);
			conectado = false;
			cout << "Socket desconectado!\n";
		}
		
		
		else if(cmd.compare("list") == 0 && conectado)//veo si el comando del usuairo es list
		{
			cout << "Lista de claves: \n";
			string msg = "list";
			send(sock, msg.c_str(), strlen(msg.c_str()),0);
			
			char tempbuff[1024] = {0};
			read(sock, tempbuff, 1024);
			buffer = tempbuff;
			cout<<buffer;
		}
		
		
		//si no es ninguno de los anteriores,significa que el comando es del tipo cmd(a) o cmd(a,b)
		else
		{
			//entonces empiezo a recorrer el string cmd letra por letra hasta llegar a un '('
			stringstream temp;//genero una variable temporal donde ire guardando el string hasta llegar a un '('
			for(int i = 0; i<= (int)cmd.length();i++)//recorro el string
			{
				if(cmd[i] != '(')//si el cracter no es '(', lo agrego a temp
				{
					temp << cmd[i];
				}
				else if(cmd[i] == '(') //si encuentro un '(' significa que dentro de temp esta la palabra de la funcion a ejecutar.
				{
					i++;//le sumo 1 a i para saltarme el parentesis
					//procedo a revisar que nombre de funcion esta dentro de temp
					if(temp.str().compare("insert") == 0 && conectado)//si la funcion es insert...
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key = 0;
						string value = "";
						bool un_input = true; //boolean que dira si la funcion tiene un parametro
						for(; i<=(int)cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ',' && cmd[i] != ')')//si el caracter no es ',' ni ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')')//si el caracter es ')'
							{
								value = temp.str();//guardo temp en value
								
							}
							else if(cmd[i] == ',' && temp.str() != "")//si el caracter es ','
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								temp.str(string());//borro temp
								un_input = false;//si se ejecuta este if la funcion tiene 2 parametros
							}
							
						}
						
						if(un_input && value != "")//si solo hay un input, osea solo se manda el value y no la key
						{
							string msg = "";
							msg = "1;"+value;//se mandara el mensaje "1;value", el 1 significara que la funcion sera insert con solo el value
							
							///AQUI ENVIAMOS Y RECIBIMOS EL MENSAJE DEL INSERT(X)///
							
							send(sock, msg.c_str(), strlen(msg.c_str()),0);
							
							
							char tempbuff[1024] = {0};
							read(sock, tempbuff, 1024);
							buffer = tempbuff;
							cout<<buffer<<endl;
							
						}
						else if(value != "" && key!= 0)
						{
							///AQUI ENVIAMOS Y RECIBIMOS MENSAJES DEL INSERT(X,Y)///
							string msg = "";
							msg = "2;"+to_string(key)+";"+value+")";//se mandara el mensaje "2;key;value", el 2 significara que la funcion es insert con key y value
							send(sock, msg.c_str(), strlen(msg.c_str()),0);
							char tempbuff[1024] = {0};
							read(sock, tempbuff, 1024);
							buffer = tempbuff;
							cout<<buffer<<endl;
						}
						else
						{
							cout << "comando invalido\n";
						}
					}
					else if(temp.str().compare("get") == 0 && conectado)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key = 0;
						for(; i<=(int)cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ')')//si el cracter no es ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "") //si el caracter es ')'
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								string msg = "";
								msg = "3;"+to_string(key); //se mandara el mensaje "3;key", el 3 significa que sera la funcion get()
								
								///AQUI ENVIAMOS Y RECIBIMOS MENSAJES DEL GET(KEY)///
								send(sock, msg.c_str(), strlen(msg.c_str()),0);
								
								char tempbuff[1024] = {0};
								read(sock, tempbuff, 1024);
								buffer = tempbuff;
								cout<<buffer<<endl;
							}
							else
							{
								cout << "comando invalido\n";
							}
						}
						
					}
					else if(temp.str().compare("peek") == 0 && conectado)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key;
						for(; i<=(int)cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ')')//si el cracter no es ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "") //si el caracter es ')'
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								
								///AQUI ENVIAMOS Y RECIVIMOS MENSAJE DE LA FUNCION PEEK(KEY)///
								string msg = "";
								msg = "4;"+to_string(key); //se mandara el mensaje "4;key", el 4 significa que sera la funcion peek()
								send(sock, msg.c_str(), strlen(msg.c_str()),0);
								
								char tempbuff[1024] = {0};
								read(sock, tempbuff, 1024);
								buffer = tempbuff;
								cout<<buffer<<endl;
							}
							else
							{
								cout << "comando invalido\n";
							}
						}
						
					}
					else if(temp.str().compare("update") == 0 && conectado)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key;
						string value;
						for(; i<=(int)cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ',' && cmd[i] != ')')//si el cracter no es ',' ni ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "")//si el caracter es ')'
							{
								value = temp.str();//guardo temp en value
								
								///AQUI ENVIAMOS Y RECIBIMOS MENSAJES DE LA FUNCION UPDATE(KEY,VALUE)///
								string msg = "";
								msg = "5;"+to_string(key)+";"+value+")"; //se mandara el mensaje "5;key;value", el 5 significa que sera la funcion update()
								send(sock, msg.c_str(), strlen(msg.c_str()),0);
								
								char tempbuff[1024] = {0};
								read(sock, tempbuff, 1024);
								buffer = tempbuff;
								cout<<buffer<<endl;
							}
							else if(cmd[i] == ',' && temp.str() != "")//si el caracter es ','
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								temp.str(string());//borro temp
							}
							else
							{
								cout << "comando invalido\n";
							}
						}
					}
					else if(temp.str().compare("delete") == 0 && conectado)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key = 0;
						for(; i<=(int)cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ')')//si el cracter no es ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "") //si el caracter es ')'
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								
								
								///AQUI ENVIAMOS Y RECIBIMOS MENSAJES DE LA FUNCION DELETE(KEY)///
								string msg = "";
								msg = "6;"+to_string(key); //se mandara el mensaje "6;key", el 6 significa que sera la funcion delete()
								send(sock, msg.c_str(), strlen(msg.c_str()),0);
								char tempbuff[1024] = {0};
								read(sock, tempbuff, 1024);
								buffer = tempbuff;
								cout<<buffer<<endl;
							}
							else
							{
								cout << "comando invalido\n";
							}
						}
					}
					else
					{
						cout << "comando invalido\n";
					}
				}

			}
		}		
	}

	return 0;	
}
