/*
WHICH.C - version 1.0
    Finds programs in path-like environment variables.

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

History

19.07.99 - Created by Wolf Bergenheim
26.07.99 - Added switches
2000.10.21 - Ported to BCC - WB
*/
#include "util.h"

#define FULL   (1)
#define NFLAGS 4

#define _A 0
#define _E 1
#define _X 2
#define _F 3

BYTE e[3][5] = {".COM",".EXE",".DOG"};
BYTE name[80]={0};
BYTE f = 0;
BYTE flags[4][3] = {
    "-A",
    "-E",
    "-X",
    "-F"};


void check(BYTE *p,BYTE ext);

int main(int nargs,char *args[])
{
    BYTE i,j,*env_val,*p,env_name[80];

    if(nargs == 1) {
        puts("WHICH.COM v.1.0 by Wolf Bergenheim a utilityprogram to DOG.");
        puts("usage: WHICH [program] [-A | -E env.variable] | -F | -X]");
        puts("\n-F = Full info, -X = use DOS .BAT files in stead of DOG .DOG files");
        return 0;
    }

    strcpy(env_name,"PATH");
    strcpy(name,args[1]);

    for(i=2;i<nargs;i++) {
        for(j=0;j<NFLAGS;j++) {
            if(stricmp(args[i],flags[j])==0)
                break;
        }
        switch(j) {
            case _A:
                strcpy(env_name,"APPEND");
                break;
            case _E:
                strupr(args[++i]);
                strcpy(env_name,args[i]);
                break;
            case _X:
                puts("DOG is an alternative for COMMAND.COM.\nPlease contact Wolf Bergenheim (dog__@hotmail.com) for more information");
                strcpy(e[2],".BAT");
                break;
            case _F:
                f = FULL;
                break;
        }

    }


    check(".",0);
    check(".",1);
    check(".",2);

    env_val = getenv(env_name);
    strupr(env_val);
    for(p=strtok(env_val,";");;p=strtok(NULL,";")) {
        if(p==NULL) break;

        check(p,0);
        check(p,1);
        check(p,2);
    }


    return 0;
}


void check(BYTE *p,BYTE ext)
{

    BYTE s[PATH_SIZE + 11],i,c,size[11];
    struct ffblk b,*q;

    q=&b;
    c = '\0';

    strcpy(s,p);
    if(s[strlen(s)-1] != '\\') {
        strcat(s,"\\");
        c = '\\';
    }
    strcat(s,name);
    strcat(s,e[ext]);
    i=findfirst(s,q,0);

    if(i==0) {
        printf("%s%c%s",p,c,b.ff_name);
        if(f == FULL) {
            asm {
                mov ah,03h
                xor bx,bx
                int 10h
                mov ah,02h
                cmp dl,40
                jbe same_row
                inc dh
            }
                same_row:
            asm {
                mov dl,40
                int 10h

            }

            printf("%02d.%02d.%4d",b.ff_fdate & 0x1F,(b.ff_fdate >> 5) & 0x0f,(b.ff_fdate>>9)+1980);
            printf(" %2d.%02d ",b.ff_ftime >> 11, (b.ff_ftime & 0x7e0) >> 5);


            if ((b.ff_attrib & FA_ARCH) == FA_ARCH) printf("A");
            else if((b.ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
            else printf("-");
                
            if ((b.ff_attrib & FA_SYSTEM) == FA_SYSTEM) printf("S");
            else if((b.ff_attrib & FA_LABEL) == FA_LABEL) printf("A");
            else printf("-");

            if ((b.ff_attrib & FA_RDONLY) == FA_RDONLY) printf("R");
            else if((b.ff_attrib & FA_LABEL) == FA_LABEL) printf("B");
            else printf("-");

            if ((b.ff_attrib & FA_HIDDEN) == FA_HIDDEN) printf("H");
            else if((b.ff_attrib & FA_LABEL) == FA_LABEL) printf("E");
            else printf("-");
                
            if ((b.ff_attrib & FA_DIREC) == FA_DIREC) printf("D");
            else if((b.ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
            else printf("-");

            ltoa(b.ff_fsize,size,10);
            printf("%9s",size);

        }
        puts("");

    }
    return;
}
