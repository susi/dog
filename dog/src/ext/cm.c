/*

CM.C - DOG - Alternate command processor for freeDOS

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
2024-11-06 - Branched from rm.c as the program logic is very similar
2024-11-11 - Added -a flag to act on read-only, hidden and system files.
**************************************************************************/
#include "ext.h"

#include <conio.h>

BYTE flag_i = FLAG_UNSET;
BYTE flag_r = FLAG_UNSET;
BYTE flag_v = FLAG_UNSET;
BYTE flag_h = FLAG_UNSET;
BYTE flag_a = FLAG_UNSET;

BYTE search_attribs = 0;

struct cm_attribs{
    BYTE str[6];
    WORD attr;
    BYTE op; /* +,- or = */
};

BYTE cm_dir(BYTE *dir, struct cm_attribs *attrs);
BYTE cm_file(BYTE *patt, struct cm_attribs *attrs);
BYTE cm_list(BYTE *list, struct cm_attribs *attrs);
WORD cm_chmod(BYTE *fn, struct ffblk *fb, struct cm_attribs *attrs);
BYTE *cm_attr_to_string(BYTE attr, BYTE *s);
BYTE cm_file_process(BYTE *fn, struct ffblk *fb, struct cm_attribs *attrs);
void print_help(void);

struct cm_attribs* cm_parse(BYTE *str)
{
    BYTE *p, i, s[6];
    struct cm_attribs *att = NULL;

    if (str == NULL) {
	printf("Missing ATTRIBUTES\n");
	return NULL;
    }
    att = malloc(sizeof(struct cm_attribs));
    if (att == NULL) {
	return NULL;
    }
    for(i=0; i < 5; i++) att->str[i]='-';
    att->str[5] = '\0';
    att->op = str[0];
    if ((att->op != '=') && (att->op != '+') && (att->op != '-')) {
	puts("Invalid ATTRIBUTES operation");
	free(att);
	return NULL;
    }
    p = &str[1];
    att->attr=0;
    do {
	switch(*p) {
	case 'A':
	    att->attr |= FA_ARCH;
	    att->str[0] = 'A';
	    break;
	case 'S':
	    att->attr |= FA_SYSTEM;
	    att->str[1] = 'S';
	    break;
	case 'H':
	    att->attr |= FA_HIDDEN;
	    att->str[2] = 'H';
	    break;
	case 'R':
	    att->attr |= FA_RDONLY;
	    att->str[3] = 'R';
	    break;
	default:
	    printf("Invalid ATTRIBUTE '%c'\n", *p);
	    free(att);
	    return NULL;
	}
    } while(*(++p) != '\0');
#ifdef CM_DEBUG
    printf("PARSE: '%s' -> '%s'\n", str, cm_attr_to_string(att->attr, s));
#endif
    return att;
}

BYTE *cm_attr_to_string(BYTE attr, BYTE *s)
{
    if ((attr & FA_ARCH) == FA_ARCH) {
	s[0] = 'A';
    } else {
	s[0] = '-';
    }
    if ((attr & FA_SYSTEM) == FA_SYSTEM) {
	s[1] = 'S';
    } else {
	s[1] = '-';
    }
    if ((attr & FA_HIDDEN) == FA_HIDDEN) {
	s[2] = 'H';
    } else {
	s[2] = '-';
    }
    if ((attr & FA_RDONLY) == FA_RDONLY) {
	s[3] = 'R';
    } else {
	s[3] = '-';
    }
    if ((attr & FA_DIREC) == FA_DIREC) {
	s[4] = 'D';
    } else {
	s[4] = '-';
    }
    s[5] = '\0';
#ifdef CM_DEBUG
    printf("s='%s'\n", s);
#endif
    return s;
}

#pragma option -O
WORD setmod(BYTE far *fn, WORD attr)
{
    WORD rv;
    asm lds dx, fn;
    asm mov ax, 4301h; /* CHMOD */
    asm mov cx, attr;
    asm int 21h;
    asm jc setfail;
    return 0;
setfail:
    asm mov rv, ax;
    return rv;
}

WORD getmod(BYTE far *fn)
{
    WORD rv;
    asm lds dx, fn;
    asm mov ax, 4300h; /* Get File Attribs */
    asm int 21h;
    asm jc getfail;
    asm mov rv, cx;
    return rv;
getfail:
    asm mov rv, ax;
    return 0x0ff40 & rv;
}

