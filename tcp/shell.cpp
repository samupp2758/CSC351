#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include "json.hpp"
#include "bits/stdc++.h"
using json = nlohmann::json;
using namespace std;
//Shell side
/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
char* format(string msg,string UID,string GID,string curDir){ 
    string data = "";


    if(strcmp( &msg[0], "exit")){
        json response;
        response["message"] = msg; 
        response["UID"] = UID; 
        response["GID"] = GID; 
        response["curDir"] = curDir; 

        data = response.dump();        
    }else{
        data = msg;
    }

    char *cstr = new char[data.length() + 1];
    strcpy(cstr, data.c_str());

    return cstr;
}

int main(int argc, char *argv[])
{

    
    
    string str = "127.0.0.1" ;
    char *serverIp = &str[0];
    int port = 230; 
    char msg[4096]; 
    string UID = "0";
    string GID = "0";
    string curDir = "/";

    //setup a socket and connection tools 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl;
        return -1;
    }
    cout << "Connected to the FS!" << endl;


    while(1){
        string data;
        cout << curDir << " -> ";
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, format(data,UID,GID,curDir));

        if(data == "exit"){
            cout << "Closing..." << endl;
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }

        send(clientSd, (char*)&msg, strlen(msg), 0); //sends the msg
        memset(&msg, 0, sizeof(msg));//clear the buffer
        recv(clientSd, (char*)&msg, sizeof(msg), 0); //receives the response
        

        cout << msg << endl;
    }
    close(clientSd);
    return 0;    
}