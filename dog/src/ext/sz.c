/*

SZ.C - DOG - Alternate command processor for freeDOS

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
02.01.02 - Tried to fix disk size and disk free calculations - failed ->
            this feature is now disabled
2002-11-21 - Improved help text, prints to std err - WB
2024-07-02 - Add -z for human a readable size, like ls - WB
**************************************************************************/

#include "ext.h"

#define FLAG_K 0x01 /* 0000 0001 */
#define FLAG_M 0x02 /* 0000 0010 */
#define FLAG_Z 0x04 /* 0000 0100 */

#define GIGA 1073741824
#define MEGA 1048576
#define KILO 1024

DWORD disk_free = 0, disk_size = 0;

struct ts_flags
{
	 WORD attrs;
	 BYTE **patt;
	 BYTE npatt;
	 BYTE flags;
}sz_f;

void do_sz(void);
int init(int nargs, char *arg[]);
char * format_size(DWORD sz, BYTE *s);

int main(int nargs, char *argv[])
{

	 int r;
	 r = init(nargs,argv); /* parses argv and extracts flags and patterns */
	 if (r == 0)
	     do_sz();

	 free(sz_f.patt);
	 return r;
}


/*
 * Return -1 for error 0 for OK 1 for OK, but stop (e.g. help)
 */
