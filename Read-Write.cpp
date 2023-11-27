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

//******************************************************************************

void FileSystem::clearFS() {
    disk.close();
    disk.open("disk.dat", ios::out | ios::trunc);
    disk.close();

}

//******************************************************************************

void FileSystem::createDataFile(unsigned int size, string name) {
    disk.close();
    disk.open(name, ios::out | ios::in | ios::binary | ios::trunc);
    char empty = 0;
    char* empty2 = new char[1024 * 1024];
    for (int i = 0; i < 1024 * 1024; i++) {
        empty2[i] = 0;
    }
    for (int i = 0; i < (size / (1024 * 1024)); i++) {
        disk.write(empty2, 1024 * 1024);
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
        //cerr << blockNumber << endl;
    } else {
        dataBlock = NULL;
        cerr << "Invalid blockNumber " << blockNumber << "\n";
    }
    
    return dataBlock;
}

//******************************************************************************

void FileSystem::writeBlock(int blockNumber, char* data) {
    if (blockNumber >= 0 && blockNumber <= MAXBLOCKS) {
        int position = blockNumber * BLOCKSIZE;
        disk.seekp(position);
        disk.write(reinterpret_cast<const char*>(data), BLOCKSIZE);
    } else {
        cout << "Invalid blockNumber\n";
    }
}

//******************************************************************************

FileSystem::FileSystem(string name) {
    disk.open(name, ios::out | ios::in | ios::binary);
    //FileName = name;
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
        delete blockData;
    }

    file.open(FileName, ios::out | ios::binary);
    file.write(fileData, numberOfBlocks * BLOCKSIZE);
    delete fileData;
}

//******************************************************************************

void FileSystem::my_Set_Mode(int inodeNumber, char* mode) {
    //int blockNumber = (inodeNumber / 32) + 18;
    //int offset = (inodeNumber % 32) * 128;

    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    buffer[offset] = mode[0];
    buffer[offset + 1] = mode[1];
    writeBlock(blockNumber, buffer);
    //my_Set_CTime(inodeNumber);
    delete buffer;
}

//******************************************************************************

char* FileSystem::my_Read_Mode(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    //position within block
    int offset = (inodeNumber % 32) * 128;
    char* result = new char[2];
    char* buffer = readBlock(blockNumber);
    result[0] = buffer[offset];
    result[1] = buffer[offset + 1];
    //writeBlock(blockNumber, buffer);
    delete buffer;
    return result;
}

//******************************************************************************

void FileSystem::my_Set_UID(int inodeNumber, int UID) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char* cUID = integer_To_Characters(UID);
    buffer[offset + 6] = cUID[0];
    buffer[offset + 7] = cUID[1];
    buffer[offset + 8] = cUID[2];
    buffer[offset + 9] = cUID[3];
    writeBlock(blockNumber, buffer);
    delete cUID, buffer;
}

//******************************************************************************

int FileSystem::my_Read_UID(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cInteger[4];
    cInteger[0] = buffer[offset + 6];
    cInteger[1] = buffer[offset + 7];
    cInteger[2] = buffer[offset + 8];
    cInteger[3] = buffer[offset + 9];

    int result = characters_To_Integer(cInteger);
    delete buffer;
    return result;
}

//******************************************************************************

void FileSystem::my_Set_GID(int inodeNumber, int GID) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char* cGID = integer_To_Characters(GID);
    buffer[offset + 10] = cGID[0];
    buffer[offset + 11] = cGID[1];
    buffer[offset + 12] = cGID[2];
    buffer[offset + 13] = cGID[3];
    writeBlock(blockNumber, buffer);
    delete cGID, buffer;
}

//******************************************************************************

int FileSystem::my_Read_GID(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cInteger[4];
    cInteger[0] = buffer[offset + 10];
    cInteger[1] = buffer[offset + 11];
    cInteger[2] = buffer[offset + 12];
    cInteger[3] = buffer[offset + 13];

    int result = characters_To_Integer(cInteger);
    delete buffer;
    return result;
}

//******************************************************************************

void FileSystem::my_Increment_nlinks(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char* cnlinks = new char[4];
    cnlinks[0] = buffer[offset + 2];
    cnlinks[1] = buffer[offset + 3];
    cnlinks[2] = buffer[offset + 4];
    cnlinks[3] = buffer[offset + 5];
    int nlinks = characters_To_Integer(cnlinks);
    nlinks++;
    cnlinks = integer_To_Characters(nlinks);
    buffer[offset + 2] = cnlinks[0];
    buffer[offset + 3] = cnlinks[1];
    buffer[offset + 4] = cnlinks[2];
    buffer[offset + 5] = cnlinks[3];
    writeBlock(blockNumber, buffer);
    delete cnlinks, buffer;
}

//******************************************************************************

void FileSystem::my_Decrement_nlinks(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char* cnlinks = new char[4];
    cnlinks[0] = buffer[offset + 2];
    cnlinks[1] = buffer[offset + 3];
    cnlinks[2] = buffer[offset + 4];
    cnlinks[3] = buffer[offset + 5];
    int nlinks = characters_To_Integer(cnlinks);
    nlinks--;
    cnlinks = integer_To_Characters(nlinks);
    buffer[offset + 2] = cnlinks[0];
    buffer[offset + 3] = cnlinks[1];
    buffer[offset + 4] = cnlinks[2];
    buffer[offset + 5] = cnlinks[3];
    writeBlock(blockNumber, buffer);
    if (nlinks == 0) {
        my_Delete(inodeNumber);
    }
    delete cnlinks, buffer;
}

//******************************************************************************

int FileSystem::my_Read_nlinks(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cnlinks[4];
    cnlinks[0] = buffer[offset + 2];
    cnlinks[1] = buffer[offset + 3];
    cnlinks[2] = buffer[offset + 4];
    cnlinks[3] = buffer[offset + 5];
    int nlinks = characters_To_Integer(cnlinks);
    delete buffer;
    return nlinks;
}

//******************************************************************************

void FileSystem::my_Set_Size(int inodeNumber, int size) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char* cSize = integer_To_Characters(size);
    buffer[offset + 15] = cSize[0];
    buffer[offset + 16] = cSize[1];
    buffer[offset + 17] = cSize[2];
    buffer[offset + 18] = cSize[3];
    writeBlock(blockNumber, buffer);
    delete cSize, buffer;
}

//******************************************************************************

int FileSystem::my_Read_Size(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cSize[4];
    cSize[0] = buffer[offset + 15];
    cSize[1] = buffer[offset + 16];
    cSize[2] = buffer[offset + 17];
    cSize[3] = buffer[offset + 18];
    int size = characters_To_Integer(cSize);
    delete buffer;
    return size;
}

//******************************************************************************

void FileSystem::my_Set_ATime(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    int ATime = time(nullptr);
    char* cATime = integer_To_Characters(ATime);
    buffer[offset + 79] = cATime[0];
    buffer[offset + 80] = cATime[1];
    buffer[offset + 81] = cATime[2];
    buffer[offset + 82] = cATime[3];
    writeBlock(blockNumber, buffer);
    delete cATime, buffer;
}

//******************************************************************************

int FileSystem::my_Read_ATime(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cATime[4];
    cATime[0] = buffer[offset + 79];
    cATime[1] = buffer[offset + 80];
    cATime[2] = buffer[offset + 81];
    cATime[3] = buffer[offset + 82];
    int ATime = characters_To_Integer(cATime);
    delete buffer;
    return ATime;
}

//******************************************************************************

void FileSystem::my_Set_MTime(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    int MTime = time(nullptr);
    char* cMTime = integer_To_Characters(MTime);
    buffer[offset + 83] = cMTime[0];
    buffer[offset + 84] = cMTime[1];
    buffer[offset + 85] = cMTime[2];
    buffer[offset + 86] = cMTime[3];
    writeBlock(blockNumber, buffer);
    delete cMTime, buffer;
}

//******************************************************************************

int FileSystem::my_Read_MTime(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cMTime[4];
    cMTime[0] = buffer[offset + 83];
    cMTime[1] = buffer[offset + 84];
    cMTime[2] = buffer[offset + 85];
    cMTime[3] = buffer[offset + 86];
    int MTime = characters_To_Integer(cMTime);
    delete buffer;
    return MTime;
}
//******************************************************************************

void FileSystem::my_Set_CTime(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    int CTime = time(nullptr);
    char* cCTime = integer_To_Characters(CTime);
    buffer[offset + 87] = cCTime[0];
    buffer[offset + 88] = cCTime[1];
    buffer[offset + 89] = cCTime[2];
    buffer[offset + 90] = cCTime[3];
    writeBlock(blockNumber, buffer);
    delete cCTime, buffer;
}

