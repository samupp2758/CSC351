#include "shell.h"
// Shell side
//******************************************************************************

Shell::Shell(string serverIp, int port)
{
    this->serverIp = &serverIp[0];
    this->port = port;
    this->curDir = "/";
    this->user = {{"UID", 0}, {"GID", 0}, {"curDir", curDir}};
}

Shell::~Shell()
{
}

//******************************************************************************


/*Johnny*/
// Splits the char* line with the string splitter
char **line_splitter(char *line, string splitter)
{
    int bufsize = 4096, position = 0;
    char **tokens = (char **)malloc(bufsize);       // what holds our tokens
    char *token = ::strtok(line, splitter.c_str()); // put tokenized values in tokens
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        { // expand buffer if needed
            bufsize += 4096;
            tokens = (char **)realloc(tokens, bufsize);
        }

        token = ::strtok(NULL, splitter.c_str());
    }
    tokens[position] = NULL; // end of line is set to NULL
    return tokens;
}

//******************************************************************************

// Get the mode string, and returns type and permissions properly
string format_mode(string mode)
{
    char* mode_array = new char[17]; 
    strcpy(mode_array, mode.c_str());

    string type;
    string permission = "";
    type += mode_array[0];
    type += mode_array[1];
    if(type == "01") type = "d"; else type = "-";
    int *ss;
    for(int b = 0;b<3;b++){
        char* mf = new char[4];
        for(int i = 2;i<5;i++) mf[i-2] += mode_array[(b*3)+i];
        cout<<mf<<endl;
        //ss[b] = stoi(mf,nullptr,2);
    }

   /* for(int a = 0;a<3;a++){
        switch (ss[0])
        {
        case 0:permission += "---";break;
        case 1:permission += "--x";break;
        case 2:permission += "-w-";break;
        case 3:permission += "-wx";break;
        case 4:permission += "r--";break;
        case 5:permission += "r-x";break;
        case 6:permission += "rw-";break;
        case 7:permission += "rwx";break;
        default:break;
        }
    }*/

    string rc = type;
    rc += permission;
    return rc;
}

//******************************************************************************

// Gets the raw path passed by the user and return its absolute path
string Shell::to_abspath(string raw)
{
    string response;
    if (!strcmp(&raw[0], ".") || strcmp(&raw[0], "/"))
    {
        // Absolute path + relative path
        char *char_array = new char[sizeof(raw) + 1];
        strcpy(char_array, raw.c_str());
        if(!strcmp(&raw[0], ".")) char_array = char_array + 1;

        response.append(raw);
        // char** path = ::line_splitter(&raw[0],"/");
        cout << char_array << endl;
    }
    return response;
}

//******************************************************************************

char *Shell::request(json req_json)
{
    char req[4096];
    req_json["user"] = user;
    string req_ = req_json.dump();
    memset(&req, 0, sizeof(req)); // clear the buffer
    strcpy(req, req_.c_str());
    send(clientSd, (char *)&req, strlen(req), 0);
    memset(&req, 0, sizeof(req)); // clear the buffer
    recv(clientSd, (char *)&req, sizeof(req), 0);
    return strcat(req, "");
}

//******************************************************************************

void Shell::build_ls(json callResponses,char* r){
        // Verifies if the my_getPerm received true or false
        // Starts to go over all entries in the i-node
        int count = 0;
        while (1)
        {
            json file_data = {{}};
            json readDirForm = {
                {"call", "my_read_dir"}, // get name, type, inodenumber
                {"inodeNumber", callResponses[0]["inodeNumber"]},
                {"position", count}};
            r = request(readDirForm);
            json readDirRes = json::parse((string)r);
            count = readDirRes["nextPos"];

            if (count == -1) break;

            json fileRequestForms = {
                {{"call", "my_Read_Mode"}, /* Get Type and permissions*/},
                {{"call", "my_Read_nlinks"}, /* Get nLinks */},
                {{"call", "my_Read_UID"}, /* Get UID */},
                {{"call", "my_Read_GID"}, /* Get GID */},
                {{"call", "my_Read_Size"} /* Get size of the file*/},
                {{"call", "my_Read_MTime"}, /* Get Modified time*/}};

            for (int i = 0; i < fileRequestForms.size(); i++)
            {
                fileRequestForms[i]["inodeNumber"] = readDirRes["inodeNumber"];
                r = request(fileRequestForms[i]);
                // cout<<r<<endl;
                file_data[i] = json::parse((string)r);
            }   

            r = request({{"call","get_block_use"},{"inodeNumber",callResponses[0]["inodeNumber"]}});
            json res = json::parse((string)r);
            cout<<"total "<<res["blockuse"]<<endl;


            time_t Mtime = (time_t)file_data[5]["MTime"];
            char* r_MTime = ctime(&Mtime);
            string rmt_s = r_MTime;
            rmt_s.pop_back();

            cout << format_mode(file_data[0]["mode"]);
            cout << "@ " << file_data[1]["nlinks"];
            cout << " " << file_data[2]["UID"];
            cout << " " << file_data[3]["GID"];
            cout << " " << file_data[4]["size"];
            cout << " " << rmt_s;
            cout << " " << std::string(readDirRes["name"]);
            cout << endl;
    }
}

