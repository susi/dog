/* 
DOG.H   -  Alternate command processor for (currently) MS-DOS ver 3.30

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
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include <ctype.h>
#include <alloc.h>
#include <io.h>
#include <process.h>
#include <errno.h>
#include <fcntl.h>

#define de bug
#define list
#define do_de bug
#define ls_de	bug
#define exe_de bug
#define bat_de bug
#define cd_de bug
#define eh_de bug
#define ca_de bug
#define parse_de bug
#define prompt_de bug

#define BYTE unsigned char
#define WORD unsigned int
#define DWORD unsigned long

/*used by do_exe*/
#define NON 0
#define COM 1
#define EXE 2
#define DOG 4

/* we want to use the ones in dos.h. not in dir.h NOT
#define findfirst(A,B,C) _dos_findfirst(A,C,B)
#define findnext _dos_findnext
#define ffblk find_t
#define ff_attrib attrib
#define ff_ftime  wr_time
#define ff_fdate  wr_date
#define ff_fsize  size 
#define ff_name	  name
*/

#ifndef FA_NORMAL
#define FA_NORMAL 0x0
#endif

#ifdef MK_FP
#undef MK_FP
#endif
#define MK_FP(seg,ofs) ((void far*)(((unsigned long)(seg) << 16) | (ofs)))

#define EVER ;;


#define FNF 2

#define _BS 0x8;
#define _TAB 0x9;
#define _SPC 0x32;
#define _PROMPT "$l$p$g$_"
#define _DOG_PROMPT "$n$_$_$b\\_$n$_$_$b$_.\\---.$n$_/$_$_$_,__/$n/$_$_$_/$p$_$_"
#define _DOG_PROMPT2 "$_$_$_$__$n$_$_$_/$_\\__$n$_$_(_'$_$_@\\___$n$_$_/$_$_$_$_$_$_$_$_$_O$n$_/$_$_$_(_____/$n/_____/$_$_$_U$n$p$t$t"
#define _COW_PROMPT "$t$_$_(__)$n$t$_$_~..~$n$_$_/-------(..)$n$_/$_|$_$_$_$_$_||$n*$_$_||----||$n$_$_$_^^$_$_$_$_^^$n$p$g"
#define _COW_PROMPT2 "$t$_$_(__)$n$t$_$_(oo)$n$_$_/-------(..)$n$_/$_|$_$_$_$_$_||$n*$_$_||W---||$n$_$_$_^^$_$_$_$_^^$n$p$g"

/*
  
  |\_
  | .\---.
 /   ,__/
/   /C:\DOG  _

    _
   / \__     
  (_'  @\___       
  /         O      
 /   (_____/
/     /   U 
c:\DOG _


          (__)
          ~..~
  /-------(..)
 / |     ||
*  ||----||
   ^^    ^^
C:\DOG>_

          (__)
          (oo)
  /-------(..)
 / |     ||
*  ||----||
   ^^    ^^
C:\DOG>_

          (__)
          (””)
  /-------(..)
 / |     ||
*  ||----||
   ^^    ^^

          (__)
          (**)
  /-------(..)
 / |     ||
*  ||----||
   ^^    ^^

          (__)
          (oo)
  /-------(..)
 / |     ||
*  ||----||
   ^^    ^^

          (__)
          ( oo
  /-------/\_|
 / |     ||
*  ||----||
   ^^    ^^

          (__)
          ~..~
  /-------(..)
 / |     ||
*  ||----||
   ^^    ^^

*/


#define _NCOMS 9
#define _NECOMS 11
#define _NARGS 20

/* PSP */

#define PPID_OFS 0x0016
#define ENVSEG_OFS 0x002C
#define DOG_VER 082bh
#define DOG_MA 0
#define DOG_MI 8
#define DOG_RE 0x2b

