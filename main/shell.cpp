#include "shell.h"
//Shell side
//******************************************************************************

Shell::Shell(string serverIp, int port){
    this->serverIp = &serverIp[0];
    this->port = port; 
    this->curDir = "/";
    this->user = {{"UID",0},{"GID",0},{"curDir",curDir}};
}

Shell::~Shell(){

}


//******************************************************************************

/*Johnny*/
char **line_splitter(char *line) {
	int bufsize = 4096, position = 0;
	char **tokens = (char**)malloc(bufsize);  //what holds our tokens
	char *token = strtok(line, " ");        //put tokenized values in tokens
	while (token != NULL) {
		tokens[position] = token;
		position++;
		
		if(position >= bufsize) {	       //expand buffer if needed
			bufsize += 4096;
			tokens = (char**)realloc(tokens, bufsize);
		}
		
		token = strtok(NULL, " ");
	}
	tokens[position] = NULL;           //end of line is set to NULL
	return tokens;
}

//******************************************************************************

string format_mode(string mode){
    string rc = (mode);
    return rc;
}

//******************************************************************************

//Receives in UNIX time and then returns in "Nov 23 2023 10:30"
string format_date(string date){
    string rc =  (date);
    return rc;
}

//******************************************************************************

string Shell::to_abspath(string raw){
    return raw;
}

//******************************************************************************

char *Shell::request(json req_json){
    char req[4096];
    req_json["user"] = user;
    string req_ = req_json.dump();
    memset(&req, 0, sizeof(req));//clear the buffer
    strcpy(req, req_.c_str());
    send(clientSd, (char*)&req, strlen(req), 0);
    memset(&req, 0, sizeof(req));//clear the buffer
    recv(clientSd, (char*)&req, sizeof(req), 0); 
    return strcat(req,"");
}

//******************************************************************************

void Shell::my_ls(char**input){
    char* r;
    json r_j;
    json p_r;
    bool error = false;
    json callResponses = {{}};
    json requestForms = {{
        {"call","my_getPerm"},
        {"path",(input[1] ? to_abspath(input[1]) : "/")}
    },{
        {"call","my_readPath"},
        {"path",(input[1] ? to_abspath(input[1]) : "/")}
    },};


    for(int i = 0;i<requestForms.size();i++){
        r = request(requestForms[i]);
        //cout<<r<<endl;
        callResponses[i] = json::parse((string)r);
    }

    if(callResponses[0]["permission"]){ //Verifies if the my_getPerm received true or false
        //Starts to go over all entries in the i-node
        int count = 0;
        while(1){
            json file_data = {{}};
            json readDirForm = {
                {"call","my_read_dir"}, //get name, type, inodenumber
                {"inodeNumber",callResponses[1]["inodeNumber"]},
                {"position",count}
            };
            r = request(readDirForm);
            json readDirRes = json::parse((string)r);
            count = readDirRes["nextPos"];

            if(count == -1){
                break;
            }

            json fileRequestForms = {{
                {"call","my_Read_Mode"}, //Get Type and permissions
            },{
                {"call","my_Read_nlinks"}, //Get nLinks
            },{
                {"call","my_Read_UID"}, //Get UID
            },{
                {"call","my_Read_GID"}, //Get GID
            },{
                {"call","my_Read_Size"} //Get size of the file
            },{
                {"call","my_Read_MTime"}, //Get Modified time
            }};

            for(int i = 0;i<fileRequestForms.size();i++){
                fileRequestForms[i]["inodeNumber"] = readDirRes["inodeNumber"];
                r = request(fileRequestForms[i]);
                //cout<<r<<endl;
                file_data[i] = json::parse((string)r);
            }

            cout << format_mode(file_data[0]["mode"]);
            cout << "@ " <<  file_data[1]["nlinks"];
            cout << " "  <<  file_data[2]["UID"];
            cout << " "  <<  file_data[3]["GID"];
            cout << " "  <<  file_data[4]["MTime"];
            cout << " "  <<  std::string(readDirRes["name"]);
            cout << endl;

        }
    }
    
}

//******************************************************************************

void Shell::my_cd(char**input){
    json requestForms = {{
        {"call","my_getPerm"},
        {"path",(input[1] ? to_abspath(input[1]) : "/")}
    }};
    
    char*r = request(requestForms[0]);
    json response = json::parse((string)r);
    if(response["permission"]){
        curDir = requestForms[0]["path"];
    }

}

//******************************************************************************

void Shell::my_mkdir(char**input){
    json requestForm = {
        {"call","my_mkdir"},
        {"path",(input[1] ? input[1] : "/")}
    };
    char* r = request(requestForm);
    cout<<r<<endl;
}

//******************************************************************************

void Shell::my_Lcp(char**input){
    //check permission that you are able to access that file

    //change directiories if needed

    //create the target file on the host system

    //Copy the file to the buffer, making multiple calls if necessary

}

//******************************************************************************

void Shell::my_Icp(char**input){
    //open the file on the underlying filesystem

    //make the local file

    //lseek to the beginning of the file on the host system

    //put the blocks in a buffer, then read those to the local file

}
//******************************************************************************

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
void Shell::execute(string msg){ 
    string data = "";
    char** ss = line_splitter(&msg[0]);

    if(!strcmp( ss[0], "exit")) data = msg;
    else if(!strcmp( ss[0], "ls")) my_ls(ss);
    else if(!strcmp( ss[0], "cd")) my_cd(ss);
    else if(!strcmp( ss[0], "mkdir")) my_mkdir(ss);
    else if(!strcmp( ss[0], "Lcp")) my_Lcp(ss);
    else if(!strcmp( ss[0], "Icp")) my_Icp(ss);
    else{
        string d = "shell: command not found: ";
        d.append(ss[0]);
        cout << d << endl;
        data.append("error");
    } 

}


int main(int argc, char *argv[])
{
    Shell shell = Shell("127.0.0.1",230);
    char msg[4096];


    struct hostent* host = gethostbyname(shell.serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(shell.port);
    shell.clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(shell.clientSd, (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to the FS!"<<endl;
        return -1;
    }
    cout << "Connected to the FS!" << endl;

    while(1){
        string data;
        cout << shell.curDir << " -> ";
        getline(cin, data);
        memset(&msg, 0, sizeof(msg));//clear the buffer

        string str = data;
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

        if(::strlen(&str[0]) > 0){
            if(data == "exit" || data == "shutdown"){
                strcpy(msg, data.c_str());
                cout << "Closing..." << endl;
                send(shell.clientSd, (char*)&msg, strlen(msg), 0);
                break;
            }else{
                shell.execute(data);
            }
        }

    }
    
    close(shell.clientSd);
    return 0;    
}
