/******************************************* dichiarazioni costanti *********************************************/
#define DEBUG

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

//Porta di ascolto della socket
short port = 4000; 
//Ip del Server
string ip = "192.168.0.105";


char ACK[100]={'P','1','A','1','#','Z'};
char buffer[256],config[100];     //id_aula[5],lung_trama[3],cod_com[2],temp[5],umid[5],amp[5],lux[5],man_aut[2],ora[7],s_pres[5],s_lux[5],tim_i[7],tim_f[7];
int wsastartup;
int ls_result;
fstream pacchetto;

//Id dell'Aula presente nei primi 4 byte del messaggio ricevuto dal client
string IdAula;

//COM ricevuto dal Data Logger
char COM = 0x82; 		//0x82 per Spegnere VMC e Aprire Finestra ; 0x85 per Accendere VMC e Chiudere Finestra

/********************************************* prototipo funzione salva - Salvataggio pacchetto su file di testo ************************************/
void salva(){
	int i;
	int lunghezza=0;
	
	
	cout<<"Sono nel void salva()\n\n";
	
	//Calcolo lunghezza tramite controllo del numero degli #
	for(i=0;i<256;i++)
    {
    	lunghezza++;
        if(buffer[i]=='#' && buffer[i+1]=='#')
        {
        	lunghezza++;
        	break;
		}
    }
    
    string Directory = "Data\\";// = "C:\\Program Files (x86)\\EasyPHP-Devserver-17\\eds-www\\Data\\" ;    //Directory dove salvare/trovare il file txt, ricordare di mettere \\ anche alla fine
    
	//Crea un file unico con nome l'Id dell'Aula nella Directory specificata sopra nella string Directory
    pacchetto.open((Directory + "Data_" + IdAula + ".txt").c_str(),ios::out);
	if(pacchetto.fail())
		cout<<"Errore nella creazione del File\n";
	//Inserisco nel file dell' IdAula unico per ogni classe il contenuto del buffer
	else
	{
		for(i=0;i<lunghezza;i++)
			pacchetto<<buffer[i];
		pacchetto<<endl;
	}
	pacchetto.close();
}

/********************************************* prototipo funzione carica - Caricamento pacchetto su file di testo ***********************************/
void configura(){
	int i;
	
	cout<<"\nSono nel void carica()\n\n";

	string Directory = "Conf\\";// = "C:\\Program Files (x86)\\EasyPHP-Devserver-17\\eds-www\\Data\\" ;    //Directory dove salvare/trovare il file txt, ricordare di mettere \\ anche alla fine
	
		pacchetto.open((Directory + "Conf_" + IdAula + ".txt").c_str(),ios::in);
		if(pacchetto.fail())
			cout<<"Errore nell'apertura del File\n";
		//Inserisco in config il contenuto del file Configurazione.txt
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
		cout << "Server: Errore nella creazione del Socket.\n" << endl;
		return;
	}
    else
    	cout << "Il Socket e' pronto\n"<< endl;
        
    
    //AF_INET fa riferimento alla famiglia di indirizzi IPv4
    Server_addr.sin_family = AF_INET;
    //Inserimento dell'indirizzo IPv4 del server (statico)
    Server_addr.sin_addr.s_addr = inet_addr(ip.data());       //Usare "127.0.0.1" se si vuole far funzionare in locale
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
    cout << "Accettata la Connessione con Client: " << inet_ntoa(Client_addr.sin_addr) <<"\n\n";
    //cout<<"\n\n"<<remoteSocket<<"\n\n";
    #endif
    
    //memset aggiunge il valore per i successivi byte, in questo caso impone tutti 0 per 256 byte nel buffer
    //(lo azzera)
    memset(buffer, 0, 256);
    
    //inserimento nel buffer del pacchetto ricevuto
    recv(remoteSocket, buffer, sizeof(buffer), 0);
    
    #ifdef DEBUG
    cout<<"\nIl Pacchetto ricevuto e' ";
    	for(i=0;i<256;i++)
    		cout<<buffer[i];
    cout<<"\n\n";
	#endif
 
 	IdAula="";
 	//Inserisco in IdAula i primi 4 byte del buffer, che corrispondono all'Id dell'Aula
	for (i=0;i<4;i++)
		IdAula+= buffer[i];
	
	
 	//controllo del messaggio (Se TYPE = 'd' è una richiesta, Se TYPE = 'g' è un ack, Se TYPE = 'c' è )
 	//se TYPE = 'd' è un messaggio, quindi lo salvo richiamando la funzione salva()
 	switch (buffer[5])
 	{
 		case 'd':
		{
			cout<<"Salva"<<endl;
   			salva();
    		sleep(1);
    		//Creazione messaggio da inviare al client contenente l'Id dell'Aula a cui si è connessi e l'ACK
    		string messaggio= IdAula + "#" + ACK + "##";
    		//invio del messaggio di avvenuta ricezione
			send(remoteSocket, messaggio.data(),  messaggio.length(), 0);
			sleep(1);
		}break;
		//se TYPE = 'g' è una richiesta di configurazione, quindi carico il messaggio di config e lo spedisco
		case 'g':
		{
			cout<<"Configura"<<endl;
			configura();
			//cout<<config<<endl<<sizeof(config)<<endl;
			sleep(1);
			//invio il pacchetto
			send(remoteSocket, config, sizeof(config), 0);
			sleep(1);
		}break;
		//se TYPE = 'c' è una richiesta del COM per sapere se Aprire o Chiudere la Finestra
		case 'c':
		{
			cout<<"Comando"<<endl;
			
			sleep(1);
			//Creazione messaggio da inviare al client contenente l'Id dell'Aula a cui si è connessi, il Type della richiesta e il COMando da eseguire
    		string messaggio= IdAula + "#c#" + COM + "##";
    		//invio del messaggio del COM
			send(remoteSocket, messaggio.data(),  messaggio.length(), 0);
			sleep(1);
		}break;
	}
	
}

/********************************************* prototipo funzione main ****************************************/
int main(int argc, char *argv[])
{
	//Uso un while per provare più volte le connessioni tra Server e Client
    while(1)
	{      
		//Richiamo il void ricezione()
		ricezione();
		#ifdef DEBUG
			cout << "\n\nChiudo il Server" << endl;
		#endif
		WSACleanup();
		system("pause");
		
		sleep(1);
		system("cls");
	    
	    sleep(3);
	}
   
	system("pause");
    return 0;
}
        
    
    
