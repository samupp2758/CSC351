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
    FS.my_mkdir("/directory", 15, 15);
    FS.my_mkdir("/pictures", 15, 15);
    FS.my_mkdir("/directory/peeps", 15, 15);
    //FS.my_mkdir("/apples", 15, 15);

    FS.print_dir("/directory");
    //FS.my_rmdir("/pictures");
    //FS.my_rmdir("/directory");
    FS.my_rmdir("/directory/peeps");
    FS.print_dir("/directory");

    
    for (int i = 0; i < 4096; i++) {
        buffer[i] = 'A';
    }
    //FS.writeBlock(1042, buffer);
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
