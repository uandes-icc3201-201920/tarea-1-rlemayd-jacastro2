#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "util.h"

#include <arpa/inet.h>
#define dir_socket = "/tmp/db.tuples.sock.";

using namespace std;



int main(int argc, char** argv) {
	
	string cmd = "";
	int sflag = 0;
	int opt;
	int sock = 0;
	int socket;
	
	struct sockaddr_in cliente; //Contiene la direccion del servidor que nos queremos conectar	Tambien aca se puede usar sockaddr_un

	while ((opt = getopt (argc, argv, "s:")) != -1) {
        switch (opt)
		{
			/* Procesar el flag s si el usuario lo ingresa */
			case 's':
				cout<<"socket a utilizar: "<<optarg<<endl;
				socket = optarg;
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
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) //Tambien se puede usar AF_UNIX nose cual es el correcto 
			{ 
				printf("\n Error en la creacion del socket. \n"); 
				return -1; 
			}

			//Lo conectamos
			cliente.sin_family = AF_INET; //Contiene un codigo a la ubicacion de la familia
			if(sflag==1)
			{
				cliente.sin_port = htons(socket);//Se conecta al puerto dado
			}
			else
			{
				cliente.sin_port = htons(dir_socket);//Contiene el numero del puerto		htons-> Converts a port number in host byte order to a port number in network byte order
			}
			if (connect(sock, (struct sockaddr *)&cliente, sizeof(sockaddr_in)) < 0) //Lo primero es la descripcion del archivo del socket, lo 2do la direccion donde se quiere conectar
			{ 
				printf("\nConnection Failed \n"); 
				return -1; 
			}
			cout << "Conexion exitosa!\n";
		}
		else if(cmd.compare("disconnect") == 0)//veo si el comando del usuairo es disconnect
		{
			close(sock);
			cout << "Socket desconectado!\n";
		}
		else if(cmd.compare("list") == 0)//veo si el comando del usuairo es list
		{
			cout << "ejecutar funcion list\n";
		}
		//si no es ninguno de los anteriores,significa que el comando es del tipo cmd(a) o cmd(a,b)
		else
		{
			//entonces empiezo a recorrer el string cmd letra por letra hasta llegar a un '('
			stringstream temp;//genero una variable temporal donde ire guardando el string hasta llegar a un '('
			for(int i = 0; i<=cmd.length();i++)//recorro el string
			{
				if(cmd[i] != '(')//si el cracter no es '(', lo agrego a temp
				{
					temp << cmd[i];
				}
				else if(cmd[i] == '(') //si encuentro un '(' significa que dentro de temp esta la palabra de la funcion a ejecutar.
				{
					i++;//le sumo 1 a i para saltarme el parentesis
					//procedo a revisar que nombre de funcion esta dentro de temp
					if(temp.str().compare("insert") == 0)//si la funcion es insert...
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key = 0;
						string value = "";
						bool un_input = true; //boolean que dira si la funcion tiene un parametro
						for(; i<=cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
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
						//TODO:implementar funcion
						if(un_input && value != "")
						{
							cout << "ejecutar funcion insert con valores:\n";
							cout << "key = generado por la BD" <<endl;
							cout << "value = " << value <<endl;
						}
						else if(value != "" && key!= 0)
						{
							cout << "ejecutar funcion insert con valores:\n";
							cout << "key = " << key <<endl;
							cout << "value = " << value <<endl;
						}
						else
						{
							cout << "comando invalido\n";
						}
					}
					else if(temp.str().compare("get") == 0)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key = 0;
						for(; i<=cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ')')//si el cracter no es ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "") //si el caracter es ')'
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								
								//TODO:implementar funcion
								cout << "ejecutar funcion get con valores:\n";
								cout << "key = " << key <<endl;
							}
							else
							{
								cout << "comando invalido\n";
							}
						}
						
					}
					else if(temp.str().compare("peek") == 0)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key;
						for(; i<=cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ')')//si el cracter no es ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "") //si el caracter es ')'
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								
								//TODO:implementar funcion
								cout << "ejecutar funcion peek con valores:\n";
								cout << "key = " << key <<endl;
							}
							else
							{
								cout << "comando invalido\n";
							}
						}
						
					}
					else if(temp.str().compare("update") == 0)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key;
						string value;
						for(; i<=cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ',' && cmd[i] != ')')//si el cracter no es ',' ni ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "")//si el caracter es ')'
							{
								value = temp.str();//guardo temp en value
								
								//TODO:implementar funcion
								cout << "ejecutar funcion update con parametros:\n";
								cout << "key = " << key <<endl;
								cout << "value = " << value <<endl;
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
					else if(temp.str().compare("delete") == 0)
					{
						temp.str(string());//vacio temp para asi poder colocar el primer parametro de la funcion
						unsigned long key = 0;
						for(; i<=cmd.length();i++)//empiezo a recorrer cmd denuevo pero despues del parentesis
						{
							if(cmd[i] != ')')//si el cracter no es ')', lo agrego a temp
							{
								temp << cmd[i];
							}
							else if(cmd[i] == ')' && temp.str() != "") //si el caracter es ')'
							{
								key = stoul(temp.str(),nullptr,0);//convierto a temp en unsigned long
								
								//TODO:implementar funcion
								cout << "ejecutar funcion delete con valores:\n";
								cout << "key = " << key <<endl;
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
