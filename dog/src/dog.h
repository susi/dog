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

#ifndef __DOG_H__
#define __DOG_H__
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

/* See ..\debug.h for all the possible debugs. ext.h includes it
   _DEBUG_ALL_ turns all all debugging and turns off optimization */
#include "debug.h"

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
By Ruth Ginsberg

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


#define _NCOMS 10
#define _NECOMS 11
#define _NARGS 20
#define MAX_ALIAS_LOOPS 21

/* PSP */
#define PSP_I22_OFS 0x000A
#define PSP_I22_SEG 0x000C
#define PSP_I23_OFS 0x000E
#define PSP_I23_SEG 0x0010
#define PSP_I24_OFS 0x0012
#define PSP_I24_SEG 0x0014
#define PSP_PPID_OFS 0x0016
#define PSP_ENVSEG_OFS 0x002C

/* MCB is 1 segment preceeding the block segment,
   at offset 3 is the size of the block in paragraphs (16 bytes) */
#define BLOCKSZ(segment) (peek(segment-1,3) << 4)

/* VERSION */
#define DOG_VER 083bh
#define DOG_MA 0
#define DOG_MI 8
#define DOG_RE 0x3b

/* Define Global Variables */
extern BYTE DOG_ma, DOG_mi, DOG_re;
extern BYTE Xit, Xitable, eh, D, P[];
extern BYTE comline[200], *com, *arg[_NARGS], varg[_NARGS][200], *prompt;
extern BYTE commands[_NCOMS][3];
extern BYTE ext_commands[_NECOMS][3];
extern BYTE command_des[_NCOMS][21];
extern BYTE ext_command_des[_NECOMS][21];

extern BYTE flags;
#define FLAG_P 0x1
#define FLAG_E 0x2
#define FLAG_C 0x4
#define FLAG_A 0x8

#define C_AL 0
#define	C_CC 1
#define	C_CD 2
#define	C_CT 3
#define C_EH 4
#define C_HH 5
#define C_MD 6
#define C_RD 7
#define C_SE 8
#define C_XX 9

extern BYTE cBreak;
extern WORD drvs,errorlevel,PSP;
extern WORD envseg,envsz;
extern WORD aliasseg,aliassz;
extern BYTE far * _env;
extern WORD my_i22_s, my_i22_o;
extern WORD my_i23_s, my_i23_o; /* When making a permanent shell take over */
extern WORD my_i24_s, my_i24_o; /* int 23 and int 24. */
extern WORD i23_s, i23_o; /* variables to store old ctrl-c handler*/
extern WORD i24_s, i24_o;
extern WORD i2e_s, i2e_o;
extern WORD id0_s, id0_o; /* the old handler for the D0GFunc */
extern BYTE IN,OUT,sin,sout;
extern BYTE REDIRECTED_OUT, REDIRECTED_IN;

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
};

extern struct bfile *bf;

struct red {
    BYTE name[MAXDIR];
    FILE *fp;
	  WORD fh;
	  WORD fo;
    BYTE redirect;
    BYTE opt[3];
};

extern struct red fout, fin;

struct s_pipe {
	WORD phandle;
	BYTE pname[MAXDIR+13];
	BYTE pcmd[200];
	BYTE pstatus; /*0=nopipes active 1=tmp file pending delete 2= active */
};

extern struct s_pipe pip;

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
BYTE *getevar(BYTE *varname, BYTE *value, WORD vlen);
void setevar(BYTE *varname, BYTE *value);
BYTE *getalias(BYTE *varname, BYTE *value, WORD vlen);
void setalias(BYTE *varname, BYTE *value);
BYTE *getudata(BYTE *varname, BYTE *value, WORD blockseg, WORD vlen);
BYTE setudata(BYTE *varname, BYTE *value, WORD blockseg);
WORD mkudata(WORD oldseg, WORD *nseg, WORD bsz, WORD nbsz);
BYTE myallocmem(WORD *sz, WORD *seg);
BYTE myfreemem(WORD seg);
BYTE aliasreplace(BYTE *com);
void evarreplace(BYTE *com, BYTE ln);
BYTE isfchar(BYTE c);

void clearbat(void); /* clear the b file linked list*/
void prevbat(void); /* return to the previous nest-level in bat*/
BYTE file_exist(const char * filename);
BYTE bat_check_cbreak(void);
BYTE read_key(void);

/* ints.c */
void save_error_ints(void);
void set_error_ints(void);
void make_int2e(void);
void make_intd0(void);

/* internal commands */
void do_al(BYTE n);
void do_se(BYTE n);
void do_cc(BYTE n);
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
void do_if(BYTE n);
void do_44(BYTE n);
void do_do(BYTE n);

/* command processing */
void do_exe(BYTE n);
void do_chdr(BYTE dr);
void do_bat(void);
void do_command(BYTE na);
void do_batcommand(BYTE na);

#endif /* __DOG_H__ */
