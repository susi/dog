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

void get_int(void)
{

	asm MOV ax,3523h
	asm INT 21h
	asm MOV i23_o,bx
	asm MOV i23_s,es
	asm MOV ax,3524h
	asm INT 21h
	asm MOV i24_o,bx
	asm MOV i24_s,es
  asm INT 21h

	return;
}

void set_int(void)
{

	asm MOV ax,2523h
	asm MOV dx,offset CBreak
	asm push cs
	asm pop es
	asm INT 21h
	asm MOV ax,2524h
	asm MOV dx,offset CritErr
  asm push cs
	asm pop es
	asm INT 21h
	
	return;

	asm CBreak:
	asm push ax
		asm push ds
		asm push bx
		asm MOV ah, 51h
		asm INT 21h
		asm CMP cs:_psp, bx
		asm je localCBreak
		asm pop bx
		asm pop ds
		asm pop ax
		asm pop bp
    asm stc
		asm iret /*db 0cbh*/
    
	  localCBreak:
	
	  asm pop bx
		asm MOV ax, cs
		asm MOV ds, ax
		asm push ax
		asm MOV al, 01h
		asm MOV cBreak, al
		asm pop ax
		asm pop ds
		asm pop ax
		asm clc
    asm pop bp
		asm iret /*    db 0cah*/
		asm dw 0200h
		asm CritErr:
 		asm iret
		
}

void DOGFunc(void)
{
	asm D0GFunc:       ;
	asm cmp al,01h     ;
	asm jz D0G_1       ; /* Function 0x01 */
	asm jmp D0G_naf    ; /* not our function ignore.. */
	D0G_1:         ;
	asm MOV ax,DOG_VER ; /* return the version of DOG loaded */
	D0G_naf:       ;

	asm iret           ;
 
}

