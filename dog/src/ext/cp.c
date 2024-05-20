/*

CP.C - DOG - Alternate command processor for freeDOS

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
Hari Kirian K (HK)

TODO: Make the whole thing!

History
18.03.00 - Extracted from DOG.C - WB
2002-02-22 - rewrote in C - no ASM - WB
2002-07-27 - added wild cards - HK
2002-07-27 - added wild cards - bugfix - HK
2002-31-10 - changed the way cp displays files
2002-11-23 - Added help text, prints to std err - WB
2002-11-24 - Added support for target directory. In 3] target doesn't
                          have to have \*.* prefix
2002-11-25 - Added support for 4] and 5]. -WB
2024-05-21 - Using Int 21h/AH=01 to read kbd for Yes/No/Cancel/All.
             Added -f flag for force overwrite. -WB

    1]File Copy:
    ------------
      CP *.* X:\Somepath\*.*
         ^             ^
         |             |______________
         |                           |
         Source without drive        | Destination with complete
         or directory means the      | path information.
         current working directory.  |

    2]File Copy:
    ------------
      CP X:\Somepath\*.* *.*
         ^               ^
         |               |____________
         |                           |
         Source with complete        | Destination with incomplete
         path information.           | path information means current
                                     | working  directory.

    3]File Copy:
    ------------
      CP X:\Somepath\*.* X:\Somepath\*.*
         ^               ^
         |               |____________
         |                           |
         Source with complete        | Destination with complete
         path information.           | path information.
                                     |
    4]File copy:
    ------------
      CP X:\Somepath
         ^
         |
         Can't handle COMMAND.COM does
            2002-11-24 now it can :) - WB

    5]File copy:
    ------------
      CP X:\Somepath X:\Somepath
         ^
         |
         Can't handle COMMAND.COM does
            2002-11-25 now it can :) - WB
***********************************************************************/
#include <io.h>
#include <fcntl.h>
#include "ext.h"

/* BUFF_SIZE = 10240 Bytes / 16 bytes/paragraph = 640 16-byte-paragraps */
#define BUFF_SIZE 10240
#define BUFF_PARA 280h

struct ts_args
{
     BYTE **patt;
     BYTE npatt;
}cp_p;

BYTE flag_i = FLAG_UNSET;
BYTE flag_v = FLAG_UNSET;
BYTE flag_h = FLAG_UNSET;
BYTE flag_f = FLAG_UNSET;

BYTE wild[4] = {'*','.','*','\0'};

void print_help(void);
BYTE read_key(void);

