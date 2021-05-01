/******************************************* dichiarazioni costanti *********************************************/
//#define DEBUG

/********************************************* inclusione librerie **********************************************/
#include <winsock.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unistd.h>

#include "WINDOWS.h"  
#include <mutex>

using namespace std;

/********************************************* dichiarazioni variabili ******************************************/

SOCKET listenSocket;
SOCKADDR_IN Server_addr;
SOCKADDR_IN Client_addr;
int sin_size;

short port = 4000;  																						//Porta di ascolto della socket
string ip = "127.0.0.1";																					//Ip del Server

char ACK[7]={'P','1','A','1','B','Z'};

int wsastartup;
int ls_result;
fstream pacchetto;

mutex console;
char COM = 0x82; 																							//COM ricevuto dal Data Logger		//0x82 per Spegnere VMC e Aprire Finestra ; 0x85 per Accendere VMC e Chiudere Finestra

/********************************************* prototipo funzione salva - Salvataggio pacchetto su file di testo ************************************/
void salva(char buffer[256], string IdClient)
{
	int lunghezza=0;
	
	for(int i=0;i<256;i++)																					//Calcolo lunghezza tramite controllo del numero degli #
    {
    	lunghezza++;
        if(buffer[i]=='#' && buffer[i+1]=='#')
        {
        	lunghezza++;
        	break;
		}
    }
    
    string Directory = "Data\\";																			//Directory dove salvare/trovare il file txt, ricordare di mettere \\ anche alla fine
    
    pacchetto.open((Directory + "Data_" + IdClient + ".txt").c_str(),ios::out);								//Crea un file Data unico con a nome l'Id dell'Aula nella Directory specificata sopra nella string Directory
	if(pacchetto.fail())
		cout<<"Errore nella creazione del File\n";
		
	else																									//Inserisco nel file Data_*IdClient* unico per ogni classe il contenuto del buffer
	{
		for(int i=0;i<lunghezza;i++)
			pacchetto<<buffer[i];
		pacchetto<<endl;
	}
	pacchetto.close();
}

/********************************************* prototipo funzione carica - Caricamento pacchetto su file di testo ***********************************/
void configura(char config[100], string IdClient)
{
	int i;

	string Directory = "Conf\\";					  														//Directory dove salvare/trovare il file txt, ricordare di mettere \\ anche alla fine
	
		pacchetto.open((Directory + "Conf_" + IdClient + ".txt").c_str(),ios::in);							//Legge il file Conf unico con a nome l'Id dell'Aula nella Directory specificata sopra nella string Directory
		if(pacchetto.fail())
			cout<<"Errore nell'apertura del File\n";
		
		else																								//Inserisco in config il contenuto del file Configurazione.txt
		{
			pacchetto>>config[i];
			while (!pacchetto.eof())
			{
				i++;
				pacchetto>>config[i];
			}
		}

	pacchetto.close();
}

/********************************************* prototipo funzione comandi - decide il comando da inviare ***********************************/
void comandi(char buffer[256], string IdClient)
{
	
	//COM = 0x82;
	
	//float temp=stof(string(av).substr(1,5));																			
}

/********************************************* prototipo funzione ricezione - Ricezione dati dal Client *********************************************/
DWORD WINAPI ricezione(LPVOID lpParameter)
{
    SOCKET threadSocket=*(SOCKET*)lpParameter;
    //SOCKET* threadSocket_=(SOCKET*)lpParameter;
    //SOCKET threadSocket=*threadSocket_;
    
    char buffer[256],config[100];					
    
    memset(buffer, 0, 256);																				   //memset aggiunge il valore per i successivi byte, in questo caso impone tutti 0 per 256 byte nel buffer //(lo azzera)
	memset(config, 0, 100);	
    
    recv(threadSocket, buffer, sizeof(buffer), 0);														   //inserimento nel buffer del pacchetto ricevuto
 
 																	
    string IdClient=string(buffer).substr(0,4);															   //Inserisco in IdClient i primi 4 byte del buffer (messaggio ricevuto dal client) , che corrispondono all'Id del Client
		
	
	console.lock();
    	cout<<IdClient<<" - Pacchetto ricevuto: "<<buffer<<endl;
    console.unlock();
															
 							
							 													
 	switch (buffer[5])																						//controllo del messaggio (TYPE può essere: 'd', 'g', 'c')
 	{
 		case 'd':																							//se TYPE = 'd' è un messaggio contenente dati da parte di un Client, quindi lo salvo richiamando la funzione salva()
		{
			//cout<<"Salva"<<endl;
   			salva(buffer,IdClient);
    		string messaggio= IdClient + "#" + ACK + "##";													//Creazione messaggio da inviare al client contenente l'Id dell'Aula a cui si è connessi e l'ACK
    		console.lock();
				cout<<IdClient<<" - Messaggio da Inviare: "<<messaggio<<endl<<endl;
			console.unlock();	
			send(threadSocket, messaggio.data(),  messaggio.length(), 0);									//invio del messaggio di avvenuta ricezione
		}break;
		
		case 'g':																							//se TYPE = 'g' è una richiesta di configurazione da parte in un Client, quindi carico il messaggio di config richiamando la funzione configura() e lo spedisco
		{
			//cout<<"Configura"<<endl;
			configura(config,IdClient);
			console.lock();
				cout<<IdClient<<" - Messaggio da Inviare: "<<config<<endl<<endl;
			console.unlock();
			send(threadSocket, config, sizeof(config), 0);													//invio il pacchetto del config
		}break;
		
		case 'c': 																							//se TYPE = 'c' è una richiesta del COM da parte di un CLient per sapere se Aprire o Chiudere la Finestra
		{
			//cout<<"Comando"<<endl;
    		string messaggio= IdClient + "#c#" + COM + "##";												//Creazione messaggio da inviare al client contenente l'Id dell'Aula a cui si è connessi, il Type e il COMando da eseguire
			console.lock();
				cout<<IdClient<<" - Messaggio da Inviare: "<<messaggio<<endl<<endl;				
			console.unlock();
			send(threadSocket, messaggio.data(),  messaggio.length(), 0);									//invio del messaggio del COM
		}break;
		
		case 'o': 																							//se TYPE = 'o' è un messaggio da parte di un DataLogger contenente informazioni per calcolare il COM
		{
			//cout<<""<<endl;
			// 0101#o#20.50#80.30#30.10#10.05#05.07#25.10#30.00#17.89##
   			comandi(buffer,IdClient);
    		string messaggio= IdClient + "#" + ACK + "##";													//Creazione messaggio da inviare al client contenente l'Id del DataLogger a cui si è connessi e l'ACK
    		console.lock();
				cout<<IdClient<<" - Messaggio da Inviare: "<<messaggio<<endl<<endl;
			console.unlock();	
			send(threadSocket, messaggio.data(),  messaggio.length(), 0);									//invio del messaggio di avvenuta ricezione
		}break;
	}
	
}

