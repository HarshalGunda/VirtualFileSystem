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
void CloseAllFile();
int LseekFile(int, int, int);
void ls_file();
int fstat_file(int);
int stat_file(char*);
int truncate_File(char*);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: void InitialiseSuperBlock()
Input parameters								: none
Return value of function						: void - which will return nothing
Description of function and its use in project	: Here, we are Initialising the Superblock which contains theinformation of all the inodes which are stored in the DILB block.
*/
void InitialiseSuperBlock()
{
	int i = 0;

	while (i < MAXINODE)
	{
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	}

	SUPERBLOCKobj.TotalInodes = MAXINODE;
	SUPERBLOCKobj.FreeInode = MAXINODE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: void CreateDILB()
Input parameters								: none
Return value of function						: void - which return nothing
Description of function and its use in project	: Here, we are creating the DILB(Disc Inode List Block) which contains the linked list of inodes.
												  Each inode has the unique number (or unique ID) which stores the information about the file which are stored in the data block.
*/
void CreateDILB()
{
	int j = 1;
	PINODE newn = NULL;
	PINODE temp = head;

	while (j <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));		//allocating the dynamic memory to the new inode

		//initializing all the attributes of the new inode
		newn->LinkCount = 0;
		newn->ReferenceCount = 0;
		newn->FileType = 0;
		newn->FileSize = 0;
		newn->Buffer = NULL;
		newn->next = NULL;
		newn->InodeNumber = j;

		if (temp == NULL)			//linked list contains zero inodes
		{
			head = newn;
			temp = head;
		}
		else						//linked list contains at least one inode
		{
			temp->next = newn;
			temp = temp->next;
		}
		j++;						//increment the unique inode number		
	}
	//printf("\nDILB created successfully\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int CreateFile(char *name, int permission)
Input parameters								: name of the file, permission to access the file
Return value of function						: We are returning the inode number of the file which gets created in this function.
Description of function and its use in project	: In this function, we are creating a file with the file permissions to access the file.
*/
int CreateFile(char* name, int permission)
{
	int k = 0;
	PINODE temp = head;

	if ((name == NULL) || (permission <= 0) || (permission > 3))		//exceptions
	{
		return -1;
	}

	if (SUPERBLOCKobj.FreeInode == 0)	//no free inodes for creating new file
	{
		return -2;
	}

	if (Get_Inode(name) != NULL)		//file already exists or not
	{
		return -3;
	}

	(SUPERBLOCKobj.FreeInode)--;	//free inode found

	while (temp != NULL)
	{
		if (temp->FileType == 0)
		{
			break;
		}

		temp = temp->next;
	}

	while (k < MAXINODE)
	{
		if (UFDTArr[k].ptrfiletable == NULL)
		{
			break;
		}
		k++;
	}

	UFDTArr[k].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if (UFDTArr[k].ptrfiletable == NULL)
	{
		return -4;
	}

	UFDTArr[k].ptrfiletable->count = 1;
	UFDTArr[k].ptrfiletable->mode = permission;
	UFDTArr[k].ptrfiletable->readoffset = 0;
	UFDTArr[k].ptrfiletable->writeoffset = 0;
	UFDTArr[k].ptrfiletable->ptrinode = temp;
	strcpy(UFDTArr[k].ptrfiletable->ptrinode->FileName, name);
	UFDTArr[k].ptrfiletable->ptrinode->FileType = REGULAR;
	UFDTArr[k].ptrfiletable->ptrinode->ReferenceCount = 1;
	UFDTArr[k].ptrfiletable->ptrinode->LinkCount = 1;
	UFDTArr[k].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
	UFDTArr[k].ptrfiletable->ptrinode->FileActualSize = 0;
	UFDTArr[k].ptrfiletable->ptrinode->permission = permission;
	UFDTArr[k].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);
	memset(UFDTArr[k].ptrfiletable->ptrinode->Buffer, 0, 1024);

	return k;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int rm_File(char *name)
