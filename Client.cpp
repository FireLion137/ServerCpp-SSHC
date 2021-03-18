#include <winsock.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

SOCKET remoteSocket;
short port;

SOCKET clientsocket;
SOCKADDR_IN addr;

char config[100];

//SOCKET server

/********************************************* prototipo funzione invia ***************************************/
void invia(){
	//WSA (non usato)
	//WORD wVersionRequested = MAKEWORD(2,2);
	//WSADATA wsaData;      
	//WSAStartup(wVersionRequested, &wsaData);
	
	//porta del server in ascolto
	port = 4000;
    
    //Dichiarazione tipologia del formato e del valore dell'indirizzo IPv4
    
    //AF_INET fa riferimento alla famiglia di indirizzi IPv4
    addr.sin_family = AF_INET;
    //Inserimento dell'indirizzo IPv4 del server (statico)
    addr.sin_addr.s_addr = inet_addr("192.168.0.106");
    //Porta di riferimento della socket del server (qualsiasi, anche se è consigliabile una porta effimera)
    addr.sin_port = htons(port);
    
    //Creazione socket del client. 
    //	AF_INET     -> famiglia di indirizzi IPv4
    //	SOCK_STREAM -> è usato per specificare una socket stream (flusso dati)
    //  IPPROTO_TCP -> si usa il protocollo TCP (orientato alla connessione)
    clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
       //server = connect(clientsocket,(LPSOCKADDR)(&addr),sizeof(addr));
       //cout<<server;
    //Si tenta di connettersi al server
    if (connect(clientsocket, (LPSOCKADDR) &addr, sizeof(addr)) < 0)
    {
    	cout << "Errore nella connessione con il Server" << endl;
	}
	
	//Stampa di debug
	#ifdef DEBUG
    cout << "Messaggio da Inviare: "<<config << endl;
    #endif
    
    //Invia i dati al server
    send(remoteSocket, config, sizeof(config), 0);
    
}

int main()
{
	invia();
}
