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
*/
#include "dog.h"

void get_int(void)
{
    WORD i23_off, i23_seg;
    WORD i24_off, i24_seg;
    asm mov ax,3523h;
    asm INT 21h;
    asm MOV i23_off,bx;
    asm MOV i23_seg,es;
    asm MOV ax,3524h;
    asm INT 21h;
    asm MOV i24_off,bx;
    asm MOV i24_seg,es;
    asm INT 21h;
    i23_s = i23_seg;
    i23_o = i23_off;
    i24_s = i24_seg;
    i24_o = i24_off;
    return;
}

void set_int(void)
{
    asm MOV ax,2523h;
    asm MOV dx,offset CBreak;
    asm push cs;
    asm pop es;
    asm INT 21h;
    asm MOV ax,2524h;
    asm MOV dx,offset CritErr;
    asm push cs;
    asm pop es;
    asm INT 21h;

    return;
}

void make_int2e(void)
{
    WORD i2eseg, i2eoff;
    /* save */
    asm mov ax,352eh;
    asm mov i2eoff,bx;
    asm mov i2eseg,es;
    asm int 21h;
    /* set */
    asm mov ax,252eh;
    asm mov dx,offset D0GFunc;
    asm push cs;
    asm pop es;
    asm int 21h;

    i2e_s = i2eseg;
    i2e_o = i2eoff;
}

void make_intd0(void)
{
    WORD id0seg, id0off;
    /* save */
    asm mov ax,35d0h;
    asm mov id0off,bx;
    asm mov id0seg,es;
    asm int 21h;
    /* set */
    asm mov ax,25d0h;
    asm mov dx,offset D0GFunc;
    asm push cs;
    asm pop es;
    asm int 21h;

    id0_s = id0seg;
    id0_o = id0off;
}

void DOGError(void)
{
/*********************/
    asm CBreak:
    asm push ax;
    asm push ds;
    asm push bx;
    asm MOV ah, 51h;
    asm INT 21h;
    asm CMP cs:_psp, bx;
    asm je localCBreak;
    asm pop bx;
    asm pop ds;
    asm pop ax;
    asm pop bp;
    asm stc;
    asm db 0cbh; /* retf*/
localCBreak:
    asm pop bx;
    asm MOV ax, cs;
    asm MOV ds, ax;
    asm push ax;
    asm MOV al, 01h;
    asm MOV cBreak, al;
    asm pop ax;
    asm pop ds;
    asm pop ax;
    asm clc;
    asm pop bp;
    asm db 0cah; /* fetf */
    asm dw 0200h;

/*********************/
    asm CritErr:
    asm mov al,03; /* FAIL */
    asm iret;

}

void DOGFunc(void)
{
    asm D0GFunc:
    asm cmp ah,01h;
    asm jz D0G_1;       /* Function 0x01 */
    asm cmp ah,02h;
    asm jz D0G_2;        /* Function 0x02 */
    asm jmp D0G_naf;     /* not our function ignore.. */

D0G_1:
    asm MOV ax,DOG_VER; /* return the version of DOG loaded */
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
D0G_2ret:
    asm pop ds;
D0G_naf:
D0G_ret:
    asm iret;
}
