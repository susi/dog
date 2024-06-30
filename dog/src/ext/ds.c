/*
	DS.C - Directory stack implementation for DOG. 
    Copyright (C) 2004 K. Hari Kiran

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Author's E-mail: harikiran_k@yahoo.com,
	                finite_state_automaton@yahoo.com 

*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <dir.h>
#include <dos.h>
#include <conio.h>

/* used for storing the directory of DS */
char Exe_Path[MAXPATH];

/* returns true if directory stack is empty */
int Empty_Dir(FILE *Stream)
{
	fgetc(Stream); /* read one character */

	if(feof(Stream)) /* EOF reached */
		return 1;
	else
		return 0;
}

/* pops a directory off the stack and returns it */
char *Pop_Dir(void)
{
	FILE *Stack, *Temp; /* streams for the stack and temp file */
	unsigned long Current_Line = 0, Total_Lines = 0; /* line counts */
	char Dir_Name[MAXPATH]; /* popped directory name */
	char sFile[MAXPATH]; /* stack file name with path */
	char tFile[MAXPATH]; /* temporary file name with path */

	/* build full path and name for both files */
	strcpy(sFile, Exe_Path);
	strcat(sFile, "stack.dat");

	strcpy(tFile, Exe_Path);
	strcat(tFile, "stack.tmp");

	/* open the files */
	/* "at+" mode is used since we want to create a file if it does not exist 
	   and we want to read it, using "w" overwrites a file */
	if((Stack = fopen(sFile, "at+")) == NULL) 
	{
		printf("DS: error creating stack file\n");
		return NULL;
	}

	/* "w" mode is used since we want to always overwrite the temporary file */
	if((Temp = fopen(tFile, "wt")) == NULL)
	{
		printf("DS: error creating temporary file\n");
		fclose(Stack);
		return NULL;
	}

	/* not an empty stack */
	if(!Empty_Dir(Stack))
	{
		/* determine the number of lines in the file */
		while(!feof(Stack))
		{
			fgets(Dir_Name, MAXPATH, Stack);
			Total_Lines++;
		}

		/* rewind file pointer since we have reached EOF in 
		   determining the no of lines */
		rewind(Stack);

		/* start over */
		while(!feof(Stack))
		{
			fgets(Dir_Name, MAXPATH, Stack);

			Current_Line++;

			/* copy all lines except the last one on to the temporary file */ 
			if(Current_Line < Total_Lines - 1)
			{
				fputs(strupr(Dir_Name), Temp);
			}
		}

		/* close both files */
		fclose(Stack);
		fclose(Temp);

		unlink(sFile); /* delete old stack file */
		rename(tFile, sFile); /* rename temporary file to stack file */

		/* null terminate to get rid of trailing '\n' */
		Dir_Name[strlen(Dir_Name) - 1] = '\0';

		return Dir_Name; /* return popped directory name */
	}
	else /* an empty stack */
	{
		printf("DS: directory stack empty\n");
		
		/* close both files */
		fclose(Stack);
		fclose(Temp);

		/* remove temporary file */
		unlink(tFile);

		return NULL; /* error */
	}
}

/* pushes a directory onto the stack */
void Push_Dir(char *Dir_Name)
{
	FILE *Stack; /* stream for stack file */
	char sFile[MAXPATH]; /* complete path and name of stack file */

	/* biuld complete path and file name */
	strcpy(sFile, Exe_Path);
	strcat(sFile, "stack.dat");

	/* open file for appending */
	if((Stack = fopen(sFile, "at")) == NULL)
	{
		printf("DS: error creating stack file\n");
		return;
	}

	/* put the directory onto the file and append a new line */
	fputs(strupr(Dir_Name), Stack);
	fputs("\n", Stack);

	/* close stack file */
	fclose(Stack);
}

/* builds a path for the stack and temporary files */
void Build_Path(char *Exe_Name)
{
	/* copy complete path and file name of DS.COM i.e "DRIVE:\SOMEDIR\DS.COM" */
	strcpy(Exe_Path, strupr(Exe_Name));
	/* null terminate this at the last occurance of '\'
	   i.e. "DRIVE:\SOMEDIR\DS.COM" becomes "DRIVE:\SOMEDIR\" */
	Exe_Path[(strrchr(Exe_Path, '\\') - Exe_Path) + 1] = '\0';
}

/* changes the directory */
void Change_Directory(char *New_Dir)
{
	char Drive[MAXPATH]; /* drive of the specified path */

	/* get the drive of the specified path */
	fnsplit(New_Dir, Drive, NULL, NULL, NULL);

	/* if drive is there; sometimes only directory names are
	   pushed onto the stack */
	if(Drive != NULL)
	{
		/* change drive first if the drive of the specified 
		   path not equal to current working drive */
		if((toupper(Drive[0])) != ('A' + getdisk()))
			setdisk(toupper(Drive[0]) - 'A');
	}

	/* change the working directory */
	if(chdir(New_Dir) == -1)
		printf("DS: path not found\n");
}

/* main function */
void main(int argc, char *argv[])
{
	char sFile[MAXPATH]; /* complete path and file name of stack file */
	char Current_Dir[MAXPATH], New_Dir[MAXPATH]; /* directory names */
	
	Build_Path(argv[0]); /* build paths for files */

	if(argc < 2)
	{
		printf("DS: insufficient arguments\n");
		return;
	}

	if(argv[1][0] == '-')
	{
		if(strcmpi(argv[1] + 1, "h") == 0)
		{
			printf("DS push [[drive:]path]|pop|clear\n\n");
			printf("   push          pushes the current directory on to the stack, if a\n"
			       "                 directory name is given the current directory is changed\n"
				   "                 to that directory\n"
				   "   [drive:]path	 the directory to change to\n"
				   "   pop           pops a directory off the stack and change to that directory\n"
				   "   clear         clears the stack\n");
		}
		else
			printf("DS: invalid switch\n");

		return;
	}

	if(strcmpi(argv[1], "pop") == 0) /* pop operation*/
	{
		strcpy(New_Dir, Pop_Dir()); /* pop it */
		
		if(New_Dir != NULL) /* if not null (end-of-stack) change directory */
			Change_Directory(New_Dir);
	}
	else if(strcmpi(argv[1], "push") == 0) /* push operation */
	{
		getcwd(Current_Dir, MAXPATH); /* get current directory */
		Push_Dir(Current_Dir); /* push current directory onto the stack */

		/* argument of the form "push dirname" so change to "dirname" */
		if(argc > 2)
			Change_Directory(argv[2]);
	}
	else if(strcmpi(argv[1], "clear") == 0) /* clear operation */
	{
		/* remove the stack file */
		strcpy(sFile, Exe_Path);
		strcat(sFile, "stack.dat");
		unlink(sFile);
	}
	else /* some other options? syntax error */
		printf("DS: syntax error\n");
}