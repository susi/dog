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
2024-07-04 - Add -r to recurse into subdirectories - WB
**************************************************************************/

#include "ext.h"

#define FLAG_K 0x01 /* 0000 0001 */
#define FLAG_M 0x02 /* 0000 0010 */
#define FLAG_Z 0x04 /* 0000 0100 */
#define FLAG_R 0x08 /* 0000 1000 */

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

struct sz_fsize
{
    DWORD fsize;
    DWORD hsize;
    WORD  fn;
    WORD  hn;
};

void do_sz(void);
int init(int nargs, char *arg[]);
char * format_size(DWORD sz, BYTE *s);
struct sz_fsize* sz_dir(BYTE *dir);
struct sz_fsize* mk_sz_fsize(void);
void add_sz_fsize(struct sz_fsize *sz, struct sz_fsize *add);
struct sz_fsize* sz_file(BYTE *patt);
void print_size(char *patt, struct sz_fsize *sz);

struct sz_fsize* sz_dir(BYTE *dir)
{
  struct sz_fsize *sz;
  BYTE *patt;

  patt = malloc(strlen(dir)+4);
  if (patt == NULL) {
      fprintf(stderr, "Out of memory! (malloc failed in sz_dir)\n");
      exit(-1);
  }
  strcpy(patt, dir);
  strcat(patt, "\\*.*");
#ifdef SZ_DEBUG
  printf("sz_dir (%d): patt=(%s)[%x] dir=(%s)[%x]\n",__LINE__,patt,patt,dir,dir);
#endif

  sz = sz_file(patt);
  free(patt);
  return sz;
}

struct sz_fsize* mk_sz_fsize(void)
{
    struct sz_fsize *sz;
    sz = malloc(sizeof(struct sz_fsize));
    if (sz == NULL) {
	fprintf(stderr, "Out of memory! (malloc failed in mk_sz_fsize)\n");
	exit(-1);
    }
    sz->fsize=0;
    sz->hsize=0;
    sz->fn=0;
    sz->hn=0;
    return sz;
}

void add_sz_fsize(struct sz_fsize *sz, struct sz_fsize *add)
{
    sz->fsize += add->fsize;
    sz->hsize += add->hsize;
    sz->fn += add->fn;
    sz->hn += add->hn;
}

struct sz_fsize *sz_file(BYTE *patt)
{
    struct sz_fsize *sz;
    struct sz_fsize *dsz;
    struct ffblk *fb;
    BYTE *dir, *p;
    WORD nf, hf;
    int done, attr;
    DWORD s, h;

    fb = malloc(sizeof(struct ffblk));
    if (fb == NULL) {
	fprintf(stderr, "Out of memory! (malloc failed in sz_file)\n");
	exit(-1);
    }
    sz = mk_sz_fsize();

#ifdef SZ_DEBUG
    fprintf(stderr,"sz_file:1: patt=%s\n",patt);
#endif

