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
	asm{
		mov ax,3523h
		int 21h
		mov i23_o,bx
		mov i23_s,es
		mov ax,3524h
		int 21h
		mov i24_o,bx
		mov i24_s,es
        int 21h
	}
	return;
}

void set_int(void)
{
	asm{
		mov ax,2523h
		mov dx,offset CBreak
		push cs
		pop es
		int 21h
		mov ax,2524h
		mov dx,offset CritErr
		push cs
		pop es
		int 21h
	}
	return;
	asm {
CBreak:
        push ax
        push ds
        push bx
        mov ah, 51h
        int 21h
        cmp cs:_psp, bx
        je localCBreak
        pop bx
        pop ds
      	pop ax
		pop bp
        stc
		iret /*db 0cbh*/
    }
    localCBreak:
    asm{
        pop bx
        mov ax, cs
        mov ds, ax
        push ax
        mov al, 01h
        mov cBreak, al
        pop ax
        pop ds
        pop ax
        clc
        pop bp
	    iret /*    db 0cah*/
        dw 0200h
CritErr:
 		iret
DOG2eFunc:
		mov ax,DOG_VER
		iret
	}
}

