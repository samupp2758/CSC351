#include "shell.h"
// Shell side
//******************************************************************************

Shell::Shell(string serverIp, int port, int buffer)
{
    this->STD_buffer = buffer;
    this->serverIp = &serverIp[0];
    this->port = port;
    this->curDir = "/";
    this->user = {{"UID", 0}, {"GID", 0}, {"curDir", curDir}};
}

Shell::~Shell()
{
}

//******************************************************************************

/*Johnny
Splits the char* line with the string splitter*/
char **Shell::line_splitter(char *line, string splitter)
{
    int bufsize = STD_buffer, position = 0;
    char **tokens = (char **)malloc(bufsize);       // what holds our tokens
    char *token = ::strtok(line, splitter.c_str()); // put tokenized values in tokens
    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= bufsize)
        { // expand buffer if needed
            bufsize += STD_buffer;
            tokens = (char **)realloc(tokens, bufsize);
        }

        token = ::strtok(NULL, splitter.c_str());
    }
    tokens[position] = NULL; // end of line is set to NULL
    return tokens;
}

//******************************************************************************

// Get the mode string, and returns type and permissions properly
string Shell::format_mode(string mode)
{
    char *mode_array = new char[17];
    strcpy(mode_array, mode.c_str());

    string type;
    string permission = "";
    type += mode_array[0];
    type += mode_array[1];
    if (type == "01")
        type = "d";
    else
        type = "-";
    int *ss = new int[3];
    for (int b = 0; b < 3; b++)
    {
        int mf = 0;
        for (int i = 4; i > 1; i--)
        {
            char dg = mode_array[(b * 3) + i];
            int val = (int(dg - '0') << (2 - (i - 2)));
            mf += val;
        }
        ss[b] = mf;
        // ss[b] = mf;
    }

    for (int a = 0; a < 3; a++)
    {
        switch (ss[a])
        {
        case 0:
            permission += "---";
            break;
        case 1:
            permission += "--x";
            break;
        case 2:
            permission += "-w-";
            break;
        case 3:
            permission += "-wx";
            break;
        case 4:
            permission += "r--";
            break;
        case 5:
            permission += "r-x";
            break;
        case 6:
            permission += "rw-";
            break;
        case 7:
            permission += "rwx";
            break;
        default:
            break;
        }
    }

    string rc = type;
    rc += permission;
    return rc;
}

//******************************************************************************

/*Johnny
Returns the parent path of the path that was passed*/
string Shell::get_parent_path(string path)
{
    int i = 0;
    int dircount = 0;
    string ans;
    if (path == "/")
    {
        ans = path;
    }
    else
    {

        char **ss = line_splitter((char *)path.data(), "/");
        while (ss[i])
        {
            dircount++;
            i++;
        }
        for (int i = 0; i < dircount - 1; i++)
        {
            if (ss[i])
            {
                ans.append("/");
                ans.append(ss[i]);
            }
        }
    }
    if (ans == "")
    {
        ans = "/";
    }
    return ans;
}

//******************************************************************************

string Shell::get_filename(string filepath){
    // Gets the name of the file by creating the absolute path to it
    // Then getting the very last position of the line_splitter "/"
    char **source_path_splitted = Shell::line_splitter((char *)filepath.data(), "/");

    // Gets the file name from the source path
    string source_filename;
    int j = 0;
    while (source_path_splitted[j])
    {
        source_filename = source_path_splitted[j];
        j++;
    }
    return source_filename;

}

/*Johnny
 Gets the raw path passed by the user and return its absolute path*/
string Shell::to_abspath(string curDir, string raw)
{
    int dotcounts = 0;
    int parentcounts = 0;
    int i = 0;
    int j = 0;
    string ans;
    if (raw.at(0) == '/')
    {
        // just skip to end
        ans = raw;
    }
    else
    {
        // split raw up and curDir up, then find out how many .. are included in raw
        char **ss = line_splitter((char *)raw.data(), "/");
        char **abs = line_splitter((char *)curDir.data(), "/");

        while (ss[i])
        {
            if ((strcmp(ss[i], "..")) == 0)
            {
                dotcounts++;
            }
            i++;
        }

        while (abs[j])
        {
            parentcounts++;
            j++;
        }
        if (dotcounts < parentcounts)
        { // if the number of .. is greater than the amount of directories to go up
            // put the answer together, starting with the commonality between raw and curDir
            for (int i = 0; i < parentcounts - dotcounts; i++)
            {
                if (abs[i])
                {
                    ans.append("/");
                    ans.append(abs[i]);
                }
            }
        }

        // ans.append(ss[0]);
        // add what is different about raw to answer
        int j = dotcounts;
        while (ss[j])
        {
            if (strcmp(ss[j], "."))
            {
                ans.append("/");
                ans.append(ss[j]);
            }
            j++;
        }
    }

    if (ans == "")
    {
        ans = "/";
    }
    return ans;
}

