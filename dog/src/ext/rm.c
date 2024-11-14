/*

RM.C - DOG - Alternate command processor for freeDOS

Copyright (C) 1999,2000 Wolf Bergenheim

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Developers:
Wolf Bergenheim (WB)

History
18.03.00 - Extracted from DOG.C - WB
2024-06-01 - Made the RT command buildable and documented.
2024-11-11 - Added -a flag to (try) to also remove hidden and system files.
             Read-Only files will get permission denied error.
**************************************************************************/
#include "ext.h"

#include <conio.h>

#define MODE_RM 0
#define MODE_RT 1

BYTE flag_i = FLAG_UNSET;
BYTE flag_r = FLAG_UNSET;
BYTE flag_v = FLAG_UNSET;
BYTE flag_h = FLAG_UNSET;
BYTE flag_a = FLAG_UNSET;

BYTE search_attribs = 0;

BYTE rm_dir(BYTE *dir);
BYTE rm_file(BYTE *patt);
BYTE rm_list(BYTE *list);

BYTE rm_dir(BYTE *dir)
{
  BYTE path[80];
  strcpy(path,dir);
  strcat(path,"\\*.*");
#ifdef RM_DEBUG
  printf("rm_dir (%d): path=(%s)[%x] dir=(%s)[%x]\n",__LINE__,path,path,dir,dir);
#endif
  return rm_file((BYTE *)path);

}

BYTE rm_file(BYTE *patt)
{
  BYTE r,f,i,ok=0,ret=0;
  BYTE *p;
  BYTE fn[129]={0};
  struct ffblk fb;

#ifdef RM_DEBUG
  printf("rm_file (%d): patt=(%s)[%x]\n",__LINE__,patt,patt);
#endif

  f=findfirst(patt,&fb,search_attribs|FA_DIREC);
#ifdef RM_DEBUG
  printf("rm_file (%d): patt=(%s)[%x] f=%x\n",__LINE__,patt,patt,f);
#endif

  if(f==0) {

		p = patt;
		p+=strlen(patt) - 1;
		if((*p == '\\') || (*p == '/')) {
			*p = '\0';
			p--;
		}
		for(;(*p!='\\')&&(*p!='/');p--);
		if(p>patt) {
			*(++p)=0;
			strcpy(fn,patt);
			strcat(fn,fb.ff_name);
		}
		else
			strcpy(fn,fb.ff_name);

#ifdef RM_DEBUG
		printf("rm_file (%d): fn=(%s)[%x]\n",__LINE__,fn,fn);
#endif
		if((fb.ff_name)[0] != '.') {
#ifdef RM_DEBUG
			printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif

			if(flag_i == FLAG_SET) {
				printf("remove %s (Y/N/A)? ",fn);
				r = getch();
				if (r==3) /* C-c */
					return -1;
				else
					printf("%c\n",r);

				if((r=='A') || (r=='a')) {
					r = 'y';
					flag_i = FLAG_UNSET;
				}
#ifdef RM_DEBUG
				printf("rm_file (%d): r=(%c)[%x],ok=%x\n",__LINE__,r,r,ok);
#endif
				if((r == 'y') || (r == 'Y')) {
					if((fb.ff_attrib & FA_DIREC) == FA_DIREC) {
						if(flag_r == FLAG_SET) {
							ret = rm_dir(fn);
						}
#ifdef RM_DEBUG
						printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif
						ok = rmdir(fn);
					}
					else
						ok = unlink(fn);

					if(ok==0) {
						if(flag_v == FLAG_SET) {
							puts(fn);
						}
					}
					else {
						ret = 1;
						printf("error while removing %s ",fn);
						perror("");
					}

				}
			}
			else {
				if((fb.ff_attrib & FA_DIREC) == FA_DIREC) {
					if(flag_r == FLAG_SET) {
						ret = rm_dir(fn);
					}
#ifdef RM_DEBUG
					printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif
					ok = rmdir(fn);
				}
				else
					ok = unlink(fn);

				if(ok==0) {
					if(flag_v == FLAG_SET) {
						puts(fn);
					}
				}
				else {
					ret = 1;
					printf("error while removing %s ",fn);
					perror("");
				}

			}

#ifdef RM_DEBUG
			printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif
		}
	}
  else if((f==255) && (errno==2)) {
	ret = 1;
		printf("%s - no such file or directory.\n",patt);
  }

#ifdef RM_DEBUG
  printf("rm_file (%d): f=(%d)[%x],ok=%x\n",__LINE__,f,f,fb.ff_name,ok);
#endif

  while((f=findnext(&fb))==0) {
#ifdef RM_DEBUG
		printf("rm_file (%d): f=(%d)[%x],ok=%d\n",__LINE__,f,f,fb.ff_name,ok);
		printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif
		if(fb.ff_name[0] == '.')
			continue; /* ignore . and .. */
		for(p+=strlen(patt)+1;(*p!='\\')&&(*p!='/');p--);
		if(p>patt) {
			*(++p)=0;
			strcpy(fn,patt);
			if((*p != '\\') && (*p != '/'))
				strcat(fn,fb.ff_name);
		}
		else
			strcpy(fn,fb.ff_name);
#ifdef RM_DEBUG
		printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif

		if(flag_i == FLAG_SET) {
			printf("remove %s (Y/N/A)? ",fn);
			/* Get character */
			r = getch();
			if (r==3) /* C-c */
				return -1;
			else
				printf("%c\n",r);

#ifdef RM_DEBUG
			printf("rm_file (%d): r=(%c)[%x],ok=%x\n",__LINE__,r,r,ok);
#endif
			if((r=='A') || (r=='a')) {
				r = 'y';
				flag_i = FLAG_UNSET;
			}

			if((r != 'y') && (r != 'Y')) {
				ret = 1;
				continue;
			}
		}

		if((fb.ff_attrib & FA_DIREC) == FA_DIREC) {
			if(flag_r == FLAG_SET) {
				rm_dir(fn);
			}
			ok = rmdir(fn);
		}
		else
			ok = unlink(fn);
		if(ok==0) {
			if(flag_v == FLAG_SET) {
				puts(fn);
			}
		}
		else {
	  ret = 1;
			printf("error while removing %s ",fn);
			perror("");
		}
  }
  return ret;
}

