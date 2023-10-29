



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
        int allocate();
        bool my_Add_Address(int inodeNumber, int blockNumber);
        bool my_Add_Address_Indirect(char* block, int location, int blockNumber);
        bool my_Add_Address_DIndirect(char* block, int location, int blockNumber);
        int* get_addresses(int inodeNumber, int indirect_block);
        void my_write_dir(int directoryInodeNum, int entryInodeNum, string name);
        bool add_entry_to_block(int blockNumber, int entryInodeNum, string name);
        void my_extend(int inodeNumber);
        int my_remove_entry(int directoryInodeNum, int position);
        int my_read_dir(int directoryInodeNum, int position, int& inodeNumber, string& name, char& type);
        int my_readPath(string path);
        int my_readPath(string path, int& parentInode);
        int my_readPath(string path, int& parentInode, string& name);
        int my_search_dir(int dirInode, string name, int& iNodeNum);
        int my_search_dir(int dirInode, string name);
        int my_mkdir(string path, int user, int group);
        void my_rmdir(string path);
        void my_Delete(int inodeNumber);
        void mark_inode_free(int inodeNumber);
        void mark_blocks_free(int* blockNumbers, int size);

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
        int* my_index_inodes(int inodeNumber);

        //Utility & char conversion
        bool* character_To_Binary(char c);
        char binary_To_Character(bool* binary);
        char* integer_To_Characters(int num);
        int characters_To_Integer(char* chars);

        //Debugging
        void print_inode_bitmap();
        void print_block_bitmap();
        void print_dir(string path);


};

//******************************************************************************

#endif
