#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <time.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int mistakes;
string wordlist[25] = { "orange", "bike", "university", "people", "denmark",
												"mathematics", "beautiful", "programming", "picturesque",
												"challenge", "timetable", "aggressive", "blockbuster",
												"tropical", "creativity", "intelligence", "cinematography",
												"detention", "dictionary", "poster", "patriotic", "geology",
												"comment", "prehistorical", "bibliography" };
string finalWord;
string activeWord;
fd_set master;

void sendMsg(string msg);
void init();
void evalVictory();
string getGameMessage();
string getVictoryMessage(bool endGame);
void evalInput(string clientInput);

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

	FD_ZERO(&master);
	FD_SET(listening, &master);

	//Initialize the game
	init();

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
				string welcomeMessage = "Welcome to the awesome Hangman game server!\n";
				send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);
				sendMsg(getGameMessage());
			} else {
				//Inbound message
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					//Drop client
					closesocket(sock);
					FD_CLR(sock, &master);
				} else{
					evalInput(buf);
				}
			}
		}
	}

	// Cleanup winsock
	WSACleanup();

	system("pause");
}

void sendMsg(string msg) {
	for (int i = 0; i < master.fd_count; i++) {
		SOCKET outSock = master.fd_array[i];
			send(outSock, msg.c_str(), msg.size() + 1, 0);
	}
}

void init(){

   srand(time(NULL));
   int randomWord = rand() % 4;
   finalWord = wordlist[randomWord];
   mistakes = 0;
   activeWord = finalWord;

   for(int i=0; i < finalWord.length(); i++){
    activeWord[i]= '_';
   }

   sendMsg(getGameMessage());
}

void evalVictory(){
	//Compares two words: original word and guessed word, to check if it correct
 	//and then sends the victory message if the word was guessed correct
	if (activeWord.compare(finalWord) == 0) {
		sendMsg(getVictoryMessage(true));
		init();
	}

	// sends the loss message if the maximum number of mistakes was reached
	if (mistakes >= 10){
		sendMsg(getVictoryMessage(false));
		init();
	}
}

// Split up activeWord, so the client user's see it as underscores instead of the word
string getGameMessage() {
	string sendWord = activeWord + activeWord;

	for (int i = 0; i < sendWord.length(); i++) {
		if (i % 2 == 0) {
			sendWord[i] = activeWord[i / 2];
		}
		else {
			sendWord[i] = ' ';
		}
	}

	return "[ " + sendWord + "] - Mistakes: " + to_string(mistakes) + "/10";
}

// Message send if client user is winning or loosing the game
string getVictoryMessage(bool endGame)
{
	if (endGame) // If true
		return "Winner Winner Chicken Dinner\n";
	else // If false
		return "Better Luck Next Time\n";
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
           sendMsg("upsi dupsi");
        }
    }
		sendMsg(getGameMessag());
    evalVictory();
}
