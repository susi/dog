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

NOTE: Compile with Micro-C 3.15: CC which.c -fop

History

19.07.99 - Created by Wolf Bergenheim
26.07.99 - Added switches
*/
#include <stdio.h>
#include <file.h>

#define BYTE unsigned char
#define WORD unsigned int

#define FULL   (1)
#define NFLAGS 4

#define _A 0
#define _E 1
#define _X 2
#define _F 3

BYTE e[3][5] = {".COM",".EXE",".DOG"}
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
    BYTE i,j,env[200],*p,env_name[80];
    struct FF_block b,*q;
    q=b;
    strcpy(env_name,"PATH");
    strcpy(name,args[1]);

    if(nargs == 1) {
        puts("WHICH.COM v.1.0 by Wolf Bergenheim a utilityprogram to DOG.");
        puts("usage: WHICH [program] [-A | -E env.variable] | -F | -X]");
        puts("\n-F = Full info, -X = use DOS .BAT files in stead of DOG .DOG files");
        return 0;
    }

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

/*    printf("flags %b\n",f); */


    check(".",0);
    check(".",1);
    check(".",2);

    getenv(env_name,env);
    strupr(env);
    for(p=strtok(env,";");;p=strtok(NULL,";")) {
        if(p==NULL) return 0;

        check(p,0);
        check(p,1);
        check(p,2);
    }


    return 0;
}


void check(BYTE *p,BYTE ext)
{

    BYTE s[PATH_SIZE + 11],i,c,size[11];
    struct FF_block b,*q;

    q=b;
    c = '';

    strcpy(s,p);
    if(s[strlen(s)-1] != '\\') {
        strcat(s,"\\");
        c = '\\';
    }
    strcat(s,name);
    strcat(s,e[ext]);
    i=findfirst(s,q,0);

    if(i==0) {
        printf("%s%c%s",p,c,b.FF_name);
        if(f == FULL) {
            asm {
                mov ah,03h
                xor bx,bx
                int 10h
                mov ah,02h
                cmp dl,40
                jbe same_row
                inc dh
                same_row:
                mov dl,40
                int 10h

            }


            printf("%02d.%02d.%4d",b.FF_date & 0x1F,(b.FF_date >> 5) & 0x0f,(b.FF_date>>9)+1980);
            printf(" %2d.%02d ",b.FF_time >> 11, (b.FF_time & 0x7e0) >> 5);


            if ((b.FF_attrib & ARCHIVE) == ARCHIVE) printf("A");
            else if((b.FF_attrib & VOLUME) == VOLUME) printf("L");
            else printf("-");
                
            if ((b.FF_attrib & SYSTEM) == SYSTEM) printf("S");
            else if((b.FF_attrib & VOLUME) == VOLUME) printf("A");
            else printf("-");

            if ((b.FF_attrib & READONLY) == READONLY) printf("R");
            else if((b.FF_attrib & VOLUME) == VOLUME) printf("B");
            else printf("-");

            if ((b.FF_attrib & HIDDEN) == HIDDEN) printf("H");
            else if((b.FF_attrib & VOLUME) == VOLUME) printf("E");
            else printf("-");
                
            if ((b.FF_attrib & DIRECTORY) == DIRECTORY) printf("D");
            else if((b.FF_attrib & VOLUME) == VOLUME) printf("L");
            else printf("-");

            ltoa(b.FF_size,size,10);
            printf("%9s",size);

        }
        puts("");

    }
    return;
}
