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
2024-05-21 - Added -z flag to show human readable sizes.
2024-10-18 - Added -p flag to page at screen boundary and for each pattern group.
             Screen length can be found at 0040:0084, unless CGA, then use 25.
2024-10-20 - Added -c flag to use ANSI color.
             Also added code to parse LSCOLORS env variable color rules. -WB
2024-10-21 - Added -s flag to sort the ls output.
2024-10-26 - Added n sort option to sort directories before files. -WB
2024-10-27 - Added option to quit listing when -p is set by pressing 'q'. -WB
2024-10-27 - Added ANSI check when -c is given. -WB
****************************************************************************/

#include "ext.h"

void show_entry(struct ffblk *fb);
void show_wide_entry(struct ffblk *fb);
void show_long_entry(struct ffblk *fb);
char *format_size(QWORD sz);
int init(int nargs, char *arg[]);
void do_ls(void);
BYTE read_key(void);
void pause(BYTE force);

/* Color via ANSI */
#define ANSI_GET_POS "%c[6n"

#define COLOR_FG_BLACK   30
#define COLOR_FG_RED     31
#define COLOR_FG_GREEN   32
#define COLOR_FG_YELLOW  33
#define COLOR_FG_BLUE    34
#define COLOR_FG_MAGENTA 35
#define COLOR_FG_CYAN    36
#define COLOR_FG_WHITE   37

#define COLOR_BG_BLACK   40
#define COLOR_BG_RED     41
#define COLOR_BG_GREEN   42
#define COLOR_BG_YELLOW  43
#define COLOR_BG_BLUE    44
#define COLOR_BG_MAGENTA 45
#define COLOR_BG_CYAN    46
#define COLOR_BG_WHITE   47

#define COLOR_NO_STYLE   0
#define COLOR_BOLD       1
#define COLOR_UNDERLINE  4
#define COLOR_BLINK      5
#define COLOR_REVERSE    7
#define COLOR_CONCEALED  8

#define COLOR_ESC        0x1b
#ifdef LS_DEBUG_ESC
#undef COLOR_ESC
#define COLOR_ESC        'E'
#endif
#define COLOR BYTE*
#define COLOR_BS         0x08

#define COLOR_DEFAULT_RULE "_D=34;0;1,_H=30;40;8,_R=33;40,_S=31;40;1,_L=32;40,EXE=35;40,COM=35;40,DOG=35;40"

struct color_rule {
    BYTE ext[4];
    BYTE fg[3];
    BYTE bg[3];
    BYTE attr[2];
};

struct color_rules {
    BYTE len;
    struct color_rule *rules;
}color_rules = {0, NULL};

void color_set(char *cs, const char *s, COLOR fg, COLOR bg, COLOR attr);
char * color_match(struct ffblk *fb, const char *n);
BYTE color_build_rules(void);
BYTE test_ansi(void);
BYTE direct_console_input(void);

#define SORT_DIR_FIRST 'n'
#define SORT_SIZE 's'
#define SORT_ATTR 'a'
#define SORT_DATE 'd'
#define SORT_TIME 't'
#define SORT_FILE 'f'
#define SORT_EXT  'e'

struct sort_rule {
    BYTE field;
    char order;
};

struct sort_rules {
    BYTE len;
    struct sort_rule *rules;
}sort_rules = {0, NULL};

struct sort_entries {
    WORD capacity;
    WORD used;
    struct ffblk *entries;
};

char * attr_string(const struct ffblk *fb, char *s);
int sort_fb(const void *pa, const void *pb);
BYTE sort_build_rules(const BYTE *sort_string);
void save_entry(struct sort_entries *se, struct ffblk *fb);
void sort_entries(struct sort_entries *se);
void show_entries(struct sort_entries *se);
DWORD ftime_sec(const struct ffblk *fb);

