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
#include <sys/stat.h>
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

            //MAX SHELL BUFFER SIZE
            int STD_buffer;

            //TCP INFO
            char *serverIp;
            int port;
            int clientSd;

            json user; //UID AND GID
            string curDir; //current working directory
            json users; //[GID_user0,GID_user1,GID_user2]
            json groups; //[0,3,1]

            string *history; //TODO

            //CURRENT COMMAND SPLITTED BY " "
            char **currentCommand;

            //TCP REQUEST-RESPONSE FUNCTIONS
            json request(json req_json);
            json request_write(json req_json,char* buffer); //
            int request_read(json req_json,char* &res);
            
            //COMMAND MAIN CONTROLLER
            void execute(string data);
            
            //COMMAND CONTROLLERS
            void my_ls();
            void my_cd();
            void my_mkdir();
            void my_rmdir();
            void my_Lcp();
            void my_Icp();
            void my_cat();
            void my_cp();
            void my_mv();
            void my_ln();
            void my_rm();
            void my_chown();

            //HELPER FUNCTIONS (standalone functions)
            char **line_splitter(char *line, string splitter);
            string format_mode(string mode);
            string get_parent_path(string path);
            string to_abspath(string curDir, string raw);
            string get_filename(string filepath);

            //Repetitive request helpers
            void handleSeekHelp(string help); //-h --help
            int testPath(string path,bool noThrow = false); //my_readpath (and throws if notfound)
            bool testDirectory(string path,bool noThrow = false); //my_readpath (and throws if notfound)
            void testPermissions(string path,bool read,bool write,bool execute);
            bool remove_file(string sourcePath);
            void build_ls(int dirInode);


            //ERROR Messages
            string ERROR_generic = "Failed";
            string ERROR_args_missing = "Not enough arguments! run -h or --help to see the usage";
            string ERROR_args_overflow = "Too many arguments! run -h or --help to see the usage";
            string ERROR_dir_not_empty = "Directory not empty!";
            string ERROR_perm_denied = "Permission Denied";
            string ERROR_file_exists = "File exists";
            string ERROR_notfound = ": No such file or directory";
            string ERROR_file_not_imported = "Error while trying to import the file";
            string ERROR_file_not_created = "File NOT created :(";
            string ERROR_not_a_dir = "Not a directory";
            string ERROR_user_notfound = "ilegal user";
            string ERROR_group_notfound = "ilegal group";
            string ERROR_notenough_space = ": Not enough space!!";

    };

#endif