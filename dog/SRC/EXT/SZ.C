/*

SZ.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

#define FLAG_K 0x01 /* 0000 0001 */
#define FLAG_M 0x02 /* 0000 0010 */

DWORD disk_free, disk_size;

struct ts_flags
{
	 WORD attrs;
	 BYTE **patt;
	 BYTE npatt;
	 BYTE flags;
}sz_f;

void do_sz();
int init(int nargs, char *arg[]);

int main(int nargs, char *argv[])
{
	 
	 int r;
	 r = init(nargs,argv); /* parses argv and extracts flags and patterns */
	 if (r == 0)
				do_sz();

	 free (sz_f.patt);
	 return r;
}


/*
 * Return -1 for error 0 for OK 1 for OK, but stop (e.g. help)
 */
int init(int nargs, char *arg[])
{
	 BYTE t,i,j,k,*p;
	 DWORD max,mbx,mcx,mdx;
	 
	 sz_f.npatt = 0;
	 sz_f.patt = (BYTE **) malloc(nargs * (sizeof(BYTE *)));
	 
	 sz_f.patt[0] = "*.*";
	 
#ifdef sz_debug
	 fprintf(stderr,"init:0: sz_f.patt[0]=%s; nargs=%d\n",sz_f.patt[0],nargs);
#endif
	 
	 sz_f.attrs = FA_NORMAL|FA_HIDDEN|FA_SYSTEM;
	 k = 0;

	 if (nargs > 1) {
			for(i=1;i<nargs;i++) {

#ifdef sz_debug
fprintf(stderr,"init:1: sz_f.patt[0]=%s k=%u\n",sz_f.patt[0],k);
#endif
				 if (arg[i][0] == '-') {
						switch(arg[i][1]) {
						 case 'h':
							 printf("The switches are:\n\t-k: Show result in kilobytes\n");
							 printf("\t-m: show result in megabytes\n");
							 free(sz_f.patt);
							 return 1;
						 case 'k':
							 sz_f.flags = FLAG_K;
							 break;
						 case 'm':
							 sz_f.flags = FLAG_M;
							 break;
						 case '?':
							 printf("sz [-h|-k|-m|-?|(-r)] [d:\\dir\\filename.ext]\n");
							 free(sz_f.patt);
							 return 1;
						 default:
							 printf("Incorrect switch %s try -h\n",arg[i]);
							 free(sz_f.patt);
							 return (-1);
						}
				 }
				 else if(strnicmp(arg[i],"/",1)==0) {
						puts("In DOG we use '-' as switch NOT '/'.");
						free(sz_f.patt);
						return (-1);
				 }
				 else {
						sz_f.patt[k++] = arg[i];
#ifdef sz_debug
						printf("init:2: sz_f.patt[0]=%s sz_f.patt=0x%x, npatt=%u, k=%u\n",sz_f.patt[0],sz_f.patt,sz_f.npatt,k);
#endif
				 }
			}
	 }
	 
	 if (k == 0)
		 sz_f.npatt = 1;
	 else
		 sz_f.npatt = k;
	 
	 asm push AX
	 asm push DX
	 asm mov AH,36h
	 asm xor dl,dl
	 asm int 21h
	 asm mov max,AX
	 asm mov mbx,BX
	 asm mov mcx,CX
	 asm MOV mdx,DX
	 asm POP DX
	 asm POP AX
			
   disk_free = (DWORD)max*(DWORD)mbx*(DWORD)mcx; 
   disk_size = (DWORD)max*(DWORD)mcx*(DWORD)mdx;

#ifdef sz_debug
	 printf("init:3: sz_f.npatt=%u\n",sz_f.npatt);
#endif

	 
	 return 0;
}




void do_sz()
{
	 struct ffblk *fb;
	 BYTE r,i,j,m,str[5]={'\0'},*p,nf,hf;
	 WORD max,mbx,mcx,mdx;
	 DWORD h,s;
	 
	 j=0;
	 m=0;
	 
	 fb = malloc(sizeof(struct ffblk));
	 	 	 
	 for(j=0;j < sz_f.npatt;j++) {
			s=0;
			h=0;
			nf=0; 
			hf=0;
			
#ifdef sz_debug
		fprintf(stderr,"do_sz:3: sz_f.patt[%u]=%s offset=%x sz_f.patt[%d][last] =  %c\n",j,sz_f.patt[0],sz_f.patt[0],j,sz_f.patt[j][strlen(sz_f.patt[j])-1]);
#endif
			if((sz_f.patt[j][strlen(sz_f.patt[j])-1] == '\\') || (sz_f.patt[j][strlen(sz_f.patt[j])-1] == ':')) {
				 p = malloc(strlen(sz_f.patt[j]) + 3);
				 strcpy(p,sz_f.patt[j]);
				 strcat(p,"*.*");
				 sz_f.patt[j] = p;
				 m = 1;
			}

			r = findfirst(sz_f.patt[j], fb, FA_NORMAL|FA_HIDDEN|FA_SYSTEM);

#ifdef sz_debug
		fprintf(stderr,"do_sz: r=%d\n",r);
#endif
			while (1) {

#ifdef sz_debug
fprintf(stderr,"do_sz: r=%d\n",r);
fprintf(stderr,"do_sz: r=%d\ndate:%d time:%d\n",r,fb->ff_fdate,fb->ff_ftime);
#endif
				 if(r == 0) { /* TODO: check for possible errors */
						
						if ( (fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN ) { 
							 h += fb->ff_fsize;
							 hf++;
						}
						/*
								else if (((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM)){
						 h += fb->ff_fsize;
						 j++;
						 }
						 */
						else {
							 s += fb->ff_fsize;
							 nf++;
							 }
				 }
				 else
					 break;
				 r = findnext(fb);

			}

#ifdef sz_debug
fprintf(stderr,"do_sz: r=%d\n",r);
#endif

			if (sz_f.flags == FLAG_K) {
				 h /= 1024;
				 s /= 1024;
				 strcpy(str,"kilo");
			}
			else if (sz_f.flags == FLAG_M) {
				 h /= 1048576;
				 s /= 1048576;
				 strcpy(str,"mega");
			}
			
			printf("\n\n%s:",sz_f.patt[j]);
			printf("\n%11lu %sbytes in %d file(s)\n%11lu %sbytes in %d hidden file(s)\n",s,str,nf,h,str,hf);
			free(fb);
			
			if (m == 1) free(sz_f.patt[j]);
	 }
	 
	 if (sz_f.flags == FLAG_K) {
			disk_free /= 1024;
			disk_size /= 1024;
			strcpy(str,"kilo");
	 }
	 else if (sz_f.flags == FLAG_M) {
			disk_free /= 1048576;
			disk_size /= 1048576;
			strcpy(str,"mega");
	 }

	 printf("\n%11lu %sbytes free on drive \n%11lu %sbytes total diskspace\n",disk_free,str,disk_size,str);
	 return;
}