//******************************************************************************

int FileSystem::my_Read_CTime(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cCTime[4];
    cCTime[0] = buffer[offset + 87];
    cCTime[1] = buffer[offset + 88];
    cCTime[2] = buffer[offset + 89];
    cCTime[3] = buffer[offset + 90];
    int CTime = characters_To_Integer(cCTime);
    delete buffer;
    return CTime;
}

//******************************************************************************

void FileSystem::my_set_last_ID(int inodeNumber, int GID) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char* cGID = integer_To_Characters(GID);
    buffer[offset + 91] = cGID[0];
    buffer[offset + 92] = cGID[1];
    buffer[offset + 93] = cGID[2];
    buffer[offset + 94] = cGID[3];
    writeBlock(blockNumber, buffer);
    delete cGID, buffer;
}

//******************************************************************************

int FileSystem::my_read_last_ID(int inodeNumber) {
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    char cInteger[4];
    cInteger[0] = buffer[offset + 91];
    cInteger[1] = buffer[offset + 92];
    cInteger[2] = buffer[offset + 93];
    cInteger[3] = buffer[offset + 94];

    int result = characters_To_Integer(cInteger);
    delete buffer;
    return result;
}

//******************************************************************************

//Needs to test
//Returns the number of blocks that the given file
int FileSystem::get_block_use(int inodeNumber) {
    int numberOfBlocks = 0;
    bool done = false;
    int indirectBlock = 0;

    int* addresses = get_addresses(inodeNumber, indirectBlock);
    for (int i = 0; i < 12; i++) {
        if (addresses[i] != 0) {
            numberOfBlocks++;
        } else {
            done = true;
            break;
        }
    }
    delete addresses;

    indirectBlock++;
    while (!done && indirectBlock < 1049602) {
        addresses = get_addresses(inodeNumber, indirectBlock);
        for (int i = 0; i < 1024; i++) {
            if (addresses[i] != 0) {
                numberOfBlocks++;
            } else {
                done = true;
                break;
            }
        }
        delete addresses;
        indirectBlock++;
    }

    return numberOfBlocks;
}

//******************************************************************************

int* FileSystem::my_index_inodes(int inodeNumber) {
    int* result = new int[2];
    result[0] = (inodeNumber / 32) + 18;
    result[1] = (inodeNumber % 32) * 128;
    return result;
}

//******************************************************************************

void FileSystem::mark_inode_free(int inodeNumber) {
    char* buffer = readBlock(17);
    bool* bits = character_To_Binary(buffer[inodeNumber / 8]);
    bits[inodeNumber % 8] = false;
    buffer[inodeNumber / 8] = binary_To_Character(bits);
    writeBlock(17, buffer);
}

//******************************************************************************
//Seems good
void FileSystem::mark_blocks_free(int* blockNumbers, int size) {
    //Takes a list of block numbers and marks them free on the block bit map.
    sort(blockNumbers, blockNumbers + size);
    //for (int i = 0; i < size; i++) {
    //    cout << blockNumbers[i] << endl;
    //}
    //cerr << "size " << size << endl;

    int pos = 0;
    char* buffer;
    int offsetByte;
    bool* bits;
    for (int i = 1; i < 17; i++) {
        if (blockNumbers[pos] < 32768 * i) {
            buffer = readBlock(i);
            while (blockNumbers[pos] < 32768 * i && pos < size) {
                offsetByte = (blockNumbers[pos] % 32768) / 8;
                bits = character_To_Binary(buffer[offsetByte]);
                bits[blockNumbers[pos] % 8] = false;
                buffer[offsetByte] = binary_To_Character(bits);
                delete bits;
                pos++;
            }
            writeBlock(i, buffer);
            delete buffer;
            if (pos >= size) {
                break;
            }
        }
    }
}

//******************************************************************************
//Need to test
void FileSystem::my_Delete(int inodeNumber) {
    //Deletes an inode

    //Free all the blocks of the i-node
    int indirectBlock = 0;
    int* blockNums = get_addresses(inodeNumber, indirectBlock);
    int end = 12;
    for (int i = 0; i < 12; i++) {
        if (blockNums[i] == 0) {
            end = i;
            break;
        }
    }
    mark_blocks_free(blockNums, end);
    delete blockNums;
    if (end == 12) {
        end = 4096;
        while (end == 4096 && indirectBlock < 1049602) {
            indirectBlock++;
            blockNums = get_addresses(inodeNumber, indirectBlock);
            for (int i = 0; i < 4096; i++) {
                if (blockNums[i] == 0) {
                    end = i;
                    break;
                }
            }
            mark_blocks_free(blockNums, end);
            delete blockNums;
        }
    }

    //Set the i-node values to null
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    for (int i = 0; i <= 90; i++) {
        buffer[offset + i] = 0;
    }
    writeBlock(blockNumber, buffer);
    delete buffer;

    //Mark the inode as free
    mark_inode_free(inodeNumber);    
}

//******************************************************************************

//Returns the number of a free block. 0 means no blocks were free.
int FileSystem::single_Allocate() {
    //Returns the number of a free block. 0 means no blocks were free.
    char* buffer;
    int blockNumber = 0;
    int bitmap = 1;
    bool* currentBits;
    bool exit = false;
    while (!blockNumber && bitmap <= 17) {
        buffer = readBlock(bitmap);
        for (int i = 0; i < 4096; i++) {
            currentBits = character_To_Binary(buffer[i]);
            //cout << "char: " << (int)buffer[i] << endl;
            for (int j = 0; j < 8; j++) {
                //cout << j << endl;
                if (!currentBits[j]) {
                    currentBits[j] = true;
                    blockNumber = (bitmap - 1) * 32768 + i * 8 + j;
                    exit = true;
                    buffer[i] = binary_To_Character(currentBits);
                    writeBlock(bitmap, buffer);
                    break;
                }
            }
            delete currentBits;
            if (exit) {
                break;
            }
        }
        delete buffer;
        bitmap++;
    }
    return blockNumber;
}

//******************************************************************************

int FileSystem::allocate() {
    //Returns the beginning number of 8 consecutive free blocks. 0 means no blocks were free.
    char* buffer = new char[5];
    int blockNumber = 0;
    int bitmap = 1;
    int emptyBitCounter = 0;
    int startingBlockTracker = 0;
    bool* currentBits;
    bool exit = false;
    //cout << "allocate 1" << endl;
    while (!blockNumber && bitmap <= 16) {
        //cerr << "allocate bitmap" << bitmap << endl;
        delete buffer;
        buffer = readBlock(bitmap);
        emptyBitCounter = 0;
        //cerr << "bitmap 2" << endl;
        for (int i = 0; i < 4096; i++) {
            currentBits = character_To_Binary(buffer[i]);
            //cout << "char: " << (int)buffer[i] << endl;
            //cout << "allocate 2" << endl;
            for (int j = 0; j < 8; j++) {
                if(currentBits[j] == 1){
                    emptyBitCounter = 0;
                }
                //cout << j << endl;
                if(currentBits[j] == 0){
                    if(emptyBitCounter == 0){
                        startingBlockTracker = j;
                    }
                    //count consecutive free bits
                    emptyBitCounter++;
                }
                //cout << "allocate 3" << endl;
                if(emptyBitCounter == 8 && startingBlockTracker == 0){
                    //cerr << "allocate 4" << endl;
                    //8 free consecutive blocks were found AND its all in the same currentBits
                    //flip the bits in the bitmap
                    for(int x = 0; x < 8; x++){
                        currentBits[x] = true;
                    }
                    exit = true;
                    //beginning block number
                    blockNumber = (bitmap - 1) * 32768 + i * 8;
                    buffer[i] = binary_To_Character(currentBits);
                    writeBlock(bitmap, buffer);
                    break;
                } else if(emptyBitCounter == 8){
                    //cerr << "allocate 5" << endl;
                    //8 consecutive free bits were found but spans multiple currentBit variables
                    //get the currentBit pieces from the previous currentBits (which is where the consecutive 8 started) flipped
                    currentBits = character_To_Binary(buffer[i-1]);
                    for(int x = startingBlockTracker; x < 8; x++){
                        currentBits[x] = true;
                    }
                    //rewrite those to the buffer
                    buffer[i-1] = binary_To_Character(currentBits);
                    //write out the block
                    writeBlock(bitmap, buffer);
                    //get the remaining amount left in the current currentBits flipped
                    currentBits = character_To_Binary(buffer[i]);
                    for(int x = 0; x <= startingBlockTracker-1; x++){
                        currentBits[x] = true;
                    }
                    //put it in the buffer
                    buffer[i] = binary_To_Character(currentBits);
                    //write out the buffer
                    writeBlock(bitmap, buffer);
                    //get the starting number of block
                    blockNumber = (bitmap - 1) * 32768 + (i-1) * 8 + startingBlockTracker;
                    exit = true;
                    break;
                }
            }
            delete currentBits;
            if (exit) {
                break;
            }
        }
        //cerr << "allocate 9" << endl;
        //delete buffer;
        //cerr << "allocate 9.3" << endl;
        bitmap++;
        //cerr << "allocate 9.5" << endl;
    }
    //cerr << "allocate 10" << endl;
    return blockNumber;
}
//******************************************************************************

