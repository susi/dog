/*
DOG.C  -  Alternate command processor for (currently) MS-DOS ver 3.30

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

Contact autor: internet: dog@users.sourceforge.net
                         http://www.hut.fi/~hbergenh/DOG/

History

19.06.98 - Started
22.06.98 - Started: void initialize()
25.06.98 - made the CL command
26.06.98 - made the VR command
28.06.98 - made TM and DT commands.
29.06.98 - started on int getcom(BYTE *com,BYTE *arg[20])
22.07.98 - started on void do_cd(blah)
21.08.98 - started on void do_ls(blah)
20.10.98 - started on void do_br(blah)
20.10.98 - improved void_do_ls(blah) to take read arguments in any order
20.10.98 - Started on void do_exe(blah)
23.10.98 - do_exe ready, also do_md. do_rd, do_br, do_vf & eh
29.10.98 - most internal commands complete. Starting to exp on other stuff
30.10.98 - Woha. The immensity of the project hit me just now. Putting dog
           on ICE.
01.02.99 - Lifted DOG out of ICE. rewrote large parts of do_exe(blah).
02.02.99 - IMBOLC - rewrote parts of do_bat(), but encoutered a strange
                    thing... DOG executes every externel program twice
                    could be because of the do_exe(blah). have to snoop :)
03.02.99 - fixed some bugs in do_exe(blah) and do_dog().
04.02.99 - Modified the do_vr() to recognize different vedors of DOS.
08.02.99 - Fixed a bug. Now You can execute any .COM .EXE .DOG file by giving
           the full path.
09.02.99 - Made the void do_tp(BYTE n) function. All internal
           commands (ecept cp) are operational.
25.02.99 - Improved do_pp to understand special characters. ls can now take
           multiple file-patterns and list them all!
03.03.99 - Improved on do_cd to accept cd.... etc and interpet it correctly
04.03.99 - To Do: CTTY,CHCP?,DOSKEY,set Label          in bat exist,not,pause
           piping redirection
05.03.99 - Bugfix. Found some minor bugs. Found and old redudndant function.
           Removed it.
08.03.99 - Still can't get tm -s to work...
09.03.99 - Copied the idea of parse_time form FreeCom, improved it and NOW
           the -s switch in do_tm(blah) WORKS!!! :)
           rewrote the initialize function. now you can use many paramters.
           -P and -C are the onlyones so far.. -C MUST BE LAST!
12.03.99 - Bug in -E switch. environment scrapped. change to resize.
15.03.99 - Fixed some bugs. Inserted Ctrl-Brkeak ^C handling / recognizing.
           used ctrl-c handler fon FreeCom.
29.03.99 - ctrlc.c doesn't work... (wonder what's wrong?). the DOG-file
           commands bp (beep) and go (go to label) work just great!
           proceeding with other DOG-file commands...
07.04.99 - added meta-strings for tab ret nl bs % in do_eh()
           strange????
           The small ctrl-c handler (in this file) works but ctrlc.c doesn't.
16.04.99 - Added %c for clock and %d for date to the prompt meta-characters.
           Fixed a bug in ls.
19.04.99 - bug in do_bp added one to n.
           rewrote the batch system. each DOGfile state isa saved in a linked
           list this makes it possible to call a DOGfile and then retun to
           the old.
           Ctrl-C in a nested bfile will terminate ALL levels of nest.
21.04.99 - Added wild-cards to tp();
22.04.99 - Stared on redir.
23.04.99 - stdout redir ready. fixed do_tp.
27.04.99 - stdin redirecting ready.
28.04.99 - in:do_ls() if pattern ends in ':' or '\' then append *.* to it.
           started on piping (still commented out)
29.04.99 - Found a bug in parse_time() pointer error. fixed it.
30.04.99 - changed all occurances of getchar to getkey. i thing there is
           something wrong with the getchar in the lib..
03.05.99 - Added \b recognition to getln.
04.05.99 - F U C K I N G   M I C R O - C   C O M P I L E R ! ! ! !
           gona change to borland c++ 3.1 :(
05.05.99 - Porting to Borland C++ 3.1
06.05.99 - Done porting DOG.C & DOG.H;
           compile with options -1- -AT -B -d -mt -O1 -tDc
           dumped file CTRLC.C
19.05.99 - rewrote do_md & rd in to one function; The changing of the dir is
           handled by do_mrd (the combined do_md & do_rd. saved 100 bytes :)
21.05.99 - rewrote logical error in do_exe: searched for arg[0] before arg[0].COM
           fixed it. made my_exe to handle execution of external prog.
25.05.99 - Dirs are now executable you just need to type a dir and then you change
           there. Suggestion by Jonas Berlin.
05.08.99 - Did some Bugfixes. Wugene Wong rewrote the parse_time() function.
           It should be faster now.
09.08.99 - Fixed do_cd() to accept ...etc as directory.
10.08.99 - Modified do_dt() to accept switch -s to set date. Added function
           parse_date.
12.08.99 - Modified do_command to treat commands starting with "..." as a directory,
           thus starting the cd command.
           Fixed both parse_time() and parse_date() to default to invalid 
           values -> multiple calls -> fail.
           Fixed do_sz() to display disksize correctly.
21.08.99 - Major revition of DOG. 1:sorted dog commands alphabeticly.
24.08.99 - Rewrote do_rm(). Now removing files in other dirs is possible.
           Added file ints.c with int 23,24,2e handles.
01.09.99 - Changed %ld to %lu in do_sz()
04.09.99 - Rewrote initialize. Now the environment gets set up correctly
           The Permanent shel option(-p) is set to point the int 2eh at
           DOGVerFunc. It also creates a new environment. With the -e option
           an environment is resized. If the env block needs to belocated
           it will be recreated with the default env vars.
05.09.99 - Wrote setevar() and getevar(); Wrote do_se();
09.11.99 - Removed do_dt() and do_tm() as per the request of Eugene Wong.
           Time and date functions are now hanled by CLK.
08.12.99 - CLK renamed to DT for Day and Time.
           
*/

#include "dog.h"
#include "bat.c"
#include "ints.c"


