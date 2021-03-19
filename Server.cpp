

/******************************************* dichiarazioni costanti *********************************************/

#define DEBUG
#define Type_Server

/********************************************* inclusione librerie **********************************************/
#include <winsock.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unistd.h>

using namespace std;

/********************************************* dichiarazioni variabili ******************************************/

SOCKET listenSocket;
SOCKET remoteSocket;
SOCKADDR_IN Server_addr;
SOCKADDR_IN Client_addr;
int sin_size;
short port;   

SOCKET clientsocket;
SOCKADDR_IN addr;     


char ACK[100]={'P','1','A','1','#','Z'};
char buffer[256],config[100];     //id_aula[5],lung_trama[3],cod_com[2],temp[5],umid[5],amp[5],lux[5],man_aut[2],ora[7],s_pres[5],s_lux[5],tim_i[7],tim_f[7];
int wsastartup;
int ls_result;
fstream pacchetto;



/********************************************* prototipo funzione salva - Salvataggio pacchetto su file di testo ************************************/
void salva(){
	int i;
	int lunghezza=0;
	
	cout<<"\nSono nel void salva()\n";
	for(i=0;i<256;i++)
    {
    	lunghezza++;
        if(buffer[i]=='#' && buffer[i+1]=='#')
        {
        	lunghezza++;
        	break;
		}
    }

		pacchetto.open("Pacchetto.txt",ios::out);
		if(pacchetto.fail())
			cout<<"Errore\n";
		else
		{
			for(i=0;i<lunghezza;i++)
				pacchetto<<buffer[i];
			
			pacchetto<<endl;
			
		}
	pacchetto.close();
	
}

/********************************************* prototipo funzione carica - Caricamento pacchetto su file di testo ***********************************/
void carica(){
	int i;
	
	cout<<"\nSono nel void carica()\n";

		pacchetto.open("Configurazione.txt",ios::in);
		if(pacchetto.fail())
			cout<<"Errore\n";
		else
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

/********************************************* prototipo funzione ricezione - Ricezione dati dal Client *********************************************/
void ricezione(){
	int i;
	
	//Inizializzazione WSA (Winsock)
	WORD wVersionRequested = 0x0202;
    WSADATA wsaData;   
    wsastartup = WSAStartup(wVersionRequested, &wsaData);
    
    //Controllo errore di creazione
	if (wsastartup != NO_ERROR) 
	{
		cout << "Errore WSAStartup()" << endl;
		return;
	}
        
    //Creazione della socket di ascolto
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    //Controllo creazione della socket
	if (listenSocket < 0)
	{
		cout << "Server: errore nella creazione del socket." << endl;
		return;
	}
    else
    	cout << "il Socket e' pronto"<< endl;
        
    //Porta di ascolto della socket
    port = 4000;
    
    //AF_INET fa riferimento alla famiglia di indirizzi IPv4
    Server_addr.sin_family = AF_INET;
    //Inserimento dell'indirizzo IPv4 del server (statico)
    Server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //Porta di riferimento della socket del server (qualsiasi, anche se è consigliabile una porta effimera)
    Server_addr.sin_port = htons(port);
     
    //Una bind (cioè collegamento) della socket in modo tale da rendersi disponibile ad ascoltare
    if (bind(listenSocket, (LPSOCKADDR) &Server_addr, sizeof(Server_addr)) < 0)
	{
    	cout << "Server: errore durante la bind." << endl;
        closesocket(listenSocket);
        system("pause");
        return;
    }
        
    //Passaggio da creazione ad ascolto effettivo della socket
    ls_result = listen(listenSocket, SOMAXCONN);
    
    //Controllo dell'effettivo successo per l'ascolto della socket
    if (ls_result < 0) 
	{
    	cout << "il Server non riesce a passare in ascolto" << endl;
    	return;
	}
    else 
	{
    	cout << "Il Server e' in Ascolto" << endl;
	}
    
    //Dimensione della struttura sockaddr_in (per avere l'area effettiva usata dalla struttura dati)
    sin_size = sizeof(struct sockaddr_in);
    //Accettazione del client che tenta di comunicare con il server
    remoteSocket = accept(listenSocket, (LPSOCKADDR) &Client_addr, &sin_size);
    //Stampa dell'accettazione con un client
    #ifdef DEBUG
    cout << "Accettata la Connessione con Client: " << inet_ntoa(Client_addr.sin_addr) << endl;
    cout<<"\n\n"<<remoteSocket<<"\n\n";
    #endif
    
    //memset aggiunge il valore per i successivi byte, in questo caso impone tutti 0 per 256 byte nel buffer
    //(lo azzera)
    memset(buffer, 0, 256);
    
    //inserimento nel buffer del pacchetto ricevuto
    recv(remoteSocket, buffer, sizeof(buffer), 0);
    
    	for(i=0;i<256;i++)
    		cout<<buffer[i];

 
 	//controllo del messaggio (Se COM = 'd' è una richiesta, Se COM = 'c' è un ack)
 	//se COM = 'd' è un messaggio, quindi lo salvo richiamando la funzione salva()
	if(buffer[5]=='d')
	{
		cout<<"Salva"<<endl;
   		salva();
    	sleep(1);
    	//invio un ACK di avvenuta ricezione
		send(remoteSocket, ACK, sizeof(ACK), 0);
		sleep(1);
	}
	//se COM = 'c' è un ack, quindi carico il messaggio e spedisco il pacchetto
	else if(buffer[5]=='c')
	{
		cout<<"Carica"<<endl;
		carica();
		cout<<config<<endl<<sizeof(config)<<endl;
		sleep(1);
		//invio il pacchetto
		send(remoteSocket, config, sizeof(config), 0);
		sleep(1);
	}
	
}

/********************************************* prototipo funzione main ****************************************/
int main(int argc, char *argv[])
{
    while(1)
	{      
    	#ifdef Type_Server
			ricezione();
			#ifdef DEBUG
				cout << "Chiudo il Server" << endl;
			#endif
			WSACleanup(); 
			sleep(5);
		    system("cls");
	    #endif
	    sleep(3);
	}
   
	system("pause");
    return 0;
}
        
    
    
