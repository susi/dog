/*

NN.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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

**************************************************************************/

WORD newname(BYTE *oldname, BYTE *newname);

void do_nn(BYTE n)
{
    BYTE *p, wild=0, b, fn[129]={0}, dir[80],nn[129]={0},on[129]={0};
    WORD r;
    struct ffblk ffb,*fb=&ffb;

    if(n != 3) {
        puts("Invalid number of arguments.");
        return;
    }
    
    b = findfirst(arg[1],fb,0);

    while (b != 18) {
    
        p = arg[1];
        
        /* separate path from filename */
        for(p+=strlen(arg[1])+1;*p!='\\';p--);
        
        if(p>arg[1]) {
            *(++p)=0;
            strcpy(fn,arg[1]);
            strcat(fn,fb->ff_name);
        }
        else
            strcpy(fn,fb->ff_name);
        
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
        puts(on); puts(nn);
        r = newname(on,nn);
    
        if(r != 0) {
            switch(r) {
                case 02:
                    printf("%s NOT found.\n",on);
                    break;
                case 03: 
                    puts("Path not found.");
                    break;
                case 05: 
                    puts("Access denied.");
                    break;
                case 11: 
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
    asm{
        mov ah,56h
        push cs
        pop es
        mov dx,oldname
        mov di,newname
        int 21h
        jnc  nn_ok
        mov r,ax
    }
    return r;
    nn_ok:
    return 0;
}

/****************************************************************************/

BYTE *trueName(BYTE *name,BYTE *tn)
{
    asm{
        push ds
        pop es
        mov si,name
        mov di,tn
        mov ah,60h
        int 21h
        jnc tn_ok
    }
    return NULL;
    tn_ok:
    return tn;

}


