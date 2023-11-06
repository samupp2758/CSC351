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


    class Shell{
        private:
        public:
            Shell(string serverIp, int port);
            ~Shell();
            char *serverIp;
            int port;
            int clientSd;
            json user;
            string curDir;
            string *history; //Todo
            char *request(json req_json,char* buffer = NULL);
            void execute(string data);
            void my_ls(char**input);
            void my_cd(char**input);
            void my_mkdir(char**input);
            void my_Lcp(char**input);
            void my_Icp(char**input);
            void my_cat(char **input);


            void build_ls(json callResponses, char* r);

    };

#endif