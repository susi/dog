/*

TP.C - DOG - Alternate command processor for freeDOS

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
02.01.02 - Bugfix: return errorcode 1 on missing filename - WB

**************************************************************************/

#include "ext.h"

 /* 1 kB buffer */
#define BUFF_SIZE 1024

int main(BYTE n, BYTE *arg[])
{
    FILE *fp;
    struct ffblk *fb;
    BYTE i,j,r,*buff,dir[200],savedir[200],dir_flag;

	 buff = malloc(BUFF_SIZE);

    if(n == 1) {
        fprintf(stderr,"Missing filename.\n");
        return 1;
    }
    else if(n >1) {

    fb = malloc(sizeof(struct ffblk));
        for(i=1;i<n;i++) {
            dir_flag = 0;
            r = findfirst(arg[i],fb,FA_NORMAL);
            if(r==0) {
            strcpy(dir,arg[i]);
                for(j=strlen(dir);j>0;j--) {
#ifdef debug_tp
printf("dir[%d]=%c\n",j,dir[j]);
#endif
                    if(dir[j] == '\\') {
                        dir[j+1] = '\0';
                        dir_flag = 1;
                        break;
                    }
                }
                strcpy(savedir,dir);
#ifdef debug_tp
printf("dir=%s\n",dir);
#endif

                printf("\n---------------%s---------------\n\n",fb->ff_name);
                if (dir_flag == 1)
                    strcat(dir,fb->ff_name);
                else
                    strcpy(dir,fb->ff_name);

                fp = fopen(dir,"r");
                if (fp == NULL) {
                    fprintf(stderr,"\n****Error opening file %s\n",dir);
                }
                else {
                    while(fgets(buff,BUFF_SIZE,fp)) {
/*                        printf("%s",buff);*/
                        write(1, buff, strlen(buff)+1);
                    }
                    fclose(fp);
                }
            }
            else if((r == 255) && (errno !=ENMFILE)) {
								switch(errno) {
								 case ENOFILE:
										fprintf(stderr,"\n****File not found: %s\n",arg[i]);
										break;
								 case ENOPATH:
										fprintf(stderr,"\n****File not found: %s (Invalid path)\n",arg[i]);
										break;
								 case EMFILE:
										fprintf(stderr,"\n****File not found: %s (Too many open files)\n",arg[i]);
										break;
								 default:
										fprintf(stderr,"\n****File not found: %s (DOS error (%x))\n",arg[i],errno);
										break;

								}
								continue;
						}

            r = findnext(fb);
            while(errno != 2) {
                printf("\n---------------%s---------------\n\n",fb->ff_name);
                strcpy(dir,savedir);
                if (dir_flag == 1)
                    strcat(dir,fb->ff_name);
                else
                    strcpy(dir,fb->ff_name);

                fp = fopen(dir,"r");
                if (fp == NULL) {
                    fprintf(stderr,"\n****Error opening file %s\n",dir);
                }
                else {
                    while(fgets(buff,BUFF_SIZE,fp) != 0) {
/*                        printf("%s\n",buff);*/
                        write(1, buff, strlen(buff)+1);
                    }
                    fclose(fp);
                }
                r = findnext(fb);
            }

            if((r == 255) && (errno !=ENOFILE)) {
								switch(errno) {
/*
                    case ENOFILE:
										fprintf(stderr,"\n****File not found: %s\n",arg[i]);
										break;
 */
								 case ENOPATH:
										fprintf(stderr,"\n****File not found: %s (Invalid path)\n",arg[i]);
										break;
								 case EMFILE:
										fprintf(stderr,"\n****File not found: %s (Too many open files)\n",arg[i]);
										break;
								 default:
										fprintf(stderr,"\n****File not found: %s (DOS error (%x))\n",arg[i],errno);
										break;

								}
						}

        }

    }
    free(fb);
	 free(buff);
	 return 0;
}

