/*

LS.C - DOG - Alternate command processor for (currently) MS-DOS ver 3.30

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
18.03.00 - Extracted from DOG.C - WB
01.08.00 - Fixed a bug: when you type ls file that doesn't exist ls
           would loop in DR-DOS because it returns 1, not 18 as I
           thought it would... - WB

****************************************************************************/

#ifdef port
#include "dog.h"
#endif

void do_ls( BYTE n)
{

    int  attrs;
    BYTE *patt[20],dironly;
    BYTE any[4];
    BYTE t,r,i,l,j,k,*p;
    BYTE doit;
    WORD h, min, sec, y, m, d;
    struct ffblk *fb;

    dironly = 0;
    doit = 1;
    strcpy(any,"*.*");
    p = any;
    patt[0] = p;
    r = 0;
    fb = malloc(sizeof(struct ffblk));

#ifdef ls_debug
fprintf(stderr,"do_ls:0: patt[0]=%s; n=%d; any=%s; doit=%u;\n",patt[0],n,any,doit);
#endif

    attrs = 0|FA_DIREC;
    l = 1;
    k = 0;

    if (n>0) {
        for(i=1;i<n;i++) {

#ifdef ls_debug
fprintf(stderr,"do_ls:1: patt[0]=%s\n",patt[0]);
#endif
            if (arg[i][0] == '-') {
                switch(arg[i][1]) {
                    case 'a':
                        attrs = FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN|FA_DIREC|FA_LABEL;
                        break;
                    case 'd':
                        dironly = 1;
                        break;
                    case 'f':
                        attrs = FA_NORMAL;
                        break;
                    case 'h':
                        printf("The switches are:\n\t-a: List ALL files and directories\n");
                        printf("\t-d: list DIRECTORIES only\n\t-f: list normal FILES only\n\t-l: show volume label\n");
                        free(fb);
                        return;
                    case 'l':
                        attrs = FA_LABEL;
                        break;
                    case '?':
                        printf("ls [-a|-d|-f|-h|-l|-?] [d:\\dir\\filename.ext]\n");
                        free(fb);
                        return;
                    default:
                        printf("Incorrect switch %s\n",arg[i]);
                        free(fb);
                        return;
                }
            }
            else if(strnicmp(arg[i],"/",1)==0) {
                puts("In DOG we use '-' as switch NOT '/'.");
                free(fb);
                return;
            }
            else {
                patt[k++] = arg[i];
#ifdef ls_debug
printf("do_ls:2: patt[0]=%s hxpatt=%x\n",patt[0],patt[0]);
#endif
            }
        }
    }

    if (k > 0)
        (l = k);
    else
        (l = 1);

    for(j=0;j < l;j++) {
        if(doit>0) {

            if(l == 0)
              patt[0] = any;

#ifdef ls_debug
fprintf(stderr,"do_ls:3: patt[%u]=%s offset=%x patt[%d][last] =  %c\n",j,patt[0],patt[0],j,patt[j][strlen(patt[j])-1]);
#endif
            if((patt[j][strlen(patt[j])-1] == '\\') || (patt[j][strlen(patt[j])-1] == ':'))
                strcat(patt[j],any);

            r = findfirst(patt[j], fb,attrs);

            if((dironly == 1) && ((fb->ff_attrib & FA_DIREC) != FA_DIREC))
                r = 1;
#ifdef ls_debug
fprintf(stderr,"do_ls:*: r=%u\n",r);
#endif

            if (r == 0) {
                if(cBreak) {
                    cBreak = 0;
                    free(fb);
                    return;
                }

                printf("    size   ASRHD   dd.mm.yyyy   hh:mm.ss   name\n\n");

                printf("%8ld   ",fb->ff_fsize);

                if (((fb->ff_attrib & FA_ARCH) == FA_ARCH) && (fb->ff_attrib & FA_LABEL != FA_LABEL)) {
                    printf("A");
                }

                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
                else printf("-");

                if ((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM) printf("S");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("A");
                else printf("-");

                if ((fb->ff_attrib & FA_RDONLY) == FA_RDONLY) printf("R");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("B");
                else printf("-");

                if ((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN) printf("H");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("E");
                else printf("-");

                if ((fb->ff_attrib & FA_DIREC) == FA_DIREC) printf("D");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
                else printf("-");

                y = ((fb->ff_fdate & 0x0fe00)>>9);
                y += 1980;
                m = (fb->ff_fdate &  0x01e0)>>5;
                d = (fb->ff_fdate &  0x001f);

                printf("   %02d.%02d.%04d   ",d,m,y);

                h = (fb->ff_ftime & 0x0F800)>>11;
                min = (fb->ff_ftime & 0x03E0)>>5;
                sec = (fb->ff_ftime & 0x001f)<<1; /* *2 */

                printf("%02d:%02d.%02d   ",h,min,sec);


                printf("%s\n",fb->ff_name);


            }

#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\ndate:%d time:%d\n",r,fb->ff_fdate,fb->ff_ftime);
fprintf(stderr,"y=%d y=%d\n",y,1998);

#endif
            k = 0;
            r = findnext (fb);

            while(r == 0) {

            if(cBreak) {
                cBreak = 0;
                free(fb);
                return;
            }

#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\n",r);
#endif

                if((dironly == 1) && (k == 1))
                    r = findnext(fb);

                if((dironly == 1) && ((fb->ff_attrib & FA_DIREC) != FA_DIREC)){
                    k = 1;
                    continue;
                }

                printf("%8ld   ",fb->ff_fsize);

                if (((fb->ff_attrib & FA_ARCH) == FA_ARCH) && (fb->ff_attrib & FA_LABEL != FA_LABEL)) {
                    printf("A");
                }

                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
                else printf("-");

                if ((fb->ff_attrib & FA_SYSTEM) == FA_SYSTEM) printf("S");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("A");
                else printf("-");

                if ((fb->ff_attrib & FA_RDONLY) == FA_RDONLY) printf("R");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("B");
                else printf("-");

                if ((fb->ff_attrib & FA_HIDDEN) == FA_HIDDEN) printf("H");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("E");
                else printf("-");

                if ((fb->ff_attrib & FA_DIREC) == FA_DIREC) printf("D");
                else if((fb->ff_attrib & FA_LABEL) == FA_LABEL) printf("L");
                else printf("-");


                y = ((fb->ff_fdate & 0x0fe00)>>9)+1980;

                d = (fb->ff_fdate &  0x001f);

                printf("   %02d.%02d.%04d   ",d,m,y);

                h = (fb->ff_ftime & 0x0F800)>>11;
                min = (fb->ff_ftime & 0x03E0)>>5;
                sec = (fb->ff_ftime & 0x001f)<<1; /* *2 */

                printf("%02d:%02d.%02d   ",h,min,sec);


                printf("%s\n",fb->ff_name);



            r = findnext(fb);
            k = 0;

        }


#ifdef ls_debug
fprintf(stderr,"do_ls: r=%d\n",r);
#endif
        }
    }
    free(fb);
    return;
}


