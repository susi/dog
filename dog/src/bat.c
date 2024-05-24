/* bat.c  -  Batchfile processing for DOG.

Copyright (C) 1999  Wolf Bergenheim

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

History

06.05.99 - BAT.C is a now also ported. -WB
2024-05-11 - Building as a module. -WB
2024-05-24 - new syntax for IF -WB

*/
#include "dog.h"

#define _BAT_COMS 8

#define B_C_CA 0
#define B_C_DO 1
#define B_C_44 2
#define B_C_GO 3
#define B_C_IF 4
#define B_C_IN 5
#define B_C_SH 6
#define B_C_TI 7

static void build_cmd(BYTE start, BYTE end);

BYTE batch[_BAT_COMS][3] = {
    "CA",  /*call */
    "DO",  /*DO   */
    "44",  /*FOR  */
    "GO",  /*goto */
    "IF",  /*If   */
    "IN",  /*input*/
    "SH",  /*shift*/
    "TI"   /*Timer*/
};

/***************************************************************************/

void parse_vars(void)
{
	int i;

	for(i=0;i<bf->na;i++) {
		strcpy(varg[i],bf->args[i]);
	}
	for(;i<_NARGS;i++){
		varg[i][0] = '\0';
		bf->args[i] = NULL;
	}

}

/***************************************************************************/