BYTE initialize(int nargs, char *args[])
{

    BYTE i,j,k,*p,*q,line[200]={0},sw_P=0,rebuild=0;
    WORD w;
    envseg = peek(_psp,ENVSEG_OFS);
    _env = MK_FP(envseg,0);    
    envsz = peek(envseg-1,3) << 4; /*get size of block allocated from MCB*/

    if(nargs == 1) {
        return 0;
    }

    /* get the segment of the environment from the PSP*/
    envseg = peek(_psp,ENVSEG_OFS);

    /* make the commandline to one string without spaces */
    for(i=2,strcpy(line,args[1]);i<nargs;i++)
        strcat(line,args[i]);

    p=line;
    strupr(p); /* upper case it*/
    while(*p!='\0') {
        if(*p=='-') {
            switch(*(++p)) {
                case 'C': /* execute one command, then exit */
                    /* can't execute just one command on a permanent shell*/
                    if(sw_P == 1) { 
                        puts("Incompatible switches.");
                        puts("You are stuck in this shell, sorry.");
                        return 0;
                    }
                    /* copy the arg vectors to the command */
                    for(i=1;i<nargs;i++) {
                        if(strnicmp(args[i],"-C",2)==0){
                            if(strlen(args[i]) > 2)
                                args[i] = &args[i][2];
                            else
                                i++;
                            break;
                        }
                    }

                    for(j=0;i<nargs;i++,j++) {
                        arg[j] = args[i];
                    }
                    Xitable = eh = 1;
                    Xit = 111;
                    return j;

                case 'E': /* set the size of the environment */
                    rebuild = 1; /* the environment will be 
                                    rebuilt from scratch */
                    w = envsz;
                    envsz=0;
                    /* Accept all strings beginning with -E */
                    while((!isdigit(*p))&&(*p!='\0')&&(*p!='-')) p++;
                    while(isdigit(*p)) envsz=envsz*10+(*p++)-'0';
                    envsz /= 16;
                    if(envsz < 10) envsz=10;
                    if(envsz > 800) envsz=800;
                    asm {
                        mov AH,4ah
                        mov BX,envsz
                        mov DX,envseg
                        mov ES,DX
                        int 21h
                        jnc env_ok
                    }
                    env_not_ok:
                    asm {
                        sub ax,07h /*errors 7,8,9 possible*/
                        jz env_block_dest
                        dec ax
                        jz out_of_memory
                        jmp invalid_mem_block
                    }
                    
                    env_block_dest:
                    fprintf(stderr,"Environment block destroyed!\nBuilding new.\n");
                    env_rebuild:
                    asm {
                        mov ah,48h
                        mov bx,envsz
                        int 21h
                        jc env_not_ok
                        mov envseg,ax
                    }
                    poke(_psp,ENVSEG_OFS,envseg);
                    env_ok:
                    break;
/* probably never happens... remove? */
                    invalid_mem_block:
                    fprintf(stderr,"Invalid Environment!\nBuilding new.\n");
                    asm {
                        jmp env_rebuild
                    }

                    out_of_memory:
                    fprintf(stderr,"Environment block too small!\nBuilding new.\n");
                    asm {
                        mov ah,49h
                        mov dx,envseg
                        mov es,dx
                        int 21h
                        mov ah,1
                        mov rebuild,ah
                        jmp env_rebuild
                    }
                    break;
                case 'P':/* make a permanent shell */
                    sw_P = 1;
                    rebuild = 1;
                    envsz = 0;
                    /* Accept all strings beginning with -P */
                    while((!isdigit(*p))&&(*p!='\0')&&(*p!='-')) p++;
                    while(isdigit(*p)) envsz=envsz*10+(*p++)-'0';
                    envsz /= 16; /*paragraphs*/
                    if(envsz < 10) envsz=10;
                    if(envsz > 800) envsz=800;
                    p_env_retry:
                    asm {
                        mov bx,envsz
                    }
                    p_env_not_mem_retry:
                    asm {
                        mov ah,48h
                        int 21h
                        jnc p_env_ok
                        sub ax,7
                        jz p_env_retry
                        dec ax
                        jz p_env_not_mem_retry
                    }
                    p_env_ok:
                    asm {
                        mov envseg,ax
                    }
                    poke(_psp,ENVSEG_OFS,envseg);
                    /* make int 2e point to DOGVerFunc */
                    asm {
                        /* save */
                        mov ax,352eh
                        mov i2e_o,bx
                        mov i2e_s,es
                        int 21h
                        /* set */
                        mov ax,252eh
                        mov dx,offset DOGVerFunc
                        push cs
                        pop es
                        int 21h
                        /* point int 22 termination address at DOG loop*/
                        mov ax,2522h
                        mov dx,offset DOG_loop
                        mov i22_o,dx
                        push cs
                        pop ds
                        mov i22_s,ds
                        int 21h
                    }

                    /* put int handlers in to PSP */

                    poke(_psp,0x0a,i22_o);
                    poke(_psp,0x0a+2,i22_s);
                    poke(_psp,0x0e,i23_o);
                    poke(_psp,0x0e+2,i23_s);
                    poke(_psp,0x12,i24_o);
                    poke(_psp,0x12+2,i24_s);
                    /* makeDOG it's own parrent*/
                    poke(_psp,PPID_OFS,_psp);

                    Xitable = 0;
                    break;
            }
        }
        else
            /* ignore */
            p++;
    }
    _env = MK_FP(envseg,0);
    envsz <<= 4; 
    if (rebuild==1) {
        if((p=malloc(envsz))!=NULL) {
            q=p;
            i=0;
            sprintf(p,"COMSPEC=%s",args[0]);
            i += strlen(p)+1;
            p += i; /*point to after terminating 0 */
            strcpy(p,"PATH=C:\\DOS;C:\\DOG\\UTIL;C:\\;..");
            i += strlen(p)+1;
            p += 31;
            strcpy(p,"PROMPT=");
            strcat(p,_PROMPT);
            i += strlen(p)+1;
            p = q+i;
            *(p++) = 0; /* = terminating the env */
            *(p++) = 1; *(p++)=0; /* WORD number of strings to follow*/
            i += 3;
            sprintf(p,"%s",args[0]);
            i += strlen(p)+1;
            p += i; /*point to after terminating 0 */

            for(j=0;j<i;j++) {
                *(_env+j) = *(q+j);
#ifdef env_debug
                if(*(q+j) != '\0')
                    printf("%c",*(q+j));
                else
                    printf("0\n");
#endif
            }
#ifdef env_debug
            puts("");
#endif
        }
        free(p);
    }
    return 0;
}

/****************************************************************************/

BYTE *trueName(BYTE *name,BYTE *tn)
{
    asm{
        push ds
        pop es
        mov si,name
        mov di,tn
        mov ah,60h
        int 21h
        jnc tn_ok
    }
    return NULL;
    tn_ok:
    return tn;

}


/**************************************************************************/

void scankey(BYTE *p,BYTE len)
{
    BYTE *t,curr,start,scan,key;

    asm{
        mov AH,03h
        xor BH,BH
        int 10h
        mov start,DL
        mov curr,DL
    }
    KBD_LOOP:
    asm{
        MOV AH,01h
        INT 16h
        JZ KBD_LOOP
        MOV scan,AH
        MOV key,AL
    }

    switch(key) {
        case '\0':
            switch(scan) {
                case 0x48 : /*Up arrow*/
                    break;
                case 0x50 : /*down arrow*/
                    break;

                case 0x4B : /*left arrow*/
                    asm{
                        MOV AH,03h
                        XOR BH,BH
                        INT 10h             /*get cursor pos*/
                        MOV AL,start           /*get start pos of cmd line*/
                        CMP DL,AL           /*are we in the start?*/
                        JE  left_end        /*yes*/
                        CMP DL,00h          /*are we at poss 0?*/
                        JNE left_same_row   /*no*/
                        DEC DH              /*cursor goes up one line*/
                        MOV DL,50h          /*and to the end*/
                        JMP left_doit
                    }
                    left_same_row:          /*we are not in the beg of a row*/
                    asm{DEC DL;}            /*cursor goes back one step*/
                    left_doit:
                    asm{
                        MOV AH,02h
                        INT 10h             /*move cursor to new pos*/
                    }
                    p--;
                    left_end:
                    break;

                case 0x4D : /*Right arrow*/
                    if ((curr - start < len) && ((p+curr-start+1) !='\0'))  {
                        p++;
                        curr++;
                        asm{
                            MOV AH,03h
                            XOR BH,BH
                            INT 10h
                            CMP DL,50h      /*is it the end of the line?*/
                            JNE right_same_row /*yes*/
                            INC DH
                            MOV DL,00h      /* move cursor to nextline pos 0*/
                            JMP right_doit
                        }
                        right_same_row:
                        asm{INC DL;}
                        right_doit:
                        asm{
                            MOV AH,02h
                            INT 10h         /* move cursor to new pos*/
                        }
                    }
                    break;
                case 0x3B : /*F1*/
                    break;
                case 0x3C : /*F2*/
                    break;
                case 0x3D : /*F3*/
                    break;
                case 0x3E : /*F4*/
                    break;
                case 0x3F : /*F5*/
                    break;
                case 0x40 : /*F6*/
                    break;
                case 0x41 : /*F7*/
                    break;
                case 0x42 : /*F8*/
                    break;
                case 0x43 : /*F9*/
                    break;
                case 0x44 : /*F10*/
                    break;

            }
            break;

        case '\b':
            t = p+1;
            *p = '\0';
            strcat(p,t);
            asm{
                MOV AH,03h
                XOR BH,BH
                INT 10h             /*get cursor pos*/
                PUSH DX             /*save Pos*/
            }
            printf("%s",t);
            asm {
                MOV AH,02h
                XOR BH,BH
                POP DX
                INT 10h             /*restore cursor*/
            }
            break;

        case '\r':
            putchar('\n');
            *p = '\0';
            return;

        default:
            if((curr-start) < len) {
                putchar(key);
                *p = key;
                p++;
                curr++;
            }
            else
                putchar('\x07');
    }

    asm{
        JMP KBD_LOOP
    }
}

/**************************************************************************/


BYTE getln(BYTE *s, BYTE lim)
{
    BYTE i;

    memset(s,0,lim);

#if defined scankey
    scankey(s,lim-1);
#else
    gets(s);
#endif
    i = strlen(s);


#ifdef debug
fprintf(stderr,"getln:2: i:%d s:!%s!\n",i,s);
#endif

    return i;
}

/**************************************************************************/
BYTE parsecom(BYTE * line,BYTE ll)
{
    BYTE i=0,j=0;

#ifdef debug
    printf("parsecom:0: line(%s)\n",line);
#endif

    while((j<20) && (i<ll)) {
        while((isspace(line[i]) || (line[i] == '\0') )&& (i<ll))
            line[i++] = '\0';

        if(i<ll)
            arg[j++] = &line[i];

        while(!isspace(line[i]) && (i<ll))
            i++;
    }

#ifdef debug
    printf("parsecom:3: j=%d line: !%s! arg(",j,line);
    for(i=0;i<j;i++)
        printf("%s|",arg[i]);
    printf(")\n");
#endif

    return j;
}



/**************************************************************************/