/* Flags */
#define FLAG_D 0x01 /* 0000 0001 */
#define FLAG_W 0x02 /* 0000 0010 */
#define FLAG_Z 0x04 /* 0000 0100 */
#define FLAG_P 0x08 /* 0000 1000 */
#define FLAG_C 0x10 /* 0001 0000 */
#define FLAG_S 0x20 /* 0010 0000 */
#define FLAG_X 0x40 /* 0010 0000 */

/* If BYTE ay 0040:0084 is 0, assume a CGA screen with 25 lines */
#define MAX_Y_P ((BYTE far *)MK_FP(0x40, 0x84))
#define MAX_Y (*MAX_Y_P == 0 ? 25 : *MAX_Y_P)

#define IS_FLAG(FLAG) ((ls_f.flags & (FLAG)) == (FLAG))

#define GIGA 1073741824
#define MEGA 1048576
#define KILO 1024

struct size_stats
{
    QWORD total;
    WORD  files;
    WORD  dirs;
};

struct ts_flags
{
    WORD attrs;
    BYTE **patt;
    BYTE npatt;
    BYTE flags;
    BYTE w_entry;
    BYTE ln;
    char sz[10];
}ls_f;

int main(int nargs, char *argv[])
{
    int r;
    BYTE n;
    r = init(nargs,argv); /* parses argv and extracts flags and patterns */
    if (r == 0) {
	if(IS_FLAG(FLAG_C)) {
	    n = color_build_rules();
	    if(n != color_rules.len) {
		printf("ERROR parsing color rules, colors ignored\n");
	    }
	}
	do_ls();
	free(ls_f.patt);
	if(color_rules.rules != NULL) {
	    free(color_rules.rules);
	}
	if(sort_rules.rules != NULL) {
	    free(sort_rules.rules);
	}
    }
    return r;
}

/**
 * Returns BYTE of direct console input.
 *              0x0FF when the input buffer is empty.
 * Uses DOS INT 21h/AH=06,DL=FF
 */
BYTE direct_console_input(void)
{
    BYTE c;
    asm mov ah, 06h;    /* Direct Console I/O */
    asm mov dl, 0FFh;   /* 0x0ff means INPUT */
    asm int 21h;        /* Fetch 1st character */
    asm jnz ret_byte;
    return 0x0ff;       /* Input buffer empty */
ret_byte:
    asm mov c, al;
    return c;
}

/*
 * Checks for ANSI.SYS compatible driver and returns FLAG_C is successful, 0 otherwise
 * Code for this function is inspired by TESTANSI.ASM by Jerome Shidel:
 *   gitlab.com/DOSx86/donuts/-/blob/master/SOURCE/DONUTS/TestAnsi/TESTANSI.ASM
 */
BYTE test_ansi(void)
{
    BYTE c, al_c;

    printf("%c[6n", COLOR_ESC);
    c = direct_console_input();
    if ((c == 0x0ff) || (c != COLOR_ESC)) {
	goto test_cleanup;
    }
    c = direct_console_input();
    if ((c == 0x0ff) || (c != '[')) {
	goto test_cleanup;
    }
    do {
	c = direct_console_input();
	if(c == 'R') {
	    return FLAG_C;
	}
    } while(c != 0x0ff);
test_cleanup:
    printf("%c%c%c%c", COLOR_BS, COLOR_BS, COLOR_BS, COLOR_BS);
    printf("WARNING: ANSI driver not available, ignoring -c\n");
    return 0;
}

/*
 * Set an ANSI color
 */
