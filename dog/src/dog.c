/*
DOG.C  -  Alternate command processor for FreeDOS and others like MS-DOS.

Copyright (C) 1999-2002 Wolf Bergenheim

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

Contact author: internet: wolf@bergenheim.net
                          https://dog.zumppe.net/

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
01.08.00 - rewrote (allmost completely) do_exe to handle the PATH variable. -WB
15.08.00 - Moved BP (Beep) to a normal command out of bat.c - WB
2001-07-17 - Changed the version to be of format X.Y.Z - WB
2001-07-18 - Added variable subtitution to the commandline. $name is
             replaced with the value of the env.var name.
             ${0-9} is the parameter number ${0-9} of the PREVIOUS command. -WB
2001-07-23 - Changed the Variable char to % and the prompt char to $ for
             compatibility. -WB
2002-02-21 - Added support for dod.dog if shell is permanent -WB
2002-03-01 - Internal and external commands are now sepatate. External
             commands are treated no differently than any other commands.
             $e in prompt will expand to the errorlevel. - WB
2002-03-11 - use of aliasreplace() to replace set aliases. I put a maximum
             of 20 iterations, so we won't get unlimited loops. -WB
2002-03-15 - Moved aliasrepace() to the beginning of parsecom(), so that
             aliases will be parsed for in dog-files. Also changed
             parsecom() to use the new evarreplace() function to get env
             vars on the commandline. cleaned initialize to set the default
             env variables with setevar() instead of building it by hand -WB
2002-04-24 - Bugfixes submitted by K.Harikiran:
             * after evareplace() the ll variable needed to be updated to the
               new length of line.
             * Unable to create a temp file in the root dir of Mr Harikiran.
             * Bug in do_exe: prepended CWD to name of DOGfile, without any
               apparent reason.
             -WB
2002-04-26 - More bug fixes submitted by K.Harikiran:
              * -C switch to dog doesn't work. (not fixed yet)
             Changed mktmpfile() so that it will first check for TEMP and
             TMP env vars and use the path specified there. If that fails it
             will use the dir supplied to the function. -WB
2002-04-27 - Fixed -C switch. For some reason the code for the -C switch
             had dropped away. -WB
2002-05-12 - Changed the do_exe() to execute any .COM .EXE .DOG file even if
             a directory with the same name exists
             e.g.: FOO.COM and a directory FOO exists; if you type foo, then
             FOO.COM is executed, if you want the dir FOO you type FOO. -WB
2024-05-12 - Brought back the DOG prompt! Also uses the DOG prompt when no
             PROMPT is defined in the ENV. -WB
2024-05-20 - Fixed init code and setting of the PSP when -P is given.
             Also updated environment and alias blocks setup.
             Fixed calls to int21/AH=25h, to use DS, not ES for the segment. -WB
2024-05-27 - Changed getln to use DOS int21h/ah=0Ah to read input. -WB
*/

#include "dog.h"
#include <mem.h>

static BYTE getln(void);

/* Define Global Variables */
BYTE Xit = 0, Xitable = 1, eh = 0, D=0, P[MAXDIR]={0};
struct linebuffer combuffer;
BYTE *com = NULL, *arg[_NARGS], varg[_NARGS][200], *prompt;
BYTE commands[_NCOMS][3] = {
    "AL",
    "CC",
    "CD",
    "CT",
    "EH",
    "HH",
    "MD",
    "RD",
    "SE",
    "XX"};

BYTE ext_commands[_NECOMS][3] = {
    "BP",
    "BR",
    "CL",
    "CP",
    "LS",
    "MV",
    "RM",
    "SZ",
    "TP",
    "VF",
    "VR"};

BYTE command_des[_NCOMS][21] = {
    "ALias               ",
    "Change Codepage     ",
    "Change Directory    ",
    "Change Terminal     ",
    "EcHo                ",
    "Help                ",
    "Make Directory      ",
    "Remove Directory    ",
    "Set to Environment  ",
    "eXit                "};

BYTE ext_command_des[_NECOMS][21] = {
    "BeeP                ",
    "BReak               ",
    "CLear screen        ",
    "CoPy                ",
    "LiSt files          ",
    "MoVe file (rename)  ",
    "ReMove files        ",
    "SiZe of files in dir",
    "TyPe                ",
    "VeriFy              ",
    "VeRsion             "};

/* BYTE *command_help[_NCOMS]; */

/*
Structure of the flags variable. Tells which flags were given to DOG
3 2 1 0
^ ^ ^ ^
| | | +-- P
| | +---- E
| +------ C
+-------- A
*/
BYTE flags=0;
BYTE DOG_ma = DOG_MA, DOG_mi = DOG_MI, DOG_re = DOG_RE;
WORD PSP=0;
BYTE cBreak = 0, in_getln=0;
WORD my_i22_s, my_i22_o;
WORD my_i23_s, my_i23_o; /* When making a permanent shell take over */
WORD my_i24_s, my_i24_o; /* int 23 and int 24. */
WORD i23_s=0, i23_o=0; /* variables to store old ctrl-c handler*/
WORD i24_s=0, i24_o=0;
WORD i2e_s=0, i2e_o=0;
WORD id0_s=0, id0_o=0; /* the old handler for the D0GFunc */
WORD envseg=0,envsz=512;
WORD aliasseg=0,aliassz=2048;

WORD drvs=3,errorlevel=0;
BYTE far * _env;
BYTE IN,OUT,sin,sout;
BYTE REDIRECTED_OUT, REDIRECTED_IN;

struct bfile *bf;
struct red fout, fin;
struct s_pipe pip;

extern unsigned _psp;
extern unsigned _heaplen;
extern unsigned _stklen;

extern BYTE _osmajor;
extern BYTE _osminor;

void make_permanent(char * argv0);

/* This function is called when the -P flag is set.
   It sets the DOG interrupt handlers and makes DOG its own parent. */
