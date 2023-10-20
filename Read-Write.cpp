#include <fstream> //Manipulate files
#include <iostream> //Give access to cout
#include <math.h> //Access to pow
#include <string> //Access to strings
#include <time.h> //Access to time
#include "FS.h"



using namespace std;

//Must compile with -funsigned-char
//Assuming all characters are unsigned characters

//******************************************************************************

void FileSystem::clearFS() {
    fstream FSData;
    FSData.open("disk.dat", ios::out | ios::trunc);
    FSData.close();

}

//******************************************************************************

void FileSystem::createDataFile(unsigned int size, string name) {
    fstream FSData;
    FSData.open(name, ios::out | ios::in | ios::binary | ios::trunc);
    char empty = 0;
    for (unsigned int i = 0; i < size; i++) {
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
        disk.write(reinterpret_cast<const char*>(data), BLOCKSIZE);
        //cout << data << endl << endl;
    } else {
        cout << "Invalid blockNumber\n";
    }
}

//******************************************************************************

FileSystem::FileSystem(string name) {
    disk.open(name, ios::out | ios::in | ios::binary);
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

//Need to test
int FileSystem::my_adjust_free_blocks(int amount) {
    char* buffer = readBlock(66);
    //cout << buffer[8] << " " << buffer[9] << " " << buffer[10] << " " << buffer[11] << endl;
    int number = characters_To_Integer(&buffer[8]);
    number += amount;
    char* numberChars = integer_To_Characters(number);
    for (int i = 0; i < 4; i++) {
        buffer[8 + i] = numberChars[i];
    }
    //cout << buffer[8] << " " << buffer[9] << " " << buffer[10] << " " << buffer[11] << endl;
    writeBlock(66, buffer);
    delete numberChars, buffer;
    return number;
}

//******************************************************************************
//Need to test
int FileSystem::my_decrement_free_inodes() {
    //Decrements the number of free inodes in the group descriptor, and returns
    //the new value;
    char* buffer = readBlock(66);
    int number = characters_To_Integer(&buffer[12]);
    number--;
    char* numberChars = integer_To_Characters(number);
    for (int i = 0; i < 4; i++) {
        buffer[12 + i] = numberChars[i];
    }
    writeBlock(66, buffer);
    delete numberChars, buffer;
    return number;
}

//******************************************************************************
//Need to test
int FileSystem::my_Increment_free_inodes() {
    //Invrements the number of free inodes in the group descriptor, and returns
    //the new value;
    char* buffer = readBlock(66);
    int number = characters_To_Integer(&buffer[12]);
    number++;
    char* numberChars = integer_To_Characters(number);
    for (int i = 0; i < 4; i++) {
        buffer[12 + i] = numberChars[i];
    }
    writeBlock(66, buffer);
    delete numberChars, buffer;
    return number;
}

//******************************************************************************
//Need to test
int FileSystem::my_read_free_inodes() {
    //Returns the number of free inodes in the group descriptor;
    char* buffer = readBlock(66);
    int number = characters_To_Integer(&buffer[12]);
    delete buffer;
    return number;
}

//******************************************************************************
//Need to test
int FileSystem::my_decrement_directories() {
    //Decrements the number of directories in the group descriptor, and returns
    //the new value;
    char* buffer = readBlock(66);
    int number = characters_To_Integer(&buffer[16]);
    number--;
    char* numberChars = integer_To_Characters(number);
    for (int i = 0; i < 4; i++) {
        buffer[16 + i] = numberChars[i];
    }
    writeBlock(66, buffer);
    delete numberChars, buffer;
    return number;
}

//******************************************************************************
//Need to test
int FileSystem::my_increment_directories() {
    //Increments the number of directories in the group descriptor, and returns
    //the new value;
    char* buffer = readBlock(66);
    int number = characters_To_Integer(&buffer[16]);
    number++;
    char* numberChars = integer_To_Characters(number);
    for (int i = 0; i < 4; i++) {
        buffer[16 + i] = numberChars[i];
    }
    writeBlock(66, buffer);
    delete numberChars, buffer;
    return number;
}

//******************************************************************************
//Need to test
int FileSystem::my_read_directories() {
    //Returns the number of directories in the group descriptor.
    char* buffer = readBlock(66);
    int number = characters_To_Integer(&buffer[16]);
    delete buffer;
    return number;
}
//******************************************************************************

void FileSystem::my_Set_Mode(int inodeNumber, char* mode) {
    //int blockNumber = (inodeNumber / 32) + 84;
    //int offset = (inodeNumber % 32) * 128;

    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    buffer[offset] = mode[0];
    buffer[offset + 1] = mode[1];
    writeBlock(blockNumber, buffer);
    //my_Set_CTime(inodeNumber);
    delete buffer, address;
}

//******************************************************************************

char* FileSystem::my_Read_Mode(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    char* result = new char[2];
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    result[0] = buffer[offset];
    result[1] = buffer[offset + 1];
    //writeBlock(blockNumber, buffer);
    delete buffer, address;
    return result;
}

//******************************************************************************

void FileSystem::my_Set_UID(int inodeNumber, int UID) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char* cUID = integer_To_Characters(UID);
    buffer[offset + 6] = cUID[0];
    buffer[offset + 7] = cUID[1];
    buffer[offset + 8] = cUID[2];
    buffer[offset + 9] = cUID[3];
    writeBlock(blockNumber, buffer);
    my_Set_CTime(inodeNumber);
    delete cUID, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_UID(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cInteger[4];
    cInteger[0] = buffer[offset + 6];
    cInteger[1] = buffer[offset + 7];
    cInteger[2] = buffer[offset + 8];
    cInteger[3] = buffer[offset + 9];

    int result = characters_To_Integer(cInteger);
    delete buffer, address;
    return result;
}

//******************************************************************************

void FileSystem::my_Set_GID(int inodeNumber, int GID) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char* cGID = integer_To_Characters(GID);
    buffer[offset + 10] = cGID[0];
    buffer[offset + 11] = cGID[1];
    buffer[offset + 12] = cGID[2];
    buffer[offset + 13] = cGID[3];
    writeBlock(blockNumber, buffer);
    my_Set_CTime(inodeNumber);
    delete cGID, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_GID(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cInteger[4];
    cInteger[0] = buffer[offset + 10];
    cInteger[1] = buffer[offset + 11];
    cInteger[2] = buffer[offset + 12];
    cInteger[3] = buffer[offset + 13];

    int result = characters_To_Integer(cInteger);
    delete buffer, address;
    return result;
}

//******************************************************************************

void FileSystem::my_Increment_nlinks(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
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
    my_Set_CTime(inodeNumber);
    delete cnlinks, buffer, address;
}

//******************************************************************************

void FileSystem::my_Decrement_nlinks(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
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
    my_Set_CTime(inodeNumber);
    delete cnlinks, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_nlinks(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cnlinks[4];
    cnlinks[0] = buffer[offset + 2];
    cnlinks[1] = buffer[offset + 3];
    cnlinks[2] = buffer[offset + 4];
    cnlinks[3] = buffer[offset + 5];
    int nlinks = characters_To_Integer(cnlinks);
    delete buffer, address;
    return nlinks;
}

//******************************************************************************

void FileSystem::my_Set_Size(int inodeNumber, int size) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char* cSize = integer_To_Characters(size);
    buffer[offset + 15] = cSize[0];
    buffer[offset + 16] = cSize[1];
    buffer[offset + 17] = cSize[2];
    buffer[offset + 18] = cSize[3];
    writeBlock(blockNumber, buffer);
    my_Set_CTime(inodeNumber);
    delete cSize, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_Size(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cSize[4];
    cSize[0] = buffer[offset + 15];
    cSize[1] = buffer[offset + 16];
    cSize[2] = buffer[offset + 17];
    cSize[3] = buffer[offset + 18];
    int size = characters_To_Integer(cSize);
    delete buffer, address;
    return size;
}

//******************************************************************************

void FileSystem::my_Set_ATime(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    int ATime = time(nullptr);
    char* cATime = integer_To_Characters(ATime);
    buffer[offset + 79] = cATime[0];
    buffer[offset + 80] = cATime[1];
    buffer[offset + 81] = cATime[2];
    buffer[offset + 82] = cATime[3];
    writeBlock(blockNumber, buffer);
    delete cATime, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_ATime(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cATime[4];
    cATime[0] = buffer[offset + 79];
    cATime[1] = buffer[offset + 80];
    cATime[2] = buffer[offset + 81];
    cATime[3] = buffer[offset + 82];
    int ATime = characters_To_Integer(cATime);
    delete buffer, address;
    return ATime;
}

//******************************************************************************

void FileSystem::my_Set_MTime(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    int MTime = time(nullptr);
    char* cMTime = integer_To_Characters(MTime);
    buffer[offset + 83] = cMTime[0];
    buffer[offset + 84] = cMTime[1];
    buffer[offset + 85] = cMTime[2];
    buffer[offset + 86] = cMTime[3];
    writeBlock(blockNumber, buffer);
    delete cMTime, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_MTime(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cMTime[4];
    cMTime[0] = buffer[offset + 83];
    cMTime[1] = buffer[offset + 84];
    cMTime[2] = buffer[offset + 85];
    cMTime[3] = buffer[offset + 86];
    int MTime = characters_To_Integer(cMTime);
    delete buffer, address;
    return MTime;
}
//******************************************************************************

void FileSystem::my_Set_CTime(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    int CTime = time(nullptr);
    char* cCTime = integer_To_Characters(CTime);
    buffer[offset + 87] = cCTime[0];
    buffer[offset + 88] = cCTime[1];
    buffer[offset + 89] = cCTime[2];
    buffer[offset + 90] = cCTime[3];
    writeBlock(blockNumber, buffer);
    delete cCTime, buffer, address;
}

//******************************************************************************

int FileSystem::my_Read_CTime(int inodeNumber) {
    int* address = my_index_inodeTable(inodeNumber);
    int blockNumber = address[0];
    int offset = address[1];
    char* buffer = readBlock(blockNumber);
    char cCTime[4];
    cCTime[0] = buffer[offset + 87];
    cCTime[1] = buffer[offset + 88];
    cCTime[2] = buffer[offset + 89];
    cCTime[3] = buffer[offset + 90];
    int CTime = characters_To_Integer(cCTime);
    delete buffer, address;
    return CTime;
}

//******************************************************************************

int* FileSystem::my_index_inodeTable(int inodeNumber) {
    int* result = new int[2];
    int blockNumber = (inodeNumber / 512) + 1;
    int offset = (inodeNumber % 512) * 8;
    char* buffer = readBlock(blockNumber);
    char temp[4];
    temp[0] = buffer[offset];
    temp[1] = buffer[offset + 1];
    temp[2] = buffer[offset + 2];
    temp[3] = buffer[offset + 3];
    //cout << "temp 0: " << (int)buffer[offset] << endl;
    //cout << "temp 1: " << (int)buffer[offset + 1] << endl;
    //cout << "temp 2: " << (int)buffer[offset + 2] << endl;
    //cout << "temp 3: " << (int)buffer[offset + 3] << endl;
    //cout << "value: " << characters_To_Integer(temp) << endl;
    result[0] = characters_To_Integer(temp);
    //cout << "result 0: " << result[0] << endl;
    temp[0] = buffer[offset + 4];
    temp[1] = buffer[offset + 5];
    temp[2] = buffer[offset + 6];
    temp[3] = buffer[offset + 7];
    result[1] = characters_To_Integer(temp);
    //cout << result[0] << " " << result[1] << endl;
    delete buffer;
    return result;
}

//******************************************************************************

int FileSystem::single_Allocate() {
    //Returns the number of a free block. 0 means no blocks were free.
    char* buffer;
    int blockNumber = 0;
    int bitmap = 67;
    bool* currentBits;
    bool exit = false;
    while (!blockNumber && bitmap <= 82) {
        buffer = readBlock(bitmap);
        for (int i = 0; i < 4096; i++) {
            currentBits = character_To_Binary(buffer[i]);
            //cout << "char: " << (int)buffer[i] << endl;
            for (int j = 0; j < 8; j++) {
                //cout << j << endl;
                if (!currentBits[j]) {
                    currentBits[j] = true;
                    blockNumber = (bitmap - 67) * 32768 + i * 8 + j;
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
//Need to test
bool FileSystem::my_Add_Address_Indirect(char* block, int location, int blockNumber) {
    //Takes in a block and an offset within that block. It then treates that
    // position as an indirect block and tries to add the given address to it.
    int indirectBlock;
    int success = false;
    //Check if the indirect block exists
    if (!block[location] && !block[location + 1] && !block[location + 2] && !block[location + 3]) {
        //If it doesn't, then create it.
        indirectBlock = single_Allocate();
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
    } else { //It does exist
        char cIndirectBlock[4];
        cIndirectBlock[0] = block[location];
        cIndirectBlock[1] = block[location + 1];
        cIndirectBlock[2] = block[location + 2];
        cIndirectBlock[3] = block[location + 3];
        indirectBlock = characters_To_Integer(cIndirectBlock);
    }
    char* buffer = readBlock(indirectBlock);
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
    return success;
}

//******************************************************************************

bool FileSystem::my_Add_Address_DIndirect(char* block, int location, int blockNumber) {
    bool success = false;
    int DIndirect;
    if (!block[location] && !block[location + 1] && !block[location + 2] && !block[location + 3]) {
        DIndirect = single_Allocate();
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
        char cDIndirect[4];
        cDIndirect[0] = block[location];
        cDIndirect[1] = block[location + 1];
        cDIndirect[2] = block[location + 2];
        cDIndirect[3] = block[location + 3];
        DIndirect = characters_To_Integer(cDIndirect);
    }
    char* buffer = readBlock(DIndirect);
    for (int i = 0; i < 4096; i += 4) {
        success = my_Add_Address_Indirect(buffer, i, blockNumber);
        if (success) {
            writeBlock(DIndirect, buffer);
            break;
        }
    }
    delete buffer;
    return success;
}

//******************************************************************************
//Need to test
bool FileSystem::my_Add_Address(int inodeNumber, int blockNumber) {
    int* location = my_index_inodeTable(inodeNumber);
    char* buffer = readBlock(location[0]);
    int offset = location[1] + 19; //Offset of the addresses
    bool found = false;

    char cAddress[4];
    int address;
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
            break;
        }
    }

    //Search the first indirect block
    if (!found) {
        found = my_Add_Address_Indirect(buffer, location[1] + 67, blockNumber);
    }
    //Search the double indirect block
    if (!found) {
        found = my_Add_Address_DIndirect(buffer, location[1] + 71, blockNumber);
    }
    //Seaerch the triple indirect block
    if (!found) {
        int TIndirect;
        int TOffset = location[1] + 75;
        if (!buffer[TOffset] && !buffer[TOffset + 1] && !buffer[TOffset + 2] && !buffer[TOffset + 3]) {
            TIndirect = single_Allocate();
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

            delete buffer2, cTDirect;
        } else {
            char cTDirect[4];
            cTDirect[0] = buffer[TOffset];
            cTDirect[1] = buffer[TOffset + 1];
            cTDirect[2] = buffer[TOffset + 2];
            cTDirect[3] = buffer[TOffset + 3];
            TIndirect = characters_To_Integer(cTDirect);
        }
        char* buffer2 = readBlock(TIndirect);
        for (int i = 0; i < 4096; i += 4) {
            found = my_Add_Address_DIndirect(buffer2, i, blockNumber);
            if (found) {
                writeBlock(TIndirect, buffer);
                break;
            }
        }
        delete buffer2;
    }

    delete location, buffer;
    return found;
}

//******************************************************************************

//Need to test
int FileSystem::create_inode(char* mode, int user, int group) {
    //Returns -1 if the creation failed.
    char* buffer = readBlock(83);
    bool* currentBits;
    bool exit = false;
    int inodeNumber = -1;
    for (int i = 0; i < 4096; i++) {
        currentBits = character_To_Binary(buffer[i]);
        for (int j = 0; j < 8; j++) {
            if (!currentBits[j]) {
                currentBits[j] = true;
                buffer[i] = binary_To_Character(currentBits);
                writeBlock(83, buffer);
                inodeNumber = i * 8 + j;
                my_decrement_free_inodes();
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

        int blockNumber = single_Allocate();
        my_Add_Address(inodeNumber, blockNumber);
    }
    return inodeNumber;
}

//******************************************************************************

void FileSystem::Create_New_FS(string name) {
//needs testing

    //createDataFile(pow(2, 31), name);
    createDataFile(1024 * 1024 * 8, name);
    char* buffer;

    //Create superblock
    buffer = readBlock(65);
    char* numberOfinodes = integer_To_Characters(32768);
    char* numberOfBlocks = integer_To_Characters(524288);
    char* startOfFreeBlocks = integer_To_Characters(1008);
    for (int i = 0; i < 4; i++) {
        buffer[i] = numberOfinodes[i];
        buffer[4 + i] = numberOfBlocks[i];
        buffer[8 + i] = startOfFreeBlocks[i];
    }
    writeBlock(65, buffer);
    delete buffer;

    //Create group descriptor
    buffer = readBlock(66);
    char* blockBitmap = integer_To_Characters(67);
    char* inodeBitmap = integer_To_Characters(83);
    char* numberOfFreeBlocks = integer_To_Characters(523124);
    char* numberOfFreeinodes = integer_To_Characters(32768);
    char* numberOfDirectories = integer_To_Characters(0);
    for (int i = 0; i < 4; i++) {
        buffer[i] = blockBitmap[i];
        buffer[4 + i] = inodeBitmap[i];
        buffer[8 + i] = numberOfFreeBlocks[i];
        buffer[12 + i] = numberOfFreeinodes[i];
        buffer[16 + i] = numberOfDirectories[i];
    }
    writeBlock(66, buffer);
    delete buffer;

    //Mark 0-1107 as used on the block bitmap
    buffer = readBlock(67);
    bool bits[] = {true, true, true, true, false, false, false, false};
    for (int i = 0; i < 138; i++) {
        buffer[i] = 255;
    }
    buffer[138] = binary_To_Character(bits);
    writeBlock(67, buffer);
    delete buffer;

    //Create the i-node table;
    int currentInode = 0;
    char* inodesBlock;
    char* inodesOffset;
    for (int i = 1; i < 65; i++) {
        buffer = readBlock(i);
        for (int j = 0; j < 512; j++) {
            inodesBlock = integer_To_Characters((currentInode / 32) + 84);
            inodesOffset = integer_To_Characters((currentInode % 32) * 128);

            for (int k = 0; k < 4; k++) {
                buffer[j * 8 + k] = inodesBlock[k];
                buffer[j * 8 + 4 + k] = inodesOffset[k];
            }
            delete inodesBlock, inodesOffset;
            currentInode++;
        }
        writeBlock(i, buffer);
        delete buffer;
    }

    //Make the root directory
    //my_make_root();
    bool mode1[] = {false, true, true, true, true, true, true, true};
    bool mode2[] = {true, true, true, false, false, false, false, false};
    char mode[] = {binary_To_Character(mode1), binary_To_Character(mode2)};
    create_inode(mode, 0, 0);
    my_increment_directories();

    delete numberOfinodes, numberOfBlocks, startOfFreeBlocks, blockBitmap,
     inodeBitmap, numberOfFreeBlocks, numberOfFreeinodes, numberOfDirectories;
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

void FileSystem::print_inode_bitmap() {
    char* buffer = readBlock(83);
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
        buffer = readBlock(67 + i);
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

int main() {
    FileSystem FS("disk.dat");

    //FS.clearFS();
    //FS.createDataFile(1024 * 1024, "disk.dat");

    //char* result = FS.my_Read_Mode(2);

    //cout << result[0] << " " << result[1];
    
    FS.Create_New_FS("disk.dat");
    
    char* buffer = new char[4096];
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

    int* address = FS.my_index_inodeTable(0);
    char* buffer2 = FS.readBlock(address[0]);
    int blockNumber = FS.characters_To_Integer(&buffer2[address[1] + 19]);
    int blockNumber2 = FS.characters_To_Integer(&buffer2[address[1] + 23]);
    int blockNumber3 = FS.characters_To_Integer(&buffer2[address[1] + 27]);
    int blockNumber4 = FS.characters_To_Integer(&buffer2[address[1] + 31]);
    cout << "Address1: " << blockNumber << endl;
    cout << "Address2: " << blockNumber2 << endl;
    cout << "Address3: " << blockNumber3 << endl;
    cout << "Address4: " << blockNumber4 << endl;

    FS.writeBlock(66, buffer);
    FS.writeBlock(83, buffer);


    FS.print_block_bitmap();

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