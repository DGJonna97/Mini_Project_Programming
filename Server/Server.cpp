#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <thread>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int mistakes;
string[] wordlist;
string finalWord;
string activeWord;

evalVictory();
getGameMessage();
getVictoryMessage();

void main()
{
	//Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0){
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	//Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET){
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	//Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(18339);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));


	//Tell Winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true) {
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++){
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) {
				//Accept new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//Add new connection to the list of connected clients
				FD_SET(client, &master);

				//Send a welcome message to the connected client.
				string welcomeMessage = "Welcome to the awesome chat server!\r\n";
				send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);
			} else {
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					closesocket(sock);
					FD_CLR(sock, &master);
				} else {
					for (int i = 0; i < master.fd_count; i++){
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock) {
							ostringstream ss;
							ss << "Socket #" << sock << ": " << buf << "\r\n";
							string strOut = ss.str();

							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}

	// Cleanup winsock
	WSACleanup();

	system("pause");
}

string wordlist[5] = { "orange", "bike", "university", "people", "denmark" };

void init(){

   srand(time(NULL));
   int randomWord = rand() % 4;
   finalWord = wordlist[randomWord];
   int mistakes = 0;
   activeWord = finalWord;
   for(int i=0; i < finalWord.length(); i++){
    activeWord[i]= '_';
   }

   cout << getGameMessage();
}

void evalVictory(){
	if (activeWord.compare(finalWord) == 0) {
		cout >> getVictoryMessage(true) >> endl;
		init();
	}


	if (mistakes >= 10){
		cout >> getVictoryMessage(false) >> endl;
		init();
	}
}

string getGameMessage() {
	string sendWord = "";
	
	for (int i = 0; i < activeWord.length(); i++) {
		sendWord += activeWord[i] + " ";
	}

	return "[" + sendWord + "] - Mistakes: " + mistakes + "/10";
}

string getVictoryMessage(bool endGame)
{
	if (endGame) // If true
		return "Winner Winner Chicken Dinner";
	else // If false
		return "Better Luck Next Time";
}

void evalInput(string clientInput){
    if(clientInput.length()==1){
       if(finalWord.find(clientInput)!= std::string::npos){
        for(int i=0; i<finalWord.length(); i++){
            if(finalWord[i]==clientInput[0]){
            activeWord[i] = clientInput[0];
            cout << activeWord <<endl;
            }
        }
       } else {
           mistakes++;
            cout << "nah" <<endl;
        }
    }
    evalVictory();
}