/* Define Global Variables */
BYTE DOG_ma = DOG_MA, DOG_mi = DOG_MI, DOG_re = DOG_RE;
BYTE Xit = 0, Xitable = 1, eh = 0, D=0, P[MAXDIR]; 
BYTE comline[200]= {0}, *com = comline, *arg[_NARGS], varg[_NARGS][200], *prompt;
BYTE commands[_NCOMS][3] = {
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
	"VerFy               ",
	"VeRsion             "};

/* BYTE *command_help[_NCOMS]; */

/*
Structure of the flags variable. Tells which flags were given to DOG
3 2 1 0
^ ^ ^ ^
| | | +-- P
| | +---- E
| +------ C
+-------- 0
*/
BYTE flags;
#define FLAG_P 0x1
#define FLAG_E 0x2
#define FLAG_C 0x4
#define FLAG_A 0x8

#define	C_CC 0
#define	C_CD 1
#define	C_CT 2 
#define C_EH 3 
#define C_HH 4 
#define C_MD 5
#define C_RD 6
#define C_SE 7
#define C_XX 8



BYTE cBreak = 0;
WORD drvs,errorlevel=0,PSP;
WORD envseg,envsz;
WORD aliasseg,aliassz;
BYTE far * _env;
WORD i22_s, i22_o;
WORD i23_s, i23_o; /* variables to store old ctrl-c handler*/
WORD i2e_s, i2e_o;
WORD i24_s, i24_o;
WORD id0_s, id0_o; /* the old handler for the D0GFunc */
BYTE IN,OUT,sin,sout;
BYTE REDIRECTED_OUT, REDIRECTED_IN;

struct bfile {
    BYTE name[MAXDIR];
    BYTE *args[20];
	BYTE cline[200];
 	BYTE com[200];
    BYTE na;
    BYTE in;       /* used to determine if we are in a dog-file*/
    WORD line;
    WORD nest;
    struct bfile *prev;
}*bf;

struct red {
    BYTE name[MAXDIR];
    FILE *fp;
    BYTE redirect;
    BYTE opt[3];
}fout, fin;

struct s_pipe {
	WORD phandle;
	BYTE pname[MAXDIR+13];
	BYTE pcmd[200];
	BYTE pstatus; /*0=nopipes active 1=tmp file pending delete 2= active */
}pip;

struct ExecBlock {
	WORD env;
	BYTE far *line;
	struct fcb far *fcb1, far *fcb2;
};


/*
struct pipe{
    BYTE line[200];
    BYTE pipe;
}pip;
*/

/* Function prototypes */

/* startup and utility */
void printprompt(void);
BYTE initialize(int nargs, char* argv[]);
BYTE parsecom(BYTE * line, BYTE ll); 
BYTE getcom(BYTE *com);
BYTE redir(BYTE *c);
BYTE getcur(BYTE *p); 
WORD my_exe(BYTE *prog, BYTE *args);
BYTE *trueName(BYTE *name, BYTE *tn);
BYTE *getevar(BYTE *varname, BYTE *value);
void setevar(BYTE *varname, BYTE *value);
BYTE *getalias(BYTE *varname, BYTE *value);
void setalias(BYTE *varname, BYTE *value);
BYTE *getudata(BYTE *varname, BYTE *value, WORD blockseg);
BYTE setudata(BYTE *varname, BYTE *value, WORD blockseg);

void clearbat(void); /* clear the b file linked list*/
void prevbat(void); /* return to the previous nest-level in bat*/

/* internal commands */
void do_se(BYTE n);
void do_ct(BYTE n);
void do_cd(BYTE n);
void do_hh(BYTE n);
void do_eh(BYTE n);
void do_mrd(BYTE n);
void do_xx(void);

/* DOG-file commands */
void do_ca(BYTE n);
void do_go(BYTE n);
void do_sh(BYTE n);


/* command processing */
void do_exe(BYTE n);
void do_chdr(BYTE dr);
void do_bat(void);
void do_command(BYTE na);
void do_batcommand(BYTE na);
