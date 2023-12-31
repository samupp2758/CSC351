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
#include <thread>
#include "_tcp_fs.h"
#define bzero(b, len) (memset((b), '\0', (len)), (void)0)
using namespace std;
int STD_buffer = 100000;
// FS side

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
char *FS_CONNECTOR::execute(char *msg, int clientSd,int received)
{
    FileSystem FS(this->file_name);
    json response;
    char *res = new char[STD_buffer];

    if (strcmp(&msg[0], "exit"))
    {
        if (writing)
        {
            response["call"] = "my_write";
            file_descriptor[4] = file_descriptor[4] + received;
            for(int i = 0;i<received;i++){
                buff[i+file_descriptor[0]] = msg[i];
            }

/*
                file_descriptor[0] = request["position"];
                file_descriptor[1] = request["nBytes"];
                file_descriptor[2] = request["inodeNumber"];
                file_descriptor[3] = request["size"];
                file_descriptor[4] = 0;*/

                if((file_descriptor[5] == 1) && 
                file_descriptor[4] == file_descriptor[1]){
                
                response["status"] = int(FS.my_Write(file_descriptor[2],
                                                0,
                                                file_descriptor[3], buff));

                 
                delete [] buff;

            }

            if(file_descriptor[4] == file_descriptor[1]){
                file_descriptor[0] = -1;
                file_descriptor[1] = -1;
                file_descriptor[2] = -1;
                file_descriptor[3] = -1;
                file_descriptor[4] = -1;
                file_descriptor[5] = -1;
                response["status"] = 1;
                writing = false;
            }else{
                response["status"] = -1;
                file_descriptor[0] = file_descriptor[0] + received;
            }
            
            
        }
        else
        {   
            try{
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

                cout<<inodenumber<<endl;
                response["nextPos"] = nextPos < -1 ? -1 : nextPos;
                response["inodeNumber"] = inodenumber < -1 ? -1 : inodenumber;
                if(inodenumber >= 0){
                    response["type"] = type;
                    response["name"] = name;
                }else{
                    response["type"] = "";
                    response["name"] = "";
                }
                //******************************************************************************
            }
            else if (request["call"] == "my_Read_Mode")
            {
                char *mode = FS.my_Read_Mode((int)request["inodeNumber"]);

                bool *bool_0 = FS.character_To_Binary(mode[0]); // Perms
                bool *bool_1 = FS.character_To_Binary(mode[1]); // Type
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

                int size = (int)request["nBytes"];
                char *temp = FS.my_Read(request["inodeNumber"],
                                        request["position"],
                                        size);

                n_of_bytes_reading = (int)request["nBytes"];
                return temp;
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
                response["status"] = 1;
                file_descriptor = new int[5];
                file_descriptor[0] = request["position"];
                file_descriptor[1] = request["nBytes"];
                file_descriptor[2] = request["inodeNumber"];
                file_descriptor[3] = request["size"];
                file_descriptor[4] = 0;
                file_descriptor[5] = request["last"];
                if(request["position"] == 0){
                    buff = new char[(int)request["size"]];
                }
                writing = true;
                //******************************************************************************
            }
            else if (request["call"] == "get_block_use")
            {
                response["blockuse"] = FS.get_block_use(request["inodeNumber"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_remove_entry")
            {
                response["status"] = FS.my_remove_entry(request["inodeNumber"], request["position"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_rmdir")
            {
                response["status"] = FS.my_rmdir(request["path"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_search_dir")
            {
                response["res"] = FS.my_search_dir(request["dirinodeNumber"],request["filename"]);
                //******************************************************************************
            }
            else if (request["call"] == "set_UID")
            {
                response["status"] = true;
                FS.my_Set_UID(request["inodeNumber"],request["UID"]);
                //******************************************************************************
            }else if (request["call"] == "set_GID")
            {
                response["status"] = true;
                FS.my_Set_GID(request["inodeNumber"],request["GID"]);
                //******************************************************************************
            }
            else if (request["call"] == "copy_data")
            {
                response["status"] = FS.copy_data(request["source"],request["destination"]);
                //******************************************************************************
            }
            else if (request["call"] == "my_write_dir")
            {
                response["status"] = true;
                FS.my_write_dir(request["destination"],request["source"],request["dest_path"]);
                //******************************************************************************
            }
            else
            {
                string data = "";
                data.append(request["call"]);
                data.append(": call not found!");
                response = {{"error", true}, {"message", data}};
            }

            }catch(exception e){
                response["parsing_status"] = -1;
            }
        }

        if (!reading)
        {
            strcpy(res, (char *)response.dump().data());
        }
        else
        {
            reading = false;
        }

    }
    else
    {
        strcpy(res, msg);
    }
    return res;
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
    // argv[0] is the name of the filesytem
    // argv[1] is the flag "new filesystem" and it can be 0 or 1
    string help = "usage ./fs filesytem.dat [0 or 1 | open or create]\n*\n*";
    ::system("clear");
    int port = 230;
    int buff_size = STD_buffer;
    char msg[buff_size]; // message is 4096 bytes long
    try
    {
        if (!argv[1] || !argv[2])
            throw help;
        string filename = argv[1];
        cout << (!(argv[2])[1] && (argv[2])[0] == '0' ? "Opening" : "Creating") << " filesytem in " << filename << "" << endl;

        FS_CONNECTOR FS_C = FS_CONNECTOR(filename);
        if ((argv[2])[0] != '0')
        {
            FileSystem FS(filename);
            FS.Create_New_FS(filename);
            FS.disk.close();
        }
        else
        {
            ifstream file;
            file.open(argv[1]);
            string f = argv[1];
            f.append(": No such file or directory");
            if (!file)
                throw f;
        }
        // setup a socket and connection tools
        sockaddr_in servAddr;
        bzero((char *)&servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(port);

        sockaddr_in servAddr_write;
        bzero((char *)&servAddr_write, sizeof(servAddr_write));
        servAddr_write.sin_family = AF_INET;
        servAddr_write.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr_write.sin_port = htons(port+1);

        // open stream oriented socket with internet address
        // also keep track of the socket descriptor
        int serverSd = socket(AF_INET, SOCK_STREAM, 0);
        int writeSd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSd < 0 || writeSd < 0)
        {
            cerr << "Error establishing the server socket" << endl;
            exit(0);
        }

        int bindStatus_write = ::bind(writeSd, (struct sockaddr *)&servAddr_write, sizeof(servAddr_write));
        int bindStatus = ::bind(serverSd, (struct sockaddr *)&servAddr, sizeof(servAddr));

            if (bindStatus_write < 0)
        {
            cerr << "Error binding socket to local address (write)" << endl;
            exit(0);
        }

        if (bindStatus < 0)
        {
            cerr << "Error binding socket to local address" << endl;
            exit(0);
        }

        cout << "Waiting for the shell to connect..." << endl;
        listen(serverSd, 5);
        listen(writeSd, 5);

        sockaddr_in newSockAddr;
        socklen_t newSockAddrSize = sizeof(newSockAddr);

        sockaddr_in newSockAddr_write;
        socklen_t newSockAddrSize_write = sizeof(newSockAddr_write);

        int newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
        int newSd_write = accept(writeSd, (sockaddr *)&newSockAddr_write, &newSockAddrSize_write);

        if (newSd < 0 || newSd_write < 0)
        {
            cerr << "Error accepting msg from shell!" << endl;
            exit(1);
        }

        cout << "Connected with the shell!" << endl;
        while (1)
        {
            memset(&msg, 0, buff_size);  
            int n_bytes;

            if(FS_C.writing){
                n_bytes = recv(newSd_write, (char *)&msg, buff_size, 0);
            }else{
                n_bytes  = recv(newSd, (char *)&msg, buff_size, 0); // receives message
            }

            if (!strcmp(msg, "exit"))
            {
                close(newSd);
                close(newSd_write);
                newSd = accept(serverSd, (sockaddr *)&newSockAddr, &newSockAddrSize);
                newSd_write = accept(writeSd, (sockaddr *)&newSockAddr_write, &newSockAddrSize_write);
            }
            else if (!strcmp(msg, "shutdown"))
            {
                cout << "Closing..." << endl;
                send(newSd, (char *)&msg, strlen(msg), 0);
                send(newSd_write, (char *)&msg, strlen(msg), 0);
                break;
            }
            else if (n_bytes > 0)
            {
                cout << "Received: " << n_bytes << endl;
                if (FS_C.writing)
                {
                    cout << "Request:[data]" << endl;
                }
                else
                {
                    cout << "Request:" << msg << endl;
                }
                bool writing = FS_C.writing;
                char *result_ = FS_C.execute(msg, newSd,n_bytes);

                memset(&msg, 0, buff_size); // clear the buffer

                strcpy(msg, result_);       // message handler

                int numberOfBytesSent;
                if (FS_C.n_of_bytes_reading > 0)
                {
                    cout << "Response:[data]" << endl;
                    numberOfBytesSent = send(newSd, result_, FS_C.n_of_bytes_reading, 0); // sends response
                    FS_C.n_of_bytes_reading = 0;
                }
                else
                {
                    cout << "Response:" << msg << endl;
                    cout<<"writing:"<<writing<<endl;

                    if(writing && FS_C.file_descriptor[5] != 1){
                        numberOfBytesSent = send(newSd_write, msg, strlen(msg), 0); // sends response
                    }else if(!writing || FS_C.file_descriptor[5] == 1){
                        numberOfBytesSent = send(newSd, msg, strlen(msg), 0); // sends response
                    }
                }

                cout << "bytes sent:" << numberOfBytesSent << endl;
            }
        }

        close(newSd);
        close(serverSd);
    }
    catch (string e)
    {
        cout << "*\n*\n./fs: " << e << endl;
    }
    return 0;
}