int main(BYTE n, BYTE *arg[])
{
    signed char i;
    int r, nfil = 0, done, j, k, c, attrib;
    long fskspc;
    BYTE key;
    BYTE sn_fil[MAXPATH+13], st_fil[MAXPATH+13], dn_fil[MAXPATH+13], dt_fil[MAXPATH+13];
    BYTE s_drv[MAXDRIVE], s_dir[MAXDIR], s_fil[MAXFILE], s_ext[MAXEXT];
    BYTE d_drv[MAXDRIVE], d_dir[MAXDIR], d_fil[MAXFILE], d_ext[MAXEXT];
    WORD *buff;
    DWORD src_sz, trg_sz;
    FILE *src, *dst;
    struct ffblk ffb;
    struct dfree dfree;

    k = 0;
    cp_p.npatt = 0;

    for(i=1;i<n;i++) {
#ifdef CP_DEBUG
        printf("flags: arg = %s\n",arg[i]);
#endif
        if ((arg[i][0] == '-') || (arg[i][0] == '/')) {
            for(j=1;arg[i][j]!='\0';j++) {
                switch(arg[i][j]) {
                 case 'i':
                    flag_i = FLAG_SET;
                    break;
                 case 'H':
                 case 'h':
                 case '?':
                    print_help();
                        return 0;
                 case 'v':
                    flag_v = FLAG_SET;
                    break;
		case 'f':
		    flag_f = FLAG_SET;
		    break;
                }
            }
        }
        else {
            cp_p.patt[k] = arg[i];
#ifdef CP_DEBUG
	    printf("rm:2: cp_p.patt[%d]=(%s) hxcp_p.patt=0x%x\n",
		   k,cp_p.patt[k],cp_p.patt[k]);
	    printf("rm:2: arg[%d]=(%s) hxarg=0x%x\n",i,arg[i],cp_p.patt[i]);
#endif
            k++;
        }
    }

    cp_p.npatt = k;

    if(cp_p.npatt == 0){
        fputs("Missing source and destination",stderr);
        print_help();
        return -1;
    }
    else if(cp_p.npatt == 1){
        cp_p.patt[1] = wild;
        cp_p.npatt++;
    }
    else if(cp_p.npatt > 2) {
        fputs("Too many file arguments",stderr);
        print_help();
        return -1;
    }

    strcpy(st_fil, cp_p.patt[0]);
    attrib = _chmod(st_fil,0);
    if (attrib != -1) {
        if (attrib & FA_DIREC) {
            strcat(st_fil, "\\*.*");
        }
    }

    i = findfirst(st_fil, &ffb, FA_NORMAL|FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN);
    if (i == -1) {
        fprintf(stderr,"File %s NOT found\n",cp_p.patt[0]);
        return -1;
    }

    if ((buff = malloc(BUFF_SIZE)) == NULL) {
        fprintf(stderr,"Not enough memory!\n");
        return -1;
    }

#if CP_DEBUG
    printf("cp : %d : st_fil = (%s)\n", __LINE__, st_fil);
#endif

    /*some types of error were handled above.*/
    fnsplit(st_fil, s_drv, s_dir, s_fil, s_ext);
    fnsplit(cp_p.patt[1], d_drv, d_dir, d_fil, d_ext);

    strcpy(dt_fil, cp_p.patt[1]);

    done = findfirst(st_fil, &ffb, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH | FA_NORMAL);
    while(!done){
        strcpy(sn_fil, s_drv);
        strcat(sn_fil, s_dir);
        strcat(sn_fil, ffb.ff_name); /*Got the fullpath of source*/

        strcpy(dn_fil, dt_fil);

        attrib = _chmod(dt_fil,0);
        if (attrib != -1) {
            if (attrib & FA_DIREC) {
                strcat(dn_fil, "\\*.*");
            }
        }

        /*
        strcpy(dn_fil, d_drv);
        strcat(dn_fil, d_dir);
        strcat(dn_fil, d_fil);
        strcat(dn_fil, d_ext);
        */

        /*Patch - must reset the source file name and source extension*/
        memset(s_fil,'\0',8);
        memset(s_ext,'\0',3);
/*
      for(j = 0; j <= 8; j++){
            s_fil[j] = ' ';
        }
        for(j = 0; j <= 3; j++){
            s_ext[j] = ' ';
        }
*/
        fnsplit(sn_fil, s_drv, s_dir, s_fil, s_ext);
        fnsplit(dn_fil, d_drv, d_dir, d_fil, d_ext);


        /*Replace wildcards '?' and/or '*'*/
        for(j = 0; j < strlen(d_fil); j++){
            if(d_fil[j] == '?'){
                d_fil[j] = s_fil[j];
            }
            else if((d_fil[j] == '*') && (strlen(s_fil) >= strlen(d_fil))){
                for(k = j; k <= strlen(s_fil); k++){
                    d_fil[k] = s_fil[k];
                }
            }
            else if((d_fil[j] == '*') && (strlen(s_fil) < strlen(d_fil))){
                for(k = j; k <= strlen(d_fil); k++){
                    d_fil[k] = '\0';
                }
            }
        }
        for(j = 0; j < strlen(d_ext); j++){
            if(d_ext[j] == '?'){
                d_ext[j] = s_ext[j];
            }
            else if((d_ext[j] == '*') && (strlen(s_ext) >= strlen(d_ext))){
                for(k = j; k <= strlen(s_ext); k++){
                    d_ext[k] = s_ext[k];
                }
            }
            else if((d_ext[j] == '*') && (strlen(s_ext) < strlen(d_ext))){
                for(k = j; k <= strlen(d_ext); k++){
                    d_ext[k] = '\0';
                }
            }
        }

        strcpy(dn_fil, d_drv);
        strcat(dn_fil, d_dir);
        strcat(dn_fil, d_fil);
        strcat(dn_fil, d_ext);

        /*Start copying the files one by one.*/
        if(flag_v==FLAG_SET) {
            fprintf(stderr, "%s -> %s\n", strlwr(sn_fil), strlwr(dn_fil));
        }
        /*No of files.*/
        nfil++;

        if(stricmp(sn_fil, dn_fil) == 0){
            fputs("File cannot be copied on to itself\n",stderr);
            nfil--;
            break;
        }

        if((access(dn_fil, 0) == 0) && (flag_f == FLAG_UNSET)) {
            back:
            fprintf(stderr, "\rReplace %s (Yes/No/Cancel/All)? ", dn_fil);
            while(1){
                key = read_key();
                switch(key){
		case 'A':
		    flag_f = FLAG_SET;
		case 'Y':
		    printf("\n");
                    goto cont;
		case 'N':
		    printf("\n");
		    nfil--;
                    goto bottom;
		case 'C':
		    return 3;
		default:
                    goto back;
                }
            }
        }
        cont:
        trg_sz = ffb.ff_fsize; /*Size of source file.*/

        if(strcmp(d_drv, "") == 0)
            getdfree(getdisk()+1, &dfree);
        else
            getdfree(toupper(d_drv[0]) - 'A'+1, &dfree);

        fskspc = (long) dfree.df_avail * (long) dfree.df_bsec * (long) dfree.df_sclus;

        if(trg_sz > fskspc){
            fprintf(stderr, "Insufficient disk space\n");
            nfil--;
            break; /*One time error no more message exit*/
        }

        src = fopen(sn_fil, "rb");
        dst = fopen(dn_fil, "wb");

        if(dst == NULL){
            fprintf(stderr, "File creation error\n");
            nfil--;
            break;
        }

        /*Copy the source file to destination file.*/
        while ((c = fgetc(src)) != EOF){
            if (fputc(c, dst) == EOF){
                fclose(src);
                fclose(dst);
            }
        }

        /*
        if ((src = open(sn_fil, O_RDONLY | O_BINARY)) == -1) {
            fprintf(stderr, "Cannot open %s error is\n", sn_fil);
            perror("");
            return -1;
        }
        if ((dst = open(dn_fil, O_WRONLY | O_TRUNC | O_CREAT | O_BINARY)) == -1) {
            fprintf(stderr, "Cannot open %s error is: %s\n", dn_fil);
            perror("");
            return -1;
        }

        while(!eof(src)){
            r = read(src, buff, BUFF_SIZE);
            write(dst, buff, r);
        }

        close(src);
        close(dst);
        */

        fclose(src);
        fclose(dst);
        bottom:
        done = findnext(&ffb);
    }
    fprintf(stderr, "%9d file(s) copied\n", nfil);
    free(buff);

    return 0;
}

void print_help(void)
{
    fputs("Usage: CP [OPTION]... SOURCE DEST\n  or:  CP [OPTION]... [PATH\\]PATTERN1 [PATH\\]PATTERN2\n\n", stderr);
    fputs("CoPy SOURCE to DEST or All files with PATTERN1 [with an optional path]\n"
                "  to new names with PATTERN2 [with an optionional path]\n\n",stderr);
    fputs("The OPTIONS are:\n",stderr);
    fputs("  -v: verbose: print filename of each copied file\n",stderr);
    fputs("  -f: force:   force all files to be overwritten\n",stderr);
    fputs("  -h: help:    display this help and exit\n",stderr);
    fputs("\nFILE is a name of a file to write\n", stderr);
    fputs("\nCP is part of DOG (https://dog.zumppe.net/)\n", stderr);
}

BYTE read_key(void)
{
    BYTE c;
    asm mov ah, 01h; /* read key with echo */
    asm int 21h;
    asm mov c,al;

    if(c >= 'a') {
	c -= ('a' - 'A'); /* uppercase the character */
    }
    return c;
}
