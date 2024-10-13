/*

TN.C - DOG - TRUENAME

Copyright (C) 2024 Wolf Bergenheim

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
2024-10-13 - Initial implementation - WB
*/
#include "dog.h"

void do_tn(BYTE n)
{
    BYTE buffer[128];
    BYTE far *fn;
    BYTE far *tn;
    WORD tn_error_code;

    if (n != 2) {
	printf("Usage: TN FILEPATH\n");
	return;
    }

    fn = (BYTE far*)arg[1];
    tn = (BYTE far*)buffer;

    asm push di;
    asm push si;
    asm lds  si, fn;     /* DS:SI is the input */
    asm les  di, tn;     /* ES:DI is a 128 byte buffer for output */
    asm mov  ah, 60h;    /* Function 60h = TRUENAME */
    asm int  21h;
    asm pop  si;
    asm pop  di;
    asm jc   tn_error;   /* CF set on error */
    printf("%s => %s\n", arg[1], tn);
    return;
tn_error:
    asm mov  tn_error_code, ax;
    if (tn_error_code == 2) {
	printf("Cannot resolve %s - invalid path component\n", arg[1]);
    }
    else if (tn_error_code == 3) {
	printf("Cannot resolve %s - malformed path or invalid drive letter\n", arg[1]);
    }
    return;
}
