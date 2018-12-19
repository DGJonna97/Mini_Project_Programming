#include <iostream>
#include <ws2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {

	// Initialize winsock



	// Create a Socket
	SOCKET listen = socket(AF_INET, SOCK_STREAM, 0);
	
	// Cerr is an object of ostream, that represents the standard 'error stream' oriented to narrow characters (of type 'char')
	if (listen == INVALID_SOCKET) {
		cerr << "Can't create a socket! Terminating" << endl
			return;
	}

	// Bind ip address and port to a socket



	// Tell winsock the socket is for listening



	// Wait for connection



	// Close listening socket



	// While loop accept and echo message back to client



	// wait for client to send data



	// echo message back to client



	// Close the socket



	// Shutdown winsock




}