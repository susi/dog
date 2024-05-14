/*

LS.C - DOG - Alternate command processor for freeDOS

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
2002-02-21 - Changed the date to be in yyy-mm-dd format
2024-05-13 - Fixed bug with freeing inappropriate memory.
2024-05-14 - Added -w flag, and each pattern shows in a separate block as if
             chaining several commands (it's how it's implemented anyway)
****************************************************************************/

#include "ext.h"

void show_entry(struct ffblk *fb);
void show_wide_entry(struct ffblk *fb);
void show_long_entry(struct ffblk *fb);
int init(int nargs, char *arg[]);
void do_ls(void);

#define FLAG_D 0x01 /* 0000 0001 */
#define FLAG_W 0x02 /* 0000 0010 */

struct ts_flags
{
    WORD attrs;
    BYTE **patt;
    BYTE npatt;
    BYTE flags;
    BYTE w_entry;
}ls_f;


int main(int nargs, char *argv[])
{
    int r;
    r = init(nargs,argv); /* parses argv and extracts flags and patterns */
    if (r == 0) {
	do_ls();
	free(ls_f.patt);
    }

    return r;
}

/*
 * Print a dir entry.
 */
void show_entry(struct ffblk *fb)
{
    if ((ls_f.flags & FLAG_W) == FLAG_W) {
	show_wide_entry(fb);
    }
    else {
	show_long_entry(fb);
    }
}

/*
 * Print the name only (wide format).
 */
