#include "myheader.h"

void InitialiseSuperBlock()
{
	int i = 0;

	while (i < MAXINODE)
	{
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	}

	SUPERBLOCKobj.TotalInodes = MAXINODE;
	SUPERBLOCKobj.FreeInodes = MAXINODE;
}

//##############################################################################################################################

void CreateDILB()
{
	int j = 1;
	PINODE newn = NULL;
	PINODE temp = head;

	while (j <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));

		newn->LinkCount = 0;
		newn->ReferenceCount = 0;
		newn->FileType = 0;
		newn->FileSize = 0;
		newn->Buffer = NULL;
		newn->next = NULL;
		newn->InodeNumber = j;
	}

	if (temp == NULL)
	{
		head = newn;
		temp = head;
	}
	else
	{
		temp->next = newn;
		temp = temp->next;
	}

	j++;

	printf("\nDILB created successfully\n\n");
}

//##############################################################################################################################

int CreateFile(char* name, int permission)
{
	int k = 0;
	PINODE temp = head;

	if ((name == NULL) || (permission <= 0) || (permission > 3))
	{
		return -1;
	}

	if (SUPERBLOCKobj.FreeInodes == 0)
	{
		return -2;
	}

	if (Get_Inode(name) != NULL)
	{
		return -3;
	}

	(SUPERBLOCKobj.FreeInodes)--;

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
	strcpy_s(UFDTArr[k].ptrfiletable->ptrinode->FileName, name);
	UFDTArr[k].ptrfiletable->ptrinode->FileType = REGULAR;
	UFDTArr[k].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
	UFDTArr[k].ptrfiletable->ptrinode->ReferenceCount = 1;
	UFDTArr[k].ptrfiletable->ptrinode->LinkCount = 1;
	UFDTArr[k].ptrfiletable->ptrinode->FileActualSize = 0;
	UFDTArr[k].ptrfiletable->ptrinode->Permission = permission;
	UFDTArr[k].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);
	memset(UFDTArr[k].ptrfiletable->ptrinode->Buffer, 0, 1024);

	return k;
}

//##############################################################################################################################

int rm_File(char* name)
{
	int fd = 0;

	fd = GetFDFromName(name);

	if (fd == -1)
	{
		return -1;
	}

	UFDTArr[fd].ptrfiletable->ptrinode->LinkCount--;

	if (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
		UFDTArr[fd].ptrfiletable->ptrinode->FileName[0] = '\0';
		free(UFDTArr[fd].ptrfiletable);
	}

	UFDTArr[fd].ptrfiletable = NULL;
	(SUPERBLOCKobj.FreeInodes)++;

	return 0;
}

//##############################################################################################################################

void DisplayHelp()
{
	printf("\nls : list out all the existing files\n");
	printf("create : To create a new file\n");
	printf("rm : To delete the file\n");
	printf("open : To open an existing file\n");
	printf("close : To close the opened file\n");
	printf("closeall : To close all the opened files\n");
	printf("read : To read the data from the file\n");
	printf("write : To write the data into the file\n");
	printf("clear : To clear the console\n");
	printf("exit : To terminate the Virtual File System\n");
	printf("stat : To display the attributes about the existing file\n");
	printf("fstat : To display the attributes of the opened file\n");
	printf("truncate : To remove all the data from the file\n");
}

//##############################################################################################################################

int GetFDFromName(char* name)
{
	int x = 0;

	while (x < MAXINODE)
	{
		if (UFDTArr->ptrfiletable != NULL)
		{
			if (_stricmp((UFDTArr->ptrfiletable->ptrinode->FileName), name) == 0)
			{
				break;
			}
		}

		x++;
	}

	if (x == MAXINODE)
	{
		return -1;
	}
	else
	{
		return x;
	}
}

//##############################################################################################################################

PINODE Get_Inode(char* name)
{
	PINODE temp = head;
	int i = 0;

	if (name == NULL)
	{
		return NULL;
	}

	while (temp != NULL)
	{
		if (strcmp(name, temp->FileName) == 0)
		{
			break;
		}

		temp = temp->next;
	}

	return temp;
}

//##############################################################################################################################

void ls_file()
{
	int y = 0;
	PINODE temp = head;

	if (SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		printf("ERROR : There are no files\n");
	}
	else
	{
		printf("File Name\tInode Number\tFile Actual Size\tLink Count\n\n");

		while (temp != NULL)
		{
			if (temp->FileType != 0)
			{
				printf("%s\t\t%d\t\t%d\t\t%d\n", temp->FileName, temp->InodeNumber, temp->FileActualSize,temp->LinkCount);
			}

			temp = temp->next;
		}
	}
}

//##############################################################################################################################

