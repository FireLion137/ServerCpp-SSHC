#define DEBUG
//#define Type_Server
#define Type_Client

#include <winsock.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>

using namespace std;

SOCKET remoteSocket;
short port;

SOCKET clientSocket;
SOCKADDR_IN addr;

char config[100];
char buffer[100];

string s="6485#d#D#55#35##";
string c="6485#c#D#55#35##";

/********************************************* prototipo funzione invia ***************************************/
void invia(){
	
	//Inizializzazione WSA (Winsock)
	WORD wVersionRequested = 0x0202;
    WSADATA wsaData;   
    int wsastartup = WSAStartup(wVersionRequested, &wsaData);

    //Controllo errore di creazione
	if (wsastartup != NO_ERROR) 
	{
		cout << "Errore WSAStartup()" << endl;
		return;
	}
 
	//porta del server in ascolto
	port = 4000;
    
    //Dichiarazione tipologia del formato e del valore dell'indirizzo IPv4
    
    //AF_INET fa riferimento alla famiglia di indirizzi IPv4
    addr.sin_family = AF_INET;
    //Inserimento dell'indirizzo IPv4 del server (statico)
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //Porta di riferimento della socket del server (qualsiasi, anche se è consigliabile una porta effimera)
    addr.sin_port = htons(port);
    
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    
    //Si tenta di connettersi al server
    if (connect(clientSocket, (LPSOCKADDR) &addr, sizeof(addr)) < 0)
    {
    	cout << "Errore nella connessione con il Server" << endl;
	}
	
	//Stampa di debug
	#ifdef DEBUG
    cout << "Messaggio da Inviare: "<<config << endl;
    #endif
    
    //Invia i dati al server
    send(clientSocket, config, sizeof(config), 0);
    
    //Riceve i dati dal server
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout<<"\nMessaggio ricevuto dal server: "<<buffer<<"\n";
    
}

int main()
{
	while(1)
	{   
		//Inserisco in config una stringa di caratteri
		strcpy(config,s.c_str());
		
    	#ifdef Type_Client
			invia();
			#ifdef DEBUG
				cout << "\n\nChiudo il Client" << endl;
			#endif
			//WSACleanup();   //Non funziona se lo uso
			sleep(5);
		    system("cls");
	    #endif
	    sleep(5);
	}
   
	system("pause");
    return 0;
    
}
