#include <fstream> //Manipulate files
#include <iostream> //Give access to cout
#include <math.h> //Access to pow
#include <string> //Access to strings
#include "FS.h"


using namespace std;

//******************************************************************************

void FileSystem::clearFS() {
    fstream FSData;
    FSData.open("disk.dat", ios::out | ios::trunc);
    FSData.close();

}

//******************************************************************************

void FileSystem::createDataFile(long size) {
    fstream FSData;
    FSData.open("disk.dat", ios::out | ios::in | ios::binary | ios::trunc);
    char empty = 0;
    for (unsigned long i = 0; i < size; i++) {
        FSData.write(&empty, 1); 
    }
}

//******************************************************************************

char* FileSystem::readBlock(int blockNumber) {
    char* dataBlock;
    if (blockNumber >= 0 && blockNumber <= MAXBLOCKS) {
        dataBlock = new char [BLOCKSIZE];
        int position = blockNumber * BLOCKSIZE;
        disk.seekg(position);
        disk.read(dataBlock, BLOCKSIZE);
    } else {
        dataBlock = NULL;
        cout << "Invalid blockNumber\n";
    }
    
    return dataBlock;
}

//******************************************************************************

void FileSystem::writeBlock(int blockNumber, char* data) {
    if (blockNumber >= 0 && blockNumber <= MAXBLOCKS) {
        int position = blockNumber * BLOCKSIZE;
        disk.seekp(position);
        disk.write(data, BLOCKSIZE);
        //cout << data << endl << endl;
    } else {
        cout << "Invalid blockNumber\n";
    }
}

//******************************************************************************

FileSystem::FileSystem() {
    disk.open("disk.dat", ios::out | ios::in | ios::binary);
}

//******************************************************************************

FileSystem::~FileSystem() {
    disk.close();
}

//******************************************************************************

int FileSystem::readFileIn(string FileName, int startingBlock) {
    fstream file;
    file.open(FileName, ios::in | ios::binary);
    file.seekg(0, ios::end);
    int length = file.tellg();
    file.seekg(0, ios::beg);
    char* fileData = new char[length];
    file.read(fileData, length);

    //cout << data;
    char* blockData = new char[BLOCKSIZE];
    for (int i = 0; i < ceil((float)length / (float)BLOCKSIZE); i++) {
        //Pass in a pointer to the current index which makes it its own array.
        //write does not mind too little data, just puts null.
        //writeBlock(i + startingBlock, &data[i*BLOCKSIZE]);
        //writeBlock(i + startingBlock, &data[i*BLOCKSIZE]);
        for (int j = 0; j < BLOCKSIZE; j++) {
            blockData[j] = fileData[i * BLOCKSIZE + j];
            if ((i * BLOCKSIZE + j) >= length) {
                blockData[j] = 0;
            }
        }
        writeBlock(i + startingBlock, blockData);
        //Data is not cleared between writes to block. This means left over data can get
        // written again at the last run, when only the first elemetn were overwritten.
    }
    
    delete fileData, blockData;
    return ceil((float)length / (float)BLOCKSIZE);
}

//******************************************************************************

void FileSystem::readFileOut(string FileName, int startingBlock, int numberOfBlocks) {
    fstream file;
    char* fileData = new char [numberOfBlocks * BLOCKSIZE];

    for (int i = 0; i < numberOfBlocks; i++) {
        char* blockData = readBlock((i + startingBlock));
        for (int j = 0; j < BLOCKSIZE; j++) {
            fileData[i * BLOCKSIZE + j] = blockData[j];
        }
        cout << readBlock((i + startingBlock)) << endl;
    }

    file.open(FileName, ios::out | ios::binary);
    file.write(fileData, numberOfBlocks * BLOCKSIZE);
    delete fileData;
}

//******************************************************************************

int main() {
    FileSystem FS;
    FS.clearFS();
    FS.createDataFile(1024*1024);

    //string strData = "HelloWorldBlock1";
    //char *data = &strData[0];

    //FS.writeBlock(0, data);



    //char* results = FS.readBlock(0);  

    //int numBlocks = FS.readFileIn("demo.txt", 0);
    //FS.readFileOut("demo2.txt", 0, 22);

    //cout << numBlocks;
    //FSData << "some text f\n";
    //FSData.seekp(30);
    //FSData << "some text f2";
    FS.disk.close();
}