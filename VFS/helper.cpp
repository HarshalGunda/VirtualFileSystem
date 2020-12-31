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

