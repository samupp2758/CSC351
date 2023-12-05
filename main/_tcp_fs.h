#ifndef FSTCP_
#define FSTCP_


#include "./utils/json.hpp"
#include "../FS.h"
using json = nlohmann::json;

    class FS_CONNECTOR{
        public:
            string file_name;
            int*file_descriptor;//POS,SIZE,INODENUMBER
            bool writing;
            bool reading;
            char*buff;
            int n_of_bytes_reading;
            FS_CONNECTOR(string file);
            ~FS_CONNECTOR();

            char* execute(char* msg,int clientSd,int received);
    };
#endif