BYTE rm_list(BYTE *list)
{
  BYTE fn[129]={0};
  FILE *f;
  BYTE ret=0;

#ifdef RM_DEBUG
  fprintf(stderr,"list=(%s)\n",list);
#endif

  f = fopen(list,"r");
  if(f != NULL) {
	while(fscanf(f,"%s",&fn)!=EOF){
	  ret = rm_file(fn);
	}
	fclose(f);
  }
  else {
	ret = 1;
	fprintf(stderr,"Can not open list-file: %s - ",list);
	perror("");
  }

  return ret;

}

void print_help(BYTE mode) {
	if(mode == MODE_RM)
		printf("Usage: RM [OPTION]... FILE...\n\nRemove the FILE(s).\n\n");
	else
		printf("Usage: RT [OPTION]... DIRECTORY...\n\nRemove the DIRECTORY(s) with all subdirectories and files.\n\n");
	printf("The OPTIONS are:\n\t-i:  interactive mode: prompt (Yes/No/All) for each file or directory\n");
	printf("\t-v:  verbose: print filename of each removed file\n");
	if(mode == MODE_RM) {
		printf("\t-a:  also remove read-only, system and hidden files\n");
		printf("\t-r:  recurse sub-directories. Removes all files in subdirectories too\n");
		printf("\t       if the program is executed as rt, -r is implicit\n");
	}
	printf("  -h|-H|-?:  display this help and exit\n");
	if(mode == MODE_RM)
		printf("\nFILE is either a filename OR a filename preceeded by '@',\n");
	else
		printf("\nDIRECTORY is either a directory OR a filename preceeded by '@',\n");
	printf(" in which case the file is opened and read, treating every\n"
				 " line as a file/directory to remove.\n");
	printf("\nRM is part of DOG (https://dog.zumppe.net/)\n");
	return;
}

int main(BYTE n,BYTE *arg[])
{
  BYTE r,f,i,j,dir[80];
  BYTE mode = MODE_RM;
  int ret = 0;

	if(strstr(arg[0],"RT.COM")!=NULL) {
		mode = MODE_RT;
		flag_r = FLAG_SET;
	}

  if(n > 1) {

		/* check flags */
		for(i=1;i<n;i++) {
#ifdef RM_DEBUG
			printf("flags: arg = %s\n",arg[i]);
#endif
			if ((arg[i][0] == '-') || (arg[i][0] == '/')) {
				for(j=1;arg[i][j]!='\0';j++) {
					switch(arg[i][j]) {
					case 'a':
					    search_attribs = FA_NORMAL | FA_HIDDEN | FA_SYSTEM | FA_RDONLY;
					    flag_a = FLAG_SET;
					    break;
					 case 'i':
						flag_i = FLAG_SET;
						break;
					 case 'H':
					 case 'h':
					 case '?':
						print_help(mode);
						return 0;
					 case 'r':
						flag_r = FLAG_SET;
					break;
					 case 'v':
						flag_v = FLAG_SET;
						break;
					}
				}
			}
		}
		for(i=1;i<n;i++) {
#ifdef RM_DEBUG
			printf("main for-loop: arg = %s\n",arg[i]);
#endif
			if (arg[i][0] == '@') {
				rm_list(&arg[i][1]);
			}
			else if (arg[i][0] == '-') {
				;
			}
			else {
				if(strstr(arg[i],"*.*")!=NULL) {
					printf("Removing %s - Are you sure(Y/N)? ",arg[i]);
					/* Get character */

					asm mov ah,1 ;
					asm int 21h ;
					asm mov r,al ;
					putchar('\n');
					putchar('\r');

					/* get to next arg if not 'Y' or 'y' */
					if((r != 'y') && (r != 'Y'))
						break;
				}
				ret = rm_file(arg[i]);
			}
    }
  }
  else {
		puts("Invalid number of arguments.");
		print_help(mode);
		ret = 0xff;
  }

  return ret;
}