void make_permanent(char * argv0)
{
    BYTE path[200]={0}, *p;
    /* Install the error interrupt vectors in the PSP */
    /* These will be used by child processes, so point to me since we are THE shell */
#ifdef DOG_DEBUG
    printf("make_permanent():0:PSP=%04Xh PPID=%04Xh\n",_psp,peek(_psp,PSP_PPID_OFS));
    printf("make_permanent():0:PSP[i22(%02Xh)]=%04Xh:%04Xh\n",
	   PSP_I22_OFS, peek(_psp,PSP_I22_SEG), peek(_psp,PSP_I22_OFS));
    printf("make_permanent():0:PSP[i23(%02Xh)]=%04Xh:%04Xh\n",
	   PSP_I23_OFS, peek(_psp,PSP_I23_SEG), peek(_psp,PSP_I23_OFS));
    printf("make_permanent():0:PSP[i24(%02Xh)]=%04Xh:%04Xh\n",
	   PSP_I24_OFS, peek(_psp,PSP_I24_SEG), peek(_psp,PSP_I24_OFS));
#endif
    poke(_psp,PSP_I22_OFS,my_i22_o); /* termination address, top of DOG loop */
    poke(_psp,PSP_I22_SEG,my_i22_s);
    poke(_psp,PSP_I23_OFS,my_i23_o); /* Crit C handler */
    poke(_psp,PSP_I23_SEG,my_i23_s);
    poke(_psp,PSP_I24_OFS,my_i24_o); /* Crit Err handler */
    poke(_psp,PSP_I24_SEG,my_i24_s);

#ifdef DOG_DEBUG
    printf("make_permanent():1:PSP[i22(%02Xh)]=%04Xh:%04Xh\n",
	   PSP_I22_OFS, peek(_psp,PSP_I22_SEG), peek(_psp,PSP_I22_OFS));
    printf("make_permanent():1:PSP[i23(%02Xh)]=%04Xh:%04Xh\n",
	   PSP_I23_OFS, peek(_psp,PSP_I23_SEG), peek(_psp,PSP_I23_OFS));
    printf("make_permanent():1:PSP[i24(%02Xh)]=%04Xh:%04Xh\n",
	   PSP_I24_OFS, peek(_psp,PSP_I24_SEG), peek(_psp,PSP_I24_OFS));
#endif

#ifdef DOG_DEBUG
    printf("make_permanent():2:PSP[parent(%02Xh)]=%04Xh\n",
	   PSP_PPID_OFS, peek(_psp,PSP_PPID_OFS));
#endif
    /* make DOG it's own parrent*/
    poke(_psp,PSP_PPID_OFS,_psp);
#ifdef DOG_DEBUG
    printf("make_permanent():3:PSP[parent(%02Xh)]=%04Xh\n",
	   PSP_PPID_OFS, peek(_psp,PSP_PPID_OFS));
#endif

    Xitable = 0;
    setevar("COMSPEC",argv0);

    /* Make sure there is at least a path to the directory from where DOG was run. */
    p = getevar("PATH", path, 200);
    if (p == NULL) {
	for(p = & argv0[strlen(argv0)];*p != '\\';p--);
	*p = '\0';
	setevar("PATH", argv0);
    }
#ifdef DOG_DEBUG
    printf("make_permanent():4:_env=%Fp envz:%p PATH:'%s', p='%s'\n", _env, envsz, path, p);
#endif
}

