/*

CP.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

TODO: Make the whole thing!

History
18.03.00 - Extracted from DOG.C - WB

****************************************************************************/

#pragma argsused
void do_cp( BYTE n)
{
    printf("Sorry NOT implemented yet. Use xcopy.\n");
/*
    FILE *src,*trg;
    BYTE i,*buff;
    struct ffblk ffb;

    if(findfirst(arg[2],ffb,0+1+2+4+8)) {
        fprintf(stderr,"Replace %s (Yes/No)? ");
        i = getchar();
        if((i == 'n') || (i == 'N'))
            return;
    }
    i = findfirst(arg[1],ffb,0);
        if (i == 18)
            fprintf(stderr,"File %s NOT found",arg[1]);

    src = fopen(arg[1],"br");
    trg = fopen(arg[2],"bw");
    if(src ==  NULL) {
        fprintf(stderr,"File %s NOT found",arg[1]);
        return;
    }
    if(trg ==  NULL) {
        fprintf(stderr,"Error while creating file %s",arg[2]);
        return;
    }




*/
}


