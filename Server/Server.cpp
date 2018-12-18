#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
  // Initialize winsock
  WSADATA wsData;
  WORD ver = MAKEWORD(2, 2);

  int wsOk = WSAStartup(ver, &wsData);
  if(wsOk != 0){
    cerr << "Can't Initialize winsock! Terminating" << endl;
    return;
  }

  //Create a socket
  SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
  if(listening == INVALID_SOCKET){
    cerr << "Can't create a socket! Terminating" << endl;
    return;
  }

  // Bind ip address and port to a socket
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(8080);
  hint.sin_addr.S_un.S_addr = INADDR_ANY;

  bind(listening, (sockaddr*)&hint, sizeof(hint));

  // Tell winsock the socket is for listening
  listen(listening, SOMAXCONN);

  // Wait for connection
  sockaddr_in client;
  int clientSize = sizeof(client);

  SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
  if(clientSocket == INVALID_SOCKET){
    cerr << "Invalid client socket! Terminating" << endl;
    return;
  }

  char host[NI_MAXHOST];    //Client's remote name
  char service[NI_MAXSERV]; //Service (port) the client is connected on

  ZeroMemory(host, NI_MAXHOST);
  ZeroMemory(service, NI_MAXSERV);

  if(getnameinfo((sockaddr*)&client), sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0){
    cout << host << " connected on port " << service << endl;
  }  else{
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    cout << host << " connected on port " << ntohos(client.sin_port) << endl;
  }

  // Close listening socket
  closesocket(listening);

  //While loop accept and echo message back to client
  char buf[4096];

  while(true){
    ZeroMemory(buf, 4096);

    //wait for client to send data
    int bytesRecieved = recv(clientSocket, buf, 4096, 0)
    if(bytesRecieved == SOCKET_ERROR){
      cerr << "Error in recv(). Terminating" << endl;
      break;
    }

    if(bytesRecieved == 0){
      cout << "Client disconnected. " << endl;
      break;
    }

    //echo message back to client
    send(clientSocket, buf, bytesRecieved + 1, 0);

  }

  // Close the socket
  closesocket(clientSocket);

  // Shutdown winsock
  WSACleanup();

}
