/*

CMRD.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

NOTE! Includes functions do_cd() and do_mrd()

Developers:
Wolf Bergenheim (WB)

History
18.03.00 - Extracted from DOG.C - WB
2024-05-11 - Building as a module. - WB
2024-11-19 - Added -p to MD to allow creating a full path - WB
*/
#include "dog.h"

void do_cd( BYTE n)
{
    BYTE i,j,k,dir[80],*p,*q;

    j=strlen(arg[0]);


    /* checking if user typed cd..(something)  OR cd\path*/

    if(j>2) {
        arg[1] = &arg[0][2];
        arg[0][1] = '\0';

        n++;
    }
        /* for every dot(.) after .. change to \.. */


    p = arg[1];
    q = dir;

    for(i=0;i<80;i++) {
        dir[i]=0;
    }

    while(*p!=0) {
        if(*p=='.'){
            for(i=0;*p=='.';i++) {
                *(q++) = *(p++);
            }
            q--;
            if(i>2){
                i-=2;
                for(j=0;j<i;j++) {
                    *(q--) = '\0';
                }
                for(j=0;j<i;j++) {
                    strcat(dir,"\\..");
                    q+=3;
                }
            }
            q++;
        }
        else {
            *(q++)=*(p++);
        }
    }
    *q='\0';
    strcpy(arg[1],dir);

    /* NO arguments provided.. so print current dir */

    if(n==1) {
        D = getcur(P) + 'A';
        if (D >= 'A')
            printf("%c:\\%s\n",D,P);
        return;
    }
    else {
        do_mrd(n);
        return;
    }

}

/**************************************************************************/

void do_mrd(BYTE n)
{
    BYTE f, r=0, *p, *q;

    f = toupper(arg[0][0]);        /* check the first letter to determine what func*/
    p = arg[1];
    if (n == 1) {
        puts("Required argument missing");
        return;
    }
    if (n == 3) {
	if ((arg[1][0] == '-') && (toupper(arg[1][1]) == 'P') && (f == 'M')) {
	    r = 1;
	    p = arg[2];
	}
    }
    if ((n > 2) && (r == 0)) {
        puts("Invalid number of arguments.");
        return;
    }

    asm mov AL, f;
    asm cmp AL, 'C';
    asm je    do_mrd_cd;
    asm cmp AL, 'M';
    asm je    do_mrd_md;
    asm cmp AL, 'R';
    asm je    do_mrd_rd;

do_mrd_md:
    asm mov AH, 39h;      /* mkdir */
    asm jmp do_mrd_doit;

do_mrd_rd:
    asm mov AH, 3ah;      /* rmdir */
    asm jmp do_mrd_doit;

do_mrd_cd:
    asm mov AH,3bh;       /* chdir */

do_mrd_doit:
    asm mov DX, p;        /* path to act on */
    asm int 21h;
    asm jnc    do_mrd_OK;
    asm cmp AL, 03h;      /* path not found */
    asm je    do_mrd_pnf;
    asm cmp AL,05h;       /* access denied */
    asm je    do_mrd_ad;
    asm cmp AL,06h;
    asm je    do_mrd_ih;   /* invalid handle */
    asm jmp do_mrd_rcd;    /* rm current dir */

do_mrd_pnf:
    if (r == 1) { /* recusrsive path creation was requested */
	q = strrchr(p, '\\');
	*q = '\0';
	do_mrd(n);
	*q = '\\';
	do_mrd(n);
    } else {
	puts("Invalid path.");
    }
    return;
    do_mrd_ad:
    puts("Access denied.");
    return;
    do_mrd_ih:
    puts("Invalid handle.");
    return;
    do_mrd_rcd:
    puts("Attempted to remove current directory.");
    do_mrd_OK:
    return;

}
