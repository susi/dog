/*

CL.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

Developers:
Wolf Bergenheim (WB)

History
18.03.00 - Extracted from DOG.C - WB

****************************************************************************/

void do_cl(void)
{
	
	  asm MOV ax,0600h
		asm MOV bh,07h
		asm MOV cx,0000h        /* (0,0) to*/
		asm MOV dx,5079h        /* (50,79) */
		asm INT 10h             /*Clear screen*/
		asm MOV ah,02h
		asm MOV bh,00h
		asm MOV dx,0000h        /* (0,0)*/
		asm INT 10h             /*Move cursor to upper left corner*/
    
}

