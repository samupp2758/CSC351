



//******************************************************************************

#ifndef __P1_H
#define __P1_H

//******************************************************************************

#include <fstream> //Manipulate files

using namespace std;

//******************************************************************************

class FileSystem {
    private:
        const int BLOCKSIZE = 4096;
        const int MAXBLOCKS = 524288;

    public:
        fstream disk;

        FileSystem(string name);
        ~FileSystem();

        void clearFS();
        void createDataFile(unsigned int size, string name);
        char* readBlock(int blockNumber);
        void writeBlock(int blockNumber, char* data);
        int readFileIn(string FileName, int startingBlock);
        void readFileOut(string FileName, int startingBlock, int numberOfBlocks);
        void Create_New_FS(string name);
        int create_inode(char* mode, int user, int group);
        int single_Allocate();
        bool my_Add_Address(int inodeNumber, int blockNumber);
        bool my_Add_Address_Indirect(char* block, int location, int blockNumber);
        bool my_Add_Address_DIndirect(char* block, int location, int blockNumber);

        //inode helper functions
        void my_Set_Mode(int inodeNumber, char* mode);
        char* my_Read_Mode(int inodeNumber);
        void my_Set_UID(int inodeNumber, int UID);
        int my_Read_UID(int inodeNumber);
        void my_Set_GID(int inodeNumber, int GID);
        int my_Read_GID(int inodeNumber);
        void my_Increment_nlinks(int inodeNumber);
        void my_Decrement_nlinks(int inodeNumber);
        int my_Read_nlinks(int inodeNumber);
        void my_Set_Size(int inodeNumber, int size);
        int my_Read_Size(int inodeNumber);
        void my_Set_ATime(int inodeNumber);
        int my_Read_ATime(int inodeNumber);
        void my_Set_MTime(int inodeNumber);
        int my_Read_MTime(int inodeNumber);
        void my_Set_CTime(int inodeNumber);
        int my_Read_CTime(int inodeNumber);
        int* my_index_inodeTable(int inodeNumber);

        //Utility & char conversion
        bool* character_To_Binary(char c);
        char binary_To_Character(bool* binary);
        char* integer_To_Characters(int num);
        int characters_To_Integer(char* chars);

        //Group Descriptor
        int my_adjust_free_blocks(int amount);
        int my_decrement_free_inodes();
        int my_Increment_free_inodes();
        int my_read_free_inodes();
        int my_decrement_directories();
        int my_increment_directories();
        int my_read_directories();

        //Debugging
        void print_inode_bitmap();
        void print_block_bitmap();


};

//******************************************************************************

#endif