void Shell::my_ls(char **input)
{
    char *r; //Request
    json r_j;
    json p_r;
    bool rc = false;
    json callResponses = {{}};
    string pd = (input[1] ? to_abspath(input[1]) : curDir);
    json requestForms = {
        {{"call", "my_readPath"}},
        {{"call", "my_getPerm"}},
    };

    //Requests all the system calls on the list
    for (int i = 0; i < requestForms.size(); i++){
        requestForms[i]["path"] = pd;
        r = request(requestForms[i]);
        callResponses[i] = json::parse((string)r);
    }

    //Verifies if the path exists
    if(callResponses[0]["inodeNumber"] == -1){
        string d = "ls: ";
        d.append(pd);
        d.append(": No such file or directory");
        cout << d << endl;
    }else{
        //Verifies if the user has permission to ls
        switch (int(callResponses[1]["permission"]))
        {
        case 4:/* r-- */rc = true;break;
        case 5:/* r-x */rc = true;break;
        case 6:/* rw- */rc = true;break;
        case 7:/* rwx */rc = true;break;
        default:
            string d = "ls: access denied: ";
            d.append(pd);
            cout << d << endl;
            break;
        }

        /*If the directory exists and the user has permissions to see it,
        Builds the ls output by calling all data from all files in the dir*/
        if (rc) build_ls(callResponses,r);
    }
}

//******************************************************************************

void Shell::my_cd(char **input)
{
    char *r; //Request
    json r_j;
    json p_r;
    bool rc = false;
    json callResponses = {{}};
    string pd = (input[1] ? to_abspath(input[1]) : curDir);
    json requestForms = {
        {{"call", "my_readPath"}},
        {{"call", "my_getPerm"}},
    };

    //Requests all the system calls on the list
    for (int i = 0; i < requestForms.size(); i++){
        requestForms[i]["path"] = pd;
        r = request(requestForms[i]);
        callResponses[i] = json::parse((string)r);
    }

    //Verifies if the path exists
    if(callResponses[0]["inodeNumber"] == -1){
        string d = "ls: ";
        d.append(pd);
        d.append(": No such file or directory");
        cout << d << endl;
    }else{
        //Verifies if the user has permission to ls
        switch (int(callResponses[1]["permission"]))
        {
        case 3:/* -wx */rc = true;break;
        case 5:/* r-x */rc = true;break;
        case 7:/* rwx */rc = true;break;
        default:
            string d = "ls: access denied: ";
            d.append(pd);
            cout << d << endl;
            break;
        }

        /*If the directory exists and the user has permissions to see it,
        Builds the ls output by calling all data from all files in the dir*/
        if (rc) curDir = pd;
    }
}

//******************************************************************************

void Shell::my_mkdir(char **input)
{
    //Verify if the input[1] is empty
    if(input[1] && !input[2]){
        json requestForm = {
            {"call", "my_mkdir"},
            {"path", input[1]}};
        char *r = request(requestForm);
    }else if(input[2]){
        cout << "mkdir: too many arguments!" << endl;
    }else{
        cout << "usage: mkdir directory_name" << endl;
    }
}

//******************************************************************************

void Shell::my_Lcp(char **input)
{
    // check permission that you are able to access that file

    // change directiories if needed

    // create the target file on the host system

    // Copy the file to the buffer, making multiple calls if necessary
}

//******************************************************************************

void Shell::my_Icp(char **input)
{
    // open the file on the underlying filesystem

    // make the local file

    // lseek to the beginning of the file on the host system

    // put the blocks in a buffer, then read those to the local file
}
//******************************************************************************

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
void Shell::execute(string msg)
{
    string data = "";
    char **ss = line_splitter(&msg[0], " ");

    if (!strcmp(ss[0], "exit"))
        data = msg;
    else if (!strcmp(ss[0], "ls"))
        my_ls(ss);
    else if (!strcmp(ss[0], "cd"))
        my_cd(ss);
    else if (!strcmp(ss[0], "mkdir"))
        my_mkdir(ss);
    else if (!strcmp(ss[0], "Lcp"))
        my_Lcp(ss);
    else if (!strcmp(ss[0], "Icp"))
        my_Icp(ss);
    else
    {
        string d = "shell: command not found: ";
        d.append(ss[0]);
        cout << d << endl;
        data.append("error");
    }
}

int main(int argc, char *argv[])
{
    ::system ("clear");
    Shell shell = Shell("127.0.0.1", 230);
    char msg[4096];

    struct hostent *host = gethostbyname(shell.serverIp);
    sockaddr_in sendSockAddr;
    bzero((char *)&sendSockAddr, sizeof(sendSockAddr));
    sendSockAddr.sin_family = AF_INET;
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)*host->h_addr_list));
    sendSockAddr.sin_port = htons(shell.port);
    shell.clientSd = socket(AF_INET, SOCK_STREAM, 0);
    // try to connect...
    int status = connect(shell.clientSd, (sockaddr *)&sendSockAddr, sizeof(sendSockAddr));
    if (status < 0)
    {
        cout << "Error connecting to the FS!" << endl;
        return -1;
    }
    cout << "Connected to the FS!" << endl;

    while (1)
    {
        string data;
        cout << shell.curDir << " -> ";
        getline(cin, data);

        memset(&msg, 0, sizeof(msg)); // clear the buffer

        string str = data;
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end()); // Verfiy if the input is empty

        if (::strlen(&str[0]) > 0)
        {
            if (data == "exit" || data == "shutdown")
            {
                strcpy(msg, data.c_str());
                cout << "Closing..." << endl;
                send(shell.clientSd, (char *)&msg, strlen(msg), 0);
                break;
            }
            else
            {
                shell.execute(data);
            }
        }
    }

    close(shell.clientSd);
    return 0;
}
