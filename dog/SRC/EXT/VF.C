/*

VF.C - DOG - Alternate command processor for freeDOS

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
02.01.02 - Fixed bug: In call to set state of verify flag A call ti int 21 and
            NOT int 21h was made (oops :) Other bugfixes too: main returns int
            added return to the end of the prog to make the compiler happy - WB
2002-11-21 - Added help text, prints to std err - WB

***************************************************************************/

#include "ext.h"

int main(BYTE n, BYTE *arg[])
{

    if (n==1) {
        printf("Verify is ");

			  asm MOV ah,54h  /*  Get verify flag */
        asm INT 21h
	  		asm MOV dl,01h
        asm CMP al,dl
        asm JE  on

        puts("OFF");
        return 0;
        on:
        puts("ON");
        return 0;
    }
    else if (n==2) {
        if (stricmp(arg[1],"ON")==0) {

					asm MOV ah,2eh  /*Set Verify flag*/
					asm MOV al,01h  /* to on*/
					asm XOR dl,dl
					asm INT 21h

        }
        else if (stricmp(arg[1],"OFF")==0) {

					asm MOV ah,2eh  /* Set Verify flag */
					asm MOV al,00h  /* to off */
					asm XOR dl,dl
					asm INT 21h

        }
        else {
					fputs("Usage: VF [ ON | OFF ]\n\nGet [ Set | Clear ] the system verify flag\n\n", stderr);
					fputs("Second argument is either ON or OFF\n", stderr);
					fputs("\tON\tSet the system verify flag.\n\tOFF\tClear the system verify flag\n", stderr);
					fputs("\nVF is part of DOG (http://dog.sf.net/)\n", stderr);
					return 1;
        }

    }
    else {
			fputs("Invalid number of arguments.\n", stderr);
			fputs("Usage: VF [ ON | OFF ]\n\nGet [ Set | Clear ] the system verify flag\n\n", stderr);
			fputs("Second argument is either ON or OFF\n", stderr);
			fputs("\tON\tSet the system verify flag.\n\tOFF\tClear the system verify flag\n", stderr);
			fputs("\nVF is part of DOG (http://dog.sf.net/)\n", stderr);
			return 0xFF;
    }
    return 0;
}


