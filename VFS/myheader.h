//This is the header file in which we have included all the necessary header files and the prototypes for the project.

#define _CRT_SECURE_NO_WARNINGS		//used to hide the warnings
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>
#include<malloc.h>

#define MAXINODE 50					//we are including maximum 50 inodes here

#define READ 1						//used for read instruction
#define WRITE 2						//used for write instruction

#define MAXFILESIZE 1024			//maximum file size which is in the Data Block (DB) is 1024 bytes

#define REGULAR 1					//for regular file
#define SPECIAL 2					//for special file

#define START 0						
#define CURRENT 1
#define END 2

typedef struct superblock			//structure for superblock
{
	int TotalInodes;				//count of total inodes
	int FreeInode;					//count of free inodes
}SUPERBLOCK, * PSUPERBLOCK;

typedef struct inode				//structure for every inode in the DILB (Disk Inode List Block)
{
	char FileName[MAXINODE];		//file name
	int InodeNumber;				//inode number
	int FileSize;					//file size
	int FileActualSize;				//actual file size
	int FileType;					//type of the file
	char* Buffer;					//buffer where the data is stored 
	int LinkCount;
	int ReferenceCount;				//referencwe count in the IIT
	int permission;					//permission for accessing of the file
	struct inode* next;
}INODE, * PINODE, ** PPINODE;

typedef struct filetable			//structure for File Table block
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE, * PFILETABLE;

typedef struct ufdt					//structure for UFDT (User File Descriptor Table)
{
	PFILETABLE ptrfiletable;
}UFDT;

//These are the global variables
UFDT UFDTArr[MAXINODE];
SUPERBLOCK SUPERBLOCKobj;			//object of the superblock
PINODE head = NULL;

typedef int BOOL;
#define TRUE 1
#define FALSE 0

//Prototypes
void man(char*);
void DisplayHelp();
int GetFDFromName(char*);
PINODE Get_Inode(char*);
void CreateDILB();
void InitialiseSuperBlock();
int CreateFile(char*, int);
int rm_File(char*);
int ReadFile(int, char*, int);
int WriteFile(int, char*, int);
int OpenFile(char*, int);
void CloseFileByName1(int);
int CloseFileByName2(char*);
void CloseAllFiles();
int LseekFile(int, int, int);
void ls_file();
int fstatFile(int);
int statFile(char*);
int TruncateFile(char*);
int ChangeMode(char*, int);
//int CreateHardLink(char*, char*);