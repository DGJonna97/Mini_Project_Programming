#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int sock;

int port = 18339;
string ipAddr = "127.0.0.1";

bool running = true;

void pollInput() { // Function for continually polling for user input. Will be started in a seperate thread

	string sendMsg;

	//Polling loop
	while (running) {
		getline(cin, sendMsg);

		if(sendMsg.length() == 1){ //The message must be one, and only one character
			int sendResult = send(sock, sendMsg.c_str(), sendMsg.size() + 1, 0);

			if (sendResult == -1) {
				cout << "Failed to send message" << endl;
				continue;
			}

		}else{
			cout << "You may only enter one character at a time!" << endl;
		}
	}
}

int main() {
	// Initialize Windows Socket
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Window Socket" << wsResult << endl;
		return 1;
	}

  sock = socket(AF_INET, SOCK_STREAM, 0);


	if (sock == -1) {
		cout << "Failed to create socket" << endl;
		return 1;
	}

	sockaddr_in hint; //IPv4
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port); //Setting hint port: Host to network short
	inet_pton(AF_INET, ipAddr.c_str(), &hint.sin_addr); //Setting hint address: string to binary (network byte order)

	//Connect to srv
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint)); //Connect to socket -> casting from sockaddr_in to sockaddr*

	if (connResult == -1) {
		cout << "Failed to connect to sockaddr" << endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	char recvMsg[64];

	//starting input polling on seperate thread
	thread input(pollInput);

	//starting network loop, waiting to recieve messages from the server
	while (running) {
		memset(recvMsg, 0, sizeof(recvMsg));
		int recvResult = recv(sock, recvMsg, sizeof(recvMsg), 0);

		cout << string(recvMsg, recvResult) << endl;
	}

	closesocket(sock);

	cout << "Done" << endl;
	return 0;
}
