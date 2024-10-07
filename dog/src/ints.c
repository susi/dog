/*
INTS.C   -  Alternate command processor for (currently) MS-DOS ver 3.30

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

2024-05-11 - Building as a module.
2024-05-20 - Fixed calls to int21/AH=25h, to use DS, not ES for the segment.
             Started drafting DOG func 3 - Run command.
             Also in plans Installable commands. - WB
2024-05-29 - Implemented a proper C-c handler. - WB
2024-06-08 - Implemented a proper Critical Error handler. - WB
*/
#include "dog.h"

static void DOGCtrlC(void);
void interrupt DOGError(WORD bp, WORD di, WORD si,
			WORD ds, WORD es, WORD dx,
			WORD ex, WORD bx, WORD ax);
static BYTE ce_resolve(BYTE err, BYTE d, WORD driver_type, WORD error_code);

BYTE cCQuestion[]="\r\nBark! Terminate program (Y/N)? $";

BYTE *ceDiskArea[]={
    /* 00h */ "DOS area",
    "--CODE ERROR--",
    /* 02h */ "FAT",
    "--CODE ERROR--",
    /* 04h */ "root directory",
    "--CODE ERROR--",
    /* 06h */ "data area",
};

#define CE_IS_WRITE(ERR) (ERR & 0x01)
#define CE_DISK_AREA(ERR) (ERR & 0x06)
#define CE_FAIL_OK(ERR) (ERR & 0x08)
#define CE_RETRY_OK(ERR) (ERR & 0x10)
#define CE_IGNORE_OK(ERR) (ERR & 0x20)
#define CE_IS_DISK_IO(ERR) ((ERR & 0x80) == 0x00)
#define CE_DEV_IS_CHAR(DEV) ((DEV & 0x8000) == 0x8000)
#define CE_IS_BAD_FAT(ERR, DEV) (!(CE_IS_DISK_IO(ERR)) && !(CE_DEV_IS_CHAR(DEV)))

BYTE *ceCodeTable[] = {
    /* 00h */ "write-protection violation attempted",
    /* 01h */ "unknown unit for driver",
    /* 02h */ "drive not ready",
    /* 03h */ "unknown command given to driver",
    /* 04h */ "data error (bad CRC)",
    /* 05h */ "bad device driver request structure length",
    /* 06h */ "seek error",
    /* 07h */ "unknown media type (non-DOS disk)",
    /* 08h */ "sector not found",
    /* 09h */ "printer out of paper",
    /* 0Ah */ "write fault",
    /* 0Bh */ "read fault",
    /* 0Ch */ "general failure",
    /* 0Dh */ "sharing violation",
    /* 0Eh */ "lock violation",
    /* 0Fh */ "invalid disk change / wrong disk",
    /* 10h */ "FCB unavailable",
    /* 11h */ "sharing buffer overflow",
    /* 12h */ "code page mismatch",
    /* 13h */ "out of input",
    /* 14h */ "insufficient disk space",
};
static BYTE err_msg[90], afir[40];

void save_error_ints(void)
{
    WORD i23_off, i23_seg;
    WORD i24_off, i24_seg;
    asm mov ax,3523h;
    asm int 21h;
    asm mov i23_off,bx;
    asm mov i23_seg,es;
    asm mov ax,3524h;
    asm int 21h;
    asm mov i24_off,bx;
    asm mov i24_seg,es;
    asm int 21h;
    i23_s = i23_seg;
    i23_o = i23_off;
    i24_s = i24_seg;
    i24_o = i24_off;

    return;
}