bool* FileSystem::set_comparePerms(int UID, int GID, int inodeNumber) {
    bool* binaryPerms = new bool[3];
    //cout<<"INODDDEEE: "<<inodeNumber<<endl;
    // get mode with permision bits,UID,GID
    char *mode = my_Read_Mode(inodeNumber);
    int nodeUID = my_Read_UID(inodeNumber);
    int nodeGID = my_Read_GID(inodeNumber);
    // compare default to node perms
    // 2-7
    bool *firstMode = character_To_Binary(mode[0]);
    // 0-2
    bool *secondMode = character_To_Binary(mode[1]);
    //cout << "nUID:"<<nodeUID<<" gID:"<<nodeGID<<endl;
    //cout<<"myUID: "<<UID<<" myGID:"<<GID<<endl;
    
    
    
    // Check if UID = nodeUID
    if (UID == nodeUID) {
        cout<<"can use UID"<<endl;
        // if user owns file, keep user permisions
        binaryPerms[0] = firstMode[2];
        binaryPerms[1] = firstMode[3];
        binaryPerms[2] = firstMode[4];
     // check if caller is member of group
    } else if (GID == nodeGID) {
        cout<<"can use GID"<<endl;
        // if caller is member of group, keep group perms
        binaryPerms[0] = firstMode[5];
        binaryPerms[1] = firstMode[6];
        binaryPerms[2] = firstMode[7];
    } else {
        // keep all world permisions
        cout<<"can use else"<<endl;
        //cout<<"yeeye"<<secondMode[0]<<secondMode[1]<<secondMode[2]<<endl;
        binaryPerms[0] = secondMode[0];
        binaryPerms[1] = secondMode[1];
        binaryPerms[2] = secondMode[2];
        //cout<<"tata"<<binaryPerms[0]<<binaryPerms[1]<<binaryPerms[2]<<endl;
    }


/*
    //now that I have permisions, take U|G|W
    //store to permRes
    //read perms(0,3,6)
    permRes[0] = binaryPerms[0]|binaryPerms[3]|binaryPerms[6];
    //write perms(1,4,7)
    permRes[1] = binaryPerms[1]|binaryPerms[4]|binaryPerms[7];
    //execute perms(2,5,8)
    permRes[2] = binaryPerms[2]|binaryPerms[5]|binaryPerms[8];

    //set perms to permRest---TRIED PASS BY REFERENCE COULDNT FIX,MAYBE ASK NATE HELP
    //perms = permRes;
*/
    //delete dynam alloc memory
    delete mode;
    delete firstMode;
    delete secondMode;
    
    //return rwx perms
    return binaryPerms;
}

//******************************************************************************
//Not Tested
//Return max permisions for path given UID and GID
int FileSystem::my_getPerm(string path, int UID, int GID) {
    bool* perms = new bool[3];
    bool* tempPerms = new bool[3];
    bool flag = true;
    int permCode = 0;
    int i = 1;
    int currentInodeNum = 0;
    int parentInodeNum = -1;
    string currentName = "root";


    //Store permisions defauld rwx for all
    for(int j = 0;j < 3; j++){
        perms[j] = 1;
    }

    //loop through path looking at each directory or file and can still edit
    while (path.length() > i && flag) {
        currentName = "";
        while (path[i] != '/' && path.length() > i) {
            currentName += path[i];
            i++;
            //cout<<"teeeeee"<<i<<endl;
        }
        parentInodeNum = currentInodeNum;
        //set currentInodeNum to i-node for currentPath Name
        my_search_dir(parentInodeNum,currentName,currentInodeNum);
        //get permisions for current path
        tempPerms = set_comparePerms(UID,GID,parentInodeNum);
        
        //cout<<"taaaaaa "<<i<<endl;
        //if more to do check that can execute
        if (path.length() > i && tempPerms[2] == 0) {
            //if cant execute into further directories break loop and set perms ---
            //cout<<"HERE"<<endl;
            flag = false;
            perms[0] = 0;
            perms[1] = 0;
            perms[2] = 0;
        
        //Nothing more to do, set perms to temp perms, we are done
        } else if (path.length() == i) {
            //cout<<"Length:"<<path.length()<<" i: "<<i<<endl;
            flag = false;
            perms = tempPerms;
            //cout <<"toodle"<<perms[0]<<perms[1]<<perms[2]<<endl;
        //if more work to do and can execute
        }
        i++;
    }
    //cout<<perms[0]<<perms[1]<<perms[2]<<endl;
    //take perms convert to int
    for(int j = 2; j > -1 ;j--) {
        //cout << j<<":";
        if (perms[j] == 1) {
            //cout<<perms[j]<<endl;
            permCode += 1 << (2-j);
            //cout<<"perms "<<perms[j]<<" codeCount "<<permCode<<endl;
        }
    }

    //remove dynamic alloc mem for tempPerms
    delete tempPerms;
    return permCode;
}


//******************************************************************************

bool FileSystem::my_extend(int inodeNumber) {
    //Extends the file by 8 blocks. If the i-node ran out of room, it adds as many
    // as many of the allocated entries as possible. The remaining allocated blocks
    // are marked as free.
    //cout << "extend 1" << endl;
    int startingBlock = 0;
    bool rc = false;
    bool successfulAdd;
    //cout << "extend 2" << endl;
    startingBlock = allocate();
    //cout << "extend 3" << endl;
    //cerr << endl;
    //for (int i= 0; i < 8; i++) {
    //    cerr << startingBlock + i << " ";
    //}
    //cout << "extend 4" << endl;


    if(startingBlock != 0) {
        for(int x = startingBlock; x < (startingBlock + 8); x++) {
            //cout << "extend 5" << endl;
            successfulAdd = my_Add_Address(inodeNumber, x);
            //cout << "extend 6" << endl;
            if (!successfulAdd) {
                mark_blocks_free(&x, 1);
            }
        }
        rc = true;
    }
    //cout << "extend 9" << endl;
    return rc;
}

//******************************************************************************
//Need to test
//Returns T/F for success
bool FileSystem::my_Add_Address_Indirect(char* block, int location, int blockNumber, bool full) {
    //Takes in a block and an offset within that block. It then treates that
    // position as an indirect block and tries to add the given address to it.
    int indirectBlock;
    int success = false;
    //Check if the indirect block exists
    //int IDNum = characters_To_Integer(&block[location]);
    //cout << IDNum << endl;
    if (!block[location] && !block[location + 1] && !block[location + 2] && !block[location + 3]) {
    //if (IDNum == 0) {
        //If it doesn't, then create it.
        indirectBlock = single_Allocate();
        if (indirectBlock != 0) {
            char* buffer = readBlock(indirectBlock);
            for (int i = 0; i < 4096; i++) {
                buffer[i] = 0;
            }
            writeBlock(indirectBlock, buffer);
            char* cIndirectBlock = integer_To_Characters(indirectBlock);
            block[location] = cIndirectBlock[0];
            block[location + 1] = cIndirectBlock[1];
            block[location + 2] = cIndirectBlock[2];
            block[location + 3] = cIndirectBlock[3];

            delete cIndirectBlock, buffer;
        } else {
            full = true;
        }
        //cout << "does not exist" << endl;
    } else { //It does exist

        //cout << "does exist" << endl;
        char cIndirectBlock[4];
        cIndirectBlock[0] = block[location];
        cIndirectBlock[1] = block[location + 1];
        cIndirectBlock[2] = block[location + 2];
        cIndirectBlock[3] = block[location + 3];
        indirectBlock = characters_To_Integer(cIndirectBlock);
        //cerr << "location " << location << endl;

    }
    if (!full) {
        char* buffer = readBlock(indirectBlock);
        //cout << "indirect block number " << indirectBlock << endl;
        //cout << "block number " << blockNumber << endl;

        for (int i = 0; i < 4096; i += 4) {
            //Check if an address in null, and thus not in use.
            if (!buffer[i] && !buffer[i + 1] && !buffer[i + 2] && !buffer[i + 3]) {
                char* cBlockNumber = integer_To_Characters(blockNumber);
                buffer[i] = cBlockNumber[0];
                buffer[i + 1] = cBlockNumber[1];
                buffer[i + 2] = cBlockNumber[2];
                buffer[i + 3] = cBlockNumber[3];
                writeBlock(indirectBlock, buffer);
                success = true;
                break;
            }
        }
        delete buffer;
    }
    return success;
}

