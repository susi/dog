/*

BR.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

BYTE cbreak(BYTE s);

void do_br(BYTE n)
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
            return;
        }
    }
    else {
        puts("Invalid number of arguments.");
    }
    return;
}

/**************************************************************************/

BYTE cbreak(BYTE s)
{
    asm{
        mov AH,33h
        mov AL,s
        cmp AL,0FFh
        je     cbreak_query
        mov DL,AL
        mov AL,01h
        int 21h
    }
    return 0;
    cbreak_query:
    asm{
        mov AL,00h
        int 21h
        mov s,DL
    }
    return s;
}


