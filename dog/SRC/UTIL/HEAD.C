/*
HEAD.C - version 1.0
    Displays a certain amount of a file.

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

This program is part of DOG - The DOG Operating Ground

NOTE: Compile with Micro-C 3.15: CC head.c -fop

History

30.07.99 - Created by Wolf Bergenheim
04.08.99 - Added switches.
*/
#include <file.h>
#include <stdio.h>

#define BYTE unsigned char
#define WORD unsigned int

WORD lines = 10;
WORD count = 0;

int main(int nargs,char *args[])
{
    FILE *fp;
    BYTE i,p[150];
    int r;

    if(nargs == 1) {
        puts("HEAD.COM v.1.0 by Wolf Bergenheim a utilityprogram to DOG.");
        puts("usage: HEAD filename.ext [-N lines | -C count]");
        return 1;
    }
    else if(nargs == 4) {
        if(stricmp(args[2],"-N") == 0) {
            fp = fopen(args[1],"rvq");
            lines = atoi(args[3]);
        }
        else if(stricmp(args[2],"-C") == 0) {
            fp = fopen(args[1],"rbvq");
            count  = atoi(args[3]);
            for(i=0;(i<count) && ((r = fgetc(fp)) != -1);i++) {
                putchar((BYTE)r);
            }
            return 0;
        }
    }
    else
        fp = fopen(args[1],"rvq");

    for(i=0;(i<lines) && (fgets(p,149,fp) != NULL);i++) {
        puts(p);
    }

    return 0;
}