//******************************************************************************

bool FileSystem::my_Add_Address_DIndirect(char* block, int location, int blockNumber, bool full, int inodeNumber, int& tID) {
    bool success = false;
    int DIndirect;
    if (!block[location] && !block[location + 1] && !block[location + 2] && !block[location + 3]) {
        DIndirect = single_Allocate();
        if (DIndirect != 0) {
            char* buffer = readBlock(DIndirect);
            for (int i = 0; i < 4096; i++) {
                buffer[i] = 0;
            }
            writeBlock(DIndirect, buffer);
            char* cDIndirect = integer_To_Characters(DIndirect);
            block[location] = cDIndirect[0];
            block[location + 1] = cDIndirect[1];
            block[location + 2] = cDIndirect[2];
            block[location + 3] = cDIndirect[3];
            delete buffer, cDIndirect;
        } else {
            full = true;
        }
    } else {
        char cDIndirect[4];
        cDIndirect[0] = block[location];
        cDIndirect[1] = block[location + 1];
        cDIndirect[2] = block[location + 2];
        cDIndirect[3] = block[location + 3];
        DIndirect = characters_To_Integer(cDIndirect);
    }
    if (!full) {
        //cout << "DIndirect" << DIndirect << endl;
        char* buffer = readBlock(DIndirect);
        int ID = my_read_last_ID(inodeNumber);
        //int num = num ? (ID - 2) * 4: 0;
        for (int i = (ID - 2) * 4; i < 4096; i += 4) {
            success = my_Add_Address_Indirect(buffer, i, blockNumber, full);
            //cout << "i " << i << endl;
            if (full) {
                break;
            }
            if (success) {
                //cerr << "ran ran ran" << endl;
                writeBlock(DIndirect, buffer);
                int num = (i/4 + 2);
                //my_set_last_ID(inodeNumber, num);
                tID = num;
                my_set_last_ID(inodeNumber, num);
                //my_read_last_ID(inodeNumber);
                break;
            }
        }
        delete buffer;
    }
    return success;
}

//******************************************************************************
//Need to test
bool FileSystem::my_Add_Address(int inodeNumber, int blockNumber) {
    int* location = my_index_inodes(inodeNumber);
    char* buffer = readBlock(location[0]);
    int offset = location[1] + 19; //Offset of the addresses
    bool found = false;
    bool full = false;

    char cAddress[4];
    int address;
    int tID;

    int ID = my_read_last_ID(inodeNumber);

    //Search the normal addresses in the inode.
    for (int i = 0; i < 12; i++) {
        cAddress[0] = buffer[offset + i * 4];
        cAddress[1] = buffer[offset + i * 4 + 1];
        cAddress[2] = buffer[offset + i * 4 + 2];
        cAddress[3] = buffer[offset + i * 4 + 3];
        address = characters_To_Integer(cAddress);
        if (address == 0) { //Set the empty address to blockNumber.
            char* cBlockNumber = integer_To_Characters(blockNumber);
            buffer[offset + i * 4] = cBlockNumber[0];
            buffer[offset + i * 4 + 1] = cBlockNumber[1];
            buffer[offset + i * 4 + 2] = cBlockNumber[2];
            buffer[offset + i * 4 + 3] = cBlockNumber[3];
            writeBlock(location[0], buffer);
            found = true;
            delete cBlockNumber;
            my_set_last_ID(inodeNumber, 0);
            break;
        }
    }
   
    //Search the first indirect block
    if (!found && ID <= 1) {
        //my_set_last_ID(inodeNumber, 1);
        found = my_Add_Address_Indirect(buffer, location[1] + 67, blockNumber, full);
        writeBlock(location[0], buffer);

        if (!found) {
            my_set_last_ID(inodeNumber, 2);
        }
        //cout << characters_To_Integer(&buffer[(location[1] + 67)]) << endl;
    }
    //Search the double indirect block
    if (!found && !full && ID <= 1025) {

        //tID = my_read_last_ID(inodeNumber);
        found = my_Add_Address_DIndirect(buffer, location[1] + 71, blockNumber, full, inodeNumber, tID);

        //cout << "last" << endl;
        writeBlock(location[0], buffer);
        if (found) {
            my_set_last_ID(inodeNumber, tID);
        }
    }

    //Seaerch the triple indirect block
    if (!found && !full) {
        int TIndirect;
        int TOffset = location[1] + 75;
        if (!buffer[TOffset] && !buffer[TOffset + 1] && !buffer[TOffset + 2] && !buffer[TOffset + 3]) {
            TIndirect = single_Allocate();
            if (TIndirect != 0) {
                char* buffer2 = readBlock(TIndirect);
                for (int i = 0; i < 4096; i++) {
                    buffer2[i] = 0;
                }
                writeBlock(TIndirect, buffer2);
                char* cTDirect = integer_To_Characters(TIndirect);
                buffer[TOffset] = cTDirect[0];
                buffer[TOffset + 1] = cTDirect[1];
                buffer[TOffset + 2] = cTDirect[2];
                buffer[TOffset + 3] = cTDirect[3];
                writeBlock(location[0], buffer);
                delete buffer2, cTDirect;
            } else {
                full = true;
            }
        } else {
            char cTDirect[4];
            cTDirect[0] = buffer[TOffset];
            cTDirect[1] = buffer[TOffset + 1];
            cTDirect[2] = buffer[TOffset + 2];
            cTDirect[3] = buffer[TOffset + 3];
            TIndirect = characters_To_Integer(cTDirect);
        }
        if (!full) {
            char* buffer2 = readBlock(TIndirect);
            for (int i = 0; i < 4096; i += 4) {
                found = my_Add_Address_DIndirect(buffer2, i, blockNumber, full, inodeNumber, tID);
                if (full) {
                    break;
                }
                if (found) {
                    writeBlock(TIndirect, buffer);
                    break;
                }
            }
            delete buffer2;
        }
        if (my_read_last_ID(inodeNumber) != 1026) {
            my_set_last_ID(inodeNumber, 1026);
        }
    }
    if (full) { //Should already be true, but made it explicit.
        found = false;
    }
    
    ID = my_read_last_ID(inodeNumber);

    delete location, buffer;
    return found;
}

//******************************************************************************
//Somewhat tested
int* FileSystem::get_addresses(int inodeNumber, int indirect_block){
    //The first 12 addresses are the indirect block 0.
    //The first indirect block is indirect block 1.
    // -1 means the end of the file has been reach
    // Reading an address of 0 means that there is no address there. NULL.
    //Does not have protections for trying to read indirect blocks that have not
    // been allocated to the file.
    int* result;
    int blockNumber = (inodeNumber / 32) + 18;
    int offset = (inodeNumber % 32) * 128;
    char* buffer = readBlock(blockNumber);
    //cerr << "ran 2.1" << endl;
    if (indirect_block == 0) {
        //cerr << "ran 2.2" << endl;
        //First 12 addresses
        result = new int[12];
        for (int i = 0; i < 12; i++) {
            result[i] = characters_To_Integer(&buffer[i * 4 + 19 + offset]);
        }
    } else if (indirect_block == 1) {
        //cerr << "ran 2.3" << endl;
        //First indirect block
        result = new int[1024];
        int indirectBlockNumber = characters_To_Integer(&buffer[67 + offset]);
        char* indirectBuffer = readBlock(indirectBlockNumber);
        for (int i = 0; i < 1024; i++) {
            result[i] = characters_To_Integer(&indirectBuffer[i * 4]);
        }
        delete indirectBuffer;
    } else if (indirect_block < 1026) {
        //cerr << "ran 2.4" << endl;
        char* buffer2 = readBlock((inodeNumber / 32) + 18);
        char* buffer3 = readBlock(characters_To_Integer(&buffer2[(inodeNumber % 32) * 128 + 71]));
        //for (int i = 0; i < 1024; i++) {
        //    cout << characters_To_Integer(&buffer3[i * 4]) << " ";
        //}
        //Double indirect block
        result = new int[1024];
        //?cerr << "is this it" << endl;
        char* DIBuffer = readBlock(characters_To_Integer(&buffer[71 + offset]));
        //?cerr << "DI offset " << indirect_block - 2 << endl;
        //?cerr << "\n DI number " << characters_To_Integer(&DIBuffer[(indirect_block - 2) * 4]) << endl;
        char* IBuffer = readBlock(characters_To_Integer(&DIBuffer[(indirect_block - 2) * 4]));
        for (int i = 0; i < 1024; i++) {
            result[i] = characters_To_Integer(&IBuffer[i * 4]);
        }
        delete DIBuffer, IBuffer;
    } else if (indirect_block < 1049602) {
        result = new int[1024];
        char* TIBuffer = readBlock(characters_To_Integer(&buffer[75 + offset]));
        int DINum = (indirect_block - 2) / 1024;
        char* DIBuffer = readBlock(characters_To_Integer(&TIBuffer[DINum * 4]));
        int INum = (indirect_block - 1) % 1024;
        char* IBuffer = readBlock(characters_To_Integer(&DIBuffer[INum * 4]));
        for (int i = 0; i < 1024; i++) {
            result[i] = characters_To_Integer(&IBuffer[i * 4]);
        }
        delete TIBuffer, DIBuffer, IBuffer;
    } else {
        result = new int[1];
        result[0] = -1;
    }
    delete buffer;
    return result;
}

