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

****************************************************************************/
#include <io.h>
#include <fcntl.h>
#include "ext.h"

/* BUFF_SIZE = 10240 Bytes / 16 bytes/paragraph = 640 16-byte-paragraps */
#define BUFF_SIZE 10240
#define BUFF_PARA 280h

int main(BYTE n, BYTE *arg[])
{
	signed char i;
	int r, nfil = 0, done, j, k, c;
	long fskspc;
	BYTE sn_fil[MAXPATH+13], dn_fil[MAXPATH+13], dt_fil[MAXPATH+13];
	BYTE s_drv[MAXDRIVE], s_dir[MAXDIR], s_fil[MAXFILE], s_ext[MAXEXT];
	BYTE d_drv[MAXDRIVE], d_dir[MAXDIR], d_fil[MAXFILE], d_ext[MAXEXT];
	WORD *buff;
	DWORD src_sz, trg_sz;
	FILE *src, *dst;
	struct ffblk ffb;
	struct dfree dfree;

	/***********************************************************************
	1]File Copy:
	------------
	  CP *.* X:\Somepath\*.*
	      ^		     ^
	      |		     |_____________________
	      |						           |
	      Source without drive		    | Destination with complete
	      or directory means the		| path information.
	      current working directory.	|
	
	2]File Copy:
	------------
	  CP X:\Somepath\*.* *.*
	      ^		          ^
	      |		          |______________
	      |			 			    	 |
	      Source with complete        | Destination with incomplete
	      path information.           | path information means current
	                                  | working  directory.
	
	3]File Copy:
	------------
	  CP X:\Somepath\*.* X:\Somepath\*.*
	      ^		          ^
	      |		          |______________
	      |			 			    	 |
	      Source with complete        | Destination with complete
	      path information.           | path information.        
	                                  |
	4]File copy:
	------------
	  CP X:\Somepath
	      ^
		  |
	      Can't handle COMMAND.COM does
	
	5]File copy:
	------------
	  CP X:\Somepath X:\Somepath
	      ^
		  |
	      Can't handle COMMAND.COM does
	***********************************************************************/

	if(n < 3){
		fprintf(stderr, "Required parameter missing\n");
		return -1;
	}

	i = findfirst(arg[1], &ffb, FA_NORMAL|FA_ARCH|FA_SYSTEM|FA_RDONLY|FA_HIDDEN);
	if (i == -1) {
		fprintf(stderr,"File %s NOT found\n",arg[1]);
		return -1;
	}
	
	if ((buff = malloc(BUFF_SIZE)) == NULL) {
		fprintf(stderr,"Not enough memory!\n");
		return -1;
	}

	/*some types of error were handled above.*/
	fnsplit(arg[1], s_drv, s_dir, s_fil, s_ext);
	fnsplit(arg[2], d_drv, d_dir, d_fil, d_ext);

	/*Save destination.*/
	strcpy(dt_fil, arg[2]);

	done = findfirst(arg[1], &ffb, FA_RDONLY | FA_HIDDEN | FA_SYSTEM | FA_ARCH | FA_NORMAL);
	while(!done){
		strcpy(sn_fil, s_drv);
		strcat(sn_fil, s_dir);
		strcat(sn_fil, ffb.ff_name); /*Got the fullpath of source*/

		strcpy(dn_fil, dt_fil);
		/*
		strcpy(dn_fil, d_drv);
		strcat(dn_fil, d_dir);
		strcat(dn_fil, d_fil);
		strcat(dn_fil, d_ext);
		*/

		/*Patch - must reset the source file name and source extension*/
	    memset(s_fil,'\0',8);
	    memset(s_ext,'\0',8);
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
		fprintf(stderr, "%s -> %s\n", strlwr(sn_fil), strlwr(dn_fil));
		
		/*No of files.*/
		nfil++;

		if(stricmp(sn_fil, dn_fil) == 0){
			fprintf(stderr, "File cannot be copied on to itself\n");
			nfil--;
			break;
		}
		
		if(access(dn_fil, 0) == 0) {
			back:
			fprintf(stderr, "\rReplace %s (Yes/No)? ", dn_fil);
			while(1){
				i = getchar();
				switch(toupper(i)){
					case 'Y':
						printf("\n");
					goto cont;
					case 'N':
						printf("\n");
					    nfil--;
					goto bottom;
					case 3:  /*Ctrl-Break.*/
					    printf("\n");
						return -1;
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