BYTE initialize(int nargs, char *args[])
{
    BYTE i,j,k,*p,*q,line[200]={0};
    BYTE far *s;
    BYTE far *d;
    BYTE far *ep;
    WORD w,nenvsz=0,nenvseg=0,eoesz,o,naliassz=0;
    BYTE DOS_ma, DOS_mi;

    /* Get DOS Version */
    asm MOV ah,30h;
    asm INT 21h;
    asm MOV DOS_ma, al;
    asm MOV DOS_mi, ah;

    if((DOS_ma < 3) && (DOS_mi < 30)) {
	printf("Sorry your DOS is too lame.\nGet at least version 3.30\n");
	exit(1);
    }

    j = 0;
    Xitable = 1;

    /* save and set int23 (CtrlC) and int24 (Crit Err) handlers */
    save_error_ints();
    set_error_ints();

    /* make int D0 point to D0GFunc */
    /* this should also be int 2fh/ah=d0 */
    set_intd0();

    /* For now don't grab 2e. It's command.com */
#ifdef HAVE_2E
    /* make int 2e point to D0GFunc */
    make_int2e();
#endif

    /* point int 22 termination address at DOG loop*/
    /* and save it to i22_s:i22_o */
    asm mov ax,2522h;
    asm mov dx,offset DOG_loop;
    asm mov my_i22_o,dx;
    asm push cs;
    asm pop ds;
    asm mov my_i22_s,ds;
    asm int 21h;

#ifdef DOG_DEBUG
  printf("initialize():0:PSP = %x PPID = %x\n",_psp,peek(_psp,PSP_PPID_OFS));
#endif
    /* save STDIN STDOUT */

  IN = dup(fileno(stdin));
  OUT = dup(fileno(stdout));

  pip.pstatus = 0;

  D = getcur(P) + 'A';

  bf = malloc(sizeof(struct bfile));
  bf->prev = NULL;
  bf->in = 0;
  bf->nest = 0;

  drvs = 0;

  /* get the segment of the environment from the PSP*/
  envseg = peek(_psp, PSP_ENVSEG_OFS);
  _env = MK_FP(envseg,0);
  envsz = BLOCKSZ(envseg); /*get size of block allocated from MCB*/
  aliassz = 2048;

#ifdef ENV_DEBUG
  printf("initialize():1:nargs=%u\n",nargs);
  printf("initialize():1:envsz=%04Xh\n",envsz);
  printf("initialize():1:aliassz=%04Xh\n",aliassz);
#endif

  if(nargs > 1) {

    /* make the commandline to one string without spaces */
    for(i=2,strcpy(line,args[1]);i<nargs;i++)
    strcat(line,args[i]);

    p=line;
    strupr(p); /* upper case it*/
    while(*p!='\0') {
      if((*p=='-') || (*p=='/')) {
        switch(*(++p)) {
         case 'A': /* set size of alias block */
          flags |= FLAG_A;
          naliassz=0;
          /* Accept all strings beginning with -A */
          while((!isdigit(*p))&&(*p!='\0')&&(*p!='-')) p++;
          while(isdigit(*p)) naliassz=naliassz*10+(*p++)-'0';
#ifdef ENV_DEBUG
          printf("initialize():2:naliassz=04X%h\n",naliassz);
#endif
          break;

         case 'C': /* execute one command, then exit */
          flags |= FLAG_C;
          eh = 1;
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
          break;

         case 'E': /* set the size of the environment */
          flags |= FLAG_E;
          nenvsz=0;
          /* Accept all strings beginning with -E */
          while((!isdigit(*p))&&(*p!='\0')&&(*p!='-')) p++;
          while(isdigit(*p)) nenvsz=nenvsz*10+(*p++)-'0';
          break;

         case 'P':/* make a permanent shell */
          flags |= FLAG_P;
          /* Accept all strings beginning with -P, ignore also numbers */
          while((!isdigit(*p))&&(*p!='\0')&&(*p!='-')) p++;
#ifdef ENV_DEBUG
          printf("initialize():2:envseg=04X%h envsz=04X%h\n",envseg,envsz);
#endif
#if 0
          nenvsz = 0;
          while(isdigit(*p)) nenvsz=nenvsz*10+(*p++)-'0';
#ifdef ENV_DEBUG
          printf("initialize():2:envseg=04X%h envsz=04X%h\n",envseg,envsz);
#endif
#endif
          break;
        }
      }
      else {
        /* ignore */
        p++;
      }
    }
  }

  if ( naliassz == 0 ) {
    naliassz = aliassz;
  }
  /* Allocate a block for alias */
  naliassz = naliassz >> 4; /* para */
  if(naliassz < 0x5) naliassz=0x5;
  if(naliassz > 0x800) naliassz=0x800;
#ifdef ENV_DEBUG
  printf("initialize():3:naliassz=%04Xh para (%d bytes) aliasseg=%04Xh\n", naliassz, naliassz<<4, aliasseg);
#endif
  aliassz = mkudata(0, &aliasseg, 0, naliassz);

#ifdef ENV_DEBUG
  printf("initialize():4:aliassz=%04Xh para (%u bytes) aliasseg=%04Xh\n",aliassz, aliassz<<4,aliasseg);
#endif

  if((flags & FLAG_E ) == FLAG_E) {
    nenvsz = nenvsz >> 4; /* paragraphs */
    envsz = envsz >> 4; /* paragraphs */
    if(nenvsz < 0x5) nenvsz=0x5;
    if(nenvsz > 0x800) nenvsz=0x800;

    nenvseg = 0;
#ifdef ENV_DEBUG
    printf("initialize():5:envsz=%04Xh para (%d bytes) envseg=%04Xh\n",envsz,envsz<<4,envseg);
    printf("initialize():5:nenvsz=%04Xh para (%d bytes) nenvseg=%04Xh\n",nenvsz,nenvsz<<4,nenvseg);
#endif
    nenvsz = mkudata(envseg, &nenvseg, envsz, nenvsz);
#ifdef ENV_DEBUG
    printf("initialize():6:envsz=%04Xh para (%d bytes) envseg=%04Xh\n",envsz,envsz<<4,envseg);
    printf("initialize():6:nenvsz=%04Xh para (%d bytes) nenvseg=%04Xh\n",nenvsz,nenvsz<<4,nenvseg);
#endif

#ifdef ENV_DEBUG
    printf("initialize():7:_psp=%04X\n", _psp);
#endif
    envseg = nenvseg;
    envsz = nenvsz << 4;
    poke(_psp, PSP_ENVSEG_OFS, envseg);
    _env = MK_FP(envseg,0);
#ifdef ENV_DEBUG
    printf("initialize():8:_env=%Fp\n", _env);
#endif
  }
  else {
    _env = MK_FP(envseg,0);
    envsz = BLOCKSZ(envseg); /*get size of block allocated from MCB*/
#ifdef ENV_DEBUG
    printf("initialize():9:_env=%Fp envz:%u\n", _env, envsz);
#endif
  }

  if((flags & FLAG_P) == FLAG_P) {
      make_permanent(args[0]);
  }

  return j;
}



/**************************************************************************/

void buffered_input(struct linebuffer *buffer)
{
#ifdef DOG_DEBUG
    printf("calling int21h/ah=0Ah buffer size=%d buffer=%p\n",
	   buffer->size, buffer->buffer);
#endif
    asm mov dx, buffer;
    asm mov ah, 0Ah;   /* buffered input */
    asm int 21h;
    buffer->buffer[buffer->length] = '\0';
    puts("");
#ifdef DOG_DEBUG
    printf("int21h/ah=0Ah got %d characters = '%s'\n", buffer->length, buffer->buffer);
#endif
    return;  /* buffer is populated */
}

/**************************************************************************/
/*
 * Reads a line of buffered input.
 * Sets com to point at the input buffer.
 */
static BYTE getln(void)
{
  combuffer.size=120; /* leaving futture space , for alias expansion and such*/
  combuffer.length=0;
  com = combuffer.buffer;

  in_getln = 1;
  buffered_input(&combuffer);
  in_getln = 0;

#ifdef DOG_DEBUG
  fprintf(stderr,"getln:2: cb.size: %d cb.length:%d cb.buffer:!%s!\n",
	  combuffer.size, combuffer.length, combuffer.buffer);
#endif

  return combuffer.length;
}