//******************************************************************************

//Somewhat tested
bool FileSystem::add_entry_to_block(int blockNumber, int entryInodeNum, string name) {
    //Seaches through the given block and tries to add a directory entry to said block
    //Must be called on a block with entries. Assumes there is an entry at the
    //start of the block.
    bool result = false;
    char* buffer = readBlock(blockNumber);

    int position = 0;
    int nextEntry;
    int entryEnd;
    int newEntrySize = name.length() + 10;

    //cout << "\n\n" << "Add entry to block" << endl;

    if (!buffer[4] && !buffer[5] && !buffer[6] && !buffer[7]) {
        //If this is the first entry in the block
        char* cEntryInode = integer_To_Characters(entryInodeNum);
        buffer[position] = cEntryInode[0];
        buffer[position + 1] = cEntryInode[1];
        buffer[position + 2] = cEntryInode[2];
        buffer[position + 3] = cEntryInode[3];

        //Set my->next to 4096
        char* cNextEntry = integer_To_Characters(4096);
        buffer[position + 4] = cNextEntry[0];
        buffer[position + 5] = cNextEntry[1];
        buffer[position + 6] = cNextEntry[2];
        buffer[position + 7] = cNextEntry[3];
        //cout << "Block number: " << blockNumber << endl;
        //cout << "NextENtry " << (int)cNextEntry[0] << (int)cNextEntry[1] << (int)cNextEntry[2] << (int)cNextEntry[3] << endl;

        //Store mode either normal of directory file.
        char* mode = my_Read_Mode(entryInodeNum);
        bool* modeBits = character_To_Binary(mode[0]);
        if (!modeBits[0] && !modeBits[0]) {
            buffer[position + 8] = 'n';
        } else if (!modeBits[0] && modeBits[1]) {
            buffer[position + 8] = 'd';
        }

        //Store name length.
        buffer[position + 9] = (char)name.length();

        //cout << name[0] << endl;
        //Writes the name. Assumes name <= 255 characters
        for (int i = 0; i < name.length(); i++) {
            buffer[i + 10] = name[i];
            //cout << name[i];
        }
            //cout << endl;

        result = true;
        writeBlock(blockNumber, buffer);
        delete cEntryInode, cNextEntry, mode, modeBits;
    } else {
        //If this block already has entries
        //cout << "\n";
        while (position < 4096) {
            //cout << "read offset " << characters_To_Integer(&buffer[4 + position]) << endl;
            nextEntry = position + characters_To_Integer(&buffer[4 + position]);
            //cout << "next Entry " << nextEntry << endl;
            entryEnd = position + (int)buffer[9 + position] + 10;
            //cout << "entry end "  << entryEnd << endl;

            //EntryEnd is an index that points to the first free space after the entry
            // at the current position.
            //cout << "nextEntry " << nextEntry << "entryEnd" << entryEnd << endl;
            //?cout << "entry difference " << nextEntry - entryEnd << endl;
            if (nextEntry - entryEnd >= newEntrySize) {
                //Record i-node
                char* cEntryInode = integer_To_Characters(entryInodeNum);
                buffer[entryEnd] = cEntryInode[0];
                buffer[entryEnd + 1] = cEntryInode[1];
                buffer[entryEnd + 2] = cEntryInode[2];
                buffer[entryEnd + 3] = cEntryInode[3];

                //Set my->next = prev->next
                //Get the index of the next entry and subtrack position from it to get the offset.
                //Then subtract the length of the previous entry from the offset to get the new offset.
                char* cNextEntry = integer_To_Characters((nextEntry - entryEnd)); // I think position should be removed
                //?cout << "New offset " << (nextEntry - entryEnd) << endl;
                buffer[entryEnd + 4] = cNextEntry[0];
                buffer[entryEnd + 5] = cNextEntry[1];
                buffer[entryEnd + 6] = cNextEntry[2];
                buffer[entryEnd + 7] = cNextEntry[3];

                //Store mode
                char* mode = my_Read_Mode(entryInodeNum);
                bool* modeBits = character_To_Binary(mode[0]);
                if (!modeBits[0] && !modeBits[0]) {
                    buffer[entryEnd + 8] = 'n';
                } else if (!modeBits[0] && modeBits[1]) {
                    buffer[entryEnd + 8] = 'd';
                }

                //Store name length.
                //?cerr << "Name length stored " << name.length() + position + 10 << endl;
                buffer[entryEnd + 9] = (char)name.length();

                //?cout << "Pre offset updated " << entryEnd << endl;
                //Previous->next = me
                char* cUpdateNextEntry = integer_To_Characters(entryEnd - position);
                buffer[position + 4] = cUpdateNextEntry[0];
                buffer[position + 5] = cUpdateNextEntry[1];
                buffer[position + 6] = cUpdateNextEntry[2];
                buffer[position + 7] = cUpdateNextEntry[3];

                //Writes the name. Assumes name <= 255 characters
                for (int i = 0; i < name.length(); i++) {
                    buffer[entryEnd + i + 10] = name[i];
                }

                result = true;
                writeBlock(blockNumber, buffer);
                delete cEntryInode, cNextEntry, mode, modeBits, cUpdateNextEntry;
                break;
            }
            position = nextEntry;
        }
    }

    delete buffer;
    return result;
}

//******************************************************************************

//Somewhat tested
//Won't work properly until extend has been implimented.
void FileSystem::my_write_dir(int directoryInodeNum, int entryInodeNum, string name) {
    //cout << "Write to directory" << endl;
    //Should check that name < 256 characters.
    int entryLength = 13 + name.length();
    int blockNumber = (directoryInodeNum / 32) + 18;
    int offset = (directoryInodeNum % 32) * 128;
    char* buffer = readBlock(blockNumber);
    int dirSize = characters_To_Integer(&buffer[offset + 15]);
    int lastIndirectBlock;
    int extendResult = true;
    if (dirSize < 49152) {
        lastIndirectBlock = 0;
    } else {
        lastIndirectBlock = ((dirSize - 49152) / 4194304) + 1;
    }

    //Work through the first 12 addresses
    bool placedEntry = false;
    int* blockNums = get_addresses(directoryInodeNum, 0);
    for(int i = 0; i < 12; i++) {
        if (blockNums[i] == 0) {
            extendResult = my_extend(directoryInodeNum);
            if (!extendResult) {
                break;
            }
            my_Set_Size(directoryInodeNum, my_Read_Size(directoryInodeNum) + 8 * 4096); //The size of the directory could be off if extend quietly fails
            delete blockNums;
            blockNums = get_addresses(directoryInodeNum, 0);
        }
        placedEntry = add_entry_to_block(blockNums[i], entryInodeNum, name);
        if (placedEntry) {
            break;
        }
    }
    delete blockNums;

    //Work through every other address
    if (!placedEntry && extendResult) {
        for (int i = 1; i < lastIndirectBlock; i++) {
            blockNums = get_addresses(directoryInodeNum, i);
            for (int j = 0; j < 1024; i++) {
                if (blockNums[i] == 0) {
                    extendResult = my_extend(directoryInodeNum);
                    if (!extendResult) {
                        break;
                    }
                    my_Set_Size(directoryInodeNum, my_Read_Size(directoryInodeNum) + 8 * 4096); //The size of the directory could be off if extend quietly fails
                    delete blockNums;
                    blockNums = get_addresses(directoryInodeNum, i);
                }
                placedEntry = add_entry_to_block(blockNums[i], entryInodeNum, name);
            }
            delete blockNums;
            if (!extendResult) {
                break;
            }
        }
    }
    if (placedEntry) {
        my_Set_MTime(directoryInodeNum);
        my_Increment_nlinks(entryInodeNum);        
    }
}

