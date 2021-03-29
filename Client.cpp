/******************************************* dichiarazioni costanti *********************************************/
#define DEBUG

/********************************************* inclusione librerie **********************************************/
#include <winsock.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <unistd.h>

using namespace std;

/********************************************* dichiarazioni variabili ******************************************/
SOCKET remoteSocket;
short port;

SOCKET clientSocket;
SOCKADDR_IN addr;

char config[100];
char buffer[100];

//Stringhe di prova da inviare al Server
string d="6485#d#+10.3#80.30#27.36#06.85#57.41#03.60#a#1#40.42#12.75#19#36#40#29#03#21#1#17#42#53#21#24#51##";
string c="6485#c##";
string g="6485#g##";

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
    addr.sin_addr.s_addr = inet_addr("192.168.0.103"); 			//Usare "127.0.0.1" se si vuole far funzionare in locale
    //Porta di riferimento della socket del server (qualsiasi, anche se è consigliabile una porta effimera)
    addr.sin_port = htons(port);
    
    //Creazione della socket di invio
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    //Controllo creazione della socket
	if (clientSocket < 0)
	{
		cout << "Server: errore nella creazione del socket.\n" << endl;
		return;
	}
    else
    	cout << "Il Socket e' pronto\n"<< endl;
    	
    
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
    if(recv(clientSocket, buffer, sizeof(buffer), 0) < 0 )
    	cout<<"\nErrore: Nessun Messaggio ricevuto dal server\n";
    else
    	cout<<"\nMessaggio ricevuto dal server: "<<buffer<<"\n";
    
}

int main()
{
	//Uso un while per provare più volte le connessioni tra Server e Client
	while(1)
	{   
		//Inserisco in config una stringa di caratteri
		strcpy(config,g.c_str());
		
		//Richiamo il void invia()
		invia();
		#ifdef DEBUG
			cout << "\n\nChiudo il Client" << endl;
		#endif
		system("pause");
		
		sleep(1);
		system("cls");
		    
	    sleep(3);
	}
   
	system("pause");
    return 0;
    
}
