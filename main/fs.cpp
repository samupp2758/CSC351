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
#include "./utils/json.hpp"
//#include "../FS.h"
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
            json request = json::parse(msg);
            response["call"] = request["call"];

            if(request["call"] == "my_getPerm"){
                response["permission"] = true;
            }else if(request["call"] == "my_readPath"){
                response["inodeNumber"] = 1283198237;
            }else if(request["call"] == "my_Read_Size"){
                response["size"] = 10;
            }else if(request["call"] == "my_read_dir"){
                response["inodeNumber"] = 1293817231829;
                response["nextPos"] = (int)request["position"] < 9 ? ((int)request["position"]+1) : -1;
                response["type"] = "cooltype";
                response["name"] = "coolfilename";
            }else if(request["call"] == "my_Read_Mode"){
                response["mode"] = "101011100";
            }else if(request["call"] == "my_Read_nlinks"){
                response["nlinks"] = 12345;
            }else if(request["call"] == "my_Read_UID"){
                response["UID"] = 0;
            }else if(request["call"] == "my_Read_GID"){
                response["GID"] = 0;
            }else if(request["call"] == "my_Read_MTime"){
                response["MTime"] = to_string(123894571823);
            }else if(request["call"] == "my_read"){
            }else if(request["call"] == "my_mkdir"){
            }else{
                string data = "";
                data.append(request["call"]);
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

    //FileSystem FS("./fs.dat");

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
            close(newSd);
            newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        }else if(!strcmp(msg, "shutdown")){
            cout << "Closing..." << endl;
            send(newSd, (char*)&msg, strlen(msg), 0);
            break;
        }else if(strlen(msg) != 0){
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