//******************************************************************************

// it throws if the user called for help, it will throw the help
void Shell::handleSeekHelp(string help)
{
    if ((currentCommand[1]) &&
        (currentCommand[1][0] == '-' ||
         !strcmp(currentCommand[1], "--help") ||
         !strcmp(currentCommand[1], "-h")))
    {
        throw help;
    }
}

//******************************************************************************

/*Tests if the path exists, if not, it throws no such file of directory*/
int Shell::testPath(string path, bool noThrow)
{
    json response = request({{"call", "my_readPath"}, {"path", path}});
    if (response["inodeNumber"] == -1 && !noThrow)
    {
        string g = path;
        g.append(ERROR_notfound);
        throw g;
    }
    return int(response["inodeNumber"]);
}

//******************************************************************************

/*Tests to see if the path points to a directory or not*/
bool Shell::testDirectory(string path, bool noThrow)
{
    int inodeNumber = testPath(path,noThrow);
    bool rc = false;

    json mode_res = request({{"call", "my_Read_Mode"}, {"inodeNumber", inodeNumber}});
    string mode_pretty = format_mode(mode_res["mode"]);
    if (mode_pretty[0] != 'd')
    {
        if (!noThrow)
        {
            throw ERROR_not_a_dir;
        }
    }
    else
    {
        rc = true;
    }
    return rc;
}

//******************************************************************************

/*Test permissions will receive a absolute path, and will verify if you can
or not read (if you need to read, send read as true, if not, false, same thing
for write and execute)*/
void Shell::testPermissions(string path, bool read, bool write, bool execute)
{
    json response = request({{"call", "my_getPerm"}, {"path", path}});
    bool rc = false;
    bool r_read;
    bool r_write;
    bool r_execute;
    r_read = r_write = r_execute = false;

    switch (int(response["permission"]))
    {
    case 0: /* --- */
        break;
    case 1: /* --x */
        r_execute = true;
        break;
    case 2: /* -w- */
        r_write = true;
        break;
    case 3: /* -wx */
        r_write = r_execute = true;
        break;
    case 4: /* r-- */
        r_read = true;
        break;
    case 5: /* r-x */
        r_read = r_execute = true;
        break;
    case 6: /* rw- */
        r_read = r_write = true;
        break;
    case 7: /* rwx */
        r_read = r_write = r_execute = true;
        break;
    }

    if ((read && !r_read) ||
        (write && !r_write) ||
        (execute && !r_execute))
    {
        throw ERROR_perm_denied;
    }
}

//******************************************************************************

bool Shell::remove_file(string sourcePath){
    int filePOS;
    string sourceDIR = get_parent_path(sourcePath);
    //If failure occurs in remove entry, display to user via shell that a failure occurred, else do nothing.
    int diriNodeNum = testPath(sourceDIR);
    
    string source_filename = get_filename(sourcePath);

    json response_search_dir = request({{"call", "my_search_dir"},
                        {"dirinodeNumber", diriNodeNum},
                        {"filename", source_filename}});

    filePOS = (int)response_search_dir["res"];
    
    if(filePOS == -1){
        throw ERROR_generic;
    }


    //Remove entry by sending call to FS via TCP to remove_entry(Path)
    json requestForm = {{"call", "my_remove_entry"},
                        {"inodeNumber", diriNodeNum},
                        {"position", filePOS}};

    json response = request(requestForm);

    return response["status"] >= 0;
}

//******************************************************************************

// Request for normal json
json Shell::request(json req_json)
{
    int size = STD_buffer;

    char req[size];
    req_json["user"] = user;
    string req_ = req_json.dump();
    memset(&req, 0, size); // clear the buffer
    strcpy(req, req_.c_str());
    int sent_bytes = send(clientSd, (char *)&req, strlen(req), 0);
    memset(&req, 0, size); // clear the buffer
    int received_bytes = recv(clientSd, (char *)&req, size, 0);

    json res = json::parse((string)req);
    if (res["error"] != nullptr)
    {
        throw res["message"];
    }

    return res;
}

