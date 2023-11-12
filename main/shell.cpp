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

char* appendCharToCharArray(char* array, char a)
{
    size_t len = strlen(array);

    char* ret = new char[len+2];

    strcpy(ret, array);    
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
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
    int *ss = new int[3];
    for(int b = 0;b<3;b++){
        int mf = 0;
        for(int i = 4;i>1;i--){
            char dg = mode_array[(b*3)+i];
            int val = (int(dg-'0') << (2-(i-2)));
            mf += val;
        }
        ss[b] = mf;
       // ss[b] = mf;
    }

    for(int a = 0;a<3;a++){
        switch (ss[a])
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
    }

    string rc = type;
    rc += permission;
    return rc;
}

//******************************************************************************

string get_parent_path(string path){
    int i = 0;
    int dircount = 0;
    string ans;
    if(path == "/"){
        ans = path;
    }else{

    char **ss = ::line_splitter((char*)path.data(), "/");
    while(ss[i]){
        dircount++;
        i++;
    }
    for(int i = 0; i < dircount -1; i++){
        if(ss[i]){
			ans.append("/");
            ans.append(ss[i]);
		}
		}
    }

    return ans;
}

//******************************************************************************

// Gets the raw path passed by the user and return its absolute path
string to_abspath(string curDir, string raw){
    int dotcounts = 0;
	int parentcounts = 0;
    int i = 0;
	int j = 0;
    string ans;
    if(raw.at(0) == '/'){
        //just skip to end
	ans = raw;
    } else{
        //split raw up and curDir up, then find out how many .. are included in raw
        char **ss = ::line_splitter((char*)raw.data(), "/");
        char **abs = ::line_splitter((char*)curDir.data(), "/");

        while(ss[i]) {
            if((strcmp(ss[i],"..")) == 0){
                dotcounts++;
            }
            i++;
        }
		

		while(abs[j]) {
            parentcounts++;
            j++;
        }
        if(dotcounts < parentcounts){ //if the number of .. is greater than the amount of directories to go up
            //put the answer together, starting with the commonality between raw and curDir
            for(int i = 0; i < parentcounts - dotcounts; i++){
                if(abs[i]){
					ans.append("/");
                    ans.append(abs[i]);
				}
			}
            
            
        }

	    //ans.append(ss[0]);
            //add what is different about raw to answer
			int j = dotcounts;
            while(ss[j]){
                if(strcmp(ss[j], ".")){
                    ans.append("/");
                    ans.append(ss[j]);
                }
				j++;
            }
    }

    if(ans == ""){
        ans = "/";
    }
    return ans;
}

char *Shell::request(json req_json, char* buffer)
{
    int size = 4096;
    if(req_json["call"] == "my_read"){
        size = int(req_json["nBytes"]);
    }else if(req_json["call"] == "my_write"){
        size = int(req_json["nBytes"]);
    }

    char req[size];
    req_json["user"] = user;
    string req_ = req_json.dump();
    memset(&req, 0, sizeof(req)); // clear the buffer
    strcpy(req, req_.c_str());
    send(clientSd, (char *)&req, strlen(req), 0);
    memset(&req, 0, sizeof(req)); // clear the buffer
    recv(clientSd, (char *)&req, sizeof(req), 0);


    if(buffer != NULL){
        //memset(&req, 0, size); // clear the buffer

        
        cout<<endl;
        cout<<endl;
        cout<<endl;
        cout<<req_<<endl;
        int i = 0;
        while(size >= i){
            req[i] = buffer[i];
            cout<<buffer[i];
            i++;
        }
        cout<<endl<<"sending the buffer of size "<<size<<"...."<<endl;
        
        send(clientSd, (char *)&req, size, 0);
        memset(&req, 0, sizeof(req)); // clear the buffer
        cout<<"***********************************************Buffer sent!"<<endl;
        memset(&req, 0, sizeof(req)); // clear the buffer
        recv(clientSd, (char *)&req, sizeof(req), 0);
        cout<<"***********************************************Response received!"<<endl;

        cout<<endl;
        cout<<endl;
        cout<<endl;
    }
   
    char *res = new char[size];
    strcpy(res,req);
    return res;
}