WORD cm_chmod(BYTE *fn, struct ffblk *fb, struct cm_attribs *attrs)
{
    int r;
    WORD cm;
    BYTE doit = 0, old[6], new[6], da;

    /* in case fb is a directory display it as such */
    da = fb->ff_attrib & FA_DIREC;
    cm_attr_to_string(fb->ff_attrib, old);
    switch(attrs->op) {
    case '=':
	cm_attr_to_string(attrs->attr | da, new);
	break;
    case '+':
	cm_attr_to_string(fb->ff_attrib | attrs->attr, new);
	break;
    case '-':
	cm_attr_to_string(fb->ff_attrib & ~attrs->attr, new);
	break;
    }

    if(flag_i == FLAG_SET) {
	printf("Change attributes for '%s' from %s to %s (Y/N/A)? ", fn, old, new);

	r = getch();
	if (r==3) /* C-c */
	    return -1;
	else
	    printf("%c\n",r);

	if((r=='A') || (r=='a')) {
	    r = 'y';
	    flag_i = FLAG_UNSET;
	}
#ifdef CM_DEBUG
	printf("cm_chmod (%d): r=(%c)[%x],attr=0x04x\n",__LINE__,r, r, r, attrs->attr);
#endif
	if((r == 'y') || (r == 'Y')) {
	    doit = 1;
	}
    }
    else {
	doit = 1;
    }
    if (!doit) {
	return 0xffff;
    }

    switch(attrs->op) {
    case '=':
	cm = setmod((BYTE far *)fn, attrs->attr);
	break;
    case '+':
	cm = setmod((BYTE far *)fn, (fb->ff_attrib | attrs->attr) & ~FA_DIREC);
	break;
    case '-':
	cm = setmod((BYTE far *)fn, (fb->ff_attrib & ~attrs->attr) & ~FA_DIREC);
	break;
    }
    switch (cm) {
    case 0:
	if(flag_v == FLAG_SET) {
	    printf("attributes of '%s' changed from %s to %s\n", fn, old, new);
	}
	return 0;
    case 2:
    case 3:
	printf("File not found '%s'\n", fn);
	return cm;
    case 5:
	printf("Access denied '%s'\n", fn);
	return cm;
    default:
	printf("Unknown Error %d\n", cm);
	return cm;
    }
}


BYTE cm_dir(BYTE *dir, struct cm_attribs *attrs)
{
  BYTE path[80];
  strcpy(path,dir);
  strcat(path,"\\*.*");
#ifdef CM_DEBUG
  printf("cm_dir (%d): path=(%s)[%x] dir=(%s)[%x]\n",__LINE__,path,path,dir,dir);
#endif
  return cm_file((BYTE *)path, attrs);
}

BYTE *cm_mkfn(BYTE *patt, struct ffblk *fb, BYTE *fn)
{
  BYTE *p;

  p = patt;
  p+=strlen(patt) - 1;
  if((*p == '\\') || (*p == '/')) {
      *p = '\0';
      p--;
  }
  for(;(*p!='\\')&&(*p!='/');p--);
  if(p>patt) {
      *(++p)=0;
      strcpy(fn, patt);
      strcat(fn, fb->ff_name);
  }
  else {
      strcpy(fn, fb->ff_name);
  }
#ifdef CM_DEBUG
  printf("cm_mkfn (%d): fn=(%s)[%x]\n", __LINE__, fn, fn);
#endif
  return fn;
}

BYTE cm_file_process(BYTE *fn, struct ffblk *fb, struct cm_attribs *attrs)
{
  BYTE r,f,i,ok=0;

  if((fb->ff_attrib & FA_DIREC) == FA_DIREC) {
      if(flag_r == FLAG_SET) {
	  ok = cm_dir(fn, attrs);
#ifdef CM_DEBUG
	  printf("cm_file_process (%d): fb->ff_name=(%s)[%x],ok=%x\n",
		 __LINE__, fb->ff_name, fb->ff_name,ok);
#endif
      }
  }
  ok = cm_chmod(fn, fb, attrs);
#ifdef CM_DEBUG
  printf("cm_file_process (%d): fb->ff_name=(%s)[%x], ok=%x\n",
	 __LINE__, fb->ff_name, fb->ff_name,ok);
#endif

  return ok;
}

