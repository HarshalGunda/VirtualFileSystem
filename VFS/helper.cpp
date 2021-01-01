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

		}

		else if (from == START)
		{

		}

		else if (from == END)
		{

		}
	}
}

//##############################################################################################################################


//##############################################################################################################################


//##############################################################################################################################