BYTE getcom(BYTE *com)
{

    BYTE ln,r;

    ln = getln(com, 200);

    if(redir(com)==0) {
        com[0] = 0;
        arg[0] = com;
        return 0;
    }

    ln = strlen(com);
#ifdef debug
    fprintf(stderr,"getcom:1: com: !%s! ln:%u\n",com,ln);
#endif
    r = parsecom(com,ln);
#ifdef debug
    fprintf(stderr,"getcom:2: com: !%s! ln:%u\n",com,ln);
#endif

    if(cBreak) {
        cBreak = 0;
        return 0;
    }

#ifdef debug
    fprintf(stderr,"getcom:3: r=%d com: !%s!\n",r,com);
#endif
    return r;
}

/**************************************************************************/

BYTE isfchar(BYTE c)
{
    if(isalnum(c))
        return 1;

    switch(c) {
        case '!':
        case '@':
        case '#':
        case '$':
        case '%':
        case '&':
        case '(':
        case ')':
        case '{':
        case '}':
        case '\'':
        case '\\':
        case '`':
        case '.':
        case ':':
            return 1;
        default:
            return 0;

    }

}

/**************************************************************************/

BYTE redir(BYTE *c)
{
    BYTE l,i;
    BYTE *fo,*fi,*p;
    FILE *in,*out;

    l = strlen(c);
    p = c;


    for(i=0;i<l;i++) {
#ifdef debug
printf("redir:0: &p(%x) *p(%c)\n",p,*p);
#endif
        if(*p == '>') {
            *p = '\0';
            strcpy(fout.opt,"w");
            if((*(++p)) == '>') {
                strcat(fout.opt,"a");
                *p = ' '; /*erase the >*/
            }

#ifdef debug
printf("redir:1:c(%s) p(%s) *p(%c)\n",c,p,*p);
#endif
            /* skip spaces and tabs*/
            while(isspace(*(p++)));
            fo=p-1; /*begining of filename*/
            while(isfchar(*(++p)));
            /* p points to first spc AFTER filename*/
#ifdef debug
printf("redir:2:*fo(%c) *p-1(%x) *p(%x) *p+1(%x)\n",*fo,*(p-1),*p,*(p+1));
#endif
            if(*(p-1)=='\n')
                *(p-1)='\0';
            *p = '\0';
            strcpy(fout.name,fo);
            fout.redirect = 1;
            strcat(c," ");
            strcat(c,p);
        }

        else if(*p == '<') {
            *p = '\0';
            strcpy(fin.opt,"r");

            /* skip spaces and tabs*/
            while(isspace(*(p++))) ;
            if(isspace(*p))
                p++;
            fi=p; /*begining of filename*/
#ifdef debug
printf("redir:3:fi(%x) &fi(%x) &p(%x) *p(%x)\n",fi,fi,p,p);
#endif
            while(isfchar(*(++p)))
            {
#ifdef debug
printf("redir:3:fi(%x) &fi(%x) &p(%x) *p(%x)\n",fi,fi,p,p);
#endif
            }

            /* p points to first spc AFTER filename*/
            if(*(p-1)=='\n')
                *(p-1)='\0';
            *p = '\0';
#ifdef debug
printf("redir:4:fi(%s) &fi(%x) &p-1(%x) &p(%x) &p+1(%x)\n",fi,fi,(p-1),p,(p+1));
#endif
            strcpy(fin.name,fi);
            fin.redirect = 1;
            strcat(c," ");
            strcat(c,p);
        }
/*
        else if(*p == '|') {
            *p = '\0';
            p++;
            strcpy(pipe.line,p);
            pip.pipe = 1;
            sprintf(fout.name,"%c:\\%s\\$$%%!!DOG.$$$",D,P);
            fout.redirect = 1;
            strcpy(fout.opt,"w");

        }
*/
        else {
            p++;
        }

    }
    if(fout.redirect) {
#ifdef debug
printf("redir:5:c(%s) fout.name(%s)\n",c,fout.name);
#endif
        if((fout.fp = fopen(fout.name,fout.opt)) == NULL) {
            fprintf(stderr,"Unable to redirect output to file %s.\n",fout.name);
            fout.redirect=0;
            return 0;
        }

        dup2(fileno(fout.fp),fileno(stdout));
    }

    if(fin.redirect) {
#ifdef debug
printf("redir:6:c(%s) fin.name(%s)\n",c,fin.name);
#endif
        if((fin.fp = fopen(fin.name,fin.opt)) == NULL) {
            fprintf(stderr,"Unable to redirect input from file %s.\n",fin.name);
            fin.redirect=0;
            return 0;
        }

        dup2(fileno(fin.fp),fileno(stdin));
    }

    return 1;
}

/**************************************************************************/

BYTE getcur(BYTE *p)
{
    BYTE d,t;
    asm {
        MOV ax,1900h
        INT 21h
        MOV d,al
    }
    t = getcurdir(0,p);

    if (t!=0) {
        fprintf(stderr,"Error %d while reading dir\n",t);
        return 0xff;
    }

    return d;
}

/****************************************************************************/

BYTE *getevar(BYTE *varname, BYTE *value)
{
    BYTE *p,*q,ev[512];
    WORD nlen;

    p=varname;
    q=value;
    nlen = strlen(p);

    while(*_env) {
        if(*(_env+nlen) == '=') { /*possible match */
            *(_env+nlen) = '\0';
            sprintf(ev,"%Fs",_env);
            *(_env+nlen) = '=';
            if(stricmp(varname,ev) == 0) { /*found it! */
                _env += nlen+1;
                while(*_env != '\0') {
                    *(q++) = *(_env++);
                }
                /* restore _env */
                *q='\0';
                _env=MK_FP(envseg,0);
                return value;
            }
            else { /* no match in _env to next */
                while(*(_env++) != '\0');
            }
        }
        else { /*no match */
            while(*(_env++) != '\0');
        }
    } /* _env points to '\0' if end of env */

    /* env not found return NULL */
    /* restore _env */
    _env=MK_FP(envseg,0);
    return NULL;
}

/****************************************************************************/

void setevar(BYTE *varname, BYTE *value)
{
    BYTE t[512],far *rest,*p,*b,i,far *eoe, far *evalue,found=0;
    WORD w,ss,writesize,nlen,envleft;

    nlen = strlen(varname);
    writesize = strlen(varname)+strlen(value)+2; /* = strings + 0 + '='*/
    b=malloc(writesize);
    strupr(varname);
    strcpy(b,varname);
    strcat(b,"=");
    strcat(b,value);
    envsz = peek(envseg-1,3) << 4;

    evalue = eoe = rest = _env;


    /* make eoe point to last byte of used env */

    for(w=0;w<envsz;w++,eoe++) {
        if(*eoe== 0) {
            if(*(++eoe) == 0) {
                i = *(++eoe);
                eoe+=2;
                 while(i>0) {
                    while(*(eoe++) != '\0');
                    i--;
                    }

                break;
            }

        }
        else {
        }
    }

    /* check if var exists, if it does write new value in place of old
                            if not then write new var to end of env */


    while(*rest!='\0') {
        if(*(rest+nlen) == '=') { /*possible match */
            *(rest+nlen) = '\0';
            sprintf(t,"%Fs",rest);
            *(rest+nlen) = '=';
            if(stricmp(varname,t) == 0) { /*found it! */
                /*save pos*/
                evalue = rest;
                /* make rest point to next var */ 
                while(*(rest++)!='\0');
                /*bail out */
                found = 1;
                break;
            }
            else { /* no match; point rest to next */
                while(*(rest++));
            }
        }
        else { /*no match */
            while(*(rest++)!='\0');
        }
    } /* rest points to '\0' if end of env */
    if(found==0)
        evalue = rest;

    sprintf(t,"%Fs",evalue);
    /* calculate the number of bytes left in the env. */
    envleft = envsz - FP_OFF(eoe) + (found * strlen(t));

    if(writesize > envleft) {
        puts("Out of environment space");
        return;
    }

    /*calc save size */
    ss = FP_OFF(eoe) - FP_OFF(rest) + 1;

    /*save rest of environment*/
    p=malloc(ss);
    for(w=0;w<ss;w++) {
        *(p+w) = *(rest+w);
    }


    if((found == 1) && (value == NULL)) {
        w=0;
        /*the env var will be owerwritten with rest.*/
    }
    else { /*write value to environment. evalue points*/
        for(w=0;w<writesize;w++) {
            *(evalue+w) = *(b+w);
        }
    }
    evalue += w;

    /*put the rest back*/
    for(w=0;w<ss;w++) {
        *(evalue+w) = *(p+w);
    }

    free(p);
    free(b);

    return;
}

/****************************************************************************/

