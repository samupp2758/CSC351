#include <fstream> //Manipulate files
#include <iostream> //Give access to cout
#include <math.h> //Access to pow
#include <string> //Access to strings
#include <time.h> //Access to time
#include<algorithm> //Access to sort
#include "FS.h"



using namespace std;

//Must compile with -funsigned-char
//Assuming all characters are unsigned characters

int main() {
    FileSystem FS("disk.dat");

    //FS.clearFS();
    //FS.createDataFile(1024 * 1024, "disk.dat");

    //char* result = FS.my_Read_Mode(2);

    //cout << result[0] << " " << result[1];
    char* buffer = new char[4096];
    FS.Create_New_FS("disk.dat");
    cout << "done" << endl;
    cout << "done" << endl;
    cout << "done" << endl;
    cout << "done" << endl;
    cout << "done" << endl;

    //FS.print_dir("/");



    //FS.my_create("/delete", 0, 0);
    //int dirInode = FS.my_mkdir("/directory", 15, 15);

    fstream file;
    //file.open("PH1_Design_Document.txt");
    
    
    file.open("Gigafile.txt"); 
    file.seekg(0, ios::end);
    int length = file.tellg();
    file.seekg(0, ios::beg);
    char* fileData = new char[length];
    file.read(fileData, length);
    file.close();

    //int num;
    //FS.my_search_dir(0, "Gigafile2", num);
    cerr << "ran" << endl;
    int inodeNum = FS.my_create("/Gigafile", 0, 0);
    cerr << "ran" << endl;
    bool result = FS.my_Write(inodeNum, 0, length, fileData);
    cout << "\nresult " << result << endl;

    FS.print_block_bitmap();

    //FS.my_set_last_ID(inodeNum, (100/5) + 2);
    //cerr << "test " << FS.my_read_last_ID(inodeNum) << endl;

    /*
    file.open("25MBfile.txt"); 
    file.seekg(0, ios::end);
    length = file.tellg();
    file.seekg(0, ios::beg);
    fileData = new char[length];
    file.read(fileData, length);
    file.close();

    
    result = FS.my_Write(inodeNum, 0, length, fileData);
    cout << "\nresult " << result << endl;
    */


    
    FS.print_dir("/");
   
    /*
    char* c = new char[1];
    file.open("100MBfile.txt", ios::out | ios::trunc);
    for (int i = 0; i < 100 * pow(2, 20); i++) {
        c[0] = (char)(rand() % 255);
        file.write(c, 1);
    }
    file.close();
    */

    /*
    FS.my_mkdir("/things", 0, 0);
    int num = FS.my_mkdir("/moreThings1", 0, 0);
    FS.my_mkdir("/moreThings2", 0, 0);
    FS.my_mkdir("/moreThings3", 0, 0);
    int removed = FS.my_mkdir("/moreThings4", 0, 0);
    FS.my_mkdir("/moreThings5", 0, 0);
    FS.print_dir("/");
    FS.my_rmdir("/moreThings4");
    */


    /*
    FS.my_create("/stuff", 0, 0);
    FS.my_create("/stuff2", 0, 0);
    cerr << "ran 1" << endl;
    FS.my_write_dir(0, 1111, "name");
    FS.my_write_dir(0, 1111, "tttt");
    FS.my_create("/11111111111111111111111111111111111111111111111111111111111111", 0, 0);
    FS.my_create("/2222222222222222222222222222222222222222222222222222222222222222", 0, 0);
    FS.my_create("/333333333333333333333333333333333333333333333333333333333333333", 0, 0);
    FS.my_create("/4444444444444444444444444444444444444444444444444444444444444444", 0, 0);
    FS.my_create("/55555555555555555555555555555555555555555555555555555555555555", 0, 0);
    FS.my_create("/66666666666666666666666666666666666666666666666666666666666666", 0, 0);
    FS.my_create("/test1", 0, 0);
    FS.my_create("/test2", 0, 0);
    FS.my_create("/test3", 0, 0);
    FS.my_create("/test4", 0, 0);
    */

    /*
    for (int i = 0; i < 4096; i++) {
        buffer[i] = 'A';
    }
    */

    //FS.writeBlock(1042, buffer);
    //FS.writeBlock(1044, buffer);

    //FS.my_Write(num, 0, 4000, buffer);



    //FS.writeBlock(1292, buffer);
    //FS.writeBlock(1291, buffer);



    //cout << "creation time of directory " << FS.my_Read_CTime(dirInode) << endl;
    //cout << "CTime of moreThings " << FS.my_Read_CTime(num) << endl;
    //cout << "CTime of removed " << FS.my_Read_CTime(removed) << endl;



    /*
    char mode[] = {63, 224};
    int inodeNumber = FS.create_inode(mode, 1, 1);
    FS.my_write_dir(dirInode, inodeNumber, "text");
    FS.my_Write(inodeNumber, 0, length, fileData);
    
    int nInode = FS.my_create("/directory/NewText", 0, 0);

    cout << "success: " << (FS.copy_data(inodeNumber, nInode) ? "success" : "failed") << endl;
    */

    /*
    
    FS.my_extend(inodeNumber);
    FS.my_extend(inodeNumber);
    FS.my_extend(inodeNumber);
    FS.my_extend(inodeNumber);
    FS.my_extend(inodeNumber);
    FS.my_extend(inodeNumber);

    cout << endl;
    cout << endl;

    int* addresses = FS.get_addresses(inodeNumber, 1);
    for (int i = 0; i< 1024; i++) {
        cout << addresses[i] << " ";
    }
    
    */

    
    cerr << "time to read" << endl;
    int num;
    FS.my_search_dir(0, "Gigafile", num);
    char* result2 = FS.my_Read(num, 0, length);
    //cerr << "ran" << endl;
    file.open("ResultsDoc.txt", ios::out | ios::binary);
    file.write(result2, length);
    file.close();
    

    /*
    cout << endl;
    for (int i = 0; i < length; i++) {
        cout << result[i];
    }
    cout << endl;
    */
    


    //FS.my_mkdir("/pictures", 15, 15);
    //FS.my_mkdir("/directory/peeps", 15, 15);
    //FS.my_mkdir("/apples", 15, 15);

    //FS.print_dir("/directory");
    
    
    //FS.my_rmdir("/pictures");
    //FS.my_rmdir("/directory");
    //FS.my_rmdir("/directory/peeps");
    //FS.print_dir("/directory");

    
    
    
    //FS.writeBlock(1045, buffer);
    //FS.writeBlock(1046, buffer);
    //FS.writeBlock(1047, buffer);
    //FS.writeBlock(1048, buffer);

    

    /*
    FS.writeBlock(1068, buffer);
    FS.writeBlock(1069, buffer);
    FS.writeBlock(1070, buffer);
    FS.writeBlock(1071, buffer);
    FS.writeBlock(1072, buffer);
    FS.writeBlock(1073, buffer);
    */


    /*
    int nums[83];
    for(int i = 0; i < 83; i++) {
        nums[i] = 33000 + i;
    }
    FS.mark_blocks_free(&nums[0], 83);
    */

    //string path = "/the/way/the/truth/and/the/light";
    //int num;
    //FS.my_readPath(path, num);


    /*for (int i = 0; i < 512; i++) {
        buffer[i * 8] = '1';
        buffer[i * 8 + 1] = '2';
        buffer[i * 8 + 2] = '3';
        buffer[i * 8 + 3] = '4';
        buffer[i * 8 + 4] = '5';
        buffer[i * 8 + 5] = '6';
        buffer[i * 8 + 6] = '7';
        buffer[i * 8 + 7] = '8';
    }*/

    /*
    for (int i = 0; i < 4096; i++) {
        buffer[i] = 'A';
    }
    
    char* mode = FS.my_Read_Mode(0);
    bool* mode1 = FS.character_To_Binary(mode[0]);
    bool* mode2 = FS.character_To_Binary(mode[1]);
    int UID = FS.my_Read_UID(0);
    int GID = FS.my_Read_GID(0);
    time_t ATime = (time_t)FS.my_Read_ATime(0);
    time_t MTime = (time_t)FS.my_Read_MTime(0);
    time_t CTime = (time_t)FS.my_Read_CTime(0);

    cout << "Mode: ";
    for(int i = 0; i < 8; i++) {
        cout << mode1[i] << " ";
    }
    for(int i = 0; i < 8; i++) {
        cout << mode2[i] << " ";
    }
    cout << endl;
    cout << "UID: " << UID << endl;
    cout << "GID: " << GID << endl;
    cout << "ATime: " << ctime(&ATime) << endl;
    cout << "MTime: " << ctime(&MTime) << endl;
    cout << "CTime: " << ctime(&CTime) << endl;

    int* address = FS.my_index_inodes(0);
    char* buffer2 = FS.readBlock(address[0]);
    int blockNumber = FS.characters_To_Integer(&buffer2[address[1] + 19]);
    int blockNumber2 = FS.characters_To_Integer(&buffer2[address[1] + 23]);
    int blockNumber3 = FS.characters_To_Integer(&buffer2[address[1] + 27]);
    int blockNumber4 = FS.characters_To_Integer(&buffer2[address[1] + 31]);
    cout << "Address1: " << blockNumber << endl;
    cout << "Address2: " << blockNumber2 << endl;
    cout << "Address3: " << blockNumber3 << endl;
    cout << "Address4: " << blockNumber4 << endl;
    */
    

    /*
    cout << "bits: ";
    bool* bits = FS.character_To_Binary(128);
    for (int i = 0; i < 8; i++) {
        cout << bits[i];
    }
    cout << endl;
    */

    //cout << FS.single_Allocate();

    /*
    bool* data = FS.character_To_Binary(0);
    for (int i = 0; i < 8; i++) {
        cout << data[i] << " ";
    }
    cout << endl;

    cout << (int)FS.binary_To_Character(data);
    */


    //FS.clearFS();
    //FS.createDataFile(1024*1024);

    //string strData = "HelloWorldBlock1";
    //char *data = new char[8192];
    //for (int i = 0; i < 40; i++) {
    //    data[i] = '1';
    //}

    //FS.writeBlock(0, data);



    //char* results = FS.readBlock(0);  

    //int numBlocks = FS.readFileIn("demo.txt", 0);
    //FS.readFileOut("demo2.txt", 0, 1);

    //cout << numBlocks;
    //FSData << "some text f\n";
    //FSData.seekp(30);
    //FSData << "some text f2";

    FS.disk.close();
}
