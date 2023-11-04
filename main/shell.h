#ifndef S_H
#define S_H

#include <typeinfo>
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
#include "./utils/bits/stdc++.h"
#include "./utils/json.hpp"
#include <set>
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
            char *request(json req_json);
            void execute(string data);
            void my_ls(char**input);
            void my_cd(char**input);
            void my_mkdir(char**input);
            void my_Lcp(char**input);
            void my_Icp(char**input);
            string to_abspath(string raw);

            void chdir();
            void opendir();
            void closedir();
            void rewinddir();
            void open();
            void close();
            void lseek();
    };

#endif