void set_error_ints(void)
{
    WORD i23_off, i23_seg;
    WORD i24_off, i24_seg;

    asm xor ah,ah;       /* set inCc to 0 */
    asm mov cs:inCc, ah
    /* set and save the CBreak handler address */
    asm mov dx,offset CBreak;
    asm mov i23_off, dx; /* save the offset */
    asm mov ax, cs;
    asm mov ds, ax;
    asm mov i23_seg, ax; /* save the segment */
    asm mov ax,2523h;
    asm INT 21h;         /* int 21h/ah=25h,al=23h - set IRQ vector 23 */

    /* set and save the CritError handler address */
    asm mov dx,offset DOGError;
    asm mov i24_off, dx; /* save the offset */
    asm mov ax, cs;
    asm mov ds, ax;
    asm mov i24_seg, ax; /* save the segment */
    asm mov ax,2524h;
    asm INT 21h;         /* int 21h/ah=25h,al=24h - set IRQ vector 24 */

    /* save to global variables */
#ifdef DOG_DEBUG
    printf("set_error_ints():0:i23=%04X:%04X\n", i23_seg, i23_off);
    printf("set_error_ints():0:i24=%04X:%04X\n", i24_seg, i24_off);
#endif
    my_i23_s = i23_seg;
    my_i23_o = i23_off;
    my_i24_s = i24_seg;
    my_i24_o = i24_off;
#ifdef DOG_DEBUG
    printf("set_error_ints():1:i23=%04X:%04X\n", my_i23_s, my_i23_o);
    printf("set_error_ints():1:i24=%04X:%04X\n", my_i24_s, my_i24_o);
    printf("set_error_ints():1:DOGError: %Fp\n", (void far *)&DOGError);
#endif
    return;
}

void restore_error_ints(void)
{
    /* restore the CBreak handler address */
    asm mov  ax, 2523h;
    asm mov  dx, i23_o;
    asm mov  bx, i23_s;
    asm push bx;
    asm pop  ds;
    asm int 21h;         /* int 21h/ah=25h,al=23h - restore IRQ vector 23 */
    asm push cs;
    asm pop  ds;

    /* restore the Critical Error handler address */
    asm mov  ax, 2524h;
    asm mov  dx, i24_o;
    asm mov  bx, i24_s;
    asm push bx;
    asm pop  ds;
    asm int 21h;         /* int 21h/ah=25h,al=24h - restore IRQ vector 24 */
    asm push cs;
    asm pop  ds;
    return;
}

void set_int2e(void)
{
    WORD i2eseg, i2eoff;
    WORD my_2e_s, my_2e_o;
    /* save */
    asm mov ax,352eh;
    asm mov i2eoff,bx;
    asm mov i2eseg,es;
    asm int 21h;
    /* set */
    asm mov ax,252eh;
    asm mov dx,offset D0GFunc; /* need to point to DOG2e */
    asm mov my_2e_o, dx;
    asm push cs;
    asm pop ds;
    asm mov my_2e_s, ds;
    asm int 21h;

    i2e_s = i2eseg;
    i2e_o = i2eoff;
#ifdef DOG_DEBUG
    printf("make_int2e():1:i2e=%04X:%04X\n", my_2e_s, my_2e_o);
    printf("make_int2e():1:DOGFunc: %Fp\n", (void far *)&DOGFunc);
#endif

    return;
}

void set_intd0(void)
{
    WORD id0seg, id0off;
    WORD my_d0_s, my_d0_o;
    /* save */
    asm mov ax,35d0h;
    asm mov id0off,bx;
    asm mov id0seg,es;
    asm int 21h;
    /* set */
    asm mov ax,25d0h;
    asm mov dx,offset D0GFunc;
    asm mov my_d0_o, dx;
    asm push cs;
    asm pop ds;
    asm mov my_d0_s, ds;
    asm int 21h;

    id0_s = id0seg;
    id0_o = id0off;
#ifdef DOG_DEBUG
    printf("make_intd0():1:id0=%04X:%04Xh\n", my_d0_s, my_d0_o);
    printf("make_intd0():1:DOGFunc: %Fp\n", (void far *)&DOGFunc);
#endif

    return;
}

