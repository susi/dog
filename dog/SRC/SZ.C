/*

SZ.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

**************************************************************************/

#ifdef port
#include "dog.h"
#endif

void do_sz(BYTE n)
{
    struct ffblk *fb;
    BYTE b,i,j,str[5];
    WORD max,mbx,mcx,mdx;
    DWORD h,s,d,t;

    if (n > 2) {
        puts("Invalid number of arguments.");
        return;
    }
    str [0] = '\0';

    i=0; j=0;
    s=0; d=0;
    h=0;

    fb = malloc(sizeof(struct ffblk));

    asm mov AH,36h
		asm xor dl,dl
		asm int 21h
		asm mov max,AX
		asm mov mbx,BX
		asm mov mcx,CX
		asm MOV mdx,DX

    d = (DWORD)max*(DWORD)mbx*(DWORD)mcx; 
    t = (DWORD)max*(DWORD)mcx*(DWORD)mdx;

    b = findfirst("*.*",fb,FA_NORMAL|FA_HIDDEN|FA_SYSTEM);
    if (b!=18) {
        if (((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN)){
            h += fb->ff_fsize;
            j++;
        }
/*        else if (((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM)){
            s += fb->ff_fsize;
            i++;
        }
*/
        else {
            s += fb->ff_fsize;
            i++;
        }
    }
    b = findnext(fb);
    while (b!=18) {
        if(cBreak) {
            cBreak = 0;
            return;
        }
        if (((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN)) {
            h += fb->ff_fsize;
            j++;
        }
/*
        else if (((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM)){
            h += fb->ff_fsize;
            j++;
        }
*/
        else {
            s += fb->ff_fsize;
            i++;
        }
        b = findnext(fb);
    }

    if (strnicmp(arg[1],"-k",2)==0) {
        h /= 1024;
        s /= 1024;
        d /= 1024;
        t /= 1024;
        strcpy(str,"kilo");
    }

    if (strnicmp(arg[1],"-m",2)==0) {
        h /= 1048576;
        s /= 1048576;
        d /= 1048576;
        t /= 1048576;
        strcpy(str,"mega");
    }

    printf("\n%11lu %sbytes in %d file(s)\n%11lu %sbytes in %d hidden file(s)\n",s,str,i,h,str,j);
    printf("%11lu %sbytes free on drive %c:\n%11lu %sbytes total diskspace\n",d,str,D,t,str);
    free(fb);
    return;
}