void printprompt(void)
{
    WORD yr;
    BYTE dow,mo,day,i,k,t,m,s,h;

    prompt = malloc(200);

    if((prompt = getevar("PROMPT",prompt)) != NULL)
        k = strlen(prompt);
    else 
        k = 0;
       
/*  0x01 = path
    0x02 = date
    0x03 = time
*/

    for(i=0;i<k;i++) {
        if((prompt[i] == '%')||(prompt[i]=='$')) {
            switch(prompt[++i]) {
                case '%' :
                    putchar('%');
                    break;
                case  '_':
                case  'S':
                    putchar(' ');
                    break;
                case  'b':
                case  'B':
                    putchar('|');
                    break;
                case  'e':
                case  'E':
                    putchar('\x1b');
                    break;
                case  'h':
                case  'H':
                    putchar('\b');
                    break;
                case  'l':
                case  'L':
                    putchar('<');
                    break;
                case  'g':
                case  'G':
                    putchar('>');
                    break;
                case  'n':
                    putchar('\n');
                    break;
                case  'r':
                    putchar('\r');
                    break;
                case  't':
                    putchar('\t');
                    break;
                case  'p':
                case  'P':
                    D = getcur(P) + 'A';
                    printf("%c:\\%s",D,P);
                    break;
                case  'd':
                case  'D':
                    asm {
                        MOV ah,2ah
                        INT 21h
                        MOV dow,AL  /*day of week*/
                        MOV yr,CX     /*year*/
                        MOV m,DH     /*month*/
                        MOV day,DL     /*day*/
                    }
                    switch(dow) {
                        case 0 :
                            printf("Sun ");
                            break;
                        case 1 :
                            printf("Mon ");
                            break;
                        case 2 :
                            printf("Tue ");
                            break;
                        case 3 :
                            printf("Wed ");
                            break;
                        case 4 :
                            printf("Thu ");
                            break;
                        case 5 :
                            printf("Fri ");
                            break;
                        case 6 :
                            printf("Sat ");
                            break;
                        default :
                            printf("    ");
                    }
                    printf("%02u.%02u.%04u",day,mo,yr);
                    break;
                case  'c':
                case  'T':
                  asm {
                      MOV ah,2ch
                      INT 21h
                      MOV t,ch     /*hours*/
                      MOV m,cl     /*minutes*/
                      MOV s,dh     /*seconds*/
                      MOV h,dl     /*hundreds*/
                  }
                  printf("%d.%02d.%02d,%02d",t,m,s,h);
                  break;
              default :
                ;
            }
        }
        else
            putchar(prompt[i]);
    }
    free(prompt);
    return;
}




/**************************************
***************************************
**                                   **
** int main(int nargs, char *argv[]) **
**                                   **
***************************************
*****************************************************************************/



int main(int nargs, char *argv[])
{
    BYTE i,na,ch;

    Xitable = 1;

printf("PSP = %x PPID = %x\n",_psp,peek(_psp,PPID_OFS));


    if((_osmajor < 3) && (_osminor < 30)) {
        printf("Sorry your DOS is too lame.\nGet at least version 3.30\n");
        exit((_osmajor<<8)+_osminor);
    }

    /* save STDIN STDOUT */
    IN = dup(fileno(stdin));
    OUT = dup(fileno(stdout));

/*
  set own int 23h (Ctrl-C) handler/
    get_vector(0x23,&cc_s,&cc_o);
    set_vector(0x23,get_cs(), &ctrlc);
*/
    get_int();

    strcpy(prompt,_PROMPT);
    D = getcur(P) + 'A';
    na = initialize(nargs, argv);

    bf = malloc(sizeof(struct bfile));
    bf->prev = NULL;
    bf->in = 0;
    bf->nest = 0;


    drvs = 0;


    if (eh == 0) {
        printf("旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커\n");
        printf("쿏OG � Dog Operating Ground Version %u.%02u           �\n",DOG_ma,DOG_mi);
        printf("�       Copyright Wolf Bergenheim 1997-2007        �\n");
        printf("�                                                  �\n");
        printf("쿟ype HH for Help                                  �\n");
        printf("읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸\n\n");
    }

/*******************************.D.O.G. .L.O.O.P****************************/
                                                                         /**/
    for(EVER) {                                                          /**/
        asm {                                                            /**/
            DOG_loop:                                                    /**/
        }                                                                /**/
                                                                         /**/
        set_int();                                                       /**/
                                                                         /**/
        if(fout.redirect) {                                              /**/
#ifdef debug                                                             /**/
fprintf(stderr,"main:0:closing handle %x\n",fileno(fout.fp));            /**/
#endif                                                                   /**/
            dup2(OUT,fileno(stdout));                                    /**/
            close(fileno(fout.fp));                                      /**/
            fout.redirect = 0;                                           /**/
        }                                                                /**/
                                                                         /**/
        if(fin.redirect) {                                               /**/
#ifdef debug                                                             /**/
fprintf(stderr,"main:0:closing handle %x\n",fileno(fin.fp));             /**/
#endif                                                                   /**/
            dup2(IN,fileno(stdin));                                      /**/
            close(fileno(fin.fp));                                       /**/
            fin.redirect = 0;                                            /**/
        }                                                                /**/
                                                                         /**/
        for(i=0;i<MAXDIR;i++)                                            /**/
            P[i] = 0;                                                    /**/
        D = getcur(P) + 'A';                                             /**/
                                                                         /**/
        /* Check for cBreak                                               **/
                                                                         /**/
        if(cBreak == 1) {                                                /**/
            if(bf->in) {                                                 /**/
                do {                                                     /**/
                    fprintf(stderr,"Abort DOG batch (Y/N)? ");           /**/
                    ch = getchar();                                      /**/
                    if((ch=='y') || (ch=='Y')) {                         /**/
                        clearbat();                                      /**/
                        cBreak = 0;                                      /**/
                        break;                                           /**/
                    }                                                    /**/
                    else if((ch=='n') || (ch=='N')) {                    /**/
                        cBreak = 0;                                      /**/
                        break;                                           /**/
                    }                                                    /**/
                } while (1);                                             /**/
                fprintf(stderr,"\n");                                    /**/
                continue;                                                /**/
            }                                                            /**/
            else {                                                       /**/
#ifdef debug                                                             /**/
fprintf(stderr,"Ctrl Break found!\n");                                   /**/
#endif                                                                   /**/
                cBreak = 0;                                              /**/
                continue;                                                /**/
            }                                                            /**/
        }                                                                /**/
                                                                         /**/
                                                                         /**/
        if (eh == 0) {                                                   /**/
                                                                         /**/
#ifdef bat_debug                                                         /**/
fprintf(stderr,"main:1:bf:%x  bf->in=%d\n",&(*bf),bf->nest);             /**/
#endif                                                                   /**/
            for(i=0;i<_NARGS;i++)                                        /**/
                arg[i] = NULL;                                           /**/
                                                                         /**/
            if (bf->in) {                                                /**/
                do_bat();                                                /**/
            }                                                            /**/
            else {                                                       /**/
                printprompt();                                           /**/
                na = getcom(com);                                        /**/
#ifdef debug                                                             /**/
    fprintf(stderr,"main:2:You said:/%s/\n",com);                        /**/
    fprintf(stderr,"main:2:You said:/%s/\n",arg[0]);                     /**/
    fprintf(stderr,"main:3:Arguments=%d\n",na);                          /**/
    fprintf(stderr,"main:4:path=%c:\\%s\n",D,P);                         /**/
    fprintf(stderr,"main:5:fout.redirect=%u\n",fout.redirect);           /**/
    fprintf(stderr,"main:5:fout.name(%s)\n",fout.name);                  /**/
    fprintf(stderr,"main:5:fin.redirect=%u\n",fin.redirect);             /**/
    fprintf(stderr,"main:5:fin.name(%s)\n",fin.name);                    /**/
#endif                                                                   /**/
                do_command(na);                                          /**/
                                                                         /**/
            }                                                            /**/
                                                                         /**/
            /* printf("\n"); */                                          /**/
                                                                         /**/
        }                                                                /**/
        else if(Xit==111) {                                              /**/
            do_command(na);                                              /**/
            Xit = 1;                                                     /**/
        }                                                                /**/
                                                                         /**/
#ifdef debug                                                             /**/
fprintf(stderr,"main:7:xit = %u\n",Xit);                                 /**/
#endif                                                                   /**/
                                                                         /**/
        if(Xit ==1 && Xitable==1) do_xx();                               /**/
#ifdef debug                                                             /**/
    fprintf(stderr,"Xit: %d\tXitable: %d\n",Xit,Xitable);                /**/
#endif                                                                   /**/
    }                                                                    /**/
                                                                         /**/
/*******************************.D.O.G. .L.O.O.P****************************/
}

/*****************************
******************************
***       END OF main      ***
******************************
*****************************************************************************/

/*****************************************
******************************************
**                                      **
**        void do_command( BYTE n)      **
**                                      **
******************************************
****************************************************************************/