void restore_intd0(void)
{
  asm mov ax, 25d0h;
  asm mov dx, id0_o;
  asm mov bx, id0_s;
  asm push bx;
  asm pop ds;
  asm int 21h;
  asm push cs;
  asm pop ds;
}

static void DOGCtrlC(void)
{
/****************************/
/*  Ctrl-C handler (int 23h)
/****************************/
    asm CBreak:
    /* first check if we're already working on a C-c, if so, ignore */
    asm dec cs:inCc;  /* inCc is 1 when handling a C-c. */
    asm jnz newCc;    /* if inCc was 0, it will now be FF, so not 0 */
    asm inc cs:inCc;  /* Already handling C-c, inc inCc (back to 1) and ignore C-c */
    asm clc;
    asm retf;
    asm db 0;            /* padding */
    asm inCc db 0;       /* This is the variable inCc, sneakily in cs */
newCc:
    asm push ax;         /* save ax */
    asm mov ah, 1;
    asm mov cs:inCc, ah; /* set inCc to 1 */
    asm push ds;         /* save ds, and set our own ds */
    asm mov ax, cs;
    asm mov ds, ax;      /* okay DS is now correct we can continue normally,*/
                         /* stack is not ours, though. */
    asm push bx;         /* now check if PPS is DOG or someone else */
    asm mov  ah, 62h;    /* Get PSP / PID */
    asm int 21h;
    asm cmp _psp, bx;
    asm je  localCBreak;
    asm push dx;         /* external program, ask question from the user. */
cCQQ:
    asm mov ah, 09h;
    asm mov dx, OFFSET cCQuestion;
    asm int 21h;         /* Ask the question */
    asm mov ah, 01h;
    asm int 21h;         /* read key with echo */
    asm pop dx;          /* dx no longer needed, pop it */
    asm cmp al, 'Y';
    asm jz cCterminate;
    asm cmp al, 'y';
    asm jz cCterminate;
    asm cmp al, 'N';
    asm jz cCignore;
    asm cmp al, 'n';
    asm jz cCignore;
    asm jmp cCQQ;
localCBreak:
    asm mov al, in_getln; /* if reading input (waiting in int21/AH=0ah;) */
                          /* we need to print a new line and the prompt */
    asm or al, al;
    asm jz cCsetFlag;     /* No we're not, set C-c flag */
    /* We're actually on the command line waiting for input and user pressed C-c */
    /* save all registers, printprompt might use them all */
    asm push ax;
    asm push bx;
    asm push cx;
    asm push dx;
    asm push bp;
    asm push si;
    asm push di;
    puts("");
    printprompt();      /* offset 0d81h */
    asm pop di;
    asm pop si;
    asm pop bp;
    asm pop dx;
    asm pop cx;
    asm pop bx;
    asm pop ax;
    asm jmp cCignore;   /* we're done. */
cCsetFlag:
    asm mov al, 01h;    /* Set C-c flag so that we catch it */
                        /* and handle it internally, eg DOGfiles, parsing input... */
    asm mov cBreak, al;
    asm jmp cCignore;
cCterminate:
    asm stc;            /* setc + retf = abort with errorlevel 0, */
		        /* but termination reason C-c */
    asm jmp cCret;
cCignore:
    asm clc;            /* clc + retf = continue */
cCret:
    asm pop bx;
    asm pop ds;
    asm pop ax;
    asm dec cs:inCc;    /* done with C-c set inCc to 0 */
    asm retf;
}


