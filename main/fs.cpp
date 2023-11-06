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
#include "_tcp_fs.h"
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
using namespace std;
// FS side

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
char *FS_CONNECTOR::execute(string msg, int clientSd)
{
    FileSystem FS(this->file_name);
    json response;
    string res = "";
    if (strcmp(&msg[0], "exit"))
    {   
            if(writing){
                char* str = (char*)msg.c_str();
                str[strlen(str)] = '\0';
                response["call"] = "my_write";
                response["status"] =  int(FS.my_Write(file_descriptor[2],
                file_descriptor[0],
                file_descriptor[1],str));

                file_descriptor[0] = -1;
                file_descriptor[1] = -1;
                file_descriptor[2] = -1;
                writing = false;

                res = response.dump();

            }else{

            json request = json::parse(msg);
            response["call"] = request["call"];

            if (request["call"] == "my_getPerm")
            {
                response["permission"] = FS.my_getPerm(
                std::string(request["path"]),
                request["user"]["UID"],
                request["user"]["GID"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_readPath")
            {
                response["inodeNumber"] = FS.my_readPath(request["path"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_Read_Size")
            {
                response["size"] = FS.my_Read_Size(request["inodeNumber"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_read_dir")
            {
                int inodenumber;
                string name;
                char type;
                int nextPos = FS.my_read_dir(
                    request["inodeNumber"],
                    request["position"],
                    inodenumber,
                    name,
                    type);
                response["nextPos"] = nextPos;
                response["inodeNumber"] = inodenumber;
                response["type"] = type;
                response["name"] = name;
                //******************************************************************************
            }
            else if (request["call"] == "my_Read_Mode")
            {   
                char*mode = FS.my_Read_Mode((int)request["inodeNumber"]);

                bool* bool_0= FS.character_To_Binary(mode[0]); //Perms
                bool* bool_1= FS.character_To_Binary(mode[1]); //Type
                int _0 = FS.characters_To_Integer(mode);
                
                string type = "";
                string permissions = "";

                type.append(to_string(int(bool_0[0])));
                type.append(to_string(int(bool_0[1])));
                permissions.append(to_string(int(bool_0[2])));
                permissions.append(to_string(int(bool_0[3])));
                permissions.append(to_string(int(bool_0[4])));
                permissions.append(to_string(int(bool_0[5])));
                permissions.append(to_string(int(bool_0[6])));
                permissions.append(to_string(int(bool_0[7])));

                permissions.append(to_string(int(bool_1[0])));
                permissions.append(to_string(int(bool_1[1])));
                permissions.append(to_string(int(bool_1[2])));

                type.append(permissions);
                response["mode"] = type;
                //******************************************************************************
            }
            else if (request["call"] == "my_Read_nlinks")
            {
                response["nlinks"] = FS.my_Read_nlinks(request["inodeNumber"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_Read_UID")
            {
                response["UID"] = FS.my_Read_UID(request["inodeNumber"]);
                //************s******************************************************************
            }
            else if (request["call"] == "my_Read_GID")
            {
                response["GID"] = FS.my_Read_GID(request["inodeNumber"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_Read_MTime")
            {
                response["MTime"] = FS.my_Read_MTime(request["inodeNumber"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_mkdir")
            {
                response["status"] = FS.my_mkdir(
                    request["path"],
                    request["user"]["UID"],
                    request["user"]["GID"]);

                response["what"] = request["path"];
                //******************************************************************************
            }
            else if (request["call"] == "my_read")
            {

                res = FS.my_Read(request["position"],
                request["size"],
                request["inodeNumber"]);
                reading = true;   
            }
            else if (request["call"] == "my_create")
            {

                int inn = FS.my_create(request["path"],
                                        request["user"]["UID"],
                                        request["user"]["GID"]);
                response["inodeNumber"] = inn;
                //******************************************************************************
            }
            else if (request["call"] == "my_write")
            {
                response["status"] = 0;
                file_descriptor = new int[3];
                file_descriptor[0] = request["position"];
                file_descriptor[1] = request["size"];
                file_descriptor[2] = request["inodeNumber"];
                writing = true;
                //******************************************************************************
            }
            else if (request["call"] == "get_block_use")
            {
                response["blockuse"] = FS.get_block_use(request["inodeNumber"]);
                //******************************************************************************
            }else if (request["call"] == "my_remove_entry")
            {
                request["status"] = FS.my_remove_entry(request["inodeNumber"],0);
                //******************************************************************************
            }
            else
            {
                string data = "";
                data.append(request["call"]);
                data.append(": call not found!");
                response = {{"error", true}, {"message", data}};
            }

            if(!reading){
                res = response.dump();
            }else{
                reading = false;   
            }
        }
    }else
    {
        res = msg;
    }
    char *cstr = new char[res.length() + 1];
    strcpy(cstr, res.c_str());
    return cstr;
}

//*************s*****************************************************************

FS_CONNECTOR::FS_CONNECTOR(string file)
{
    this->reading = false;
    this->writing = false;
    this->file_name = file;
}

FS_CONNECTOR::~FS_CONNECTOR()
{
}

//******************************************************************************

int main(int argc, char *argv[])
{
    //argv[0] is the name of the filesytem
    //argv[1] is the flag "new filesystem" and it can be 0 or 1
    string help = "usage ./fs filesytem.dat 0 (0 for just opening, 1 for erasing any filesystem.dat and creating a new filesytem)\n*\n*";
    ::system ("clear");
    int port = 230;
    char msg[4096]; // message is 4096 bytes long
    try{
        if(!argv[1] || !argv[2]) throw help;
        string filename = argv[1];
        cout<<((argv[2])[0] == '0' ? "Opening" : "Creating")<<" filesytem in "<<filename<<""<<endl;
        
        FS_CONNECTOR FS_C = FS_CONNECTOR(filename);
        FileSystem FS(filename);
        if((argv[2])[0] == '1'){
            FS.Create_New_FS(filename);
        }else{
            ifstream file;
            file.open(argv[1]);
            string f = argv[1];
            f.append(": No such file or directory");
            if(!file) throw f;
            
        }
        // setup a socket and connection tools
        sockaddr_in servAddr;
        bzero((char *)&servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(port);

        // open stream oriented socket with internet address
        // also keep track of the socket descriptor
        int serverSd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSd < 0)
        {
            cerr << "Error establishing the server socket" << endl;
            exit(0);
        }

        int bindStatus = ::bind(serverSd, (struct sockaddr *)&servAddr, sizeof(servAddr));

        if (bindStatus < 0)
        {
            cerr << "Error binding socket to local address" << endl;
            exit(0);
        }

        cout << "Waiting for the shell to connect..." << endl;
        listen(serverSd, 5);
        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);
        int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);

        if (newSd < 0)
        {
            cerr << "Error accepting msg from shell!" << endl;
            exit(1);
        }

        cout << "Connected with the shell!" << endl;
        while (1)
        {
            memset(&msg, 0, sizeof(msg));              // clear the buffer
            recv(newSd, (char *)&msg, sizeof(msg), 0); // receives message

            if (!strcmp(msg, "exit"))
            {
                close(newSd);
                newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
            }
            else if (!strcmp(msg, "shutdown"))
            {
                cout << "Closing..." << endl;
                send(newSd, (char *)&msg, strlen(msg), 0);
                break;
            }
            else if (strlen(msg) != 0)
            {
                cout << "Request:" << msg << endl;
                strcpy(msg, FS_C.execute(msg,newSd)); // message handler
                cout << "Response:" << msg << endl;
                send(newSd, (char *)&msg, strlen(msg), 0); // sends response
            }
        }

        FS.disk.close();
        close(newSd);
        close(serverSd);
    }catch(string e){
        cout<<"*\n*\n./fs: "<<e<<endl;
    }
    return 0;
}