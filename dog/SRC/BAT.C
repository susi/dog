/*BAT.C  -  Alternate command processor for (currently) MS-DOS ver 3.30

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

06.05.99 - BAT.C is a now also ported.

*/
#define _BAT_COMS 8

#define B_C_CA 0
#define B_C_DO 1
#define B_C_44 2
#define B_C_GO 3
#define B_C_IF 4
#define B_C_IN 5
#define B_C_SH 6
#define B_C_TI 7

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

void parse_vars(BYTE *s,BYTE *t)
{

  BYTE *p,l,*e,i,j,ename[80],eval[127];
  
  *t = 0;
  l = strlen(s);

  e = t;

  for(i=0;i<l;i++) {
    if(s[i] == '$') {
      if(isdigit(s[++i])) {
	strcat(t,bf->args[s[i] -'0']);
#ifdef parse_debug
printf("parse_vars:0:t=(%s)\n",t);
#endif
	e += strlen(bf->args[s[i] -'0']);
      }
      else {
	for(j=0;!isspace(s[i]);j++,i++) {
	  ename[j] = s[i];
	}
	strcat(t,getevar(ename,eval)); /* replace $varname with value*/
	e += strlen(eval);
      }
    }
    else {
      *e = s[i];
      *(++e) = '\0';
    }
  }
}
/***************************************************************************/

void prevbat(void)
{
    struct bfile *obf;
#ifdef bat_debug
printf("prevbat:1:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif
    if(bf->prev != NULL) {
        obf = bf->prev;
        free(bf);
        bf = obf;
#ifdef bat_debug
printf("prevbat:2:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif
    }
	return;
}


/***************************************************************************/

void clearbat(void)
{
    struct bfile *obf;
#ifdef bat_debug
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
#ifdef bat_debug
printf("clearbat:2:bf:%x bf->prev:%x\n",&(*bf),bf->prev);
#endif

    }
#ifdef bat_debug
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

#ifdef bat_debug
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
        if((i==255) && (errno==ENOFILE)) {
            fprintf(stderr,"Dogfile %s missing\n",bf->name);
            clearbat();
            bf->nest = 0;
            bf->in = 0;
            bf->line = 0;
            bf->na = 0;
			free(fba);
			for(i=0;i<bf->na;i++) {
				free(bf->args[i]);
			}
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
                for(i=0;i<bf->na;i++) {
                    free(bf->args[i]);
                }
                return;
            }
        }
    }
	free(fba);

#ifdef bat_debug
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
            for(i=0;i<bf->na;i++) {
                free(bf->args[i]);
            }
            return;
        }
        else {
            prevbat();
            fclose(fp);
            for(i=0;i<bf->na;i++) {
                free(bf->args[i]);
            }
            return;
        }

    }

	/* extract $0..$9 to arg[0]..arg[9] */

    bf->line++;
	p=malloc(200);

#ifdef parse_debug
for(i=0;i<_NARGS;i++)
printf("do_bat:2:bf->args[%d]=(%s)\n",i,bf->args[i]);
#endif

	parse_vars(com,p);
	strcpy(com,p);
	free(p);

    if(redir(com)==0) {
        com[0] = 0;
        arg[0] = com;
        for(i=0;i<bf->na;i++) {
            free(bf->args[i]);
        }
        return;
    }
    ll=strlen(com);
    na = parsecom(com,ll);
    fclose(fp);

#ifdef bat_debug
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
            for(i=0;i<bf->na;i++) {
                free(bf->args[i]);
            }
            return;
        }
        else {
            cBreak = 0;
            return;
        }
    }

    do_batcommand(na);

#ifdef bat_debug
printf("do_bat:7:i=%u\n",i);
#endif
    for(i=0;i<bf->na;i++) {
        free(bf->args[i]);
    }
    return;
}

/**************************************************************************/

void do_batcommand(BYTE n)
{
    BYTE i;

#ifdef bat_debug
printf("do_batc:0:n = %d\n",n);
for(i=0;i<n;i++)
printf("do_batc:1:arg[%d]=(%s)\n",i,arg[i]);
#endif

    if(strlen(arg[0]) == 2) {
        for(i=0;i< _BAT_COMS;i++) {
#ifdef bat_debug_detail
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
                printf("do_batc:3:command=(%s)\n",batch[i]);
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
                printf("do_batc:3:i=%d\n",i);
                break;

        }
        if(arg[0][0] == ':')
        	return;
        else {
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

#ifdef bat_debug
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

#ifdef ca_debug
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
#ifdef ca_debug
printf("---------\n");
printf("do_ca:2:name=%s na=%u in=%u line=%u nest=%u\n",bf->name,bf->na,bf->in,bf->line,bf->nest);
printf("do_ca:3:bf = %x\n",&(*bf));
#endif
/*        do_bat(); */
#ifdef ca_debug
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