//******************************************************************************
//Somewhat tested
//Returns the position of the next entry, -1 otherwise.
int FileSystem::my_remove_entry(int directoryInodeNum, int position) {
    //Returns -1 if trying to remove an entry from an empty block
    //Byte 9 and bytes 4-7 are offsets from the current position/start of entry

    //cout << "\nremove directory " << endl;
    //cout << "position: " << position << endl;

    int* blockNums = get_addresses(directoryInodeNum, position/(4194304));
    char* buffer = readBlock(blockNums[(position/4096)%1024]);
    int offset = position % 4096;

    int result = position;

    //cout << "blockNumber: " << blockNums[(position/4096)%1024] << endl;
    my_Decrement_nlinks(characters_To_Integer(&buffer[position]));
    my_Set_MTime(directoryInodeNum);

    //There was no entry in the block
    if (offset == 0 && !buffer[4] && !buffer[5] && !buffer[6] && !buffer[7]) {

        result = -1;
    } else if (offset == 0 && characters_To_Integer(&buffer[4]) == 4096) {

        //If removing the first entry of a block with a single entry
        for (int i = 4; i < 8; i++) {
            buffer[i] = 0;
        }
        writeBlock(blockNums[(position/4096)%1024], buffer);
    } else if (offset == 0) {
        //Removing the first entry from a block with other entries.
        //Overwrites the first entry with the entry immediately after it.
        int nextEntry = characters_To_Integer(&buffer[4]);
        int nextEntrySize = (int)buffer[nextEntry + 9] + 10; //Have fiddled with this 10, but this seems right
        for (int i = 0; i < nextEntrySize; i++) {
            buffer[i] = buffer[i + nextEntry];
        }

        //Get the nextNode's offset and add it to the first node's offset to get the new offset.
        char* newOffset = integer_To_Characters(characters_To_Integer(&buffer[4]) + nextEntry);
        buffer[4] = newOffset[0];
        buffer[5] = newOffset[1];
        buffer[6] = newOffset[2];
        buffer[7] = newOffset[3];

        delete newOffset;
        writeBlock(blockNums[(position/4096)%1024], buffer);
    } else {
        //Remove an entry from the middle of a block
        //Offset is offset within the block. Temp offset is the offset of the previous entry to position in the block.
        //rEntryOffset and newOffset are offsets to the next entry.

        int nextEntry = characters_To_Integer(&buffer[4]);
        int tempOffset = 0;
        //cerr << "nextEntry" << characters_To_Integer(&buffer[4]) << endl;
        while (nextEntry != offset) {
            tempOffset = nextEntry;
            nextEntry = tempOffset + characters_To_Integer(&buffer[4 + tempOffset]);
        }
        int rEntryOffset = characters_To_Integer(&buffer[offset + 4]);
        int newOffset = (offset - tempOffset) + rEntryOffset;
        char* cNewOffset = integer_To_Characters(newOffset);
        buffer[tempOffset + 4] = cNewOffset[0];
        buffer[tempOffset + 5] = cNewOffset[1];
        buffer[tempOffset + 6] = cNewOffset[2];
        buffer[tempOffset + 7] = cNewOffset[3];
        delete cNewOffset;
        result += rEntryOffset;
        writeBlock(blockNums[(position/4096)%1024], buffer);
    }


    delete blockNums, buffer;
    return result;
}

//******************************************************************************
//Somewhat tested
int FileSystem::my_read_dir(int directoryInodeNum, int position, int& inodeNumber, string& name, char& type) {
    //Returns the position of the next directory entry
    //Sets inodeNumber, name, and type to the values stored in the entry.
    //Returns -1 if the end of the file has been reached

    //cout << "\n" << "Read a Directory" << endl;

    int result;
    //cout << "position: " << position << endl;
    //cerr << "ran 1" << endl;
    int* blockNums = get_addresses(directoryInodeNum, position / (4194304));
    //cout << "first block of directory: " << (position/4096)%1024 << endl;
        //cerr << "ran 2" << endl;

    char* buffer = readBlock(blockNums[(position/4096)%1024]);
        //cerr << "ran 3" << endl;

    int offset = position % 4096;
    //cout << "Offset: " << offset << endl;
    //cout << "NextEntry: " << characters_To_Integer(&buffer[offset + 4]) << endl;
    if (offset == 0 && !buffer[4] && !buffer[5] && !buffer[6] && !buffer[7]) {
        //cout << "Block number: " << blockNums[(position/4096)%1024] << endl;
        //cout << "NextENtry Read: " << (int)buffer[4] << (int)buffer[5] << (int)buffer[6] << (int)buffer[7] << endl;
        result = -1;
    } else {
        inodeNumber = characters_To_Integer(&buffer[offset]);
        type = buffer[offset + 8];
            //cerr << "ran 4" << endl;


        string temp = "";
        for (int i = 0; i < (int)buffer[offset + 9]; i++) {
            temp = temp + buffer[offset + 10 + i];
            //cout << (char)buffer[offset + 10 + i];
        }
        //cout << endl;
        name = temp;
        //cout << "read: "<<temp<< endl;
        //cerr << "ran 5" << endl;

        result = position + characters_To_Integer(&buffer[offset + 4]);
    }
    delete blockNums, buffer;
    return result;
}

//******************************************************************************

//Somewhat tested
//Returns the position of the desired entry, -1 otherwise.
int FileSystem::my_search_dir(int dirInode, string name, int& inodeNum) {
    //
    int inode = -1;
    int entryPosition = -1;
    int lastPosition;
    int position = 0;
    string entryName;
    int entryInodeNum;
    char entryType;
    while (inode == -1 && position != -1) {
        lastPosition = position;
        position = my_read_dir(dirInode, position, entryInodeNum, entryName, entryType);
        if (name.compare(entryName) == 0) {
            inode = entryInodeNum;
            entryPosition = lastPosition;
        }
    }
    inodeNum = inode;
    return entryPosition;
}

//******************************************************************************
//Returns the position of the desired entry, -1 otherwise.
int FileSystem::my_search_dir(int dirInode, string name) {
    int num;
    return my_search_dir(dirInode, name, num);
}

//******************************************************************************
//Somewhat tested
//Returns the i-node number of the file with the given path. -1 if it doesn't exist.
int FileSystem::my_readPath(string path, int& parentInode, string& name) {
    //Returns the inode number of the file at the end of the given path/
    //Returns -1 if the last file does not exist.
    //Doesn't work if one of the directories does not exist.
    bool flag = true;
    int i = 1;
    int currentInodeNum = 0;
    int parentInodeNum = -1;
    string currentName = "root";

    while(path.length() > i) {
        currentName = "";
        while (path[i] != '/' && path.length() > i) {
            currentName += path[i];
            i++;
        }
        //cout << currentName << endl;
        parentInodeNum = currentInodeNum;
        int worked = my_search_dir(parentInodeNum, currentName, currentInodeNum);
        if (worked == -1) {
            currentInodeNum = -1;
            break;
        }
        i++;
    }
    //cerr << currentName;
    name.assign(currentName);
    parentInode = parentInodeNum;

    return currentInodeNum;
};

//******************************************************************************

int FileSystem::my_readPath(string path, int&parentInode) {
    string name;
    return my_readPath(path, parentInode, name);
}

//******************************************************************************
//Shouldn't need to test
int FileSystem::my_readPath(string path) {
    int num;
    return my_readPath(path, num);
};

//******************************************************************************
//Somewhat tested
//Returns the i-node number of the created directory.
int FileSystem::my_mkdir(string path, int user, int group) {
    char mode[2] = {125, 160};
    int myInodeNum = create_inode(mode, user, group);
    my_Set_Size(myInodeNum, 4096);
    int parentInodeNum;
    string name;
    my_readPath(path, parentInodeNum, name);
    my_write_dir(parentInodeNum, myInodeNum, name);
    cout << "mkdir " << name << endl;
    return myInodeNum;
}

//******************************************************************************
//Somewhat tested
bool FileSystem::my_rmdir(string path) {
    //Maybe should return a boolean success fail
    bool success = false;
    string name;
    int parInodeNum;
    int myInodeNum;
    int throwAwayI;
    string throwAwayS;
    char throwAwayC;
    myInodeNum = my_readPath(path, parInodeNum, name);
    int isEmpty = my_read_dir(myInodeNum, 0, throwAwayI, throwAwayS, throwAwayC);
    if (isEmpty == -1) {
        int position = my_search_dir(parInodeNum, name);
        //cout << "\nposition remove " << position << endl;
        if (position != -1) {
            my_remove_entry(parInodeNum, position);
            success = true;
        }
    }
    cout << "rmdir " << name << " " << (success ? "success" : "failed") << endl;
    return success;
}