/**************************************************************************/
BYTE parsecom(BYTE * line, BYTE ll)
{
  BYTE i=0,j=0;
  BYTE ename[80],eval[80];
  BYTE *p;

#ifdef PARSE_DEBUG
  printf("parsecom:0-1: line(%s)\n",line);
#endif
    for(i=0;(i<MAX_ALIAS_LOOPS) && (aliasreplace(line) != 0xFF);i++);
#ifdef PARSE_DEBUG
  printf("parsecom:0-2: line(%s)\n",line);
#endif
  ll = strlen(line);
  evarreplace(line,ll);
  ll = strlen(line);
#ifdef PARSE_DEBUG
  printf("parsecom:0-3: line(%s) ll=%d\n",line,ll);
#endif

  i=0;
  while((j<_NARGS) && (i<ll)) {
    while((isspace(line[i]) || (line[i] == '\0') )&& (i<ll)) {
#ifdef PARSE_DEBUG
      printf("parsecom:2-0: %c(%x)[%d]\n",line[i],line[i],i);
#endif
      line[i++] = '\0';
    }
    if(i<ll) {
      arg[j++] = &line[i];
    }

    while(!isspace(line[i]) && (i<ll)) {
#ifdef PARSE_DEBUG
      printf("parsecom:2-1: %c(%x)[%d]\n",line[i],line[i],i);
#endif
			i++;
		}
  }
#ifdef PARSE_DEBUG
  printf("parsecom:3: j=%d line: !%s! arg(varg):",j,line);
  for(i=0;i<j;i++)
  printf("\t%s(%s)\n",arg[i],varg[i]);
  printf("\n");
#endif

  return j;
}



/**************************************************************************/

