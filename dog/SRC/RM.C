/*

RM.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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
18.03.00 - Extracte Dbbbbbbbbeeee ,mk”lo.-p„*
 from DOG.C - WB

**************************************************************************/

#ifdef port
#include "dog.h"
#endif

void do_rm(BYTE n)
{
    BYTE r,f,i,*p,fn[129]={0},dir[80];
    struct ffblk *fb;

    if(n > 1) {
        fb = malloc(sizeof(struct ffblk));

        for(i=1;i<n;i++) {
            if ((p=strstr(arg[i],"*.*"))!=NULL) {
                printf("Removing %s - Are you sure(Y/N)? ",arg[i]);
                /* Get character */

                asm mov ah,1
                asm int 21h
                asm mov r,al
                
                putchar('\n');
                putchar('\r');
            
                /* get to next arg if not 'Y' or 'y' */
                if((r != 'y') && (r != 'Y'))
                    break;
            }
            
            f=findfirst(arg[i],fb,0);

            if(f==0) {

                if(cBreak) {
                    cBreak = 0;
                    return;
                }

                p = arg[i];
                for(p+=strlen(arg[i])+1;*p!='\\';p--);
                if(p>arg[i]) {
                    *(++p)=0;
                    strcpy(fn,arg[i]);
                    strcat(fn,fb->ff_name);
                }
                else
                    strcpy(fn,fb->ff_name);
                    
                r=unlink(fn);

                if(r==0) {
                    puts(fn);
                }
            }
            else if((f==255) && (errno==2)) {
                printf("%s NOT found.\n",arg[i]);
            }
            r=findnext(fb);
            while(r==0) {
                if(cBreak) {
                    cBreak = 0;
                    return;
                }

                for(p+=strlen(arg[i])+1;*p!='\\';p--);
                if(p>arg[i]) {
                    *(++p)=0;
                    strcpy(fn,arg[i]);
                    strcat(fn,fb->ff_name);
                }
                else
                    strcpy(fn,fb->ff_name);

                r=unlink(fn);
                
                if(r==0) {
                    puts(fn);
                }
                r=findnext(fb);
            }
        
        }
        free(fb);
    }
        
    else puts("Invalid number of arguments.");  
    return;
}


