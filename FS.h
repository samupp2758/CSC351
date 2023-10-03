



//******************************************************************************

#ifndef __P1_H
#define __P1_H

//******************************************************************************

#include <fstream> //Manipulate files

using namespace std;

//******************************************************************************

class FileSystem {
    private:
        const int BLOCKSIZE = 8192;
        const int MAXBLOCKS = 32768;

    public:
        fstream disk;

        FileSystem();
        ~FileSystem();

        void clearFS();
        void createDataFile(long size);
        char* readBlock(int blockNumber);
        void writeBlock(int blockNumber, char* data);
        int readFileIn(string FileName, int startingBlock);
        void readFileOut(string FileName, int startingBlock, int numberOfBlocks);

};

//******************************************************************************

#endif
