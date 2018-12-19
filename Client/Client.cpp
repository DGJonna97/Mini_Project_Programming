#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

int sock;

int main(){
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if(sock == -1){
    cout << "Failed to create socket" << endl;
    return 1;
  }

  int port = 80;
  string ipAddr = "95.154.22.40";

  sockaddr_in hint; //IPv4
  hint.sin_family = AF_INET;
  hint.sin_port = htons(port); //Setting hint port: Host to network short
  inet_pton(AF_INET, ipAddr.c_str(), &hint.sin_addr); //Setting hint address: string to binary (network byte order)

  //Connect to srv
  int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint)); //Connect to socket -> casting from sockaddr_in to sockaddr*

  if(connResult == -1){
    cout << "Failed to connect to sockaddr" << endl;
    return 1;
  }

  char recvMsg[2048];
  string sendMsg;

  do {

    cout << "> ";
    getline(cin, sendMsg);

    int sendResult = send(sock, sendMsg.c_str(), sendMsg.size() + 1, 0);

    if(sendResult == -1){
      cout << "Failed to send message" << endl;
      continue;
    }

    memset(recvMsg, 0, sizeof(recvMsg));
    int recvResult = recv(sock, recvMsg, sizeof(recvMsg), 0);

    if(recvResult >= 0){
      cout << "Recieved empty message" << endl;
    }else{
      cout << "= " << string(recvMsg, recvResult) << endl;
    }

  } while(true);
  
  close(sock);

  cout << "Done" << endl;
  return 0;
}