void do_command( BYTE na)
{
    BYTE i;
#ifdef do_debug
BYTE dbi;
#endif
    if (na==0) return;
    if((strlen(arg[0])==2) || (arg[0][2]=='.') || (arg[0][2]=='\\')) {

        for(i=0;i<_NCOMS;i++) {
#ifdef do_debug
fprintf(stderr,"do_command:1: searching command(%d)=%s\n",i,commands[i]);
fprintf(stderr,"do_command:2: line is: /");
for(dbi=0;dbi<na;dbi++) {
    fprintf(stderr,"%s/",arg[dbi]);
}
fprintf(stderr,"\n");
#endif
            if(strnicmp(arg[0],commands[i],2) == 0) {
                break;
            }
        }
#ifdef do_debug
   fprintf(stderr,"\n");
#endif
        switch(i) {
    
        case C_BR :
            do_br(na);
            break;
        
        case C_CC :
            
            break;
        
        case C_CD :
            do_cd(na);
            break;
        
        case C_CL :
            do_cl();
            break;
        
        case C_CP :
        
            break;
        
        case C_CT :
            
            break;

        case C_EH :
            do_eh(na);
            break;
        
        case C_HH :
            printf("The commands are:\n");
              for(i=0;i<_NCOMS;i++) {
                  printf("%s ",commands[i]);
            }
            printf("\n");
            break;
        
        case C_LS :
            do_ls(na);
            break;
        
        case C_MD :
            do_mrd(na);
            break;
        
        case C_NN :
            do_nn(na);
            break;
        
        case C_RD :
            do_mrd(na);
            break;
        
        case C_RM :
            do_rm(na);
            break;
        
        case C_SE :
            do_se(na);
            break;
        
        case C_SZ :
            do_sz(na);
            break;

        case C_TP :
            do_tp(na);
            break;
        
        case C_VF :
            do_vf(na);
            break;
        
        case C_VR :
            do_vr();
            break;
        
        case C_XX :
            if(Xitable==1)
                do_xx();
            break;
        default :
            if((arg[0][1]==':') && (arg[0][2] == '\0')) {
                do_chdr(arg[0][0]);
                return;
            }
            else if((arg[0][0] == '.') && (arg[0][1] == '.')) {
                arg[1] = arg[0];
                arg[0] = commands[C_CD];
                do_cd(2);
                return;
            }
            else if((arg[0][0] =='*') || (arg[0][0]=='?'))
                return;
            else {
                do_exe(na);
                D = getcur(P) +'A';
                return;
            }

             /*
             printf("Bad command or filename.\n");
             */
        }
        return;
    }

    else if ((strlen(arg[0])==0) || (strcmp(arg[0],".")==0) || (arg[0][0] =='*') || (arg[0][0]=='?')) {
        printf("");
        return;
    }
    else if(strcmp(arg[0],"\\")==0) {
        arg[1] = arg[0];
        arg[0] = commands[C_CD];
        do_mrd(2);
        return;
    }

    else if (strlen(arg[0])!=2) {
            do_exe(na);
            D = getcur(P) +'A';
            return;


    }
}

/****************************************************************************/


BYTE cbreak(BYTE s)
{
    asm{
        mov AH,33h
        mov AL,s
        cmp AL,0FFh
        je     cbreak_query
        mov DL,AL
        mov AL,01h
        int 21h
    }
    return 0;
    cbreak_query:
    asm{
        mov AL,00h
        int 21h
        mov s,DL
    }
    return s;
}

/****************************************************************************/

void do_br(BYTE n)
{

    BYTE f;

    if(n==1) {

        f = cbreak(0xff);
        printf("BReak is ");
        switch(f) {
            case 0 :
                    puts("OFF");
                    break;
            case 1 :
                    puts("ON");
                    break;
       }
    }
    else if(n==2) {

        if (stricmp(arg[1],"ON")==0) {
            f=cbreak(1);
        }
        else if (stricmp(arg[1],"OFF")==0) {
            f=cbreak(0);
        }
        else {
            puts("You MUST specify either ON or OFF.");
            return;
        }
    }
    else {
        puts("Invalid number of arguments.");
    }
    return;
}

/**************************************************************************/

void do_vf(BYTE n)
{

    if (n==1) {
        printf("Verify is ");
        asm{
            MOV ah,54h  /*  Get verify flag */
            INT 21h
            MOV dl,01h
            CMP al,dl
            JE  on
            }
        puts("OFF");
        return;
        on:
        puts("ON");
        return;
    }
    if (n==2) {
        if (stricmp(arg[1],"ON")==0) {
            asm {
                MOV ah,2eh  /*Set Verify flag*/
                MOV al,01h  /* to on*/
                INT 21
            }
        }
        else if (stricmp(arg[1],"OFF")==0) {
            asm {
                MOV ah,2eh  /* Set Verify flag */
                MOV al,00h  /* to off */
                INT 21
            }
        }
        else {
            puts("You MUST specify either ON or OFF.");
            return;
        }

    }
    if ((n!=1) && (n!=2)) {
        puts("Invalid number of arguments.");
        return;
    }
}


/**************************************************************************/

void do_cl(void)
{
    asm{
            MOV ax,0600h
            MOV bh,07h
            MOV cx,0000h        /* (0,0) to*/
            MOV dx,5079h        /* (50,79) */
            INT 10h             /*Clear screen*/
            MOV ah,02h
            MOV bh,00h
            MOV dx,0000h        /* (0,0)*/
            INT 10h             /*Move cursor to upper left corner*/
        }
}
/**************************************************************************/