/***************************************************/
/*      Critical Error Handler (int 24h)           */
/***************************************************/
/* called with
   AH = Error type
   7 6 5 4 3 2 1 0   BitMask:
   | | | | | | | +-- 01h: 1 = write, 0 = read
   | | | | | | +---  06h: Disk area: 00h=DOS area 02h=FAT
   | | | | | +/                      04h=root dir 06h=data area
   | | | | |
   | | | | +-------- 08h: 08h = fail allowed
   | | | +---------- 10h: 10h = retry allowed
   | | +------------ 20h: 20h = ignore allowed
   | +-------------- ---: unused
   +---------------- 80h: 00h = disk I/O error
                          80h = if block device -> bad FAT in memory
			        if char  device -> error code in DI
   AL = Drive number if AH[.7] == 0
   BP:SI = device driver header BP:[SI+4] bit 15 set if char
   DI = Error code if AH & 80h == 80h and BP:[SI+4] & 8000h = 8000h
*/
#pragma argsused
void interrupt DOGError(WORD bp, WORD di, WORD si,
			WORD ds, WORD es, WORD dx,
			WORD ex, WORD bx, WORD ax)
{
    WORD driver_type, error_code;
    BYTE err, d;

    d = (BYTE)ax;
    err = (BYTE)(ax >> 8);
    driver_type = (WORD) peek(bp, si+4);
    error_code = di;
    ax = ce_resolve(err, d, driver_type, error_code);
    return;
}

/**************************************************************************/

void display_string(char *sd)
{
    asm mov ax, cs;
    asm mov ds, ax;
    asm mov ah, 09h;
    asm mov dx, OFFSET sd;
    asm int 21h;
}

/**************************************************************************/

BYTE read_key(void)
{
    BYTE c;
    asm mov ah, 01h; /* read key with echo */
    asm int 21h;
    asm mov c,al;

    if(c >= 'a') {
	c -= ('a' - 'A'); /* uppercase the character */
    }
    return c;
}

/**************************************************************************/

/* Displays error message and gets user input.
   Returns a resolution (Abort, Fail, Ignore, Retry)
 */
static BYTE ce_resolve(BYTE err, BYTE d, WORD driver_type, WORD error_code)
{
    BYTE drive='A', key, l;

    asm mov ax, cs;
    asm mov ds, ax;

    /* Build options */
    strcpy(afir, " (Abort");
    if (CE_FAIL_OK(err))
	strcat(afir, "/Fail");
    if (CE_IGNORE_OK(err))
	strcat(afir, "/Ignore");
    if (CE_RETRY_OK(err))
	strcat(afir, "/Retry");
    strcat(afir, ")? $");

    /* Build Error message */
    strcpy(err_msg, "\r\n");

    if (CE_IS_DISK_IO(err)) {
	if (CE_IS_WRITE(err)) {
	    strcat(err_msg, "Write error drive X: ");
	}
	else {
	    strcat(err_msg, "Read error drive X: ");
	}
	drive += d;
	l = strlen(err_msg);
	err_msg[l-3] = drive;
	strcat(err_msg, ceDiskArea[CE_DISK_AREA(err)]);
    }
    else if (CE_IS_BAD_FAT(err, driver_type)) {
	if (CE_IS_WRITE(err)) {
	    strcat(err_msg, "Write error: FAT in memory corrupt");
	}
	else {
	    strcat(err_msg, "Read error: FAT in memory corrupt");
	}
    } else {
	/* character device error_code gives details */
	if (CE_IS_WRITE(err)) {
	    strcat(err_msg, "Write error: ");
	}
	else {
	    strcat(err_msg, "Read error: ");
	}
	strcat(err_msg, ceCodeTable[error_code]);
    }
    strcat(err_msg, afir);

    do {
	display_string(err_msg);
	key = read_key();
	display_string("\r\n$");
	if (key == 'A') { /* Abort is always allowed */
	    return 0x02; /* Abort */
	}
	else if(key == 'F' && CE_FAIL_OK(err)) {
	    return 0x03; /* Fail */
	}
	else if(key == 'I' && CE_IGNORE_OK(err)) {
	    return 0x00; /* Ignore */
	}
	else if(key == 'R' && CE_RETRY_OK(err)) {
	    return 0x01; /* Retry */
	}
	display_string("Invalid option.$");
    } while(1);
}