void prevbat(void)
{
    struct bfile *obf;
#ifdef BAT_DEBUG
printf("prevbat:1:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif
    if(bf->prev != NULL) {
        obf = bf->prev;
        free(bf);
        bf = obf;
#ifdef BAT_DEBUG
printf("prevbat:2:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif
    }
	return;
}


/***************************************************************************/

void clearbat(void)
{
    struct bfile *obf;
#ifdef BAT_DEBUG
printf("clearbat:1:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif
    while(1) {
        if(bf->prev != NULL) {
            obf = bf->prev;
            free(bf);
            bf = obf;
        }
        else {
            break;
        }
#ifdef BAT_DEBUG
printf("clearbat:2:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif

    }
#ifdef BAT_DEBUG
printf("clearbat:3:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif
    return;
}

/**************************************************************************/

void do_bat(void)
{

	BYTE ll,i,na,ch, *p,*q;
	struct ffblk *fba;
	FILE *fp;

#ifdef BAT_DEBUG
printf("do_bat:0:bf:%x name=|%s|\n",&(*bf),bf->name);
#endif

    /*
    nest = 0 means that the batchfile is exed from the commandline
    nest > 0 means bfile is jumped to from othe bfile -> change name & line
    in = 0 means not in bfile
    in = 1 means old bfile name is in bf->name
    */

	if(bf->in == 1) {
		fba = malloc(sizeof(struct ffblk));
		i=findfirst(bf->name,fba,0);
		if((i!=0) && (errno==ENOFILE)) {
			fprintf(stderr,"Dogfile %s missing\n",bf->name);
			clearbat();
			bf->nest = 0;
			bf->in = 0;
			bf->line = 0;
			bf->na = 0;
			free(fba);
			return;
		}
		else if(i==0) {
			fp = fopen(bf->name,"r");
			if(fp==NULL) {
				fprintf(stderr,"Can't open DOGfile %s\n",bf->name);
				clearbat();
				bf->nest = 0;
				bf->in = 0;
				bf->line = 0;
				bf->na = 0;
				return;
			}
		}
	}
	free(fba);

#ifdef BAT_DEBUG
printf("do_bat:1:bf->line=%d,bf->nest=%d\n",bf->line,bf->nest);
#endif


/* Return to the previous line as the batchfile is closed after each line*/

	for(i=0;i<bf->line;i++) {
		fgets(com,200,fp);
	}

    /* end of bfile:
                    if nest = 0 return
                    if nest > 0 return one lvl down */

	if (fgets(com,200,fp) == NULL ) {
		if(bf->nest == 0) {
			bf->in = 0;
			bf->line = 0;
			bf->na=0;
			fclose(fp);
			return;
		}
		else {
			prevbat();
			fclose(fp);
			return;
		}

	}

	/* extract $0..$9 to varg[0]..varg[9] */

	bf->line++;

	parse_vars();

#ifdef PARSE_DEBUG
for(i=0;i<_NARGS;i++)
printf("do_bat:2:bf->args[%d]=(%s) varg[%d]=(%s)\n",i,bf->args[i],i,varg[i]);
#endif

	if(redir(com)==0) {
		com[0] = 0;
		arg[0] = com;
		return;
	}
	ll=strlen(com);

	na = parsecom(com,ll);
	fclose(fp);

#ifdef BAT_DEBUG
printf("do_bat:3:ll=%d com=/%s/\n",i,com);
#endif

/* Check for ctrl break */

	if(cBreak) {
		if(bf->in) {
			do {
				fprintf(stderr,"Abort DOG batch (Y/N)? ");
				ch = getchar();
				if((ch=='y') || (ch=='Y')) {
					clearbat();
					cBreak = 0;
					break;
				}
				else if((ch=='n') || (ch=='N')) {
					cBreak = 0;
					break;
				}
			} while (1);
			fprintf(stderr,"\n");
			return;
		}
		else {
			cBreak = 0;
			return;
		}
	}

	do_batcommand(na);

#ifdef BAT_DEBUG
printf("do_bat:7:i=%u\n",i);
#endif
	return;
}

/**************************************************************************/

void do_batcommand(BYTE n)
{
	BYTE i;

#ifdef BAT_DEBUG
	printf("do_batc:0:n = %d\n",n);
	for(i=0;i<n;i++)
	printf("do_batc:1:arg[%d]=(%s)\n",i,arg[i]);
#endif

	if(strlen(arg[0]) == 2) {
		for(i=0;i< _BAT_COMS;i++) {
#ifdef BAT_DEBUG_detail
			printf("do_batc:2:batch[%d]=(%s)\n",i,batch[i]);
#endif
			if(!stricmp(arg[0], batch[i]))
			break;
		}

		switch(i) {
		 case B_C_CA :
			do_ca( n);
			return;
		 case B_C_DO :
			printf("do_batc:3:command=(%s)\n",batch[i]);
			return;
		 case B_C_44 :
			printf("do_batc:3:command=(%s)\n",batch[i]);
			return;
		 case B_C_GO :
			do_go(n);
			return;
		 case B_C_IF :
			do_if(n);
			return;
		 case B_C_IN :
			printf("do_batc:3:command=(%s)\n",batch[i]);
			return;
		 case B_C_SH :
			do_sh(n);
			return;
		 case B_C_TI :
			printf("do_batc:3:command=(%s)\n",batch[i]);
			return;
		 default :
			do_command(n);
			return;
		}
	}
	else {
		do_command(n);
		return;
	}
}

/**************************************************************************/


void do_go(BYTE n)
{
    BYTE l[121], *p,i,label[20];
    FILE *fp;
    if(n < 2)
        return;
    strcpy(label,":");
    strcat(label,arg[1]);

    fp = fopen(bf->name,"r");
    if(fp == NULL) {
        fprintf(stderr,"Batchfile %s missing.\n",bf->name);
        return;
    }
    for(i=0;1;i++) {
        p = fgets(l,120,fp);

#ifdef BAT_DEBUG
printf("label=%8s l=%s\n",label,l);
#endif

        /* EOF reached no label found*/
        if (p == NULL) {
            clearbat();
            bf->line=0;
            fclose(fp);
            return;
        }
        if(strnicmp(l,label,strlen(label))==0) {
            bf->line =i+1;
            fclose(fp);
            return;
        }
    }
}

/****************************************************************************/


void do_ca(BYTE n)
{
    BYTE i;
    struct bfile *nbf;

#ifdef CA_DEBUG
printf("do_ca:1:name=%s na=%u in=%u line=%u nest=%u\n",bf->name,bf->na,bf->in,bf->line,bf->nest);
printf("do_ca:1:bf = %x\n",&(*bf));
#endif


    if(n >= 2) {
/* set up for the new  bf*/
        nbf = (struct bfile*)malloc(sizeof(struct bfile));
        if (nbf == NULL) {
            fprintf(stderr,"Out of memory in DOGfile");
        }

        for(i=1;i<n;i++) {
           nbf->args[i-1] = arg[i];
        }

        D = getcur(P) + 'A';
        sprintf(nbf->name,"%c:\\%s\\%s",D,P,arg[1]);

        nbf->na = n;
        nbf->line = 0;
        nbf->nest = bf->nest+1;
        nbf->in = 1;
        nbf->prev = bf;
        bf = nbf;
#ifdef CA_DEBUG
printf("---------\n");
printf("do_ca:2:name=%s na=%u in=%u line=%u nest=%u\n",bf->name,bf->na,bf->in,bf->line,bf->nest);
printf("do_ca:3:bf = %x\n",&(*bf));
#endif
/*        do_bat(); */
#ifdef CA_DEBUG
printf("do_ca:3:name=%s na=%u in=%u line=%u nest=%u\n",bf->name,bf->na,bf->in,bf->line,bf->nest);
printf("do_ca:3:bf = %x\n",&(*bf));
#endif
    }

    return;

}

/****************************************************************************/
#pragma argsused
void do_sh(BYTE n)
{
	BYTE i;
	for(i=0;i<10;i++)
		bf->args[i] = bf->args[i+1];
	return;
}

/****************************************************************************/


/*
Syntax: IF [CONDITION] <COMMAND1> [ELSE <COMMAND2>]

CONDITION can take the form:
    ERROR [IS|NOT] <NUMBER>     - NUMBER compared to the ERRORLEVEL
    <VARIABLE> [IS|NOT] <VALUE> - VARIABLE is an environment variable
                                and is compard to VALUE
    [NOT] EXIST <FILE>          - Returns true as long as FILE exists.
    IS  - is optional and has no effect other than makes the statement
          look a little bit more like English.
    NOT - reverses the condition value in all cases.
*/

#define COND_ERROR 0x1
#define COND_ERROR_NOT 0x2
#define COND_EXIST 0x4
#define COND_EXIST_NOT 0x8
#define COND_VAR 0x10
#define COND_VAR_NOT 0x20
#define IF_ELSE 0x40

void do_if(BYTE n)
{
    BYTE i, nn=0, el, if_type, if_cmd=0, else_cmd=0, *filename, *var, *val, exist, eval[80];

  if(n < 2) {
      puts("syntax error in command IF:\n"
	   "Syntax: IF [CONDITION] <COMMAND1> [ELSE <COMMAND2>]");
    return;
  }
#ifdef BAT_DEBUG
  printf("do_if:0:n = %d\n",n);
  for(i=0;i<n;i++)
      printf("do_if:1:arg[%d]=(%s)\n",i,arg[i]);
#endif

  if(stricmp(arg[1],"ERROR") == 0) {
#ifdef BAT_DEBUG
      printf("do_if():2:errorlevel=%u el=%u\n", errorlevel, el);
#endif
      if (stricmp(arg[2], "NOT") == 0) {
	  if_type = COND_ERROR_NOT;
	  el = atoi(arg[3]);
	  if_cmd = 4;
      }
      else if (stricmp(arg[2], "IS") == 0) {
	  if_type = COND_ERROR;
	  el = atoi(arg[3]);
	  if_cmd = 4;
#ifdef BAT_DEBUG
	  printf("do_if():3:errorlevel=%u el=%u arg[3]='%s'\n", errorlevel, el, arg[3]);
#endif
      }
      else {
	  if_type = COND_ERROR;
	  el = atoi(arg[2]);
	  if_cmd = 3;
#ifdef BAT_DEBUG
	  printf("do_if():4:errorlevel=%u el=%u arg[2]='%s'\n", errorlevel, el, arg[2]);
#endif
      }
  }
  else if ((stricmp(arg[1], "EXIST") == 0) || (stricmp(arg[2], "EXIST") == 0)) {
      if (stricmp(arg[1], "NOT") == 0) {
	  if_type = COND_EXIST_NOT;
	  filename = arg[3];
	  if_cmd = 4;
      }
      else {
	  if_type = COND_EXIST;
	  filename = arg[2];
	  if_cmd = 3;
      }
  }
  else {
      if (stricmp(arg[2], "NOT") == 0) {
	  var = arg[1];
	  val = arg[3];
	  if_cmd = 4;
	  if_type = COND_VAR_NOT;
      }
      else if (stricmp(arg[2], "IS") == 0) {
	  var = arg[1];
	  val = arg[3];
	  if_cmd = 4;
	  if_type = COND_VAR;
      }
      else {
	  var = arg[1];
	  val = arg[2];
	  if_cmd = 3;
	  if_type = COND_VAR;
      }
  }

  /* look for an else */
  for (i=if_cmd+1; i < n; i++) {
      if (stricmp(arg[i], "ELSE") == 0 && (i < (n-2))) {
	  /* true when we fine ELSE which is not the last word */
	  if_type |= IF_ELSE;
	  else_cmd = i+1;
	  break;
      }
  }

  switch(if_type & 0x3F) {
  case COND_ERROR:
#ifdef BAT_DEBUG
      printf("do_if():5:errorlevel=%u el=%u\n", errorlevel, el);
#endif
      if (errorlevel == el) {
	  /* true, build commandline */
	  nn = (if_type & IF_ELSE)?else_cmd-1:n;
	  build_cmd(if_cmd, nn);
	  nn = nn - if_cmd;
      } else if (if_type & IF_ELSE) {
	  /* false, with else, build commandline */
	  build_cmd(else_cmd, n);
	  nn = n - else_cmd;
      }
      break;
  case COND_ERROR_NOT:
      if (errorlevel != el) {
	  /* true, build commandline */
	  nn = (if_type & IF_ELSE)?else_cmd-1:n;
	  build_cmd(if_cmd, nn);
	  nn = nn - if_cmd;
      } else if (if_type & IF_ELSE) {
	  /* false, with else, build commandline */
	  build_cmd(else_cmd, n);
	  nn = n - else_cmd;
      }
      break;
  case COND_EXIST:
      if (file_exist(filename)) {
	  /* true, build commandline */
	  nn = (if_type & IF_ELSE)?else_cmd-1:n;
	  build_cmd(if_cmd, nn);
	  nn = nn - if_cmd;
      } else if (if_type & IF_ELSE) {
	  /* false, with else, build commandline */
	  build_cmd(else_cmd, n);
	  nn = n - else_cmd;
      }
      break;
  case COND_EXIST_NOT:
      if (!file_exist(filename)) {
	  /* true, build commandline */
	  nn = (if_type & IF_ELSE)?else_cmd-1:n;
	  build_cmd(if_cmd, nn);
	  nn = nn - if_cmd;
      } else if (if_type & IF_ELSE) {
	  /* false, with else, build commandline */
	  build_cmd(else_cmd, n);
	  nn = n - else_cmd;
      }
      break;
  case COND_VAR:
      if ((getevar(var, eval, 80) != NULL) && (strncmp(eval, val, 80)==0)) {
	  /* true, build commandline */
	  nn = (if_type & IF_ELSE)?else_cmd-1:n;
	  build_cmd(if_cmd, nn);
	  nn = nn - if_cmd;
      } else if (if_type & IF_ELSE) {
	  /* false, with else, build commandline */
	  build_cmd(else_cmd, n);
	  nn = n - else_cmd;
      }
      break;
  case COND_VAR_NOT:
      if ((getevar(var, eval, 80) == NULL) || (strncmp(eval, val, 80)!=0)) {
	  /* true, build commandline */
	  nn = (if_type & IF_ELSE)?else_cmd-1:n;
	  build_cmd(if_cmd, nn);
	  nn = nn - if_cmd;
      } else if (if_type & IF_ELSE) {
	  /* false, with else, build commandline */
	  build_cmd(else_cmd, n);
	  nn = n - else_cmd;
      }
      break;
  default:
      puts("syntax error in command IF:\n"
	   "Syntax: IF CONDITION <COMMAND1> [ELSE <COMMAND2>]");
      return;
  }

  if (nn > 0) {
      do_batcommand(nn);
  }

  return;
}

/***************************************************************************/
static void build_cmd(BYTE start, BYTE end)
{
    BYTE i;
    for (i=start; i < end; i++) {
	arg[i-start] = arg[i];
    }
}

/***************************************************************************/

BYTE file_exist(const char * filename)
{
    const char *fn=filename;

    asm mov ax, 4300h; /* Get file attrib */
    asm mov dx, fn; /* DS:DX: path to file to open */
    asm push cs;
    asm pop ds;
    asm clc;
    asm int 21h;
    asm jnc found;
    return 0;
found:
    return 1;
}
