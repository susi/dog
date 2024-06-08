/*
tint24.c   -  Test for int24 handler

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

2024-06-08 - Implemented a test for all int 24 handlers. - WB
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

#define BYTE unsigned char
#define WORD unsigned int
#define DWORD unsigned long

#ifdef MK_FP
#undef MK_FP
#endif
#define MK_FP(seg,ofs) ((void far*)(((unsigned long)(seg) << 16) | (ofs)))

/* PSP */
#define PSP_I22_OFS 0x000A
#define PSP_I22_SEG 0x000C
#define PSP_I23_OFS 0x000E
#define PSP_I23_SEG 0x0010
#define PSP_I24_OFS 0x0012
#define PSP_I24_SEG 0x0014
#define PSP_PPID_OFS 0x0016
#define PSP_ENVSEG_OFS 0x002C

extern unsigned _psp;
extern unsigned _heaplen;
extern unsigned _stklen;

extern BYTE _osmajor;
extern BYTE _osminor;

BYTE cBreak = 0, in_getln=0;
WORD my_i22_s, my_i22_o;
WORD my_i23_s, my_i23_o; /* When making a permanent shell take over */
WORD my_i24_s, my_i24_o; /* int 23 and int 24. */
WORD i23_s, i23_o; /* variables to store old ctrl-c handler*/
WORD i24_s, i24_o;
WORD i2e_s, i2e_o;
WORD id0_s, id0_o; /* the old handler for the D0GFunc */
WORD envseg=0,envsz=512;
WORD aliasseg=0,aliassz=2048;

#define _NCOMS 2
#define _NECOMS 2

BYTE *commands[] = {"FOO", "BAR"};
BYTE *command_des[] = {"foo", "bar"};
BYTE *ext_commands[] = {"BAZ", "FOZ"};
BYTE *ext_command_des[] = {"baz", "foz"};
BYTE *bf = "bf";

/* ints.c */
void save_error_ints(void);
void set_error_ints(void);
void restore_error_ints(void);
void set_int2e(void);
void set_intd0(void);
void restore_intd0(void);
void display_string(char *sd);
void DOGFunc(void);

void printprompt(void);
void make_int24_call(BYTE rAH, BYTE rAL, WORD rBP, WORD rSI, WORD rDI);

#define ABORT  0x02
#define FAIL   0x03
#define IGNORE 0x00
#define RETRY  0x01

BYTE *response[] = {
    "Ignore",
    "Retry",
    "Abort",
    "Fail",
};

void printprompt(void)
{
    puts("PROMPT");
}

void make_int24_call(BYTE rAH, BYTE rAL, WORD rBP, WORD rSI, WORD rDI)
{
    BYTE retval;
    WORD far *fpBPSI;
    WORD rBPSI;
    fpBPSI = MK_FP(rBP, rSI+4);
    rBPSI = *fpBPSI;
    printf("Triggering int 24h with AH=%02Xh AL=%02Xh BP:[SI+4]=%Fp=%04Xh DI=%04X\n",
	   rAH, rAL, fpBPSI, rBPSI, rDI);
    asm push ax;
    asm push bx;
    asm push si;
    asm push di;

    asm mov si, rSI;
    asm mov di, rDI;
    asm mov ah, rAH;
    asm mov al, rAL;
    asm mov bx, rBP;
    asm push bp;
    asm mov bp, bx;

    asm int 24h;
    asm pop bp;
    asm mov retval, al;

    asm pop di;
    asm pop si;
    asm pop bx;
    asm pop ax;

    printf("int 24h returned %s(%02Xh)\n", response[retval], retval);
}

int main(void)
{
    BYTE BPSIa[6]={0x12, 0x34, 0x56, 0x78, 0xbc, 0x9a};
    WORD far *fpBPSI;
    WORD rDI=0;
    BYTE d, fri, rw, diskarea,i, j, rAH, rAL;

    fpBPSI = (WORD far *)(BPSIa+4);

    save_error_ints();
    set_error_ints();

    puts("Starting I/O error tests:");
    for(i=0; i < 4; i++) {
	diskarea = i<<1;
	printf("disk area: %02Xh\n", diskarea);
	for (j=0; j < 8; j++) {
	    fri = j << 3;
	    printf("fri: %02Xh\n", fri);
	    for (d=0; d<6; d++) {
		printf("drive: %02Xh\n", d);
		for (rw=0; rw < 2; rw++) {
		    if (rw)
			puts("write");
		    else
			puts("read");
		    rAH = rw + diskarea + fri;
		    rAL = d;
		    make_int24_call(rAH, rAL, _DS, (WORD) BPSIa, rDI);
		}
	    }
	}
    }

    puts("Block device errors tests:");
    *fpBPSI=(WORD)0x0d06;
    diskarea = 0;
    for (j=0; j < 8; j++) {
	fri = j << 3;
	printf("fri: %02Xh\n", fri);
	d=0;
	for (rw=0; rw < 2; rw++) {
	    if (rw)
		puts("write");
	    else
		puts("read");
	    rAH = 0x80 + rw + diskarea + fri;
	    rAL = d;
	    make_int24_call(rAH, rAL, _DS, (WORD)BPSIa, rDI);
	}
    }

    puts("CHAR dev errors tests:");
    *fpBPSI=(WORD)0x8d06;
    diskarea = 0;
    d = 0;
    for(i=0; i < 0x15; i++) {
	printf("errno: %02Xh\n", i);
	for (j=0; j < 8; j++) {
	    fri = j << 3;
	    printf("fri: %02Xh\n", fri);
	    for (rw=0; rw < 2; rw++) {
		if (rw)
		    puts("write");
		else
		    puts("read");
		rAH = 0x80 + rw + diskarea + fri;
		rAL = d;
		rDI = (WORD)i;
		make_int24_call(rAH, rAL, _DS, (WORD) BPSIa, rDI);
	    }
	}
    }

    puts("int 24 testing done");
    return 0;
}
