/*

BR.C - DOG - Alternate command processor for freeDOS

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

#include "ext.h"

BYTE cbreak(BYTE s);

int main(BYTE n, BYTE *arg[])
{

    BYTE f;

    if(n==1) {

        f = cbreak(0xff);
        printf("BReak is ");
        switch(f) {
            case 0 :
                    puts("OFF");
                    break;
            case 1 :
                    puts("ON");
                    break;
       }
    }
    else if(n==2) {

        if (stricmp(arg[1],"ON")==0) {
            f=cbreak(1);
        }
        else if (stricmp(arg[1],"OFF")==0) {
            f=cbreak(0);
        }
        else {
            puts("You MUST specify either ON or OFF.");
            return 0xFF;
        }
    }
    else {
        puts("Invalid number of arguments.");
			 return 0xFF;
    }
    return 0;
}

/**************************************************************************/

BYTE cbreak(BYTE s)
{

    asm MOV AH,33h
    asm MOV AL,s
    asm CMP AL,0FFh
    asm JE  cbreak_query
    asm MOV DL,AL
    asm MOV AL,01h
    asm INT 21h

    return 0;
    cbreak_query:

    asm MOV AL,00h
    asm INT 21h
    asm MOV s,DL

    return s;
}