//******************************************************************************

//Somewhat tested
int FileSystem::create_inode(char* mode, int user, int group) {
    //Returns -1 if the creation failed.
    char* buffer = readBlock(17);
    bool* currentBits;
    bool exit = false;
    int inodeNumber = -1;
    int blockNumber = single_Allocate();
    if (blockNumber != 0) { //If there is room to create the file.
        for (int i = 0; i < 4096; i++) {
            currentBits = character_To_Binary(buffer[i]);
            for (int j = 0; j < 8; j++) {
                if (!currentBits[j]) {
                    currentBits[j] = true;
                    buffer[i] = binary_To_Character(currentBits);
                    writeBlock(17, buffer);
                    inodeNumber = i * 8 + j;
                    exit = true;
                    break;
                }
            }
            if (exit) {
                break;
            }
        }

        if (inodeNumber != -1) {
            my_Set_Mode(inodeNumber, mode);
            my_Set_UID(inodeNumber, user);
            my_Set_GID(inodeNumber, group);
            my_Set_ATime(inodeNumber);
            my_Set_MTime(inodeNumber);
            my_Set_CTime(inodeNumber);

            my_set_last_ID(inodeNumber, 0);
            my_Add_Address(inodeNumber, blockNumber);
        }
    }
    
    return inodeNumber;
} 

//******************************************************************************

// Needs testing and debuging.
// Needs testing.
//Returns an character array of the read data. Returns a pointer to Null if failed.
char* FileSystem::my_Read(int inodeNumber, int position, int nBytes) {
	char* rc;
	char* temp = new char[5];
	rc = new char[nBytes];
	int fileSize = my_Read_Size(inodeNumber);
	int start = 0;
	int startBlock = 0;
	int currentByte = 0;
	int* fileBlocks;
	int indirectBlockNumber = 0;
	int numberOfBlocks = 0;

	if (position < 0|| (position + nBytes) > fileSize) {
		cerr << "start of read position is invalid";
        delete rc, temp;
        rc = NULL; 
	} else {
		
		if (position > 0) {
			start = position % BLOCKSIZE;
			startBlock = (position - start) / BLOCKSIZE;
		}
		
		if (startBlock > 11) { 
			startBlock = startBlock - 12;
			indirectBlockNumber = ((startBlock - (startBlock % 1024)) / 1024) + 1; //find starting indirect block
			startBlock = startBlock % 1024; // mod by size of indirect block map size
		}
		
		fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
		numberOfBlocks = ((nBytes - (nBytes % BLOCKSIZE))/BLOCKSIZE) + 1;
        temp = readBlock(fileBlocks[startBlock]);
        for (int j = start; j < BLOCKSIZE; j++) {
            if (currentByte < nBytes) {
                rc[currentByte] = temp[j];
            } else {
                break;
            }
            currentByte++;
        }

        //cerr << "Number Of Blocks " << numberOfBlocks << endl;
        //cerr << "Starting Block " << startBlock << endl;
		for(int i = startBlock + 1; i < numberOfBlocks + startBlock; i++) {
            //cerr << "called" << endl;
            //cerr << "\nreading" << endl;

			if (currentByte == nBytes) {
				break;
			}
			if (indirectBlockNumber == 0) {
				if (i > 11) {
                    numberOfBlocks = (((nBytes - currentByte) - ((nBytes - currentByte) % BLOCKSIZE))/BLOCKSIZE) + 1;
					i = i - 12;
					indirectBlockNumber++;
                    delete fileBlocks;
					fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
				}
			} else if (indirectBlockNumber > 0) {
				if (i >= 1024) {
                    numberOfBlocks = (((nBytes - currentByte) - ((nBytes - currentByte) % BLOCKSIZE))/BLOCKSIZE) + 1;
					i = i - 1024;
					indirectBlockNumber++;
                    delete fileBlocks;
					fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
				}
			}
			if (fileBlocks[i] == 0) {
				cerr << "no block alocated" << endl;
				break;
			}
            delete temp;
            //cerr << "Block to write to " << fileBlocks[i] << endl;
            //cerr << "currentByte " << currentByte << endl;
			
            temp = readBlock(fileBlocks[i]);
			//cerr << "called";
            for (int j = 0; j < BLOCKSIZE; j++) {
                if (currentByte < nBytes) {
                    rc[currentByte] = temp[j];
                } else {
                    break;
                }
                currentByte++;
            }
		}	
	}

	delete temp, fileBlocks;
	return rc;
}

//******************************************************************************

//Returns T/F based on if the write was successful.
bool FileSystem::my_Write(int inodeNumber, int position, int nBytes, char* buffer) {
    //If the write was unsuccessful, it does add the data to the file. The file will
    // still have the blocks that were allocated to it in the unsuccessful write.
    //If you write off the end of the FS it will just say that the write failed.
	bool success = true;
    bool extendedSuccess = true;
	int numberOfBlocks = 0;
	int currentByte = 0;
	int start = 0;
	int startBlock = 0;
	int indirectBlockNumber = 0;
	char* newBuffer;
	int* fileBlocks;
	newBuffer = new char[BLOCKSIZE];
	if(position > my_Read_Size(inodeNumber)) {
		success = false;
		cerr << "write position greater than the size of the existing file." << endl;
	}
	if (success) {
        /*
		if (position > 0) {
			start = position % BLOCKSIZE;
			startBlock = (position - start) / BLOCKSIZE;
		}
        */
		// if starting block is not in first indirect block map.

        /* debugging. I don't think this was ever getting run due to startingBlock being set to 0.
		if (startBlock > 11) { 
			startBlock = startBlock - 12;
			indirectBlockNumber = ((startBlock - (startBlock % 1024)) / 1024) + 1; //find starting indirect block
			startBlock = startBlock % 1024; // mod by size of indirect block map size
		}
        */
		
		//fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
		//numberOfBlocks = (((nBytes + start) - ((nBytes + start) % BLOCKSIZE))/BLOCKSIZE) + 1;
		
        /*
		// check if my_extend() will ever fail before write / wont be able to complete write.
		int nb = numberOfBlocks;
		int ibn = indirectBlockNumber;	//temp variables
		for (int i = startBlock; i < nb; i++) {
			if (ibn == 0) {
				if (i > 11) {
					nb = nb - 12;
					i = i - 12;
					ibn++;
					fileBlocks = get_addresses(inodeNumber, ibn);
				}
			} else if (ibn > 0) {
				if (i > 1024) {
					nb = nb - 1024;
					i = i - 1024;
					ibn++;
					fileBlocks = get_addresses(inodeNumber, ibn);
				}
			}
			if (fileBlocks[i] == 0) {
				success = my_extend(inodeNumber);
			}
			if (!success) {
				break;
			}
		}
		fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
        */
	}
    //cout << "number of blocks " << numberOfBlocks << endl;
    //cout << "start block " << startBlock << endl;
	if (success) {
	
        /*
		if (start != 0) {
			char * temp = readBlock(fileBlocks[startBlock]);
			for (int i = 0; i < start; i++) {
				newBuffer[i] = temp[i];
			}
			delete temp;
		}
        */
        start = position % BLOCKSIZE;
        
		int* addresses;
        char* buffer2;
        int bytesWritten = 0;
        bool done = false;
        int currentBlock = position / BLOCKSIZE;
        int currentID;
        if (currentBlock < 12) {
            currentID = 0;
        } else {
            currentID = (currentBlock - 12) / 1024 + 1;
        }
        int k = 0;
        if (currentBlock < 12) {
            addresses = get_addresses(inodeNumber, 0);
            for (int i = currentBlock; i < 12; i++) {
                if (addresses[i] == 0) {
                    extendedSuccess = my_extend(inodeNumber);
                    delete addresses;
                    addresses = get_addresses(inodeNumber, 0);

                    if (!extendedSuccess) {
                        break;
                    }
                    //cout << " extended ";
                }
                buffer2 = readBlock(addresses[i]);

                for (int j = start; j < 4096; j++) {
                    if (bytesWritten >= nBytes) {
                        done = true;
                        break;
                    }
                    buffer2[j] = buffer[k];
                    bytesWritten++;
                    k++;
                }
                if (start > 0) {
                    start = 0;
                }
                writeBlock(addresses[i], buffer2);
                currentBlock++;
                delete buffer2;
            }
            delete addresses;
            currentID++;
        }
        //cout << "current id " << currentID << endl;
        //while (!done && extendedSuccess) {
        while (!done) {

            cout << bytesWritten << endl; //Debugging/progress report
            addresses = get_addresses(inodeNumber, currentID);
            //int t = time(nullptr);

            for (int i = (currentBlock - 12) % 1024; i < 1024; i++) {
                //cerr << "ran 1" << endl;
                if (addresses[i] == 0) {
                    //cerr << "ran 1.1" << endl;
                    extendedSuccess = my_extend(inodeNumber);
                    //cerr << "ran 1.2" << endl;
                    delete addresses;
                    addresses = get_addresses(inodeNumber, currentID);

                    if (!extendedSuccess) {
                        //cerr << "exited 1" << endl;
                        break;
                    }
                }
                //cerr << "time: " << (auto begin = std::chrono::high_resolution_clock::now() - t) << endl;
                //t = time(nullptr);

                buffer2 = readBlock(addresses[i]);
                //cerr << "ran 2" << endl;

                for (int j = start; j < 4096; j++) {
                    if (bytesWritten >= nBytes) {
                        done = true;
                        //cerr << "exited 3" << endl;
                        break;
                    }
                    buffer2[j] = buffer[k];
                    bytesWritten++;
                    k++;
                }
                //cerr << "ran 3" << endl;
                if (start > 0) {
                    start = 0;
                }
                writeBlock(addresses[i], buffer2);
                currentBlock++;
                delete buffer2;
                if (done) {
                    //cerr << "exited 1.5" << endl;
                    break;
                }
                //cerr << "ran 4" << endl;
            }
            currentID++;
            delete addresses;
            if (!extendedSuccess) {
                //cerr << "exited 2" << endl;
                break;
            }
        }

        /*
		for (int i = startBlock; i < numberOfBlocks; i++) {
			if (currentByte >= nBytes) {
				break;
			}
			
            
			if (indirectBlockNumber == 0) {
				if (i > 11) {
					numberOfBlocks = numberOfBlocks - 12;
					i = i - 12;
					indirectBlockNumber++;
					fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
				}
			} else if (indirectBlockNumber > 0) {
                cout << "ran" << endl;
				if (i > 1024) {
					numberOfBlocks = numberOfBlocks - 1024;
					i = i - 1024;
					indirectBlockNumber++;
					fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
				}
			}
            
            if (indirectBlockNumber == 0) {
                if (i < 12) {
                    indirectBlockNumber++;
					fileBlocks = get_addresses(inodeNumber, indirectBlockNumber);
                }
            }
			
			for (int j = 0; j < BLOCKSIZE; j++) {
				if (currentByte == nBytes) {
					char * temp = readBlock(fileBlocks[i]);
					for (j; j < BLOCKSIZE; j++) {
						newBuffer[j] = temp[j];
					}
					delete temp;
					break;
				} else {
				newBuffer[j] = buffer[(i * BLOCKSIZE) + j];
				currentByte++;
				}
			}
			writeBlock(fileBlocks[i], newBuffer);	
		}
        */
        if (extendedSuccess) {
            int currentSize = my_Read_Size(inodeNumber);
		    my_Set_Size(inodeNumber, (currentSize >= position + nBytes ? currentSize : position + nBytes));
        }
		my_Set_MTime(inodeNumber);
		cout << "last 7" << endl;
	}
    if (!(success && extendedSuccess)) {
		cerr << "Write has failed" << endl;
	}
	delete newBuffer, fileBlocks;
	return (success && extendedSuccess);
}