void do_sz(BYTE n) /*** ADD total disk sz + free on disk ***/
{
    struct ffblk *fb;
    BYTE b,i,j,str[5];
    WORD max,mbx,mcx,mdx;
    DWORD h,s,d,t;

    if (n > 2) {
        puts("Invalid number of arguments.");
        return;
    }
    str [0] = '\0';

    i=0; j=0;
    s=0; d=0;
    h=0;

    fb = malloc(sizeof(struct ffblk));

    asm{
        mov AH,36h
        xor dl,dl
        int 21h
        mov max,AX
        mov mbx,BX
        mov mcx,CX
        MOV mdx,DX
    }

    d = (DWORD)max*(DWORD)mbx*(DWORD)mdx; 
    t = (DWORD)max*(DWORD)mcx*(DWORD)mdx;

    b = findfirst("*.*",fb,FA_NORMAL|FA_HIDDEN|FA_SYSTEM);
    if (b!=18) {
        if (((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN)){
            h += fb->ff_fsize;
            j++;
        }
        else if (((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM)){
            h += fb->ff_fsize;
            j++;
        }

        else {
            s += fb->ff_fsize;
            i++;
        }
    }
    b = findnext(fb);
    while (b!=18) {
        if(cBreak) {
            cBreak = 0;
            return;
        }
        if (((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN)) {
            h += fb->ff_fsize;
            j++;
        }

        else if (((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM)){
            h += fb->ff_fsize;
            j++;
        }

        else {
            s += fb->ff_fsize;
            i++;
        }
        b = findnext(fb);
    }

    if (strnicmp(arg[1],"-k",2)==0) {
        h /= 1024;
        s /= 1024;
        d /= 1024;
        t /= 1024;
        strcpy(str,"kilo");
    }

    if (strnicmp(arg[1],"-m",2)==0) {
        h /= 10264576;
        s /= 10264576;
        d /= 10264576;
        t /= 10264576;
        strcpy(str,"mega");
    }

    printf("\n%11lu %sbytes in %d file(s)\n%11lu %sbytes in %d hidden file(s)\n",s,str,i,h,str,j);
    printf("%11lu %sbytes free on drive %c:\n%11lu %sbytes total diskspace\n",d,str,D,t,str);
    free(fb);
    return;
}

/**************************************************************************/

void do_vr(void)
{
    WORD w;
    BYTE b,DOS_ma=3,DOS_mi=30,DOS_OEM=0xFD;

    asm {
        MOV ax,3000h
        INT 21h
        MOV DOS_ma ,al
        MOV DOS_mi ,ah
        MOV DOS_OEM,bh
        MOV b,bl
        MOV w,cx
    }
    switch(DOS_ma) {  /*versions 1,2 & 4 NOT supported */
        case 3 :
            printf("DOS version %u.%u\nDOG version %u.%u",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
            break;
        case 5 :
            asm{
                mov ax,3306h
                int 21h
                cmp bh,50
                je vr_NT
                jmp vr_DOS
            }
            vr_NT:
            printf("DOS Command Prompt under Windows NT\nDog version %u.%u\n",DOG_ma,DOG_mi);
            break;
        case 6 :
        case 7 :
        vr_DOS:
            switch(DOS_OEM) {
                case  0x00:
                    if((DOS_mi ==0) && (DOS_ma==6)) DOS_mi = 1;
                    printf("PC-DOS version %u.%u\nDog version %u.%u\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
                    break;
                case  0xEE:
                    printf("DR DOS version 5.0 OR 6.0 (pretends to be MS-DOS %u.%u)\nDog version %u.%u\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
                    break;
                case  0xEF:
                    printf("Novell DOS 7\nDog version %u.%u\n",DOG_ma,DOG_mi);
                    break;
                case  0xFD:
                    printf("FreeDOS version %u.%u\nDog version %u.%u\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
                    if(b == 0xff)
                        printf("FreeDOS Kernel (build 1993 or prior)\n");
                    else
                        printf("FreeDOS Kernel version %u.%u.%u\n",b,w >> 8,w & 0xff);
                    break;
                case  0xFF:
                    printf("M$-DOS version %u.%u\nDog version %u.%u\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
                    break;
                default :
                    printf("MS-DOS version %u.%u\nDog version %u.%u\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
            }
            break;
/*
        case 7 :
            if (DOS_mi==10)
                printf("Microsoft Windows 95 OSR2\nDog version %u.%u\n",DOG_ma,DOG_mi);
            else
                printf("Microsoft Windows 95\nDog version %u.%u\n",DOG_ma,DOG_mi);
            break;
*/
        case 10 :
            printf("OS/2 version 1.%u\nDog version %u.%u\n",DOS_mi/10,DOG_ma,DOG_mi);
            break;
        case 20 :
            if (DOS_mi < 30)
                printf("OS/2 version %u.%u\nDog version %u.%u\n",DOS_ma/10,DOS_mi/10,DOG_ma,DOG_mi);
            else if(DOS_mi==30)
                printf("OS/2 Warp 3.0 virtual DOS machine\nDog version %u.%u\n",DOG_ma,DOG_mi);
            else if(DOS_mi==40)
                printf("OS/2 Warp 4.0 virtual DOS machine\nDog version %u.%u\n",DOG_ma,DOG_mi);
            break;
        default :
            printf("Unknown DOS version %u.%u\nDog version %u.%u\n",DOS_ma,DOS_mi,DOG_ma,DOG_mi);
    }
}

/**************************************************************************/

void do_se( BYTE n)
{
    BYTE b,*p;
    WORD w;

    if(n == 1) {
        printf("env @ %Fp %u(%xh) bytes\n",_env,envsz,envsz);
        for(w=0;w<envsz;w++) {
            if(*(_env+w)== 0) {
                if(*(_env+w+1) == 0)
                    break;
                else
                    puts("");
            }
            else
                printf("%Fc",*(_env+w));
        
        }
        printf("\n\n");
    }
    else {
        p = malloc(200);
        p[0] = '\0';
        for(b=2;b<n;b++) {
            strcat(p,arg[b]);
            strcat(p," ");
        }
        if (strlen(p) == 0) p = NULL;
        setevar(arg[1],p);
        free(p);
    }
    
}


/**************************************************************************/

void do_tp( BYTE n)
{
    FILE *fp;
    struct ffblk *fb;
    BYTE i,r,buff[256];

    if(n == 1) {
        fprintf(stderr,"Missing filename.\n");
        return;
    }
    else if(n >1) {
    fb = malloc(sizeof(struct ffblk));
        for(i=1;i<n;i++) {
            r = findfirst(arg[i],fb,FA_NORMAL);
            if(r==0) {
                printf("\n---------------%s---------------\n\n",fb->ff_name);
                fp = fopen(fb->ff_name,"r");
                if (fp == NULL) {
                    fprintf(stderr,"\n****Error opening file\n");
                }
                else {
                    while(fgets(buff,255,fp)) {
                        if(cBreak) {
                            cBreak = 0;
                            printf("\n");
                            fclose(fp);
                            return;
                        }
/*                        printf("%s",buff);*/
                        write(1, buff, strlen(buff)+1);
                    }
                    fclose(fp);
                }
            }
            else if(r == 3) {
                fprintf(stderr,"\n****File not found: %s (Invalid path)\n",arg[i]);
                continue;
            }
            else if(r == 4) {
                fprintf(stderr,"\n****File not found: %s (Too many open files)\n",arg[i]);
                continue;
            }
            else if(r == 18) {
                fprintf(stderr,"\n****File not found: %s\n",arg[i]);
                continue;
            }
            else {
                fprintf(stderr,"\n****File not found: %s (Unknown error(%d))\n",arg[i],i);
                continue;
            }
            r = findnext(fb);
            while(r==0) {
                printf("\n---------------%s---------------\n\n",fb->ff_name);
                fp = fopen(fb->ff_name,"r");
                if (fp == NULL) {
                    fprintf(stderr,"\n****Error opening file\n");
                }
                else {
                    while(fgets(buff,sizeof(buff),fp) != 0) {
                        if(cBreak) {
                            cBreak = 0;
                            printf("\n");
                            fclose(fp);
                            return;
                        }
                        printf("%s\n",buff);
                    }
                    fclose(fp);
                }
                r = findnext(fb);
            }
            if(r == 3) {
                fprintf(stderr,"\n****File not found: %s (Invalid path)\n",fb->ff_name);
            }
            else if(r == 4) {
                fprintf(stderr,"\n****File not found: %s (Too many open files)\n",arg[i]);
            }
            else if(r == 18) {
            }
            else {
                fprintf(stderr,"\n****File not found: %s (Unknown error(%d))\n",arg[i],i);
            }
        }

    }
    free(fb);
    return;
}

/**************************************************************************/

WORD newname(BYTE *oldname, BYTE *newname)
{
    WORD r;
    asm{
        mov ah,56h
        push cs
        pop es
        mov dx,oldname
        mov di,newname
        int 21h
        jnc  nn_ok
        mov r,ax
    }
    return r;
    nn_ok:
    return 0;
}

/**************************************************************************/

void do_nn(BYTE n)
{
    BYTE *p, wild=0, b, fn[129]={0}, dir[80],nn[129]={0},on[129]={0};
    WORD r;
    struct ffblk ffb,*fb=&ffb;

    if(n != 3) {
        puts("Invalid number of arguments.");
        return;
    }
    
    b = findfirst(arg[1],fb,0);

    while (b != 18) {
    
        p = arg[1];
        
        /* separate path from filename */
        for(p+=strlen(arg[1])+1;*p!='\\';p--);
        
        if(p>arg[1]) {
            *(++p)=0;
            strcpy(fn,arg[1]);
            strcat(fn,fb->ff_name);
        }
        else
            strcpy(fn,fb->ff_name);
        
        if(wild == 1) {
            if(trueName(nn,arg[2]) == NULL) {
                printf("Malformed path:\n%s\n",arg[2]);
                /* break */
            }
                
            if(trueName(on,fn) == NULL) {
                printf("Malformed path:\n%s\n",arg[1]);
                /* break */
            }
            
            for(b=0;nn[b] != '\0';b++) {
                if(nn[b] == '?')
                    nn[b]=on[b];
            }
        }
        puts(on); puts(nn);
        r = newname(on,nn);
    
        if(r != 0) {
            switch(r) {
                case 02:
                    printf("%s NOT found.\n",on);
                    break;
                case 03: 
                    puts("Path not found.");
                    break;
                case 05: 
                    puts("Access denied.");
                    break;
                case 11: 
                    puts("Invalid format.");
                    break;
            }
        }
    
        b = findnext(fb);
    }
    return;

}
/**************************************************************************/

void do_rm(BYTE n)
{
    BYTE r,f,i,*p,fn[129]={0},dir[80];
    struct ffblk *fb;

    if(n > 1) {
        fb = malloc(sizeof(struct ffblk));

        for(i=1;i<n;i++) {
            if ((p=strstr(arg[i],"*.*"))!=NULL) {
                printf("Removing %s - Are you sure(Y/N)? ",arg[i]);
                /* Get character */
                asm{
                    mov ah,1
                    int 21h
                    mov r,al
                }
                putchar('\n');
                putchar('\r');
            
                /* get to next arg if not 'Y' or 'y' */
                if((r != 'y') && (r != 'Y'))
                    break;
            }
            
            f=findfirst(arg[i],fb,0);

            if(f!=18) {

                if(cBreak) {
                    cBreak = 0;
                    return;
                }

                p = arg[i];
                for(p+=strlen(arg[i])+1;*p!='\\';p--);
                if(p>arg[i]) {
                    *(++p)=0;
                    strcpy(fn,arg[i]);
                    strcat(fn,fb->ff_name);
                }
                else
                    strcpy(fn,fb->ff_name);
                    
                r=unlink(fn);

                if(r==0) {
                    puts(fn);
                }
            }
            else if(f==18) {
                printf("%s NOT found.\n",arg[i]);
            }
            r=findnext(fb);
            while(r==0) {
                if(cBreak) {
                    cBreak = 0;
                    return;
                }

                for(p+=strlen(arg[i])+1;*p!='\\';p--);
                if(p>arg[i]) {
                    *(++p)=0;
                    strcpy(fn,arg[i]);
                    strcat(fn,fb->ff_name);
                }
                else
                    strcpy(fn,fb->ff_name);

                r=unlink(fn);
                
                if(r==0) {
                    puts(fn);
                }
                r=findnext(fb);
            }
        
        }
        free(fb);
    }
        
    else puts("Invalid number of arguments.");  
    return;
}

/**************************************************************************/

void do_eh( BYTE n)
{
    BYTE i,j;

    for(i=1;i<n;i++) {
        for(j=0;j < strlen(arg[i]);j++) {
            if(arg[i][j] == '%') {
                switch(arg[i][++j]) {
                    case '%' :
                        putchar('%');
                        break;
                    case  '_':
                        putchar(' ');
                        break;
                    case  'b':
                        putchar('|');
                        break;
                    case  'e':
                        putchar('\x1b');
                        break;
                    case  'g':
                        putchar('>');
                        break;
                    case  'h':
                        putchar('\b');
                        break;
                    case  'l':
                        putchar('<');
                        break;
                    case  'n':
                        putchar('\n');
                        break;
                    case  'r':
                        putchar('\r');
                        break;
                    case  't':
                        putchar('\t');
                        break;
                    default :
                        putchar(arg[i][j]);

                }
            }
            else
                putchar(arg[i][j]);

        }
        printf(" ");
    }
    printf("\n");
}

/**************************************************************************/

void do_cd( BYTE n)
{
    BYTE i,j,k,dir[80],*p,*q;

    j=strlen(arg[0]);


    /* checking if user typed cd..(something)  OR cd\path*/

    if(j>2) {
        arg[1] = &arg[0][2];
        arg[0][1] = '\0';

        n++;
    }
        /* for every dot(.) after .. change to \.. */


    p = arg[1];
    q = dir;

    for(i=0;i<80;i++) {
        dir[i]=0;
    }

    while(*p!=0) {
        if(*p=='.'){
            for(i=0;*p=='.';i++) {
                *(q++) = *(p++);
            }
            q--;
            if(i>2){
                i-=2;
                for(j=0;j<i;j++) {
                    *(q--) = '\0';
                }
                for(j=0;j<i;j++) {
                    strcat(dir,"\\..");
                    q+=3;
                }
            }
            q++;
        }
        else {
            *(q++)=*(p++);
        }
    }
    *q='\0';
    strcpy(arg[1],dir);

    /* NO arguments provided.. so print current dir */

    if(n==1) {
        D = getcur(P) + 'A';
        if (D >= 'A')
            printf("%c:\\%s\n",D,P);
        return;
    }
    else {
        do_mrd(n);
        return;
    }

}

/**************************************************************************/

void do_mrd(BYTE n)
{
    BYTE f;

    if (n == 1) {
        puts("Required argument missing");
        return;
    }
    if (n > 2) {
        puts("Invalid number of arguments.");
        return;
    }

    f = arg[0][0];        /* check the first letter to determine what func*/
    asm {
        mov AL,f
        cmp AL,43h        /* C */
        je    do_mrd_cd
        cmp AL,4dh        /* M */
        je    do_mrd_md
        cmp AL,52h        /* R */
        je    do_mrd_rd
        cmp AL,63h        /* c */
        je    do_mrd_cd
        cmp AL,6dh        /* m */
        je    do_mrd_md
        cmp AL,72h        /* r */
        je    do_mrd_rd
    }
    do_mrd_md:            /* mkdir */
    asm {
        mov AH,39h
        jmp do_mrd_doit
    }
    do_mrd_rd:            /* rmdir */
    asm {
        mov AH,3ah
        jmp do_mrd_doit
    }
    do_mrd_cd:            /* chdir */
    asm {
        mov AH,3bh
    }
    do_mrd_doit:
    asm {
        mov DX,arg[2]    /*=arg[1] = first arg after command */
        int 21h
        jnc    do_mrd_OK
        cmp AL,03h
        je    do_mrd_pnf
        cmp AL,05h
        je    do_mrd_ad
        cmp AL,06h
        je    do_mrd_ih
        jmp do_mrd_rcd
    }
    do_mrd_pnf:
    puts("Invalid path.");
    return;
    do_mrd_ad:
    puts("Access denied.");
    return;
    do_mrd_ih:
    puts("Invalid handle.");
    return;
    do_mrd_rcd:
    puts("Attempted to remove current directory.");
    do_mrd_OK:
    return;

}

/**************************************************************************/

/*************************************
**************************************
**                                  **
**       void do_cp( BYTE n)        **
**                                  **
**************************************
****************************************************************************/

#pragma argsused
void do_cp( BYTE n)
{
    printf("Sorry NOT implemented yet. Use xcopy.\n");
/*
    FILE *src,*trg;
    BYTE i,*buff;
    struct ffblk ffb;

    if(findfirst(arg[2],ffb,0+1+2+4+8)) {
        fprintf(stderr,"Replace %s (Yes/No)? ");
        i = getchar();
        if((i == 'n') || (i == 'N'))
            return;
    }
    i = findfirst(arg[1],ffb,0);
        if (i == 18)
            fprintf(stderr,"File %s NOT found",arg[1]);

    src = fopen(arg[1],"br");
    trg = fopen(arg[2],"bw");
    if(src ==  NULL) {
        fprintf(stderr,"File %s NOT found",arg[1]);
        return;
    }
    if(trg ==  NULL) {
        fprintf(stderr,"Error while creating file %s",arg[2]);
        return;
    }




*/
}

/*************************************
**************************************
**                                  **
**             void do_ls( BYTE n)        **
**                                  **
**************************************
****************************************************************************/

void do_ls( BYTE n)
{

    int  attrs;
    BYTE *patt[20],dironly;
    BYTE any[4];
    BYTE t,r,i,l,j,k,*p;
    BYTE doit;
    WORD h, min, sec, y, m, d;
    struct ffblk *fb;

    dironly = 0;
    doit = 1;
    strcpy(any,"*.*");
    p = any;
    patt[0] = p;
    r = 0;
    fb = malloc(sizeof(struct ffblk));

#ifdef ls_debug
fprintf(stderr,"do_ls:0: patt[0]=%s; n=%d; any=%s; doit=%u;\n",patt[0],n,any,doit);
#endif

    attrs = 0|FA_DIREC;
    l = 1;
    k = 0;

    if (n>0) {
        for(i=1;i<n;i++) {

#ifdef ls_debug
fprintf(stderr,"do_ls:1: patt[0]=%s\n",patt[0]);
#endif
            if (arg[i][0] == '-') {
                switch(arg[i][1]) {
                    case 'a':
                        attrs = FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN|FA_DIREC|FA_LABEL;
                        break;
                    case 'd':
                        dironly = 1;
                        break;
                    case 'f':
                        attrs = FA_NORMAL;
                        break;
                    case 'h':
                        printf("The switches are:\n\t-a: List ALL files and directories\n");
                        printf("\t-d: list DIRECTORIES only\n\t-f: list normal FILES only\n\t-l: show volume label\n");
                        free(fb);
                        return;
                    case 'l':
                        attrs = FA_LABEL;
                        break;
                    case '?':
                        printf("ls [-a|-d|-f|-h|-l|-?] [d:\\dir\\filename.ext]\n");
                        free(fb);
                        return;
                    default:
                        printf("Incorrect switch %s\n",arg[i]);
                        free(fb);
                        return;
                }
            }
            else if(strnicmp(arg[i],"/",1)==0) {
                puts("In DOG we use '-' as switch NOT '/'.");
                free(fb);
                return;
            }
            else {
                patt[k++] = arg[i];
#ifdef ls_debug
printf("do_ls:2: patt[0]=%s hxpatt=%x\n",patt[0],patt[0]);
#endif
            }
        }
    }

    if (k > 0)
        (l = k);
    else
        (l = 1);

    for(j=0;j < l;j++) {
        if(doit>0) {

            if(l == 0)
              patt[0] = any;

#ifdef ls_debug
fprintf(stderr,"do_ls:3: patt[%u]=%s offset=%x patt[%d][last] =  %c\n",j,patt[0],patt[0],j,patt[j][strlen(patt[j])-1]);
#endif
            if((patt[j][strlen(patt[j])-1] == '\\') || (patt[j][strlen(patt[j])-1] == ':'))
                strcat(patt[j],any);

            r = findfirst(patt[j], fb,attrs);

            if((dironly == 1) && ((fb->ff_attrib & FA_DIREC) != FA_DIREC))
                r = 1;

            if (r == 0) {
                if(cBreak) {
                    cBreak = 0;
                    free(fb);
                    return;
                }

                printf("    size   ASRHD   dd.mm.yyyy   hh:mm.ss   name\n\n");

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


            }

#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\ndate:%d time:%d\n",r,fb->ff_fdate,fb->ff_ftime);
fprintf(stderr,"y=%d y=%d\n",y,1998);

#endif
            k = 0;
            r = findnext (fb);

            while(r != 18) {

            if(cBreak) {
                cBreak = 0;
                free(fb);
                return;
            }

#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\n",r);
#endif

                if((dironly == 1) && (k == 1))
                    r = findnext(fb);

                if((dironly == 1) && ((fb->ff_attrib & FA_DIREC) != FA_DIREC)){
                    k = 1;
                    continue;
                }

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


                y = ((fb->ff_fdate & 0x0fe00)>>9)+1980;

                d = (fb->ff_fdate &  0x001f);

                printf("   %02d.%02d.%04d   ",d,m,y);

                h = (fb->ff_ftime & 0x0F800)>>11;
                min = (fb->ff_ftime & 0x03E0)>>5;
                sec = (fb->ff_ftime & 0x001f)<<1; /* *2 */

                printf("%02d:%02d.%02d   ",h,min,sec);


                printf("%s\n",fb->ff_name);



            r = findnext(fb);
            k = 0;

        }


#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\n",r);
#endif
        }
    }
    free(fb);
    return;
}


/*************************************
**************************************
**                                  **
**        void do_exe(BYTE n)       **
**                                  **
**************************************
****************************************************************************/


void do_exe(BYTE n)
{

    BYTE d,i,*p,*q,*r;
    BYTE s[200],  com[80],exe[80],dog[80],path[60],trunam[128];
    struct ffblk *fb;

    for (i=0;i<200;i++) {
        s[i] = '\0';
    }
    fb = malloc(sizeof(struct ffblk));

    for (i=0;i<12;i++) {
        com[i] = '\0';
        exe[i] = '\0';
        dog[i] = '\0';
    }

    /* Make s contain ALL of the args */
    for (i=1;i<n;i++) {
        strcat(s,arg[i]);
        strcat(s," ");
    }

    strcpy(com, arg[0]); /* append .com .exe .dog to filename and then search */
    strcpy(exe, arg[0]);
    strcpy(dog, arg[0]);
    strcat(com,".COM");
    strcat(exe,".EXE");
    strcat(dog,".DOG");

    i = findfirst(com,fb,0);
    if(i==3){
        puts("Invalid path.");
        free(fb);
        return;
    }
    else if (i==FNF) {
        i = findfirst(exe,fb,0);
        if(i==FNF) {
            i = findfirst(dog,fb,0);
/*
            if((i!=0) && (i!=18)) {
                fprintf(stderr,"ERROR - %u.\n",i);
                    return;
            }
            else
*/
            if(i==0) {
                bf->na = n;
                bf->line = 0;
                for(i=0;i<_NARGS;i++) {
                    bf->args[i] = malloc(strlen(arg[i]+1));
                    memset(bf->args[i],0,strlen(arg[i]+1));
                    strcpy(bf->args[i],arg[i]);
                }
                d = getcur(path) + 'A';
                sprintf(bf->name,"%c:\\%s\\%s",d,path,dog);
                bf->in = 1;
                do_bat();
                free(fb);
                return;
            }
            else {

                /* try to find arg[0] file */
            /*    strupr(arg[0]);                */


                i = findfirst(arg[0],fb,0x3f); /*ANY attrib = 00111111 */
                if(i == 0) {

                    if(cBreak) {
                        cBreak = 0;
                        return;
                    }

                    if((fb->ff_attrib & FA_DIREC) == FA_DIREC) {
                        arg[1] = arg[0];
                        arg[0] = commands[C_CD];
                        do_cd(2);
                        free(fb);
                        return;
                    }

                    p = strstr(arg[0],".COM");
                    q = strstr(arg[0],".EXE");
                    r = strstr(arg[0],".DOG");


                    if(p == NULL) {
                        if(q == NULL) {
                            if(r == NULL) {
                                fprintf(stderr,"File is NOT executable!\n");
                                return;
                            }
                            else {
                                bf->na = n;
                                bf->line = 0;
                                for(i=0;i<_NARGS;i++) {
                                    bf->args[i] = malloc(strlen(arg[i]+1));
                                    memset(bf->args[i],0,strlen(arg[i]+1));
                                    strcpy(bf->args[i],arg[i]);
                                }
                                d = getcur(path) + 'A';
                                sprintf(bf->name,"%c:\\%s\\%s",d,path,arg[0]);
                                bf->in = 1;
                                do_bat();
                                free(fb);
                                return;
                            }
                        }
                        else
                            errorlevel = my_exe(trueName(arg[0],trunam),s);
                    }
                    else
                        errorlevel = my_exe(trueName(arg[0],trunam),s);
                }
                else if(i==FNF) {
                    if(strcmp(arg[0],"..") == 0) {
                        arg[1] = arg[0];
                        arg[0] = commands[C_CD];
                        do_mrd(2);
                    }
                    else
                        fprintf(stderr,"Bad command or Filename.\n");

                    free(fb);
                    return;
                }
            }
        }
        else {
            errorlevel=my_exe(trueName(exe,trunam),s);
        }
    }
    else
        errorlevel=my_exe(trueName(com,trunam),s);

    switch(errorlevel >> 8) {
        case 0:
            if((errorlevel & 0xFF) == 0)
                printf("\n%s exited.\n",trunam);
            else
                printf("Program %s returned %d.\n",trunam,errorlevel & 0xFF);

            break;
        case 1:
            printf("%s aborted by Ctrl-C.\n",trunam);
            break;
        case 2:
            printf("%s: critical error abort.\n",trunam);
            return;
        case 3:
            printf("TSR %s loaded in to memory.\n",trunam);
            return;
        case 77:
            switch(errorlevel & 0xFF) {
                case 0x1:
                    puts("Function number invalid.");
                    break;
                case 0x2:
                    puts("File not found.");
                    break;
                case 0x5:
                    puts("Access denied.");
                    break;
                case 0x8:
                    puts("Insufficient memory.");
                    break;
                case 0xA:
                    puts("Invalid environment.");
                    break;
                case 0xB:
                    puts("Invalid format.");
                    break;
                default :
                    break;
            }
            break;
        default:
            break;
    }

    free(fb);
    return;

}

/***************************************************************************/

WORD my_exe(BYTE *prog, BYTE *args){


    BYTE buf[128];
    WORD rc;
    WORD saveSS, saveSP;
    struct fcb fcb1, fcb2;
    struct ExecBlock execBlock, *eb;

    eb = &execBlock;

/* generate Pascal string from the command line */
    memcpy(&buf[1], args, buf[0] = strlen(args));
    memcpy(&buf[1] + buf[0], "\xd", 2);

/* fill execute structure */
    execBlock.env = 0;
    execBlock.line = (char far *)buf;
    execBlock.fcb1 = (struct fcb far *)&fcb1;
    execBlock.fcb2 = (struct fcb far *)&fcb2;

/* fill FCBs */
    if ((args = parsfnm(args, &fcb1, 1)) != NULL)
          parsfnm(args, &fcb2, 1);

    asm{
        push    si
        push    di
        push    ds

        mov     dx, prog              /* load file name */
        push    ds
        pop     es
        mov     bx, eb                /* load parameter block */
        mov     ax, 4b00h

        mov     Word Ptr cs:[saveSP], sp
        mov     Word Ptr cs:[saveSS], ss
        int     21h
        cli
        mov     ss, Word Ptr cs:[saveSS]
        mov     sp, Word Ptr cs:[saveSP]
        sti

        jc      exec_error  /*if there was an error, the error code is in AX*/
        xor     ax, ax      /*otherwise, clear AX */
        mov     ah,4dh
        int     21h
        jmp     exec_OK
    }
    exec_error:
    asm{
        mov        ah,77h
    }
    exec_OK:
    asm{
        mov     rc,ax
        pop     ds
        pop     di
        pop     si
    }
    return rc;
}



/**************************************************************************/

void do_chdr(BYTE dr)
{
    BYTE d;



    d = toupper(dr) - 'A';

    asm {
        MOV ah,0eh
        MOV dl,d
        INT 21h
        MOV ah,19h
        INT 21h
        CMP al,dl
        JE  chdr_yes
    }
    puts("Invalid Drive");
    chdr_yes:
    return;
}

/**************************************************************************/

void do_xx(void)
{
    WORD w;
    printf("EXITING.....\n");
    
    asm{
        mov ax,offset do_xx
        mov w,ax
    }

    printf("ip:%x",w);
    asm{
        xor ax,ax
        int 21h
    }
}