int LseekFile(int fd, int size, int from)
{
	if ((fd < 0) || (from < 0) || (from > 2))
	{
		return -1;
	}

	if (UFDTArr[fd].ptrfiletable == NULL)
	{
		return -1;
	}

	if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ + WRITE))
	{
		if (from == CURRENT)
		{
			if (((UFDTArr[fd].ptrfiletable->readoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
			{
				return -1;
			}

			if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
		}

		else if (from == START)
		{
			if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
			{
				return -1;
			}

			if (size < 0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->readoffset) = size;
		}

		else if (from == END)
		{
			if (((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size) > MAXFILESIZE)
			{
				return -1;
			}

			if (((UFDTArr[fd].ptrfiletable->readoffset) + size) < 0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) + size;
		}
	}

	else if (UFDTArr[fd].ptrfiletable->mode == WRITE)
	{
		if (from == CURRENT)
		{
			if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > MAXFILESIZE)
			{
				return -1;
			}

			if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
			{
				return -1;
			}

			if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
			{
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
			}

			(UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
		}

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

			if (size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
			{
				(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
			}

			(UFDTArr[fd].ptrfiletable->writeoffset) = size;
		}

		else if (from == END)
		{
			if (((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size) > MAXFILESIZE)
			{
				return -1;
			}

			if (((UFDTArr[fd].ptrfiletable->writeoffset) + size) < 0)
			{
				return -1;
			}

			(UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size;
		}
	}

	return 0;
}

//##############################################################################################################################

int OpenFile(char* name, int mode)
{
	int z = 0;
	PINODE temp3 = NULL;

	if ((name == NULL) || (mode < 0))
	{
		return -1;
	}

	temp3 = Get_Inode(name);

	if (temp3 == NULL)
	{
		return -2;
	}

	if ((temp3->Permission) < mode)
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
	
	(UFDTArr[z].ptrfiletable) = (PFILETABLE)malloc(sizeof(FILETABLE));

	if ((UFDTArr[z].ptrfiletable) == NULL)
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
	else if(mode == WRITE)
	{
		UFDTArr[z].ptrfiletable->writeoffset = 0;
	}

	UFDTArr[z].ptrfiletable->ptrinode = temp3;
	(UFDTArr[z].ptrfiletable->ptrinode->ReferenceCount)++;

	return z;
}

//##############################################################################################################################

void CloseFileByName1(int fd4)
{
	UFDTArr[fd4].ptrfiletable->readoffset = 0;
	UFDTArr[fd4].ptrfiletable->writeoffset = 0;

	if ((UFDTArr[fd4].ptrfiletable->ptrinode->ReferenceCount) > 0)
	{
		(UFDTArr[fd4].ptrfiletable->ptrinode->ReferenceCount)--;
	}
}

//##############################################################################################################################

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

//##############################################################################################################################

void CloseAllFile()
{
	int p = 0;

	while (p < MAXINODE)
	{
		if (UFDTArr[p].ptrfiletable == NULL)
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

//##############################################################################################################################

int ReadFile(int fd5, char* arr, int isize)
{
	int read_size = 0;

	if ((UFDTArr[fd5].ptrfiletable) == NULL)
	{
		return -1;
	}

	if (((UFDTArr[fd5].ptrfiletable->mode) != READ) && ((UFDTArr[fd5].ptrfiletable->mode) != READ + WRITE))
	{
		return -2;
	}

	if (((UFDTArr[fd5].ptrfiletable->ptrinode->Permission) != READ) && ((UFDTArr[fd5].ptrfiletable->ptrinode->Permission) != READ + WRITE))
	{
		return -3;
	}

	if ((UFDTArr[fd5].ptrfiletable->readoffset) == (UFDTArr[fd5].ptrfiletable->ptrinode->FileActualSize))
	{
		return -4;
	}

	read_size = (UFDTArr[fd5].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd5].ptrfiletable->readoffset);

	if (read_size < isize)
	{
		strncpy(arr, (UFDTArr[fd5].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd5].ptrfiletable->readoffset), read_size);
		(UFDTArr[fd5].ptrfiletable->readoffset) = (UFDTArr[fd5].ptrfiletable->readoffset) + read_size;
	}
	else
	{
		strncpy(arr, (UFDTArr[fd5].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd5].ptrfiletable->readoffset), read_size);
		(UFDTArr[fd5].ptrfiletable->readoffset) = (UFDTArr[fd5].ptrfiletable->readoffset) + isize;
	}

	return isize;
}

//##############################################################################################################################

int WriteFile(int fd6, char* arr, int isize)
{
	/*if ((UFDTArr[fd5].ptrfiletable) == NULL)
	{
		return -1;
	}*/

	if (((UFDTArr[fd6].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd6].ptrfiletable->mode) != READ + WRITE))
	{
		return -1;
	}

	if (((UFDTArr[fd6].ptrfiletable->ptrinode->Permission) != WRITE) && ((UFDTArr[fd6].ptrfiletable->ptrinode->Permission) != READ + WRITE))
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

//##############################################################################################################################

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

	printf("\n\t\tStatistical Information\n");
	printf("File Name :			%s\n", temp4->FileName);
	printf("Inode Number :		%d\n", temp4->InodeNumber);
	printf("File Size :			%d\n", temp4->FileSize);
	printf("Actual File Size :	%d\n", temp4->FileActualSize);
	printf("Link Count :		%d\n", temp4->LinkCount);
	printf("Reference Count :	%d\n", temp4->ReferenceCount);
	
	if (temp4->Permission == 1)
	{
		printf("File Permission : Read Only\n");
	}
	else if (temp4->Permission == 2)
	{
		printf("File Permission : Write\n");
	}
	else if (temp4->Permission == 3)
	{
		printf("File Permission : Read and Write\n");
	}

	return 0;
}

//##############################################################################################################################

int stat_file(char* name)
{
	PINODE temp5 = head;
	int t = 0;

	if (name == NULL)
	{
		return -1;
	}

	if (temp5 != NULL)
	{
		if (strcmp(name, temp5->FileName) == 0)
		{
			break;
		}

		temp5 = temp5->next;
	}

	if (temp5 == NULL)
	{
		return -2;
	}

	printf("\n\t\tStatistical Information\n");
	printf("File Name :			%s\n", temp5->FileName);
	printf("Inode Number :		%d\n", temp5->InodeNumber);
	printf("File Size :			%d\n", temp5->FileSize);
	printf("Actual File Size :	%d\n", temp5->FileActualSize);
	printf("Link Count :		%d\n", temp5->LinkCount);
	printf("Reference Count :	%d\n", temp5->ReferenceCount);

	if (temp5->Permission == 1)
	{
		printf("File Permission : Read Only\n");
	}
	else if (temp5->Permission == 2)
	{
		printf("File Permission : Write\n");
	}
	else if (temp5->Permission == 3)
	{
		printf("File Permission : Read and Write\n");
	}

	return 0;
}

//##############################################################################################################################

int truncate_file(char* name)
{
	int fd8 = GetFDFromName(name);

	if (fd8 == -1)
	{
		return -1;
	}

	memset(UFDTArr[fd8].ptrfiletable->ptrinode->Buffer, 0, MAXFILESIZE);
	UFDTArr[fd8].ptrfiletable->readoffset = 0;
	UFDTArr[fd8].ptrfiletable->writeoffset = 0;
	UFDTArr[fd8].ptrfiletable->ptrinode->FileActualSize = 0;

	return 0;
}

//##############################################################################################################################

void man(char* name)
{
	if (name == NULL)
	{
		return;
	}

	if (_stricmp(name, "create") == 0)
	{
		printf("Description : Used to create a new regular file\n");
		printf("Usage : create filename permission\n");
		printf("Permissions : \n");
		printf("Read = 1\tWrite = 2\tRead+Write = 3\n");
		printf("e.g. create demo.txt 1\n");
	}
	else if (_stricmp(name, "read") == 0)
	{
		printf("Description : Used to read the data from the regular file\n");
		printf("Usage : read filename no_of_bytes_to_read\n");
		printf("Caution : No. of bytes must be less than 1024\n");
		printf("e.g. read demo.txt 425\n");
	}
	else if (_stricmp(name, "write") == 0)
	{
		printf("Description : Used to write the data into the regular file\n");
		printf("Usage : read filename\nAfter this, enter the data you want to write into the file\n");
		printf("e.g. read demo.txt\nHello, Have a good day !\n");
	}
	else if (_stricmp(name, "ls") == 0)
	{
		printf("Description : Used to list all the existing files\n");
		printf("Usage : ls\n");
	}
	else if (_stricmp(name, "stat") == 0)
	{
		printf("Description : Used to display the information of the file\n");
		printf("Usage : stat filename\n");
		printf("e.g. stat demo.txt\n");
	}
	else if (_stricmp(name, "fstat") == 0)
	{
		printf("Description : Used to display the information of the opened file\n");
		printf("Usage : fstat file_descriptor\n");
	}
	else if (_stricmp(name, "truncate") == 0)
	{
		printf("Description : Used to remove the data from the file\n");
		printf("Usage : truncate file_descriptor\n");
	}
	else if (_stricmp(name, "open") == 0)
	{
		printf("Description : Used to open the existing file\n");
		printf("Usage : open filename mode\n");
		printf("Modes :\n");
		printf("READ = 1\WRITE = 2\tREAD+WRITE = 3\n");
		printf("e.g.1) open demo.txt 2\n");
		printf("e.g.2) open hello.txt 1\n");
	}
	else if (_stricmp(name, "close") == 0)
	{
		printf("Description : Used to close the opened file\n");
		printf("Usage : close filename\n");
		printf("e.g. close demo.txt\n");
	}
	else if (_stricmp(name, "closeall") == 0)
	{
		printf("Description : Used to close all the opened files\n");
		printf("Usage : closeall\n");
	}
	else if (_stricmp(name, "lseek") == 0)
	{
		printf("Description : Used to change the file offset\n");
		printf("Usage : lseek filename ChangeInOffset StartPoint\n");
		printf("For ChangeInOffset :\n");
		printf("\tSTART = 0\n\tCURRENT = 1\n\tEND = 2\n");
		printf("e.g. lseek demo.txt 10 1\n");
	}
	else if (_stricmp(name, "rm") == 0)
	{
		printf("Description : Used to delete the esisting file\n");
		printf("Usage : rm filename\n");
		printf("e.g. rm demo.txt\n");
	}
	else
	{
		printf("ERROR : No manual entry available\n");
	}
}