//******************************************************************************
   
bool FileSystem::copy_data(int sourceInode, int destInode) {
    bool rc = false;
    int size = my_Read_Size(sourceInode);
    char* buffer;
    buffer = my_Read(sourceInode, 0, size);
    if (buffer) { //Check that read was successful.
        rc = my_Write(destInode, 0, size, buffer);
        delete buffer;
    }
    return rc;
}

//******************************************************************************
//Needs testing
//Returns the inode number of the created file.
int FileSystem::my_create(string path, int user, int group) {
    int parentInode;
    string name;
    int doesExist = my_readPath(path, parentInode, name);
    if (doesExist != -1) {
        int position = my_search_dir(parentInode, name);
        my_remove_entry(parentInode, position);
    }
    char mode[] = {61, 160};
    int myInode = create_inode(mode, user, group);
    my_write_dir(parentInode, myInode, name);
    return myInode;
}


//******************************************************************************
//Somewhat tested
void FileSystem::Create_New_FS(string name) {
    createDataFile(pow(2, 31), name);
    //createDataFile(1024 * 1024 * 8, name);
    //FileName = name;
    char* buffer;

    //Mark 0-1041 as used on the block bitmap
    buffer = readBlock(1);
    bool bits[] = {true, false, false, false, false, false, false, false};
    for (int i = 0; i < 130; i++) {
        buffer[i] = 255;
    }
    buffer[130] = binary_To_Character(bits);
    writeBlock(1, buffer);
    delete buffer;

    //Make the root directory
    //my_make_root();
    bool mode1[] = {false, true, true, true, true, true, true, true};
    bool mode2[] = {true, true, true, false, false, false, false, false};
    char mode[] = {binary_To_Character(mode1), binary_To_Character(mode2)};
    create_inode(mode, 0, 0);
}

//******************************************************************************

//Creates a file with the given name and size filled with random characters.
void FileSystem::make_Test_File(string name, int size) {
    fstream file;
    char* c = new char[1];
    file.open(name, ios::out | ios::trunc);
    for (int i = 0; i < size; i++) {
        c[0] = (char)(rand() % 255);
        file.write(c, 1);
    }
    file.close();
}

//******************************************************************************

bool* FileSystem::character_To_Binary(char c) {
    //Returns a boolean array of length 8, that represents the binary of c.
    bool* binary = new bool [8];

    for (int i = 0; i < 8; i++) {
        //binary[7 - i] = c % 2;
        binary[7 - i] = c & 1;
        c = c >> 1;
    }
    return binary;
}
//******************************************************************************

char FileSystem::binary_To_Character(bool* binary) {
    //Takes a boolean array of length 8 representing some binary value, and
    // converts it into a charater.
    char result = 0;
    for (int i = 0; i < 8; i++) {
        //if (binary[i]) {
        //    result = (char)result + (char)pow(2, 7 - i);
        //}
        result = result << 1;
        result = result | binary[i];
    }
    return result;
}

//******************************************************************************

char* FileSystem::integer_To_Characters(int num) {
    unsigned int unum = (unsigned int)num;
    char* result = new char[4];

    for (int i = 0; i < 4; i++) {
        //result[3-i] = unum % 256;
        result[3-i] = unum & 255;
        unum = unum >> 8;
    }
    return result;
}

//******************************************************************************

int FileSystem::characters_To_Integer(char* chars) {
    unsigned int temp = 0;

    for (int i = 0; i < 4; i++) {
        temp = temp << 8;
        temp = temp | chars[i];
   }
    signed int result = (signed int)temp;
    return result;
}

//******************************************************************************

int FileSystem::decimal_To_Binary(int num) {
    const int size = 32;
    int binaryArray[size];
    return 0;
}

//******************************************************************************

void FileSystem::print_inode_bitmap() {
    char* buffer = readBlock(17);
    bool* bits;
    
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 16; j++) {
            bits = character_To_Binary(buffer[i * 16 + j]);
            for (int k = 0; k < 8; k++) {
                cout << bits[k];
            }
            delete bits;
        }
        cout << endl;
    }
    delete buffer;
}

//******************************************************************************

void FileSystem::print_block_bitmap() {
    char* buffer;
    bool* bits;
    for (int i = 0; i < 16; i++) {
        buffer = readBlock(1 + i);
        for (int j = 0; j < 4096; j++) {
            bits = character_To_Binary(buffer[i]);
            for (int k = 0; k < 8; k++) {
                cout << bits[k];
            }
            delete bits;
        }
        delete buffer;
    }
}

//******************************************************************************
//Somewhat tested
void FileSystem::print_dir(string path) {
    int inode = -1;
    int position = 0;
    string entryName;
    int entryInodeNum;
    char entryType;
    int inodeNum = my_readPath(path);
    //cout << "inode numer of directory: " << inodeNum << endl;
    cout << "ls: ";
    string display = "";
    while (inode == -1 && position != -1) {
        position = my_read_dir(inodeNum, position, entryInodeNum, entryName, entryType);
        display += entryName + " ";
        entryName = "";
    }
    cout << display << endl;
}