//******************************************************************************

// Request for writing data to the FS, it returns the number of bytes sent
json Shell::request_write(json req_json, char *buffer)
{
    int size = int(req_json["nBytes"]);
    char req[size];
    memset(&req, 0, size); // clear the buffer

    json res = request(req_json);

    int i = 0;
    while (size >= i)
    {
        req[i] = buffer[i];
        i++;
    }

    int sent = send(clientSd, (char *)&req, size, 0);
    memset(&req, 0, size); // clear the buffer
    int received = recv(clientSd, (char *)&req, STD_buffer, 0);

    json res_write = json::parse((string)req);

    return res_write;
}

//******************************************************************************

// Request for reading data from the FS (returns the number of bytes received)
int Shell::request_read(json req_json, char *&res)
{
    int size = (int)req_json["nBytes"];
    char req[STD_buffer];
    req_json["user"] = user;
    string req_ = req_json.dump();
    memset(&req, 0, sizeof(req)); // clear the buffer
    strcpy(req, req_.c_str());

    int sent_bytes = send(clientSd, (char *)&req, strlen(req), 0);

    memset(&req, 0, STD_buffer); // clear the buffer

    int received = recv(clientSd, (char *)&req, size, 0);

    int i = 0;
    while (i < size)
    {
        res[i] = req[i];
        i++;
    }

    return received;
}

//******************************************************************************

