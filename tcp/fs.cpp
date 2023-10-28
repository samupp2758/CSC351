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
using json = nlohmann::json;
using namespace std;
//FS side

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
char* execute(string msg){
    json response;
    string res = "";
        if(strcmp( &msg[0], "exit")){
            json j = json::parse(msg);
            response["message"] = j["call"];

            if(j["call"] == "my_getPerm"){
            }else if(j["call"] == "my_readPath"){
                response["inode"] = 12907088;
            }else if(j["call"] == "my_Read_Size"){
            }else if(j["call"] == "my_read_dir"){
            }else if(j["call"] == "my_Read_Mode"){
            }else if(j["call"] == "my_Read_nlinks"){
            }else if(j["call"] == "my_Read_UID"){
            }else if(j["call"] == "my_Read_GID"){
            }else if(j["call"] == "my_Read_MTime"){
            }else if(j["call"] == "my_read"){
            }else if(j["call"] == "my_mkdir"){
            }else{
                string data = "";
                data.append(j["call"]);
                data.append(": call not found!");
                response = {{"error",true},{"message",data}};
            }    
            res = response.dump();
        }else{
            res = msg;
        }
    char *cstr = new char[res.length() + 1];
    strcpy(cstr, res.c_str());
    return cstr;
}

//******************************************************************************

int main(int argc, char *argv[]){

    int port = 230;
    char msg[4096]; //message is 4096 bytes long

    //setup a socket and connection tools
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
 
    //open stream oriented socket with internet address
    //also keep track of the socket descriptor
    int serverSd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSd < 0){
        cerr << "Error establishing the server socket" << endl;
        exit(0);
    }

    int bindStatus = ::bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));

    if(bindStatus < 0){
        cerr << "Error binding socket to local address" << endl;
        exit(0);
    }

    cout << "Waiting for the shell to connect..." << endl;
    listen(serverSd, 5);
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);
    int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);

    if(newSd < 0){
        cerr << "Error accepting msg from shell!" << endl;
        exit(1);
    }

    cout << "Connected with the shell!" << endl;
    while(1){
        
        memset(&msg, 0, sizeof(msg));//clear the buffer
        recv(newSd, (char*)&msg, sizeof(msg), 0); //receives message
        
        if(!strcmp(msg, "exit")){
            cout << "Shell has quit the session" << endl;
            send(newSd, (char*)&msg, strlen(msg), 0);
            break;
        }

        if(strlen(msg) != 0){
            cout << "Request:" << msg << endl;
            strcpy(msg, execute(msg)); //message handler
            cout << "Response:" << msg << endl;
            send(newSd, (char*)&msg, strlen(msg), 0); //sends response
        }

    
    }

    close(newSd);
    close(serverSd);
    return 0;   
}