    done = findfirst(patt, fb, sz_f.attrs);
    while(!done) {
#ifdef SZ_DEBUG
	fprintf(stderr,"sz_file:2: date:%x time:%x size:%lu name:%s\n",
		fb->ff_fdate,fb->ff_ftime, fb->ff_fsize, fb->ff_name);
#endif
	if ( (fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN ) {
	    sz->hsize += fb->ff_fsize;
	    sz->hn++;
	}
	else if ((fb->ff_attrib & FA_DIREC) == FA_DIREC) {
	    if (fb->ff_name[0] != '.') { // ignore . and ..
		dir = malloc(strlen(patt)+strlen(fb->ff_name));
		if (dir == NULL) {
		    fprintf(stderr, "Out of memory! (malloc failed in sz_file)\n");
		    exit(-1);
		}
		strcpy(dir, patt);
#ifdef SZ_DEBUG
		printf("sz_file:3a: dir='%s'\n");
#endif
		p = strrchr(dir, '\\');
#ifdef SZ_DEBUG
		printf("sz_file:3b: dir='%s'\n");
#endif
		if (p == NULL) {
		    p = dir + strlen(dir)-1;
		}
		else {
		    p++;
		    *p='\0';
		}
#ifdef SZ_DEBUG
		printf("sz_file:3c: dir='%s'\n");
#endif
		strcat(dir, fb->ff_name);
#ifdef SZ_DEBUG
		printf("sz_file:3d: dir='%s'\n");
#endif
		dsz = sz_dir(dir);
#ifdef SZ_DEBUG
		print_size(fb->ff_name, dsz);
#endif
		add_sz_fsize(sz, dsz);
		free(dir);
		free(dsz);
	    }
	}
	else {
	    sz->fsize += fb->ff_fsize;
	    sz->fn++;
	}
	done = findnext(fb);
    }
    free(fb);
    return sz;
}

void print_size(char *patt, struct sz_fsize *sz)
{
    BYTE *p, s10[10], s10h[10];
    DWORD s, h;

    printf("\n\n%s:", patt);
    if ((sz_f.flags & FLAG_Z) == FLAG_Z) {
	printf("\n%10s in %d file(s)"
	       "\n%10s in %d hidden file(s)\n",
	       format_size(sz->fsize, s10), sz->fn,
	       format_size(sz->hsize, s10h), sz->hn);
    }
    else {
	if ((sz_f.flags & FLAG_K) == FLAG_K) {
	    h = sz->hsize / KILO;
	    s = sz->fsize / KILO;
	    p = "kilo";
	}
	else if ((sz_f.flags & FLAG_M) == FLAG_M) {
	    h = sz->hsize / MEGA;
	    s = sz->fsize / MEGA;
	    p = "mega";
	}
	else {
	    h = sz->hsize;
	    s = sz->fsize;
	    p = "";
	}
	printf("\n%11lu %sbytes in %d file(s)\n"
	       "%11lu %sbytes in %d hidden file(s)\n",
	       s, p, sz->fn, h, p, sz->hn);
    }
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

void do_sz(void)
{
    struct sz_fsize *tsz;
    struct sz_fsize *sz;
    BYTE i, c;

    tsz = mk_sz_fsize();

    for(i=0;i < sz_f.npatt;i++) {
	c = sz_f.patt[i][strlen(sz_f.patt[i])-1];
	if((c == '\\') || (c == ':')) {
	    sz = sz_dir(sz_f.patt[i]);
	} else {
	    sz = sz_file(sz_f.patt[i]);
	}
	add_sz_fsize(tsz, sz);
	print_size(sz_f.patt[i], sz);
	free(sz);
    }
    if (sz_f.npatt > 1) {
	print_size("Total", tsz);
    }
    free(tsz);
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
    if (sz_f.patt == NULL) {
	fprintf(stderr, "Out of memory! (malloc failed in init)\n");
	exit(-1);
    }

    sz_f.patt[0] = "*.*";

#ifdef SZ_DEBUG
    fprintf(stderr,"init:0: sz_f.patt[0]=%s; nargs=%d\n",sz_f.patt[0],nargs);
#endif

    sz_f.attrs = FA_NORMAL|FA_HIDDEN|FA_SYSTEM;
    sz_f.flags = 0;
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
		    fputs("\t-z:  show human readabale sizes (B, KB, MB, GB)\n",stderr);
		    fputs("\t-r:  recurse into sub-directories\n",stderr);
		    fputs("\t-h:  display this help and exit\n",stderr);
		    fputs("\t-?:  display usage and exit\n",stderr);
		    fputs("\nFILEPATTERN is file pattern like *.COM, *.? or *.*\n", stderr);
		    fputs("\nSZ is part of DOG (http://dog.sf.net/)\n", stderr);
		    free(sz_f.patt);
		    return 0;
		case 'k':
		    sz_f.flags |= FLAG_K;
		    break;
		case 'm':
		    sz_f.flags |= FLAG_M;
		    break;
		case 'r':
		    sz_f.flags |= FLAG_R;
		    sz_f.attrs |= FA_DIREC;
		    break;
		case 'z':
		    sz_f.flags |= FLAG_Z;
		    break;
		case '?':
		    printf("Usage: SZ [-h|-k|-m|-r|-z|-?] FILEPATTERN...\n");
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
    printf("init:3: sz_f.flags=%02xh\n",sz_f.flags);
    printf("init:3: sz_f.npatt=%u\n",sz_f.npatt);
#endif

    return 0;
}

int main(int nargs, char *argv[])
{
    int r;
    r = init(nargs,argv); /* parses argv and extracts flags and patterns */
    if (r == 0)
	do_sz();

    free(sz_f.patt);
    return r;
}
