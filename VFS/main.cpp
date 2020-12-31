#include "myheader.h"

int main()
{
	char* ptr = NULL;
	int ret = 0, fd = 0, count = 0;
	BOOL bret;
	//char username[20];
	//char password[20];

	char command[4][80], str[80], arr[MAXFILESIZE];
	InitialiseSuperBlock();
	CreateDILB();

	/*while (1)
	{
		printf("Enter the username : ");
		scanf("%s", &username);

		printf("Enter the password : ");
		scanf("%s", &password);

		//bret = Authentication(username, password);

		if (bret == TRUE)
		{
			break;
		}
		else
		{
			printf("Wrong username or password");
		}
	}*/

	printf("\n\t\t\tWelcome to the Virtual File System\n\n");

	while (1)
	{
		fflush(stdin);
		strcpy_s(str, "");

		printf("\nuser@VFS:~$ ");
		fgets(str, 80, stdin);

		count = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

		//##############################################################################################################################
		if (count == 1)
		{
			if (_stricmp(command[0], "ls") == 0)
			{
				//ls_file();
			}
			else if (_stricmp(command[0], "closeall") == 0)
			{
				CloseAllFile();
				printf("\nAll files has been closed successfully\n");
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
				printf("\nTerminating the Virtual File System...\n");
				break;
			}
			else if (_stricmp(command[0], "stat") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : stat filename\n");
			}
			else if (_stricmp(command[0], "fstat") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : fstat filename\n");
			}
			else if (_stricmp(command[0], "close") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : close filename\n");
			}
			else if (_stricmp(command[0], "rm") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : rm filename\n");
			}
			else if (_stricmp(command[0], "man") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : man command_name\n");
			}
			else if (_stricmp(command[0], "write") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : write filename\n");
			}
			else if (_stricmp(command[0], "truncate") == 0)
			{
				printf("ERROR : Incorrect Parameters\nUsage : truncate filename\n");
			}
			else
			{
				printf("\nERROR : Command Not Found !!!\n");
			}

			continue;
		}
		//##############################################################################################################################
		else if (count == 2)
		{
			if (_stricmp(command[0], "stat") == 0)
			{
				ret = stat_file(command[1]);

				if (ret == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
				}
				else if (ret == -2)
				{
					printf("\nERROR : There is no such file\n");
				}
			}

			else if (_stricmp(command[0], "fstat") == 0)
			{
				ret = fstat_file(atoi(command[1]));

				if (ret == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
				}
				else if (ret == -2)
				{
					printf("\nERROR : There is no such file\n");
				}
			}

			else if (_stricmp(command[0], "close") == 0)
			{
				ret = CloseFileByName2(command[1]);

				if (ret == -1)
				{
					printf("\nERROR : There is no such file\n");
				}
				else
				{
					printf("\nFile has been closed successfully\n");
				}
			}

			else if (_stricmp(command[0], "rm") == 0)
			{
				ret = rm_File(command[1]);

				if (ret == -1)
				{
					printf("\nERROR : There is no such file\n");
				}
				else
				{
					printf("\nFile has been deleted successfully\n");
				}
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
					printf("\nERROR : There is no such file\n");
					continue;
				}

				printf("\nEnter the data: \n");
				scanf("%[^'\n']s", arr);

				ret = strlen(arr);

				if (ret == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
					continue;
				}

				ret = WriteFile(fd, arr, ret);

				if (ret == -1)
				{
					printf("\nERROR : Permission Denied\n");
				}
				else if (ret == -2)
				{
					printf("\nERROR : There is sufficient memory to write\n");
				}
				else if (ret == -3)
				{
					printf("\nERROR : It is not a regular file\n");
				}
			}

			else if (_stricmp(command[0], "truncate") == 0)
			{
				fd = truncate_file(command[1]);

				if (ret == -1)
				{
					printf("\nERROR : Incorrect Parameters\n");
				}
				else
				{
					printf("\nERROR : File cleaned successfully\n");
				}
			}

			continue;
		}
		//##############################################################################################################################
		else if (count == 3)
		{
			if (_stricmp(command[0], "create") == 0)
			{
				fd = CreateFile(command[1], atoi(command[2]));

				if (fd >= 0)
				{
					printf("File has been created successfully with file descriptor : %d\n", fd);
				}
				else if (fd == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
				}
				else if (fd == -2)
				{
					printf("ERROR : There are no inodes\n");
				}
				else if (fd == -3)
				{
					printf("ERROR : File already exists\n");
				}
				else if (fd == -4)
				{
					printf("ERROR : Memory allocation failure\n");
				}
			}

			else if (_stricmp(command[0], "open") == 0)
			{
				fd = OpenFile(command[1], atoi(command[2]));

				if (fd >= 0)
				{
					printf("File has been successfully opened with file descriptor : %d\n", fd);
				}
				else if (fd == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
				}
				else if (fd == -2)
				{
					printf("ERROR : File not found\n");
				}
				else if (fd == -3)
				{
					printf("ERROR : Permission Denied\n");
				}
			}

			else if (_stricmp(command[0], "read") == 0)
			{
				fd = GetFDFromName(command[1]);

				if (fd == -1)
				{
					printf("ERROR : Incorrect Parameters\n");
					continue;
				}

				ptr = (char*)malloc(sizeof(atoi(command[2])) + 1);

				if (ptr == NULL)
				{
					printf("ERROR : Memory allocation failure\n");
					continue;
				}

				ret = ReadFile(fd, ptr, atoi(command[2]));

				if (ret == -1)
				{
					printf("ERROR : File does not exists\n");
				}
				else if (ret == -2)
				{
					printf("ERROR : Permission Denied\n");
				}
				else if (ret == -3)
				{
					printf("ERROR : Reached at end of the file\n");
				}
				else if (ret == -4)
				{
					printf("ERROR : It is not a regular file\n");
				}
				else if (ret == 0)
				{
					printf("ERROR : File is empty\n");
				}
				else if (ret > 0)
				{
					_write(2, ptr, ret);
				}
			}

			else
			{
				printf("\nERROR : Command not found !!!\n");
			}

			continue;
		}
		//##############################################################################################################################
		else if (count == 4)
		{
			if (_stricmp(command[0], "lseek") == 0)
			{
				fd = GetFDFromName(command[1]);

				if (fd == -1)
				{
					printf("\nERROR : Incorrect Paremeters\n");
					continue;
				}

				ret = LseekFile(fd, atoi(command[2]), atoi(command[3]));

				if (ret == -1)
				{
					printf("\nERROR : Unable to perform check\n");
				}
			}
			else
			{
				printf("\nERROR : Command not found !!!\n");
			}

			continue;
		}
		//##############################################################################################################################
		else
		{
			printf("\nERROR : Command not found !!!\n");
			continue;
		}
	}

	return 0;
}