int init(int nargs, char *arg[])
{
    BYTE t,i,j,k,*p;
    WORD df[2], dt[2];

    sz_f.npatt = 0;
    sz_f.patt = (BYTE **) malloc(nargs * (sizeof(BYTE *)));

    sz_f.patt[0] = "*.*";

#ifdef SZ_DEBUG
    fprintf(stderr,"init:0: sz_f.patt[0]=%s; nargs=%d\n",sz_f.patt[0],nargs);
#endif

    sz_f.attrs = FA_NORMAL|FA_HIDDEN|FA_SYSTEM;
    k = 0;

    if (nargs > 1) {
	for(i=1;i<nargs;i++) {

#ifdef SZ_DEBUG
	    fprintf(stderr,"init:1: sz_f.patt[0]=%s k=%u\n",sz_f.patt[0],k);
#endif
	    if (arg[i][0] == '-') {
		switch(arg[i][1]) {
		case 'h':
		    fputs("Usage: SZ [OPTION] FILEPATTERN...\n\n"
			  "Counts size of files in directory\n\n", stderr);
		    fputs("The OPTIONS are:\n",stderr);
		    fputs("\t-k:  Show result in kilobytes\n",stderr);
		    fputs("\t-m:  show result in megabytes\n",stderr);
		    fputs("\t-h:  display this help and exit\n",stderr);
		    fputs("\t-?:  display usage and exit\n",stderr);
		    fputs("\t-r:  NOT IMPLEMENTED YET. recurse sub-directories.\n",stderr);
		    fputs("\nFILEPATTERN is file pattern like *.COM, *.?\n", stderr);
		    fputs("\nSZ is part of DOG (http://dog.sf.net/)\n", stderr);
		    free(sz_f.patt);
		    return 0;
		case 'k':
		    sz_f.flags = FLAG_K;
		    break;
		case 'm':
		    sz_f.flags = FLAG_M;
		    break;
		case 'z':
		    sz_f.flags = FLAG_Z;
		    break;
		case '?':
		    printf("Usage: SZ [-h|-k|-m|-z|-?|(-r)] FILEPATTERN...\n");
		    free(sz_f.patt);
		    return 0;
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
#ifdef SZ_DEBUG
		printf("init:2: sz_f.patt[0]=%s sz_f.patt=0x%x, npatt=%u, k=%u\n",sz_f.patt[0],sz_f.patt,sz_f.npatt,k);
#endif
	    }
	}
    }

    if (k == 0)
	sz_f.npatt = 1;
    else
	sz_f.npatt = k;

#if 0
    asm push AX      ;
    asm push BX      ;
    asm push CX      ;
    asm push DX      ;
    asm mov ah,19h   ;
    asm int 21h      ;
    asm mov dl,al    ;
    asm mov AH,36h   ;
    asm int 21h      ;
    asm push dx      ;
    asm push cx      ;
    asm push ax      ;
    asm mul bx       ;
    asm mul cx       ;
    asm mov df[0],dx ;
    asm mov df[1],ax ;
    asm pop ax       ;
    asm pop cx       ;
    asm pop bx       ;
    asm mul bx       ;
    asm mul cx       ;
    asm mov dt[0],dx ;
    asm mov dt[1],ax ;
    asm POP DX       ;
    asm pop CX       ;
    asm pop BX       ;
    asm POP AX       ;

    disk_free = ((DWORD)df[0] << 16) + df[1];
    disk_size = ((DWORD)dt[0] << 16) + df[1];
#endif

#ifdef SZ_DEBUG
    printf("init:3: sz_f.npatt=%u\n",sz_f.npatt);
#endif

    return 0;
}

void do_sz(void)
{
    struct ffblk *fb;
    BYTE r,i,j,m,str[5]={'\0'},strz[10]={'\0'},strhz[10]={'\0'},*p,nf,hf;
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

#ifdef SZ_DEBUG
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

#ifdef SZ_DEBUG
	fprintf(stderr,"do_sz: r=%d\n",r);
#endif
	while (1) {

#ifdef SZ_DEBUG
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

#ifdef SZ_DEBUG
	fprintf(stderr,"do_sz: r=%d\n",r);
#endif

	printf("\n\n%s:",sz_f.patt[j]);
	if (sz_f.flags == FLAG_Z) {
	    printf("\n%10s in %d file(s)"
		   "\n%10s in %d hidden file(s)\n",
		   format_size(s, strz), nf, format_size(h, strhz), hf);

	}
	else {
	    if (sz_f.flags == FLAG_K) {
		h /= KILO;
		s /= KILO;
		strcpy(str,"kilo");
	    }
	    else if (sz_f.flags == FLAG_M) {
		h /= MEGA;
		s /= MEGA;
		strcpy(str,"mega");
	    }

	    printf("\n%11lu %sbytes in %d file(s)\n"
		   "%11lu %sbytes in %d hidden file(s)\n",s,str,nf,h,str,hf);
	}
	free(fb);
	if (m == 1) free(sz_f.patt[j]);
	 }

	 if (sz_f.flags == FLAG_K) {
	     disk_free /= KILO;
	     disk_size /= KILO;
	     strcpy(str,"kilo");
	 }
	 else if (sz_f.flags == FLAG_M) {
	     disk_free /= MEGA;
	     disk_size /= MEGA;
	     strcpy(str,"mega");
	 }
#if 0
	 printf("\n%11lu %sbytes free on drive \n%11lu %sbytes total diskspace\n",disk_free,str,disk_size,str);
#endif

	 return;
}

/*
 * Returns a char string representing the size in human readable form:
 * e.g.
 *  - >1024 ==> 1KB
 *  - >1048576 ==> 1MB
 *  - etc.
 */
char * format_size(DWORD sz, BYTE *s)
{
    unsigned long int f;

    if (sz < KILO) {
	sprintf(s, "%lu B ", sz);
    }
    else if (sz < MEGA) {
	f =  ((sz % KILO) * 10) / KILO; /* 1 decimal point */
	sz = sz / KILO;
	sprintf(s, "%lu.%u KB", sz, f);
    }
    else if (sz < GIGA) {
	f =  ((sz % MEGA) * 10) / MEGA; /* 1 decimal point */
	sz = sz / MEGA;
	sprintf(s, "%lu.%u MB", sz, f);
    }
    else {
	f =  ((sz % GIGA) / KILO);
	f = (f * 10) / KILO;
	f = f / KILO; /* 1 decimal point, keep it within long int... */
	sz = sz / GIGA;
	sprintf(s, "%lu.%u GB", sz, f);
    }

    return s;
}
