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

void rm_file(BYTE *patt)
{
	BYTE r,f,i;
	BYTE *p;
	BYTE fn[129]={0};
	struct ffblk fb;
	
	f=findfirst(patt,&fb,0);
	
	if(f==0) {
		
		p = patt;
		for(p+=strlen(patt)+1;*p!='\\';p--);
		if(p>patt) {
			*(++p)=0;
			strcpy(fn,patt);
			strcat(fn,fb.ff_name);
		}
		else
			strcpy(fn,fb.ff_name);
				
		if(unlink(fn)==0) {
			puts(fn);
		}
	}
	else if((f==255) && (errno==2)) {
		printf("%s NOT found.\n",patt);
	}
	
	while(findnext(&fb)==0) {
		
		for(p+=strlen(patt)+1;*p!='\\';p--);
		if(p>patt) {
			*(++p)=0;
			strcpy(fn,patt);
			strcat(fn,fb.ff_name);
		}
		else
			strcpy(fn,fb.ff_name);
		
		if(unlink(fn)==0) {
			puts(fn);
		}
	}
	return;
}

void rm_list(BYTE *list)
{
  BYTE fn[129]={0};
  FILE *f;

  fprintf(stderr,"list=(%s)\n",list);
  
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
	for(i=1;i<n;i++) {
	  if (arg[i][0] == '@') {
		rm_list(&arg[i][1]);
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


