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

*/

#ifdef port
#include "dog.h"
#endif

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
    BYTE f;

    if (n == 1) {
        puts("Required argument missing");
        return;
    }
    if (n > 2) {
        puts("Invalid number of arguments.");
        return;
    }

    f = arg[0][0];        /* check the first letter to determine what func*/

    asm mov AL,f
    asm cmp AL,43h        /* C */
    asm je    do_mrd_cd
    asm cmp AL,4dh        /* M */
    asm je    do_mrd_md
    asm cmp AL,52h        /* R */
    asm je    do_mrd_rd
    asm cmp AL,63h        /* c */
    asm je    do_mrd_cd
    asm cmp AL,6dh        /* m */
    asm je    do_mrd_md
    asm cmp AL,72h        /* r */
    asm je    do_mrd_rd

    do_mrd_md:            /* mkdir */

    asm mov AH,39h
    asm jmp do_mrd_doit

    do_mrd_rd:            /* rmdir */

    asm mov AH,3ah
    asm jmp do_mrd_doit

    do_mrd_cd:            /* chdir */

    asm mov AH,3bh

    do_mrd_doit:

    asm mov DX,arg[2]    /*=arg[1] = first arg after command */
    asm int 21h
    asm jnc    do_mrd_OK
    asm cmp AL,03h
    asm je    do_mrd_pnf
    asm cmp AL,05h
    asm je    do_mrd_ad
    asm cmp AL,06h
    asm je    do_mrd_ih
    asm jmp do_mrd_rcd

    do_mrd_pnf:
    puts("Invalid path.");
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


