/*

LS.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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
01.08.00 - Fixed a bug: when you type ls file that doesn't exist ls
           would loop in DR-DOS because it returns 1, not 18 as I
           thought it would... - WB
2001-05-34 - Extracted from DOG to be a standalone program.

****************************************************************************/

#include "ext.h"

void show_entry(struct ffblk *fb);
int init(int nargs, char *arg[]);
void do_ls(void);

#define FLAG_D 0x01 /* 0000 0001 */

struct ts_flags
{
	 WORD attrs;
	 BYTE **patt;
	 BYTE npatt;
	 BYTE flags;
}ls_f;

 

int main(int nargs, char *argv[])
{
	 
	 int r;
	 r = init(nargs,argv); /* parses argv and extracts flags and patterns */
	 if (r == 0)
				do_ls();

	 free (ls_f.patt);
	 return r;
}


/*
 * Print the size, name, etc of a dir entry.
 */
void show_entry(struct ffblk *fb)
{
	WORD h, min, sec, y, m, d;

	printf("%8ld   ",fb->ff_fsize);
	
	if (((fb->ff_attrib & FA_ARCH) == FA_ARCH) && (fb->ff_attrib & FA_LABEL != FA_LABEL)) {
		printf("A");
	}
	
	else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
	else printf("-");
	
	if ((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM) printf("S");
	else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("A");
	else printf("-");
	
	if ((fb->ff_attrib & FA_RDONLY) == FA_RDONLY) printf("R");
	else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("B");
	else printf("-");
	
	if ((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN) printf("H");
	else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("E");
	else printf("-");
	
	if ((fb->ff_attrib & FA_DIREC) == FA_DIREC) printf("D");
	else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
	else printf("-");
	
	y = ((fb->ff_fdate & 0x0fe00)>>9);
	y += 1980;
	m = (fb->ff_fdate &  0x01e0)>>5;
	d = (fb->ff_fdate &  0x001f);
	
	printf("   %02d.%02d.%04d   ",d,m,y);
	
	h = (fb->ff_ftime & 0x0F800)>>11;
	min = (fb->ff_ftime & 0x03E0)>>5;
	sec = (fb->ff_ftime & 0x001f)<<1; /* *2 */
	
	printf("%02d:%02d.%02d   ",h,min,sec);
	
	
	printf("%s\n",fb->ff_name);
	
	return;
			
}
/*
 * Return -1 for error 0 for OK 1 for OK, but stop (e.g. help)
 */
int init(int nargs, char *arg[])
{
	BYTE t,i,l,j,k,*p;

	ls_f.npatt = 1;
	ls_f.patt = (BYTE **) malloc(nargs * (sizeof(BYTE *)));
	
	ls_f.patt[0] = "*.*";

#ifdef ls_debug
fprintf(stderr,"do_ls:0: ls_f.patt[0]=%s; nargs=%d\n",ls_f.patt[0],nargs);
#endif

	ls_f.attrs = 0|FA_DIREC;
	l = 1;
	k = 0;

	if (nargs > 1) {
		for(i=1;i<nargs;i++) {

#ifdef ls_debug
fprintf(stderr,"do_ls:1: ls_f.patt[0]=%s\n",ls_f.patt[0]);
#endif
			if (arg[i][0] == '-') {
				switch(arg[i][1]) {
					case 'a':
					ls_f.attrs = FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN|FA_DIREC|FA_LABEL;
					break;
					case 'd':
					ls_f.flags = FLAG_D;
					break;
					case 'f':
					ls_f.attrs = FA_NORMAL;
					break;
					case 'h':
					printf("The switches are:\n\t-a: List ALL files and directories\n");
					printf("\t-d: list DIRECTORIES only\n\t-f: list normal FILES only\n\t-l: show volume label\n");
					free(ls_f.patt);
					return 1;
					case 'l':
					ls_f.attrs = FA_LABEL;
					break;
					case '?':
					printf("ls [-a|-d|-f|-h|-l|-?] [d:\\dir\\filename.ext]\n");
					free(ls_f.patt);
					return 1;
					default:
					printf("Incorrect switch %s\n",arg[i]);
					free(ls_f.patt);
					return (-1);
				}
			}
			else if(strnicmp(arg[i],"/",1)==0) {
				puts("In DOG we use '-' as switch NOT '/'.");
				free(ls_f.patt);
				return (-1);
			}
			else {
				ls_f.patt[k++] = arg[i];
#ifdef ls_debug
printf("do_ls:2: ls_f.patt[0]=%s hxls_f.patt=%x\n",ls_f.patt[0],ls_f.patt[0]);
#endif
			}
		}
	}
	
	ls_f.npatt += k;
	return 0;
}


/*
 * The main ls loop. searches for files untill no more files with patterrns
 * pointed by ls_f.patt are found.
 */
void do_ls(void)
{
	
	 BYTE *p, j, m, k;
	 signed char r;
	 struct ffblk *fb;
	 fb = malloc(sizeof(struct ffblk));
	 m = 0;

	 printf("    size   ASRHD   dd.mm.yyyy   hh:mm.ss   name\n\n");
		
	 for(j=0;j < ls_f.npatt;j++) {

#ifdef ls_debug
		fprintf(stderr,"do_ls:3: ls_f.patt[%u]=%s offset=%x ls_f.patt[%d][last] =  %c\n",j,ls_f.patt[0],ls_f.patt[0],j,ls_f.patt[j][strlen(ls_f.patt[j])-1]);
#endif
		if((ls_f.patt[j][strlen(ls_f.patt[j])-1] == '\\') || (ls_f.patt[j][strlen(ls_f.patt[j])-1] == ':')) {
			p = malloc(strlen(ls_f.patt[j]) + 3);
			strcpy(p,ls_f.patt[j]);
			strcat(p,"*.*");
			ls_f.patt[j] = p;
			m = 1;
		}
		
		r = findfirst(ls_f.patt[j], fb,ls_f.attrs);
		
#ifdef ls_debug
		fprintf(stderr,"do_ls: r=%d\n",r);
#endif
		
		while(1) {
				 
#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\n",r);
fprintf(stderr,"do_ls: r=%d\ndate:%d time:%d\n",r,fb->ff_fdate,fb->ff_ftime);
#endif


			 if(r == 0) { /* TODO: check for possible errors */
					if((ls_f.flags == FLAG_D) && ((fb->ff_attrib & FA_DIREC) != FA_DIREC))
						; /* if user specified -d flag, but entry isn't a directory, don't display */
					else
						show_entry(fb);
			 }
			 else
				 break;
			 r = findnext (fb);
		}
			 
#ifdef ls_debug
		fprintf(stderr,"do_ls: r=%d\n",r);
#endif
	 if(m == 1) free(ls_f.patt[j]); /* if patt[j] was previously malloced */
		 
	}
	 
	 free(fb);
	 return;
}