BYTE cm_file(BYTE *patt, struct cm_attribs *attrs)
{
  BYTE r,f,i,ok=0,ret=0;
  BYTE *p;
  BYTE fn[129]={0};
  struct ffblk fb;

#ifdef CM_DEBUG
  printf("cm_file (%d): patt=(%s)[%x]\n",__LINE__,patt,patt);
#endif

  f=findfirst(patt,&fb,search_attribs|FA_DIREC);
#ifdef CM_DEBUG
  printf("cm_file (%d): patt=(%s)[%x] f=%x\n",__LINE__,patt,patt,f);
#endif

  if(f==0) {
      /* ignore . and .. */
      if(fb.ff_name[0] != '.') {
	  cm_mkfn(patt, &fb, fn);
	  ok = cm_file_process(fn, &fb, attrs);
#ifdef CM_DEBUG
	  printf("cm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",
		 __LINE__,fb.ff_name,fb.ff_name,ok);
#endif
	  if (ok != 0) {
	      ret = 1;
	  }
      }
  }
  else if((f==255) && (errno==2)) {
      ret = 1;
      printf("%s - no such file or directory.\n", patt);
      return ret;
  }

#ifdef CM_DEBUG
  printf("cm_file (%d): f=(%d)[%x],ok=%x\n",__LINE__,f,f,fb.ff_name,ok);
#endif

  while((f=findnext(&fb))==0) {
#ifdef CM_DEBUG
      printf("cm_file (%d): f=(%d)[%x],ok=%d\n",__LINE__,f,f,fb.ff_name,ok);
      printf("cm_file (%d): fb.ff_name=(%s)[%x],ok=%x\n",__LINE__,fb.ff_name,fb.ff_name,ok);
#endif

      if(fb.ff_name[0] == '.')
	  continue; /* ignore . and .. */
      cm_mkfn(patt, &fb, fn);
      ok = cm_file_process(fn, &fb, attrs);
      if (ok != 0) {
	  ret = 1;
      }
  }
  return ret;
}

BYTE cm_list(BYTE *list, struct cm_attribs *attrs)
{
  BYTE fn[129]={0};
  FILE *f;
  BYTE ret=0;

#ifdef CM_DEBUG
  fprintf(stderr,"list=(%s)\n",list);
#endif

  f = fopen(list,"r");
  if(f != NULL) {
	while(fscanf(f,"%s",&fn)!=EOF){
	    ret = cm_file(fn, attrs);
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

void print_help(void)
{
    printf("Usage: CM [OPTION]... ATTRIBUTES FILE...\n\nChange ATTRIBUTES for the FILE(s).\n\n");
    printf("The OPTIONS are:\n");
    printf("  -i:  interactive mode: prompt (Yes/No/All) for each FILE\n");
    printf("  -v:  verbose: print filename of each removed file\n");
    printf("  -a:  act on read-only, system and hidden files\n");
    printf("  -r:  recurse sub-directories.\n");
    printf("  -h|-H|-?:  display this help and exit\n");
    printf("\nATTRIBUTES should be preceded by a +,- or = character\n");
    printf("    indicating whether to add, remove or set ATTRIBUTES.\n");
    printf("    Then the list of attributes is given (in any order):\n");
    printf("        - A - Archive\n");
    printf("        - S - System\n");
    printf("        - R - Read-only\n");
    printf("        - H - Hidden\n");
    printf("\nFILE is either a filename OR a filename preceeded by '@',\n");
    printf(" in which case the file is opened and read, treating every\n"
	   " line as a file/directory to remove.\n");
    printf("\nCM is part of DOG (https://dog.zumppe.net/)\n");
    return;
}

int main(BYTE n,BYTE *arg[])
{
  BYTE a,r,f,i,j,dir[80];
  int ret = 0;
  struct cm_attribs *attrs = NULL;

  if(n > 2) {
      /* check flags */
      for(i=1;i<n;i++) {
#ifdef CM_DEBUG
	  printf("flags: arg = %s\n",arg[i]);
#endif
	  if ((arg[i][0] == '-') || (arg[i][0] == '/')) {
	      for(j=1;arg[i][j]!='\0';j++) {
		  switch(arg[i][j]) {
		  case 'a':
		      search_attribs = FA_NORMAL | FA_HIDDEN | FA_SYSTEM | FA_RDONLY;
		      flag_f = FLAG_SET;
		      break;
		  case 'i':
		      flag_i = FLAG_SET;
		      break;
		  case 'h':
		  case '?':
		      print_help();
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
	  a = arg[i][0];
	  if (a == '-' || a == '+' || a == '=') {
	      a = arg[i][1];
	      if (a == 'A' || a == 'S' || a == 'H' || a == 'R') {
		  attrs = cm_parse(arg[i]);
		  break;
	      }
	  }
      }
      if (attrs == NULL) {
	  printf("attrs is NULL??\n");
	  print_help();
	  return 0xff;
      }
      for(i=1;i<n;i++) {
#ifdef CM_DEBUG
	  printf("main for-loop: arg = %s\n",arg[i]);
#endif
	  a = arg[i][0];
	  if (a == '@') {
	      cm_list(&arg[i][1], attrs);
	  }
	  else if (a == '-' || a == '+' || a == '=') {
#ifdef CM_DEBUG
	      printf("skipping arg[%d]='%s'\n", i, arg[i]);
#endif
	      ;
	  }
	  else {
#ifdef CM_DEBUG
	      printf("processing arg[%d]='%s'\n", i, arg[i]);
#endif
	      ret = cm_file(arg[i], attrs);
	  }
      }
  }
  else {
      puts("Invalid number of arguments.");
      print_help();
      ret = 0xff;
  }
  free(attrs);
  return ret;
}