//******************************************************************************

void Shell::build_ls(json callResponses,char* r){
        // Verifies if the my_getPerm received true or false
        // Starts to go over all entries in the i-node
        int count = 0;
        r = request({{"call","get_block_use"},{"inodeNumber",callResponses[0]["inodeNumber"]}});
        json res = json::parse((string)r);
        cout<<"total "<<res["blockuse"]<<endl;
        while (1)
        {
            json file_data = {{}};
            json readDirForm = {
                {"call", "my_read_dir"}, // get name, type, inodenumber
                {"inodeNumber", callResponses[0]["inodeNumber"]},
                {"position", count}};
            r = request(readDirForm);
            json readDirRes = json::parse((string)r);
            count = count == readDirRes["nextPos"]? (-1) : (int)readDirRes["nextPos"];

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

//************************* *****************************************************

void Shell::my_ls(char **input)
{
    string help = "usage: ls path/to/directory";
    char *r; //Reque st
    json r_j;
    json p_r;
    bool rc = false;
    string pd;
    json callResponses = {{}};
    json requestForms = {
        {{"call", "my_readPath"}},
        {{"call", "my_getPerm"}},
    };

    try{   
        if(input[1] && (input[1][0] == '-' || !strcmp(input[1],"--help") || !strcmp(input[1],"-h"))){
            throw help;
        }else{
            pd = (input[1] ? to_abspath(curDir,input[1]) : curDir);
        } 
        if(input[2]){
            string g = "";
            g.append("Too many arguments!");
            throw g;
        }

        //Requests all the system calls on the list
        for (int i = 0; i < requestForms.size(); i++){
            requestForms[i]["path"] = pd;
            r = request(requestForms[i]);
            callResponses[i] = json::parse((string)r);
        }

        //Verifies if the path exists
        if(callResponses[0]["inodeNumber"] == -1){
            string g = requestForms[0]["path"];
            g.append(": No such file or directory");
            throw g;
        }
        //Verifies if the user has permission to ls (just needs read)
        switch (int(callResponses[1]["permission"]))
        {
        case 4:/* r-- */rc = true;break;
        case 5:/* r-x */rc = true;break;
        case 6:/* rw- */rc = true;break;
        case 7:/* rwx */rc = true;break;
        default:
            string g = requestForms[0]["path"];
            g.append(": Permission denied");
            throw g;
            break;
        }
        /*If the directory exists and the user has permissions to see it,
        Builds the ls output by calling all data from all files in the dir*/
        if (rc) build_ls(callResponses,r);
        
    }catch(string e){
        cout<<"ls: "<<e<<endl;
    }
}

//******************************************************************************

void Shell::my_cd(char **input)
{    

    string help = "usage: cd path/to/directory";
    char *r; //Request
    json r_j;
    json p_r;
    string pd;
    bool rc = false;
    json callResponses = {{}};
    json requestForms = {
        {{"call", "my_readPath"}},
        {{"call", "my_getPerm"}},
    };

    try{

        if(input[1] && (input[1][0] == '-' || !strcmp(input[1],"--help") || !strcmp(input[1],"-h"))){
            throw help;
        }else{
            pd = (input[1] ? to_abspath(curDir,input[1]) : "/");
        }

        //Requests all the system calls on the list
        for (int i = 0; i < requestForms.size(); i++){
            requestForms[i]["path"] = pd;
            r = request(requestForms[i]);
            callResponses[i] = json::parse((string)r);
        }

        //Verifies if the path exists
        if(callResponses[0]["inodeNumber"] == -1){
            string g = pd;
            g.append(": No such file or directory");
            throw g;
        }

        //Verifies if the user has permission to cd (just needs execute)
        switch (int(callResponses[1]["permission"]))
        {
        case 3:/* -wx */rc = true;break;
        case 5:/* r-x */rc = true;break;
        case 7:/* rwx */rc = true;break;
        default:
            string g = pd;
            g.append(": Permission denied");
            throw g;
            break;
        }
        /*If the directory exists and the user has permissions to see it,
        Builds the ls output by calling all data from all files in the dir*/
        if (rc) curDir = pd;

    }catch(string e){
        cout<<"cd: "<<e<<endl;
    }
}

//******************************************************************************

void Shell::my_mkdir(char **input)
{
    string help = "usage: mkdir path/to/directory";
    try{
        if(input[1] && (input[1][0] == '-' || !strcmp(input[1],"--help") || !strcmp(input[1],"-h"))){
            throw help;
        }
        //Verify if the input[1] is empty
        if(input[1] && !input[2]){
            char *r; //Request
            bool rc = false;
            json callResponses = {{}};
            string pd = to_abspath(curDir,input[1]);
            json requestForms = {
                {{"call", "my_readPath"},{"path",get_parent_path(pd)}},
                {{"call", "my_readPath"},{"path",pd}},
                {{"call", "my_getPerm"},{"path",get_parent_path(pd)}},
            };

            //Requests all the system calls on the list
            for (int i = 0; i < requestForms.size(); i++){
                r = request(requestForms[i]);
                callResponses[i] = json::parse((string)r);
            }

            //Verifies if the path exists
            if(callResponses[0]["inodeNumber"] == -1){
                string g = requestForms[0]["path"];
                g.append(": No such file or directory");
                throw g;
            }

            if(callResponses[1]["inodeNumber"] != -1){
                string g = requestForms[1]["path"];
                g.append(": File exists");
                throw g;
            }
                
            //Check permissions for mkdir (just needs write)
            switch (int(callResponses[2]["permission"]))
                {
                case 2:/* -w- */rc = true;break;
                case 3:/* -wx */rc = true;break;
                case 6:/* rw- */rc = true;break;
                case 7:/* rwx */rc = true;break;
                default:
                    string g = requestForms[0]["path"];
                    g.append(": Permission denied");
                    throw g;
                    break;
                }

            json requestForm = {
                {"call", "my_mkdir"},
                {"path", pd}};
            r = request(requestForm);

        }else if(input[2]){
            throw "too many arguments!";
        }else{
            throw help;
        }
    }catch(string e){
            cout<<"mkdir: "<<e<<endl;
    }
}

//******************************************************************************

void Shell::my_Lcp(char **input)
{
    string help = "usage: Lcp filesystem/source machine/destination";
    bool rc = false;
    char* r;
    char curDir_m[3000];
    getcwd(curDir_m,3000);
    json callResponses = {{}};
    
    try{

        if(input[3] || !input[2] || !input[1] || input[1] && (input[1][0] == '-' || !strcmp(input[1],"--help") || !strcmp(input[1],"-h"))){
            throw help;
        }
        // check permission that you are able to access that file
        bool rc = false;
        char* r;
        json callResponses = {{}};
        string pd = to_abspath(curDir,input[1]);
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

        switch (int(callResponses[1]["permission"]))
            {
            case 4:/* r-- */rc = true;break;
            case 5:/* r-x */rc = true;break;
            case 6:/* rw- */rc = true;break;
            case 7:/* rwx */rc = true;break;
            default:
                string d = "ls: permission denied: ";
                d.append(pd);
                throw d;
                break;
            }
        

        //Verifies if the path exists
        if(callResponses[0]["inodeNumber"] == -1){
            string g = pd;
            g.append(": No such file or directory");
            throw g;
        }

        string path = to_abspath(curDir_m,input[2]);
        
        string path_2 = to_abspath(curDir,input[1]);
        char **abs = ::line_splitter((char*)path_2.data(), "/");

        string filename;
        int j = 0;
		while(abs[j]) {
            filename = abs[j];
            j++;
        }

        if(path == curDir_m){
            path = input[2];
            path.append(filename);
        }

        ofstream file;
        file.open (path);

        if(!file.is_open()){
            string g = path;
            g.append(": No such file or directory");
            throw g;
        }

        if(rc){
            //Gets the size of the file
            json req = {{"call", "my_Read_Size"},
            {"inodeNumber",callResponses[0]["inodeNumber"]}};
            char* size_res = request(req);
            json size_res_json = json::parse((string)size_res);
            
            int count = 0;
            int buffer_s = 1024;
            //int buffer_s = size_res_json["size"];
            string final_ = "";
            while(1){
                if(count >= size_res_json["size"]){
                    break;
                }
                if((count+buffer_s) >= size_res_json["size"]){
                    buffer_s = (int)size_res_json["size"] - count;
                }

                json req = {{"call", "my_read"},
                {"inodeNumber",callResponses[0]["inodeNumber"]},
                {"nBytes",buffer_s},
                {"size",size_res_json["size"]},
                {"position",count}};
                char* my_read_res = request(req);
                    
                count += buffer_s;
                final_.append(my_read_res);
            }
            //cout<<final_<<endl;
            file << final_;
            file.close();
            cout<<"Success! Created in: "<<path<<endl;
        }
    }catch(string e){
        cout<<"Icp: "<<e<<endl;
    }
}

//******************************************************************************

void Shell::my_cat(char **input){
     string help = "usage: cat path/to/file";
    bool rc = false;
    char* r;
    json callResponses = {{}};
    
    try{

        if((!input[1] || input[2]) || (input[1][0] == '-' || !strcmp(input[1],"--help") || !strcmp(input[1],"-h"))){
            throw help;
        }
        // check permission that you are able to access that file
        bool rc = false;
        char* r;
        json callResponses = {{}};
        string pd = to_abspath(curDir,input[1]);
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

        switch (int(callResponses[1]["permission"]))
            {
            case 4:/* r-- */rc = true;break;
            case 5:/* r-x */rc = true;break;
            case 6:/* rw- */rc = true;break;
            case 7:/* rwx */rc = true;break;
            default:
                string d = "ls: permission denied: ";
                d.append(pd);
                throw d;
                break;
            }
        

        //Verifies if the path exists
        if(callResponses[0]["inodeNumber"] == -1){
            string g = pd;
            g.append(": No such file or directory");
            throw g;
        }

        if(rc){
            //Gets the size of the file
            json req = {{"call", "my_Read_Size"},
            {"inodeNumber",callResponses[0]["inodeNumber"]}};
            char* size_res = request(req);
            json size_res_json = json::parse((string)size_res);
            
            int count = 0;
            int buffer_s = 1024;
            //int buffer_s = size_res_json["size"];
            string final_ = "";
            while(1){
                if(count >= (int)size_res_json["size"]){
                    break;
                }
                if((count+buffer_s) >= (int)size_res_json["size"]){
                    buffer_s = (int)size_res_json["size"] - count;
                }

                json req = {{"call", "my_read"},
                {"inodeNumber",callResponses[0]["inodeNumber"]},
                {"nBytes",buffer_s},
                {"size",size_res_json["size"]},
                {"position",count}};
                
                //int i = 0;
                request(req);

                count += buffer_s;
                ///cout<<endl<<count<<" | "<<(int)size_res_json["size"]<<endl;
            }
                cout<<endl;
        }
    }catch(string e){
        cout<<"cat: "<<e<<endl;
    }
}

//******************************************************************************

void Shell::my_Icp(char **input)
{   
    string help = "usage: Icp machine/source filesystem/destination";
    bool rc = false;
    char* r;
    char curDir_m[3000];
    getcwd(curDir_m,3000);
    json callResponses = {{}};
    
    try{
        if(input[3] || !input[2] || !input[1] || input[1] && (input[1][0] == '-' || !strcmp(input[1],"--help") || !strcmp(input[1],"-h"))){
            throw help;
        }

        json requestForms = {
            {{"call", "my_readPath"}},
            {{"call", "my_getPerm"}},
        };
        string pd = to_abspath(curDir,input[2]);
        if(pd != "/"){
            pd.append("/");
        }
        //Requests all the system calls on the list
        for (int i = 0; i < requestForms.size(); i++){
            requestForms[i]["path"] = pd;
            r = request(requestForms[i]);
            callResponses[i] = json::parse((string)r);
        }

        //Verifies if the path exists
        if(callResponses[0]["inodeNumber"] == -1){
            string g = pd;
            g.append(": No such file or directory");
            throw g;
        }

        //Verifies if the user has permission to ls
        switch (int(callResponses[1]["permission"]))
        {
        case 2:/* -w- */rc = true;break;
        case 3:/* -wx */rc = true;break;
        case 7:/* rwx */rc = true;break;
        default:
            string g = pd;
            g.append(": Permission denied");
            throw g;
            break;
        }

    
        if(rc){
            int file_size = 0;
            fstream file;
            file.open(to_abspath(curDir_m,input[1]), ios::in | ios::binary);

            
            if(file.is_open()) { //File exists!
                
                file.seekg(0, ios::end);
                int file_size = file.tellg();
                file.seekg(0, ios::beg);


                char *buffer = new char[file_size];
                file.read(buffer,file_size);

                //Gets the name of the file by creating the absolute path to it
                //Then getting the very last position of the line_splitter "/"
                char curDir_m[3000];
                getcwd(curDir_m, 3000);
                string path = to_abspath(curDir_m,input[1]);
                char **abs = ::line_splitter((char*)path.data(), "/");

                string filename;
                int j = 0;
		        while(abs[j]) {
                    filename = abs[j];
                    j++;
                }

                path = "";
                path.append(to_abspath(curDir,input[2]));

                if(path != "/"){
                    path.append("/");
                }

                path.append(filename);

                json requestForm = {{"call", "my_create"},{"path",path}};
                r = request(requestForm);
                json my_create_res = json::parse((string)r);

                if(my_create_res["inodeNumber"] <= 0){
                    string g = "";
                    g.append("Error while trying to import the file");
                    throw g;
                }

                int buf_size = 4096;
                int count = 0;
                while(1){

                    if(count >= file_size){
                        break;
                        rc = true;
                    }

                    if((count+buf_size) >= file_size){
                        buf_size = (file_size - count);
                    }

                    char* buffer_piece = new char[buf_size];
                    int lb = count;
                    int ub = count+buf_size;
                    file.seekg(lb, ios::beg);
                    file.read(buffer_piece,buf_size);
                    file.seekg(0, ios::beg);

                    json requestForm = {{"call", "my_write"},
                    {"position",count},
                    {"nBytes",buf_size},
                    {"size",file_size},
                    {"inodeNumber",my_create_res["inodeNumber"]}};

                    r = request(requestForm,buffer_piece);
                    json my_write_res = json::parse((string)r);
                    count += buf_size;

                }

                if(rc){
                    cout<<"Success! File "<<filename<<endl;
                }else{
                    string g = "File NOT created :(";
                    throw g;
                }

            }else{
                string g = "";
                g.append("No such file or directory");
                throw g;
            }
                //

                // lseek to the beginning of the file on the host system

                // put the blocks in a buffer, then read those to the local file

        }

    }catch(string e){
            cout<<"Icp: "<<e<<endl;
    }

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
    else if (!strcmp(ss[0], "cat"))
        my_cat(ss);
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
    string help = "usage ./shell [0, 1 or 2 for the user to be used]\n*\n*";
    ::system ("clear");
    Shell shell = Shell("127.0.0.1", 230);
    char msg[4096];
    json users = {0,0,1};


    try{
        if(!argv[1]) throw help;
        
        int user = ((int)argv[1][0] - 48);
        shell.user = {{"GID",users[user]},{"UID",user}};

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

            memset(&msg, 0, sizeof(msg)); // c lear the buffer

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
    }catch(string e){
        cout<<"*\n*\n./shell: "<<e<<endl;
    }    
        return 0;
}
