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

**************************************************************************/

#include "ext.h"

#define FLAG_UNSET 0
#define FLAG_SET 1

void rm_dir(BYTE *dir);
void rm_file(BYTE *patt);
void rm_list(BYTE *list);

BYTE flag_i = FLAG_UNSET;
BYTE flag_v = FLAG_UNSET;
BYTE flag_r = FLAG_UNSET;
BYTE flag_h = FLAG_UNSET;

void rm_dir(BYTE *dir)
{
  BYTE path[80];
  strcpy(path,dir);
  strcat(path,"\\*.*");
#ifdef debug
  printf("rm_dir (%d): path=(%s)[%x] dir=(%s)[%x]\n",__LINE__,path,path,dir,dir);
#endif  
  rm_file((BYTE *)path);
  
  return;
}

void rm_file(BYTE *patt)
{
  BYTE r,f,i,ok=1;
  BYTE *p;
  BYTE fn[129]={0};
  struct ffblk fb;

#ifdef debug	  
  printf("rm_file (%d): patt=(%s)[%x]\n",__LINE__,patt,patt);
#endif
  
  f=findfirst(patt,&fb,0|FA_DIREC);
#ifdef debug	
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
	
#ifdef debug
	printf("rm_file (%d): fn=(%s)[%x]\n",__LINE__,fn,fn);
#endif	
	if((fb.ff_name)[0] != '.') {
#ifdef debug
	  printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif	  

	  if(flag_i == FLAG_SET) {
		printf("remove %s (Y/N)? ",fn);
		/* Get character */
		asm mov ah,1
		asm int 21h
		asm mov r,al
		putchar('\n');
		putchar('\r');
		
#ifdef debug	
		printf("rm_file (%d): r=(%c)[%x],ok=%x\n",__LINE__,r,r,ok);
#endif
		if((r == 'y') || (r == 'Y')) {
		  if((fb.ff_attrib & FA_DIREC) == FA_DIREC) {
			if(flag_r == FLAG_SET) {
			  rm_dir(fn);
			}
#ifdef debug	
			printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif
			ok = rmdir(fn);
		  }
		  else
			ok = unlink(fn);
		}
	  }
	  else {
		if((fb.ff_attrib & FA_DIREC) == FA_DIREC) {
		  if(flag_r == FLAG_SET) {
			rm_dir(fn);
		  }
#ifdef debug			  
		  printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif
		  ok = rmdir(fn);
		}
		else
		  ok = unlink(fn);
	  }
	}
#ifdef debug	
	printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif	
	if(ok==0) {
	  if(flag_v == FLAG_SET) {
		puts(fn);
	  }
	}
	else if(ok == 0xff) {
	  printf("error [%x] while removing %s ",errno,fn);
	  perror("");
	}
  }
  else if((f==255) && (errno==2)) {
	printf("%s - no such file or directory.\n",patt);
  } 
  
#ifdef debug	
  printf("rm_file (%d): f=(%d)[%x],ok=%x\n",__LINE__,f,f,fb.ff_name,ok);
#endif  
  
  while((f=findnext(&fb))==0) {	
#ifdef debug	
	printf("rm_file (%d): f=(%d)[%x],ok=%d\n",__LINE__,f,f,fb.ff_name,ok);
	printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif	
	for(p+=strlen(patt)+1;(*p!='\\')&&(*p!='/');p--);
	if(p>patt) {
	  *(++p)=0;
	  strcpy(fn,patt);
	  if((*p != '\\') && (*p != '/'))
		strcat(fn,fb.ff_name);
	}
	else
	  strcpy(fn,fb.ff_name);
#ifdef debug	
	printf("rm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif	
	if(fb.ff_name[0] == '.')
	  continue; /* ignore . and .. */
	else if(flag_i == FLAG_SET) {
	  printf("remove %s (Y/N)? ",fn);
	  /* Get character */
	  asm mov ah,1
	  asm int 21h
	  asm mov r,al
	  putchar('\n');
	  putchar('\r');

#ifdef debug	
	  printf("rm_file (%d): r=(%c)[%x],ok=%x\n",__LINE__,r,r,ok);
#endif
	  if((r != 'y') && (r != 'Y'))
		continue;
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
	  printf("error while removing %s ",fn);
	  perror("");
	}
  }
  return;
}

void rm_list(BYTE *list)
{
  BYTE fn[129]={0};
  FILE *f;

#ifdef DEBUG  
  fprintf(stderr,"list=(%s)\n",list);
#endif
  
  f = fopen(list,"r");
  if(f != NULL) {
	while(fscanf(f,"%s",&fn)!=EOF){
	  rm_file(fn);
	}
	fclose(f);
  }
  else {
	fprintf(stderr,"Can not open list-file: %s\n",list);
	perror("");
  }
  
  return;
  
}
	
int main(BYTE n,BYTE *arg[])
{
  BYTE r,f,i,*p,fn[129]={0},dir[80];
  
  if(n > 1) {
	if(strncmp(arg[0],"deltree")==0) {
	  flag_r = FLAG_SET;
	}
	
	/* check flags */
	for(i=1;i<n;i++) {
#ifdef DEBUG
	  printf("flags: arg = %s\n",arg[i]);
#endif
	  if (arg[i][0] == '-') {
		switch(arg[i][1]) {
		 case 'i':
		  flag_i = FLAG_SET;
		  break;
		 case 'h':
		  printf("Usage: RM [OPTION]... FILE...\n\nRemove the FILE(s).\n\n");
		  printf("The OPTIONS are:\n\t-i: interactive mode: prompt (Y/N) for each file\n");
		  printf("\t-v: verbose: print filename of each removed file\n");
		  printf("\t-r: recurse sub-directories. Removes all files in subdirs too\n");
		  printf("\t     if the program is executed as deltree -r is implicit");
		  printf("\t-h: display this help and exit\n");
		  printf("\nFILE is either a filename OR a filename preceeded by '@',\n"
				 " in which case the file is opened and read, treating every\n"
				 " line as a filename to remove.\n");
		  printf("\nRM is part of DOG (http://dog.sf.net/)\n");
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
	for(i=1;i<n;i++) {
#ifdef debug
printf("main for-loop: arg = %s\n",arg[i]);
#endif
	  if (arg[i][0] == '@') {
		rm_list(&arg[i][1]);
	  }
	  else if (arg[i][0] == '-') {
		;
	  }
	  else {
		if((p=strstr(arg[i],"*.*"))!=NULL) {
		  printf("Removing %s - Are you sure(Y/N)? ",arg[i]);
		  /* Get character */
		  
		  asm mov ah,1
		  asm int 21h
		  asm mov r,al
		  putchar('\n');
		  putchar('\r');
		  
		  /* get to next arg if not 'Y' or 'y' */
		  if((r != 'y') && (r != 'Y'))
		  break;
		}
		rm_file(arg[i]);
	  }
    }
  }
  else
	puts("Invalid number of arguments.");  
  
  return 0;
}