void show_wide_entry(struct ffblk *fb)
{
    BYTE n[14] = {"             "};

    if ((fb->ff_attrib & FA_DIREC) == FA_DIREC) {
	sprintf(n, "%s\\", fb->ff_name);
    }
    else if ((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN) {
	sprintf(n, "%s.", fb->ff_name);
    }
    else if ((fb->ff_attrib & FA_RDONLY) == FA_RDONLY) {
	sprintf(n, "%s=", fb->ff_name);
    }
    else if ((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM) {
	sprintf(n, "%s!", fb->ff_name);
    }
    else if ((fb->ff_attrib & FA_LABEL) == FA_LABEL) {
	sprintf(n, "%s:", fb->ff_name);
    }
    else {
	sprintf(n, "%s", fb->ff_name);
    }

    if (ls_f.w_entry == 4) {
	printf("%s\n", n);
	ls_f.w_entry = 0;
    }
    else {
	printf("%-13s   ", n);
	ls_f.w_entry++;
    }

    return;
}

/*
 * Print the size, name, etc of a dir entry (long format).
 */
void show_long_entry(struct ffblk *fb)
{
    WORD h, min, sec, y, m, d;
#ifdef LS_DEBUG
    BYTE l;
    fprintf(stderr,"s_e: fb->reserved:");
    for(l=0;l<22;l++) {
	fprintf(stderr," %02X", fb->ff_reserved[l]);
    }
    fprintf(stderr,"h\n");
    fprintf(stderr,"s_e: fb->reserved[0h] (drive):%02Xh\n", fb->ff_reserved[0]);
    fprintf(stderr,"s_e: fb->reserved[1h] (search):'%11s'\n", fb->ff_reserved[1]);
    fprintf(stderr,"s_e: fb->reserved[Ch] (s_attrib):%02Xh\n", fb->ff_reserved[0x0c]);
    fprintf(stderr,"s_e: fb->reserved[Dh] (entry):%02X%02Xh\n", fb->ff_reserved[0x0e], fb->ff_reserved[0x0d]);
    fprintf(stderr,"s_e: fb->reserved[Fh] (cluster):%02X%02Xh\n", fb->ff_reserved[0x10], fb->ff_reserved[0x0f]);

    fprintf(stderr,"s_e: fb->attrib:%02Xh\n", fb->ff_attrib);
    fprintf(stderr,"s_e: fb->date:%04Xh\n", fb->ff_fdate);
    fprintf(stderr,"s_e: fb->time:%04Xh\n", fb->ff_ftime);
    fprintf(stderr,"s_e: fb->fsize:%08lXh\n", fb->ff_fsize);
    fprintf(stderr,"s_e: fb->name:'%s'\n", fb->ff_name);
#endif

    printf("%8ld   ",fb->ff_fsize);

    if (((fb->ff_attrib & FA_ARCH) == FA_ARCH) && ((fb->ff_attrib & FA_LABEL) != FA_LABEL)) {
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

    printf("   %04d-%02d-%02d   ",y,m,d);

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
    BYTE t,i,j,k,*p;

    ls_f.w_entry = 0;
    ls_f.npatt = 1;
    ls_f.patt = (BYTE **) malloc(nargs * (sizeof(BYTE *)));

    ls_f.patt[0] = "*.*";

#ifdef LS_DEBUG
    fprintf(stderr,"init:1: ls_f.patt=%ph; nargs=%d\n",ls_f.patt,nargs);
#endif

    ls_f.attrs = 0|FA_DIREC;
    k = 0;

    if (nargs > 1) {
	for(i=1;i<nargs;i++) {

#ifdef LS_DEBUG
	    fprintf(stderr,"init:2: ls_f.patt=%ph, k=%d\n",ls_f.patt, k);
#endif
	    if (arg[i][0] == '-') {
		switch(arg[i][1]) {
		case 'a':
		    ls_f.attrs = FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN|FA_DIREC|FA_LABEL;
		    break;
		case 'd':
		    ls_f.flags += FLAG_D;
		    break;
		case 'f':
		    ls_f.attrs = FA_NORMAL;
		    break;
		case 'h':
		    printf("The switches are:\n\t-a: List ALL files and directories\n");
		    printf("\t-d: list DIRECTORIES only\n"
			   "\t-f: list normal FILES only\n"
			   "\t-l: show volume label\n"
			   "\t-w: show file names only (6 per row)\n");
		    free(ls_f.patt);
		    return 1;
		case 'l':
		    ls_f.attrs += FA_LABEL;
		    break;
		case 'w':
		    ls_f.flags += FLAG_W;
		    break;
		case '?':
		    printf("ls [-a|-d|-f|-h|-l|-w|-?] [d:\\dir\\filename.ext]\n");
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
		/* add pattern to the list of patterns */
		ls_f.patt[k] = arg[i];
		k++;
#ifdef LS_DEBUG
		printf("init:3: ls_f.patt[%d]='%s' (%ph) ls_f.patt=%ph k=%d\n",k-1,ls_f.patt[k-1],ls_f.patt[k-1],ls_f.patt, k);
#endif
	    }
	}
    }

    if (k>0) {
	ls_f.npatt = k;
    }
#ifdef LS_DEBUG
    printf("init:4: ls_f.patt=%ph ls_f.npatt=%d\n",ls_f.patt, ls_f.npatt);
    for (i=0;i<ls_f.npatt;i++) {
	printf("init:4: ls_f.patt[%d]='%s' (%ph)\n",i,ls_f.patt[i],ls_f.patt);
    }
#endif
    return 0;
}


/*
 * The main ls loop. searches for files untill no more files with patterrns
 * pointed by ls_f.patt are found.
 */
void do_ls(void)
{
    BYTE *p, j, m, k;
    WORD len;
    signed char r;
    struct ffblk fb;
    m = 0;


    for(j=0;j < ls_f.npatt;j++) {
	if(j > 0) {
	    if(((ls_f.flags & FLAG_W) == FLAG_W) && (ls_f.w_entry > 0)) printf("\n\n");
	    else printf("\n");
	}
	printf("Files matching %s\n\n", strupr(ls_f.patt[j]));
	if ((ls_f.flags & FLAG_W) == 0) {
	    printf("    size   ASRHD   yyyy-mm-dd   hh:mm.ss   name\n\n");
	}
	len = strlen(ls_f.patt[j]);
#ifdef LS_DEBUG
	fprintf(stderr,"do_ls:1: ls_f.patt[%u]='%s'(%ph) ls_f.patt[%d][%d]='%c'\n",j,ls_f.patt[j],ls_f.patt[j],j,len-1,ls_f.patt[j][len-1]);
#endif
	if((ls_f.patt[j][len-1] == '\\') || (ls_f.patt[j][len-1] == ':')) {
	    p = malloc(sizeof(BYTE)*(len + 3));
	    if(p == NULL) {
#ifdef LS_DEBUG
		fprintf(stderr,"do_ls:2a: malloc failed!\n");
#endif
	    }
	    else {
#ifdef LS_DEBUG
		fprintf(stderr,"do_ls:2b: ls_f.patt[%u]=(%ph) p=%ph\n", j, ls_f.patt[j], p);
#endif
		strcpy(p,ls_f.patt[j]);
		strcat(p,"*.*");
		ls_f.patt[j] = p;
		m = 1;
	    }
#ifdef LS_DEBUG
	    fprintf(stderr,"do_ls:3: ls_f.patt[%u]='%s'(%ph) p=%ph\n", j, ls_f.patt[j], ls_f.patt[j], p);
#endif
	}

	r = findfirst(ls_f.patt[j], &fb,ls_f.attrs);

#ifdef LS_DEBUG
	fprintf(stderr,"do_ls:4: r=%d\n",r);
#endif

	while(1) {
#ifdef LS_DEBUG
	    fprintf(stderr,"do_ls:5: r=%d\n",r);
#endif
	    if(r == 0) { /* TODO: check for possible errors */
		if(((ls_f.flags & FLAG_D) == FLAG_D) && ((fb.ff_attrib & FA_DIREC) != FA_DIREC)) {
#ifdef LS_DEBUG
		    fprintf(stderr,"do_ls:6a: r=%d\n found dir (but not showing): %s",r,fb.ff_name);
#endif
		    ; /* if user specified -d flag, but entry isn't a directory, don't display */
		}
		else {
#ifdef LS_DEBUG
		    fprintf(stderr,"do_ls:6b: r=%d\n",r);
#endif
		    show_entry(&fb);
		}
	    }
	    else {
#ifdef LS_DEBUG
		fprintf(stderr,"do_ls:6c: r=%d\n",r);
#endif
		break;
	    }
	    r = findnext (&fb);
	}

#ifdef LS_DEBUG
	fprintf(stderr,"do_ls:7: r=%d\n",r);
#endif
	if(m == 1) {
#ifdef LS_DEBUG
	    fprintf(stderr,"do_ls:8: free malloc:ed patt[%d]: '%s'(%ph)\n",j,ls_f.patt[j],ls_f.patt[j]);
#endif
	    free(ls_f.patt[j]); /* if patt[j] was previously malloced */
	    ls_f.patt[j] = NULL;
	}
    }

    return;
}
