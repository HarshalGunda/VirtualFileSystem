#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <malloc.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define	SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
	int TotalInodes;
	int FreeInodes;
}SUPERBLOCK, * PSUPERBLOCK;

typedef struct inode
{
	char FileName[MAXINODE];
	int InodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char* Buffer;
	int LinkCount;
	int ReferenceCount;
	int Permission;
	struct inode* next;
}INODE, * PINODE, ** PPINODE;

typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE, * PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[MAXINODE];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

typedef int BOOL;

#define TRUE 1
#define FALSE 0

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
void CloseAllFile();
int LseekFile(int, int, int);
void ls_file();
int fstat_file(int);
int stat_file(char*);
int truncate_file(char*);