/*

MV.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

TODO: Wildcard recognition (*.*, etc)

History
18.03.00 - Extracted from DOG.C - WB
20.10.00 - Renamed nn to mv (It should havee been named mv from the beginning)
           Fixed do_mv. - WB

**************************************************************************/

#ifdef port
#include "dog.h"
#endif

WORD newname(BYTE *oldname, BYTE *newname);

void do_mv(BYTE n)
{
    BYTE *p, b, ndir[129], odir[129], nn[129]={0},on[129]={0},tn[12]={0};
#ifdef MV_WILD
    BYTE fn[129]={0}, wild=0;
#endif
    WORD r;
    struct ffblk ffb,*fb=&ffb;

    if(n != 3) {
        puts("Invalid number of arguments.");
        return;
    }

    b = findfirst(arg[1],fb,0);

    while (b == 0) {

        strcpy(odir,arg[1]);
        p = odir;

        /* separate path from filename */
        for(p+=strlen(odir)+1;(*p!='\\') && (p > odir);p--);
#ifdef debug_mv
printf("%s:odir=(%p)%s,p=(%p)%s\n",__FILE__,odir,odir,p,p);
#endif
        if (p>odir) {
            *(++p) = '\0';
        }
        else
            strcpy(odir,"");

        strcpy(ndir,arg[2]);
        p = ndir;

        /* separate path from filename */
        for(p+=strlen(ndir)+1;(*p!='\\') && (p > ndir);p--);
        strcpy(tn,p);
#ifdef debug_mv
printf("%s:ndir=(%p)%s,p=(%p)%s\n",__FILE__,ndir,ndir,p,p);
#endif
        if (p>ndir) {
            *(p) = '\0';
        }
        else
            strcpy(ndir,"");

#ifdef debug_mv
printf("%s:odir = /%s/ ndir = /%s/\n",__FILE__,odir,ndir);
#endif

#ifdef MV_WILD
        if(wild == 1) {
            if(trueName(nn,arg[2]) == NULL) {
                printf("Malformed path:\n%s\n",arg[2]);
                /* break */
            }

            if(trueName(on,fn) == NULL) {
                printf("Malformed path:\n%s\n",arg[1]);
                /* break */
            }

            for(b=0;nn[b] != '\0';b++) {
                if(nn[b] == '?')
                    nn[b]=on[b];
            }
        }
#else
        strcpy(on,odir);
        strcat(on,fb->ff_name);
        strcpy(nn,ndir);
        strcat(nn,tn);
#endif
        printf("%s --> %s\n",on,nn);
        r = newname(on,nn);

        if(r == 255) {
            switch(r) {
						 case ENOFILE:
								printf("%s NOT found.\n",on);
								break;
						 case ENOPATH:
								puts("Path not found.");
								break;
						 case EACCES:
								puts("Access denied.");
								break;
						 case EINVFMT:
								puts("Invalid format.");
								break;
            }
        }

        b = findnext(fb);
    }
    return;

}

/**************************************************************************/

WORD newname(BYTE *oldname, BYTE *newname)
{
    WORD r;
    
	  asm mov ah,56h
		asm push cs
		asm pop es
		asm mov dx,oldname
		asm mov di,newname
		asm int 21h
		asm jnc  nn_ok
		asm mov r,ax

    return r;
    nn_ok:
    return 0;
}

/****************************************************************************/

BYTE *trueName(BYTE *name,BYTE *tn)
{
  	asm push ds
	 	asm pop es
		asm mov si,name
		asm mov di,tn
		asm mov ah,60h
		asm int 21h
		asm jnc tn_ok
    
    return NULL;
    tn_ok:
    return tn;

}