Input parameters								: name of the file entered by the user.
Return value of function						: int - which will return 0 if file deleted successfully
Description of function and its use in project	: This function is used to delete the file from the file system.
*/
int rm_File(char* name)
{
	int fd1 = 0;

	fd1 = GetFDFromName(name);		//search for the file name which we want to remove/delete

	if (fd1 == -1)
	{
		return -1;
	}

	(UFDTArr[fd1].ptrfiletable->ptrinode->LinkCount)--;

	if ((UFDTArr[fd1].ptrfiletable->ptrinode->LinkCount) == 0)
	{
		UFDTArr[fd1].ptrfiletable->ptrinode->FileType = 0;
		UFDTArr[fd1].ptrfiletable->ptrinode->FileName[0] = '\0';
		free(UFDTArr[fd1].ptrfiletable);
	}

	UFDTArr[fd1].ptrfiletable = NULL;
	(SUPERBLOCKobj.FreeInode)++;			//file removed successfully so inode gets added
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: void DisplayHelp()
Input parameters								: none
Return value of function						: void
Description of function and its use in project	: This funcion is used to help the user, how to enter the command.
*/
void DisplayHelp()
{
	printf("\nls: To list out all files\n");
	printf("create: To create the new file\n");
	printf("clear: To clear the console\n");
	printf("open: To open the file\n");
	printf("close: To close the file\n");
	printf("closeall: To close all opened files\n");
	printf("read: To read the contents from the file\n");
	printf("write: To write the contents into the file\n");
	printf("exit: To terminate the file system\n");
	printf("stat: To display the information of the file using name\n");
	printf("fstat: To display the information of the file using file descriptor\n");
	printf("truncate: To remove all data from the file\n");
	printf("rm: To delete the file\n");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int GetFDFromName(char *name)
Input parameters								: name of the file entered by the user.
Return value of function						: int - which is used to return the file descriptor value
Description of function and its use in project	: In this function we are calculating the file descriptor, we find the same file name.
												  If file name not found means no file is there in the file system.
*/
int GetFDFromName(char* name)
{
	int x = 0;

	while (x < MAXINODE)
	{
		if (UFDTArr[x].ptrfiletable != NULL)
		{
			if (_stricmp((UFDTArr[x].ptrfiletable->ptrinode->FileName), name) == 0)		//filename already exists
			{
				break;
			}
		}
		x++;
	}

	if (x == MAXINODE)	//no empty inodes
	{
		return -1;
	}
	else
	{
		return x;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: PINODE Get_Inode(char *name)
Input parameters								: name of the file entered by the user.
Return value of function						: PINODE which is used to find the inode.
Description of function and its use in project	: Here, we are finding the free inode which will be used to store the file.
*/
//link the inode to the file in the data block
PINODE Get_Inode(char* name)
{
	PINODE temp1 = head;
	int i = 0;

	if (name == NULL)
	{
		return NULL;
	}

	while (temp1 != NULL)
	{
		if (strcmp(name, temp1->FileName) == 0)		//file name is same, or existing the file which has same name
		{
			break;
		}

		temp1 = temp1->next;
	}

	return temp1;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: void ls_file()
Input parameters								: none
Return value of function						: void - which returns nothing
Description of function and its use in project	: This function used to list out the files in the file system.
*/
void ls_file()
{
	int y = 0;
	PINODE temp2 = head;

	if (SUPERBLOCKobj.FreeInode == MAXINODE)
	{
		printf("---------------------------------------------------------");
		printf("\nERROR: There are no files \n");
	}
	else
	{
		printf("-------------------------------------------------------------------------\n");
		printf("File Name\tInode Number\tFile Actual Size\tLink Count\n");
		printf("-------------------------------------------------------------------------\n");

		while (temp2 != NULL)
		{
			if (temp2->FileType != 0)
			{
				printf("%s\t\t%d\t\t%d\t\t%d\n", temp2->FileName, temp2->InodeNumber, temp2->FileActualSize, temp2->LinkCount);
			}

			temp2 = temp2->next;
		}
		printf("-------------------------------------------------------------------------\n");
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int LseekFile(int fd2, int size, int from)
Input parameters								: file descriptor, size of the bytes, position of seek
Return value of function						: int
Description of function and its use in project	: This function contains 3 positions - START, CURRENT and END.
												  START - The seek will be set to the 0th byte of the file
												  CURRENT - The seek will be set to the current byte of the file.
												  END - The seek will be set at the end of the file.
*/
int LseekFile(int fd2, int size, int from)
{
	if ((fd2 < 0) || (from < 0) || (from > 2))
	{
		return -1;
	}

	if (UFDTArr[fd2].ptrfiletable == NULL)
	{
		return -1;
	}

	if ((UFDTArr[fd2].ptrfiletable->mode == READ) || (UFDTArr[fd2].ptrfiletable->mode == READ + WRITE))
	{
		if (from == CURRENT)		//file read is from current state
		{
			if (((UFDTArr[fd2].ptrfiletable->readoffset) + size) > (UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize))	//byte reading is greater than actual file size
			{
				return -1;
			}

			if (((UFDTArr[fd2].ptrfiletable->readoffset) + size) < 0)		//read offset is less than 0
			{
				return -1;
			}

			(UFDTArr[fd2].ptrfiletable->readoffset) = (UFDTArr[fd2].ptrfiletable->readoffset) + size;		//read the further number if bytes from current position of read offset
		}
		//***************************************************************************************************************************************
		else if (from == START)		//file read is from first byte i.e. at the start of the file
		{
			if (size > (UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize))		//size of bytes to read the file is greater than the actual file size of the file
			{
				return -1;
			}

			if (size < 0)			//read size is less than 0
			{
				return -1;
			}

			(UFDTArr[fd2].ptrfiletable->readoffset) = size;		//read the number of bytes entered by the user
		}
		//***************************************************************************************************************************************
		else if (from == END)		//read from the end of the file
		{
			if ((UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)		//if read offset is greater than maximum file size
			{
				return -1;
			}

			if (((UFDTArr[fd2].ptrfiletable->readoffset) + size) < 0)		//if the read offset + size is less than 0
			{
				return -1;
			}

			(UFDTArr[fd2].ptrfiletable->readoffset) = (UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize) + size;		//
		}
	}
	//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	else if (UFDTArr[fd2].ptrfiletable->mode == WRITE)
	{
		if (from == CURRENT)
		{
			if (((UFDTArr[fd2].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
			{
				return -1;
			}
			if (((UFDTArr[fd2].ptrfiletable->writeoffset) + size) < 0)
			{
				return -1;
			}

			if (((UFDTArr[fd2].ptrfiletable->writeoffset) + size) > (UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize))
			{
				(UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd2].ptrfiletable->writeoffset) + size;
			}
			(UFDTArr[fd2].ptrfiletable->writeoffset) = (UFDTArr[fd2].ptrfiletable->writeoffset) + size;
		}
		//***************************************************************************************************************************************
		else if (from == START)
		{
			if (size > MAXFILESIZE)
			{
				return -1;
			}

			if (size < 0)
			{
				return -1;
			}

			if (size > (UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize))
			{
				(UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize) = size;
			}
			(UFDTArr[fd2].ptrfiletable->writeoffset) = size;
		}
		//***************************************************************************************************************************************
		else if (from == END)
		{
			if ((UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
			{
				return -1;
			}

			if (((UFDTArr[fd2].ptrfiletable->writeoffset) + size) < 0)
			{
				return -1;
			}

			(UFDTArr[fd2].ptrfiletable->writeoffset) = (UFDTArr[fd2].ptrfiletable->ptrinode->FileActualSize) + size;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int OpenFile(char *name, int mode)
Input parameters								: name of the file and mode or permission of the file
Return value of function						: int
Description of function and its use in project	: This function is used to open the file with the file permission.
*/
int OpenFile(char* name, int mode)
{
	int z = 0;
	PINODE temp3 = NULL;

	if ((name == NULL) || (mode <= 0))
	{
		return -1;
	}

	temp3 = Get_Inode(name);

	if (temp3 == NULL)
	{
		return -2;
	}
	if (temp3->permission < mode)
	{
		return -3;
	}
	while (z < MAXINODE)
	{
		if (UFDTArr[z].ptrfiletable == NULL)
		{
			break;
		}
		z++;
	}

	UFDTArr[z].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if (UFDTArr[z].ptrfiletable == NULL)
	{
		return -1;
	}
	UFDTArr[z].ptrfiletable->count = 1;
	UFDTArr[z].ptrfiletable->mode = mode;
	if (mode == READ + WRITE)
	{
		UFDTArr[z].ptrfiletable->readoffset = 0;
		UFDTArr[z].ptrfiletable->writeoffset = 0;
	}
	else if (mode == READ)
	{
		UFDTArr[z].ptrfiletable->readoffset = 0;
	}
	else if (mode == WRITE)
	{
		UFDTArr[z].ptrfiletable->writeoffset = 0;
	}
	UFDTArr[z].ptrfiletable->ptrinode = temp3;
	(UFDTArr[z].ptrfiletable->ptrinode->ReferenceCount)++;

	return z;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int CloseFileByName2(char *name)
Input parameters								: name of the file
Return value of function						: void - which returns nothing
Description of function and its use in project	: This function closes the single file, which is entered by the user
*/
void CloseFileByName1(int fd4)
{
	UFDTArr[fd4].ptrfiletable->readoffset = 0;
	UFDTArr[fd4].ptrfiletable->writeoffset = 0;

	if ((UFDTArr[fd4].ptrfiletable->ptrinode->ReferenceCount) > 0)
	{
		(UFDTArr[fd4].ptrfiletable->ptrinode->ReferenceCount)--;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int CloseFileByName2(char *name)
Input parameters								: name of the file
Return value of function						: int
Description of function and its use in project	: This function closes the single file, which is entered by the user
*/
int CloseFileByName2(char* name)
{
	int q = 0;
	q = GetFDFromName(name);
	if (q == -1)
	{
		return -1;
	}
	UFDTArr[q].ptrfiletable->readoffset = 0;
	UFDTArr[q].ptrfiletable->writeoffset = 0;

	if ((UFDTArr[q].ptrfiletable->ptrinode->ReferenceCount) > 0)
	{
		(UFDTArr[q].ptrfiletable->ptrinode->ReferenceCount)--;
	}
	return q;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: void CloseAllFile()
Input parameters								: none
Return value of function						: void - which returns nothing
Description of function and its use in project	: This function closes all the opened files.
*/
void CloseAllFile()
{
	int p = 0;
	while (p < MAXINODE)
	{
		if (UFDTArr[p].ptrfiletable != NULL)		//check the opened files
		{
			UFDTArr[p].ptrfiletable->readoffset = 0;
			UFDTArr[p].ptrfiletable->writeoffset = 0;

			if ((UFDTArr[p].ptrfiletable->ptrinode->ReferenceCount) > 0)
			{
				(UFDTArr[p].ptrfiletable->ptrinode->ReferenceCount)--;
			}

			break;
		}
		p++;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int ReadFile(int fd5, char *arr, int isize)
Input parameters								: file descriptor, data to read, bytes or size of the data to be read
Return value of function						: Shows the data which user wants to read.
Description of function and its use in project	: This function shows the user, the data, how many bytes he wants to read in the file.
*/
int ReadFile(int fd5, char* arr, int isize)
{
	int read_size = 0;

	if ((UFDTArr[fd5].ptrfiletable) == NULL)		//array is null
	{
		return -1;
	}

	if ((UFDTArr[fd5].ptrfiletable->mode != READ) && (UFDTArr[fd5].ptrfiletable->mode != READ + WRITE))		//file has no permission to read and write
	{
		return -2;
	}

	if ((UFDTArr[fd5].ptrfiletable->ptrinode->permission != READ) && ((UFDTArr[fd5].ptrfiletable->ptrinode->permission != (READ + WRITE))))		//file has no permission to read and write
	{
		return -2;
	}

	if ((UFDTArr[fd5].ptrfiletable->readoffset) == (UFDTArr[fd5].ptrfiletable->ptrinode->FileActualSize))
	{
		return -3;
	}

	if (UFDTArr[fd5].ptrfiletable->ptrinode->FileType != REGULAR)				//file is not regular
	{
		return -4;
	}

	read_size = (UFDTArr[fd5].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd5].ptrfiletable->readoffset);

	if (read_size < isize)
	{
		strncpy(arr, (UFDTArr[fd5].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd5].ptrfiletable->readoffset), read_size);
		UFDTArr[fd5].ptrfiletable->readoffset = (UFDTArr[fd5].ptrfiletable->readoffset) + read_size;
	}
	else
	{
		strncpy(arr, (UFDTArr[fd5].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd5].ptrfiletable->readoffset), isize);
		UFDTArr[fd5].ptrfiletable->readoffset = (UFDTArr[fd5].ptrfiletable->readoffset) + isize;
	}

	return isize;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int WriteFile(int fd6, char *arr, int isize)
Input parameters								: file descriptor, the data which is user wants o write in the file, size of the data in bytes.
Return value of function						: It return the Buffer which contains the data which is entered by the user.
Description of function and its use in project	: In this function, what user wants to write the data in the file, is written in the Buffer of that file.
												  This function writes the data which is entered by the user, in the file buffer.
*/
int WriteFile(int fd6, char* arr, int isize)
{
	if (((UFDTArr[fd6].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd6].ptrfiletable->mode) != READ + WRITE))
	{
		return -1;
	}

	if (((UFDTArr[fd6].ptrfiletable->ptrinode->permission) != WRITE) && ((UFDTArr[fd6].ptrfiletable->ptrinode->permission) != READ + WRITE))
	{
		return -2;
	}

	if ((UFDTArr[fd6].ptrfiletable->writeoffset) == MAXFILESIZE)
	{
		return -3;
	}

	if ((UFDTArr[fd6].ptrfiletable->ptrinode->FileType) != REGULAR)
	{
		return -4;
	}

	strncpy((UFDTArr[fd6].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd6].ptrfiletable->writeoffset), arr, isize);

	(UFDTArr[fd6].ptrfiletable->writeoffset) = (UFDTArr[fd6].ptrfiletable->writeoffset) + isize;

	(UFDTArr[fd6].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd6].ptrfiletable->ptrinode->FileActualSize) + isize;

	return isize;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int fstat_file(char *name)
Input parameters								: name of the file
Return value of function						: returns the statistical information of the file
Description of function and its use in project	: In this function, we accept the file name from user to display the statistical information of the file.
*/
int fstat_file(int fd7)
{
	PINODE temp4 = head;
	int r = 0;

	if (fd7 < 0)
	{
		return -1;
	}

	if (UFDTArr[fd7].ptrfiletable == NULL)
	{
		return -2;
	}

	temp4 = UFDTArr[fd7].ptrfiletable->ptrinode;

	printf("\n------------Statistical Information------------\n");
	printf("\nFile Name: \t\t%s", temp4->FileName);
	printf("\nInode Number: \t\t%d", temp4->InodeNumber);
	printf("\nFile Size: \t\t%d", temp4->FileSize);
	printf("\nActual File Size: \t%d", temp4->FileActualSize);
	printf("\nLink Count: \t\t%d", temp4->LinkCount);
	printf("\nReference Count: \t%d", temp4->ReferenceCount);

	if (temp4->permission == 1)
	{
		printf("\nFile Permission:\tRead Only\n");
	}
	else if (temp4->permission == 2)
	{
		printf("\nFile Permission:\tWrite\n");
	}
	else if (temp4->permission == 3)
	{
		printf("\nFile Permission:\tRead and Write\n");
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int stat_file(char *name)
Input parameters								: name of the file
Return value of function						: returns the statistical information of the file
Description of function and its use in project	: In this function, we accept the file name from user to display the statistical information of the file.
*/
int stat_file(char* name)
{
	PINODE temp5 = head;
	int t = 0;

	if (name == NULL)
	{
		return -1;
	}

	while (temp5 != NULL)							//search if the file name exists
	{
		if (strcmp(name, temp5->FileName) == 0)		//file found
		{
			break;
		}

		temp5 = temp5->next;
	}

	if (temp5 == NULL)
	{
		return -2;
	}

	printf("\n------------Statistical Information------------\n");
	printf("\nFile Name: \t\t%s", temp5->FileName);
	printf("\nInode Number: \t\t%d", temp5->InodeNumber);
	printf("\nFile Size: \t\t%d", temp5->FileSize);
	printf("\nActual File Size: \t%d", temp5->FileActualSize);
	printf("\nLink Count: \t\t%d", temp5->LinkCount);
	printf("\nReference Count: \t%d", temp5->ReferenceCount);

	if (temp5->permission == 1)
	{
		printf("\nFile Permission:\tRead Only\n");
	}
	else if (temp5->permission == 2)
	{
		printf("\nFile Permission:\tWrite\n");
	}
	else if (temp5->permission == 3)
	{
		printf("\nFile Permission:\tRead and Write\n");
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int truncate_File(char *name)
Input parameters								: name of the file
Return value of function						: int
Description of function and its use in project	: This function is used to delete all the data writen in the file.
*/
int truncate_File(char* name)
{
	int fd8 = GetFDFromName(name);

	if (fd8 == 1)
	{
		return -1;
	}

	memset(UFDTArr[fd8].ptrfiletable->ptrinode->Buffer, 0, MAXFILESIZE);
	UFDTArr[fd8].ptrfiletable->readoffset = 0;
	UFDTArr[fd8].ptrfiletable->writeoffset = 0;
	UFDTArr[fd8].ptrfiletable->ptrinode->FileActualSize = 0;

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: void man(char *name)
Input parameters								: name of the file
Return value of function						: void - which returns nothing
Description of function and its use in project	: In this function, we have provided a manual help for "How to use the command" and its information, so that user can easily use it.
*/
void man(char* name)
{
	if (name == NULL)						//file name is not given
	{
		return;
	}

	if (_stricmp(name, "create") == 0)		//create the new regular file
	{
		printf("Description: Used to create the new regular file\n");
		printf("Usage: create file_name permission\n");
		printf("Permissions:\n");
		printf("Read = 1\t\nWrite = 2\nRead+Write = 3\n");
		printf("e.g. create demo.txt 1\n");
	}
	else if (_stricmp(name, "read") == 0)		//reading the data from the file
	{
		printf("\nDescription: Used to read the data from regular file\n");
		printf("Usage: read file_name number_of_bytes_to_read\n");
		printf("Caution: Number of bytes count should be less than 1024\n");
		printf("e.g. read demo.txt 425\n");
	}
	else if (_stricmp(name, "write") == 0)		//write the information into the file
	{
		printf("\nDescription: Used to write in the regular file\n");
		printf("Usage: write file_name\n After this, enter the data that you want to write\n");
		printf("e.g. write demo.txt\n Hello, Have a good day!\n");
	}
	else if (_stricmp(name, "ls") == 0)			//list out all the files in the current directory
	{
		printf("\nDescription: Used to list all the files in the directory\n");
		printf("Usage: ls\n");
	}
	else if (_stricmp(name, "stat") == 0)		//the properties of the file or the information about the file
	{
		printf("\nDescription: Used to display the information of the file\n");
		printf("Usage: stat file_name\n");
		printf("e.g. stat Demo.txt\n");
	}
	else if (_stricmp(name, "fstat") == 0)
	{
		printf("\nDescription: Used to display the information of the file\n");
		printf("Usage: fstat file_descriptor\n");
	}
	else if (_stricmp(name, "truncate") == 0)		//remove or delete the data from the file
	{
		printf("\nDescription: Used to remove the data from the file\n");
		printf("Usage: truncate file_descriptor\n");
	}
	else if (_stricmp(name, "open") == 0)			//open the file
	{
		printf("\nDescription: Used to open the existing file\n");
		printf("Usage: open file_name mode\n");
		printf("Mode:\nREAD = 1\nWRITE = 2\nREAD+WRITE = 3\n");
		printf("e.g.1) open Demo.txt 1\n");
		printf("e.g.2) open Demo.txt 3\n");
	}
	else if (_stricmp(name, "close") == 0)			//close the file
	{
		printf("\nDescription: Used to close the opened file\n");
		printf("Usage: close file_name\n");
		printf("e.g. close Demo.txt\n");
	}
	else if (_stricmp(name, "closeall") == 0)		//close all the opened files
	{
		printf("\nDescription: Used to close all the opened files\n");
		printf("Usage: closeall\n");
	}
	else if (_stricmp(name, "lseek") == 0)			//read or write in the file by using lseek
	{
		printf("\nDescription: Used to change the file offset\n");
		printf("Usage: lseek file_name ChangeInOffset StartPoint\n");
		printf("For StartPoint: START = 0\n");
		printf("				CURRENT = 1\n");
		printf("				END = 2\n");
		printf("e.g. lseek demo.txt 10 1");
	}
	else if (_stricmp(name, "rm") == 0)				//delete the file
	{
		printf("\nDescription: Used to delete the existing file\n");
		printf("Usage: rm file_name\n");
		printf("e.g. rm Demo.txt\n");
	}
	else
	{
		printf("\nERROR: No manual entry available.\n");		//invalid command
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: BOOL Authentication(char *username, char *password)
Input parameters								: username entered by the user & password entered by the user
Return value of function						: BOOL which will be either TRU or FALSE
Description of function and its use in project  : In this function, we are accepting username and password from user and compare it
												  with the username and password which is stored inside the database or function.
												  It will return TRUE if both username and password entered by the user matches the original username and password,
												  else it will return FALSE.
*/
BOOL Authentication(char* username, char* password)
{
	char name[] = "user";
	char pwd[] = "123";

	if ((strcmp(name, username) == 0) && (strcmp(pwd, password) == 0))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int GetBackup()
{
	int file;
	int i=0,j=0,iRet=0;
	const char *ch="123";
	char c;
	PINODE temp=head;

	if(sObj.Free_Blocks==MAX_INODES)
	{
		return ERR_FileIsNotOpened;
	}
	file=open(BACKUPFILE,O_RDWR|O_APPEND|O_CREAT);
	if(file<=(-1))
	{
		return ERR_UnableLocateFile;
	}
	while((temp!=NULL))
	{
		if((temp->FileType==REGULAR)&&(temp->Buffer!=NULL)&&(temp->bckup!=TRUE))
		{
			if(write(file,temp->FileName,MAX_FILE_NAME_SIZE)<0)
			{
				iRet = ERR_FailedToWriteBackupData;
				break;
			}
			c=ch[(temp->Permission-1)];
			if(write(file,&c,1)<0)
			{
				iRet = ERR_FailedToWriteBackupData;
				break;
			}
			if(write(file,temp->Buffer,MAX_FILE_SiZE)<0)
			{
				iRet = ERR_FailedToWriteBackupData;
				break;
			}
			temp->bckup=TRUE;
			j++;
			iRet=j;
		}
		temp=temp->Next;
	}
	close(file);
	return iRet;
}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Name of function								: int main()
Input parameters								: none
Return value of function						: It will return 0 to the OS which tells the OS that program is successful.
Description of function and its use in project  : This function is the entry point function which will communicate with the end user according to his requirement.
*/
int main()
{
	char* ptr = NULL;
	int ret = 0, fd = 0, count = 0;
	BOOL bret;

	/*char username[20];
	char password[20];*/

	char command[4][80], str[80], arr[MAXFILESIZE];
	InitialiseSuperBlock();				//initialising the superblock
	CreateDILB();						//creating DILB

	/*while (1)
	{
		printf("Enter the username:\t");
		scanf("%s", &username);

		printf("\nEnter the password:\t");
		scanf("%s", &password);

		bret = Authentication(username, password);

		if (bret == TRUE)
		{
			break;
		}
		else
		{
			printf("Wrong username or password\n\n");
		}
	}*/

	printf("\n******************* Welcome to the Virtual File System *******************\n");
	while (1)
	{
		fflush(stdin);				//flush the input buffer
		strcpy(str, "");			//str reset
		printf("\nuser@VFS:~$ ");
		fgets(str, 80, stdin);		//take the input from the user like scanf() function

		count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (count == 1)
		{
			if (_stricmp(command[0], "ls") == 0)
			{
				ls_file();
			}

			else if (_stricmp(command[0], "closeall") == 0)
			{
				CloseAllFile();
				printf("All files closed successfully\n");
			}

			else if (_stricmp(command[0], "clear") == 0)
			{
				system("cls");
			}

			else if (_stricmp(command[0], "help") == 0)
			{
				DisplayHelp();
			}
			else if (_stricmp(command[0], "exit") == 0)
			{
				printf("\nTerminating the Marvellous Virtual File System...\n");
				break;
			}

			else if (_stricmp(command[0], "stat") == 0)
			{
				printf("\nERROR : File name is missing !!!\nUse 'man stat' command.\n");
			}

			else if (_stricmp(command[0], "fstat") == 0)
			{
				printf("\nERROR : File name is missing !!!\nUse 'man fstat' command.\n");
			}

			else if (_stricmp(command[0], "close") == 0)
			{
				printf("\nERROR : File name is missing !!!\nUse 'man close' command.\n");
			}

			else if (_stricmp(command[0], "rm") == 0)
			{
				printf("\nERROR : File name is missing !!!\nUse 'man rm' command.\n");
			}

			else if (_stricmp(command[0], "man") == 0)
			{
				printf("\nERROR : Command name is missing !!!\nUse 'help' command.\n");
			}

			else if (_stricmp(command[0], "write") == 0)
			{
				printf("\nERROR : File name is missing !!!\nUse 'man write' command.\n");
			}

			else if (_stricmp(command[0], "truncate") == 0)
			{
				printf("\nERROR : File name is missing !!!\nUse 'man truncate' command.\n");
			}

			else if (_stricmp(command[0], "create") == 0)
			{
				printf("\nERROR : File name and permission is missing !!!\nUse 'man create' command.\n");
			}

			else if (_stricmp(command[0], "open") == 0)
			{
				printf("\nERROR : File name and permission is missing !!!\nUse 'man open' command.\n");
			}

			else if (_stricmp(command[0], "read") == 0)
			{
				printf("\nERROR : File name and no. of bytes to read is missing !!!\nUse 'man read' command.\n");
			}

			else if (_stricmp(command[0], "lseek") == 0)
			{
				printf("\nERROR : Insufficiient Arguments !!!\nUse 'man lseek' command.\n");
			}

			else
			{
				printf("\nERROR: Command not found!!!\n");
			}

			continue;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else if (count == 2)												//count = 2
		{
			if (_stricmp(command[0], "stat") == 0)							//stat
			{
				ret = stat_file(command[1]);

				if (ret == -1)
				{
					printf("ERROR: Incorrect Parameters\n");
				}

				else if (ret == -2)
				{
					printf("ERROR: There is no such file\n");
				}
			}

			else if (_stricmp(command[0], "fstat") == 0)					//fstat
			{
				ret = fstat_file(atoi(command[1]));

				if (ret == -1)
				{
					printf("ERROR: Incorrect Parameters\n");
				}
				
				else if (ret == -2)
				{
					printf("ERROR: There is no such file\n");
				}
			}

			else if (_stricmp(command[0], "close") == 0)					//close
			{
				ret = CloseFileByName2(command[1]);

				if (ret == -1)
				{
					printf("ERROR: There is no such file\n");
					continue;
				}

				printf("\nFile closed successfully");
			}

			else if (_stricmp(command[0], "rm") == 0)						//rm
			{
				ret = rm_File(command[1]);

				if (ret == -1)
				{
					printf("ERROR: There is no such file\n");
					continue;
				}

				printf("\nFile has been deleted successfully\n");
			}

			else if (_stricmp(command[0], "man") == 0)
			{
				man(command[1]);
			}

			else if (_stricmp(command[0], "write") == 0)
			{
				fd = GetFDFromName(command[1]);

				if (fd == -1)
				{
					printf("ERROR: Incorrect Parameter\n");
					continue;
				}

				printf("Enter the data:\n");
				scanf("%[^\n]", arr);

				ret = strlen(arr);

				if (ret == -1)
				{
					printf("ERROR: Incorrect Parameters\n");
					continue;
				}

				ret = WriteFile(fd, arr, ret);

				if (ret == -1)
				{
					printf("ERROR: Permission Denied\n");
				}

				else if (ret == -2)
				{
					printf("ERROR: There is no sufficient memory to write\n");
				}

				else if (ret == -3)
				{
					printf("ERROR: It is not a regular file\n");
				}
			}

			else if (_stricmp(command[0], "truncate") == 0)
			{
				ret = truncate_File(command[1]);

				if (ret == -1)
				{
					printf("ERROR: Incorrect Parameters\n");
				}

				else
				{
					printf("\nFile cleaned successfully\n");
				}
			}

			else if (_stricmp(command[0], "lseek") == 0)
			{
				printf("\nERROR : Insufficiient Arguments !!!\nUse 'man lseek' command.\n");
			}

			else
			{
				printf("\nERROR : Command not found!!!!!\n");
				continue;
			}

			continue;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else if (count == 3)
		{
			if (_stricmp(command[0], "create") == 0)
			{
				ret = CreateFile(command[1], atoi(command[2]));

				if (ret >= 0)
				{
					printf("File is Successfully created with file descriptor: %d\n", ret);
				}

				else if (ret == -1)
				{
					printf("ERROR : Incorrect parameters\n");
				}

				else if (ret == -2)
				{
					printf("ERROR : There are no inodes\n");
				}

				else if (ret == -3)
				{
					printf("ERROR : File Already Exists\n");
				}

				else if (ret == -4)
				{
					printf("ERROR : Memory allocation failure\n");
				}

				continue;
			}

			else if (_stricmp(command[0], "open") == 0)
			{
				ret = OpenFile(command[1], atoi(command[2]));

				if (ret >= 0)
				{
					printf("File is Successfully opend with file descriptor: %d\n", ret);
				}

				else if (ret == -1)
				{
					printf("ERROR : Incorrect parameters\n");
				}

				else if (ret == -2)
				{
					printf("ERROR : File not present\n");
				}

				else if (ret == -3)
				{
					printf("ERROR : Permission Denied\n");
				}

				continue;
			}

			else if (_stricmp(command[0], "read") == 0)
			{
				fd = GetFDFromName(command[1]);

				if (fd == -1)
				{
					printf("Error : Incorrect parameters\n");
					continue;
				}

				ptr = (char*)malloc(sizeof(atoi(command[2])) + 1);

				if (ptr == NULL)
				{
					printf("Error : Memory allocation failure\n");
					continue;
				}

				ret = ReadFile(fd, ptr, atoi(command[2]));

				if (ret > 0)
				{
					_write(2, ptr, ret);
				}

				else if (ret == -1)
				{
					printf("ERROR : File not existing\n");
				}

				else if (ret == -2)
				{
					printf("ERROR : Permission Denied\n");
				}
				
				else if (ret == -3)
				{
					printf("ERROR : Reached at end of File\n");
				}
				
				else if (ret == -4)
				{
					printf("ERROR : It is not a regular file\n");
				}
				
				else if (ret == 0)
				{
					printf("ERROR : File is empty\n");
				}

				continue;
			}

			else if (_stricmp(command[0], "lseek") == 0)
			{
				printf("\nERROR : Insufficiient Arguments !!!\nUse 'man lseek' command.\n");
			}

			else
			{
				printf("\nERROR : Command not found!!!!!\n");
				continue;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else if (count == 4)
		{
			if (_stricmp(command[0], "lseek") == 0)
			{
				fd = GetFDFromName(command[1]);

				if (fd == -1)
				{
					printf("Error: Incorrect parameters\n");
					continue;
				}

				ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));

				if (ret == -1)
				{
					printf("ERROR: Unable to perform lseek\n");
				}
			}
			else
			{
				printf("\nERROR:Command Not found!!!\n");
				continue;
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		else
		{
			printf("\n ERROR: Command not found!!!\n");
			continue;
		}
	}
	return 0;
}