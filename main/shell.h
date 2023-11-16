#ifndef S_H
#define S_H

#include <chrono>
#include <termios.h>
#include <typeinfo>
#include <iostream>
#include <cstring>
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
#include "./utils/bits/stdc++.h"
#include "./utils/json.hpp"
#include <set>
#include<sstream>
#include <filesystem>
using json = nlohmann::json;
using namespace std;
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)



    class Shell{
        private:
        public:
            Shell(string serverIp, int port,int buffer);
            ~Shell();
            int std_buffer;
            char *serverIp;
            int port;
            int clientSd;
            json user;
            string curDir;
            string *history; //Todo

            //TCP REQUEST-RESPONSE FUNCTIONS
            json request(json req_json);
            int request_write(json req_json,char* buffer);
            int request_read(json req_json,char* &res);
            
            //COMMAND MAIN CONTROLLER
            void execute(string data);
            
            //COMMAND CONTROLLERS
            void my_ls(char**input);
            void my_cd(char**input);
            void my_mkdir(char**input);
            void my_Lcp(char**input);
            void my_Icp(char**input);
            void my_cat(char **input);
            void my_rmdir(char **input);

            //HELPER FUNCTIONS
            char **line_splitter(char *line, string splitter);
            string format_mode(string mode);
            string get_parent_path(string path);
            void build_ls(json callResponses, char* r);
            string to_abspath(string curDir, string raw);

    };

#endif