void DOGFunc(void)
{
#if 0
    BYTE far *cmd_ps, cmd_sz[200];
#endif

    asm D0GFunc:
    asm cmp ah,01h;
    asm jz D0G_1;       /* Function 0x01 */
    asm cmp ah,02h;
    asm jz D0G_2;        /* Function 0x02 */
#if 0
    asm cmp ah,03h;
    asm jz D0G_3;        /* Function 0x03 */
#endif
    asm jmp D0G_naf;     /* not our function ignore.. */

D0G_1:
    asm MOV ax, DOG_VER; /* return the version of DOG loaded */
    asm jmp D0G_ret;

D0G_2:
    asm push ds;
    asm push cs;
    asm pop ds;
    asm cmp al,01h;
    asm jz D0G_2_1;       /* Function 0x0201 */
    asm cmp al,02h;
    asm jz D0G_2_2;       /* Function 0x0202 */
    asm cmp al,03h;
    asm jz D0G_2_3;       /* Function 0x0203 */
    asm cmp al,04h;
    asm jz D0G_2_4;       /* Function 0x0204 */
    asm cmp al,05h;
    asm jz D0G_2_5;       /* Function 0x0205 */
    asm cmp al,06h;
    asm jz D0G_2_6;       /* Function 0x0206 */
    asm cmp al,07h;
    asm jz D0G_2_7;       /* Function 0x0207 */
    asm jmp D0G_2ret;

D0G_2_1:
    asm push ds;
    asm pop es;
    asm mov bx,offset envseg; /* return a pointer to envseg in ES:BX*/
    asm mov dx,envseg;        /* DX contains the value of envseg */
    asm jmp D0G_2ret;
D0G_2_2:
    asm push ds;
    asm pop es;
    asm mov bx,offset aliasseg; /* return a pointer to aliasseg in ES:BX*/
    asm mov dx,aliasseg;        /* DX contains the value of aliasseg  */
    asm jmp D0G_2ret;
D0G_2_3:
    asm push ds;
    asm pop es;
    asm mov bx,offset commands; /* return a pointer to commands in ES:BX*/
    asm mov cx, _NCOMS;         /* number of strings */
    asm jmp D0G_2ret;
D0G_2_4:
    asm push ds;
    asm pop es;
    asm mov bx,offset ext_commands; /* return a pointer to ext_commands in ES:BX*/
    asm mov cx, _NECOMS;            /* number of strings */
    asm jmp D0G_2ret;
D0G_2_5:
    asm push ds;
    asm pop es;
    asm mov bx,offset command_des; /* return a pointer to command_des in ES:BX*/
    asm mov cx, _NCOMS;            /* number of strings */
    asm jmp D0G_2ret;
D0G_2_6:
    asm push ds;
    asm pop es;
    asm mov bx,offset ext_command_des; /* return a pointer to ext_command_des in ES:BX*/
    asm mov cx, _NECOMS;               /* number of strings */
    asm jmp D0G_2ret;
D0G_2_7:
    asm push ds;
    asm pop es;
    asm mov bx,offset bf; /* return a pointer to bf (the first Dogfile param block) in ES:BX*/
    asm jmp D0G_2ret;

#if 0
D0G_3:
    asm D0GF2e:      /* run command */
    asm push es;     /* save es */
    asm push ds;     /* save ds */

    asm push ds;     /* command string is in DS:SI */
    asm push cs;
    asm pop ds;      /* our DS */
    asm pop es;      /* pointer to string is now ES:SI */
    asm cmd_ps, es:si;  /* The command to run as a Pascal String */
    /*
      TODO: use the string instructions to copy input to cms_sz
      TODO: parse cmdline and run do_command(), reset com
    */

    asm pop ds;
    asm pop es;
    asm jmp D0G_ret;
#endif
D0G_2ret:
    asm pop ds;
D0G_naf:
D0G_ret:
    asm iret;
}
