/*
HEAD.C - version 1.0
    Displays a certain amount of a file.

Copyright (C) 2000 Wolf Bergenheim

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

History

30.07.99 - Created by Wolf Bergenheim
04.08.99 - Added switches.
12.03.00 - Ported to Borland C++ 3.1
13.03.00 - Addded arg checking
*/
#include "util.h"

WORD lines = 10;
WORD count = 0;

int main(int nargs,char *args[])
{
    FILE *fp;
    BYTE i,p[150];
    int r;

    switch(nargs) {
        case 1:
            puts("HEAD.COM v.1.0 by Wolf Bergenheim. Displays The top of a file.");
            puts("usage: HEAD [-N lines | -C count | -H | -?] filename.ext");
            return 1;
        case 2:
            if(stricmp(args[1],"-H") == 0) {
                puts("HEAD.COM v.1.0 by Wolf Bergenheim. Displays The top of a file.");
                puts("usage: HEAD [-N lines | -C count | -H | -?] filename.ext");
                return 0;
            }
            if(stricmp(args[1],"-?") == 0) {
                puts("HEAD.COM v.1.0 by Wolf Bergenheim. Displays The top of a file.");
                puts("usage: HEAD [-N lines | -C count | -H | -?] filename.ext");
                return 0;
            }
            
            fp = fopen(args[1],"r");
            if (fp == NULL) {
                fprintf(stderr,"HEAD.COM: Unable to open file: %s\n",args[1]);
                return 2;
            }

            break;
        case 3:

            puts("Invalid number of arguments");
            puts("HEAD.COM v.1.0 by Wolf Bergenheim. Displays The top of a file.");
            puts("usage: HEAD [-N lines | -C count | -H | -?] filename.ext");
            return 1;
        case 4:
            if(stricmp(args[1],"-N") == 0) {
                fp = fopen(args[3],"r");
                if (fp == NULL) {
                    fprintf(stderr,"HEAD.COM: Unable to open file: %s\n",args[1]);
                    return 2;
                }
                lines = atoi(args[2]);
            }
            else if(stricmp(args[1],"-C") == 0) {
                fp = fopen(args[3],"rb");
                if (fp == NULL) {
                    fprintf(stderr,"HEAD.COM: Unable to open file: %s\n",args[1]);
                    return 2;
                }
                count  = atoi(args[2]);
                for(i=0;(i<count) && ((r = fgetc(fp)) != -1);i++) {
                    putchar((BYTE)r);
                }
                return 0;
            }
            else {
                printf("Unknown switch \"%s\"\n",args[2]);
                puts("HEAD.COM v.1.0 by Wolf Bergenheim. Displays The top of a file.");
                puts("usage: HEAD [-N lines | -C count | -H | -?] filename.ext");
                return 1;
            }
            break;

        default:
            puts("HEAD.COM v.1.0 by Wolf Bergenheim. Displays The top of a file.");
            puts("usage: HEAD [-N lines | -C count | -H | -?] filename.ext");
            return 1;
    }


    for(i=0;(i<lines) && (fgets(p,149,fp) != NULL);i++) {
        printf("%s",p);
    }

    return 0;
}