/********************************************* prototipo funzione connessione - Connessione tra Server e Client *********************************************/
void connessione()
{
																											//Inizializzazione WSA (Winsock)
	WORD wVersionRequested = 0x0202;						
    WSADATA wsaData;   
    wsastartup = WSAStartup(wVersionRequested, &wsaData);
    
    SOCKET remoteSocket;
    
	if (wsastartup != NO_ERROR) 																			//Controllo errore di creazione
	{
		cout<<"Errore WSAStartup()"<<endl;
		return;
	}
        
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);												//Creazione della socket di ascolto
    
	if (listenSocket < 0)																					//Controllo creazione della socket
	{
		cout<<"Server: Errore nella creazione del Socket."<<endl<<endl;
		return;
	}
    else
    	cout<<"Il Socket e' pronto"<<endl;
        

    Server_addr.sin_family = AF_INET;																		//AF_INET fa riferimento alla famiglia di indirizzi IPv4
    Server_addr.sin_addr.s_addr = inet_addr(ip.data());      												//Inserimento dell'indirizzo IPv4 del server (statico)  //Usare "127.0.0.1" se si vuole far funzionare in locale
    Server_addr.sin_port = htons(port);																		//Porta di riferimento della socket del server (qualsiasi, anche se è consigliabile una porta effimera)
     
    if (bind(listenSocket, (LPSOCKADDR) &Server_addr, sizeof(Server_addr)) < 0)								//Una bind (cioè collegamento) della socket in modo tale da rendersi disponibile ad ascoltare
	{
    	cout<<"Server: errore durante la bind."<<endl;
        closesocket(listenSocket);
        system("pause");
        return;
    }
        
    ls_result = listen(listenSocket, SOMAXCONN);															//Passaggio da creazione ad ascolto effettivo della socket
    	
    if (ls_result < 0) 																						//Controllo dell'effettivo successo per l'ascolto della socket
	{
    	cout<<"Il Server non riesce a passare in ascolto"<<endl;
    	return;
	}
    else 
	{
    	cout<<"Il Server e' in Ascolto..."<<endl;
	}
    sin_size = sizeof(struct sockaddr_in);																	//Dimensione della struttura sockaddr_in (per avere l'area effettiva usata dalla struttura dati)
    
    while(true)
    {
    	remoteSocket = accept(listenSocket, (LPSOCKADDR) &Client_addr, &sin_size);							//Accettazione del client che tenta di comunicare con il server

    	console.lock();
    		cout << "\nAccettata la Connessione con Client: " << inet_ntoa(Client_addr.sin_addr) <<"\n\n";	//Stampa dell'accettazione con un client
    	console.unlock();
       	
       	DWORD ID1;
    	HANDLE arh;
    	arh=CreateThread(NULL,0,ricezione,&remoteSocket,0,&ID1);
  		
    }
}

/********************************************* prototipo funzione main ****************************************/
int main(int argc, char *argv[])
{
	
    while(1)
	{      
		connessione();																						//Richiamo il void connessione()
		cout <<"\n\nChiudo il Server" << endl;
		WSACleanup();
		
		system("cls");
	    sleep(3);
	}
   
	system("pause");
    return 0;
}
        
    
    