void color_set(char *cs, const char *s, COLOR fg, COLOR bg, COLOR attr)
{
    if (*attr == '0') {
	if (*fg == '0') {
	    sprintf(cs, "%c[37;%.2sm%s%c[0m", COLOR_ESC, bg, s, COLOR_ESC);
	} else if (*bg == '0') {
	    sprintf(cs, "%c[%.2s;40m%s%c[0m", COLOR_ESC, fg, s, COLOR_ESC);
	} else {
	    sprintf(cs, "%c[%.2s;%.2sm%s%c[0m", COLOR_ESC, fg, bg, s, COLOR_ESC);
	}
    } else {
	if (*fg == '0') {
	    sprintf(cs, "%c[%.1s;37;%.2sm%s%c[0m", COLOR_ESC, attr, bg, s, COLOR_ESC);
	} else if (*bg == '0') {
	    sprintf(cs, "%c[%.1s;%.2s;40m%s%c[0m", COLOR_ESC, attr, fg, s, COLOR_ESC);
	} else {
	    sprintf(cs, "%c[%.1s;%.2s;%.2sm%s%c[0m", COLOR_ESC, attr, fg, bg, s, COLOR_ESC);
	}
    }
}

char * color_match(struct ffblk *fb, const char *n)
{
    char *ext=NULL;
    char *color_fn=NULL;
    BYTE i;
    BYTE set_color = 0;

    ext = strrchr(fb->ff_name, '.');
    if (ext) {
	ext++;
    }
    for (i=0;i<color_rules.len;i++) {
	if (ext != NULL && strnicmp(ext, color_rules.rules[i].ext, 3)==0) {
	    set_color = 1;
	} else if(((fb->ff_attrib & FA_DIREC) == FA_DIREC) &&
		  strcmp("_D", color_rules.rules[i].ext)==0) {
	    set_color = 1;
	} else if(((fb->ff_attrib & FA_ARCH) == FA_ARCH) &&
		  strcmp("_A", color_rules.rules[i].ext)==0) {
	    set_color = 1;
	} else if(((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN) &&
		  strcmp("_H", color_rules.rules[i].ext)==0) {
	    set_color = 1;
	} else if(((fb->ff_attrib & FA_RDONLY) == FA_RDONLY) &&
		  strcmp("_R", color_rules.rules[i].ext)==0) {
	    set_color = 1;
	} else if(((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM) &&
		  strcmp("_S", color_rules.rules[i].ext)==0) {
	    set_color = 1;
	} else if(((fb->ff_attrib & FA_LABEL) == FA_LABEL) &&
		  strcmp("_L", color_rules.rules[i].ext)==0) {
	    set_color = 1;
	}
	if (set_color) {
	    color_fn = malloc(strlen(n)+20);
	    color_set(color_fn, n,
		      color_rules.rules[i].fg,
		      color_rules.rules[i].bg,
		      color_rules.rules[i].attr);
	    return color_fn;
	}
    }
    return NULL;
}

BYTE color_build_rules(void)
{
    BYTE *rules, *p, *q, *s, n, i;

    rules = getenv("LSCOLORS");
    if (rules == NULL) {
	rules = COLOR_DEFAULT_RULE;
    }
    rules = strdup(rules);
    n = strlen(rules);
    /* count commas to find out how many rulese there are */
    /* rule,rule,rule */
    for(i=0; i<n; i++) {
	if(rules[i]==',') {
	    color_rules.len++;
	}
    }
    color_rules.len++; /* last rule is not followed by a , character */
    color_rules.rules = calloc(color_rules.len, sizeof(struct color_rule));
    if(color_rules.rules == NULL) {
	color_rules.len = 0;
	return 0;
    }
    p = strtok(rules, ",");
    for(i=0; p!=NULL; i++) {
#ifdef LS_DEBUG
	printf("parsing color rule: '%s'\n", p);
#endif
	q = strchr(p, '=');
	if (q==NULL) {
	    free(color_rules.rules);
	    color_rules.rules = NULL;
	    color_rules.len = 0;
	    printf("ERROR parsing color rule: '%s'\n", p);
	    return 0; /* bail out if rule values are not properly formed */
	}
	strncpy(color_rules.rules[i].ext, p, q-p);
	s = color_rules.rules[i].fg;
	while(isdigit(*(++q))){
	    *s = *q;
	    s++;
	}
	if (s == color_rules.rules[i].fg) {
	    s[0]='0';
	    s[1]='\0';
	}
	s = color_rules.rules[i].bg;
	while(isdigit(*(++q))){
	    *s = *q;
	    s++;
	}
	if (s == color_rules.rules[i].bg) {
	    s[0]='0';
	    s[1]='\0';
	}
	s = color_rules.rules[i].attr;
	while(isdigit(*(++q))){
	    *s = *q;
	    s++;
	}
	if (s == color_rules.rules[i].attr) {
	    s[0]='0';
	    s[1]='\0';
	}
#ifdef LS_DEBUG
	printf("color rule[%d]:ext=%.3s fg=%.2s bg=%.2s attr=%.1s\n",
	       i,
	       color_rules.rules[i].ext,
	       color_rules.rules[i].fg,
	       color_rules.rules[i].bg,
	       color_rules.rules[i].attr
	    );
#endif
	/* get the next token */
	p = strtok(NULL, ",");
    }
    free(rules);
    return i; /* number of successfully parsed rules */
}


BYTE sort_build_rules(const BYTE *rules)
{
    BYTE n, i;
    const BYTE *p;

    if ((rules == NULL) || ((n = strlen(rules)/2)==0)) {
	rules = "+n+f";
	n = 2;
    }
#ifdef LS_DEBUG
    printf("parsing sort string: '%s'\n", rules);
#endif
    sort_rules.len = n;
    sort_rules.rules = calloc(sort_rules.len, sizeof(struct sort_rule));
    if(sort_rules.rules == NULL) {
	sort_rules.len = 0;
	return 0;
    }
    for(i=0; i<n; i++) {
	p=&rules[i*2];
#ifdef LS_DEBUG
	printf("parsing sort rule: '%.2s'\n", p);
#endif
	if (*p=='+') {
	    sort_rules.rules[i].order = 1;
	} else if (*p=='-') {
	    sort_rules.rules[i].order = -1;
	} else {
	    sort_rules.rules[i].order = 0; /* basically don't sort */
	}
	sort_rules.rules[i].field = rules[i*2+1];

#ifdef LS_DEBUG
	printf("sort rule[%d]:field=%c order=%d\n",
	       i,
	       sort_rules.rules[i].field,
	       sort_rules.rules[i].order);
#endif
    }

    return i; /* number of successfully parsed rules */
}

char * attr_string(const struct ffblk *fb, char *s)
{
    if ((fb->ff_attrib & FA_ARCH) == FA_ARCH) {
	s[0] = 'A';
    } else {
	s[0] = '_';
    }
    if ((fb->ff_attrib & FA_DIREC) == FA_DIREC) {
	s[1] = 'D';
    } else {
	s[1] = '_';
    }
    if ((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN) {
	s[2] = 'H';
    } else {
	s[2] = '_';
    }
    if ((fb->ff_attrib & FA_LABEL) == FA_LABEL) {
	s[3] = 'L';
    } else {
	s[3] = '_';
    }
    if ((fb->ff_attrib & FA_RDONLY) == FA_RDONLY) {
	s[4] = 'R';
    } else {
	s[4] = '_';
    }
    if ((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM) {
	s[5] = 'S';
    } else {
	s[5] = '_';
    }
    s[6] = '\0';
    return s;
}

DWORD ftime_sec(const struct ffblk *fb)
{
    DWORD h, min, sec;
    h = (fb->ff_ftime & 0x0F800)>>11;
    min = (fb->ff_ftime & 0x03E0)>>5;
    sec = (fb->ff_ftime & 0x001f)<<1; /* *2 */
    return h*3600+min*60+sec;
}

int sort_fb(const void *pa, const void *pb)
{
    BYTE i;
    int l;
    long int ll;
    const struct ffblk *a, *b;
    BYTE aattrs[7], battrs[7], *aext, *bext;

    a = (const struct ffblk *)pa;
    b = (const struct ffblk *)pb;

    for(i=0; i < sort_rules.len; i++) {
	switch(sort_rules.rules[i].field) {
	case SORT_DIR_FIRST:
	    if ((a->ff_attrib & FA_DIREC) == FA_DIREC) {
		if ((b->ff_attrib & FA_DIREC) == FA_DIREC) {
		    continue;
		}
		return sort_rules.rules[i].order * -1;
	    } else {
		if((b->ff_attrib & FA_DIREC) == FA_DIREC) {
		    return sort_rules.rules[i].order * 1;
		} else {
		    continue;
		}
	    }
	case SORT_SIZE:
	    if (a->ff_fsize > b->ff_fsize) {
		l = 1;
	    } else if (a->ff_fsize == b->ff_fsize) {
		continue;
	    } else {
		l = -1;
	    }
	    return sort_rules.rules[i].order * l;
	case SORT_ATTR:
	    l = strcmp(attr_string(a, aattrs), attr_string(b, battrs));
	    if (l==0) {
		continue;
	    }
	    return sort_rules.rules[i].order * l;
	case SORT_DATE:
	    l = a->ff_fdate - b->ff_fdate;
	    if (l==0) {
		continue;
	    }
	    return sort_rules.rules[i].order * l;
	case SORT_TIME:
	    if (ftime_sec(a) > ftime_sec(b)) {
		l = 1;
	    } else if (ftime_sec(a) == ftime_sec(b)) {
		continue;
	    } else {
	        l = -1;
	    }
	    return sort_rules.rules[i].order * l;
	case SORT_FILE:
	    l = strcmp(a->ff_name, b->ff_name);
	    if (l==0) {
		continue;
	    }
	    return sort_rules.rules[i].order * l;
	case SORT_EXT:
	    aext = strrchr(a->ff_name, '.');
	    bext = strrchr(b->ff_name, '.');

	    if (a==b) { /* both are null */
		continue;
	    } else if (bext == NULL) { /* both can't be null here anymore*/
		l = 1;
	    } else if (aext == NULL) {
		l = -1;
	    } else {
		l = strcmp(aext, bext);
	    }
	    if (l==0) {
		continue;
	    }
	    return sort_rules.rules[i].order * l;
	default:
	    continue;
	}
    }
    return 0; /* can't sort */
}

void save_entry(struct sort_entries *se, struct ffblk *fb)
{
    struct ffblk *e, *ne;
    BYTE l;
    WORD i;
    if (se->used == se->capacity) {
	se->capacity += 100;
	ne = realloc(se->entries, se->capacity * sizeof(struct ffblk ));
	if(ne == NULL) {
	    return;
	}
	se->entries = ne;
    }
    memcpy(&se->entries[se->used], fb, sizeof(struct ffblk));
    se->used++;
}

void sort_entries(struct sort_entries *se)
{
    qsort(se->entries, se->used, sizeof(struct ffblk ), sort_fb);
    return;
}

void show_entries(struct sort_entries *se)
{
    WORD i;
    sort_entries(se);

    for(i=0; i < se->used; i++) {
	show_entry(&se->entries[i]);
	ls_f.ln++;
	pause(0);
    }
}

/*
 * Print a dir entry.
 */
void show_entry(struct ffblk *fb)
{
    if (IS_FLAG(FLAG_W)) {
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
    BYTE *fn;
    BYTE is_colored=0;
    BYTE width=13;

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

    if(IS_FLAG(FLAG_C)) {
	fn = color_match(fb, n);
	if (fn == NULL) {
	    fn = n;
	} else {
	    is_colored=1;
	}
    } else {
	fn = n;
    }

    if (ls_f.w_entry == 4) {
	printf("%s\n", fn);

	ls_f.w_entry = 0;
	ls_f.ln++;
    }
    else {
	if(IS_FLAG(FLAG_C) && is_colored) {
	    width = 13-strlen(n)+strlen(fn);
	}
	printf("%-*s   ", width, fn);
	ls_f.w_entry++;
    }
    if(IS_FLAG(FLAG_C) && is_colored) {
	free(fn);
    }

    return;
}

/*
 * Returns a char string representing the size in human readable form:
 * e.g.
 *  - >1024 ==> 1kB
 *  - >1048576 ==> 1MB
 *  - etc.
 */
char * format_size(QWORD sz)
{
    unsigned long int f;
    char *s = (char *)ls_f.sz;

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

/*
 * Print the size, name, etc of a dir entry (long format).
 */
void show_long_entry(struct ffblk *fb)
{
    WORD h, min, sec, y, m, d;
    BYTE *fn;
    BYTE is_colored=0;
#ifdef LS_DEBUG_2
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

    if (IS_FLAG(FLAG_Z)) {
	printf("%10s   ", format_size(fb->ff_fsize));
    }
    else {
	printf("%10lu   ",fb->ff_fsize);
    }

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

    if(IS_FLAG(FLAG_C)) {
	fn = color_match(fb, fb->ff_name);
	if (fn == NULL) {
	    fn = fb->ff_name;
	} else {
	    is_colored=1;
	}
    } else {
	fn = fb->ff_name;
    }
    printf("%s\n",fn);
    ls_f.ln++;
    if(IS_FLAG(FLAG_C) && is_colored) {
	free(fn);
    }

    return;
}

BYTE read_key(void)
{
    BYTE c;
    asm mov ah, 08h; /* read key without echo */
    asm int 21h;
    asm mov c,al;

    if(c >= 'a' && c <= 'z') {
	c -= ('a' - 'A'); /* uppercase the character */
    }
    return c;
}

void pause(BYTE force)
{
    BYTE k;
    if (IS_FLAG(FLAG_P) && ((ls_f.ln >= (MAX_Y-1)) || force)) {
#ifdef LS_DEBUG
	printf("ls_f.ln=%d/%d\n", ls_f.ln, MAX_Y);
#endif
	printf("<<<Press any key to continue listing or 'q' to quit>>>\r");
	k = read_key();
	printf("                                                      \r");
	if (k=='Q') {
	    exit(-1);
	}
	ls_f.ln = 0;
    }
}

/*
 * Return -1 for error 0 for OK 1 for OK, but stop (e.g. help)
 */
int init(int nargs, char *arg[])
{
    BYTE t,i,j,k,n,*p;

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
		case 'c':
		    ls_f.flags += test_ansi();
		    break;
		case 'd':
		    ls_f.flags += FLAG_D;
		    break;
		case 'f':
		    ls_f.attrs = FA_NORMAL;
		    break;
		case 'h':
		    printf("The switches are:\n\t-a: List ALL files and directories\n"
			   "\t-d: list DIRECTORIES only\n"
			   "\t-f: list normal FILES only\n"
			   "\t-l: show volume label\n"
			   "\t-p: pause at every screen full or pattern\n"
			   "\t-s: sort output\n"
			   "\t-w: show file names only (6 per row)\n"
			   "\t-z: show human readable sizes (B, KB, MB, GB)\n");
		    free(ls_f.patt);
		    return 1;
		case 'l':
		    ls_f.attrs += FA_LABEL;
		    break;
		case 'p':
		    ls_f.flags += FLAG_P;
		    break;
		case 's':
		    ls_f.flags += FLAG_S;
		    n = sort_build_rules(&arg[i][2]);
		    if(n != sort_rules.len) {
			printf("ERROR parsing sort rules, sorting ignored\n");
			sort_rules.len = 0;
		    }
		    break;
		case 'w':
		    ls_f.flags += FLAG_W;
		    break;
		case 'x':
		    ls_f.flags += FLAG_X;
		    break;
		case 'z':
		    ls_f.flags += FLAG_Z;
		    break;
		case '?':
		    printf("ls [-a|-d|-f|-h|-l|-w|-x|-z|-?] [d:\\dir\\filename.ext]\n");
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
    struct sort_entries entries={0, 0, NULL};
    WORD len;
    signed char r;
    struct ffblk fb;
    struct size_stats sz={0, 0, 0};
    m = 0;
    ls_f.ln = 0;

    for(j=0;j < ls_f.npatt;j++) {
	if (IS_FLAG(FLAG_X)) {
	    sz.total = 0;
	    sz.files = 0;
	    sz.dirs = 0;
	}
	if (IS_FLAG(FLAG_S)) {
#ifdef LS_DEBUG
	    printf("CAPACITY USED is %d/%d\n", entries.used, entries.capacity);
#endif
	}
	ls_f.w_entry = 0;
	if(j > 0) {
	    if((IS_FLAG(FLAG_W)) && (ls_f.w_entry > 0)) {
		printf("\n\n");
		ls_f.ln += 2;
	    }
	    else {
		printf("\n");
		ls_f.ln++;
	    }
	    pause(1);
	}
	printf("Files matching %s\n\n", strupr(ls_f.patt[j]));
	ls_f.ln += 2;
	if ((ls_f.flags & FLAG_W) == 0) {
	    printf("      size   ASRHD   yyyy-mm-dd   hh:mm.ss   name\n\n");
	    ls_f.ln += 2;
	}
	len = strlen(ls_f.patt[j]);
#ifdef LS_DEBUG
	fprintf(stderr,"do_ls:1: ls_f.patt[%u]='%s'(%ph) ls_f.patt[%d][%d]='%c'\n",j,ls_f.patt[j],ls_f.patt[j],j,len-1,ls_f.patt[j][len-1]);
#endif
	if((ls_f.patt[j][len-1] == '\\') || (ls_f.patt[j][len-1] == ':')) {
	    p = malloc(sizeof(BYTE)*(len + 4));
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
		if((IS_FLAG(FLAG_D)) && ((fb.ff_attrib & FA_DIREC) != FA_DIREC)) {
#ifdef LS_DEBUG
		    fprintf(stderr,"do_ls:6a: r=%d\n found dir (but not showing): %s",r,fb.ff_name);
#endif
		    ; /* if user specified -d flag, but entry isn't a directory, don't display */
		}
		else {
#ifdef LS_DEBUG
		    fprintf(stderr,"do_ls:6b: r=%d\n",r);
#endif
		    if (IS_FLAG(FLAG_S)) {
			save_entry(&entries, &fb);
		    } else {
			show_entry(&fb);
		    }
		    if (IS_FLAG(FLAG_X)) {
			if ((fb.ff_attrib & FA_DIREC) & FA_DIREC) {
			    sz.dirs++;
			} else {
			    sz.files++;
			    sz.total += (QWORD)fb.ff_fsize;
			}
		    }

		}
	    }
	    else {
#ifdef LS_DEBUG
		fprintf(stderr,"do_ls:6c: r=%d\n",r);
#endif
		break;
	    }
	    r = findnext (&fb);
	    pause(0);
	}

#ifdef LS_DEBUG
	fprintf(stderr,"do_ls:7: r=%d\n",r);
#endif
	if (IS_FLAG(FLAG_S)) {
	    show_entries(&entries);
	    entries.used=0;
	}
	if (IS_FLAG(FLAG_X)) {
	    if(IS_FLAG(FLAG_W)) {
		if (ls_f.w_entry > 0) {
		    printf("\n\n");
		}
		else {
		    printf("\n");
		}
		if (IS_FLAG(FLAG_Z)) {
		    printf("%s in ", format_size(sz.total));
		}
		else {
		    printf("%lu bytes in ", sz.total);
		}
	    }
	    else {
		printf("%-45s==========\n", "==========");
		if (IS_FLAG(FLAG_Z)) {
		    printf("%10s%35s", format_size(sz.total)," ");
		}
		else {
		    printf("%10lu%-35s", sz.total, " bytes");
		}
	    }
	    printf("%d file(s) %d dir(s)\n", sz.files, sz.dirs);
	}

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
