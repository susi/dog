/*

VF.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

***************************************************************************/

void do_vf(BYTE n)
{

    if (n==1) {
        printf("Verify is ");
        asm{
            MOV ah,54h  /*  Get verify flag */
            INT 21h
            MOV dl,01h
            CMP al,dl
            JE  on
            }
        puts("OFF");
        return;
        on:
        puts("ON");
        return;
    }
    if (n==2) {
        if (stricmp(arg[1],"ON")==0) {
            asm {
                MOV ah,2eh  /*Set Verify flag*/
                MOV al,01h  /* to on*/
                INT 21
            }
        }
        else if (stricmp(arg[1],"OFF")==0) {
            asm {
                MOV ah,2eh  /* Set Verify flag */
                MOV al,00h  /* to off */
                INT 21
            }
        }
        else {
            puts("You MUST specify either ON or OFF.");
            return;
        }

    }
    if ((n!=1) && (n!=2)) {
        puts("Invalid number of arguments.");
        return;
    }
}


