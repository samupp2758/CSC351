#ifndef FSTCP_
#define FSTCP_


#include "./utils/json.hpp"
#include "../FS.h"
using json = nlohmann::json;

    class FS_CONNECTOR{
        public:
            string file_name;
            FS_CONNECTOR(string file);
            ~FS_CONNECTOR();

            char* execute(string msg);
    };
#endif