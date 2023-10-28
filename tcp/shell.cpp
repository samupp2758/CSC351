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
char *request(json req_json,json user,int clientSd){
    char req[4096];
    req_json["user"] = user;
    string req_ = req_json.dump();
    memset(&req, 0, sizeof(req));//clear the buffer
    strcpy(req, req_.c_str());
    send(clientSd, (char*)&req, strlen(req), 0);
    memset(&req, 0, sizeof(req));//clear the buffer
    recv(clientSd, (char*)&req, sizeof(req), 0); 
    return strcat(req,"");
}

void my_ls(char**input,json userInfo,int clientSd){
    json requestForm = {
        {"call","my_getPerm"},
        {"path",(input[1] ? input[1] : "/")}
    };
    char* r = request(requestForm,userInfo,clientSd);
    cout<<r<<endl;
}

void my_cd(char**input,json userInfo,int clientSd){
    json requestForm = {
        {"call","my_readPath"},
        {"path",(input[1] ? input[1] : "/")}
    };
    char* r = request(requestForm,userInfo,clientSd);
    cout<<r<<endl;
}

void my_mkdir(char**input,json userInfo,int clientSd){
    json requestForm = {
        {"call","my_mkdir"},
        {"path",(input[1] ? input[1] : "/")}
    };
    char* r = request(requestForm,userInfo,clientSd);
    cout<<r<<endl;
}

void my_Lcp(char**input,json userInfo,int clientSd){
}


void my_Icp(char**input,json userInfo,int clientSd){
}

char **line_splitter(char *line) {
	int bufsize = 4096, position = 0;
	char **tokens = (char**)malloc(bufsize);  //what holds our tokens
	char *token = strtok(line, " ");        //put tokenized values in tokens
	while (token != NULL) {
		tokens[position] = token;
		position++;
		
		if(position >= bufsize) {	       //expand buffer if needed
			bufsize += 4096;
			tokens = (char**)realloc(tokens, bufsize);
		}
		
		token = strtok(NULL, " ");
	}
	tokens[position] = NULL;           //end of line is set to NULL
	return tokens;
}

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
void execute(string msg,json user,int clientSd){ 
    string data = "";
    char** ss = line_splitter(&msg[0]);

    if(strcmp( &msg[0], "exit")){

        if(!strcmp( ss[0], "ls")) my_ls(ss,user,clientSd);
        else if(!strcmp( ss[0], "cd")) my_cd(ss,user,clientSd);
        else if(!strcmp( ss[0], "mkdir")) my_mkdir(ss,user,clientSd);
        else if(!strcmp( ss[0], "Lcp")) my_Lcp(ss,user,clientSd);
        else if(!strcmp( ss[0], "Icp")) my_Icp(ss,user,clientSd);
        else{
            string d = "shell: command not found: ";
            d.append(ss[0]);
            cout << d << endl;
            data.append("error");
        }     

    }else{
        data = msg;
    }
}

int main(int argc, char *argv[])
{
    string str = "127.0.0.1" ;
    char *serverIp = &str[0];
    int port = 230; 
    char msg[4096]; 

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

    string curDir = "/";
    json user = {{"UID",0},{"GID",0},{"curDir",curDir}};

    while(1){
        string data;
        cout << curDir << " -> ";
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer

        execute(data,user,clientSd);

        if(data == "exit"){
            strcpy(msg, "exit");
            cout << "Closing..." << endl;
            send(clientSd, (char*)&msg, strlen(msg), 0);
            break;
        }

    }
    close(clientSd);
    return 0;    
}
