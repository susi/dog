/*
DOG.C  -  Alternate command processor for (currently) MS-DOS ver 3.30

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

Contact author: internet: dog@users.sourceforge.net
                         http://www.hut.fi/~hbergenh/DOG/
			 
Developers:
Wolf Bergenheim (WB)
Eugene Wong (EW)

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
18.03.00 - Split DOG.C int individual files. - WB           
25.03.00 - added code for do_ct() - WB
02.04.00 - fixed printprompt so that month will be displayed properly and
           changed its variable names to be more consistent with dt.c - EW
13.04.00 - rewrote initialize(). now both XX and SE work. As does the 
           -E command-line switch - WB
01.08.00 - rewrote (allmost completely) do_exe to handle the PATH variable.

*/

#include "dog.h"
#include "bat.c"
#include "ints.c"

/*
#include "dog.h"
#include "bat.c"
#include "ints.c"
*/

BYTE initialize(int nargs, char *args[])
{

    BYTE i,j,k,*p,*q,line[200]={0},sw_P=0,rebuild=0;
    BYTE far *s;
    BYTE far *d;
    BYTE far *ep;
    WORD w,nenvsz,nenvseg,eoesz,o;


    Xitable = 1;

printf("PSP = %x PPID = %x\n",_psp,peek(_psp,PPID_OFS));

    /* save STDIN STDOUT */
    IN = dup(fileno(stdin));
    OUT = dup(fileno(stdout));

    D = getcur(P) + 'A';

    bf = malloc(sizeof(struct bfile));
    bf->prev = NULL;
    bf->in = 0;
    bf->nest = 0;

    drvs = 0;

    /* get the segment of the environment from the PSP*/
    envseg = peek(_psp,ENVSEG_OFS);
    _env = MK_FP(envseg,0);    
    envsz = peek(envseg-1,3) << 4; /*get size of block allocated from MCB*/

#ifdef env_debug
printf("nargs=%u\n",nargs);
printf("envsz=%ux\n",envsz);
#endif

    if(nargs == 1) {
        return 0;
    }

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
                    if(sw_P == 1)
                        Xit = 3;
                    else
                        Xit = 2;
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
                    return j;

                case 'E': /* set the size of the environment */

                    nenvsz=0;
                    /* Accept all strings beginning with -E */
                    while((!isdigit(*p))&&(*p!='\0')&&(*p!='-')) p++;
                    while(isdigit(*p)) nenvsz=nenvsz*10+(*p++)-'0';
                    
                    nenvsz >>= 4; /* paragraphs */
                    if(nenvsz < 10) nenvsz=10;
                    if(nenvsz > 800) nenvsz=800;

                    if((nenvsz << 4) < envsz) {
                        asm {
                            mov AH,4ah      ;/*resize mem block BX=newsz ES=seg */
                            mov BX,nenvsz
                            mov DX,envseg
                            mov ES,DX
                            int 21h
                            jnc e_env_ok
                        }
                        e_env_not_ok:
                        asm {
                            sub ax,07h /*errors 7,8,9 possible*/
                            jz  e_block_dest
                            dec ax
                            jz  e_no_mem
                            jmp e_inv_mem
                        }
                        /* probably never happens... remove? */
                        e_block_dest:
                        fprintf(stderr,"Environment block destroyed!\nBuilding new.\n");
                        e_inv_mem:
                        fprintf(stderr,"Invalid Environment!\nBuilding new.\n");
                        e_no_mem:
                        fprintf(stderr,"Environment block too small!\nBuilding new.\n");
                        asm {
                            mov ah,49h
                            mov dx,envseg
                            mov es,dx
                            int 21h
                        }
                        rebuild = 1;
    
                        break;
                        e_env_ok:
                        
                        nenvsz <<= 4; /* bytes */
                        eoesz = strlen(args[0]) + 4;
                        ep = MK_FP(envseg,0); /* point to beg of env*/
#ifdef env_debug
printf("ep=%Fp\n",ep);
#endif

                        for(w = 0;(eoesz+w) < nenvsz;w++) {
                            if(*(ep+w) == 0){
                                o = w; /* save pos */
#ifdef env_debug
printf("w=%u; ",w);
#endif

                            }
                        }
                        ep += o;
#ifdef env_debug
printf("ep=%Fp\n",ep);
#endif

                        *(++ep) = 0;
                        *(++ep) = 1;
                        *(++ep) = 0;
                        q=args[0]; ep++;
                        while(*q != 0)
                            *(ep++)=*(q++);                        
                        *ep = 0;

                    }
                    else {
                        asm {
                            mov ah,48h
                            mov bx,nenvsz
                            int 21h
                            jc  l_e_not_ok
                            mov nenvseg,ax
                        }

                        goto l_e_ok;
                        l_e_not_ok:
                        goto e_env_not_ok;
                        l_e_ok:

                        s = MK_FP(envseg,0);
                        d = MK_FP(nenvseg,0);

#ifdef env_debug
printf("envseg=%x envsz=%x\n",envseg,envsz);
#endif
                        for(w=0;w<envsz;w++) {
                            *(d++) = *(s++);
#ifdef env_debug
printf("s(%Fp)->%Fc d(%Fp)->%Fc\n",s,*s,d,*d);
#endif
                        }

                        asm {
                            mov ah,49h      ;/*free old env*/
                            mov dx,envseg
                            mov es,dx
                            int 21h
                        }
                        envsz = nenvsz;
                        envseg = nenvseg;
                        poke(_psp,ENVSEG_OFS,envseg);

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

                    get_int();
                    set_int();

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
    envsz = peek(envseg-1,3) << 4; /*get size of block allocated from MCB*/

    if (rebuild==1) {
        if((p=malloc(envsz))!=NULL) {
            q=p;
            i=0;
            sprintf(p,"COMSPEC=%s",args[0]);
            i += strlen(p)+1;
            p += i; /*point to after terminating 0 */
            strcpy(p,"PATH=C:\\DOS;C:\\DOG;C:\\;..");
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

            for(j=0;j<i;j++) { /* copy it to the env block */
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


void printprompt(void)
{
    WORD yr;
    BYTE dow,mo,day,i,k,h,mi,s,ms;

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
                        MOV mo,DH     /*month*/
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
                      MOV h,ch     /*hours*/
                      MOV mi,cl     /*minutes*/
                      MOV s,dh     /*seconds*/
                      MOV ms,dl     /*milliseconds or hundreds*/
                  }
                  printf("%d.%02d.%02d,%02d",h,mi,s,ms);
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

    na = initialize(nargs, argv);

    if((_osmajor < 3) && (_osminor < 30)) {
        printf("Sorry your DOS is too lame.\nGet at least version 3.30\n");
        exit(1);
    }


    if (eh == 0) {
        printf("DOG Ä Dog Operating Ground Version %u.%02u\n",DOG_ma,DOG_mi);
        printf("      Copyright (C) Wolf Bergenheim 1997-2000\n\n");
        printf("Type HH for Help\n\n");
    }

/*******************************.D.O.G. .L.O.O.P****************************/
                                                                         /**/
    for(EVER) {                                                          /**/
        asm {                                                            /**/
            DOG_loop:                                                    /**/
        }                                                                /**/
                                                                         /**/
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
        else if(Xit==2) {                                                /**/
            do_command(na);                                              /**/
            Xit = 0;                                                     /**/
            eh = 0;                                                      /**/
        }                                                                /**/
                                                                         /**/
        else if(Xit==3) {                                                /**/
            do_command(na);                                              /**/
            Xit = 1;                                                     /**/
        }                                                                /**/
                                                                         /**/
#ifdef debug                                                             /**/
fprintf(stderr,"main:7:xit = %u\n",Xit);                                 /**/
#endif                                                                   /**/
                                                                         /**/
        if(Xit ==1 && Xitable==1) break;                                 /**/
#ifdef debug                                                             /**/
    fprintf(stderr,"Xit: %d\tXitable: %d\n",Xit,Xitable);                /**/
#endif                                                                   /**/
    }                                                                    /**/
                                                                         /**/
/*******************************.D.O.G. .L.O.O.P****************************/

    return 0;

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
            do_ct(na);
            break;

        case C_EH :
            do_eh(na);
            break;
        
        case C_HH :
            do_hh(na);
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

/*************************************
**************************************
**                                  **
**        INTERNAL COMMANDS         **
**                                  **
**************************************
*************************************/

#include "br.c"
#include "cc.c"
#include "cmrd.c"
#include "cl.c"
#include "cp.c"
#include "ct.c"
#include "eh.c"
#include "hh.c"
#include "ls.c"
#include "nn.c"
#include "rm.c"
#include "se.c"
#include "sz.c"
#include "tp.c"
#include "vf.c"
#include "vr.c"
#include "xx.c"

/*************************************
**************************************
**                                  **
**        void do_exe(BYTE n)       **
**                                  **
**************************************
****************************************************************************/


void do_exe(BYTE n)
{

    BYTE d,i,ic,ie,id,*p,*q,*r,*cpath,envi[255],file[128],exec_f;
    BYTE s[200], com[80],exe[80],dog[80],path[60],trunam[128];
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

    /*First try to exec arg[0]*/

    i = findfirst(arg[0],fb,0x3f); /*ANY attrib = 00111111 */
    if (i == 0) {
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
            if (q == NULL) {
                if (r == NULL) {
                    exec_f = NON;
                }
                else {
                    exec_f = DOG;
                }
            }
            else {
                exec_f = EXE;
            }
        }
        else {
            exec_f = COM;
        }

        if (exec_f != NON) goto exec_now;
    }

    getevar("PATH",envi);
    p = malloc(255);
    strcpy(p,".;");
    strcat(p,envi);
    strcpy(envi,p);
    free(p);
#ifdef exe_debug
    printf("do_exe:PATH=%s\n",envi);
#endif     
    for(cpath=strtok(envi,";");;cpath=strtok(NULL,";")) {
        if(cpath == NULL) break;
#ifdef exe_debug
        printf("do_exe:dir=%s\n",cpath);
#endif
        strcpy(file,cpath);
        if(file[strlen(file)-1] != '\\')
            strcat(file,"\\");
        strcat(file,arg[0]);
        strcpy(com,file);
        strcpy(exe,file);
        strcpy(dog,file);
        strcat(com,".COM");
        strcat(exe,".EXE");
        strcat(dog,".DOG");
#ifdef exe_debug
        printf("do_exe:file=%s\n",file);
        printf("do_exe:com=%s\n",com);
        printf("do_exe:exe=%s\n",exe);
        printf("do_exe:dog=%s\n",dog);
#endif
        ic = findfirst(com,fb,0);
        ie = findfirst(exe,fb,0);
        id = findfirst(dog,fb,0);
#ifdef exe_debug
printf("ic = %u ie = %u id = %u\n",ic,ie,id);
#endif
        if(ic == 0) {
            exec_f = COM;
            break;
        }
        else if (ie == 0) {
            exec_f = EXE;
            break;
        }
        else if (ie == 0) {
            exec_f = DOG;
            break;
        }
        else {
            exec_f = NON;
        }

    }

    exec_now:
        
    switch(exec_f) {
        case NON:
            printf("%s: Bad command or Filename\n",arg[0]);
            free(fb);
            return;
        case COM:
#ifdef exe_debug
printf("do_exe:found %s in %s\n",fb->ff_name,cpath);
#endif
            errorlevel=my_exe(trueName(com,trunam),s);
            break;
        case EXE:
#ifdef exe_debug
printf("do_exe:found %s in %s\n",fb->ff_name,cpath);
#endif
        errorlevel=my_exe(trueName(exe,trunam),s);
            break;
        case DOG:
#ifdef exe_debug
printf("do_exe:found %s in %s\n",fb->ff_name,cpath);
#endif
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