BYTE getcom(BYTE *com)
{
  BYTE ln,r,i;

  ln = getln();

  if(redir(com)==0) {
    com[0] = 0;
    arg[0] = com;
    return 0;
  }

  ln = strlen(com);
#ifdef DOG_DEBUG
  fprintf(stderr,"getcom:1: com: !%s! ln:%u\n",com,ln);
#endif

  r = parsecom(com,ln);
#ifdef DOG_DEBUG
  fprintf(stderr,"getcom:2: com: !%s! ln:%u\n",com,ln);
#endif

  if(cBreak) {
    cBreak = 0;
    return 0;
  }

#ifdef DOG_DEBUG
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

WORD mktmpfile(char *dir)
{
  WORD w;
  char tmp[128];

  memset(tmp,0,128);
  if (getevar("TEMP",tmp,128) != NULL) {
    strcpy(dir,tmp);
  }
  else if (getevar("TMP",tmp,128) != NULL) {
    strcpy(dir,tmp);
  }

  asm mov ah,5ah ; /* create temp filename*/
  asm mov cx,00h ; /* (00000000 00000010) hidden */
  asm mov dx,dir ; /* DS:DX -> dir in which to create */
  asm int 21h      ;
  asm jc mtf_1 ; /* Error */
  asm mov w,AX ;
  return w;
mtf_1:
  asm mov w,AX ;
  switch(w) {
   case 3:
    fprintf(stderr,"Path not found: %s\n",dir);
    break;
   case 4:
    fprintf(stderr,"Toomany open files.\n");
    break;
   case 5:
    fprintf(stderr,"Access Denied.\n");
   default:
    fprintf(stderr,"Error  %x.\n",w);
  }

  return 0;
}
/**************************************************************************/

BYTE redir(BYTE *c)
{
  BYTE l,i,pl;
  WORD fh;
  BYTE *fo,*fi,*p, *tmpcmd;
  FILE *in,*out;

  l = strlen(c);
  p = c;

  for(i=0;i<l;i++) {
#ifdef REDIR_DEBUG
    printf("redir:0: &p(%x) *p(%c)\n",p,*p);
#endif
    if(*p == '>') {
      *p = '\0';
      strcpy(fout.opt,"w");
      if((*(++p)) == '>') {
        strcat(fout.opt,"a");
        *p = ' '; /*erase the >*/
      }

#ifdef REDIR_DEBUG
      printf("redir:1:c(%s) p(%s) *p(%c)\n",c,p,*p);
#endif
      /* skip spaces and tabs*/
      while(isspace(*(p++)));
			fo=p-1; /*begining of filename*/
      while(isfchar(*(++p)));
      /* p points to first spc AFTER filename*/
#ifdef REDIR_DEBUG
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
#ifdef REDIR_DEBUG
      printf("redir:3:fi(%x) &fi(%x) &p(%x) *p(%x)\n",fi,fi,p,p);
#endif
      while(isfchar(*(++p))) {
#ifdef REDIR_DEBUG
        printf("redir:4:fi(%x) &fi(%x) &p(%x) *p(%x)\n",fi,fi,p,p);
#endif
      }

      /* p points to first spc AFTER filename*/
      if(*(p-1)=='\n')
      *(p-1)='\0';
      *p = '\0';
#ifdef REDIR_DEBUG
      printf("redir:5:fi(%s) &fi(%x) &p-1(%x) &p(%x) &p+1(%x)\n",fi,fi,(p-1),p,(p+1));
#endif
      strcpy(fin.name,fi);
			fin.redirect = 1;
      strcat(c," ");
      strcat(c,p);
    }
    else if(*p == '|') {
      memset(pip.pname,0,MAXDIR+13);
      memset(pip.pcmd,0,200);

      if(pip.pstatus == 1) {
				close(pip.phandle);
        unlink(pip.pname);
        pip.pstatus = 0;
      }

      *p = '\0';
      p++;
      strcpy(pip.pcmd,p); /* add code to get tmp from evar */
      sprintf(pip.pname,"%c:\\%s\\",D,P);

      /***/
#ifdef REDIR_DEBUG
      printf("redir:6:c(%s) p(%s) *p(%c) pip.pname=(%s)\n",c,p,*p,pip.pname);
#endif

      pip.phandle = mktmpfile(pip.pname);

#ifdef REDIR_DEBUG
      printf("redir:6:c(%s) p(%s) *p(%c) pip.pname=(%s)\n",c,p,*p,pip.pname);
#endif

      if (pip.phandle == 0) {
        fprintf(stderr, "Unable to create pipe\n");
        return 0;
      }

#ifdef REDIR_DEBUG
      printf("redir:7:pip.pname=(%s) pip.phandle=(%x)",pip.pname,pip.phandle);
#endif
      /***/

      if(dup2(pip.phandle,fileno(stdout)) != 0) {
#ifdef REDIR_DEBUG
        printf("redir:6.7:errno=%x",errno);
        printf("redir:7:pip.pname=(%s) pip.phandle=(%x)",pip.pname,pip.phandle);
#endif

	fprintf(stderr, "Unable to create pipe\n");
        return 0;
      }
      pip.pstatus=2;
      i = l;
    }
    else {
      p++;
    }

  }
  if(fout.redirect) {
#ifdef REDIR_DEBUG
    printf("redir:8:c(%s) fout.name(%s)\n",c,fout.name);
#endif
    if((fout.fp = fopen(fout.name,fout.opt)) == NULL) {
      fprintf(stderr,"Unable to redirect output to file %s.\n",fout.name);
      fout.redirect=0;
      return 0;
    }

    dup2(fileno(fout.fp),fileno(stdout));
  }

  if(fin.redirect) {
#ifdef REDIR_DEBUG
    printf("redir:9:c(%s) fin.name(%s)\n",c,fin.name);
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

  asm mov ax,1900h; /* Get current drive */
  asm int 21h;
  asm mov d,al;

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
  BYTE dow,mo,day,i,k,h,mi,s,ms,j;
  BYTE ename[80],eval[120];

  prompt = malloc(200);
  if (prompt==NULL) {
      printf("> ");
      return;
  }
  memset(prompt,'\0',200);
  memset(eval,'\0',120);

  if(getevar("PROMPT",prompt,200) != NULL) {
      k = strlen(prompt);
  }
  else {
      /* Make sure there is a prompt. */
      /* Long live the dog promopt! */
      k = sizeof(_DOG_PROMPT);
      memcpy(prompt, _DOG_PROMPT, k);
  }

#ifdef PROMPT_DEBUG
  printf("printprompt:0:prompt=(%s) strlen=(%d)\n",prompt,k);
#endif

  /*  0x01 = path
   0x02 = date
   0x03 = time
   */

  for(i=0;i<k;i++) {
#ifdef PROMPT_DEBUG
  printf("printprompt:1:prompt[%d]=%c\n",i,prompt[i]);
#endif
    if(prompt[i]=='$') {
      switch(prompt[++i]) {
       case '$' :
        putchar('$');
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
	   printf("%d",errorlevel);
	   break;
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
	   asm mov ah,2ah;  /* Get date */
	   asm int 21h;
	   asm mov dow,al;  /*day of week*/
	   asm mov yr,cx;   /*year*/
	   asm mov mo,dh;   /*month*/
	   asm mov day,dl;  /*day*/

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
        printf("%04u-%02u-%02u",yr,mo,day);
        break;
      case  'c':
      case  'T':
	  asm mov ah,2ch;  /* Get time */
	  asm int 21h;
	  asm mov h,ch;    /*hours*/
	  asm mov mi,cl;   /*minutes*/
	  asm mov s,dh;    /*seconds*/
	  asm mov ms,dl;   /*milliseconds or hundreds*/

	  printf("%d.%02d.%02d,%02d",h,mi,s,ms);
        break;
       default :
        break;
      }
    }
    else if(prompt[i] == '%') {
      if(prompt[++i]  == '%') {
	  putchar('%');
      }
      else if(isdigit(prompt[i])) {
	  printf("%s",varg[prompt[i] -'0']);
      }
      else {
	  for(j=0;(prompt[i] != '%') && (i<k);j++,i++) {
	      ename[j] = prompt[i];
#ifdef PROMPT_DEBUG
	      printf("printprompt:2:prompt[%d]=%c\n",i,prompt[i]);
#endif
	  }
#ifdef PROMPT_DEBUG
	printf("printprompt:3:prompt[%d]=%c\n",i,prompt[i]);
#endif
        ename[j]= '\0';
        getevar(ename,eval,80); /* replace %varname% with value*/
/*				eval[strlen(eval)-2;*/
#ifdef PROMPT_DEBUG
	printf("printprompt:4:eval=(%s\b)\n",eval);
#endif
        printf("%s\b",eval);
      }
/*			i++; */
    }
    else {
	putchar(prompt[i]);
    }
  }

#ifdef PROMPT_DEBUG
  printf("printprompt:5:prompt=(%s) length=%d\n",prompt,k);
#endif

  free(prompt);
  return;
}

void set_psp(void)
{
  WORD psp=0;

  /* fist set _psp to our PSP by using int 21h/62h (documented) */
  asm mov ah, 62h; /* Get PID / PSP */
  asm int 21h;
  asm mov psp, bx; /* save psp */
  _psp = psp;

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
  BYTE i,na;
  com = combuffer.buffer;

#ifdef DOG_DEBUG
  printf("_psp: %04Xh _heaplen: %04Xh _stklen: %04Xh DOS: %u.%u\n", _psp, _heaplen, _stklen, _osmajor, _osminor);
  printf("Coreleft() = %lu bytes\n",  coreleft());
  printf("PSP:%04Xh _heaplen: %04Xh _stklen: %04Xh\n", _psp, _heaplen, _stklen);
  printf("_psp: %04Xh _heaplen: %04Xh _stklen: %04Xh DOS: %u.%u\n", _psp, _heaplen, _stklen, _osmajor, _osminor);
#endif

  na = initialize(nargs, argv);

#ifdef DOG_DEBUG
  printf("_psp: %04Xh _heaplen: %04Xh _stklen: %04Xh DOS: %u.%u\n", _psp, _heaplen, _stklen, _osmajor, _osminor);
#endif

  if((flags & FLAG_P) == FLAG_P) {
      arg[0] = "c:\\dog.dog";
      do_command(1);
  }

  if (eh == 0) {
    printf("DOG - Dog Operating Ground Version %u.%u.%02x\n",DOG_ma,DOG_mi,DOG_re);
    printf("      Copyright (C) Wolf Bergenheim 1997-2024\n\n");
    printf("Type HH for Help\n\n");
  }

  /*******************************.D.O.G. .L.O.O.P****************************/

  for(EVER) {
      asm DOG_loop:
#ifdef BAT_DEBUG
      fprintf(stderr,"main:0 (DOG LOOP):bf:%x  bf->in=%d\n",&(*bf),bf->in);
#endif

    if(fout.redirect) {
#ifdef REDIR_DEBUG
      fprintf(stderr,"main:0:closing handle %x\n",fileno(fout.fp));
#endif
      dup2(OUT,fileno(stdout));
      close(fileno(fout.fp));
      fout.redirect = 0;
    }

    if(fin.redirect) {
#ifdef REDIR_DEBUG
      fprintf(stderr,"main:0:closing handle %x\n",fileno(fin.fp));
#endif
      dup2(IN,fileno(stdin));
      close(fileno(fin.fp));
      fin.redirect = 0;
    }

    if(pip.pstatus == 2) {
      pip.pstatus = 1;
      dup2(OUT,fileno(stdout));
      close(pip.phandle);
      pip.phandle = open(pip.pname,O_RDONLY);
      if(pip.phandle > 0x70) {
        perror("");
        pip.pstatus = 0;
        close(pip.phandle);
        unlink(pip.pname);
        goto pipe_end;
      }
      if(dup2(pip.phandle,fileno(stdin)) != 0) {
        printf("Error while reading from file %s.\n",pip.pname);
        pip.pstatus = 0;
        close(pip.phandle);
        unlink(pip.pname);
      }

      pipe_end:
      ;
    }

    for(i=0;i<MAXDIR;i++) {
	P[i] = 0;
    }
    D = getcur(P) + 'A';

    /* Check for cBreak                                               **/
    if (bat_check_cbreak()) {
#ifdef BAT_DEBUG
	printf("%s:%d C-Break!\n",__FILE__, __LINE__);
	fprintf(stderr,"main:C-b:bf:%x  bf->in=%d\n",&(*bf),bf->in);
#endif
	continue;
    }

    if (eh == 0) {

#ifdef BAT_DEBUG
      fprintf(stderr,"main:1:bf:%x  bf->in=%d\n",&(*bf),bf->in);
#endif
      if (bf->in == 1) {
#ifdef BAT_DEBUG
	  fprintf(stderr,"main:2:before do_bat()  bf->in=%d\n",&(*bf),bf->in);
#endif
	  do_bat();
#ifdef BAT_DEBUG
	  fprintf(stderr,"main:3:after do_bat()  bf->in=%d\n",&(*bf),bf->in);
#endif
      }
      else if(pip.pstatus == 1) {
        strcpy(com,pip.pcmd);
        if(redir(com)==0) {
          com[0] = 0;
          arg[0] = com;
        }

        na = parsecom(com, strlen(com));
        do_command(na);
        dup2(IN,fileno(stdin));
        close(pip.phandle);
        unlink(pip.pname);
        pip.pstatus = 0;

      }
      else {
        printprompt();
        na = getcom(com);
#ifdef DOG_DEBUG
        fprintf(stderr,"main:2.a:You said:/%s/\n",com);
        fprintf(stderr,"main:2.b:You said:/%s/\n",arg[0]);
        fprintf(stderr,"main:3:Arguments=%d\n",na);
        fprintf(stderr,"main:4:path=%c:\\%s\n",D,P);
        fprintf(stderr,"main:5:fout.redirect=%u\n",fout.redirect);
        fprintf(stderr,"main:5:fout.name(%s)\n",fout.name);
        fprintf(stderr,"main:5:fin.redirect=%u\n",fin.redirect);
        fprintf(stderr,"main:5:fin.name(%s)\n",fin.name);
        fprintf(stderr,"main:5:pip.pstatus=%u\n",pip.pstatus);
        fprintf(stderr,"main:5:pip.pname(%s)\n",pip.pname);
#endif
        do_command(na);

      }

      /* printf("\n"); */

    }
    else if( (flags & FLAG_C ) == FLAG_C ) {
      if ( (flags & FLAG_P ) == FLAG_P ) {
        Xit = 0;
        eh = 0;
#ifdef DOG_DEBUG
        fprintf(stderr,"main:6-0:Xit = %u\n",Xit);
        fprintf(stderr,"main:6-1:flags = %x\n",flags);
#endif
      }
      else {
        Xit = 1;
#ifdef DOG_DEBUG
        fprintf(stderr,"main:6-2:Xit = %u\n",Xit);
        fprintf(stderr,"main:6-3:flags = %x\n",flags);
#endif
      }
#ifdef DOG_DEBUG
      fprintf(stderr,"main:6-4:Xit = %u\n",Xit);
      fprintf(stderr,"main:6-5:flags = %x\n",flags);
#endif

      do_command(na);
    }

#ifdef DOG_DEBUG
    fprintf(stderr,"main:7:Xit = %u\n",Xit);
    fprintf(stderr,"main:7:flags = %x\n",flags);
    fprintf(stderr,"main:7:P-flag = %x\n",(flags & FLAG_P ) == FLAG_P);
#endif


    for(i=0;i<na;i++) {
      strcpy(varg[i],arg[i]);
    }
    for(;i<_NARGS;i++) {
      arg[i] = NULL;
      varg[i][0] = '\0';
    }

    if((Xit == 1) && ( (flags & FLAG_P ) != FLAG_P )) break;
  }

  /*******************************.D.O.G. .L.O.O.P****************************/

  restore_intd0();
  restore_error_ints();

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

#ifdef DO_DEBUG
  BYTE dbi;
#endif
  if (na==0) return;
  if ((arg[0][0] == ':') ||  (arg[0][0] == '#')) return;
  if ((strlen(arg[0])==2) || (arg[0][2]=='.') || (arg[0][2]=='\\')) {

    for(i=0;i<_NCOMS;i++) {
#ifdef DO_DEBUG
      fprintf(stderr,"do_command:1: searching command(%d)=%s\n",i,commands[i]);
      fprintf(stderr,"do_command:2: line is: ");
      for(dbi=0;dbi<na;dbi++) {
        fprintf(stderr,",%s",arg[dbi]);
      }
      fprintf(stderr,",\n");
#endif
      if(strnicmp(arg[0],commands[i],2) == 0) {
        break;
      }
    }
#ifdef DO_DEBUG
    fprintf(stderr,"\n");
#endif
    switch(i) {

		 case C_AL :
			do_al(na);
			break;

     case C_CC :
      do_cc(na);
      break;

     case C_CD :
      do_cd(na);
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

     case C_MD :
      do_mrd(na);
      break;

     case C_RD :
      do_mrd(na);
      break;

     case C_SE :
      do_se(na);
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
      else if((strncmp(arg[0],"...",3) == 0) || (strcmp(arg[0],"..") == 0)) {
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

/* internal commands are built as separate objects and linked */
/*
#include "alse.c"
#include "cc.c"
#include "cmrd.c"
#include "ct.c"
#include "eh.c"
#include "hh.c"
#include "xx.c"
*/


/*************************************
 **************************************
 **                                  **
 **        void do_exe(BYTE n)       **
 **                                  **
 **************************************
 ****************************************************************************/


void do_exe(BYTE n)
{

  BYTE d,i,ic,ie,id,*p,*q,*r,*cpath,envi[255],file[128],exec_f, xd;
  BYTE s[200], com[80],exe[80],dog[80],path[60],trunam[128],prog[120];
  struct ffblk *fb;

	exec_f = NON;

  xd = 0;

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
  strcpy(prog,arg[0]);
#ifdef EXE_DEBUG
  printf("do_exe:0:prog=%s\n",prog);
#endif
  i = findfirst(prog,fb,0x37); /*attrib = 00100111 */
  if (i == 0) {
#ifdef EXE_DEBUG
    printf("do_exe:1:prog=%s\n",prog);
    printf("do_exe:2:%s a directory\n",((fb->ff_attrib & FA_DIREC) == FA_DIREC)?"Is":"Isn't");
    printf("do_exe:3:ff_attrib = 0x%x\n",fb->ff_attrib);
#endif
    if((fb->ff_attrib & FA_DIREC) == FA_DIREC) {
      xd = 1;
    }

    if(strstr(fb->ff_name,".COM") == NULL) {
      if (strstr(fb->ff_name,".EXE") == NULL) {
        if (strstr(fb->ff_name,".DOG") == NULL) {
#ifdef EXE_DEBUG
          printf("do_exe:4:ff_name = %s\n",fb->ff_name);
#endif
          exec_f = NON;
        }
        else {
          exec_f = DOG;
          strcpy(dog,prog);
#ifdef EXE_DEBUG
          printf("do_exe:5:ff_name = %s\n",fb->ff_name);
#endif
        }
      }
      else {
        exec_f = EXE;
        strcpy(exe,prog);
#ifdef EXE_DEBUG
        printf("do_exe:6:ff_name = %s\n",fb->ff_name);
#endif
      }
    }
    else {
      exec_f = COM;
      strcpy(com,prog);
#ifdef EXE_DEBUG
      printf("do_exe:7:ff_name = %s\n",fb->ff_name);
#endif
    }
  }

  if (exec_f == NON) {
		if(prog[1] == ':') {
			strcpy(file,prog);
			goto do_exe_enp;
		}
		if(prog[0] == '\\') {
			strcpy(file,prog);
			goto do_exe_enp;
		}

		getevar("PATH",envi,255);
		p = malloc(255);
		strcpy(p,".;");
		strncat(p,envi,253);
		strncpy(envi,p,255);
    free(p);
#ifdef EXE_DEBUG
		printf("do_exe:8:PATH=%s\n",envi);
#endif
		for(cpath=strtok(envi,";");;cpath=strtok(NULL,";")) {
			if(cpath == NULL) break;
#ifdef EXE_DEBUG
			printf("do_exe:9:dir=%s\n",cpath);
#endif
			strcpy(file,cpath);
			if(file[strlen(file)-1] != '\\') {
				strcat(file,"\\");
			}
			strcat(file,arg[0]);
			do_exe_enp:
			strcpy(com,file);
			strcpy(exe,file);
			strcpy(dog,file);
			strcat(com,".COM");
			strcat(exe,".EXE");
			strcat(dog,".DOG");
#ifdef EXE_DEBUG
			printf("do_exe:10:file=%s\n",file);
			printf("do_exe:11:com=%s\n",com);
			printf("do_exe:12:exe=%s\n",exe);
			printf("do_exe:13:dog=%s\n",dog);
#endif
			if(findfirst(com,fb,0x27) == 0) {
#ifdef EXE_DEBUG
				printf("do_exe:11:com=%s\n",com);
				printf("\ndo_exe:14:file=%s\n",fb->ff_name);
#endif
				exec_f = COM;
				break;
			}
			else if (findfirst(exe,fb,0x27) == 0) {
#ifdef EXE_DEBUG
				printf("\ndo_exe:15:file=%s\n",fb->ff_name);
#endif
				exec_f = EXE;
				break;
			}
			else if (findfirst(dog,fb,0x27) == 0) {
#ifdef EXE_DEBUG
				printf("\ndo_exe:16:file=%s\n",fb->ff_name);
#endif
				exec_f = DOG;
				break;
			}
			else if(findfirst(file,fb,0x27) == 0) {
#ifdef EXE_DEBUG
				printf("\ndo_exe:17:file=%s\n",fb->ff_name);
#endif
				if(strstr(fb->ff_name,".COM") == NULL) {
					if (strstr(fb->ff_name,".EXE") == NULL) {
						if (strstr(fb->ff_name,".DOG") == NULL) {
							exec_f = NON;
						}
						else {
            strcpy(dog,file);
							exec_f = DOG;
						}
					}
					else {
						strcpy(exe,file);
						exec_f = EXE;
					}
				}
				else {
					strcpy(com,file);
					exec_f = COM;
				}
				break;
			}

			else {
				exec_f = NON;
			}

#ifdef EXE_DEBUG
			printf("do_exe:18:exec_f = %u\n",exec_f);
#endif

		}
  }
  exec_now:

#ifdef EXE_DEBUG
  printf("do_exe:19:exec_f = %u\n",exec_f);
  printf("do_exe:20:file=%s\n",file);
  printf("do_exe:21:com=%s\n",com);
  printf("do_exe:22:exe=%s\n",exe);
  printf("do_exe:23:dog=%s\n",dog);
#endif

  switch(exec_f) {
   case NON:
    if (xd == 1) {
      arg[1] = prog;
      arg[0] = commands[C_CD];
      do_cd(2);
      free(fb);
      return;
    }
    printf("%s: Bad command or Filename\n",arg[0]);
    free(fb);
    return;
   case COM:
#ifdef EXE_DEBUG
    printf("do_exe:24:found %s in %s\n",fb->ff_name,cpath);
#endif
    errorlevel=my_exe(trueName(com,trunam),s);
    break;
   case EXE:
#ifdef EXE_DEBUG
    printf("do_exe:25:found %s in %s -> %s\n",fb->ff_name,cpath,exe);
#endif
    errorlevel=my_exe(trueName(exe,trunam),s);
    break;
   case DOG:
#ifdef EXE_DEBUG
    printf("do_exe:26:found %s in %s ->%s\n",fb->ff_name,cpath,dog);
		printf("do_exe:27:bf->args[i] = %x\n",bf->args);
#endif
    bf->na = n;
    bf->line = 0;
    memcpy(bf->cline, com, 200);
    for(i=0;i<_NARGS;i++) {
      if(arg[i] != NULL) {
	  /* Transfer the pointers with same offsets.
	     What could possibly go wrong? >_<
	   */
	  bf->args[i] = bf->cline + (arg[i] - com);
#ifdef BAT_DEBUG
        printf("do_exe:28:bf->args[%u](%x) =  bf->cline(%x) + (arg[%u](%x) - com(%x))\n",
	       i, bf->args[i], bf->cline, i, arg[i], com);
	printf("do_exe:29:bf->args[i] = %s\n",bf->args[i]);
#endif
      }
#ifdef BAT_DEBUG
      else {
	  printf("do_exe:30:arg[%u] == 0\n",i);
      }
#endif
    }
#ifdef EXE_DEBUG
    printf("do_exe:31:bf->name= %s\n",dog);
#endif
    sprintf(bf->name,"%s",dog);
    bf->in = 1;
    do_bat();
    free(fb);
    return;
  }

  switch(errorlevel >> 8) {
   case 0:
    if((errorlevel & 0xFF) != 0)
      printf("Program %s returned %d.\n",trunam,errorlevel & 0xFF);
    break;
   case 1:
    printf("\n%s aborted by Ctrl-C.\n",trunam);
    break;
   case 2:
    printf("\n%s: critical error abort.\n",trunam);
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
      puts("Unknown unexpected program termination.");
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

BYTE *trueName(BYTE *name,BYTE *tn)
{
    asm push ds;
    asm pop es;
    asm mov si,name;
    asm mov di,tn;
    asm mov ah,60h;  /* Get TrueName - cannonicalize file name */
    asm int 21h;
    asm jnc tn_ok;

    return NULL;
tn_ok:
    return tn;

}

/***************************************************************************/

WORD my_exe(BYTE *prog, BYTE *args){


    BYTE buf[128], strat;
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
  if ((args = parsfnm(args, &fcb1, 1)) != NULL) {
      parsfnm(args, &fcb2, 1);
  }

#ifdef EXE_DEBUG
  asm mov ax, 5800h ; /* Get memory alloc strat */
  asm int 21h;
  asm mov strat, al ; /* memory allocation strategy */

  switch(strat) {
  case 0x00:
      printf("my_exe():1:Memory alloc strat: low memory first fit\n");
      break;
  case 0x01:
      printf("my_exe():1:Memory alloc strat: low memory best fit\n");
      break;
  case 0x02:
      printf("my_exe():1:Memory alloc strat: low memory last fit\n");
      break;
  case 0x40:
      printf("my_exe():1:Memory alloc strat: high memory first fit\n");
      break;
  case 0x41:
      printf("my_exe():1:Memory alloc strat: high memory best fit\n");
      break;
  case 0x42:
      printf("my_exe():1:Memory alloc strat: high memory last fit\n");
      break;
  case 0x80:
      printf("my_exe():1:Memory alloc strat: first fit, try high then low\n");
      break;
  case 0x81:
      printf("my_exe():1:Memory alloc strat: best fit, try high then low\n");
      break;
  case 0x82:
      printf("my_exe():1:Memory alloc strat: last fit, try high then low\n");
      break;
  default:
      printf("my_exe():1:Memory alloc strat: unknown (%02Xh)", strat);
      break;
  }

  asm mov bl, 01; /* force low memory only first fit strat */
  asm xor bh,bh;
  asm mov ax, 5801h;
  asm int 21h;

#endif

  asm push    si;
  asm push    di;
  asm push    ds;

  asm MOV     dx, prog;             /* load file name */
  asm push    ds;
  asm pop     es;
  asm MOV     bx, eb;                /* load parameter block */
  asm MOV     ax, 4b00h;             /* exec - load and execute */

  asm MOV     word PTR cs:[saveSP], sp;
  asm MOV     word PTR cs:[saveSS], ss;
  asm INT     21h;
  asm cli;
  asm MOV     ss, word PTR cs:[saveSS];
  asm MOV     sp, word PTR cs:[saveSP];
  asm sti;

  asm jc      exec_error;  /* if there was an error, the error code is in AX*/
  asm xor     ax, ax;      /* otherwise, clear AX */
  asm mov     ah,4dh;      /* Get return code */
  asm int     21h;
  asm jmp     exec_OK;

exec_error:

  asm mov     ah,77h;

exec_OK:

  asm mov     rc,ax;
  asm pop     ds;
  asm pop     di;
  asm pop     si;

#ifdef EXE_DEBUG
  printf("my_exe():2:exec: %s - return: %04Xh\n", prog, rc);

  asm mov bl, strat;
  asm xor bh,bh;
  asm mov ax, 5801h;

#endif
  return rc;
}



/**************************************************************************/

void do_chdr(BYTE dr)
{
  BYTE d;

  d = toupper(dr) - 'A';

  asm mov ah,0eh;  /* set drive */
  asm mov dl,d;
  asm int 21h;
  asm mov ah,19h;  /* get current drive */
  asm int 21h;
  asm cmp al,dl;
  asm je  chdr_yes;

  puts("Invalid Drive");
  chdr_yes:
  return;
}
