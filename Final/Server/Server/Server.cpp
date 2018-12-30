#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <time.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int mistakes = 0;

string wordList[] = { "orange", "bike", "university", "people", "denmark",
"mathematics", "beautiful", "programming", "picturesque",
"challenge", "timetable", "aggressive", "blockbuster",
"tropical", "creativity", "intelligence", "cinematography",
"detention", "dictionary", "poster", "patriotic", "geology"
};

string finalWord = "done";
string activeWord = "____";

fd_set master;

void init();
void sendMsg(string msg);
void evalInput(string clientInput);
void evalVictory();

string getGameMessage();
string getVictoryMessage(bool endGame);


void main()
{
	// Initialize the game.
	init();

	// Initialze winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);
	if (wsOk != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	// Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	// Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(18339);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton .... 

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening 
	listen(listening, SOMAXCONN);

	// Create the master file descriptor set and zero it
	FD_ZERO(&master);

	// Add our first socket that we're interested in interacting with; the listening socket!
	// It's important that this socket is added for our server or else we won't 'hear' incoming
	// connections 
	FD_SET(listening, &master);

	// this will be changed by the \quit command (see below, bonus not in video!)
	bool running = true;

	while (running) {
		fd_set copy = master;

		// See who's talking to us
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		// Loop through all the current connections / potential connect
		for (int i = 0; i < socketCount; i++) {
			// Makes things easy for us doing this assignment
			SOCKET sock = copy.fd_array[i];

			// Is it an inbound communication?
			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				//Send a welcome message to the connected client.
				string welcomeMessage = "Welcome to the awesome Hangman game server!\n";
				send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);
				sendMsg(getGameMessage());
			}
			else {
				//Inbound message
				char buf[4096];
				ZeroMemory(buf, 4096);

				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					//Drop client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else {
					cout << buf << endl;
					evalInput(buf);
				}
			}
		}
	}

	// Remove the listening socket from the master file descriptor set and close it
	// to prevent anyone else trying to connect.
	FD_CLR(listening, &master);
	closesocket(listening);

	// Message to let users know what's happening.
	string msg = "Server is shutting down. Goodbye\r\n";

	while (master.fd_count > 0)
	{
		// Get the socket number
		SOCKET sock = master.fd_array[0];

		// Send the goodbye message
		send(sock, msg.c_str(), msg.size() + 1, 0);

		// Remove it from the master file list and close the socket
		FD_CLR(sock, &master);
		closesocket(sock);
	}

	// Cleanup winsock
	WSACleanup();

	system("pause");
}

void init() {

	srand(time(NULL));
	int randomWord = rand() % 4;
	finalWord = wordList[randomWord];
	mistakes = 0;
	activeWord = finalWord;

	for (int i = 0; i < finalWord.length(); i++) {
		activeWord[i] = '_';
	}

	sendMsg(getGameMessage());
}


void sendMsg(string msg) {
	for (int i = 0; i < master.fd_count; i++) {
		SOCKET outSock = master.fd_array[i];
		send(outSock, msg.c_str(), msg.size() + 1, 0);
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

void evalInput(string clientInput) {
	if (clientInput.length() == 1) {
		if (finalWord.find(clientInput) != std::string::npos) {
			for (int i = 0; i < finalWord.length(); i++) {
				if (finalWord[i] == clientInput[0]) {
					activeWord[i] = clientInput[0];
					cout << activeWord << endl;
				}
			}
		}
		else {
			mistakes++;
			sendMsg("upsi dupsi");
		}
	}
	sendMsg(getGameMessage());
	evalVictory();
}

void evalVictory() {
	//Compares two words: original word and guessed word, to check if it correct
	//and then sends the victory message if the word was guessed correct
	if (activeWord.compare(finalWord) == 0) {
		sendMsg(getVictoryMessage(true));
		init();
	}

	// sends the loss message if the maximum number of mistakes was reached
	if (mistakes >= 10) {
		sendMsg(getVictoryMessage(false));
		init();
	}
}