/*
Helper function that populates the list of files in the directory*/
void Shell::build_ls(int dirInode)
{
    // Verifies if the my_getPerm received true or false
    // Starts to go over all entries in the i-node
    int count = 0;
    json res = request({{"call", "get_block_use"}, {"inodeNumber", dirInode}});

    cout << "total " << res["blockuse"] << endl;

    bool run = true;
    while (run)
    {
        json file_data = {{}};
        json readDirForm = {
            {"call", "my_read_dir"}, // get name, type, inodenumber
            {"inodeNumber", dirInode},
            {"position", count}};
        json readDirRes = request(readDirForm);

        if (readDirRes["nextPos"] == -1)
            break;
        if (count == readDirRes["nextPos"])
            run = false;

        count = readDirRes["nextPos"];

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
            file_data[i] = request(fileRequestForms[i]);
            // cout<<r<<endl;
        }

        time_t Mtime = (time_t)file_data[5]["MTime"];
        char *r_MTime = ctime(&Mtime);
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

//******************************************************************************

void Shell::my_ls()
{
    string help = "usage: ls path/to/directory";
    string dir;

    handleSeekHelp(help);

    // Gets the directory passed by the client or sends the client to current directory
    dir = (currentCommand[1] ? to_abspath(curDir, currentCommand[1]) : curDir);

    // Veryfies if the client passed more arguments than necessary
    if (currentCommand[2])
    {
        throw ERROR_args_overflow;
    }

    // Verifies if the path exists and returns the inode of that dir inode
    int dirInode = testPath(dir);

    // Verifies if the user has permission to ls (just needs read)
    testPermissions(dir, true, false, false);

    /*If the directory exists and the user has permissions to see it,
    Builds the ls output by calling all data from all files in the dir*/
    build_ls(dirInode);
}

//******************************************************************************

void Shell::my_cd()
{
    string help = "usage: cd path/to/directory";
    string dir;
    string root = "/";
    // Calls the help conditional
    handleSeekHelp(help);

    // Gets the directory passed by the client or sends the client to root
    dir = (currentCommand[1] ? to_abspath(curDir, currentCommand[1]) : root);

    int inodeNumber = testPath(dir);

    // Verifying if the path is to a directory
    json mode_res = request({{"call", "my_Read_Mode"}, {"inodeNumber", inodeNumber}});
    string mode_pretty = format_mode(mode_res["mode"]);
    if (mode_pretty[0] != 'd')
    {
        throw ERROR_not_a_dir;
    }

    testPermissions(dir, false, false, true);

    /*If the directory exists and the user has permissions to see it,
    Builds the ls output by calling all data from all files in the dir*/
    curDir = dir;
}

//******************************************************************************

void Shell::my_rmdir()
{

    string help = "usage: rmdir path/to/directory";
    string dir;
    handleSeekHelp(help);

    // Verify if the currentCommand[1] is empty
    if (!currentCommand[1])
    {
        throw ERROR_args_missing;
    }

    if (currentCommand[2])
    {
        throw ERROR_args_overflow;
    }

    // Gets the path of the
    dir = to_abspath(curDir, currentCommand[1]);

    // Test to see if directory exists
    int dirInode = testPath(dir);

    // Check permissions for rmdir (just needs write on parents dir)
    testPermissions(get_parent_path(dir), false, true, false);

    json requestForm = {
        {"call", "my_rmdir"},
        {"path", dir}};
    json res_json = request(requestForm);

    if (!res_json["status"])
    {
        throw ERROR_dir_not_empty;
    }
}

//******************************************************************************

void Shell::my_mkdir()
{
    string help = "usage: mkdir path/to/directory";
    string dir;

    handleSeekHelp(help);

    if (!currentCommand[1])
    {
        throw ERROR_args_missing;
    }

    if (currentCommand[2])
    {

        throw ERROR_args_overflow;
    }

    dir = to_abspath(curDir, currentCommand[1]);

    testPath(get_parent_path(dir));

    if (testPath(dir, true) != -1)
    {
        throw ERROR_file_exists;
    }

    // Check permissions for mkdir (just needs write)
    testPermissions(get_parent_path(dir), false, true, false);

    json requestForm = {
        {"call", "my_mkdir"},
        {"path", dir}};
    json res_json = request(requestForm);
}

//******************************************************************************

void Shell::my_Lcp()
{
    string help = "usage: Lcp filesystem/source machine/destination_folder";
    string source;
    string destination;
    int source_inode;
    char curDir_machine[3000];      //directory on underlying machine
    ofstream file;
    int file_size;
    char *buffer;
    int inode_num;

    handleSeekHelp(help);

    //set absolute path of file on host machine
    getcwd(curDir_machine, 3000);

    //sees if the function was called incorrectly
    if (!currentCommand[2])
    {
        throw ERROR_args_missing;
    }

    if (currentCommand[3])
    {
        throw ERROR_args_overflow;
    }

    //set the paths needed
    source = to_abspath(curDir, currentCommand[1]);
    destination = to_abspath(curDir_machine, currentCommand[2]);

    //check the path and get the size of file to transfer
    inode_num = testPath(source);
    
    json size_res_json = request({{"call", "my_Read_Size"}, {"inodeNumber", inode_num}});
    file_size = size_res_json["size"];

    //check permissions to read file
    testPermissions(source, true, false, false);




    struct stat s;
    if( stat(destination.c_str(),&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            
            // Gets the file name from the source path
            string destination_filename = get_filename(source);

            // Inserts in to the destination path
            destination.append("/");
            destination.append(destination_filename);
        }
        remove(destination.c_str());
    }


    //create the file on the underlying system
    file.open(destination, ios::out | ios::app | ios::binary);

    if (file.is_open())
    {

        cout << "Transfering file to underlying system...." << endl;

        int buf_size = STD_buffer;
        int position = 0;
        bool successful = false;
        while (1)
        {
            // If the buffer size is too big to send the chunk, it sets the buffer size to the remaining chunk size
            if ((position + buf_size) >= file_size)
            {
                buf_size = (file_size - position);
            }

            // Reads the buffer piece to underlying file <-- needs to change to reading from fs TO underlying file
            char *buffer_piece = new char[buf_size];

            //json for reading from the file
            json chunkForm = {{"call", "my_read"},
                              {"position", position},
                              {"nBytes", buf_size},
                              {"size", file_size},
                              {"inodeNumber", inode_num}};
            request_read(chunkForm, buffer_piece);  //gives you the number of bytes read

            //put buffer_piece into destination file
            file.write(buffer_piece, buf_size);

            //move position so you write to the next part
            position += buf_size;

            // Print the percentage
            cout << "\r" << (int)(((float)position / (float)file_size) * 100) << "%                   ";

            if (position >= file_size)
            {
                successful = true;
                break;
            } // TODO IMPLEMENT AN ERROR HANDLER
        }
        cout << endl;

        if (successful)
        {
            cout << "Success! File " << source << " was copied!" << endl;
            file.close();
        }
        else
        {
            throw ERROR_file_not_created;
        }
    }
    else
    {
        throw ERROR_notfound;
    }
}

//******************************************************************************

void Shell::my_Icp()
{
    string help = "usage: Icp machine/source filesystem/destination";
    string source_path;
    string destination_path;
    string filename;
    int destination_inode_number;
    char curDir_machine[3000];
    fstream file;
    int file_size;
    char *buffer;

    // Sees if it needs help (-h --help)
    handleSeekHelp(help);

    // Gets the absolute path of the file coming in
    getcwd(curDir_machine, 3000);

    // Sees if there is any missing or overflowing args
    if (!currentCommand[2] || !currentCommand[1])
    {
        throw ERROR_args_missing;
    }

    if (currentCommand[3])
    {
        throw ERROR_args_overflow;
    }

    // Gets the absolute paths from both source and destination (machine and FS)
    source_path = to_abspath(curDir_machine, currentCommand[1]);
    destination_path = to_abspath(curDir, currentCommand[2]);

    if(testDirectory(destination_path,true)){
        // Verifies if the user has permission to Icp (just write)
        testPermissions(destination_path, false, false, true);
    }else{
        testPath(get_parent_path(destination_path));
        testPermissions(get_parent_path(destination_path), false, false, true);

    }

    file.open(source_path, ios::in | ios::binary);

    if (file.is_open())
    { // File exists!

        cout << "Loading file...." << endl;

        // Gets file size in bytes and file data in a buffer;
        file.seekg(0, ios::end);
        file_size = file.tellg();
        file.seekg(0, ios::beg);

        buffer = new char[file_size];
        file.read(buffer, file_size);


        if(testDirectory(destination_path,true)){   
            filename = get_filename(source_path);
            // Inserts in to the destination path
            if (destination_path != "/")
            {
                destination_path.append("/");
            }

            destination_path.append(filename);
        }else{
            filename = get_filename(destination_path);
        }

        if(testDirectory(destination_path,true)){
            throw ERROR_file_exists;
        }

        // Creates the destination path (file)
        json create_dest_path_res = request({{"call", "my_create"}, {"path", destination_path}});
        destination_inode_number = create_dest_path_res["inodeNumber"];

        if (destination_inode_number == -1)
        {
            throw ERROR_file_not_imported;
        }

        cout << "Transfering file to FS...." << endl;

        int buf_size = STD_buffer;
        int position = 0;
        bool successful = false;
        while (position < file_size)
        {

            // If the buffer size is too big to send the chunk, it sets the buffer size to the remaining chunk size
            if ((position + buf_size) >= file_size)
            {
                buf_size = (file_size - position);
            }

            // Reads the buffer piece
            char *buffer_piece = new char[buf_size];
            file.seekg(position, ios::beg);
            file.read(buffer_piece, buf_size);
            file.seekg(0, ios::beg);

            json chunkForm = {{"call", "my_write"},
                              {"position", position},
                              {"nBytes", buf_size},
                              {"size", file_size},
                              {"inodeNumber", destination_inode_number}};
            json res = request_write(chunkForm, buffer_piece);

            if(res["status"] == 0){
                break;
            }

            position += buf_size;

            // Print the percentage
            cout << "\r" << (int)(((float)position / (float)file_size) * 100) << "%                   ";

            if (position >= file_size)
            {
                successful = true;
            }
        }
        cout << endl;

        json size_res_json = request({{"call", "my_Read_Size"}, {"inodeNumber", destination_inode_number}});
        int remote_file_size = size_res_json["size"];

        if(remote_file_size != file_size){
            remove_file(destination_path);
            throw ERROR_notenough_space;
        }

        if (successful)
        {
            cout << "Success! File " << filename << " created" << endl;
        }
        else
        {
            throw ERROR_file_not_created;
        }
    }
    else
    {
        throw ERROR_notfound;
    }
}

//******************************************************************************

void Shell::my_cat()
{
    string help = "usage: cat path/to/file";
    string file_path;
    int file_size;

    handleSeekHelp(help);

    if (!currentCommand[1])
    {
        throw ERROR_args_missing;
    }

    int i = 1;
    while(currentCommand[i] != nullptr){
        try{
            file_path = to_abspath(curDir, currentCommand[i]);

            int file_inode_number = testPath(file_path);

            if(testDirectory(file_path,true)){
                throw ERROR_not_file;
            }

            // Testing permissions with the file we are trying to read
            testPermissions(file_path, true, false, false);

            // Gets the size of the file
            json size_res_json = request({{"call", "my_Read_Size"}, {"inodeNumber", file_inode_number}});
            file_size = size_res_json["size"];

            int position = 0;
            int buffer_size = 1024;
            while (position < file_size)
            {

                if ((position + buffer_size) >= file_size)
                {
                    buffer_size = file_size - position;
                }

                char *block_received = new char[buffer_size];
                int size_ = request_read({{"call", "my_read"},
                                        {"inodeNumber", file_inode_number},
                                        {"nBytes", buffer_size},
                                        {"size", file_size},
                                        {"position", position}},
                                        block_received);

                int i = 0;
                while (i < buffer_size)
                {
                    cout << block_received[i];
                    i++;
                }

                position += buffer_size;
                /// cout<<endl<<count<<" | "<<(int)size_res_json["size"]<<endl;
            }
            cout << endl;

        }catch(string e){
            cout<<e<<endl;
        }
        i++;
    }
}

//******************************************************************************

void Shell::my_cp()
{
    string help = "usage: cp source destination";
string source;
    string destination;
    int destination_inode_number;
    int source_inode_number;

    handleSeekHelp(help);

    if (!currentCommand[2])
    {
        throw ERROR_args_missing;
    }

    if (currentCommand[3])
    {
        throw ERROR_args_overflow;
    }

    // Gets their absolute path
    source = to_abspath(curDir, currentCommand[1]);
    destination = to_abspath(curDir, currentCommand[2]);

    // Test if they exist, source must exist at all costs
    source_inode_number = testPath(source);
    destination_inode_number = testPath(destination, true);

    // Tests if destinations parent exists (if not even its parents exists, throw error)
    testPath(get_parent_path(destination));

    // See if we can copy from source
    testPermissions(source, true, false, false);

    // If the destination didn't exist before, creates it
    if (destination_inode_number == -1 && !testDirectory(source, true))
    {
        json create_dest_path_res = request({{"call", "my_create"}, {"path", destination}});
        destination_inode_number = create_dest_path_res["inodeNumber"];

        if (destination_inode_number == -1)
        {
            throw ERROR_file_not_created;
        }
    }

    if(testDirectory(source, true)){
       json requestForm = {
        {"call", "my_mkdir"},
        {"path", destination}};
        json res_json = request(requestForm); 
        destination_inode_number = testPath(destination);
    }
    else if (testDirectory(destination, true))
    {
        string source_filename = get_filename(source);

        // Inserts in to the destination path
        if (destination != "/")
        {
            destination.append("/");
        }

        destination.append(source_filename);

        json create_dest_path_res = request({{"call", "my_create"}, {"path", destination}});
        destination_inode_number = create_dest_path_res["inodeNumber"];
    }

    // Tests if destinations parent has permissions (if not even its parents exists, throw error)
    testPermissions(get_parent_path(destination), false, true, false);

    json copy_res_json = request({{"call", "copy_data"},
                                  {"source", source_inode_number},
                                  {"destination", destination_inode_number}});

    if (copy_res_json["status"] == nullptr || !copy_res_json["status"])
    {
        throw ERROR_generic;
    }
}

//******************************************************************************

// TODO
void Shell::my_mv()
{
    string help = "usage: mv source destination";
    handleSeekHelp(help);
}

//******************************************************************************

// TODO
void Shell::my_ln()
{
    string help = "usage: ln source destination";
    handleSeekHelp(help);
}

//******************************************************************************

void Shell::my_chown()
{
    string help = "usage: chown [UID:GID or UID] path/to/fileORdirectory";
    long UID;
    long GID;
    bool mini_rc_for_GID = false;
    string sourcePath;
    int srciNodeNumber;

    handleSeekHelp(help);
    
    //Check that input contains path, and new user of file.
    if(!currentCommand[2]){
        throw ERROR_args_missing;
    }
    else if(currentCommand[3]) {
        throw ERROR_args_overflow;
    }

    //Gets the path of the directory and tests to see if it exists
    sourcePath = to_abspath(curDir,currentCommand[2]);
    srciNodeNumber = testPath(sourcePath);

    //Split UID:GID into UID and GID
    char** uid_gid = line_splitter(currentCommand[1],":");
    
    //Checks to see if the new user exists (currently the shell deals with the user list)
    char *output;
    UID = strtol(uid_gid[0], &output, 10);

    if(users[UID] == nullptr || strcmp(uid_gid[0], to_string(UID).c_str())){
        throw ERROR_user_notfound;
    }

    if(uid_gid[1] != nullptr){
        GID = strtol(uid_gid[1], &output, 10);
        for(int i = 0; i<groups.size();i++){
            if(!strcmp(uid_gid[1], to_string(groups[i]).c_str())){
                mini_rc_for_GID = true;
                break;
            }
        }
        if(!mini_rc_for_GID){
            throw ERROR_group_notfound;
        }
    }

    //Gets the UID of the file and checks to see if the current user can edit the ownership
    json res_perms = request({{"call","my_Read_UID"},{"inodeNumber",srciNodeNumber}});    
    if(user["UID"] != res_perms["UID"]){
        throw ERROR_perm_denied;
    }

    //Changes the UID:
    request({{"call","set_UID"},
    {"inodeNumber",srciNodeNumber},
    {"UID",UID}});    

    //Changes the GID:
    if(mini_rc_for_GID){
        request({{"call","set_GID"},
        {"inodeNumber",srciNodeNumber},
        {"GID",GID}});   
    }

   //cout<<"Changing from "<<res_perms["UID"]<<endl;
    //cout<<"to "<<UID<<endl;

}

//******************************************************************************

void Shell::my_rm()
{
    string sourceDIR;
    string sourcePath;
    string help = "usage: rm path/to/file";
    int diriNodeNum;
    int filePOS;

    handleSeekHelp(help);

    //Check that input contains path
    if(!currentCommand[1]){
        throw ERROR_args_missing;
    }
    else if(currentCommand[2]) {
        throw ERROR_args_overflow;
    }

    sourcePath = to_abspath(curDir, currentCommand[1]);
    sourceDIR = get_parent_path(sourcePath);
    
    testPath(sourcePath);

    //Confirm that the user has write permissions and that the path exists. If either of them do not, return failure to the user via shell and reason for failure.
    testPermissions(sourceDIR, false, true, false);

    remove_file(sourcePath);


}

//******************************************************************************

/*Function will be called in the main function every time a new message
is received, and then returns the response to the client (return cstr).
The decode (decode = parsed json) and handling of the encoded message
 (encoded = stringfied json ) would be in here */
void Shell::execute(string msg)
{
    try
    {
        currentCommand = line_splitter(&msg[0], " ");

        if (!strcmp(currentCommand[0], "ls"))
            my_ls();
        else if (!strcmp(currentCommand[0], "cd"))
            my_cd();
        else if (!strcmp(currentCommand[0], "mkdir"))
            my_mkdir();
        else if (!strcmp(currentCommand[0], "rmdir"))
            my_rmdir();
        else if (!strcmp(currentCommand[0], "Lcp"))
            my_Lcp();
        else if (!strcmp(currentCommand[0], "Icp"))
            my_Icp();
        else if (!strcmp(currentCommand[0], "cat"))
            my_cat();
        else if (!strcmp(currentCommand[0], "cp"))
            my_cp();
        else if (!strcmp(currentCommand[0], "mv"))
            my_mv();
        else if (!strcmp(currentCommand[0], "ln"))
            my_ln();
        else if (!strcmp(currentCommand[0], "rm"))
            my_rm();
        else if (!strcmp(currentCommand[0], "chown"))
            my_chown();
        else if (!strcmp(currentCommand[0], "clear"))
            ::system("clear");
        else
        {
            cout << "shell: command not found: " << currentCommand[0] << endl;
        }
    }
    catch (string e)
    {
        cout << currentCommand[0] << ": " << e << endl;
    }
}

int main(int argc, char *argv[])
{
    string help = "usage ./shell [0, 1 or 2 for the user to be used]\n*\n*";
    ::system("clear");
    Shell shell = Shell("127.0.0.1", 230, 4096);
    char msg[shell.STD_buffer];
    shell.groups = {2, 1, 0}; //[GID_0,GID_1......]
    shell.users = {shell.groups[2], shell.groups[2], shell.groups[1]}; //[GID_user0,GID_user1......]

    try
    {
        if (!argv[1])
            throw help;

        int user = ((int)argv[1][0] - 48);
        shell.user = {{"GID", shell.users[user]}, {"UID", user}};

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
            str.erase(std::remove(str.begin(), str.end(), ' '), str.end()); // Verfiy if the currentCommand is empty

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
    }
    catch (string e)
    {
        cout << "*\n*\n./shell: " << e << endl;
    }
    return 0;
}
